/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "cJSON.h"
#include "esp_wifi_types.h"
#include "esp_netif_ip_addr.h"
#include "esp_mesh_lite_port.h"

extern const char* ESP_MESH_LITE_EVENT;

#define MAC_MAX_LEN                   (18)
#define IP_MAX_LEN                    (16)
#define DEVICE_CATEGORY               (32)

/* Definitions for error constants. */
#define ESP_ERR_DUPLICATE_ADDITION    0x110   /*!< Duplicate addition */

#ifdef CONFIG_MESH_LITE_MAXIMUM_NODE_NUMBER
#define MESH_LITE_MAXIMUM_NODE_NUMBER CONFIG_MESH_LITE_MAXIMUM_NODE_NUMBER
#else
#define MESH_LITE_MAXIMUM_NODE_NUMBER 0
#endif

#ifdef CONFIG_JOIN_MESH_IGNORE_ROUTER_STATUS
#define JOIN_MESH_IGNORE_ROUTER_STATUS CONFIG_JOIN_MESH_IGNORE_ROUTER_STATUS
#else
#define JOIN_MESH_IGNORE_ROUTER_STATUS 0
#endif

#ifdef CONFIG_JOIN_MESH_WITHOUT_CONFIGURED_WIFI_INFO
#define JOIN_MESH_WITHOUT_CONFIGURED_WIFI_INFO CONFIG_JOIN_MESH_WITHOUT_CONFIGURED_WIFI_INFO
#else
#define JOIN_MESH_WITHOUT_CONFIGURED_WIFI_INFO 0
#endif

#ifdef CONFIG_LEAF_NODE
#define LEAF_NODE CONFIG_LEAF_NODE
#else
#define LEAF_NODE 0
#endif

#ifdef CONFIG_OTA_DATA_LEN
#define OTA_DATA_LEN CONFIG_OTA_DATA_LEN
#else
#define OTA_DATA_LEN 0
#endif

#ifdef CONFIG_OTA_WND_DEFAULT
#define OTA_WND_DEFAULT CONFIG_OTA_WND_DEFAULT
#else
#define OTA_WND_DEFAULT 0
#endif

/**
 * @brief Enumeration representing different networking modes for ESP-Mesh-Lite.
 */
typedef enum {
    ESP_MESH_LITE_ROUTER,
    ESP_MESH_LITE_MESH,
} esp_mesh_lite_networking_mode_t;

#define ESP_MESH_LITE_DEFAULT_INIT() { \
    .vendor_id = {CONFIG_MESH_LITE_VENDOR_ID_0, CONFIG_MESH_LITE_VENDOR_ID_1}, \
    .mesh_id = CONFIG_MESH_LITE_ID, \
    .max_connect_number = CONFIG_BRIDGE_SOFTAP_MAX_CONNECT_NUMBER, \
    .max_router_number = CONFIG_MESH_LITE_MAX_ROUTER_NUMBER, \
    .max_level = CONFIG_MESH_LITE_MAXIMUM_LEVEL_ALLOWED, \
    .max_node_number = MESH_LITE_MAXIMUM_NODE_NUMBER, \
    .join_mesh_ignore_router_status = JOIN_MESH_IGNORE_ROUTER_STATUS, \
    .join_mesh_without_configured_wifi = JOIN_MESH_WITHOUT_CONFIGURED_WIFI_INFO, \
    .leaf_node = LEAF_NODE, \
    .ota_data_len = OTA_DATA_LEN, \
    .ota_wnd = OTA_WND_DEFAULT, \
    .softap_ssid = CONFIG_BRIDGE_SOFTAP_SSID, \
    .softap_password = CONFIG_BRIDGE_SOFTAP_PASSWORD, \
    .device_category = CONFIG_DEVICE_CATEGORY \
}

/**
 * @brief Mesh-Lite event declarations
 *
 */
typedef enum {
    ESP_MESH_LITE_EVENT_CORE_STARTED,
    ESP_MESH_LITE_EVENT_CORE_INHERITED_NET_SEGMENT_CHANGED,
    ESP_MESH_LITE_EVENT_CORE_ROUTER_INFO_CHANGED,
    ESP_MESH_LITE_EVENT_CORE_MAX,
} esp_mesh_lite_event_core_t;

typedef enum {
    ESP_MESH_LITE_EVENT_OTA_START = ESP_MESH_LITE_EVENT_CORE_MAX,
    ESP_MESH_LITE_EVENT_OTA_FINISH,
    ESP_MESH_LITE_EVENT_OTA_PROGRESS,
    ESP_MESH_LITE_EVENT_OTA_MAX,
} esp_mesh_lite_event_ota_t;

/**
 * @brief Enumeration for the soft AP status of ESP-Mesh-Lite leaf nodes.
 *
 * Defines two possible states: DISABLE_SOFTAP and ENABLE_SOFTAP.
 */
typedef enum {
    DISABLE_SOFTAP,
    ENABLE_SOFTAP,
} esp_mesh_lite_leaf_node_softap_status_t;

/**
 * @brief Mesh-Lite configuration parameters passed to esp_mesh_lite_core_init call.
 */
typedef struct {
    uint8_t vendor_id[2];                   /**< The VID_1, VID_2 of Mesh-Lite */
    uint8_t mesh_id;                        /**< The Mesh_ID of Mesh-Lite */
    uint8_t max_connect_number;             /**< Max number of stations allowed to connect in */
    uint8_t max_router_number;              /**< Maximum number of trace router number */
    uint8_t max_level;                      /**< The maximum level allowed of Mesh-Lite */
    uint8_t max_node_number;                /**< The maximum node number of Mesh-Lite */
    bool join_mesh_ignore_router_status;    /**< Join Mesh no matter whether the node is connected to router */
    bool join_mesh_without_configured_wifi; /**< Join Mesh without configured with information */
    bool leaf_node;                         /**< Whether it is a leaf node */
    uint32_t ota_data_len;                  /**< The maximum length of an OTA data transmission */
    uint16_t ota_wnd;                       /**< OTA data transfer window size */
    const char* softap_ssid;                /**< SoftAP SSID */
    const char* softap_password;            /**< SoftAP Password */
    const char* device_category;            /**< Device Category */
} esp_mesh_lite_config_t;

// Define a structure for mesh lite fusion configuration settings.
typedef struct {
    int8_t fusion_rssi_threshold;   // During fusion, the device to be fused will scan the primary fusion device. If the signal strength of the primary fusion device is lower than this threshold, the fusion process will be aborted. By default, this value is set to -85.
    uint32_t fusion_start_time_sec; // The start time in seconds after getting ip when fusion will begin, default is 0 seconds.
    uint32_t fusion_frequency_sec;  // The frequency in seconds at which fusion will occur after the start time, default is 600 seconds.
} esp_mesh_lite_fusion_config_t;

typedef esp_err_t (*extern_url_ota_cb_t)(void);
typedef cJSON* (*msg_process_cb_t)(cJSON *payload, uint32_t seq);

typedef enum {
    ESP_MESH_LITE_EVENT_OTA_SUCCESS = 0,
    ESP_MESH_LITE_EVENT_OTA_FAIL,
    ESP_MESH_LITE_EVENT_OTA_REJECTED,
    ESP_MESH_LITE_EVENT_OTA_WRITE_ERR,
    ESP_MESH_LITE_EVENT_OTA_GET_PARTITION_ERR,
    ESP_MESH_LITE_EVENT_OTA_SET_BOOT_PARTITION_ERR,
} ota_finish_reason_t;

typedef struct {
    ota_finish_reason_t reason;
} mesh_lite_event_ota_finish_t;

typedef struct {
    uint8_t percentage;
} mesh_lite_event_ota_progress_t;

/**
 * @brief Structure representing an RSSI threshold list for ESP-Mesh-Lite.
 */
typedef struct {
    int8_t rssi_min;         /**< The lower level of the RSSI threshold range. */
    int8_t rssi_max;         /**< The upper level of the RSSI threshold range. */
    uint8_t rssi_threshold;  /**< RSSI threshold level. */
} esp_mesh_lite_rssi_threshold_list_t;

/**
 * @brief Mesh-Lite message action parameters passed to esp_mesh_lite_msg_action_list_register call.
 */
typedef struct esp_mesh_lite_msg_action {
    const char* type;         /**< The type of message sent */
    const char* rsp_type;     /**< The message type expected to be received*/
                              /**< When a message of the expected type is received, stop retransmitting*/
                              /**< If set to NULL, it will be sent until the maximum number of retransmissions is reached*/
    msg_process_cb_t process; /**< The callback function when receiving the 'type' message, The cjson information in the type message can be processed in this cb*/
} esp_mesh_lite_msg_action_t;

/**
 * @brief Define the type for the callback function.
 */
typedef const uint8_t*(*esp_mesh_lite_get_ssid_by_mac_cb_t)(const uint8_t *bssid);

/*****************************************************/
/**************** ESP Wi-Fi Mesh Lite ****************/
/*****************************************************/

/**
 * @brief Check if the network segment is used to avoid conflicts.
 *
 * @return
 *     - true :be used
 *     - false:not used
 */
bool esp_mesh_lite_network_segment_is_used(uint32_t ip);

/**
 * @brief Initialization Mesh-Lite.
 * 
 * @return
 *     - OK   : successful
 *     - Other: fail
 */
esp_err_t esp_mesh_lite_core_init(esp_mesh_lite_config_t* config);

/**
 * @brief Scan to find a matched node and connect.
 * 
 */
void esp_mesh_lite_connect(void);

/**
 * @brief Set the mesh_lite_id
 *
 * @param[in] mesh_id: Each mesh network should have a different and unique ID.
 * 
 * @param[in] force_update_nvs: Whether to force update the value of mesh_id in nvs.
 * 
 */
void esp_mesh_lite_set_mesh_id(uint8_t mesh_id, bool force_update_nvs);

/**
 * @brief Set the specific level to which this node is restricted.
 * 
 * @attention  1. Please invoke this API after `esp_mesh_lite_init`.
 * @attention  2. Please invoke this API before `esp_mesh_lite_start`.
 *
 * @param[in]  level: An integer from 1 to CONFIG_MESH_LITE_MAXIMUM_LEVEL_ALLOWED. Setting it to 0 will unset the restriction.
 * 
 */
esp_err_t esp_mesh_lite_set_allowed_level(uint8_t level);

/**
 * @brief Set the level that this node is not allowed to use.
 * 
 * @attention  1. Please invoke this API after `esp_mesh_lite_init`.
 * @attention  2. Please invoke this API before `esp_mesh_lite_start`.
 *
 * @param[in]  level: An integer from 1 to CONFIG_MESH_LITE_MAXIMUM_LEVEL_ALLOWED. Setting it to 0 is invalid.
 *
 */
esp_err_t esp_mesh_lite_set_disallowed_level(uint8_t level);

/**
 * @brief  Set router information
 * 
 * @attention  1. Please call this API after  `esp_mesh_lite_init`
 * @attention  2. Please call this API before `esp_mesh_lite_start`
 *
 * @param[in]  conf
 *
 */
esp_err_t esp_mesh_lite_set_router_config(mesh_lite_sta_config_t *conf);

/**
 * @brief  Whether to allow other nodes to join the mesh network.
 * 
 * @attention  1. Please call this API after  `esp_mesh_lite_init`
 * @attention  2. Please call this API before `esp_mesh_lite_start`
 *
 * @param[in]  enable: true -> allow; false -> disallow
 *
 */
esp_err_t esp_mesh_lite_allow_others_to_join(bool enable);

/**
 * @brief  Set argot number.
 * 
 * @attention  1. Please call this API after  `esp_mesh_lite_init`
 * @attention  2. Please call this API before `esp_mesh_lite_start`
 *
 * @param[in]  argot
 *
 */
esp_err_t esp_mesh_lite_set_argot(uint32_t argot);

/**
 * @brief Set the RSSI threshold for ESP-Mesh-Lite.
 *
 * This function sets the RSSI threshold list for ESP-Mesh-Lite. The list is an array of
 * esp_mesh_lite_rssi_threshold_list_t structures, where each structure defines an RSSI range
 * and the corresponding threshold value. The size parameter indicates the number of elements
 * in the array.
 *
 * Usage Example:
 * @code{.c}
 * static const esp_mesh_lite_rssi_threshold_list_t rssi_level[] = {
 *     {0,  -35, 20},
 *     {-35,  -55, 15},
 *     {-55,  -75, 10},
 *     {-75,  -125, 5},
 * };
 *
 * esp_err_t result = esp_mesh_lite_set_rssi_threshold(rssi_level, sizeof(rssi_level) / sizeof(esp_mesh_lite_rssi_threshold_list_t), 0);
 * if (result == ESP_OK) {
 *     // RSSI threshold list set successfully
 * } else {
 *     // Failed to set the RSSI threshold list
 * }
 * @endcode
 *
 * @param[in] rssi_threshold_list An array of RSSI thresholds to be applied.
 * @param[in] size Number of elements in the rssi_threshold_list array.
 * @param[in] min_rssi Minimum RSSI threshold value for parent node signal strength. Set to 0 to use the default value (-90).
 * @return
 *      - ESP_OK: Success
 *      - ESP_FAIL: Failed to set the RSSI threshold list
 */
esp_err_t esp_mesh_lite_set_rssi_threshold(esp_mesh_lite_rssi_threshold_list_t rssi_threshold_list[], size_t size, int8_t min_rssi);

/**
 * @brief  Set SoftAP information.
 *
 * @param[in]  softap_ssid
 * @param[in]  softap_password
 *
 */
esp_err_t esp_mesh_lite_set_softap_info(const char* softap_ssid, const char* softap_password);

/**
 * @brief Set the networking mode for ESP-Mesh-Lite.
 *
 * This function sets the networking mode for ESP-Mesh-Lite. The networking mode can be either
 * Router mode or Mesh mode.
 *
 * @param mode           Networking mode to set (either ESP_MESH_LITE_ROUTER or ESP_MESH_LITE_MESH).
 * @param rssi_threshold RSSI threshold for router mode. Only effective when mode is ESP_MESH_LITE_ROUTER.
 *                       If the router's signal strength is below this threshold, the device will connect to a mesh node.
 * @return esp_err_t ESP_OK on success, or an error code indicating the reason for failure.
 */
esp_err_t esp_mesh_lite_set_networking_mode(esp_mesh_lite_networking_mode_t mode, int8_t rssi_threshold);

/**
 * @brief  Set Node as leaf node.
 *
 * @param[in]  enable: true -> Leaf Node; false -> Regular Node
 *
 */
esp_err_t esp_mesh_lite_set_leaf_node(bool enable);

/**
 * @brief Set the soft AP status for ESP-Mesh-Lite leaf nodes.
 *
 * This function allows the configuration of the soft AP status for ESP-Mesh-Lite leaf nodes.
 *
 * @param status Soft AP status to set (either DISABLE_SOFTAP or ENABLE_SOFTAP).
 */
void esp_mesh_lite_set_leaf_node_softap_status(esp_mesh_lite_leaf_node_softap_status_t status);

/**
 * @brief Set the mesh lite fusion configuration.
 *
 * This API is used to configure the timing parameters for mesh lite fusion process.
 *
 * Usage Example:
 * @code{.c}
 * // Initialize fusion configuration with specified values.
 * esp_mesh_lite_fusion_config_t fusion_config = {
 *     .fusion_start_time_sec = 60,   // Set the fusion to start 60 seconds after the mesh network is up.
 *     .fusion_frequency_sec = 120,   // Set the network to perform fusion every 120 seconds thereafter.
 * };
 *
 * // Apply the fusion configuration to the mesh network.
 * esp_mesh_lite_set_fusion_config(&fusion_config);
 * @endcode
 *
 * @param[in] config A pointer to the `esp_mesh_lite_fusion_config_t` struct that holds the fusion configuration.
 * @return
 *      - ESP_OK: Success
 *      - ESP_FAIL: Failed to set the fusion config
 */
esp_err_t esp_mesh_lite_set_fusion_config(esp_mesh_lite_fusion_config_t *config);

/**
 * @brief  Get the mesh_lite_id
 * 
 * @return
 *      - mesh_lite_id
 */
uint8_t esp_mesh_lite_get_mesh_id(void);

/**
 * @brief  Get the node level
 *
 * @return
 *      - level
 */
uint8_t esp_mesh_lite_get_level(void);

/**
 * @brief  Erase rtc_memory store information
 *
 * @attention  Must be called before node factory reset.
 *
 */
void esp_mesh_lite_erase_rtc_store(void);

/**
 * @brief  Get the argot number
 *
 * @return
 *      - argot
 */
uint32_t esp_mesh_lite_get_argot(void);

/**
 * @brief  Get the allowed level
 *
 * @return
 *      - allowed_level
 */
uint8_t esp_mesh_lite_get_allowed_level(void);

/**
 * @brief  Get the disallowed level
 *
 * @return
 *      - disallowed_level
 */
uint8_t esp_mesh_lite_get_disallowed_level(void);

/**
 * @brief Get the current networking mode of ESP-Mesh-Lite.
 *
 * This function retrieves the current networking mode of ESP-Mesh-Lite.
 *
 * @param[out] mode Pointer to a variable where the current networking mode will be stored.
 * @return esp_err_t ESP_OK on success, or an error code indicating the reason for failure.
 */
esp_err_t esp_mesh_lite_get_networking_mode(esp_mesh_lite_networking_mode_t *mode);

/**
 * @brief  Check if Node is a Leaf Node
 *
 */
bool esp_mesh_lite_is_leaf_node(void);

/**
 * @brief  Get the device category
 *
 * @return
 *      - device category
 */
char *esp_mesh_lite_get_device_category(void);

/**
 * @brief  Get the Root SoftAP GW ip
 *
 * @param[in]  type
 *             IPADDR_TYPE_V4
 *             IPADDR_TYPE_V6
 * @param[in]  ip_addr
 *
 * @return
 *      - Root SoftAP GW ip
 */
esp_err_t esp_mesh_lite_get_root_ip(uint8_t type, esp_ip_addr_t *ip_addr);

/**
 * @brief  Get router config information
 *
 * @param[out] router_config output router config
 *
 * @return
 *    - ESP_OK
 *    - ESP_ERR_INVALID_ARG
 */
esp_err_t esp_mesh_lite_get_router_config(mesh_lite_sta_config_t *router_config);

/**
 * @brief  Scan all available APs.
 *
 * @attention  If you want to scan externally, don't call esp_wifi_scan_start directly, please call this interface.
 *
 * @return
 *    - 0: scan start successful
 */
esp_err_t esp_mesh_lite_wifi_scan_start(const wifi_scan_config_t *config, uint32_t timeout);

/**
 * @brief Register a callback function to check if BSSID is contained.
 *
 * @param[in] callback Pointer to the callback function.
 *
 * @return
 *     - ESP_OK: Registration successful.
 */
esp_err_t esp_mesh_lite_bssid_check_cb_register(esp_mesh_lite_get_ssid_by_mac_cb_t cb);

/*****************************************************/
/************ ESP Wi-Fi Mesh Lite LAN OTA ************/
/*****************************************************/

/**
 * @brief Start Mesh-Lite OTA
 * 
 * @param[in] filesize: The size of the firmware file to be updated.
 * 
 * @param[in] fw_version: The version of the firmware file to be updated.
 * 
 * @param[in] extern_url_ota_cb: The callback function of ota from the external url,
 *                               when the local area network ota cannot be performed,
 *                               the callback function will be called to perform ota.
 * 
 */
esp_err_t esp_mesh_lite_ota_start(int filesize, char *fw_version, extern_url_ota_cb_t extern_url_ota_cb);

/**
 * @brief 
 * 
 * @attention  Called when executing the external OTA process.
 *             When the higher-level node also starts the OTA process,
 *             the current external OTA process will be stopped.
 * 
 * @attention  Please refer to examples/rainmaker/led_light/components/app_rainmaker/app_rainmaker_ota.c
 *
 */
esp_err_t esp_mesh_lite_wait_ota_allow(void);

/**
 * @brief Notify the child node to suspend the OTA process
 *
 */
esp_err_t esp_mesh_lite_ota_notify_child_node_pause(void);

/**
 * @brief Notify the child node to restart the OTA process
 *
 */
esp_err_t esp_mesh_lite_ota_notify_child_node_restart(void);


/*****************************************************/
/********* ESP Wi-Fi Mesh Lite Communication *********/
/*****************************************************/

/**
 * @brief  Send broadcast message to child nodes.
 *
 * @param[in]  payload
 * 
 */
esp_err_t esp_mesh_lite_send_broadcast_msg_to_child(const char* payload);

/**
 * @brief  Send broadcast message to parent node.
 * 
 * @attention For non-root nodes, Please choose `esp_mesh_lite_send_msg_to_parent(const char* payload)`
 *
 * @param[in]  payload
 * 
 */
esp_err_t esp_mesh_lite_send_broadcast_msg_to_parent(const char* payload);

/**
 * @brief  Send message to root node.
 *
 * @param[in]  payload
 * 
 */
esp_err_t esp_mesh_lite_send_msg_to_root(const char* payload);

/**
 * @brief  Send message to parent node.
 *
 * @param[in]  payload
 * 
 */
esp_err_t esp_mesh_lite_send_msg_to_parent(const char* payload);

/**
 * @brief Send a specific type of message and set the number of retransmissions.
 *
 * @param[in] send_msg:    Send message type.
 * @param[in] expect_msg:  The type of message expected to be received,
 *                         if the corresponding message type is received, stop retransmission.
 * @param[in] max_retry:   Maximum number of retransmissions.
 * @param[in] req_payload: Message payload, req_payload will not be free in this API.
 * @param[in] resend:      Send msg function pointer.
 *                         - esp_mesh_lite_send_broadcast_msg_to_child()
 *                         - esp_mesh_lite_send_broadcast_msg_to_parent()
 *                         - esp_mesh_lite_send_msg_to_root()
 *                         - esp_mesh_lite_send_msg_to_parent()
 * 
 */
esp_err_t esp_mesh_lite_try_sending_msg(char* send_msg,
                                       char* expect_msg,
                                       uint32_t max_retry,
                                       cJSON* req_payload,
                                       esp_err_t (*resend)(const char* payload));

/**
 * @brief Register custom message reception and recovery logic
 * 
 * @attention  Please refer to components/mesh_lite/src/esp_mesh_lite.c
 *
 * @param[in] msg_action
 * 
 */
esp_err_t esp_mesh_lite_msg_action_list_register(const esp_mesh_lite_msg_action_t* msg_action);

/**
 * @brief Register custom message reception and recovery logic
 * 
 * @attention  Please refer to components/mesh_lite/src/esp_mesh_lite.c
 *
 * @param[in] msg_action
 * 
 */
esp_err_t esp_mesh_lite_msg_action_list_unregister(const esp_mesh_lite_msg_action_t* msg_action);

/**
  * @brief This function initialize AES context and set key schedule (encryption or decryption).
  * 
  * @param[in]  key      encryption key
  * @param[in]  keybits  currently only supports 128
  * 
  * @attention this function must be called before Mesh-Lite initialization.
  * 
  * @return
  *     - ESP_OK : successful
  *     - Other  : fail
  */
esp_err_t esp_mesh_lite_aes_set_key(const unsigned char* key, unsigned int keybits);
#ifdef __cplusplus
}
#endif
