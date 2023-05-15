#include "Udp.hpp"

#include <lwip/netdb.h>
#include <string.h>
#include <sys/param.h>

#include "esp_event.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "esp_netif.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "nvs_flash.h"

const char *Udp::tag = "Udp";

Udp::Udp(const char *ip_out, const char *port_out, const char *ip_in,
         const char *port_in)
    : m_ip_out(ip_out),
      m_port_out(port_out),
      m_ip_in(ip_in),
      m_port_in(port_in) {}

bool Udp::init() {
    uint16_t port_in = (uint16_t)atoi(m_port_in);
    uint16_t port_out = (uint16_t)atoi(m_port_out);

    m_sock_rx = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (m_sock_rx < 0) {
        ESP_LOGE(Udp::tag, "Unable to create rx socket: errno %d", errno);
        goto exit_fail;
    }

    m_sock_tx = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (m_sock_tx < 0) {
        ESP_LOGE(Udp::tag, "Unable to create tx socket: errno %d", errno);
        goto exit_fail;
    }

    uint32_t ip_out;
    if (!inet_pton(AF_INET, m_ip_out, &ip_out)) {
        ESP_LOGE(Udp::tag, "Failed to convert out ip address");
        goto exit_fail;
    }

    uint32_t ip_in;
    if (!inet_pton(AF_INET, m_ip_in, &ip_in)) {
        ESP_LOGE(Udp::tag, "Failed to convert in ip address");
        goto exit_fail;
    }

    m_addr_tx.sin_family = AF_INET;
    m_addr_tx.sin_port = htons(port_out);
    m_addr_tx.sin_addr.s_addr = ip_out;

    m_addr_rx.sin_family = AF_INET;
    m_addr_rx.sin_port = htons(port_in);
    m_addr_rx.sin_addr.s_addr = ip_in;

    if (bind(m_sock_rx, (struct sockaddr *)&m_addr_rx, sizeof(m_addr_rx))) {
        ESP_LOGE(Udp::tag, "Failed to bind rx socket: %s", strerror(errno));
        goto exit_fail;
    }

    ESP_LOGI(Udp::tag, "Sockets successfully opened");
    char ip[16];
    inet_ntop(AF_INET, &ip_in, ip, INET_ADDRSTRLEN);
    ESP_LOGI(Udp::tag, "Server: %s:%u", ip, port_in);
    inet_ntop(AF_INET, &ip_out, ip, INET_ADDRSTRLEN);
    ESP_LOGI(Udp::tag, "Client: %s:%u", ip, port_out);

    m_init = true;

    return true;

exit_fail:
    if (m_sock_rx > 0) {
        close(m_sock_rx);
    }
    if (m_sock_tx > 0) {
        close(m_sock_tx);
    }
    return false;
}

ssize_t Udp::read(void *data, size_t size) {
    if (!m_init) {
        return -1;
    }
    socklen_t addrlen = sizeof(m_addr_rx);
    ssize_t rv = recvfrom(m_sock_rx, data, size, 0,
                          (struct sockaddr *)&m_addr_rx, &addrlen);
    if (rv > 0) {
        ESP_LOGI(Udp::tag, "Received: %d", rv);
    } else {
        ESP_LOGE(Udp::tag, "Receive failed");
    }
    return rv;
}

ssize_t Udp::write(void *data, size_t size) {
    if (!m_init) {
        return -1;
    }
    ssize_t rv = sendto(m_sock_tx, data, size, 0, (struct sockaddr *)&m_addr_tx,
                        (socklen_t)sizeof(m_addr_tx));
    if (rv > 0) {
        ESP_LOGI(Udp::tag, "Sent: %d", rv);
    } else {
        ESP_LOGE(Udp::tag, "Send failed");
    }
    return rv;
}
