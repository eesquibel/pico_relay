#include "json-maker/json-maker.h"
#include "home-assistant.h"

char *ha_discovery_device_json(char *dest, struct ha_discovery_device const *device)
{
    dest = json_objOpen(dest, "device", NULL);
    dest = json_nstr(dest, "name", device->name, -1, NULL);
    dest = json_nstr(dest, "identifiers", device->identifiers, -1, NULL);
    dest = json_objClose(dest, NULL);
    return dest;
}

char *ha_discovery_json(char *dest, struct ha_discovery const *discovery)
{
    dest = json_objOpen(dest, NULL, NULL);
    dest = ha_discovery_device_json(dest, discovery->device);
    dest = json_nstr(dest, "name", discovery->name, -1, NULL);
    dest = json_nstr(dest, "device_class", discovery->device_class, -1, NULL);
    dest = json_nstr(dest, "platform", discovery->platform, -1, NULL);
    dest = json_nstr(dest, "availability_topic", discovery->availability_topic, -1, NULL);
    dest = json_objClose(dest, NULL);
    dest = json_end(dest, NULL);
    return dest;
}
