/*
 * Copyright (c) 2026
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <lvgl.h>

/*
 * Contrato de la biblioteca de animaciones (src/display/animations/).
 *
 * Cada animacion vive en su propia carpeta e implementa esta unica funcion.
 * Kconfig deja compilar solo una a la vez (choice CORNE_ANIMATION), asi que no
 * hay choque de nombres. `screen` es la pantalla completa de 128x32 en
 * coordenadas de LVGL; la animacion crea ahi sus objetos y sus timers.
 */
void animation_init(lv_obj_t *screen);
