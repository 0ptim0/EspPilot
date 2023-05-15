#include <sys/param.h>

#include "esp_event.h"
#include "freertos/event_groups.h"

class WifiStation {
public:
    WifiStation(const char *ssid, const char *password);
    void init();

public:
    static EventGroupHandle_t s_wifi_event_group;
    static const char *tag;
    static void event_handler(void *arg, esp_event_base_t event_base,
                              int32_t event_id, void *event_data);
    static const int max_retry;
    static int retry_num;

private:
    const char *ssid;
    const char *password;
};
