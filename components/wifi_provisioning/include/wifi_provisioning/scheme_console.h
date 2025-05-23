/*
 * SPDX-FileCopyrightText: 2019-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <protocomm.h>
#include <protocomm_console.h>

#include "wifi_provisioning/manager.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Scheme that can be used by manager for provisioning
 *          over console (Serial UART)
 */
extern const wifi_prov_scheme_t wifi_prov_scheme_console;

#ifdef __cplusplus
}
#endif
