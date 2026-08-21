/*
 * Copyright (c) 2026
 *
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>

#include <lvgl.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>

#include <zmk/display.h>
#include <zmk/event_manager.h>
#include <zmk/events/battery_state_changed.h>

#if IS_ENABLED(CONFIG_USB_DEVICE_STACK)
#include <zmk/events/usb_conn_state_changed.h>
#include <zmk/usb.h>
#endif

#include "battery_alert.h"

#if !DT_HAS_CHOSEN(zmk_battery)
#error "battery_alert necesita un nodo chosen zmk,battery"
#endif

#if !defined(CONFIG_LV_FONT_MONTSERRAT_12)
#error "battery_alert necesita CONFIG_LV_FONT_MONTSERRAT_12=y para el glifo de warning"
#endif

/*
 * Por que no se usa el widget de bateria de ZMK
 * ---------------------------------------------
 * ZMK convierte mV a porcentaje con una recta entre 3450 mV = 0% y 4200 mV =
 * 100% (lithium_ion_mv_to_pct en battery_common.c). Arriba coincide con la
 * realidad, pero la curva de descarga de una LiPo tiene una meseta larga entre
 * 3.7 y 4.0 V donde se va casi toda la capacidad, asi que abajo la recta miente
 * a favor: 3.70 V son ~12% reales y la recta reporta 34%.
 *
 * Que se puede saber con el cable conectado
 * -----------------------------------------
 * El cargador de la nice_nano_v2 es lineal, asi que su salida es la terminal de
 * la bateria y VDDH sigue midiendo algo real, no basura. Pero cambia por fase:
 *
 *   - CC (bateria bajo ~4.1 V): se lee V_bateria + I_carga * R_interna. Con
 *     ~100 mA y ~200 mOhm son ~+20 mV, o sea un 2% optimista. Sirve.
 *   - CV (el tramo final): el cargador clava el nodo en 4.2 V y deja caer la
 *     corriente. La bateria sube de ~80% a 100% sin que el voltaje se mueva, asi
 *     que la lectura se queda pegada arriba durante los ultimos 30-60 min.
 *
 * O sea que detectar el 100% real por voltaje es imposible, y el pin STAT del
 * cargador no esta ruteado a ningun GPIO del nRF52840 (solo maneja el LED de la
 * placa), asi que el firmware tampoco puede preguntarle al cargador.
 *
 * Lo que si se puede, y es lo que de verdad hace falta: avisar cuando llego a la
 * meseta de 4.2 V. Ahi la celda ya trae >=80%, que para un teclado son semanas,
 * y es el momento de desconectarla y cargar la otra mitad. De ahi el estado
 * LISTO en vez de fingir un 100%.
 *
 * No hace falta forkear ZMK para arreglar la curva. app/src/battery.c hace
 * sensor_sample_fetch_chan() y justo despues levanta zmk_battery_state_changed,
 * y el driver deja los mV crudos disponibles en el mismo sample. O sea que
 * desde el listener de ese evento podemos hacer sensor_channel_get() con
 * SENSOR_CHAN_GAUGE_VOLTAGE y quedarnos con el voltaje de la muestra que ZMK
 * acaba de tomar, sin disparar otra lectura del ADC, y aplicarle nuestra curva.
 */
static const struct device *const battery_dev = DEVICE_DT_GET(DT_CHOSEN(zmk_battery));

/* Alerta al bajar de ON, se apaga al volver a subir de OFF. La histeresis evita
 * que el aviso entre y salga en cada refresco cuando la carga queda justo en el
 * umbral, y hace falta que sea ancha: aqui abajo la curva esta en picada, asi
 * que 15% y 18% quedarian a 12 mV de distancia, dentro del ruido del ADC. Con
 * 25% la separacion son ~40 mV. No cuesta nada tener el OFF tan arriba porque
 * una bateria descargandose nunca sube: en la practica el aviso solo se apaga
 * despues de cargarla, y para entonces anda muy por encima del 25%. */
#define BATTERY_ALERT_ON_PCT 15
#define BATTERY_ALERT_OFF_PCT 25

/* Por debajo de esto no hay bateria puesta, o el driver todavia no ha tomado su
 * primera muestra y `millivolts` sigue en 0. Sin este piso, el widget alertaria
 * 0% durante el arranque. */
#define BATTERY_MV_VALID_MIN 2500

#define BATTERY_BLINK_MS 500

/* Meseta de la fase CV. Por encima de esto el voltaje ya no dice nada mas, asi
 * que en vez de un porcentaje que miente se muestra LISTO. Los ~20 mV de
 * inflado de la fase CC no estorban: 4150 mV ya esta bien dentro de la meseta. */
#define BATTERY_CHARGE_DONE_MV 4150

/* Curva de descarga de una LiPo 1S a corriente baja (un teclado consume ~0.02C,
 * asi que el voltaje medido queda cerca del de reposo y la tabla aplica bien).
 * Interpolado linealmente entre puntos. */
static const struct {
    uint16_t mv;
    uint8_t pct;
} lipo_curve[] = {
    {4200, 100}, {4150, 95}, {4110, 90}, {4080, 85}, {4020, 80}, {3980, 75}, {3950, 70},
    {3910, 65},  {3870, 60}, {3850, 55}, {3840, 50}, {3820, 45}, {3800, 40}, {3790, 35},
    {3770, 30},  {3750, 25}, {3730, 20}, {3710, 15}, {3690, 10}, {3610, 5},  {3270, 0},
};

static uint8_t lipo_mv_to_pct(uint16_t mv) {
    if (mv >= lipo_curve[0].mv) {
        return 100;
    }

    for (size_t i = 1; i < ARRAY_SIZE(lipo_curve); i++) {
        if (mv >= lipo_curve[i].mv) {
            const int32_t d_mv = lipo_curve[i - 1].mv - lipo_curve[i].mv;
            const int32_t d_pct = lipo_curve[i - 1].pct - lipo_curve[i].pct;

            return lipo_curve[i].pct + ((mv - lipo_curve[i].mv) * d_pct) / d_mv;
        }
    }

    return 0;
}

struct battery_alert_state {
    uint16_t mv;
    uint8_t pct;
    bool valid;
#if IS_ENABLED(CONFIG_USB_DEVICE_STACK)
    bool usb_present;
#endif
};

static lv_obj_t *alert_label;
static lv_timer_t *blink_timer;
static bool alert_active;

static void battery_alert_blink_cb(lv_timer_t *timer) {
    ARG_UNUSED(timer);

    if (!alert_active) {
        return;
    }

    if (lv_obj_has_flag(alert_label, LV_OBJ_FLAG_HIDDEN)) {
        lv_obj_clear_flag(alert_label, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_add_flag(alert_label, LV_OBJ_FLAG_HIDDEN);
    }
}

static void battery_alert_update_cb(struct battery_alert_state state) {
    if (!state.valid) {
        alert_active = false;
        lv_obj_add_flag(alert_label, LV_OBJ_FLAG_HIDDEN);
        return;
    }

#if IS_ENABLED(CONFIG_USB_DEVICE_STACK)
    if (state.usb_present) {
        /* Cargando. Sin parpadeo: esto es informativo, no una alerta. Se muestra
         * el avance a proposito, porque la pregunta al cargar es "cuanto falta
         * para poder pasar a la otra mitad" y para eso hay que ver el numero
         * subir. No estorba en el uso normal porque con BLANK_ON_IDLE la
         * pantalla se apaga a los 30 s de no teclear. */
        alert_active = false;

        if (state.mv >= BATTERY_CHARGE_DONE_MV) {
            lv_label_set_text(alert_label, LV_SYMBOL_CHARGE " LISTO");
        } else {
            char text[16];

            snprintf(text, sizeof(text), LV_SYMBOL_CHARGE " %u%%", state.pct);
            lv_label_set_text(alert_label, text);
        }

        lv_obj_clear_flag(alert_label, LV_OBJ_FLAG_HIDDEN);
        return;
    }
#endif

    const bool low =
        alert_active ? state.pct < BATTERY_ALERT_OFF_PCT : state.pct < BATTERY_ALERT_ON_PCT;

    if (!low) {
        alert_active = false;
        lv_obj_add_flag(alert_label, LV_OBJ_FLAG_HIDDEN);
        return;
    }

    char text[16];

    snprintf(text, sizeof(text), LV_SYMBOL_WARNING " %u%%", state.pct);
    lv_label_set_text(alert_label, text);

    if (!alert_active) {
        alert_active = true;
        /* Que el primer parpadeo dure lo mismo que los demas. */
        lv_timer_reset(blink_timer);
    }

    lv_obj_clear_flag(alert_label, LV_OBJ_FLAG_HIDDEN);
}

static struct battery_alert_state battery_alert_get_state(const zmk_event_t *eh) {
    ARG_UNUSED(eh);

    struct battery_alert_state state = {
#if IS_ENABLED(CONFIG_USB_DEVICE_STACK)
        .usb_present = zmk_usb_is_powered(),
#endif
    };

    struct sensor_value voltage;

    if (!device_is_ready(battery_dev) ||
        sensor_channel_get(battery_dev, SENSOR_CHAN_GAUGE_VOLTAGE, &voltage) != 0) {
        return state;
    }

    const int32_t mv = voltage.val1 * 1000 + voltage.val2 / 1000;

    if (mv < BATTERY_MV_VALID_MIN) {
        return state;
    }

    state.valid = true;
    state.mv = (uint16_t)mv;
    state.pct = lipo_mv_to_pct((uint16_t)mv);

    return state;
}

/* El listener se llama battery_alert_widget y no battery_alert porque la macro
 * genera un <nombre>_init() que choparia con battery_alert_init(). */
ZMK_DISPLAY_WIDGET_LISTENER(battery_alert_widget, struct battery_alert_state,
                            battery_alert_update_cb, battery_alert_get_state)

ZMK_SUBSCRIPTION(battery_alert_widget, zmk_battery_state_changed);
#if IS_ENABLED(CONFIG_USB_DEVICE_STACK)
ZMK_SUBSCRIPTION(battery_alert_widget, zmk_usb_conn_state_changed);
#endif

void battery_alert_init(lv_obj_t *parent) {
    alert_label = lv_label_create(parent);

    /* El font default del build es unscii_8, que solo trae ASCII 32-127 y por
     * eso no puede dibujar LV_SYMBOL_WARNING (ni ningun otro glifo
     * FontAwesome). Montserrat 12 si los trae. */
    lv_obj_set_style_text_font(alert_label, &lv_font_montserrat_12, LV_PART_MAIN);

    /* Mismo rincon donde estaba el porcentaje antes: con la pantalla en
     * vertical, el borde derecho de LVGL es el borde de arriba fisico. */
    lv_obj_align(alert_label, LV_ALIGN_TOP_RIGHT, 0, 0);

    /* Fondo opaco para que la lluvia no se le encime. Se copia el color del
     * padre en vez de fijar negro, por lo mismo que el resto de la pantalla:
     * asi da igual como quede la polaridad. */
    lv_obj_set_style_bg_color(alert_label, lv_obj_get_style_bg_color(parent, LV_PART_MAIN),
                              LV_PART_MAIN);
    lv_obj_set_style_bg_opa(alert_label, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_pad_all(alert_label, 1, LV_PART_MAIN);

    lv_obj_add_flag(alert_label, LV_OBJ_FLAG_HIDDEN);

    blink_timer = lv_timer_create(battery_alert_blink_cb, BATTERY_BLINK_MS, NULL);

    battery_alert_widget_init();
}
