#include "Mpu6050.hpp"
#include "Udp.hpp"
#include "WifiStation.hpp"

extern "C" {

void main_thread(void *arg) {
    WifiStation wifi("iNet_07435", "12345678");
    Udp udp("192.168.31.114", "20000", "0.0.0.0", "20001");
    Mpu6050 mpu6050(10);
    wifi.init();
    udp.init();
    mpu6050.init();
    mpu6050.start();
    while (1) {
        Mpu6050::Meas meas = mpu6050.getMeas();
        udp.write(&meas, sizeof(meas));
    }
}

void app_main(void) {
    xTaskCreate(main_thread, "Main thread", 4096, NULL, 10, NULL);
}
}
