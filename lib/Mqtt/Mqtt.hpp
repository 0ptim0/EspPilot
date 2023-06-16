#ifndef MQTT
#define MQTT

#include "mqtt5_client.h"

class Mqtt {
private:
    esp_mqtt_client_config_t m_cfg;
    esp_mqtt_client_handle_t m_client;

public:
    static const char *tag;

public:
    ssize_t write(void *data, size_t size);

public:
    Mqtt();
    bool init();
};

#endif // MQTT
