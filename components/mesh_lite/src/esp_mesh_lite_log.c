/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "string.h"
#include "inttypes.h"
#include "esp_mesh_lite_log.h"

#define LOG_COLOR_LEN    8

__attribute__((weak)) void esp_mesh_lite_log_write(esp_log_level_t level, const char *tag, const char *format, ...)
{
    char letter = 'I';
    char log_color[LOG_COLOR_LEN] = {0};
    memset(log_color, 0, sizeof(log_color));
    switch (level) {
    case ESP_LOG_ERROR:
        letter = 'E';
#ifdef CONFIG_LOG_COLORS
        memcpy(log_color, "\033[0;" "31" "m", LOG_COLOR_LEN);
#endif
        break;
    case ESP_LOG_WARN:
        letter = 'W';
#ifdef CONFIG_LOG_COLORS
        memcpy(log_color, "\033[0;" "33" "m", LOG_COLOR_LEN);
#endif
        break;
    case ESP_LOG_INFO:
        letter = 'I';
#ifdef CONFIG_LOG_COLORS
        memcpy(log_color, "\033[0;" "32" "m", LOG_COLOR_LEN);
#endif
        break;
    case ESP_LOG_DEBUG:
        letter = 'D';
#ifdef CONFIG_LOG_COLORS
        memcpy(log_color, "\033[0;" "36" "m", LOG_COLOR_LEN);
#endif
        break;
    case ESP_LOG_VERBOSE:
        letter = 'V';
        break;
    default:
        break;
    }
    va_list list;
    va_start(list, format);

    size_t new_format_length = strlen(tag) + strlen(format) + 30;
    char new_format[new_format_length];
    snprintf(new_format, new_format_length, "%s%c (%"PRIu32") [%s]: %s " LOG_RESET_COLOR "\n", log_color, letter, esp_log_timestamp(), tag, format);

    esp_log_writev(level, tag, new_format, list);
    va_end(list);
}
