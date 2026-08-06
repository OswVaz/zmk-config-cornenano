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

#define MATRIX_COLS 16
#define MATRIX_ROWS 3
#define MATRIX_CELL_W 8
#define MATRIX_CELL_H 9
#define KATAKANA_FIRST 0x30A1
#define KATAKANA_COUNT 86
#define MATRIX_TICK_MS 120

#if IS_ENABLED(CONFIG_ZMK_WIDGET_BATTERY_STATUS)
static struct zmk_widget_battery_status battery_status_widget;
#endif

static lv_obj_t *matrix_labels[MATRIX_COLS][MATRIX_ROWS];
static char matrix_text[MATRIX_COLS][MATRIX_ROWS][4];
static uint16_t matrix_chars[MATRIX_COLS][MATRIX_ROWS];
static uint8_t matrix_speed[MATRIX_COLS];
static uint8_t matrix_counter[MATRIX_COLS];

static void matrix_codepoint_to_utf8(uint16_t codepoint, char *out) {
    out[0] = (char)(0xE0 | ((codepoint >> 12) & 0x0F));
    out[1] = (char)(0x80 | ((codepoint >> 6) & 0x3F));
    out[2] = (char)(0x80 | (codepoint & 0x3F));
    out[3] = '\0';
}

static uint16_t matrix_random_katakana(void) {
    return KATAKANA_FIRST + (sys_rand32_get() % KATAKANA_COUNT);
}

static void matrix_update_label(int col, int row) {
    matrix_codepoint_to_utf8(matrix_chars[col][row], matrix_text[col][row]);
    lv_label_set_text(matrix_labels[col][row], matrix_text[col][row]);
}

static void matrix_init_column(int col) {
    matrix_speed[col] = 1 + (sys_rand32_get() % 3);
    matrix_counter[col] = sys_rand32_get() % matrix_speed[col];

    for (int row = 0; row < MATRIX_ROWS; row++) {
        matrix_chars[col][row] = matrix_random_katakana();
        matrix_update_label(col, row);
    }
}

static void matrix_step_column(int col) {
    for (int row = MATRIX_ROWS - 1; row > 0; row--) {
        matrix_chars[col][row] = matrix_chars[col][row - 1];
    }

    matrix_chars[col][0] = matrix_random_katakana();

    if ((sys_rand32_get() % 4) == 0) {
        matrix_chars[col][sys_rand32_get() % MATRIX_ROWS] = matrix_random_katakana();
    }

    for (int row = 0; row < MATRIX_ROWS; row++) {
        matrix_update_label(col, row);
    }
}

static void matrix_timer_cb(lv_timer_t *timer) {
    ARG_UNUSED(timer);

    for (int col = 0; col < MATRIX_COLS; col++) {
        matrix_counter[col]++;

        if (matrix_counter[col] < matrix_speed[col]) {
            continue;
        }

        matrix_counter[col] = 0;
        matrix_step_column(col);
    }
}

static void matrix_style_label(lv_obj_t *label) {
    lv_obj_set_style_pad_all(label, 0, LV_PART_MAIN);
    lv_obj_set_style_border_width(label, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(label, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_text_font(label, &matrix_katakana_8, LV_PART_MAIN);
}

static void matrix_rain_init(lv_obj_t *screen) {
    lv_obj_set_style_pad_all(screen, 0, LV_PART_MAIN);
    lv_obj_set_style_border_width(screen, 0, LV_PART_MAIN);

    for (int col = 0; col < MATRIX_COLS; col++) {
        for (int row = 0; row < MATRIX_ROWS; row++) {
            lv_obj_t *label = lv_label_create(screen);

            matrix_labels[col][row] = label;
            matrix_style_label(label);
            lv_obj_set_pos(label, col * MATRIX_CELL_W, row * MATRIX_CELL_H);
        }

        matrix_init_column(col);
    }

    lv_timer_create(matrix_timer_cb, MATRIX_TICK_MS, NULL);
}

lv_obj_t *zmk_display_status_screen(void) {
    lv_obj_t *screen = lv_obj_create(NULL);

    matrix_rain_init(screen);

#if IS_ENABLED(CONFIG_ZMK_WIDGET_BATTERY_STATUS)
    zmk_widget_battery_status_init(&battery_status_widget, screen);
    lv_obj_align(zmk_widget_battery_status_obj(&battery_status_widget), LV_ALIGN_TOP_RIGHT, 0, 0);
#endif

    return screen;
}
