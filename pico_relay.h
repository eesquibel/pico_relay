#ifndef __PICO_RELAY_H
#define __PICO_RELAY_H
#include <pico/unique_id.h>

struct pico_relay_t {
    char uuid[2 * PICO_UNIQUE_BOARD_ID_SIZE_BYTES + 1];
    bool is_connected;
    bool discovery_sent;
    uint relay_gpio;
    bool state;
};

void pico_relay_on_connected();

void pico_relay_on_command(char *payload);

#endif
