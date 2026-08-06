/*
 * Copyright (c) 2026
 *
 * SPDX-License-Identifier: MIT
 */

#include <lvgl.h>
#include <zephyr/random/random.h>
#include <zmk/display/status_screen.h>
#include <zmk/display/widgets/battery_status.h>

LV_FONT_DECLARE(matrix_katakana_8);

/* La OLED del Corne es de 128x32. El font es monoespaciado de 8x9 px, asi que
 * caben exactamente 16 columnas x 3 filas. */
#define MATRIX_COLS 16
#define MATRIX_ROWS 3
#define MATRIX_CELL_W 8
#define MATRIX_CELL_H 9
#define MATRIX_TICK_MS 120

#define KATAKANA_FIRST 0x30A1
#define KATAKANA_COUNT 86

#if IS_ENABLED(CONFIG_ZMK_WIDGET_BATTERY_STATUS)
/* Celdas que se dejan en blanco arriba a la derecha para que el widget de
 * bateria quede legible. Poner ambas en 0 para llenar toda la pantalla. */
#define MATRIX_RESERVED_COLS 5
#define MATRIX_RESERVED_ROWS 2
static struct zmk_widget_battery_status battery_status_widget;
#else
#define MATRIX_RESERVED_COLS 0
#define MATRIX_RESERVED_ROWS 0
#endif

/* Cada katakana ocupa 3 bytes en UTF-8, cada hueco 1. Peor caso: todo lleno,
 * mas un '\n' por fila y el terminador. */
#define MATRIX_BUF_SIZE (MATRIX_ROWS * (MATRIX_COLS * 3 + 1) + 1)

static lv_obj_t *matrix_label;
static char matrix_buf[MATRIX_BUF_SIZE];

static uint8_t matrix_glyph[MATRIX_COLS][MATRIX_ROWS]; /* indice dentro del rango katakana */
static int8_t matrix_head[MATRIX_COLS];                /* fila donde va la cabeza de la gota */
static uint8_t matrix_trail[MATRIX_COLS];              /* largo de la estela */
static uint8_t matrix_speed[MATRIX_COLS];              /* ticks entre paso y paso */
static uint8_t matrix_count[MATRIX_COLS];

static uint8_t matrix_random_glyph(void) {
    return (uint8_t)(sys_rand32_get() % KATAKANA_COUNT);
}

static void matrix_reset_column(int col) {
    matrix_speed[col] = 1 + (sys_rand32_get() % 3);
    matrix_trail[col] = 1 + (sys_rand32_get() % MATRIX_ROWS);
    /* Arranca por encima de la pantalla para que las columnas no caigan a la vez. */
    matrix_head[col] = -(int8_t)(sys_rand32_get() % (MATRIX_ROWS + 2));
    matrix_count[col] = 0;
}

static void matrix_step_column(int col) {
    matrix_head[col]++;

    if (matrix_head[col] - matrix_trail[col] >= MATRIX_ROWS) {
        matrix_reset_column(col);
        return;
    }

    if (matrix_head[col] >= 0 && matrix_head[col] < MATRIX_ROWS) {
        matrix_glyph[col][matrix_head[col]] = matrix_random_glyph();
    }

    /* Parpadeo ocasional de un caracter ya dibujado. */
    if ((sys_rand32_get() % 4) == 0) {
        matrix_glyph[col][sys_rand32_get() % MATRIX_ROWS] = matrix_random_glyph();
    }
}

static bool matrix_cell_visible(int col, int row) {
    if (row < MATRIX_RESERVED_ROWS && col >= MATRIX_COLS - MATRIX_RESERVED_COLS) {
        return false;
    }

    int depth = matrix_head[col] - row;

    return depth >= 0 && depth < matrix_trail[col];
}

/* Arma las 3 filas en un solo buffer y se lo pasa a LVGL sin copiarlo. */
static void matrix_render(void) {
    char *out = matrix_buf;

    for (int row = 0; row < MATRIX_ROWS; row++) {
        if (row > 0) {
            *out++ = '\n';
        }

        for (int col = 0; col < MATRIX_COLS; col++) {
            if (!matrix_cell_visible(col, row)) {
                *out++ = ' ';
                continue;
            }

            uint16_t codepoint = KATAKANA_FIRST + matrix_glyph[col][row];

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

    for (int col = 0; col < MATRIX_COLS; col++) {
        if (++matrix_count[col] < matrix_speed[col]) {
            continue;
        }

        matrix_count[col] = 0;
        matrix_step_column(col);
    }

    matrix_render();
}

static void matrix_rain_init(lv_obj_t *screen) {
    matrix_label = lv_label_create(screen);

    lv_label_set_long_mode(matrix_label, LV_LABEL_LONG_CLIP);
    lv_obj_set_style_text_font(matrix_label, &matrix_katakana_8, LV_PART_MAIN);
    lv_obj_set_style_text_letter_space(matrix_label, 0, LV_PART_MAIN);
    lv_obj_set_style_text_line_space(matrix_label, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(matrix_label, 0, LV_PART_MAIN);
    lv_obj_set_pos(matrix_label, 0, 0);
    lv_obj_set_size(matrix_label, MATRIX_COLS * MATRIX_CELL_W, MATRIX_ROWS * MATRIX_CELL_H);

    for (int col = 0; col < MATRIX_COLS; col++) {
        matrix_reset_column(col);

        for (int row = 0; row < MATRIX_ROWS; row++) {
            matrix_glyph[col][row] = matrix_random_glyph();
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

#if IS_ENABLED(CONFIG_ZMK_WIDGET_BATTERY_STATUS)
    zmk_widget_battery_status_init(&battery_status_widget, screen);
    lv_obj_align(zmk_widget_battery_status_obj(&battery_status_widget), LV_ALIGN_TOP_RIGHT, 0, 0);
#endif

    return screen;
}
