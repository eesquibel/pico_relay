#ifndef __MQTT_H
#define __MQTT_H

#include "lwip/apps/mqtt.h"
#include "lwip/apps/mqtt_priv.h"

void mqtt_init(ip_addr_t *ip_addr);

void mqtt_pub(const char *topic, const char *message);

#endif
