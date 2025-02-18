/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>

#include <esp_log.h>
#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_timer.h>

#include <wifi_provisioning/manager.h>

#ifdef CONFIG_MESH_LITE_PROV_TRANSPORT_BLE
#include <wifi_provisioning/scheme_ble.h>
#endif /* CONFIG_MESH_LITE_PROV_TRANSPORT_BLE */

#ifdef CONFIG_MESH_LITE_PROV_TRANSPORT_SOFTAP
#include <wifi_provisioning/scheme_softap.h>
#endif /* CONFIG_MESH_LITE_PROV_TRANSPORT_SOFTAP */
#ifdef CONFIG_PROV_SHOW_QR
#include "qrcode.h"
#endif

#include "esp_mesh_lite.h"

#define MESH_LITE_PROV_QR_VERSION         "v1"
#define MESH_LITE_PROV_TRANSPORT_SOFTAP   "softap"
#define MESH_LITE_PROV_TRANSPORT_BLE      "ble"
#define MESH_LITE_QRCODE_BASE_URL         "https://espressif.github.io/esp-jumpstart/qrcode.html"

typedef struct mesh_lite_config {
    uint8_t ssid[32];                         /**< SSID of target AP. */
    uint8_t password[64];
    uint32_t mesh_id;
} mesh_lite_config_t;

static const char *TAG = "esp_mesh_lite_wifi_prov_mgr";

static bool wifi_prov_status = false;
const int WIFI_CONNECTED_EVENT = BIT0;
static TimerHandle_t prov_timeout_timer = NULL;
static mesh_lite_config_t wifi_prov_mgr_mesh_lite_cfg;

#if CONFIG_PROV_SECURITY_VERSION_2
#if CONFIG_PROV_SEC2_DEV_MODE
#define MESH_LITE_PROV_SEC2_USERNAME          "wifiprov"
#define MESH_LITE_PROV_SEC2_PWD               "abcd1234"

/* This salt,verifier has been generated for username = "wifiprov" and password = "abcd1234"
 * IMPORTANT NOTE: For production cases, this must be unique to every device
 * and should come from device manufacturing partition.*/
static const char sec2_salt[] = {
    0x03, 0x6e, 0xe0, 0xc7, 0xbc, 0xb9, 0xed, 0xa8, 0x4c, 0x9e, 0xac, 0x97, 0xd9, 0x3d, 0xec, 0xf4
};

static const char sec2_verifier[] = {
    0x7c, 0x7c, 0x85, 0x47, 0x65, 0x08, 0x94, 0x6d, 0xd6, 0x36, 0xaf, 0x37, 0xd7, 0xe8, 0x91, 0x43,
    0x78, 0xcf, 0xfd, 0x61, 0x6c, 0x59, 0xd2, 0xf8, 0x39, 0x08, 0x12, 0x72, 0x38, 0xde, 0x9e, 0x24,
    0xa4, 0x70, 0x26, 0x1c, 0xdf, 0xa9, 0x03, 0xc2, 0xb2, 0x70, 0xe7, 0xb1, 0x32, 0x24, 0xda, 0x11,
    0x1d, 0x97, 0x18, 0xdc, 0x60, 0x72, 0x08, 0xcc, 0x9a, 0xc9, 0x0c, 0x48, 0x27, 0xe2, 0xae, 0x89,
    0xaa, 0x16, 0x25, 0xb8, 0x04, 0xd2, 0x1a, 0x9b, 0x3a, 0x8f, 0x37, 0xf6, 0xe4, 0x3a, 0x71, 0x2e,
    0xe1, 0x27, 0x86, 0x6e, 0xad, 0xce, 0x28, 0xff, 0x54, 0x46, 0x60, 0x1f, 0xb9, 0x96, 0x87, 0xdc,
    0x57, 0x40, 0xa7, 0xd4, 0x6c, 0xc9, 0x77, 0x54, 0xdc, 0x16, 0x82, 0xf0, 0xed, 0x35, 0x6a, 0xc4,
    0x70, 0xad, 0x3d, 0x90, 0xb5, 0x81, 0x94, 0x70, 0xd7, 0xbc, 0x65, 0xb2, 0xd5, 0x18, 0xe0, 0x2e,
    0xc3, 0xa5, 0xf9, 0x68, 0xdd, 0x64, 0x7b, 0xb8, 0xb7, 0x3c, 0x9c, 0xfc, 0x00, 0xd8, 0x71, 0x7e,
    0xb7, 0x9a, 0x7c, 0xb1, 0xb7, 0xc2, 0xc3, 0x18, 0x34, 0x29, 0x32, 0x43, 0x3e, 0x00, 0x99, 0xe9,
    0x82, 0x94, 0xe3, 0xd8, 0x2a, 0xb0, 0x96, 0x29, 0xb7, 0xdf, 0x0e, 0x5f, 0x08, 0x33, 0x40, 0x76,
    0x52, 0x91, 0x32, 0x00, 0x9f, 0x97, 0x2c, 0x89, 0x6c, 0x39, 0x1e, 0xc8, 0x28, 0x05, 0x44, 0x17,
    0x3f, 0x68, 0x02, 0x8a, 0x9f, 0x44, 0x61, 0xd1, 0xf5, 0xa1, 0x7e, 0x5a, 0x70, 0xd2, 0xc7, 0x23,
    0x81, 0xcb, 0x38, 0x68, 0xe4, 0x2c, 0x20, 0xbc, 0x40, 0x57, 0x76, 0x17, 0xbd, 0x08, 0xb8, 0x96,
    0xbc, 0x26, 0xeb, 0x32, 0x46, 0x69, 0x35, 0x05, 0x8c, 0x15, 0x70, 0xd9, 0x1b, 0xe9, 0xbe, 0xcc,
    0xa9, 0x38, 0xa6, 0x67, 0xf0, 0xad, 0x50, 0x13, 0x19, 0x72, 0x64, 0xbf, 0x52, 0xc2, 0x34, 0xe2,
    0x1b, 0x11, 0x79, 0x74, 0x72, 0xbd, 0x34, 0x5b, 0xb1, 0xe2, 0xfd, 0x66, 0x73, 0xfe, 0x71, 0x64,
    0x74, 0xd0, 0x4e, 0xbc, 0x51, 0x24, 0x19, 0x40, 0x87, 0x0e, 0x92, 0x40, 0xe6, 0x21, 0xe7, 0x2d,
    0x4e, 0x37, 0x76, 0x2f, 0x2e, 0xe2, 0x68, 0xc7, 0x89, 0xe8, 0x32, 0x13, 0x42, 0x06, 0x84, 0x84,
    0x53, 0x4a, 0xb3, 0x0c, 0x1b, 0x4c, 0x8d, 0x1c, 0x51, 0x97, 0x19, 0xab, 0xae, 0x77, 0xff, 0xdb,
    0xec, 0xf0, 0x10, 0x95, 0x34, 0x33, 0x6b, 0xcb, 0x3e, 0x84, 0x0f, 0xb9, 0xd8, 0x5f, 0xb8, 0xa0,
    0xb8, 0x55, 0x53, 0x3e, 0x70, 0xf7, 0x18, 0xf5, 0xce, 0x7b, 0x4e, 0xbf, 0x27, 0xce, 0xce, 0xa8,
    0xb3, 0xbe, 0x40, 0xc5, 0xc5, 0x32, 0x29, 0x3e, 0x71, 0x64, 0x9e, 0xde, 0x8c, 0xf6, 0x75, 0xa1,
    0xe6, 0xf6, 0x53, 0xc8, 0x31, 0xa8, 0x78, 0xde, 0x50, 0x40, 0xf7, 0x62, 0xde, 0x36, 0xb2, 0xba
};
#endif

static esp_err_t esp_mesh_lite_get_sec2_salt(const char **salt, uint16_t *salt_len)
{
#if CONFIG_PROV_SEC2_DEV_MODE
    ESP_LOGI(TAG, "Development mode: using hard coded salt");
    *salt = sec2_salt;
    *salt_len = sizeof(sec2_salt);
    return ESP_OK;
#elif CONFIG_PROV_SEC2_PROD_MODE
    ESP_LOGE(TAG, "Not implemented!");
    return ESP_FAIL;
#endif
}

static esp_err_t esp_mesh_lite_get_sec2_verifier(const char **verifier, uint16_t *verifier_len)
{
#if CONFIG_PROV_SEC2_DEV_MODE
    ESP_LOGI(TAG, "Development mode: using hard coded verifier");
    *verifier = sec2_verifier;
    *verifier_len = sizeof(sec2_verifier);
    return ESP_OK;
#elif CONFIG_PROV_SEC2_PROD_MODE
    /* This code needs to be updated with appropriate implementation to provide verifier */
    ESP_LOGE(TAG, "Not implemented!");
    return ESP_FAIL;
#endif
}
#endif

bool wifi_provision_in_progress(void)
{
    return wifi_prov_status;
}

esp_err_t __attribute__((weak)) wifi_prov_wifi_connect(wifi_sta_config_t *conf)
{
    esp_err_t ret = ESP_OK;
#if CONFIG_MESH_LITE_ENABLE
    mesh_lite_sta_config_t config = {0};
    memcpy((char*)config.ssid, (char*)conf->ssid, sizeof(config.ssid));
    memcpy((char*)config.password, (char*)conf->password, sizeof(config.password));
    config.bssid_set = conf->bssid_set;
    if (config.bssid_set) {
        memcpy((char*)config.bssid, (char*)conf->bssid, sizeof(config.bssid));
    }
    esp_mesh_lite_set_router_config(&config);
    esp_mesh_lite_connect();
#else
    esp_wifi_set_storage(WIFI_STORAGE_FLASH);
    ret = esp_wifi_set_config(ESP_IF_WIFI_STA, (wifi_config_t *)conf);
    esp_wifi_set_storage(WIFI_STORAGE_RAM);
    esp_wifi_disconnect();
    esp_wifi_connect();
#endif
    return ret;
}

static void prov_timeout_timer_callback(TimerHandle_t timer)
{
    ESP_LOGW(TAG, "Provisioning timed out. Please reboot device to restart provisioning.");
    xTimerStop(timer, 10);
    wifi_prov_status = false;
    wifi_prov_mgr_deinit();
}

void wifi_provision_stop(void)
{
    wifi_prov_mgr_stop_provisioning();
}

/* Event handler for catching system events */
static void event_handler(void *arg, esp_event_base_t event_base,
                          int32_t event_id, void *event_data)
{
    if (!wifi_prov_status) {
        return;
    }

#ifdef CONFIG_PROV_RESET_PROV_MGR_ON_FAILURE
    static int retries;
#endif

    if (event_base == WIFI_PROV_EVENT) {
        switch (event_id) {
        case WIFI_PROV_START:
            ESP_LOGI(TAG, "Provisioning started");
            break;

        case WIFI_PROV_CUST_DATA_RECV: {
            mesh_lite_config_t* mesh_lite_cfg = (mesh_lite_config_t*)event_data;
            char softap_ssid[40];
            uint8_t softap_mac[6];
            esp_wifi_get_mac(WIFI_IF_AP, softap_mac);
            memset(softap_ssid, 0x0, sizeof(softap_ssid));
            ESP_LOGI(TAG, "Received Mesh credentials\n\tSSID     : %s\n\tPassword : %s\n\tMeshID : %ld",
                     (const char *)mesh_lite_cfg->ssid, (const char *)mesh_lite_cfg->password, mesh_lite_cfg->mesh_id);
            if (strlen((char *)mesh_lite_cfg->ssid)) {
                memcpy(&wifi_prov_mgr_mesh_lite_cfg, mesh_lite_cfg, sizeof(wifi_prov_mgr_mesh_lite_cfg));
#ifdef CONFIG_BRIDGE_SOFTAP_SSID_END_WITH_THE_MAC
                snprintf(softap_ssid, sizeof(softap_ssid), "%.25s_%02x%02x%02x", mesh_lite_cfg->ssid, softap_mac[3], softap_mac[4], softap_mac[5]);
#else
                snprintf(softap_ssid, sizeof(softap_ssid), "%.32s", mesh_lite_cfg->ssid);
#endif
                strncpy((char*)wifi_prov_mgr_mesh_lite_cfg.ssid, softap_ssid, sizeof(wifi_prov_mgr_mesh_lite_cfg.ssid));
            } else {
#ifdef CONFIG_BRIDGE_SOFTAP_SSID_END_WITH_THE_MAC
                snprintf(softap_ssid, sizeof(softap_ssid), "%.25s_%02x%02x%02x", CONFIG_BRIDGE_SOFTAP_SSID, softap_mac[3], softap_mac[4], softap_mac[5]);
#else
                snprintf(softap_ssid, sizeof(softap_ssid), "%.32s", CONFIG_BRIDGE_SOFTAP_SSID);
#endif
                strncpy((char*)wifi_prov_mgr_mesh_lite_cfg.ssid, softap_ssid, sizeof(wifi_prov_mgr_mesh_lite_cfg.ssid));
                strncpy((char*)wifi_prov_mgr_mesh_lite_cfg.password, CONFIG_BRIDGE_SOFTAP_PASSWORD, sizeof(wifi_prov_mgr_mesh_lite_cfg.password));
                wifi_prov_mgr_mesh_lite_cfg.mesh_id = CONFIG_MESH_LITE_ID;
            }
            break;
        }

        case WIFI_PROV_CRED_RECV: {
            wifi_sta_config_t *wifi_sta_cfg = (wifi_sta_config_t *)event_data;
            ESP_LOGI(TAG, "Received Wi-Fi credentials"
                     "\n\tSSID     : %s\n\tPassword : %s",
                     (const char *)wifi_sta_cfg->ssid,
                     (const char *)wifi_sta_cfg->password);
            wifi_prov_wifi_connect(wifi_sta_cfg);
            break;
        }

        case WIFI_PROV_CRED_FAIL: {
            wifi_prov_sta_fail_reason_t *reason = (wifi_prov_sta_fail_reason_t *)event_data;
            ESP_LOGE(TAG, "Provisioning failed!\n\tReason : %s"
                     "\n\tPlease reset to factory and retry provisioning",
                     (*reason == WIFI_PROV_STA_AUTH_ERROR) ?
                     "Wi-Fi station authentication failed" : "Wi-Fi access-point not found");
#ifdef CONFIG_PROV_RESET_PROV_MGR_ON_FAILURE
            retries++;
            if (retries >= CONFIG_PROV_MGR_MAX_RETRY_CNT) {
                ESP_LOGI(TAG, "Failed to connect with provisioned AP, reseting provisioned credentials");
                wifi_prov_mgr_reset_sm_state_on_failure();
                retries = 0;
            }
#endif
            break;
        }

        case WIFI_PROV_CRED_SUCCESS:
            ESP_LOGI(TAG, "Provisioning successful");
#ifdef CONFIG_PROV_RESET_PROV_MGR_ON_FAILURE
            retries = 0;
#endif
            break;

        case WIFI_PROV_END:
            xTimerDelete(prov_timeout_timer, 10);
            if (wifi_prov_status) {
                /* De-initialize manager once provisioning is finished */
                wifi_prov_mgr_deinit();
                wifi_prov_status = false;
            }
            break;

        default:
            break;
        }
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        // esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "Connected with IP Address:" IPSTR, IP2STR(&event->ip_info.ip));
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 1, 0)
#ifdef CONFIG_MESH_LITE_PROV_TRANSPORT_BLE
    } else if (event_base == PROTOCOMM_TRANSPORT_BLE_EVENT) {
        switch (event_id) {
        case PROTOCOMM_TRANSPORT_BLE_CONNECTED:
            ESP_LOGI(TAG, "BLE transport: Connected!");
            break;
        case PROTOCOMM_TRANSPORT_BLE_DISCONNECTED:
            ESP_LOGI(TAG, "BLE transport: Disconnected!");
            break;
        default:
            break;
        }
#endif
    } else if (event_base == PROTOCOMM_SECURITY_SESSION_EVENT) {
        switch (event_id) {
        case PROTOCOMM_SECURITY_SESSION_SETUP_OK:
            ESP_LOGI(TAG, "Secured session established!");
            break;
        case PROTOCOMM_SECURITY_SESSION_INVALID_SECURITY_PARAMS:
            ESP_LOGE(TAG, "Received invalid security parameters for establishing secure session!");
            break;
        case PROTOCOMM_SECURITY_SESSION_CREDENTIALS_MISMATCH:
            ESP_LOGE(TAG, "Received incorrect username and/or PoP for establishing secure session!");
            break;
        default:
            break;
        }
#endif
    }
}

// register Wi-Fi Provisioning events
static void wifi_prov_event_register(void)
{
    /* Register our event handler for Wi-Fi, IP and Provisioning related events */
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_PROV_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 1, 0)
#ifdef CONFIG_MESH_LITE_PROV_TRANSPORT_BLE
    ESP_ERROR_CHECK(esp_event_handler_register(PROTOCOMM_TRANSPORT_BLE_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
#endif
    ESP_ERROR_CHECK(esp_event_handler_register(PROTOCOMM_SECURITY_SESSION_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
#endif
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));
}

void get_device_service_name(char *service_name, size_t max)
{
    uint8_t eth_mac[6];
    const char *ssid_prefix = "PROV_";
    esp_wifi_get_mac(WIFI_IF_STA, eth_mac);
    snprintf(service_name, max, "%s%02X%02X%02X",
             ssid_prefix, eth_mac[3], eth_mac[4], eth_mac[5]);
}

/* Handler for the optional provisioning endpoint registered by the application.
 * The data format can be chosen by applications. Here, we are using plain ascii text.
 * Applications can choose to use other formats like protobuf, JSON, XML, etc.
 */
esp_err_t custom_prov_data_handler(uint32_t session_id, const uint8_t *inbuf, ssize_t inlen,
                                   uint8_t **outbuf, ssize_t *outlen, void *priv_data)
{
    if (inbuf) {
        ESP_LOGI(TAG, "Received data: %.*s", inlen, (char *)inbuf);
    }
    char response[] = "SUCCESS";
    *outbuf = (uint8_t *)strdup(response);
    if (*outbuf == NULL) {
        ESP_LOGE(TAG, "System out of memory");
        return ESP_ERR_NO_MEM;
    }
    *outlen = strlen(response) + 1; /* +1 for NULL terminating byte */

    return ESP_OK;
}

static void wifi_prov_print_qr(const char *name, const char *username, const char *pop, const char *transport)
{
    if (!name || !transport) {
        ESP_LOGW(TAG, "Cannot generate QR code payload. Data missing.");
        return;
    }
    char payload[150] = { 0 };
    if (pop) {
#if CONFIG_PROV_SECURITY_VERSION_1
        snprintf(payload, sizeof(payload), "{\"ver\":\"%s\",\"name\":\"%s\"" \
                 ",\"pop\":\"%s\",\"transport\":\"%s\"}",
                 MESH_LITE_PROV_QR_VERSION, name, pop, transport);
#elif CONFIG_PROV_SECURITY_VERSION_2
        snprintf(payload, sizeof(payload), "{\"ver\":\"%s\",\"name\":\"%s\"" \
                 ",\"username\":\"%s\",\"pop\":\"%s\",\"transport\":\"%s\"}",
                 MESH_LITE_PROV_QR_VERSION, name, username, pop, transport);
#endif
    } else {
        snprintf(payload, sizeof(payload), "{\"ver\":\"%s\",\"name\":\"%s\"" \
                 ",\"transport\":\"%s\"}",
                 MESH_LITE_PROV_QR_VERSION, name, transport);
    }
#ifdef CONFIG_PROV_SHOW_QR
    ESP_LOGI(TAG, "Scan this QR code from the provisioning application for Provisioning.");
    esp_qrcode_config_t cfg = ESP_QRCODE_CONFIG_DEFAULT();
    esp_qrcode_generate(&cfg, payload);
#endif /* CONFIG_PROV_SHOW_QR */
    ESP_LOGI(TAG, "If QR code is not visible, copy paste the below URL in a browser.\n%s?data=%s", MESH_LITE_QRCODE_BASE_URL, payload);
}

esp_err_t esp_mesh_lite_wifi_prov_mgr_deinit(void)
{
    if (wifi_prov_status) {
        wifi_prov_mgr_deinit();
        wifi_prov_status = false;
        return ESP_OK;
    } else {
        ESP_LOGW(TAG, "Provisioning not in progress");
        return ESP_FAIL;
    }
}

esp_err_t esp_mesh_lite_wifi_prov_mgr_init(void)
{
    if (wifi_prov_status) {
        ESP_LOGW(TAG, "Provisioning already in progress");
        return ESP_ERR_INVALID_STATE;
    }
    wifi_prov_status = true;

    wifi_prov_event_register();

#ifdef CONFIG_MESH_LITE_PROV_TRANSPORT_SOFTAP
    esp_netif_create_default_wifi_ap();
#endif /* CONFIG_MESH_LITE_PROV_TRANSPORT_SOFTAP */

    /* Configuration for the provisioning manager */
    wifi_prov_mgr_config_t config = {
        /* What is the Provisioning Scheme that we want ?
         * wifi_prov_scheme_softap or wifi_prov_scheme_ble */
#ifdef CONFIG_MESH_LITE_PROV_TRANSPORT_BLE
        .scheme = wifi_prov_scheme_ble,
#endif /* CONFIG_MESH_LITE_PROV_TRANSPORT_BLE */
#ifdef CONFIG_MESH_LITE_PROV_TRANSPORT_SOFTAP
        .scheme = wifi_prov_scheme_softap,
#endif /* CONFIG_MESH_LITE_PROV_TRANSPORT_SOFTAP */

        /* Any default scheme specific event handler that you would
         * like to choose. Since our example application requires
         * neither BT nor BLE, we can choose to release the associated
         * memory once provisioning is complete, or not needed
         * (in case when device is already provisioned). Choosing
         * appropriate scheme specific event handler allows the manager
         * to take care of this automatically. This can be set to
         * WIFI_PROV_EVENT_HANDLER_NONE when using wifi_prov_scheme_softap*/
#ifdef CONFIG_MESH_LITE_PROV_TRANSPORT_BLE
#ifdef CONFIG_PROV_RELEASE_BLE_MEMORY_AFTER_PROVISIONED
        .scheme_event_handler = WIFI_PROV_SCHEME_BLE_EVENT_HANDLER_FREE_BTDM
#else
        .scheme_event_handler = WIFI_PROV_SCHEME_BLE_EVENT_HANDLER_FREE_BT
#endif /* CONFIG_PROV_RELEASE_BLE_MEMORY_AFTER_PROVISIONED */
#endif /* CONFIG_MESH_LITE_PROV_TRANSPORT_BLE */
#ifdef CONFIG_MESH_LITE_PROV_TRANSPORT_SOFTAP
        .scheme_event_handler = WIFI_PROV_EVENT_HANDLER_NONE
#endif /* CONFIG_MESH_LITE_PROV_TRANSPORT_SOFTAP */
    };

    /* Initialize provisioning manager with the
     * configuration parameters set above */
    ESP_ERROR_CHECK(wifi_prov_mgr_init(config));

    bool provisioned = false;
#ifdef CONFIG_PROV_RESET_PROVISIONED
    wifi_prov_mgr_reset_provisioning();
#else
    /* Let's find out if the device is provisioned */
    ESP_ERROR_CHECK(wifi_prov_mgr_is_provisioned(&provisioned));
#endif
    /* If device is not yet provisioned start provisioning service */
    if (!provisioned) {
        ESP_LOGI(TAG, "Starting provisioning");

        /* What is the Device Service Name that we want
         * This translates to :
         *     - Wi-Fi SSID when scheme is wifi_prov_scheme_softap
         *     - device name when scheme is wifi_prov_scheme_ble
         */
        char service_name[12];
        get_device_service_name(service_name, sizeof(service_name));

#ifdef CONFIG_PROV_SECURITY_VERSION_1
        /* What is the security level that we want (0, 1, 2):
         *      - WIFI_PROV_SECURITY_0 is simply plain text communication.
         *      - WIFI_PROV_SECURITY_1 is secure communication which consists of secure handshake
         *          using X25519 key exchange and proof of possession (pop) and AES-CTR
         *          for encryption/decryption of messages.
         *      - WIFI_PROV_SECURITY_2 SRP6a based authentication and key exchange
         *        + AES-GCM encryption/decryption of messages
         */
        wifi_prov_security_t security = WIFI_PROV_SECURITY_1;

        /* Do we want a proof-of-possession (ignored if Security 0 is selected):
         *      - this should be a string with length > 0
         *      - NULL if not used
         */
        const char *pop = "abcd1234";

        /* This is the structure for passing security parameters
         * for the protocomm security 1.
         */
        wifi_prov_security1_params_t *sec_params = pop;

        const char *username  = NULL;

#elif CONFIG_PROV_SECURITY_VERSION_2
        wifi_prov_security_t security = WIFI_PROV_SECURITY_2;
        /* The username must be the same one, which has been used in the generation of salt and verifier */

#if CONFIG_PROV_SEC2_DEV_MODE
        /* This pop field represents the password that will be used to generate salt and verifier.
         * The field is present here in order to generate the QR code containing password.
         * In production this password field shall not be stored on the device */
        const char *username  = MESH_LITE_PROV_SEC2_USERNAME;
        const char *pop = MESH_LITE_PROV_SEC2_PWD;
#elif CONFIG_PROV_SEC2_PROD_MODE
        /* The username and password shall not be embedded in the firmware,
         * they should be provided to the user by other means.
         * e.g. QR code sticker */
        const char *username  = NULL;
        const char *pop = NULL;
#endif
        /* This is the structure for passing security parameters
         * for the protocomm security 2.
         * If dynamically allocated, sec2_params pointer and its content
         * must be valid till WIFI_PROV_END event is triggered.
         */
        wifi_prov_security2_params_t sec2_params = {};

        ESP_ERROR_CHECK(esp_mesh_lite_get_sec2_salt(&sec2_params.salt, &sec2_params.salt_len));
        ESP_ERROR_CHECK(esp_mesh_lite_get_sec2_verifier(&sec2_params.verifier, &sec2_params.verifier_len));

        wifi_prov_security2_params_t *sec_params = &sec2_params;
#endif
        /* What is the service key (could be NULL)
         * This translates to :
         *     - Wi-Fi password when scheme is wifi_prov_scheme_softap
         *          (Minimum expected length: 8, maximum 64 for WPA2-PSK)
         *     - simply ignored when scheme is wifi_prov_scheme_ble
         */
        const char *service_key = NULL;

#ifdef CONFIG_MESH_LITE_PROV_TRANSPORT_BLE
        /* This step is only useful when scheme is wifi_prov_scheme_ble. This will
         * set a custom 128 bit UUID which will be included in the BLE advertisement
         * and will correspond to the primary GATT service that provides provisioning
         * endpoints as GATT characteristics. Each GATT characteristic will be
         * formed using the primary service UUID as base, with different auto assigned
         * 12th and 13th bytes (assume counting starts from 0th byte). The client side
         * applications must identify the endpoints by reading the User Characteristic
         * Description descriptor (0x2901) for each characteristic, which contains the
         * endpoint name of the characteristic */
        uint8_t custom_service_uuid[] = {
            /* LSB <---------------------------------------
             * ---------------------------------------> MSB */
            0xb4, 0xdf, 0x5a, 0x1c, 0x3f, 0x6b, 0xf4, 0xbf,
            0xea, 0x4a, 0x82, 0x03, 0x04, 0x90, 0x1a, 0x02,
        };

        /* If your build fails with linker errors at this point, then you may have
         * forgotten to enable the BT stack or BTDM BLE settings in the SDK (e.g. see
         * the sdkconfig.defaults in the example project) */
        wifi_prov_scheme_ble_set_service_uuid(custom_service_uuid);
#endif /* CONFIG_MESH_LITE_PROV_TRANSPORT_BLE */

        /* An optional endpoint that applications can create if they expect to
         * get some additional custom data during provisioning workflow.
         * The endpoint name can be anything of your choice.
         * This call must be made before starting the provisioning.
         */
        wifi_prov_mgr_endpoint_create("custom-data");

        /* Start provisioning service */
        ESP_ERROR_CHECK(wifi_prov_mgr_start_provisioning(security, (const void *) sec_params, service_name, service_key));

        /* The handler for the optional endpoint created above.
         * This call must be made after starting the provisioning, and only if the endpoint
         * has already been created above.
         */
        wifi_prov_mgr_endpoint_register("custom-data", custom_prov_data_handler, NULL);

        /* Uncomment the following to wait for the provisioning to finish and then release
         * the resources of the manager. Since in this case de-initialization is triggered
         * by the default event loop handler, we don't need to call the following */
        // wifi_prov_mgr_wait();
        // wifi_prov_mgr_deinit();
        /* Print QR code for provisioning */
#ifdef CONFIG_MESH_LITE_PROV_TRANSPORT_BLE
        wifi_prov_print_qr(service_name, username, pop, MESH_LITE_PROV_TRANSPORT_BLE);
#else /* CONFIG_MESH_LITE_PROV_TRANSPORT_SOFTAP */
        wifi_prov_print_qr(service_name, username, pop, MESH_LITE_PROV_TRANSPORT_SOFTAP);
#endif /* CONFIG_MESH_LITE_PROV_TRANSPORT_BLE */

        prov_timeout_timer = xTimerCreate("prov_timeout_timer", CONFIG_PROV_TIMEOUT_SECONDS * 1000 / portTICK_PERIOD_MS, pdTRUE,
                                          NULL, prov_timeout_timer_callback);
        xTimerStart(prov_timeout_timer, portMAX_DELAY);
    } else {
        ESP_LOGI(TAG, "Already provisioned, starting Wi-Fi STA");

        /* We don't need the manager as device is already provisioned,
         * so let's release it's resources */
        wifi_prov_mgr_deinit();
    }
    return ESP_OK;
}
