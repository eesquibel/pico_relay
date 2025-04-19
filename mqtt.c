#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/apps/mqtt.h"
#include "lwip/apps/mqtt_priv.h"

#include "mqtt.h"

mqtt_client_t static_client;

static void mqtt_incoming_publish_cb(void *arg, const char *topic, u32_t tot_len)
{
    printf("Incoming publish: topic=%s, total_len=%u\n", topic, tot_len);
}

static void mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags)
{
    printf("Incoming data: %.*s\n", len, data);
}

static void mqtt_sub_request_cb(void *arg, err_t err)
{
    if (err == ERR_OK)
    {
        printf("Subscription successful\n");
    }
    else
    {
        printf("Subscription failed: %d\n", err);
    }
}

static void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status)
{
    err_t err;

    if (status == MQTT_CONNECT_ACCEPTED)
    {
        printf("MQTT connection accepted\n");

        mqtt_set_inpub_callback(client, mqtt_incoming_publish_cb, mqtt_incoming_data_cb, 0);

        err = mqtt_subscribe(client, MQTT_SUBSCRIBE_TOPIC, 1, mqtt_sub_request_cb, 0);

        if (err != ERR_OK)
        {
            printf("Subscription failed: %d\n", err);
        }
        else
        {
            printf("Subscribed to topic '%s'\n", MQTT_SUBSCRIBE_TOPIC);
        }
    }
    else
    {
        printf("MQTT connection failed: %d\n", status);
    }
}

void mqtt_connect(mqtt_client_t *client, ip_addr_t *ip_addr, u16_t port, const char *username, const char *password)
{
    struct mqtt_connect_client_info_t ci;
    err_t err;

    memset(&ci, 0, sizeof(ci));

    ci.client_id = "pico_relay_client";
    ci.client_user = username;
    ci.client_pass = password;
    ci.keep_alive = 60;

    err = mqtt_client_connect(client, ip_addr, port, mqtt_connection_cb, 0, &ci);

    if (err != ERR_OK)
    {
        printf("mqtt_connect return %d\n", err);
        mqtt_init(ip_addr);
    }
    else
    {
        printf("mqtt_connect success\n");
    }
}

void mqtt_init(ip_addr_t *ip_addr)
{
    mqtt_connect(&static_client, ip_addr, MQTT_PORT, MQTT_USERNAME, MQTT_PASSWORD);
}
