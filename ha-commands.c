#include "pico_relay.h"
#include "home-assistant.h"

extern struct pico_relay_t *pico_relay;

/**
 * @ingroup home-assistant-commands
 * Home Assistant discovery message for a relay device.
 * @param buffer Pointer to the buffer to store the JSON message.
 * @return Length of the JSON message or -1 on error.
 */
int ha_discovery_msg(char* buffer)
{
    if (buffer == NULL)
    {
        printf("Failed to allocate memory for JSON buffer\n");
        return -1;
    }

    if (pico_relay == NULL)
    {
        printf("Application state is not initialized\n");
        return -1;
    }

    struct ha_discovery discovery = {
        .device = &(struct ha_discovery_device){
            .name = "Pico Relay",
            .identifiers = pico_relay->uuid,
        },
        .unique_id = pico_relay->uuid,
        .device_class = "water",
        .platform = "valve",
        .availability_topic = MQTT_TOPIC_ROOT "/availability",
        .command_topic = MQTT_TOPIC_ROOT "/set",
        .state_topic = MQTT_TOPIC_ROOT "/state",
    };

    return ha_discovery_json(buffer, &discovery) - buffer;
}
