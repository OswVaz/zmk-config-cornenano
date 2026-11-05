/*
 * Copyright (c) 2026
 *
 * SPDX-License-Identifier: MIT
 */

#include <string.h>

#include "naves_core.h"

/* ---------------------------------------------------------------- geometria */

#define ALIEN_W 7
#define ALIEN_H 5
#define ALIEN_STEP_X 11
#define ALIEN_STEP_Y 9
#define FORMATION_W ((NAVES_COLS - 1) * ALIEN_STEP_X + ALIEN_W)
#define FORMATION_TOP 10
#define FORMATION_DESCEND 8 /* px que baja en cada rebote; con 8 pierde ~1 de 20 oleadas */

#define SHIP_W 9
#define SHIP_H 6
#define SHIP_Y 118 /* fila de arriba de la nave */
#define SHIP_MIN_X (SHIP_W / 2 + 1)
#define SHIP_MAX_X (NAVES_W - 1 - SHIP_W / 2 - 1)

#define BULLET_SPEED 3
#define BOMB_SPEED 2
#define BOMB_CHANCE 10      /* 1 de cada N pasos suelta bomba */
#define BOMB_NOTICED 4      /* la nave ve 3 de cada 4 bombas */
#define DODGE_RANGE 18      /* px arriba de la nave en los que esquiva */
#define FIRE_COOLDOWN 7
#define SHIP_RESPAWN 30
#define WAVE_PAUSE_CLEAR 18 /* pausa tras limpiar la oleada */
#define WAVE_PAUSE_LOST 30  /* pausa tras ser invadido */
#define BOOM_SMALL 8
#define BOOM_BIG 14

/* Para la mitad que sigue: cuantos pasos antes del cambio de oleada todavia se
 * deja que llegue sola en vez de reconstruirla. */
#define FOLLOW_GRACE 16

/* ------------------------------------------------------------------ sprites */

/* Cada fila es una mascara con el pixel de la izquierda en el bit mas alto. */

/* Fila 0: orbe con un ojo y tentaculos que se cruzan. */
static const uint8_t spr_orb[2][ALIEN_H] = {
    {0x1C, 0x36, 0x3E, 0x14, 0x22},
    {0x1C, 0x36, 0x3E, 0x22, 0x14},
};

/* Filas 1 y 2: mantarraya que aletea arriba y abajo. */
static const uint8_t spr_manta[2][ALIEN_H] = {
    {0x41, 0x6B, 0x3E, 0x14, 0x00},
    {0x00, 0x08, 0x3E, 0x6B, 0x41},
};

/* Fila 3: escarabajo que abre y cierra las tenazas. */
static const uint8_t spr_beetle[2][ALIEN_H] = {
    {0x22, 0x14, 0x3E, 0x6B, 0x3E},
    {0x41, 0x22, 0x3E, 0x6B, 0x3E},
};

static const uint16_t spr_ship[SHIP_H] = {
    0x010, /* ....#.... */
    0x038, /* ...###... */
    0x028, /* ...#.#... */
    0x0FE, /* .#######. */
    0x1BB, /* ##.###.## */
    0x111, /* #...#...# */
};

static const uint8_t spr_boom[2][ALIEN_H] = {
    {0x14, 0x49, 0x22, 0x49, 0x14},
    {0x41, 0x00, 0x08, 0x00, 0x41},
};

static const uint16_t spr_boom_big[2][SHIP_H] = {
    {0x129, 0x0AA, 0x07C, 0x0BA, 0x145, 0x028},
    {0x101, 0x028, 0x082, 0x010, 0x082, 0x129},
};

/* -------------------------------------------------------------------- azar */

static uint32_t rng_next(struct naves_sim *s) {
    uint32_t x = s->rng;

    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    s->rng = x;

    return x;
}

static uint32_t rng_below(struct naves_sim *s, uint32_t n) { return rng_next(s) % n; }

/* -------------------------------------------------------------- formacion */

static bool alien_alive(const struct naves_sim *s, int row, int col) {
    return (s->alive >> (row * NAVES_COLS + col)) & 1U;
}

static bool column_alive(const struct naves_sim *s, int col) {
    for (int row = 0; row < NAVES_ROWS; row++) {
        if (alien_alive(s, row, col)) {
            return true;
        }
    }

    return false;
}

static int alive_count(const struct naves_sim *s) {
    int n = 0;

    for (uint32_t m = s->alive; m != 0; m &= m - 1) {
        n++;
    }

    return n;
}

/* Fila mas baja con vida de una columna, o -1. */
static int column_bottom(const struct naves_sim *s, int col) {
    for (int row = NAVES_ROWS - 1; row >= 0; row--) {
        if (alien_alive(s, row, col)) {
            return row;
        }
    }

    return -1;
}

static int random_alive_column(struct naves_sim *s) {
    int cols[NAVES_COLS];
    int n = 0;

    for (int col = 0; col < NAVES_COLS; col++) {
        if (column_alive(s, col)) {
            cols[n++] = col;
        }
    }

    return n == 0 ? -1 : cols[rng_below(s, n)];
}

/* Mas lenta con la formacion llena, mas rapida conforme caen. */
static int march_period(const struct naves_sim *s) { return 2 + alive_count(s) / 4; }

static void add_boom(struct naves_sim *s, int x, int y, bool big) {
    int slot = 0;

    for (int i = 0; i < NAVES_MAX_BOOMS; i++) {
        if (s->booms[i].t == 0) {
            slot = i;
            break;
        }
    }

    s->booms[slot] = (struct naves_boom){
        .x = (int8_t)x, .y = (int8_t)y, .t = big ? BOOM_BIG : BOOM_SMALL, .big = big};
}

static void march(struct naves_sim *s) {
    if (++s->march_count < march_period(s)) {
        return;
    }

    s->march_count = 0;
    s->frame ^= 1;

    int min_col = NAVES_COLS, max_col = -1;

    for (int col = 0; col < NAVES_COLS; col++) {
        if (column_alive(s, col)) {
            min_col = col < min_col ? col : min_col;
            max_col = col;
        }
    }

    if (max_col < 0) {
        return;
    }

    int left = s->fx + min_col * ALIEN_STEP_X;
    int right = s->fx + max_col * ALIEN_STEP_X + ALIEN_W - 1;

    if ((s->fdir > 0 && right >= NAVES_W - 2) || (s->fdir < 0 && left <= 1)) {
        s->fy += FORMATION_DESCEND;
        s->fdir = -s->fdir;
    } else {
        s->fx += s->fdir;
    }
}

/* ------------------------------------------------------------------ disparos */

static bool bullet_hits(struct naves_sim *s, struct naves_shot *b) {
    for (int row = 0; row < NAVES_ROWS; row++) {
        for (int col = 0; col < NAVES_COLS; col++) {
            if (!alien_alive(s, row, col)) {
                continue;
            }

            int ax = s->fx + col * ALIEN_STEP_X;
            int ay = s->fy + row * ALIEN_STEP_Y;

            if (b->x >= ax && b->x < ax + ALIEN_W && b->y <= ay + ALIEN_H - 1 && b->y + 2 >= ay) {
                s->alive &= ~(1U << (row * NAVES_COLS + col));
                add_boom(s, ax, ay, false);
                return true;
            }
        }
    }

    return false;
}

static void move_bullets(struct naves_sim *s) {
    for (int i = 0; i < NAVES_MAX_BULLETS; i++) {
        struct naves_shot *b = &s->bullets[i];

        if (!b->on) {
            continue;
        }

        b->y -= BULLET_SPEED;

        if (b->y + 2 < 0 || bullet_hits(s, b)) {
            b->on = false;
        }
    }
}

static void move_bombs(struct naves_sim *s) {
    for (int i = 0; i < NAVES_MAX_BOMBS; i++) {
        struct naves_shot *b = &s->bombs[i];

        if (!b->on) {
            continue;
        }

        b->y += BOMB_SPEED;

        if (b->y >= NAVES_H) {
            b->on = false;
            continue;
        }

        int dx = b->x - s->ship_x;

        if (s->ship_dead == 0 && dx >= -(SHIP_W / 2) && dx <= SHIP_W / 2 && b->y + 2 >= SHIP_Y &&
            b->y < SHIP_Y + SHIP_H) {
            b->on = false;
            s->ship_dead = SHIP_RESPAWN;
            add_boom(s, s->ship_x - SHIP_W / 2, SHIP_Y, true);
        }
    }
}

static void drop_bomb(struct naves_sim *s) {
    if (rng_below(s, BOMB_CHANCE) != 0) {
        return;
    }

    for (int i = 0; i < NAVES_MAX_BOMBS; i++) {
        struct naves_shot *b = &s->bombs[i];

        if (b->on) {
            continue;
        }

        int col = random_alive_column(s);

        if (col < 0) {
            return;
        }

        b->on = true;
        b->x = (int8_t)(s->fx + col * ALIEN_STEP_X + ALIEN_W / 2);
        b->y = (int16_t)(s->fy + column_bottom(s, col) * ALIEN_STEP_Y + ALIEN_H);
        s->bomb_seen[i] = rng_below(s, BOMB_NOTICED) != 0;
        return;
    }
}

/* -------------------------------------------------------------------- nave */

static void pilot_ship(struct naves_sim *s) {
    if (s->cooldown > 0) {
        s->cooldown--;
    }

    if (s->ship_dead > 0) {
        s->ship_dead--;
        return;
    }

    /* Esquivar manda sobre todo lo demas. */
    int dodge = 0;

    for (int i = 0; i < NAVES_MAX_BOMBS; i++) {
        const struct naves_shot *b = &s->bombs[i];
        int dx = b->x - s->ship_x;

        if (b->on && s->bomb_seen[i] && b->y < SHIP_Y + SHIP_H && b->y >= SHIP_Y - DODGE_RANGE &&
            dx >= -(SHIP_W / 2 + 1) && dx <= SHIP_W / 2 + 1) {
            dodge = dx > 0 ? -1 : (dx < 0 ? 1 : (s->ship_x < NAVES_W / 2 ? 1 : -1));
            break;
        }
    }

    int target_x = s->ship_x;

    if (s->alive != 0 && s->wave_end == 0) {
        if (s->target_col < 0 || !column_alive(s, s->target_col) || rng_below(s, 40) == 0) {
            s->target_col = (int8_t)random_alive_column(s);
        }

        int row = column_bottom(s, s->target_col);
        int ay = s->fy + row * ALIEN_STEP_Y;

        /* Apunta un poco adelante: lo que avanza la formacion mientras sube la
         * bala. Cerca de las orillas falla, y esta bien que falle. */
        int flight = (SHIP_Y - ay) / BULLET_SPEED;

        target_x = s->fx + s->target_col * ALIEN_STEP_X + ALIEN_W / 2 +
                   s->fdir * flight / march_period(s);
    }

    int move = dodge != 0 ? dodge : (target_x > s->ship_x) - (target_x < s->ship_x);
    int x = s->ship_x + move;

    s->ship_x = (int8_t)(x < SHIP_MIN_X ? SHIP_MIN_X : (x > SHIP_MAX_X ? SHIP_MAX_X : x));

    int off = target_x - s->ship_x;

    if (dodge == 0 && s->alive != 0 && s->wave_end == 0 && s->cooldown == 0 && off >= -1 &&
        off <= 1) {
        for (int i = 0; i < NAVES_MAX_BULLETS; i++) {
            if (!s->bullets[i].on) {
                s->bullets[i] =
                    (struct naves_shot){.x = s->ship_x, .y = SHIP_Y - 3, .on = true};
                s->cooldown = FIRE_COOLDOWN;
                break;
            }
        }
    }
}

/* ----------------------------------------------------------------- oleadas */

void naves_start_wave(struct naves_sim *s, uint32_t seed, uint32_t global_step) {
    memset(s, 0, sizeof(*s));

    s->wave_seed = seed;
    s->wave_start = global_step;
    s->step = global_step;
    s->rng = seed != 0 ? seed : 0x6D2B79F5U;

    s->alive = (1U << (NAVES_ROWS * NAVES_COLS)) - 1;
    s->fx = (int8_t)((NAVES_W - FORMATION_W) / 2);
    s->fy = FORMATION_TOP;
    s->fdir = (rng_next(s) & 1) ? 1 : -1;

    s->ship_x = NAVES_W / 2;
    s->target_col = -1;

    for (int i = 0; i < NAVES_STARS; i++) {
        s->star_x[i] = (uint8_t)rng_below(s, NAVES_W);
        s->star_y[i] = (uint8_t)rng_below(s, NAVES_H);
    }
}

void naves_step(struct naves_sim *s) {
    s->step++;

    /* Estrellas: bajan lento, para que se sienta que la nave avanza. */
    if (s->step % 3 == 0) {
        for (int i = 0; i < NAVES_STARS; i++) {
            s->star_y[i] = (uint8_t)((s->star_y[i] + 1) % NAVES_H);
        }
    }

    for (int i = 0; i < NAVES_MAX_BOOMS; i++) {
        if (s->booms[i].t > 0) {
            s->booms[i].t--;
        }
    }

    if (s->wave_end > 0) {
        move_bullets(s);
        move_bombs(s);

        if (--s->wave_end == 0) {
            /* El seed de la siguiente oleada sale del azar de esta, asi que la
             * otra mitad llega al mismo seed sin que nadie se lo diga. */
            naves_start_wave(s, rng_next(s), s->step);
        }

        return;
    }

    march(s);
    move_bullets(s);
    move_bombs(s);
    drop_bomb(s);
    pilot_ship(s);

    if (s->alive == 0) {
        s->wave_end = WAVE_PAUSE_CLEAR;
        return;
    }

    /* Invadidos: la formacion llego hasta la nave. */
    int bottom_row = -1;

    for (int col = 0; col < NAVES_COLS; col++) {
        int row = column_bottom(s, col);
        bottom_row = row > bottom_row ? row : bottom_row;
    }

    if (s->fy + bottom_row * ALIEN_STEP_Y + ALIEN_H >= SHIP_Y - 2) {
        if (s->ship_dead == 0) {
            add_boom(s, s->ship_x - SHIP_W / 2, SHIP_Y, true);
        }

        s->ship_dead = WAVE_PAUSE_LOST;
        s->wave_end = WAVE_PAUSE_LOST;
    }
}

bool naves_follow(struct naves_sim *s, uint32_t wave_seed, uint32_t wave_start, uint32_t target) {
    if (s->wave_seed == wave_seed && s->wave_start == wave_start) {
        return false;
    }

    /* Aqui ya se cambio de oleada pero la central todavia no llega a ese paso
     * (en autoplay cada mitad avanza con su propio reloj y esta puede ir unos
     * pasos adelante). Es la continuacion normal, no hay que tocar nada. Si la
     * central ya paso ese paso y sigue en otra oleada, entonces si se desvio. */
    if ((int32_t)(s->wave_start - wave_start) > 0 && (int32_t)(s->wave_start - target) > 0) {
        return false;
    }

    /* La central ya cambio de oleada y aqui falta poco para el mismo cambio:
     * se deja llegar sola para no cortar la explosion final. Si al llegar no
     * coincide, la siguiente llamada la reconstruye. */
    int32_t ahead = (int32_t)(wave_start - s->step);

    if (ahead > 0 && ahead <= FOLLOW_GRACE) {
        return false;
    }

    naves_start_wave(s, wave_seed, wave_start);
    return true;
}

/* ------------------------------------------------------------------- dibujo */

static void put_px(uint8_t *fb, int half, int wx, int wy) {
    int lx = wx - half * NAVES_PANEL_W;

    if (lx < 0 || lx >= NAVES_PANEL_W || wy < 0 || wy >= NAVES_H) {
        return;
    }

#if NAVES_TOP_IS_LVGL_X_MAX
    int px = NAVES_FB_W - 1 - wy;
#else
    int px = wy;
#endif

#if NAVES_MIRROR_X
    int py = NAVES_FB_H - 1 - lx;
#else
    int py = lx;
#endif

    fb[py * NAVES_FB_STRIDE + px / 8] |= (uint8_t)(0x80 >> (px % 8));
}

static void draw_rows8(uint8_t *fb, int half, int x, int y, const uint8_t *rows, int w, int h) {
    for (int r = 0; r < h; r++) {
        for (int c = 0; c < w; c++) {
            if ((rows[r] >> (w - 1 - c)) & 1U) {
                put_px(fb, half, x + c, y + r);
            }
        }
    }
}

static void draw_rows16(uint8_t *fb, int half, int x, int y, const uint16_t *rows, int w, int h) {
    for (int r = 0; r < h; r++) {
        for (int c = 0; c < w; c++) {
            if ((rows[r] >> (w - 1 - c)) & 1U) {
                put_px(fb, half, x + c, y + r);
            }
        }
    }
}

void naves_render(const struct naves_sim *s, uint8_t *fb, int half, int sway, bool flip) {
    memset(fb, 0, NAVES_FB_SIZE);

    for (int i = 0; i < NAVES_STARS; i++) {
        put_px(fb, half, s->star_x[i], s->star_y[i]);
    }

    int frame = s->frame ^ (flip ? 1 : 0);

    for (int row = 0; row < NAVES_ROWS; row++) {
        const uint8_t(*spr)[ALIEN_H] =
            row == 0 ? spr_orb : (row == NAVES_ROWS - 1 ? spr_beetle : spr_manta);

        for (int col = 0; col < NAVES_COLS; col++) {
            if (alien_alive(s, row, col)) {
                draw_rows8(fb, half, s->fx + sway + col * ALIEN_STEP_X,
                           s->fy + row * ALIEN_STEP_Y, spr[frame], ALIEN_W, ALIEN_H);
            }
        }
    }

    for (int i = 0; i < NAVES_MAX_BULLETS; i++) {
        if (s->bullets[i].on) {
            for (int d = 0; d < 3; d++) {
                put_px(fb, half, s->bullets[i].x, s->bullets[i].y + d);
            }
        }
    }

    for (int i = 0; i < NAVES_MAX_BOMBS; i++) {
        if (s->bombs[i].on) {
            /* Zigzag que alterna con la altura, para que se vea caer. */
            int z = (s->bombs[i].y / BOMB_SPEED) & 1;

            put_px(fb, half, s->bombs[i].x + z, s->bombs[i].y);
            put_px(fb, half, s->bombs[i].x + 1 - z, s->bombs[i].y + 1);
            put_px(fb, half, s->bombs[i].x + z, s->bombs[i].y + 2);
        }
    }

    for (int i = 0; i < NAVES_MAX_BOOMS; i++) {
        const struct naves_boom *b = &s->booms[i];

        if (b->t == 0) {
            continue;
        }

        if (b->big) {
            draw_rows16(fb, half, b->x, b->y, spr_boom_big[b->t > BOOM_BIG / 2 ? 0 : 1], SHIP_W,
                        SHIP_H);
        } else {
            draw_rows8(fb, half, b->x, b->y, spr_boom[b->t > BOOM_SMALL / 2 ? 0 : 1], ALIEN_W,
                       ALIEN_H);
        }
    }

    if (s->ship_dead == 0) {
        draw_rows16(fb, half, s->ship_x - SHIP_W / 2, SHIP_Y, spr_ship, SHIP_W, SHIP_H);
    }
}
