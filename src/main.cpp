#include "Mpu6050.hpp"
#include "Mqtt.hpp"
#include "Udp.hpp"
#include "WifiStation.hpp"

extern "C" {

void main_thread(void *arg) {
    WifiStation wifi("iNet_07435", "12345678");
    // Udp udp("192.168.31.114", "20000", "0.0.0.0", "20001");
    Mqtt mqtt;
    Mpu6050 mpu6050(10);
    wifi.init();
    // udp.init();
    mpu6050.init();
    mpu6050.start();
    mqtt.init();
    while (1) {
        Mpu6050::Meas meas = mpu6050.getMeas();
        char str[128] = {};
        sprintf(str,
                "ax = %lf\n"
                "ay = %lf\n"
                "az = %lf\n"
                "wx = %lf\n"
                "wy = %lf\n"
                "wz = %lf\n"
                "temp = %lf\n",
                meas.ax, meas.ay, meas.az, meas.wx, meas.wy, meas.wz,
                meas.temp);
        mqtt.write((void *)str, strlen(str));
        vTaskDelay(100);
        // printf("%lf\n", meas.az);
        // udp.write(&meas, sizeof(meas));
    }
}

void app_main(void) {
    xTaskCreate(main_thread, "Main thread", 8192, NULL, 10, NULL);
}
}
