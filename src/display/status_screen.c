/*
 * Copyright (c) 2026
 *
 * SPDX-License-Identifier: MIT
 */

#include <lvgl.h>
#include <zmk/display/status_screen.h>
#include <zmk/display/widgets/battery_status.h>

#ifdef CONFIG_ZMK_SPLIT_ROLE_CENTRAL
#define CUSTOM_STATUS_TEXT "Oswaldo"
#else
#define CUSTOM_STATUS_TEXT "Vázquez"
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_BATTERY_STATUS)
static struct zmk_widget_battery_status battery_status_widget;
#endif

lv_obj_t *zmk_display_status_screen(void) {
    lv_obj_t *screen = lv_obj_create(NULL);
    lv_obj_t *label = lv_label_create(screen);

#if IS_ENABLED(CONFIG_ZMK_WIDGET_BATTERY_STATUS)
    zmk_widget_battery_status_init(&battery_status_widget, screen);
    lv_obj_align(zmk_widget_battery_status_obj(&battery_status_widget), LV_ALIGN_TOP_RIGHT, 0, 0);
#endif

    lv_label_set_text(label, CUSTOM_STATUS_TEXT);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

    return screen;
}
