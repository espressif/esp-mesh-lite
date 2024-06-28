/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sdkconfig.h>

#include <iot_button.h>
#include <esp_rmaker_core.h>
#include <esp_rmaker_standard_types.h>
#include <esp_rmaker_standard_params.h>
#include <esp_rmaker_standard_devices.h>

#include <app_reset.h>
#include <app_light.h>
#include <lightbulb.h>

static const char *TAG = "app_light";

/* This is the button that is used for toggling the power */
#define BUTTON_GPIO          CONFIG_EXAMPLE_BOARD_BUTTON_GPIO
#define BUTTON_ACTIVE_LEVEL  0

#define WIFI_RESET_BUTTON_TIMEOUT       3
#define FACTORY_RESET_BUTTON_TIMEOUT    5

static uint16_t g_hue = DEFAULT_HUE;
static uint16_t g_saturation = DEFAULT_SATURATION;
static uint16_t g_value = DEFAULT_BRIGHTNESS;
static bool g_power = DEFAULT_POWER;

extern esp_rmaker_device_t *light_device;

esp_err_t app_light_set_led(uint32_t hue, uint32_t saturation, uint32_t brightness)
{
    /* Whenever this function is called, light power will be ON */
    if (!g_power) {
        g_power = true;
        esp_rmaker_param_update_and_report(
            esp_rmaker_device_get_param_by_type(light_device, ESP_RMAKER_PARAM_POWER),
            esp_rmaker_bool(g_power));
    }
    return lightbulb_set_hsv(hue, saturation, brightness);
}

esp_err_t app_light_set_power(bool power)
{
    g_power = power;
    if (power) {
        lightbulb_set_hsv(g_hue, g_saturation, g_value);
    } else {
        lightbulb_set_hsv(0, 0, 0);
    }
    return ESP_OK;
}

esp_err_t app_light_set_brightness(uint16_t brightness)
{
    g_value = brightness;
    return app_light_set_led(g_hue, g_saturation, g_value);
}

esp_err_t app_light_set_hue(uint16_t hue)
{
    g_hue = hue;
    return app_light_set_led(g_hue, g_saturation, g_value);
}

esp_err_t app_light_set_saturation(uint16_t saturation)
{
    g_saturation = saturation;
    return app_light_set_led(g_hue, g_saturation, g_value);
}

esp_err_t app_light_init(void)
{
    lightbulb_config_t config = {
        //1. Select and configure the chip
#ifdef CONFIG_LIGHTBULB_DEMO_DRIVER_SELECT_WS2812
        .type = DRIVER_WS2812,
        .driver_conf.ws2812.led_num = CONFIG_WS2812_LED_NUM,
        .driver_conf.ws2812.ctrl_io = CONFIG_WS2812_LED_GPIO,
#endif
#ifdef CONFIG_LIGHTBULB_DEMO_DRIVER_SELECT_PWM
        .type = DRIVER_ESP_PWM,
        .driver_conf.pwm.freq_hz = CONFIG_PWM_FREQ_HZ,
#ifdef CONFIG_IDF_TARGET_ESP32C2
        /* Adapt to ESP8684-DevKitM-1
         * For details, please refer to:
         * https://docs.espressif.com/projects/espressif-esp-dev-kits/zh_CN/latest/esp8684/esp8684-devkitm-1/user_guide.html
        */
        .driver_conf.pwm.invert_level = true,
#endif
#endif
#ifdef CONFIG_LIGHTBULB_DEMO_DRIVER_SELECT_SM2135E
        .type = DRIVER_SM2135E,
        .driver_conf.sm2135e.freq_khz = 400,
        .driver_conf.sm2135e.enable_iic_queue = true,
        .driver_conf.sm2135e.iic_clk = CONFIG_SM2135E_IIC_CLK_GPIO,
        .driver_conf.sm2135e.iic_sda = CONFIG_SM2135E_IIC_SDA_GPIO,
        .driver_conf.sm2135e.rgb_current = SM2135E_RGB_CURRENT_20MA,
        .driver_conf.sm2135e.wy_current = SM2135E_WY_CURRENT_40MA,
#endif
        // 2. Configure the drive capability
        .capability.enable_fades = false,
        .capability.fades_ms = 800,
        .capability.enable_status_storage = false,
        .capability.mode_mask = COLOR_MODE,
        .capability.storage_cb = NULL,

        //3. Configure driver io
#ifdef CONFIG_LIGHTBULB_DEMO_DRIVER_SELECT_PWM
        .io_conf.pwm_io.red = CONFIG_PWM_RED_GPIO,
        .io_conf.pwm_io.green = CONFIG_PWM_GREEN_GPIO,
        .io_conf.pwm_io.blue = CONFIG_PWM_BLUE_GPIO,
#endif
#ifdef CONFIG_LIGHTBULB_DEMO_DRIVER_SELECT_SM2135E
        .io_conf.iic_io.red = OUT3,
        .io_conf.iic_io.green = OUT2,
        .io_conf.iic_io.blue = OUT1,
        .io_conf.iic_io.cold_white = OUT5,
        .io_conf.iic_io.warm_yellow = OUT4,
#endif
        //4. Limit param
        .external_limit = NULL,

        //5. Gamma param
        .gamma_conf = NULL,

        //6. Init param
        .init_status.mode = WORK_COLOR,
        .init_status.on = true,
        .init_status.hue = 0,
        .init_status.saturation = 100,
        .init_status.value = 100,
    };
    esp_err_t err = lightbulb_init(&config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "There may be some errors in the configuration, please check the log.");
        return err;
    }

    if (g_power) {
        lightbulb_set_hsv(g_hue, g_saturation, g_value);
    } else {
        lightbulb_set_hsv(0, 0, 0);
    }
    return ESP_OK;
}

static void push_btn_cb(void *arg)
{
    app_espnow_reset_group_control();
}

void app_driver_init()
{
    app_light_init();
    button_handle_t btn_handle = iot_button_create(BUTTON_GPIO, BUTTON_ACTIVE_LEVEL);
    if (btn_handle) {
        /* Register a callback for a button tap (short press) event */
        iot_button_set_evt_cb(btn_handle, BUTTON_CB_TAP, push_btn_cb, NULL);
        /* Register Wi-Fi reset and factory reset functionality on same button */
        app_reset_button_register(btn_handle, WIFI_RESET_BUTTON_TIMEOUT, FACTORY_RESET_BUTTON_TIMEOUT);
    }
}
