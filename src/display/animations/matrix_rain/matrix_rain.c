/*
 * Copyright (c) 2026
 *
 * Animacion: lluvia de katakana estilo Matrix (CONFIG_CORNE_ANIMATION_MATRIX_RAIN).
 *
 * SPDX-License-Identifier: MIT
 */

#include <lvgl.h>
#include <zephyr/random/random.h>
#include <zephyr/sys/atomic.h>

#include <zmk/event_manager.h>
#include <zmk/events/position_state_changed.h>

#include "animation.h"

LV_FONT_DECLARE(matrix_katakana_6);

/*
 * La OLED del Corne va montada en vertical, asi que el eje largo de la pantalla
 * (128 px) es el eje por el que cae la lluvia, y el corto (32 px) es el que
 * reparte los carriles.
 *
 * En coordenadas de LVGL la pantalla sigue siendo 128x32, asi que cada carril
 * es una linea de texto horizontal y la gota avanza a lo largo de la linea:
 *
 *   LVGL:   x = 0 .......................... 127     (profundidad de caida)
 *           y = 0..29 en 5 lineas de 6 px           (carriles)
 *
 *   Fisico: los carriles quedan lado a lado y la gota cae a lo largo del teclado.
 *
 * El font es monoespaciado de 6x6 px: 21 celdas de 6 px = 126 px de profundidad,
 * y 5 carriles de 6 px = 30 px de ancho.
 */
#define MATRIX_LANES 5
#define MATRIX_DEPTH 21
#define MATRIX_CELL 6

/* Gotas simultaneas por carril. Con una sola, cada carril pasa casi todo el
 * tiempo vacio. Medido sobre 200k frames: 1 gota ~23% de celdas encendidas,
 * 3 gotas ~50%, 4 gotas ~60%. Mas de 4 se ve como un bloque solido. */
#define MATRIX_DROPS 4

/*
 * Sentido de la caida. Con la pantalla en vertical, "abajo" fisico corresponde
 * a x descendente en LVGL, asi que la gota se dibuja de derecha a izquierda del
 * buffer. Si al probarlo la lluvia sube en vez de bajar, pon esto en 0 y listo:
 * es lo unico que hay que cambiar.
 */
#define MATRIX_FALL_TOWARD_X0 1

/* Un frame completo tarda ~46 ms en salir por I2C a 100 kHz, asi que no tiene
 * sentido bajar mucho de aqui. */
#define MATRIX_TICK_MS 120

#define KATAKANA_FIRST 0x30A1
#define KATAKANA_COUNT 86

/* Cada katakana ocupa 3 bytes en UTF-8, cada hueco 1. Peor caso: todo lleno,
 * mas un '\n' por carril y el terminador. */
#define MATRIX_BUF_SIZE (MATRIX_LANES * (MATRIX_DEPTH * 3 + 1) + 1)

static lv_obj_t *matrix_label;
static char matrix_buf[MATRIX_BUF_SIZE];

static uint8_t matrix_glyph[MATRIX_LANES][MATRIX_DEPTH]; /* indice en el rango katakana */
static int8_t matrix_head[MATRIX_LANES][MATRIX_DROPS];   /* profundidad de la cabeza */
static uint8_t matrix_trail[MATRIX_LANES][MATRIX_DROPS]; /* largo de la estela */
static uint8_t matrix_speed[MATRIX_LANES][MATRIX_DROPS]; /* ticks entre paso y paso */
static uint8_t matrix_count[MATRIX_LANES][MATRIX_DROPS];

/* Bitmask de carriles donde hay que soltar una gota nueva. Lo escribe el hilo
 * de ZMK desde el listener de teclas y lo consume el hilo de UI en el timer de
 * LVGL, por eso es atomico. */
static atomic_t matrix_pending;

static uint8_t matrix_random_glyph(void) {
    return (uint8_t)(sys_rand32_get() % KATAKANA_COUNT);
}

static void matrix_reset_drop(int lane, int drop) {
    matrix_speed[lane][drop] = 1 + (sys_rand32_get() % 3);
    matrix_trail[lane][drop] = 3 + (sys_rand32_get() % 6); /* estelas de 3 a 8 celdas */
    /* Hueco corto antes de volver a entrar, para que el carril no se vacie. */
    matrix_head[lane][drop] = -(int8_t)(sys_rand32_get() % 8);
    matrix_count[lane][drop] = 0;
}

/* Gota disparada por una tecla: entra arriba, larga y a maxima velocidad.
 * Reutiliza la gota mas avanzada del carril para no pisar las otras. */
static void matrix_drop_lane(int lane) {
    int oldest = 0;

    for (int drop = 1; drop < MATRIX_DROPS; drop++) {
        if (matrix_head[lane][drop] > matrix_head[lane][oldest]) {
            oldest = drop;
        }
    }

    matrix_head[lane][oldest] = 0;
    matrix_trail[lane][oldest] = 8;
    matrix_speed[lane][oldest] = 1;
    matrix_count[lane][oldest] = 0;
}

static void matrix_step_drop(int lane, int drop) {
    matrix_head[lane][drop]++;

    if (matrix_head[lane][drop] - matrix_trail[lane][drop] >= MATRIX_DEPTH) {
        matrix_reset_drop(lane, drop);
        return;
    }

    if (matrix_head[lane][drop] >= 0 && matrix_head[lane][drop] < MATRIX_DEPTH) {
        matrix_glyph[lane][matrix_head[lane][drop]] = matrix_random_glyph();
    }
}

static bool matrix_cell_visible(int lane, int depth) {
    for (int drop = 0; drop < MATRIX_DROPS; drop++) {
        int behind = matrix_head[lane][drop] - depth;

        if (behind >= 0 && behind < matrix_trail[lane][drop]) {
            return true;
        }
    }

    return false;
}

/* Arma los 5 carriles en un solo buffer y se lo pasa a LVGL sin copiarlo. */
static void matrix_render(void) {
    char *out = matrix_buf;

    for (int lane = 0; lane < MATRIX_LANES; lane++) {
        if (lane > 0) {
            *out++ = '\n';
        }

        for (int cell = 0; cell < MATRIX_DEPTH; cell++) {
#if MATRIX_FALL_TOWARD_X0
            int depth = MATRIX_DEPTH - 1 - cell;
#else
            int depth = cell;
#endif

            if (!matrix_cell_visible(lane, depth)) {
                *out++ = ' ';
                continue;
            }

            uint16_t codepoint = KATAKANA_FIRST + matrix_glyph[lane][depth];

            *out++ = (char)(0xE0 | ((codepoint >> 12) & 0x0F));
            *out++ = (char)(0x80 | ((codepoint >> 6) & 0x3F));
            *out++ = (char)(0x80 | (codepoint & 0x3F));
        }
    }

    *out = '\0';

    lv_label_set_text_static(matrix_label, matrix_buf);
}

static void matrix_timer_cb(lv_timer_t *timer) {
    ARG_UNUSED(timer);

    /* Lee y limpia de un golpe las teclas pulsadas desde el ultimo frame. */
    atomic_val_t pending = atomic_set(&matrix_pending, 0);

    for (int lane = 0; lane < MATRIX_LANES; lane++) {
        for (int drop = 0; drop < MATRIX_DROPS; drop++) {
            if (++matrix_count[lane][drop] < matrix_speed[lane][drop]) {
                continue;
            }

            matrix_count[lane][drop] = 0;
            matrix_step_drop(lane, drop);
        }

        if (pending & BIT(lane)) {
            matrix_drop_lane(lane);
        }

        /* Parpadeo: un caracter al azar del carril cambia de glifo. */
        if ((sys_rand32_get() % 3) == 0) {
            matrix_glyph[lane][sys_rand32_get() % MATRIX_DEPTH] = matrix_random_glyph();
        }
    }

    matrix_render();
}

/* Traduce la posicion fisica de la tecla a un carril, para que la gota caiga
 * mas o menos donde esta tu dedo. El Corne tiene 12 columnas de teclas (0-35)
 * mas 6 pulgares (36-41), y las mitades ya reportan posiciones globales porque
 * corne_right.overlay aplica col-offset = <6>. */
static int matrix_lane_for_position(uint32_t position) {
    uint32_t key_col;

    if (position < 36) {
        key_col = position % 12;
    } else if (position < 42) {
        key_col = 3 + (position - 36); /* los pulgares caen bajo las columnas 3-8 */
    } else {
        key_col = 6;
    }

    return (int)((key_col * MATRIX_LANES) / 12);
}

static int matrix_position_listener(const zmk_event_t *eh) {
    const struct zmk_position_state_changed *ev = as_zmk_position_state_changed(eh);

    if (ev != NULL && ev->state) {
        atomic_or(&matrix_pending, BIT(matrix_lane_for_position(ev->position)));
    }

    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(matrix_rain, matrix_position_listener);
ZMK_SUBSCRIPTION(matrix_rain, zmk_position_state_changed);

void animation_init(lv_obj_t *screen) {
    matrix_label = lv_label_create(screen);

    lv_label_set_long_mode(matrix_label, LV_LABEL_LONG_CLIP);
    lv_obj_set_style_text_font(matrix_label, &matrix_katakana_6, LV_PART_MAIN);
    lv_obj_set_style_text_letter_space(matrix_label, 0, LV_PART_MAIN);
    lv_obj_set_style_text_line_space(matrix_label, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(matrix_label, 0, LV_PART_MAIN);
    lv_obj_set_size(matrix_label, MATRIX_DEPTH * MATRIX_CELL, MATRIX_LANES * MATRIX_CELL);
    /* 126x30 en una pantalla de 128x32: 1 px de margen para centrarlo. */
    lv_obj_set_pos(matrix_label, 1, 1);

    for (int lane = 0; lane < MATRIX_LANES; lane++) {
        for (int drop = 0; drop < MATRIX_DROPS; drop++) {
            matrix_reset_drop(lane, drop);
            /* Al arrancar si las repartimos por todo el carril, para que no
             * entren todas juntas por el mismo borde. */
            matrix_head[lane][drop] = -(int8_t)(sys_rand32_get() % (MATRIX_DEPTH + 8));
        }

        for (int depth = 0; depth < MATRIX_DEPTH; depth++) {
            matrix_glyph[lane][depth] = matrix_random_glyph();
        }
    }

    matrix_render();

    lv_timer_create(matrix_timer_cb, MATRIX_TICK_MS, NULL);
}
