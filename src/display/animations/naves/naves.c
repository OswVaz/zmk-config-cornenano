/*
 * Copyright (c) 2026
 *
 * Animacion: nave contra aliens en una sola escena repartida entre las dos
 * pantallas (CONFIG_CORNE_ANIMATION_NAVES).
 *
 * SPDX-License-Identifier: MIT
 */

/*
 * Como se juega
 * -------------
 * Nadie controla la nave: se pilotea sola, apunta y esquiva.
 *
 * Con CONFIG_CORNE_ANIMATION_NAVES_AUTOPLAY (el default) el juego avanza un paso
 * por frame sin que hagas nada, y cada tecla le da un empujon de pasos extra.
 * Las dos mitades suman ese paso por frame cada una con su reloj; la central
 * solo corrige a la otra con el latido o cuando hay teclas.
 *
 * Sin autoplay, las teclas son el "combustible" del tiempo: cada pulsacion, de
 * cualquier mitad, deja avanzar el juego unos pasos. Si dejas de teclear, todo
 * se congela y los aliens solo se mecen de un lado a otro.
 *
 * Como se ve como una sola pantalla
 * ---------------------------------
 * La izquierda (central) recibe todas las teclas y decide hasta que paso se
 * avanza. Le manda a la derecha solo tres numeros (ver naves_sync.c) y la
 * derecha corre la misma simulacion determinista (naves_core.c), asi que las
 * dos llegan al mismo estado y cada una dibuja su mitad de la escena.
 */

#include <lvgl.h>
#include <zephyr/kernel.h>
#include <zephyr/random/random.h>
#include <zephyr/spinlock.h>
#include <zephyr/sys/atomic.h>

#include <zmk/event_manager.h>
#include <zmk/events/position_state_changed.h>

#include "animation.h"
#include "naves_core.h"
#include "naves_sync.h"

/* Un frame completo tarda ~46 ms en salir por I2C, no tiene caso bajar de aqui. */
#define NAVES_TICK_MS 60

/* Pasos de juego que gana cada tecla, y tope de pasos acumulados para que al
 * dejar de teclear no siga moviendose varios segundos. */
#define NAVES_STEPS_PER_KEY 4
#define NAVES_MAX_BACKLOG 40

/* Si van muchos pasos acumulados se avanzan varios por frame para alcanzar. */
#define NAVES_CATCHUP_DIV 8
#define NAVES_MAX_STEPS_PER_TICK 6

/* Periferica: si se quedo muy atras (se durmio la pantalla, se reconecto) salta
 * sin dibujar hasta casi alcanzar, en tandas para no trabar el hilo. */
#define NAVES_SKIP_AHEAD 48
#define NAVES_SKIP_BATCH 4000

/* En reposo: cada cuantos ticks cambia el vaiven (8 x 60 ms ~ medio segundo). */
#define NAVES_SWAY_TICKS 8

/* Central: reenvia el estado cada ~2 s aunque no cambie, por si la periferica
 * se conecto tarde o se perdio un mensaje. */
#define NAVES_HEARTBEAT_TICKS 33

#define NAVES_AUTOPLAY IS_ENABLED(CONFIG_CORNE_ANIMATION_NAVES_AUTOPLAY)

#if NAVES_LEADER
#define NAVES_HALF 0
#else
#define NAVES_HALF 1
#endif

#define CANVAS_PALETTE_BYTES (2 * sizeof(lv_color32_t))

static uint8_t canvas_buf[LV_CANVAS_BUF_SIZE_INDEXED_1BIT(NAVES_FB_W, NAVES_FB_H)];
BUILD_ASSERT(sizeof(canvas_buf) >= CANVAS_PALETTE_BYTES + NAVES_FB_SIZE);

static lv_obj_t *canvas;
static struct naves_sim sim;
static uint32_t target;

static uint8_t idle_ticks;
static uint8_t sway_phase;
static const int8_t sway_offset[4] = {0, 1, 0, -1};

#if NAVES_LEADER

static atomic_t key_presses;
static uint32_t sent_seed, sent_start, sent_target;
static uint8_t heartbeat = NAVES_HEARTBEAT_TICKS; /* manda el primero de inmediato */

static int naves_position_listener(const zmk_event_t *eh) {
    const struct zmk_position_state_changed *ev = as_zmk_position_state_changed(eh);

    if (ev != NULL && ev->state) {
        atomic_inc(&key_presses);
    }

    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(naves, naves_position_listener);
ZMK_SUBSCRIPTION(naves, zmk_position_state_changed);

static void naves_take_input(void) {
    atomic_val_t presses = atomic_set(&key_presses, 0);

    if (presses <= 0) {
        return;
    }

    int32_t pending = (int32_t)(target - sim.step);
    int32_t room = NAVES_MAX_BACKLOG - (pending > 0 ? pending : 0);
    int32_t gain = presses * NAVES_STEPS_PER_KEY;

    if (room > 0) {
        target += (uint32_t)(gain < room ? gain : room);
    }
}

static void naves_publish(void) {
#if NAVES_AUTOPLAY
    /* La periferica tambien suma un paso por tick por su cuenta, asi que eso no
     * cuenta como cambio: solo se manda si hubo teclas, oleada nueva o latido. */
    sent_target++;
#endif

    if (++heartbeat < NAVES_HEARTBEAT_TICKS && target == sent_target &&
        sim.wave_seed == sent_seed && sim.wave_start == sent_start) {
        return;
    }

    heartbeat = 0;
    sent_seed = sim.wave_seed;
    sent_start = sim.wave_start;
    sent_target = target;

    naves_sync_send(sent_seed, sent_start, sent_target);
}

#else /* periferica */

struct naves_rx {
    uint32_t seed;
    uint32_t start;
    uint32_t target;
    uint32_t seq;
    bool valid;
};

static struct k_spinlock rx_lock;
static struct naves_rx rx;
static uint32_t rx_seen;

void naves_sync_receive(uint32_t wave_seed, uint32_t wave_start, uint32_t new_target) {
    k_spinlock_key_t key = k_spin_lock(&rx_lock);

    rx.seed = wave_seed;
    rx.start = wave_start;
    rx.target = new_target;
    rx.seq++;
    rx.valid = true;

    k_spin_unlock(&rx_lock, key);
}

/* Devuelve true si hubo que reconstruir o saltar (hay que redibujar). */
static bool naves_take_input(void) {
    k_spinlock_key_t key = k_spin_lock(&rx_lock);
    struct naves_rx msg = rx;
    k_spin_unlock(&rx_lock, key);

    bool fresh = msg.seq != rx_seen;

    rx_seen = msg.seq;

    /* En autoplay el objetivo local avanza solo, asi que un mensaje viejo no se
     * vuelve a aplicar: regresaria el objetivo al de hace rato. */
    if (!msg.valid || (NAVES_AUTOPLAY && !fresh)) {
        return false;
    }

    bool changed = naves_follow(&sim, msg.seed, msg.start, msg.target);

    target = msg.target;

    if ((int32_t)(target - sim.step) > NAVES_SKIP_AHEAD) {
        for (int i = 0; i < NAVES_SKIP_BATCH && (int32_t)(target - 8 - sim.step) > 0; i++) {
            naves_step(&sim);
        }
        changed = true;
    }

    return changed;
}

#endif

static void naves_draw(void) {
    int phase = sway_phase;

    naves_render(&sim, canvas_buf + CANVAS_PALETTE_BYTES, NAVES_HALF, sway_offset[phase],
                 (phase & 1) != 0);
    lv_obj_invalidate(canvas);
}

static void naves_tick(lv_timer_t *timer) {
    ARG_UNUSED(timer);

    bool dirty = false;

#if NAVES_LEADER
    naves_take_input();
#else
    dirty = naves_take_input();
#endif

#if NAVES_AUTOPLAY
    /* Un paso por tick sin teclas. Si esta mitad iba adelantada y la central la
     * regreso, el objetivo queda atras de sim.step y aqui solo espera. */
    target++;
#endif

    int32_t pending = (int32_t)(target - sim.step);

    if (pending > 0) {
        int n = 1 + pending / NAVES_CATCHUP_DIV;

        n = n > NAVES_MAX_STEPS_PER_TICK ? NAVES_MAX_STEPS_PER_TICK : n;

        while (n-- > 0) {
            naves_step(&sim);
        }

        idle_ticks = 0;
        sway_phase = 0;
        dirty = true;
    } else if (++idle_ticks >= NAVES_SWAY_TICKS) {
        idle_ticks = 0;
        sway_phase = (sway_phase + 1) & 3;
        dirty = true;
    }

#if NAVES_LEADER
    naves_publish();
#endif

    /* Solo se manda frame por I2C si algo cambio. */
    if (dirty) {
        naves_draw();
    }
}

void animation_init(lv_obj_t *screen) {
    /* La central inventa el seed; la periferica arranca con uno cualquiera y se
     * reconstruye en cuanto le llega el primer mensaje. */
    naves_start_wave(&sim, NAVES_LEADER ? sys_rand32_get() : 0, 0);
    target = sim.step;

    canvas = lv_canvas_create(screen);
    lv_canvas_set_buffer(canvas, canvas_buf, NAVES_FB_W, NAVES_FB_H, LV_IMG_CF_INDEXED_1BIT);

    /* Colores tomados del tema, igual que la alerta de bateria: asi da igual
     * como quede la polaridad de la OLED. */
    lv_canvas_set_palette(canvas, 0, lv_obj_get_style_bg_color(screen, LV_PART_MAIN));
    lv_canvas_set_palette(canvas, 1, lv_obj_get_style_text_color(screen, LV_PART_MAIN));
    lv_obj_set_pos(canvas, 0, 0);

    naves_draw();

    lv_timer_create(naves_tick, NAVES_TICK_MS, NULL);
}
