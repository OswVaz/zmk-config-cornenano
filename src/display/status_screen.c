/*
 * Copyright (c) 2026
 *
 * SPDX-License-Identifier: MIT
 */

#include <lvgl.h>

#include <zmk/display/status_screen.h>

#include "animation.h"
#include "battery_alert.h"

/*
 * Status screen comun a todas las animaciones. La animacion se elige en
 * config/corne.conf con una de las opciones CONFIG_CORNE_ANIMATION_* (ver
 * src/display/animations/README.md). La alerta de bateria va siempre encima.
 */
lv_obj_t *zmk_display_status_screen(void) {
    lv_obj_t *screen = lv_obj_create(NULL);

    lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_pad_all(screen, 0, LV_PART_MAIN);
    lv_obj_set_style_border_width(screen, 0, LV_PART_MAIN);

    animation_init(screen);

    battery_alert_init(screen);

    return screen;
}
