/*******************************************************************************
 * Size: 8 px
 * Bpp: 1
 * Opts: --font /System/Library/Fonts/Supplemental/Arial Unicode.ttf --size 8 --bpp 1 --no-compress --format lvgl -r 0x30A1-0x30F6 -o src/display/matrix_katakana_8.c
 ******************************************************************************/

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#elif __has_include(<lvgl.h>)
#include <lvgl.h>
#else
#include "lvgl/lvgl.h"
#endif

#ifndef MATRIX_KATAKANA_8
#define MATRIX_KATAKANA_8 1
#endif

#if MATRIX_KATAKANA_8

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+30A1 "ァ" */
    0xf9, 0x4c, 0x44, 0x0,

    /* U+30A2 "ア" */
    0xfe, 0x28, 0x50, 0x83, 0x4, 0x10, 0x0,

    /* U+30A3 "ィ" */
    0x1, 0x6a, 0x20,

    /* U+30A4 "イ" */
    0x4, 0x21, 0x1c, 0x90, 0x41, 0x0,

    /* U+30A5 "ゥ" */
    0xf, 0x91, 0x20,

    /* U+30A6 "ウ" */
    0xff, 0x6, 0x8, 0x20, 0xc6, 0x0,

    /* U+30A7 "ェ" */
    0xf9, 0x9, 0xf0,

    /* U+30A8 "エ" */
    0xfe, 0x20, 0x40, 0x81, 0x1f, 0xc0,

    /* U+30A9 "ォ" */
    0xf9, 0x95, 0x23, 0x0,

    /* U+30AA "オ" */
    0x9, 0xfc, 0x61, 0x4c, 0x83, 0x0,

    /* U+30AB "カ" */
    0x23, 0xf2, 0x59, 0x46, 0x60,

    /* U+30AC "ガ" */
    0x2f, 0xf2, 0x51, 0x46, 0x60,

    /* U+30AD "キ" */
    0x23, 0xe2, 0xfc, 0x10, 0x40,

    /* U+30AE "ギ" */
    0x0, 0xaf, 0x9, 0xf8, 0x81, 0x0,

    /* U+30AF "ク" */
    0x21, 0xf4, 0x62, 0x8, 0x46, 0x0,

    /* U+30B0 "グ" */
    0x1, 0x27, 0xa2, 0x8, 0x42, 0x10,

    /* U+30B1 "ケ" */
    0x20, 0xfd, 0x20, 0x41, 0x82, 0x8, 0x0,

    /* U+30B2 "ゲ" */
    0x2, 0x4d, 0xfa, 0x40, 0x83, 0x4, 0x10,

    /* U+30B3 "コ" */
    0xfc, 0x10, 0x41, 0x7, 0xf0,

    /* U+30B4 "ゴ" */
    0xb, 0xf8, 0x10, 0x20, 0x40, 0xbf, 0x0,

    /* U+30B5 "サ" */
    0x49, 0xfd, 0x22, 0x40, 0x83, 0x0, 0x0,

    /* U+30B6 "ザ" */
    0x4b, 0xfe, 0x48, 0x48, 0x8, 0x18, 0x0,

    /* U+30B7 "シ" */
    0x60, 0xc, 0x42, 0x13, 0x80,

    /* U+30B8 "ジ" */
    0x6c, 0xc, 0x42, 0x13, 0x0,

    /* U+30B9 "ス" */
    0xf8, 0x10, 0x40, 0x82, 0x98, 0x80, 0x0,

    /* U+30BA "ズ" */
    0x2, 0x7a, 0x8, 0x8, 0x18, 0x14, 0x62, 0x0,

    /* U+30BB "セ" */
    0x20, 0x43, 0xf1, 0x22, 0x7, 0xc0,

    /* U+30BC "ゼ" */
    0x26, 0x43, 0xf1, 0x22, 0x7, 0xc0,

    /* U+30BD "ソ" */
    0x85, 0x10, 0x42, 0x19, 0xc0,

    /* U+30BE "ゾ" */
    0x6, 0x34, 0x42, 0x8, 0x46, 0x0,

    /* U+30BF "タ" */
    0x21, 0xf4, 0xae, 0x18, 0xcc, 0x0,

    /* U+30C0 "ダ" */
    0x0, 0x48, 0xf2, 0x21, 0x41, 0x85, 0x30,

    /* U+30C1 "チ" */
    0x0, 0xf0, 0x47, 0xf1, 0x2, 0x8, 0x0,

    /* U+30C2 "ヂ" */
    0x0, 0xf4, 0x47, 0xf1, 0x2, 0x8, 0x0,

    /* U+30C3 "ッ" */
    0xab, 0x44, 0x44, 0x0,

    /* U+30C4 "ツ" */
    0xa6, 0x90, 0xc2, 0x11, 0x80,

    /* U+30C5 "ヅ" */
    0x6, 0xba, 0x42, 0x8, 0x46, 0x0,

    /* U+30C6 "テ" */
    0x7c, 0x3, 0xf8, 0x81, 0x4, 0x0, 0x0,

    /* U+30C7 "デ" */
    0x7a, 0x3, 0xf8, 0x81, 0x4, 0x0, 0x0,

    /* U+30C8 "ト" */
    0x84, 0x39, 0x28, 0x40,

    /* U+30C9 "ド" */
    0x85, 0x39, 0x28, 0x40,

    /* U+30CA "ナ" */
    0x11, 0xfc, 0x40, 0x81, 0xc, 0x0,

    /* U+30CB "ニ" */
    0x7c, 0x0, 0x0, 0x0, 0x1f, 0xc0,

    /* U+30CC "ヌ" */
    0xf8, 0x26, 0x84, 0x29, 0x18, 0x0,

    /* U+30CD "ネ" */
    0x20, 0x33, 0xf8, 0x43, 0x1a, 0xc4, 0x0,

    /* U+30CE "ノ" */
    0x4, 0x20, 0x84, 0x31, 0x0,

    /* U+30CF "ハ" */
    0x49, 0x24, 0x91, 0x86, 0x10,

    /* U+30D0 "バ" */
    0x2b, 0x24, 0x24, 0x44, 0x42, 0x42,

    /* U+30D1 "パ" */
    0x6, 0x9d, 0x22, 0x24, 0x50, 0xa0, 0x80,

    /* U+30D2 "ヒ" */
    0x82, 0x3f, 0x20, 0x83, 0xf0,

    /* U+30D3 "ビ" */
    0x81, 0x13, 0xc4, 0x8, 0x1f, 0x80,

    /* U+30D4 "ピ" */
    0x87, 0xe, 0xe6, 0x8, 0x1f, 0x80,

    /* U+30D5 "フ" */
    0xfc, 0x30, 0x82, 0x10, 0x84, 0x0,

    /* U+30D6 "ブ" */
    0x0, 0x2f, 0x82, 0x8, 0x42, 0x10,

    /* U+30D7 "プ" */
    0x7, 0xf0, 0x20, 0x81, 0x4, 0x10, 0x0,

    /* U+30D8 "ヘ" */
    0x30, 0x68, 0x48, 0x6, 0x0,

    /* U+30D9 "ベ" */
    0x36, 0x68, 0x48, 0x6, 0x0,

    /* U+30DA "ペ" */
    0x2c, 0xda, 0x44, 0x40, 0x40, 0x40,

    /* U+30DB "ホ" */
    0x23, 0xf6, 0x9a, 0xa4, 0x80,

    /* U+30DC "ボ" */
    0x27, 0xf6, 0x9a, 0xa4, 0x80,

    /* U+30DD "ポ" */
    0x4, 0x9f, 0xda, 0x6a, 0x92, 0x0,

    /* U+30DE "マ" */
    0xfc, 0x24, 0x8c, 0x10, 0x20,

    /* U+30DF "ミ" */
    0x78, 0x7, 0x2, 0xc0, 0xf0,

    /* U+30E0 "ム" */
    0x20, 0x84, 0x92, 0x4f, 0xd0,

    /* U+30E1 "メ" */
    0x9, 0x23, 0x6, 0x2b, 0x0,

    /* U+30E2 "モ" */
    0xfe, 0x23, 0xf8, 0x81, 0x3, 0xc0,

    /* U+30E3 "ャ" */
    0x5f, 0x44, 0x20,

    /* U+30E4 "ヤ" */
    0x21, 0xfa, 0x8a, 0x20, 0x40,

    /* U+30E5 "ュ" */
    0x70, 0x85, 0xf0,

    /* U+30E6 "ユ" */
    0x7c, 0x8, 0x20, 0x40, 0x9f, 0xc0,

    /* U+30E7 "ョ" */
    0xf8, 0x5f, 0xf0, 0x80,

    /* U+30E8 "ヨ" */
    0xfc, 0x1f, 0xc1, 0x7, 0xf0,

    /* U+30E9 "ラ" */
    0xfc, 0xf, 0xc1, 0x8, 0x46, 0x0,

    /* U+30EA "リ" */
    0x8c, 0x63, 0x11, 0x98, 0x0,

    /* U+30EB "ル" */
    0x28, 0x28, 0x28, 0x28, 0x2a, 0x4c, 0x8,

    /* U+30EC "レ" */
    0x82, 0x8, 0x21, 0x9b, 0x80,

    /* U+30ED "ロ" */
    0xfe, 0x18, 0x61, 0x87, 0xf0,

    /* U+30EE "ヮ" */
    0xfc, 0x42, 0x22, 0x0,

    /* U+30EF "ワ" */
    0xfe, 0x18, 0x41, 0x8, 0x42, 0x0,

    /* U+30F0 "ヰ" */
    0x9, 0xfd, 0x27, 0xf0, 0x81, 0x0,

    /* U+30F1 "ヱ" */
    0x7e, 0x8, 0x60, 0x81, 0x1f, 0xc0,

    /* U+30F2 "ヲ" */
    0xfc, 0x1f, 0xc1, 0x8, 0x40, 0x0,

    /* U+30F3 "ン" */
    0xc0, 0x0, 0x42, 0x13, 0x80,

    /* U+30F4 "ヴ" */
    0x15, 0xfa, 0x14, 0x20, 0x41, 0xc, 0x0,

    /* U+30F5 "ヵ" */
    0x27, 0xca, 0x54, 0xcc,

    /* U+30F6 "ヶ" */
    0x41, 0xf9, 0x4, 0x20, 0x0
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 128, .box_w = 5, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 4, .adv_w = 128, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 11, .adv_w = 128, .box_w = 4, .box_h = 5, .ofs_x = 2, .ofs_y = 1},
    {.bitmap_index = 14, .adv_w = 128, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 20, .adv_w = 128, .box_w = 4, .box_h = 6, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 23, .adv_w = 128, .box_w = 7, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 29, .adv_w = 128, .box_w = 5, .box_h = 4, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 32, .adv_w = 128, .box_w = 7, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 38, .adv_w = 128, .box_w = 5, .box_h = 5, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 42, .adv_w = 128, .box_w = 7, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 48, .adv_w = 128, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 53, .adv_w = 128, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 58, .adv_w = 128, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 63, .adv_w = 128, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 69, .adv_w = 128, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 75, .adv_w = 128, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 81, .adv_w = 128, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 88, .adv_w = 128, .box_w = 7, .box_h = 8, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 95, .adv_w = 128, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 100, .adv_w = 128, .box_w = 7, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 107, .adv_w = 128, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 114, .adv_w = 128, .box_w = 8, .box_h = 7, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 121, .adv_w = 128, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 126, .adv_w = 128, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 131, .adv_w = 128, .box_w = 7, .box_h = 7, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 138, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 146, .adv_w = 128, .box_w = 7, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 152, .adv_w = 128, .box_w = 7, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 158, .adv_w = 128, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 163, .adv_w = 128, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 169, .adv_w = 128, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 175, .adv_w = 128, .box_w = 7, .box_h = 8, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 182, .adv_w = 128, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 189, .adv_w = 128, .box_w = 7, .box_h = 8, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 196, .adv_w = 128, .box_w = 5, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 200, .adv_w = 128, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 205, .adv_w = 128, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 211, .adv_w = 128, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 218, .adv_w = 128, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 225, .adv_w = 128, .box_w = 5, .box_h = 6, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 229, .adv_w = 128, .box_w = 5, .box_h = 6, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 233, .adv_w = 128, .box_w = 7, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 239, .adv_w = 128, .box_w = 7, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 245, .adv_w = 128, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 251, .adv_w = 128, .box_w = 7, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 258, .adv_w = 128, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 263, .adv_w = 128, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 268, .adv_w = 128, .box_w = 8, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 274, .adv_w = 128, .box_w = 7, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 281, .adv_w = 128, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 286, .adv_w = 128, .box_w = 7, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 292, .adv_w = 128, .box_w = 7, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 298, .adv_w = 128, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 304, .adv_w = 128, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 310, .adv_w = 128, .box_w = 7, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 317, .adv_w = 128, .box_w = 8, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 322, .adv_w = 128, .box_w = 8, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 327, .adv_w = 128, .box_w = 7, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 333, .adv_w = 128, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 338, .adv_w = 128, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 343, .adv_w = 128, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 349, .adv_w = 128, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 354, .adv_w = 128, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 359, .adv_w = 128, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 364, .adv_w = 128, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 369, .adv_w = 128, .box_w = 7, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 375, .adv_w = 128, .box_w = 4, .box_h = 5, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 378, .adv_w = 128, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 383, .adv_w = 128, .box_w = 5, .box_h = 4, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 386, .adv_w = 128, .box_w = 7, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 392, .adv_w = 128, .box_w = 5, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 396, .adv_w = 128, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 401, .adv_w = 128, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 407, .adv_w = 128, .box_w = 5, .box_h = 7, .ofs_x = 2, .ofs_y = -1},
    {.bitmap_index = 412, .adv_w = 128, .box_w = 8, .box_h = 7, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 419, .adv_w = 128, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 424, .adv_w = 128, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 429, .adv_w = 128, .box_w = 5, .box_h = 5, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 433, .adv_w = 128, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 439, .adv_w = 128, .box_w = 7, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 445, .adv_w = 128, .box_w = 7, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 451, .adv_w = 128, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 457, .adv_w = 128, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 462, .adv_w = 128, .box_w = 7, .box_h = 7, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 469, .adv_w = 128, .box_w = 5, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 473, .adv_w = 128, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 0}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/



/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 12449, .range_length = 86, .glyph_id_start = 1,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    }
};



/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

#if LVGL_VERSION_MAJOR == 8
/*Store all the custom data of the font*/
static  lv_font_fmt_txt_glyph_cache_t cache;
#endif

#if LVGL_VERSION_MAJOR >= 8
static const lv_font_fmt_txt_dsc_t font_dsc = {
#else
static lv_font_fmt_txt_dsc_t font_dsc = {
#endif
    .glyph_bitmap = glyph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = NULL,
    .kern_scale = 0,
    .cmap_num = 1,
    .bpp = 1,
    .kern_classes = 0,
    .bitmap_format = 0,
#if LVGL_VERSION_MAJOR == 8
    .cache = &cache
#endif
};



/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
#if LVGL_VERSION_MAJOR >= 8
const lv_font_t matrix_katakana_8 = {
#else
lv_font_t matrix_katakana_8 = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 9,          /*The maximum line height required by the font*/
    .base_line = 1,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -1,
    .underline_thickness = 0,
#endif
    .dsc = &font_dsc,          /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
#if LV_VERSION_CHECK(8, 2, 0) || LVGL_VERSION_MAJOR >= 9
    .fallback = NULL,
#endif
    .user_data = NULL,
};



#endif /*#if MATRIX_KATAKANA_8*/

