#pragma once
#include <stdint.h>

// Start a UDP listener task that logs BLE lines and tags them with sender MAC.
// Returns ESP_OK on success.
int root_udp_start(uint16_t port);
