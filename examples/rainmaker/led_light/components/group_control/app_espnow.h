/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_now.h"

#define ESPNOW_DEBUG                     (0)
#define ESPNOW_PAYLOAD_HEAD_LEN          (5)
#define ESPNOW_QUEUE_SIZE                (50)
#define ESPNOW_DEVICE_NAME               "Light"
#define ESPNOW_GROUP_ID                  "group_id"
#define ESPNOW_DISTRIBUTION_NETWORK      "distribution_network"

typedef enum espnow_msg_mode {
    ESPNOW_MSG_MODE_INVALID = 0,
    ESPNOW_MSG_MODE_CONTROL = 1,
    ESPNOW_MSG_MODE_RESET   = 2
} ESPNOW_MSG_MODE;

enum {
    APP_ESPNOW_DATA_BROADCAST,
    APP_ESPNOW_DATA_UNICAST,
    APP_ESPNOW_DATA_MAX,
};

/* User defined field of ESPNOW data in this example. */
typedef struct {
    uint32_t seq;                         //Magic number which is used to determine which device to send unicast ESPNOW data.
    uint8_t mesh_id;                      //Mesh ID of ESPNOW data
    uint8_t payload[0];                   //Real payload of ESPNOW data.
} __attribute__((packed)) app_espnow_data_t;

esp_err_t group_control_init(void);
esp_err_t group_control_deinit(void);
esp_err_t app_espnow_reset_group_control(void);
void esp_now_send_group_control(uint8_t *payload, bool seq_init);
