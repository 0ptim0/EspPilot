#include "Mqtt.hpp"

#include "esp_log.h"

const char *Mqtt::tag = "Mqtt";

Mqtt::Mqtt() : m_cfg({}) {
    m_cfg.broker.address.uri = "mqtt://mqtt-dashboard.com:1883";
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base,
                               int32_t event_id, void *event_data) {
    ESP_LOGD(Mqtt::tag,
             "Event dispatched from event loop base=%s, event_id=%" PRIi32 "",
             base, event_id);
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    switch ((esp_mqtt_event_id_t)event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(Mqtt::tag, "MQTT_EVENT_CONNECTED");
            msg_id = esp_mqtt_client_subscribe(client, "/optim000000/test1",
            0); ESP_LOGI(Mqtt::tag, "sent subscribe successful, msg_id=%d",
            msg_id);
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(Mqtt::tag, "MQTT_EVENT_DISCONNECTED");
            break;

        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(Mqtt::tag, "MQTT_EVENT_SUBSCRIBED, msg_id=%d",
                     event->msg_id);
            msg_id =
                esp_mqtt_client_publish(client, "/topic/qos0", "data", 0, 0, 0);
            ESP_LOGI(Mqtt::tag, "sent publish successful, msg_id=%d", msg_id);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(Mqtt::tag, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d",
                     event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(Mqtt::tag, "MQTT_EVENT_PUBLISHED, msg_id=%d",
                     event->msg_id);
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(Mqtt::tag, "MQTT_EVENT_DATA");
            printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
            printf("DATA=%.*s\r\n", event->data_len, event->data);
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(Mqtt::tag, "MQTT_EVENT_ERROR");
            break;
        default:
            ESP_LOGI(Mqtt::tag, "Other event id:%d", event->event_id);
            break;
    }
}

bool Mqtt::init() {
    m_client = esp_mqtt_client_init(&m_cfg);
    esp_mqtt_client_register_event(m_client, MQTT_EVENT_ANY, mqtt_event_handler,
                                   m_client);
    esp_mqtt_client_start(m_client);
    return true;
}

ssize_t Mqtt::write(void *data, size_t size) {
    int msg_id = esp_mqtt_client_publish(m_client, "/optim000000/test0",
                                         (const char *)data, size, 1, 0);
    ESP_LOGI(Mqtt::tag, "sent publish successful, msg_id=%d", msg_id);
    return size;
}
