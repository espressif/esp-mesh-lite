/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_now.h"
#include "sdkconfig.h"
#include "esp_mesh_lite_espnow.h"

#ifndef zero_prov_H
#define zero_prov_H

#define ZERO_PROV_DEBUG                (0)
#define ZERO_PROV_QUEUE_SIZE           (100)
#define ZERO_PROV_LISTENING_TIMEOUT    CONFIG_ZERO_PROV_LISTENING_TIMEOUT
#define ZERO_PROV_ERR_CHECK(a, str, ret) if(!(a)) { \
        ESP_LOGE(TAG,"%s:%d (%s):%s", __FILE__, __LINE__, __FUNCTION__, str); \
        return (ret); \
    }
#define ZERO_PROV_CHECK_RETURN_VAIL(func_call) \
    do { \
        esp_err_t err = (func_call); \
        if (err == ESP_FAIL) { \
            ESP_LOGW(TAG, "%s %d %s failed with error: %d", __func__, __LINE__, #func_call, err); \
        } \
    } while (0)

typedef enum {
    ZERO_PROV_SEND_BROADCAST,
    ZERO_PROV_RECIVE_DATA,
    ZERO_PROV_SEND_UNICAST_DATA,
} zero_prov_event_id_t;

enum {
    ESPNOW_DATA_BROADCAST,
    ESPNOW_DATA_UNICAST_INFO,
    ESPNOW_DATA_UNICAST_CONFIRM,
    ESPNOW_DATA_UNICAST_ACK,
    ESPNOW_DATA_MAX,
};

typedef union {
    espnow_send_cb_t send_cb;
    espnow_recv_cb_t recv_cb;
} zero_prov_event_info_t;

typedef struct {
    zero_prov_event_id_t id;
    zero_prov_event_info_t info;
} zero_prov_event_t;

typedef struct {
    uint8_t router_ssid[32];
    uint8_t router_password[32];
    uint8_t channel;
    uint8_t mesh_id;
    uint32_t random;
    uint8_t softap_ssid[32];
    uint8_t softap_password[32];
    uint8_t mac_addr[ESP_NOW_ETH_ALEN];
} zero_prov_unicast_data_t;

typedef struct {
    char cust_data[CONFIG_CUSTOMER_DATA_LENGTH];
    char device_info[CONFIG_DEVICE_INFO_LENGTH];
} zero_prov_idle_node_data_t;

typedef struct {
    uint8_t type;
    uint8_t len;
    uint16_t crc;
    uint8_t payload[0];
} zero_prov_esp_now_data_t;

/**
 * @brief Initialize the Zero Provisioning module with customer-specific data and device information.
 *
 * This function initializes the Zero Provisioning module with the provided customer-specific data
 * and device information.
 *
 * @param[in] cust_data Pointer to the customer-specific data buffer.
 * @param[in] device_info Pointer to the device information data buffer.
 * @return
 *     - ESP_OK if initialization was successful.
 *     - ESP_FAIL if initialization failed, for example due to invalid parameters.
 */
esp_err_t zero_prov_init(char *cust_data, char *device_info);

/**
 * @brief Start the device in listening mode for Zero Provisioning to allow unconfigured devices
 *        to join the network.
 *
 * This function puts the device into listening mode, allowing unconfigured devices to discover and
 * join the network. The device will listen for a specified timeout period. This function should
 * only be called after the first provisioning attempt has been completed and the listening mode
 * has been stopped.
 *
 * @param[in] timeout_s Timeout period in seconds, specifying how long the device will remain
 *                     in listening mode before timing out.
 * @note
 *     - The device will listen for the entire duration of the specified timeout.
 *     - This function should only be called after the previous provisioning attempt has been
 *       completed and listening mode has been stopped.
 */
void zero_prov_listening(uint64_t timeout_s);

/**
 * @brief Stop the Zero Provisioning listening service.
 *
 * This function stops the Zero Provisioning module from listening for incoming provisioning requests.
 * It can be called to terminate the provisioning process once it is no longer needed or after a successful
 * provisioning has been completed.
 */
void zero_prov_listening_stop(void);

#endif
