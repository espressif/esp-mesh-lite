// uart_bridge.h
#pragma once
#ifdef __cplusplus
extern "C" {
#endif

void uart_bridge_init(void);
void uart_bridge_send_line(const char *s);

#ifdef __cplusplus
}
#endif
