#ifndef MPU6050_HPP
#define MPU6050_HPP

#include <driver/i2c.h>
#include <stdint.h>

class Mpu6050 {
public:
    Mpu6050();
    Mpu6050(uint32_t i2c_num, i2c_config_t &conf, uint32_t period);
    bool init();
    void start();
    void update();

public:
    uint32_t getPeriod();
    double getAx();
    double getAy();
    double getAz();
    double getWx();
    double getWy();
    double getWz();
    double getT();

private:
    static void update_thread(void *arg);

private:
    void reset();
    int lookUpDevice();
    uint8_t readReg(uint8_t reg);
    uint8_t readReg(uint8_t reg, uint8_t *buf, uint16_t len);
    int writeReg(uint8_t reg, uint8_t *buf, uint16_t len);
    int writeReg(uint8_t reg, uint8_t value);

private:
    const char *name = "MPU6050";
    uint8_t i2c_num;
    i2c_config_t conf;
    const uint32_t period;
    uint8_t address;
    uint32_t timeout;
    SemaphoreHandle_t mutex;

private:
    double accel_scale;
    double gyro_scale;
    double temp_scale;
    double accel_bias;
    double gyro_bias;
    double temp_bias;

private:
    double m_ax;
    double m_ay;
    double m_az;
    double m_wx;
    double m_wy;
    double m_wz;
    double m_temp;
};

#endif
