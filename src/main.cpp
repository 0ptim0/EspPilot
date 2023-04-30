#include "mpu6050.hpp"

extern "C" {

void app_main(void) {
    Mpu6050 mpu6050;
    mpu6050.init();
}

}
