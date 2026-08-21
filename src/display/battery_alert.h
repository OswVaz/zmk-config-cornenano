/*
 * Copyright (c) 2026
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <lvgl.h>

/* Crea la alerta de pila baja como hijo de `parent`. Arranca oculta: solo
 * aparece (parpadeando) cuando la carga cae por debajo del umbral. */
void battery_alert_init(lv_obj_t *parent);
