#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/apps/mqtt.h"
#include "lwip/apps/mqtt_priv.h"

#include "pico_relay.h"
#include "mqtt.h"

extern struct pico_relay_t *pico_relay;

static mqtt_client_t static_client;

struct topic_id_t {
    bool command;
};

static struct topic_id_t topic_id = {
    .command = false,
};

static void mqtt_incoming_publish_cb(void *arg, const char *topic, u32_t tot_len)
{
    printf("Incoming publish: topic=%s, total_len=%u\n", topic, tot_len);

    if (strcmp(topic, MQTT_TOPIC_ROOT "/set") == 0)
    {
        topic_id.command = true;
        printf("Command topic matched\n");
    }
    else
    {
        topic_id.command = false;
    }
}

static void mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags)
{
    printf("Incoming data: %.*s\n", len, data);

    if (topic_id.command)
    {
        char payload[len + 1];
        memcpy(payload, data, len);
        payload[len] = '\0';

        printf("Command received: %s\n", payload);

        pico_relay_on_command(payload);
    }
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
    struct pico_relay_t *pico_relay = (struct pico_relay_t *)arg;
    err_t err;

    if (status == MQTT_CONNECT_ACCEPTED)
    {
        pico_relay->is_connected = true;

        printf("MQTT connection accepted\n");

        mqtt_set_inpub_callback(client, mqtt_incoming_publish_cb, mqtt_incoming_data_cb, 0);

        err = mqtt_subscribe(client, MQTT_TOPIC_ROOT "/set", 1, mqtt_sub_request_cb, 0);

        if (err != ERR_OK)
        {
            printf("Subscription failed: %d\n", err);
            return;
        }
        else
        {
            printf("Subscribed to topic '%s'\n", MQTT_TOPIC_ROOT "/set");
        }
    }
    else
    {
        pico_relay->is_connected = false;
        printf("MQTT connection failed: %d\n", status);
        return;
    }

    pico_relay_on_connected();
}

void mqtt_connect(mqtt_client_t *client, ip_addr_t *ip_addr, u16_t port, const char *username, const char *password)
{
    struct mqtt_connect_client_info_t ci;
    err_t err;

    memset(&ci, 0, sizeof(ci));

    char client_id[18 + strlen(pico_relay->uuid) + 1];
    memset(client_id, 0, sizeof(client_id));

    sprintf(client_id, "pico_relay_client_%s", pico_relay->uuid);

    ci.client_id = client_id;
    ci.client_user = username;
    ci.client_pass = password;
    ci.keep_alive = 60;

    err = mqtt_client_connect(client, ip_addr, port, mqtt_connection_cb, pico_relay, &ci);

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

void mqtt_pub(const char *topic, const char *message)
{
    err_t err;

    if (topic == NULL || message == NULL)
    {
        printf("Invalid topic or message\n");
        return;
    }

    if (strlen(topic) == 0 || strlen(message) == 0)
    {
        printf("Topic or message cannot be empty\n");
        return;
    }

    printf("Publishing to topic '%s': %s\n", topic, message);

    err = mqtt_publish(&static_client, topic, message, strlen(message), 1, 0, NULL, 0);

    if (err != ERR_OK)
    {
        printf("mqtt_publish return %d\n", err);
    }
    else
    {
        printf("mqtt_publish success\n");
    }
}
