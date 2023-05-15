#include "lwip/sockets.h"
#include <stdint.h>

class Udp {
public:
    Udp(const char *ip_out, const char *port_out, const char *ip_in,
        const char *port_in);
    bool init();
    ssize_t read(void *data, size_t size);
    ssize_t write(void *data, size_t size);

public:
    static const char *tag;

private:
    bool m_init;
    int m_sock_rx;
    int m_sock_tx;
    const char *m_ip_out;
    const char *m_port_out;
    const char *m_ip_in;
    const char *m_port_in;
    struct sockaddr_in m_addr_rx;
    struct sockaddr_in m_addr_tx;

public:
    static void udp_server_task(void *arg);
};
