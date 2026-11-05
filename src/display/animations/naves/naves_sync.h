/*
 * Copyright (c) 2026
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <stdint.h>

#include <zephyr/sys/util_macro.h>

/*
 * Quien manda: la mitad central (la izquierda en el Corne) es la unica que ve
 * todas las teclas, asi que es la que decide cuanto avanza el juego. La
 * periferica solo repite la misma simulacion con lo que le llega.
 */
#if !IS_ENABLED(CONFIG_ZMK_SPLIT) || IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_CENTRAL)
#define NAVES_LEADER 1
#else
#define NAVES_LEADER 0
#endif

/* Central -> periferica: oleada actual (seed y paso en que arranco) y el paso
 * hasta el que hay que avanzar. Sin split no hace nada. */
void naves_sync_send(uint32_t wave_seed, uint32_t wave_start, uint32_t target);

/* Lo llama el driver en la periferica cuando llega un mensaje. Corre en el hilo
 * de Bluetooth, no en el de LVGL. Lo implementa naves.c. */
void naves_sync_receive(uint32_t wave_seed, uint32_t wave_start, uint32_t target);
