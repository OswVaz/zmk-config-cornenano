/*
 * Copyright (c) 2026
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

/*
 * Nucleo de la animacion "naves": simulacion y dibujo, en C puro (sin Zephyr ni
 * LVGL) para poder correrlo y verlo en la compu antes de flashear.
 *
 * Escena logica, en pixeles fisicos tal como se ven con el teclado enfrente:
 *
 *        mitad izquierda      mitad derecha
 *       x = 0 ....... 31  |  32 ....... 63
 *   y=0   aliens          |          aliens
 *    .                    |
 *    .                    |
 *  y=127      nave        |
 *
 * Cada OLED va en vertical (32 de ancho x 128 de alto), asi que las dos juntas
 * forman una sola escena de 64x128 y todo cruza de una pantalla a la otra.
 *
 * La simulacion es determinista: el mismo seed y el mismo numero de pasos dan
 * exactamente el mismo estado en las dos mitades. Por eso basta con mandar
 * entre mitades "seed de la oleada + paso", nunca el estado completo.
 */

#include <stdbool.h>
#include <stdint.h>

#define NAVES_W 64
#define NAVES_H 128
#define NAVES_PANEL_W 32

#define NAVES_COLS 5
#define NAVES_ROWS 4

#define NAVES_MAX_BULLETS 2
#define NAVES_MAX_BOMBS 3
#define NAVES_MAX_BOOMS 4
#define NAVES_STARS 14

/* Framebuffer en el formato de LVGL: 128x32, 1 bit por pixel, MSB primero. */
#define NAVES_FB_W 128
#define NAVES_FB_H 32
#define NAVES_FB_STRIDE (NAVES_FB_W / 8)
#define NAVES_FB_SIZE (NAVES_FB_STRIDE * NAVES_FB_H)

/*
 * Orientacion de la OLED, igual que en la lluvia Matrix: el borde de arriba
 * fisico es x = 127 de LVGL. Si la escena sale de cabeza, pon esto en 0.
 */
#ifndef NAVES_TOP_IS_LVGL_X_MAX
#define NAVES_TOP_IS_LVGL_X_MAX 1
#endif

/*
 * Si la escena se ve como en espejo (la nave cruza al lado que no es, o los
 * aliens se cortan raro entre pantallas), pon esto en 1.
 */
#ifndef NAVES_MIRROR_X
#define NAVES_MIRROR_X 0
#endif

struct naves_shot {
    int16_t y;
    int8_t x;
    bool on;
};

struct naves_boom {
    int8_t x;
    int8_t y;
    uint8_t t;
    bool big;
};

struct naves_sim {
    uint32_t rng;
    uint32_t step;       /* paso global, cuenta desde el arranque de la central */
    uint32_t wave_seed;  /* seed con el que arranco la oleada actual */
    uint32_t wave_start; /* paso global en el que arranco la oleada actual */

    uint32_t alive; /* bit (fila * NAVES_COLS + col) */
    int8_t fx;      /* esquina superior izquierda de la formacion */
    int8_t fy;
    int8_t fdir;
    uint8_t march_count;
    uint8_t frame;

    int8_t ship_x; /* centro de la nave */
    uint8_t ship_dead;
    int8_t target_col;
    uint8_t cooldown;
    uint8_t wave_end; /* pausa antes de la siguiente oleada; 0 = jugando */

    struct naves_shot bullets[NAVES_MAX_BULLETS];
    struct naves_shot bombs[NAVES_MAX_BOMBS];
    bool bomb_seen[NAVES_MAX_BOMBS]; /* la nave la vio venir y la va a esquivar */
    struct naves_boom booms[NAVES_MAX_BOOMS];

    uint8_t star_x[NAVES_STARS];
    uint8_t star_y[NAVES_STARS];
};

/* Arranca una oleada limpia. El estado resultante depende solo de los dos
 * argumentos, por eso la otra mitad puede reconstruirlo sin mas datos. */
void naves_start_wave(struct naves_sim *s, uint32_t seed, uint32_t global_step);

/* Avanza un paso de juego. */
void naves_step(struct naves_sim *s);

/*
 * Dibuja la parte de la escena que le toca a una mitad (0 izquierda, 1 derecha)
 * en un framebuffer de NAVES_FB_SIZE bytes. `sway` corre la formacion unos
 * pixeles de lado (el vaiven en reposo) y `flip` alterna el cuadro de los
 * aliens; ninguno de los dos toca la simulacion.
 */
void naves_render(const struct naves_sim *s, uint8_t *fb, int half, int sway, bool flip);

/*
 * Para la mitad que sigue a la otra. Recibe lo que mando la central (seed y
 * paso de arranque de su oleada) y decide si el estado local todavia puede
 * llegar ahi solo avanzando, o si hay que reconstruir la oleada. Despues de
 * esto el llamador avanza pasos hasta el objetivo. `target` es el paso hasta el
 * que llego la central: sirve para saber si la mitad local va adelantada (ya
 * cambio de oleada por su cuenta) o si de verdad se desvio. Devuelve true si
 * reconstruyo.
 */
bool naves_follow(struct naves_sim *s, uint32_t wave_seed, uint32_t wave_start, uint32_t target);
