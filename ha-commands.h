
#ifndef __HA_COMMANDS_H
#define __HA_COMMANDS_H

/**
 * @ingroup home-assistant-commands
 * @brief Generates the Home Assistant discovery JSON message for the relay device.
 *
 * This function creates a JSON string formatted according to the Home Assistant
 * MQTT discovery protocol for a valve-type device. It uses the device's unique
 * ID stored in the global `pico_relay` structure.
 *
 * @param buffer Pointer to the character buffer where the JSON message will be stored.
 *               The buffer must be large enough to hold the generated message (e.g., 512 bytes).
 * @return The length of the generated JSON message written into the buffer,
 *         or -1 if an error occurred (e.g., NULL buffer or uninitialized state).
 */
int ha_discovery_msg(char* buffer);

#endif // __HA_COMMANDS_H
