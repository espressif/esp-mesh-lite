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
 * @brief Define the type for the callback function.
 */
typedef const uint8_t*(*esp_mesh_lite_get_ssid_by_mac_cb_t)(const uint8_t *bssid);

/**
 * @brief Callback function pointer type for processing received messages.
 *        This function should process received message payloads and return a cJSON pointer.
 *
 * @param payload  cJSON object representing the message payload.
 * @param seq      Sequence number of the message.
 *
 * @return cJSON*  cJSON pointer representing the processed message.
 */
typedef cJSON* (*msg_process_cb_t)(cJSON *payload, uint32_t seq);

/**
 * @brief Callback function pointer type for providing LAN OTA file data.
 *        This function should provide file data for LAN OTA.
 *
 * @param filesize     Size of the entire firmware file.
 * @param fw_version   Firmware version string.
 * @param offset       Offset in the firmware file.
 * @param data_size    Size of the data to be provided.
 * @param data         Pointer to the data to be provided.
 *
 * @return esp_err_t The result of the operation.
 */
typedef esp_err_t (*lan_ota_provide_file_cb_t)(int filesize, char *fw_version, size_t offset, size_t data_size, char *data);

/**
 * @brief Callback function pointer type for getting LAN OTA file data.
 *        This function should get file data for LAN OTA.
 *
 * @param data         Buffer to store the received data.
 * @param data_size    Size of the data buffer.
 *
 * @return esp_err_t The result of the operation.
 */
typedef esp_err_t (*lan_ota_get_file_cb_t)(char *data, size_t data_size);

/**
 * @brief Callback function pointer type for indicating LAN OTA file reception completion.
 *        This function should be called when LAN OTA file reception is complete.
 *
 * @return esp_err_t The result of the operation.
 */
typedef esp_err_t (*lan_ota_get_file_done_cb_t)(void);

/**
 * @brief Callback function pointer type for handling external URL OTA.
 *        This function should return an esp_err_t indicating the result of the OTA operation.
 *
 * @return esp_err_t The result of the OTA operation.
 */
typedef esp_err_t (*extern_url_ota_cb_t)(void);

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

/**
 * @brief Enumeration representing different networking modes for ESP-Mesh-Lite.
 */
typedef enum {
    ESP_MESH_LITE_ROUTER,
    ESP_MESH_LITE_MESH,
} esp_mesh_lite_networking_mode_t;

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
 * @brief Enumeration of reasons for ESP-Mesh-Lite events.
 *        These values indicate various outcomes of ESP-Mesh-Lite operations.
 */
typedef enum {
    ESP_MESH_LITE_EVENT_OTA_START = ESP_MESH_LITE_EVENT_CORE_MAX,
    ESP_MESH_LITE_EVENT_OTA_FINISH,
    ESP_MESH_LITE_EVENT_OTA_PROGRESS,
    ESP_MESH_LITE_EVENT_OTA_MAX,
} esp_mesh_lite_event_ota_t;

/**
 * @brief Enumeration of reasons for OTA (Over-The-Air) update completion events in ESP-Mesh-Lite.
 *        These values indicate various outcomes of OTA update operations.
 */
typedef enum {
    ESP_MESH_LITE_EVENT_OTA_SUCCESS = 0,             /**< OTA update succeeded. */
    ESP_MESH_LITE_EVENT_OTA_FAIL,                    /**< OTA update failed for an unspecified reason. */
    ESP_MESH_LITE_EVENT_OTA_REJECTED,                /**< OTA update rejected by the device. */
    ESP_MESH_LITE_EVENT_OTA_WRITE_ERR,               /**< Error occurred while writing OTA data. */
    ESP_MESH_LITE_EVENT_OTA_BEGIN_FAIL,              /**< Failed to begin OTA update process. */
    ESP_MESH_LITE_EVENT_OTA_PENDING_TIMEOUT,         /**< Timeout occurred while waiting for OTA update to begin. */
    ESP_MESH_LITE_EVENT_OTA_GET_PARTITION_ERR,       /**< Error occurred while obtaining OTA partition information. */
    ESP_MESH_LITE_EVENT_OTA_SET_BOOT_PARTITION_ERR,  /**< Error occurred while setting the boot partition after OTA update. */
} esp_mesh_lite_ota_finish_reason_t;

/**
 * @brief Enumeration type definition for specifying the type of ESP-Mesh-Lite OTA transmission.
 */
typedef enum {
    ESP_MESH_LITE_OTA_TRANSMIT_FIRMWARE, /**< Transmit firmware version */
    ESP_MESH_LITE_OTA_TRANSMIT_BINARY,   /**< Transmit firmware binary file */
} esp_mesh_lite_ota_transmit_type_t;

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

/**
 * @brief Structure defining callback functions for LAN OTA (Over-The-Air) file transfer in ESP-Mesh-Lite.
 *        These callback functions are used to provide, get, and indicate completion of file transfer.
 */
typedef struct {
    lan_ota_provide_file_cb_t provide_file_cb;       /**< Callback function for providing LAN OTA file data. */
    lan_ota_get_file_cb_t get_file_cb;               /**< Callback function for getting LAN OTA file data. */
    lan_ota_get_file_done_cb_t get_file_done;        /**< Callback function for indicating completion of LAN OTA file reception. */
} esp_mesh_lite_lan_ota_file_transfer_cb_t;

/**
 * @brief Structure representing an event indicating the completion of an OTA (Over-The-Air) update in ESP-Mesh-Lite.
 *        It contains the reason for the OTA update completion.
 */
typedef struct {
    esp_mesh_lite_ota_finish_reason_t reason; /**< Reason for the OTA update completion. */
} esp_mesh_lite_event_ota_finish_t;

/**
 * @brief Structure representing an event indicating the progress of an OTA (Over-The-Air) update in ESP-Mesh-Lite.
 *        It contains the percentage of completion.
 */
typedef struct {
    uint8_t percentage; /**< Percentage of completion of the OTA update. */
} esp_mesh_lite_event_ota_progress_t;

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
    const char* rsp_type;     /**< The message type expected to be received. When a message of the expected type is received, stop retransmitting.
                                If set to NULL, it will be sent until the maximum number of retransmissions is reached. */
    msg_process_cb_t process; /**< The callback function when receiving the 'type' message. The cjson information in the type message can be processed in this cb. */
} esp_mesh_lite_msg_action_t;

/**
 * @brief Callback structure for ESP-Mesh-Lite scanning events.
 *
 * This structure defines the callback functions for handling scanning events in ESP-Mesh-Lite.
 * It consists of two function pointers:
 *   - scan_start_cb: Callback function invoked when a scanning operation starts.
 *   - scan_end_cb: Callback function invoked when a scanning operation ends.
 */
typedef struct esp_mesh_lite_scan_cb {
    void (*scan_start_cb)(void); /**< Callback function for scan start event. */
    void (*scan_end_cb)(void);   /**< Callback function for scan end event. */
} esp_mesh_lite_scan_cb_t;

/**
 * @brief Structure type definition for configuring ESP-Mesh-Lite OTA transmission parameters.
 */
typedef struct {
    esp_mesh_lite_ota_transmit_type_t type;                 /**< Transmission type */
    union {
        char fw_version[32];                                /**< Firmware version, used for ESP_MESH_LITE_OTA_TRANSMIT_FIRMWARE */
        char file_name[32];                                 /**< Firmware file name, used for not ESP_MESH_LITE_OTA_TRANSMIT_FIRMWARE type */
    };
    size_t size;                                            /**< Size of the transmitted data */
    extern_url_ota_cb_t extern_url_ota_cb;                  /**< External URL OTA callback function */
} esp_mesh_lite_file_transmit_config_t;

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
 * @brief Disconnect from the ESP-Mesh-Lite network.
 *
 * This function disconnects the device from the ESP-Mesh-Lite network.
 * After executing this function, the device will stop attempting to reconnect
 * to the network.
 *
 * @return
 *     - ESP_OK if the disconnection was successful.
 *     - ESP_FAIL if the disconnection failed, for example due to invalid state.
 */
esp_err_t esp_mesh_lite_disconnect(void);

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
 * @brief Set the minimum RSSI threshold when connecting to a router.
 *
 * This function is used to set the minimum Received Signal Strength Indication (RSSI) threshold
 * when connecting to a router. When a device attempts to connect to a router, available routers
 * will be filtered based on the configured minimum RSSI threshold. If the RSSI of an available
 * router is lower than this threshold, the device will not connect to that router.
 *
 * @param rssi_min The minimum RSSI threshold in dBm (decibels per milliwatt), ranging from -128 to 127.
 *                 Negative values indicate weak signal strength, while positive values indicate strong
 *                 signal strength. For example, -80 means the device will not connect to a router with
 *                 a signal strength lower than -80 dBm.
 *
 * @note This function allows customization of the threshold according to specific requirements.
 *       The threshold set by this function only takes effect when the device attempts to connect to a router.
 *       Ensure that ESP-Mesh-Lite has been initialized before calling this function.
 */
void esp_mesh_lite_set_router_min_rssi_threshold(int8_t rssi_min);

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
 * Usage Example:
 * @code{.c}
 * static volatile bool app_scan_stage1 = false;
 * static volatile bool app_scan_stage2 = false;
 *
 * void app_scan_start_post(void)
 * {
 *     if (app_scan_stage1) {
 *         app_scan_stage2 = true;
 *     }
 * }
 *
 * void app_scan_end_post(void)
 * {
 *     if (app_scan_stage1) {
 *         app_scan_stage1 = false;
 *         app_scan_stage2 = false;
 *     }
 * }
 *
 * static void app_scan_done_handler(void* arg, esp_event_base_t event_base,
 *                                   int32_t event_id, void* event_data)
 * {
 *     if (!app_scan_stage2) {
 *         return;
 *     }
 *     // Application code
 *     app_scan_end_post();
 * }
 *
 * static void app_scan(void)
 * {
 *     if (app_scan_stage2 == true) {
 *         // App scan in progress
 *         return;
 *     }
 *
 *     app_scan_stage1 = true;
 *     if (esp_mesh_lite_wifi_scan_start(NULL, 1000 / portTICK_PERIOD_MS) != ESP_OK) {
 *         app_scan_stage1 = false;
 *     }
 * }
 *
 * esp_mesh_lite_scan_cb_t app_scan_cb = {
 *     .scan_start_cb = app_scan_start_post,
 *     .scan_end_cb = app_scan_end_post,
 * };
 * esp_mesh_lite_scan_cb_register(&app_scan_cb);
 * @endcode
 *
 * @return
 *    - 0: scan start successful
 */
esp_err_t esp_mesh_lite_wifi_scan_start(const wifi_scan_config_t *config, uint32_t timeout);

/**
 * @brief Register a callback function to check if BSSID is contained.
 *
 * Usage Example:
 * @code{.c}
 * typedef struct esp_mesh_lite_mac_table {
 *     uint8_t bssid[6];
 *     uint8_t ssid[33];
 * } esp_mesh_lite_mac_table_t;
 *
 * static const esp_mesh_lite_mac_table_t mac_table[] = {
 *     {{0x4c, 0x1a, 0x3d, 0x9f, 0xc1, 0x71}, "ssid_83a1"},
 *     {{0x71, 0x9b, 0xc6, 0x2e, 0x57, 0x82}, "ssid_3e52"},
 * };
 *
 * static const uint8_t* check_is_contains_bssid(const uint8_t *bssid) {
 *     size_t mac_table_size = sizeof(mac_table) / sizeof(mac_table[0]);
 *     for (uint8_t i = 0; i < mac_table_size; i++) {
 *         if (!memcmp(mac_table[i].bssid, bssid, sizeof(mac_table[i].bssid))) {
 *             return mac_table[i].ssid;
 *         }
 *     }
 *     return NULL;
 * }
 *
 * esp_mesh_lite_bssid_check_cb_register(check_is_contains_bssid);
 * @endcode
 *
 * @param[in] callback Pointer to the callback function.
 *
 * @return
 *     - ESP_OK: Registration successful.
 */
esp_err_t esp_mesh_lite_bssid_check_cb_register(esp_mesh_lite_get_ssid_by_mac_cb_t cb);

/**
 * @brief Register callback functions for ESP-Mesh-Lite scanning events.
 *
 * This function registers the callback functions defined in the esp_mesh_lite_scan_cb_t structure
 * to handle scanning events in ESP-Mesh-Lite. The provided callbacks will be invoked when scanning
 * operations start and end respectively.
 *
 * Usage Example:
 * @code{.c}
 * static void app_scan_start(void)
 * {
 *     // Implementation of scan start callback
 *     return;
 * }
 *
 * static void app_scan_end(void)
 * {
 *     // Implementation of scan end callback
 *     return;
 * }
 *
 * static esp_mesh_lite_scan_cb_t scan_cb = {
 *     .scan_start_cb = app_scan_start,
 *     .scan_end_cb = app_scan_end,
 * };
 *
 * esp_mesh_lite_scan_cb_register(&scan_cb);
 * @endcode
 *
 * @param cb Pointer to the esp_mesh_lite_scan_cb_t structure containing the callback functions
 *           for handling scanning events.
 * @return
 *     - ESP_OK: Callback registration successful.
 *     - ESP_ERR_INVALID_ARG: Invalid argument provided.
 */
esp_err_t esp_mesh_lite_scan_cb_register(esp_mesh_lite_scan_cb_t *cb);

#ifdef CONFIG_ESP_MESH_LITE_OTA_ENABLE
/*****************************************************/
/************ ESP Wi-Fi Mesh Lite LAN OTA ************/
/*****************************************************/

/**
 * @brief Function to start ESP-Mesh-Lite OTA transmission.
 *
 * @param[in] transmit_config Pointer to the transmission configuration parameters.
 *
 * @return
 *     - ESP_OK: OTA transmission started successfully.
 *     - Other: Error code indicating failure to start OTA transmission.
 */
esp_err_t esp_mesh_lite_transmit_file_start(esp_mesh_lite_file_transmit_config_t *transmit_config);

/**
 * @brief Set the file name to be provided during LAN OTA (Over-The-Air) updates in ESP-Mesh-Lite.
 *
 * @param file_name Pointer to the file name string.
 *
 * @return esp_err_t Returns ESP_OK if the file name is successfully set; otherwise, an error code indicating the reason for failure.
 */
esp_err_t esp_mesh_lite_lan_ota_set_file_name(char *file_name);

/**
 * @brief Register or unregister callback functions for LAN OTA (Over-The-Air) file transfer in ESP-Mesh-Lite.
 *        These callback functions will be used to provide, get, and indicate completion of file transfer during LAN OTA updates.
 *        To unregister the callbacks, pass NULL as the argument.
 *
 * Usage Example:
 * @code{.c}
 * static esp_ota_handle_t update_handle = 0;
 * static const esp_partition_t *next_app_partition = NULL;
 *
 * static esp_err_t provide_file_cb(int filesize, char *fw_version, size_t offset, size_t data_size, char *data)
 * {
 *     // check filesize and fw_version
 *     const esp_partition_t* running_partition = esp_ota_get_running_partition();
 *     return esp_partition_read(running_partition, offset, data, data_size);
 * }
 *
 * static esp_err_t get_file_cb(char *data, size_t data_size)
 * {
 *     return esp_ota_write(update_handle, (const void *)data, data_size);
 * }
 *
 * static esp_err_t get_file_done(void)
 * {
 *     esp_ota_end(update_handle);
 *     return esp_ota_set_boot_partition(next_app_partition);
 * }
 *
 * static void app_lan_ota_start(void)
 * {
 *     next_app_partition = esp_ota_get_next_update_partition(NULL);
 *     if (next_app_partition) {
 *         esp_ota_begin(next_app_partition, OTA_WITH_SEQUENTIAL_WRITES, &update_handle);
 *         esp_mesh_lite_file_transmit_config_t transmit_config = {
 *             .type = ESP_MESH_LITE_OTA_TRANSMIT_FIRMWARE,
 *             .fw_version = "6e90e2",
 *             .size = 890880,
 *             .extern_url_ota_cb = NULL,
 *         };
 *         esp_mesh_lite_transmit_file_start(&transmit_config);
 *     }
 * }
 *
 * esp_mesh_lite_lan_ota_file_transfer_cb_t lan_ota_cb = {
 *     .provide_file_cb = provide_file_cb,
 *     .get_file_cb = get_file_cb,
 *     .get_file_done = get_file_done,
 * };
 *
 * static void app_ota_file_transfer_cb_register(void)
 * {
 *     esp_mesh_lite_ota_register_file_transfer_cb(&lan_ota_cb);
 * }
 * @endcode
 *
 * @param cb Pointer to the structure containing the callback functions, or NULL to unregister the callbacks.
 */
void esp_mesh_lite_ota_register_file_transfer_cb(esp_mesh_lite_lan_ota_file_transfer_cb_t *cb);

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

#endif /* CONFIG_ESP_MESH_LITE_OTA_ENABLE */

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
