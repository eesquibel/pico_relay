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
#include "pico_led.h"
#include "mqtt.h"
#include "home-assistant.h"
#include "ha-commands.h"

struct pico_relay_t pico_relay_instance = {
    .is_connected = false,
    .discovery_sent = false,
    .relay_gpio = RELAY_GPIO,
    .state = false,
};

struct pico_relay_t *pico_relay = &pico_relay_instance;

void pico_relay_on_connected()
{
    if (pico_relay->is_connected && !pico_relay->discovery_sent)
    {
        char discovery_msg[512];
        int len = ha_discovery_msg(discovery_msg);
        if (len > 0)
        {
            mqtt_pub(MQTT_TOPIC_ROOT "/config", discovery_msg);
            pico_relay->discovery_sent = true;
        }
    }

    if (pico_relay->is_connected)
    {
        mqtt_pub(MQTT_TOPIC_ROOT "/availability", "online");
        mqtt_pub(MQTT_TOPIC_ROOT "/state", pico_relay->state ? "open" : "closed");
    }
}

void pico_relay_on_command(char *payload)
{
    if (strcmp(payload, "OPEN") == 0)
    {
        pico_relay->state = true;
    }
    else if (strcmp(payload, "CLOSE") == 0)
    {
        pico_relay->state = false;
    }
    
    gpio_put(RELAY_GPIO, pico_relay->state);
    pico_set_led(pico_relay->state);
    mqtt_pub(MQTT_TOPIC_ROOT "/state", pico_relay->state ? "open" : "closed");
}

int main()
{
    uint8_t *ip_address;
    stdio_init_all();

    // Get the unique ID of the board
    pico_get_unique_board_id_string(pico_relay->uuid, sizeof(pico_relay_instance.uuid));

    // Initialise the Wi-Fi chip
    if (cyw43_arch_init())
    {
        pico_set_led(true);
        printf("Wi-Fi init failed\n");
        return -1;
    }

    pico_set_led(false);
    
    gpio_init(RELAY_GPIO);
    gpio_set_dir(RELAY_GPIO, GPIO_OUT);
    gpio_put(RELAY_GPIO, false);
    pico_relay->state = false;

    // Enable wifi station
    cyw43_arch_enable_sta_mode();

    printf("Connecting to Wi-Fi...\n");
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000))
    {
        pico_set_led(true);
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

    while (true)
    {
        sleep_ms(60 * 1000);
        gpio_put(RELAY_GPIO, pico_relay->state);
        if (pico_relay->is_connected)
        {
            mqtt_pub(MQTT_TOPIC_ROOT "/availability", "online");
            mqtt_pub(MQTT_TOPIC_ROOT "/state", pico_relay->state ? "open" : "closed");
        }
        else
        {
            mqtt_init(&broker_ip);
        }
    }
}
