#include "mpu6050.hpp"

#include <driver/i2c.h>
#include <esp_log.h>

#include "mpu6050_reg.hpp"

Mpu6050::Mpu6050()
    : address(ADDR[0]), i2c_num(I2C_NUM_0), timeout(10 / portTICK_PERIOD_MS) {
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = 21;
    conf.scl_io_num = 22;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = 400000;
}

bool Mpu6050::init() {
    if (i2c_param_config(i2c_num, &conf)) {
        ESP_LOGE(name, "Interface initialization failed");
        goto exit_fail;
    }
    if (i2c_driver_install(i2c_num, I2C_MODE_MASTER, 0, 0, 0)) {
        ESP_LOGE(name, "Interface installation failed");
        goto exit_fail;
    }
    if (lookUpDevice()) {
        ESP_LOGE(name, "Device was not found");
        goto exit_fail;
    }
    return true;

exit_fail:
    return false;
}

int Mpu6050::lookUpDevice() {
    for (int i = 0; i < sizeof(ADDR); i++) {
        if (readReg(WHO_AM_I) == WHO_AM_I_DEFAULT_VALUE) {
            ESP_LOGI(name, "Device was found at address = 0x%X", ADDR[i]);
            return 0;
        }
    }
    return -1;
}

uint8_t Mpu6050::readReg(uint8_t reg) {
    uint8_t buf[1];
    i2c_master_write_to_device(i2c_num, address, &reg, 1, timeout);
    i2c_master_read_from_device(i2c_num, address, buf, 1, timeout);
    return buf[0];
}
