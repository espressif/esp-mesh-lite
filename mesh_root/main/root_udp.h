#pragma once
#include <stdint.h>

// Start a UDP listener task that logs BLE lines
// Takes the UDP port as argument
void root_udp_start(uint16_t port);
