#ifndef MPU6050_HPP
#define MPU6050_HPP

#include <driver/i2c.h>
#include <stdint.h>

class Mpu6050 {
public:
    Mpu6050();
    bool init();

private:
    int lookUpDevice();
    uint8_t readReg(uint8_t reg);

private:
    uint8_t address;
    uint8_t i2c_num;
    uint32_t timeout;
    const char* name = "MPU6050";
    i2c_config_t conf;
};

#endif
