#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/apps/mqtt.h"
#include "lwip/apps/mqtt_priv.h"
#ifdef SNTP_SERVER
#include "lwip/apps/sntp.h"
#endif

#include "pico_relay.h"
#include "mqtt.h"
#include "home-assistant.h"
#include <pico/unique_id.h>

int main()
{
    uint8_t *ip_address;
    stdio_init_all();

    // Initialise the Wi-Fi chip
    if (cyw43_arch_init())
    {
        printf("Wi-Fi init failed\n");
        return -1;
    }

    // Enable wifi station
    cyw43_arch_enable_sta_mode();

    printf("Connecting to Wi-Fi...\n");
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000))
    {
        printf("failed to connect.\n");
        return 1;
    }
    else
    {
        printf("Connected.\n");
        // Read the ip address in a human readable way
        uint8_t *ip_address = (uint8_t *)&(cyw43_state.netif[0].ip_addr.addr);
        printf("IP address %d.%d.%d.%d\n", ip_address[0], ip_address[1], ip_address[2], ip_address[3]);
    }

#ifdef SNTP_SERVER
    // Initialise SNTP client
    ip_addr_t sntp_ip;
    ip4_addr_set_u32(&sntp_ip, ipaddr_addr(SNTP_SERVER));
    ip_address = (uint8_t *)&(sntp_ip.addr);
    printf("SNTP address %d.%d.%d.%d\n", ip_address[0], ip_address[1], ip_address[2], ip_address[3]);

    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setserver(0, &sntp_ip);
    sntp_init();
#endif
    
    // Initialise MQTT client
    ip_addr_t broker_ip;
    ip4_addr_set_u32(&broker_ip, ipaddr_addr(MQTT_BROKER));
    ip_address = (uint8_t *)&(broker_ip.addr);
    printf("MQTT address %d.%d.%d.%d\n", ip_address[0], ip_address[1], ip_address[2], ip_address[3]);

    mqtt_init(&broker_ip);

    struct ha_discovery discovery = {
        .device = &(struct ha_discovery_device){
            .name = "Pico Relay",
            .identifiers = malloc(2 * PICO_UNIQUE_BOARD_ID_SIZE_BYTES + 1),
        },
        .name = "Pico Relay",
        .device_class = "water",
        .platform = "valve",
        .availability_topic = "home/availability",
    };

    pico_get_unique_board_id_string(discovery.device->identifiers, sizeof(discovery.device->identifiers));

    char* json_buffer = malloc(512);
    if (json_buffer == NULL)
    {
        printf("Failed to allocate memory for JSON buffer\n");
        return -1;
    }
    char* json_ptr = ha_discovery_json(json_buffer, &discovery);
    printf("JSON: %s\n", json_buffer);

    mqtt_pub(MQTT_CONFIG_TOPIC, json_buffer);

    free(discovery.device->identifiers);
    free(json_buffer);

    while (true)
    {
        sleep_ms(1000);
    }
}
