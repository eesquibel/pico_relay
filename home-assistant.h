#ifndef __HOME_ASSISTANT_H
#define __HOME_ASSISTANT_H

struct ha_discovery_device {
    char *name;
    char *identifiers;
};

struct ha_discovery {
    struct ha_discovery_device *device;
    char *unique_id;
    char *device_class;
    char *platform;
    char *availability_topic;
    char *command_topic;
    char *state_topic;
};

char* ha_discovery_device_json(char* dest, struct ha_discovery_device const* device);
char *ha_discovery_json(char *dest, struct ha_discovery const *discovery);

#endif
