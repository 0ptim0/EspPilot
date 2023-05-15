#include "Mpu6050.hpp"

#include <driver/i2c.h>
#include <esp_log.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "Mpu6050Reg.hpp"

Mpu6050::Mpu6050()
    : i2c_num(I2C_NUM_1),
      period(10 / portTICK_PERIOD_MS),
      address(ADDR[0]),
      timeout(20 / portTICK_PERIOD_MS) {
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = 21;
    conf.scl_io_num = 22;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = 100000;
}

Mpu6050::Mpu6050(uint32_t period)
    : i2c_num(I2C_NUM_1),
      period(period / portTICK_PERIOD_MS),
      address(ADDR[0]),
      timeout(20 / portTICK_PERIOD_MS) {
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = 21;
    conf.scl_io_num = 22;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = 100000;
}

Mpu6050::Mpu6050(uint32_t i2c_num, i2c_config_t &conf, uint32_t period)
    : i2c_num(i2c_num),
      conf(conf),
      period(period / portTICK_PERIOD_MS),
      address(ADDR[0]),
      timeout(20 / portTICK_PERIOD_MS) {}

bool Mpu6050::init() {
    if (i2c_param_config(i2c_num, &conf)) {
        ESP_LOGE(name, "Interface initialization failed");
        return false;
    }
    if (i2c_driver_install(i2c_num, I2C_MODE_MASTER, 0, 0, 0)) {
        ESP_LOGE(name, "Interface installation failed");
        return false;
    }
    reset();
    if (lookUpDevice()) {
        ESP_LOGE(name, "Device was not found");
        return false;
    }
    mutex = xSemaphoreCreateMutex();
    if (mutex == nullptr) {
        return false;
    }
    accel_scale = (9.81 * 2) / (32768.0);
    accel_bias = 0.0;
    gyro_scale = (250.0) / (32768.0);
    gyro_bias = 0.0;
    temp_scale = 1.0 / 340.0;
    temp_bias = 36.53;
    return true;
}

void Mpu6050::start() {
    xTaskCreatePinnedToCore(Mpu6050::update_thread, name, 4096, this, 1, NULL,
                            0);
}

void Mpu6050::update() {
    Meas meas = {};
    readReg(ACCEL_XOUT_H, reinterpret_cast<uint8_t *>(&meas), 14);
    xSemaphoreTake(mutex, portMAX_DELAY);
    int16_t ax = meas.ACCEL_XOUT_H << 8 | meas.ACCEL_XOUT_L;
    int16_t ay = meas.ACCEL_YOUT_H << 8 | meas.ACCEL_YOUT_L;
    int16_t az = meas.ACCEL_ZOUT_H << 8 | meas.ACCEL_ZOUT_L;
    int16_t temp = meas.TEMP_OUT_H << 8 | meas.TEMP_OUT_L;
    int16_t wx = meas.GYRO_XOUT_H << 8 | meas.GYRO_XOUT_L;
    int16_t wy = meas.GYRO_YOUT_H << 8 | meas.GYRO_YOUT_L;
    int16_t wz = meas.GYRO_ZOUT_H << 8 | meas.GYRO_ZOUT_L;
    m_ax = ax * accel_scale + accel_bias;
    m_ay = ay * accel_scale + accel_bias;
    m_az = az * accel_scale + accel_bias;
    m_temp = temp * temp_scale + temp_bias;
    m_wx = wx * gyro_scale + gyro_bias;
    m_wy = wy * gyro_scale + gyro_bias;
    m_wz = wz * gyro_scale + gyro_bias;
    xSemaphoreGive(mutex);
}

// Getters
uint32_t Mpu6050::getPeriod() {
    return period;
}

// Private
void Mpu6050::update_thread(void *arg) {
    Mpu6050 *m = static_cast<Mpu6050 *>(arg);
    TickType_t xLastWakeTime = xTaskGetTickCount();
    while (1) {
        m->update();
        const uint32_t period = m->getPeriod();
        xTaskDelayUntil(&xLastWakeTime, period);
    }
}

void Mpu6050::reset() {
    writeReg(PWM_MGMT_1, PWM_MGMT_1_RESET);
    vTaskDelay(50 / portTICK_PERIOD_MS);
    writeReg(PWM_MGMT_1, 0x03);
    uint8_t pwr_mgmt_2 = readReg(PWM_MGMT_2);
    pwr_mgmt_2 &= ~(PWM_MGMT_2_STBY_A | PWM_MGMT_2_STBY_G);
    writeReg(PWM_MGMT_2, pwr_mgmt_2);
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
    uint8_t ibuf[1] = {reg};
    i2c_master_write_to_device(i2c_num, address, ibuf, 1, timeout);
    i2c_master_read_from_device(i2c_num, address, ibuf, 1, timeout);
    return ibuf[0];
}

uint8_t Mpu6050::readReg(uint8_t reg, uint8_t *buf, uint16_t len) {
    uint8_t ibuf[1] = {reg};
    i2c_master_write_to_device(i2c_num, address, ibuf, 1, timeout);
    i2c_master_read_from_device(i2c_num, address, buf, len, timeout);
    return buf[0];
}

int Mpu6050::writeReg(uint8_t reg, uint8_t value) {
    uint8_t ibuf[2] = {reg, value};
    i2c_master_write_to_device(i2c_num, address, ibuf, sizeof(ibuf), timeout);
    return 0;
}

int Mpu6050::writeReg(uint8_t reg, uint8_t *buf, uint16_t len) {
    uint8_t ibuf[1] = {reg};
    i2c_master_write_to_device(i2c_num, address, ibuf, sizeof(ibuf), timeout);
    i2c_master_write_to_device(i2c_num, address, buf, len, timeout);
    return 0;
}
