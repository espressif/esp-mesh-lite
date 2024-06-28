/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include <stdint.h>
#include <esp_err.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Enable ESP Insights in the application
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t app_insights_enable(void);

#ifdef __cplusplus
}
#endif
