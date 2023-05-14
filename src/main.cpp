#include <esp_log.h>
#include "mpu6050.hpp"

extern "C" {

void app_main(void) {
    i2c_config_t i2c_conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = 21,
        .scl_io_num = 22,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master = {
            .clk_speed = 400000,
        },
        .clk_flags = I2C_SCLK_SRC_FLAG_FOR_NOMAL
    };
    Mpu6050 mpu6050(1, i2c_conf, 1000);
    mpu6050.init();
    mpu6050.start();
    while (1) {
        vTaskDelay(1000);
    }
}

}
