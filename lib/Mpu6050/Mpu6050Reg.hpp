#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push, 1)
struct Meas {
    uint8_t ACCEL_XOUT_H;
    uint8_t ACCEL_XOUT_L;
    uint8_t ACCEL_YOUT_H;
    uint8_t ACCEL_YOUT_L;
    uint8_t ACCEL_ZOUT_H;
    uint8_t ACCEL_ZOUT_L;
    uint8_t TEMP_OUT_H;
    uint8_t TEMP_OUT_L;
    uint8_t GYRO_XOUT_H;
    uint8_t GYRO_XOUT_L;
    uint8_t GYRO_YOUT_H;
    uint8_t GYRO_YOUT_L;
    uint8_t GYRO_ZOUT_H;
    uint8_t GYRO_ZOUT_L;
};
#pragma pack(pop)

static constexpr uint8_t ADDR[2] = {0x68, 0x69};
static constexpr uint8_t WHO_AM_I_DEFAULT_VALUE = 0x68;

static constexpr uint8_t USER_CTRL = 0x6A;
static constexpr uint8_t USER_CTRL_FIFO_RESET = BIT(2);
static constexpr uint8_t USER_CTRL_FIFO_EN = BIT(6);

static constexpr uint8_t PWM_MGMT_1 = 0x6B;
static constexpr uint8_t PWM_MGMT_1_RESET = BIT(7);
static constexpr uint8_t PWM_MGMT_1_SLEEP = BIT(6);
static constexpr uint8_t PWM_MGMT_1_CYCLE = BIT(5);
static constexpr uint8_t PWM_MGMT_1_TEMP_DIS = BIT(3);
static constexpr uint8_t PWM_MGMT_1_CLKSEL = BIT(0);

static constexpr uint8_t PWM_MGMT_2 = 0x6C;
static constexpr uint8_t PWM_MGMT_2_STBY_A = BIT(3) | BIT(4) | BIT(5);
static constexpr uint8_t PWM_MGMT_2_STBY_G = BIT(0) | BIT(1) | BIT(2);

static constexpr uint8_t ACCEL_XOUT_H = 0x3B;

static constexpr uint8_t WHO_AM_I = 0x75;

#ifdef __cplusplus
}
#endif