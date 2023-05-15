#include "Mpu6050.hpp"
#include "Udp.hpp"
#include "WifiStation.hpp"

extern "C" {

void app_main(void) {
    WifiStation wifi("iNet_07435", "12345678");
    Udp udp("192.168.31.114", "20000", "0.0.0.0", "20001");
    i2c_config_t i2c_conf = {.mode = I2C_MODE_MASTER,
                             .sda_io_num = 21,
                             .scl_io_num = 22,
                             .sda_pullup_en = GPIO_PULLUP_ENABLE,
                             .scl_pullup_en = GPIO_PULLUP_ENABLE,
                             .master =
                                 {
                                     .clk_speed = 400000,
                                 },
                             .clk_flags = I2C_SCLK_SRC_FLAG_FOR_NOMAL};
    Mpu6050 mpu6050(1, i2c_conf, 1000);
    wifi.init();
    udp.init();
    mpu6050.init();
    mpu6050.start();
    uint8_t buf[1024];
    while (1) {
        ssize_t l = udp.read(buf, 1024);
        if (l > 0) {
            udp.write(buf, l);
        }
    }
}
}
