/*
 * Copyright (c) 2026
 *
 * SPDX-License-Identifier: MIT
 */

#include <lvgl.h>
#include <zephyr/random/random.h>
#include <zephyr/sys/atomic.h>

#include <zmk/display/status_screen.h>
#include <zmk/event_manager.h>
#include <zmk/events/position_state_changed.h>

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
static int8_t matrix_head[MATRIX_LANES];                 /* profundidad de la cabeza */
static uint8_t matrix_trail[MATRIX_LANES];               /* largo de la estela */
static uint8_t matrix_speed[MATRIX_LANES];               /* ticks entre paso y paso */
static uint8_t matrix_count[MATRIX_LANES];

/* Bitmask de carriles donde hay que soltar una gota nueva. Lo escribe el hilo
 * de ZMK desde el listener de teclas y lo consume el hilo de UI en el timer de
 * LVGL, por eso es atomico. */
static atomic_t matrix_pending;

static uint8_t matrix_random_glyph(void) {
    return (uint8_t)(sys_rand32_get() % KATAKANA_COUNT);
}

static void matrix_reset_lane(int lane) {
    matrix_speed[lane] = 1 + (sys_rand32_get() % 3);
    matrix_trail[lane] = 4 + (sys_rand32_get() % 8); /* estelas de 4 a 11 celdas */
    /* Arranca antes del borde para que los carriles no caigan sincronizados. */
    matrix_head[lane] = -(int8_t)(sys_rand32_get() % (MATRIX_DEPTH + 8));
    matrix_count[lane] = 0;
}

/* Gota disparada por una tecla: empieza arriba, larga y a maxima velocidad. */
static void matrix_drop_lane(int lane) {
    matrix_head[lane] = 0;
    matrix_trail[lane] = 10;
    matrix_speed[lane] = 1;
    matrix_count[lane] = 0;

    for (int depth = 0; depth < MATRIX_DEPTH; depth++) {
        matrix_glyph[lane][depth] = matrix_random_glyph();
    }
}

static void matrix_step_lane(int lane) {
    matrix_head[lane]++;

    if (matrix_head[lane] - matrix_trail[lane] >= MATRIX_DEPTH) {
        matrix_reset_lane(lane);
        return;
    }

    if (matrix_head[lane] >= 0 && matrix_head[lane] < MATRIX_DEPTH) {
        matrix_glyph[lane][matrix_head[lane]] = matrix_random_glyph();
    }

    /* Parpadeo ocasional de un caracter ya dibujado. */
    if ((sys_rand32_get() % 3) == 0) {
        matrix_glyph[lane][sys_rand32_get() % MATRIX_DEPTH] = matrix_random_glyph();
    }
}

static bool matrix_cell_visible(int lane, int depth) {
    int behind = matrix_head[lane] - depth;

    return behind >= 0 && behind < matrix_trail[lane];
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
        if (pending & BIT(lane)) {
            matrix_drop_lane(lane);
            continue;
        }

        if (++matrix_count[lane] < matrix_speed[lane]) {
            continue;
        }

        matrix_count[lane] = 0;
        matrix_step_lane(lane);
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

static void matrix_rain_init(lv_obj_t *screen) {
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
        matrix_reset_lane(lane);

        for (int depth = 0; depth < MATRIX_DEPTH; depth++) {
            matrix_glyph[lane][depth] = matrix_random_glyph();
        }
    }

    matrix_render();

    lv_timer_create(matrix_timer_cb, MATRIX_TICK_MS, NULL);
}

lv_obj_t *zmk_display_status_screen(void) {
    lv_obj_t *screen = lv_obj_create(NULL);

    lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_pad_all(screen, 0, LV_PART_MAIN);
    lv_obj_set_style_border_width(screen, 0, LV_PART_MAIN);

    matrix_rain_init(screen);

    return screen;
}
