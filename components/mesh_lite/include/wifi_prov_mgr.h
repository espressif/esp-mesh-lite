/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

/**
 * @brief   Enable Wi-Fi Provisioning
 *
 */
esp_err_t esp_mesh_lite_wifi_prov_mgr_init(void);

/**
 * @brief   Disable Wi-Fi Provisioning
 *
 */
esp_err_t esp_mesh_lite_wifi_prov_mgr_deinit(void);

/**
 * @brief   Get Wi-Fi Provisioning Status
 *
 * @return
 *        - true: Wi-Fi Provisioning still in progress
 *        - false:End of Wi-Fi Provisioning
 */
bool wifi_provision_in_progress(void);

/**
 * @brief   Stop Wi-Fi Provisioning
 *
 * This function stops the Wi-Fi provisioning process if it is in progress.
 */
void wifi_provision_stop(void);
