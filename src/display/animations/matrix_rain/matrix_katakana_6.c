/*******************************************************************************
 * Size: 6 px
 * Bpp: 1
 * Opts: --font /System/Library/Fonts/Supplemental/Arial Unicode.ttf --size 6 --bpp 1 --no-compress --format lvgl -r 0x20 -r 0x30A1-0x30F6 -o matrix_katakana_6.c
 ******************************************************************************/

/* En el build de Zephyr el include path apunta a lvgl/src, donde si esta
 * lvgl.h. El bloque que genera lv_font_conv por defecto no lo encuentra. */
#include <lvgl.h>

#ifndef MATRIX_KATAKANA_6
#define MATRIX_KATAKANA_6 1
#endif

#if MATRIX_KATAKANA_6

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0020 " " */
    0x0,

    /* U+30A1 "ァ" */
    0xf6, 0x48,

    /* U+30A2 "ア" */
    0xf9, 0x4c, 0x44, 0x0,

    /* U+30A3 "ィ" */
    0x16, 0x22,

    /* U+30A4 "イ" */
    0x12, 0x6a, 0x20,

    /* U+30A5 "ゥ" */
    0x27, 0xe4, 0x22, 0x0,

    /* U+30A6 "ウ" */
    0x7, 0xe4, 0x21, 0x0,

    /* U+30A7 "ェ" */
    0xf4, 0x4f,

    /* U+30A8 "エ" */
    0xf9, 0x8, 0x4f, 0x80,

    /* U+30A9 "ォ" */
    0xf2, 0xe2,

    /* U+30AA "オ" */
    0x17, 0xcd, 0xa3, 0x0,

    /* U+30AB "カ" */
    0x27, 0xca, 0x99, 0x80,

    /* U+30AC "ガ" */
    0x2f, 0xca, 0x99, 0x80,

    /* U+30AD "キ" */
    0x23, 0x86, 0xc1, 0x0,

    /* U+30AE "ギ" */
    0x29, 0xc1, 0x98, 0x20,

    /* U+30AF "ク" */
    0x47, 0x92, 0x24,

    /* U+30B0 "グ" */
    0x2c, 0xe4, 0x84, 0x21, 0x0,

    /* U+30B1 "ケ" */
    0x41, 0xf9, 0x4, 0x20, 0x0,

    /* U+30B2 "ゲ" */
    0x4d, 0xf9, 0x4, 0x20, 0x0,

    /* U+30B3 "コ" */
    0xf8, 0x42, 0x1f, 0x80,

    /* U+30B4 "ゴ" */
    0x7, 0xc2, 0x1f, 0x80,

    /* U+30B5 "サ" */
    0x57, 0xd4, 0x22, 0x0,

    /* U+30B6 "ザ" */
    0x5f, 0xd4, 0x22, 0x0,

    /* U+30B7 "シ" */
    0x41, 0x12, 0x26, 0x0,

    /* U+30B8 "ジ" */
    0x49, 0x12, 0x26, 0x0,

    /* U+30B9 "ス" */
    0x70, 0x41, 0xa, 0x44,

    /* U+30BA "ズ" */
    0x5, 0xc1, 0x8, 0x31, 0x20,

    /* U+30BB "セ" */
    0x43, 0xf4, 0x87, 0x80,

    /* U+30BC "ゼ" */
    0x4b, 0xf4, 0x87, 0x80,

    /* U+30BD "ソ" */
    0xa, 0x42, 0x26, 0x0,

    /* U+30BE "ゾ" */
    0x8, 0x12, 0x40,

    /* U+30BF "タ" */
    0x47, 0x92, 0x3c,

    /* U+30C0 "ダ" */
    0x2b, 0xd4, 0x62, 0x20,

    /* U+30C1 "チ" */
    0x13, 0x3e, 0x42, 0x20,

    /* U+30C2 "ヂ" */
    0x13, 0x3e, 0x42, 0x20,

    /* U+30C3 "ッ" */
    0xd1, 0x24,

    /* U+30C4 "ツ" */
    0x5, 0x42, 0x26, 0x0,

    /* U+30C5 "ヅ" */
    0x4d, 0x92, 0x40,

    /* U+30C6 "テ" */
    0x77, 0xc8, 0x44, 0x0,

    /* U+30C7 "デ" */
    0x3, 0x3e, 0x42, 0x20,

    /* U+30C8 "ト" */
    0x88, 0xe9, 0x80,

    /* U+30C9 "ド" */
    0x8a, 0xe9, 0x80,

    /* U+30CA "ナ" */
    0x27, 0xc8, 0x44, 0x0,

    /* U+30CB "ニ" */
    0xf8, 0x0, 0xf, 0x80,

    /* U+30CC "ヌ" */
    0xf2, 0x65, 0x80,

    /* U+30CD "ネ" */
    0x37, 0xc5, 0xd2, 0x0,

    /* U+30CE "ノ" */
    0x12, 0x2c,

    /* U+30CF "ハ" */
    0x31, 0x24, 0x92, 0x88,

    /* U+30D0 "バ" */
    0x55, 0x44, 0x92, 0x88,

    /* U+30D1 "パ" */
    0xd, 0x75, 0x22, 0x8a, 0x0,

    /* U+30D2 "ヒ" */
    0x85, 0xb1, 0xf, 0x80,

    /* U+30D3 "ビ" */
    0x85, 0xb1, 0xf, 0x80,

    /* U+30D4 "ピ" */
    0x9c, 0xf9, 0xf, 0x0,

    /* U+30D5 "フ" */
    0xf1, 0x22, 0x40,

    /* U+30D6 "ブ" */
    0x17, 0x88, 0x44, 0x40,

    /* U+30D7 "プ" */
    0xf, 0xc1, 0x8, 0x23, 0x0,

    /* U+30D8 "ヘ" */
    0x21, 0x49, 0x3,

    /* U+30D9 "ベ" */
    0x29, 0x49, 0x3,

    /* U+30DA "ペ" */
    0x5a, 0xe9, 0x2, 0x0,

    /* U+30DB "ホ" */
    0x27, 0xdd, 0x52, 0x0,

    /* U+30DC "ボ" */
    0x27, 0xdd, 0x52, 0x0,

    /* U+30DD "ポ" */
    0x2f, 0xdd, 0x52, 0x0,

    /* U+30DE "マ" */
    0xf1, 0xa4, 0x20,

    /* U+30DF "ミ" */
    0x43, 0x60, 0xf0,

    /* U+30E0 "ム" */
    0x0, 0x85, 0x12, 0x78,

    /* U+30E1 "メ" */
    0x12, 0x66, 0x90,

    /* U+30E2 "モ" */
    0xf9, 0x3e, 0x43, 0x80,

    /* U+30E3 "ャ" */
    0x5e, 0x42,

    /* U+30E4 "ヤ" */
    0x47, 0xd4, 0x40,

    /* U+30E5 "ュ" */
    0x62, 0x2f,

    /* U+30E6 "ユ" */
    0x70, 0x84, 0x2f, 0x80,

    /* U+30E7 "ョ" */
    0x7f, 0x1f,

    /* U+30E8 "ヨ" */
    0xf8, 0x7e, 0x1f, 0x80,

    /* U+30E9 "ラ" */
    0x77, 0xc2, 0x26, 0x0,

    /* U+30EA "リ" */
    0x99, 0x91, 0x60,

    /* U+30EB "ル" */
    0x63, 0x18, 0xd9, 0x0,

    /* U+30EC "レ" */
    0x84, 0x21, 0x2e, 0x0,

    /* U+30ED "ロ" */
    0xfc, 0x63, 0x1f, 0x80,

    /* U+30EE "ヮ" */
    0xf9, 0x16,

    /* U+30EF "ワ" */
    0xfc, 0x42, 0x22, 0x0,

    /* U+30F0 "ヰ" */
    0x27, 0xe9, 0xf2, 0x0,

    /* U+30F1 "ヱ" */
    0xf8, 0x88, 0x4f, 0x80,

    /* U+30F2 "ヲ" */
    0xf8, 0x7e, 0x11, 0x30,

    /* U+30F3 "ン" */
    0xc, 0x12, 0xc0,

    /* U+30F4 "ヴ" */
    0x27, 0xe2, 0x11, 0x10,

    /* U+30F5 "ヵ" */
    0x2f, 0x35, 0x90,

    /* U+30F6 "ヶ" */
    0x7, 0xa2, 0x40
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 96, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1, .adv_w = 96, .box_w = 4, .box_h = 4, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 3, .adv_w = 96, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 7, .adv_w = 96, .box_w = 4, .box_h = 4, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 9, .adv_w = 96, .box_w = 4, .box_h = 5, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 12, .adv_w = 96, .box_w = 5, .box_h = 5, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 16, .adv_w = 96, .box_w = 5, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 20, .adv_w = 96, .box_w = 4, .box_h = 4, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 22, .adv_w = 96, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 26, .adv_w = 96, .box_w = 4, .box_h = 4, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 28, .adv_w = 96, .box_w = 5, .box_h = 5, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 32, .adv_w = 96, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 36, .adv_w = 96, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 40, .adv_w = 96, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 44, .adv_w = 96, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 48, .adv_w = 96, .box_w = 4, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 51, .adv_w = 96, .box_w = 6, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 56, .adv_w = 96, .box_w = 6, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 61, .adv_w = 96, .box_w = 6, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 66, .adv_w = 96, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 70, .adv_w = 96, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 74, .adv_w = 96, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 78, .adv_w = 96, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 82, .adv_w = 96, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 86, .adv_w = 96, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 90, .adv_w = 96, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 94, .adv_w = 96, .box_w = 6, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 99, .adv_w = 96, .box_w = 5, .box_h = 5, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 103, .adv_w = 96, .box_w = 5, .box_h = 5, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 107, .adv_w = 96, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 111, .adv_w = 96, .box_w = 4, .box_h = 5, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 114, .adv_w = 96, .box_w = 4, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 117, .adv_w = 96, .box_w = 5, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 121, .adv_w = 96, .box_w = 5, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 125, .adv_w = 96, .box_w = 5, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 129, .adv_w = 96, .box_w = 4, .box_h = 4, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 131, .adv_w = 96, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 135, .adv_w = 96, .box_w = 4, .box_h = 5, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 138, .adv_w = 96, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 142, .adv_w = 96, .box_w = 5, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 146, .adv_w = 96, .box_w = 4, .box_h = 5, .ofs_x = 2, .ofs_y = 1},
    {.bitmap_index = 149, .adv_w = 96, .box_w = 4, .box_h = 5, .ofs_x = 2, .ofs_y = 1},
    {.bitmap_index = 152, .adv_w = 96, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 156, .adv_w = 96, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 160, .adv_w = 96, .box_w = 4, .box_h = 5, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 163, .adv_w = 96, .box_w = 5, .box_h = 5, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 167, .adv_w = 96, .box_w = 4, .box_h = 4, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 169, .adv_w = 96, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 173, .adv_w = 96, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 177, .adv_w = 96, .box_w = 6, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 182, .adv_w = 96, .box_w = 5, .box_h = 5, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 186, .adv_w = 96, .box_w = 5, .box_h = 5, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 190, .adv_w = 96, .box_w = 5, .box_h = 5, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 194, .adv_w = 96, .box_w = 4, .box_h = 5, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 197, .adv_w = 96, .box_w = 5, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 201, .adv_w = 96, .box_w = 6, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 206, .adv_w = 96, .box_w = 6, .box_h = 4, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 209, .adv_w = 96, .box_w = 6, .box_h = 4, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 212, .adv_w = 96, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 216, .adv_w = 96, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 220, .adv_w = 96, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 224, .adv_w = 96, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 228, .adv_w = 96, .box_w = 4, .box_h = 5, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 231, .adv_w = 96, .box_w = 4, .box_h = 5, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 234, .adv_w = 96, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 238, .adv_w = 96, .box_w = 4, .box_h = 5, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 241, .adv_w = 96, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 245, .adv_w = 96, .box_w = 4, .box_h = 4, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 247, .adv_w = 96, .box_w = 4, .box_h = 5, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 250, .adv_w = 96, .box_w = 4, .box_h = 4, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 252, .adv_w = 96, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 256, .adv_w = 96, .box_w = 4, .box_h = 4, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 258, .adv_w = 96, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 262, .adv_w = 96, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 266, .adv_w = 96, .box_w = 4, .box_h = 5, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 269, .adv_w = 96, .box_w = 5, .box_h = 5, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 273, .adv_w = 96, .box_w = 5, .box_h = 5, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 277, .adv_w = 96, .box_w = 5, .box_h = 5, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 281, .adv_w = 96, .box_w = 4, .box_h = 4, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 283, .adv_w = 96, .box_w = 5, .box_h = 5, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 287, .adv_w = 96, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 291, .adv_w = 96, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 295, .adv_w = 96, .box_w = 5, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 299, .adv_w = 96, .box_w = 4, .box_h = 5, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 302, .adv_w = 96, .box_w = 5, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 306, .adv_w = 96, .box_w = 4, .box_h = 5, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 309, .adv_w = 96, .box_w = 4, .box_h = 5, .ofs_x = 1, .ofs_y = 1}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/



/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 32, .range_length = 1, .glyph_id_start = 1,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    },
    {
        .range_start = 12449, .range_length = 86, .glyph_id_start = 2,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    }
};



/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

#if LV_VERSION_CHECK(8, 0, 0)
/*Store all the custom data of the font*/
static  lv_font_fmt_txt_glyph_cache_t cache;
static const lv_font_fmt_txt_dsc_t font_dsc = {
#else
static lv_font_fmt_txt_dsc_t font_dsc = {
#endif
    .glyph_bitmap = glyph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = NULL,
    .kern_scale = 0,
    .cmap_num = 2,
    .bpp = 1,
    .kern_classes = 0,
    .bitmap_format = 0,
#if LV_VERSION_CHECK(8, 0, 0)
    .cache = &cache
#endif
};


/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
#if LV_VERSION_CHECK(8, 0, 0)
const lv_font_t matrix_katakana_6 = {
#else
lv_font_t matrix_katakana_6 = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 6,          /*The maximum line height required by the font*/
    .base_line = 0,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -1,
    .underline_thickness = 0,
#endif
    .dsc = &font_dsc           /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
};



#endif /*#if MATRIX_KATAKANA_6*/

