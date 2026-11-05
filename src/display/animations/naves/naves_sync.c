/*
 * Copyright (c) 2026
 *
 * SPDX-License-Identifier: MIT
 */

/*
 * Canal entre mitades para la animacion "naves".
 *
 * ZMK no tiene una via para mandar datos arbitrarios de la central a la
 * periferica, pero si puede ejecutar un behavior en la periferica (asi
 * funcionan &rgb_ug o &ext_power en las dos mitades). Ese mensaje lleva el
 * nombre del behavior mas param1 y param2 (32 bits cada uno). Tambien lleva
 * `position`, pero en el protocolo de split es de 8 bits (uint8_t en
 * zmk/split/bluetooth/service.h), asi que no sirve para datos.
 *
 * Este archivo define un behavior que no hace nada en el keymap y solo sirve
 * de buzon: la central lo invoca en la periferica con
 *
 *   param1 = seed de la oleada (16 bits altos) | edad de la oleada (16 bajos)
 *   param2 = paso objetivo
 *
 * La edad es objetivo - paso en que arranco la oleada. Una oleada dura a lo
 * mucho un par de miles de pasos (la formacion siempre acaba bajando hasta la
 * nave), muy lejos de 65535.
 *
 * y en la periferica el driver se lo pasa a naves.c. El nodo vive en
 * bloque behaviors de config/corne.keymap.
 */

#define DT_DRV_COMPAT zmk_behavior_anim_sync

#include <zephyr/device.h>
#include <zephyr/kernel.h>

#include <drivers/behavior.h>
#include <zmk/behavior.h>

#include "naves_core.h"
#include "naves_sync.h"

#if DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)

static int anim_sync_pressed(struct zmk_behavior_binding *binding,
                             struct zmk_behavior_binding_event event) {
#if !NAVES_LEADER
    uint32_t target = binding->param2;
    uint32_t age = binding->param1 & 0xFFFFU;

    ARG_UNUSED(event);
    naves_sync_receive(binding->param1 >> 16, target - age, target);
#else
    ARG_UNUSED(binding);
    ARG_UNUSED(event);
#endif

    return ZMK_BEHAVIOR_OPAQUE;
}

static int anim_sync_released(struct zmk_behavior_binding *binding,
                              struct zmk_behavior_binding_event event) {
    ARG_UNUSED(binding);
    ARG_UNUSED(event);

    return ZMK_BEHAVIOR_OPAQUE;
}

static const struct behavior_driver_api anim_sync_driver_api = {
    .binding_pressed = anim_sync_pressed,
    .binding_released = anim_sync_released,
#if IS_ENABLED(CONFIG_ZMK_BEHAVIOR_METADATA)
    .get_parameter_metadata = zmk_behavior_get_empty_param_metadata,
#endif
};

BEHAVIOR_DT_INST_DEFINE(0, NULL, NULL, NULL, NULL, POST_KERNEL,
                        CONFIG_KERNEL_INIT_PRIORITY_DEFAULT, &anim_sync_driver_api);

#elif IS_ENABLED(CONFIG_ZMK_SPLIT)
#error "La animacion naves necesita el nodo animsync (ver behaviors en config/corne.keymap)"
#endif

#if NAVES_LEADER && IS_ENABLED(CONFIG_ZMK_SPLIT_BLE) && DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)

#include <zmk/split/bluetooth/central.h>
#include <zmk/split/bluetooth/service.h>

#define ANIM_SYNC_NAME DEVICE_DT_NAME(DT_DRV_INST(0))

BUILD_ASSERT(sizeof(ANIM_SYNC_NAME) <= ZMK_SPLIT_RUN_BEHAVIOR_DEV_LEN,
             "El nombre del nodo animsync no cabe en el mensaje de split (max 8 letras)");

void naves_sync_send(uint32_t wave_seed, uint32_t wave_start, uint32_t target) {
    struct zmk_behavior_binding binding = {
        .behavior_dev = ANIM_SYNC_NAME,
        .param1 = ((wave_seed & NAVES_SEED_MASK) << 16) | ((target - wave_start) & 0xFFFFU),
        .param2 = target,
    };
    struct zmk_behavior_binding_event event = {
        .position = 0,
        .timestamp = k_uptime_get(),
    };

    /* Indice 0 = la unica periferica. Si no esta conectada ZMK lo descarta;
     * el latido de naves.c la pone al dia cuando se conecte. */
    zmk_split_bt_invoke_behavior(0, &binding, event, true);
}

#elif NAVES_LEADER

void naves_sync_send(uint32_t wave_seed, uint32_t wave_start, uint32_t target) {
    ARG_UNUSED(wave_seed);
    ARG_UNUSED(wave_start);
    ARG_UNUSED(target);
}

#endif
