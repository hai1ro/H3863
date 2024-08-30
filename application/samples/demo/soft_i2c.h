#ifndef __SOFT_I2C_H__
#define __SOFT_I2C_H__

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#include <stdint.h>

#define SOFT_I2C_GPIO_H 1
#define SOFT_I2C_GPIO_L 0
#define SOFT_I2C_DIR_OUT 1
#define SOFT_I2C_DIR_IN 0

    typedef void (*i2c_delay_func_t)(void);
    typedef void (*i2c_scl_out_func_t)(uint8_t value);
    typedef void (*i2c_sda_dir_func_t)(uint8_t value);
    typedef void (*i2c_sda_out_func_t)(uint8_t value);
    typedef uint8_t (*i2c_sda_in_func_t)(void);

    typedef struct
    {
        i2c_delay_func_t delay;
        i2c_scl_out_func_t scl_out;
        i2c_sda_dir_func_t sda_dir;
        i2c_sda_out_func_t sda_out;
        i2c_sda_in_func_t sda_in;
    } soft_i2c_t;

    void SoftI2cInit(soft_i2c_t *i2c);

    int32_t SoftI2cWrite(soft_i2c_t *i2c, uint8_t addr, uint8_t *data, uint32_t datalen);

    int32_t SoftI2cRead(soft_i2c_t *i2c, uint8_t addr, uint8_t *data, uint32_t datalen);

    int32_t SoftI2cWriteAndRead(soft_i2c_t *i2c, uint8_t addr,
                                uint8_t *wdata, uint32_t wdatalen,
                                uint8_t *rdata, uint32_t rdatalen);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __SOFT_I2C_H__
