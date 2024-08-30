#ifndef __CSPS_H__
#define __CSPS_H__

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"
#include "soft_i2c.h"

    typedef void (*i2c_read_byte_func_t)(uint8_t addr, uint8_t *data, uint32_t datalen);
    typedef void (*i2c_write_byte_func_t)(uint8_t addr, uint8_t *data, uint32_t datalen);
    typedef uint32_t (*readCSPSword_func_t)(uint8_t dataAddr);
    typedef uint8_t *(*getROM_func_t)(uint8_t addr, uint8_t len);
    typedef uint8_t *(*getName_func_t)(void);
    typedef struct
    {
        uint8_t i2c_addr;
        uint8_t rom_addr;
        i2c_read_byte_func_t i2c_read_byte;
        i2c_write_byte_func_t i2c_write_byte;
        readCSPSword_func_t readCSPSword;
        getROM_func_t getROM;
        getName_func_t getName;
    } csps_t;

    void csps_init(csps_t *csps, uint8_t i2c_addr, uint8_t rom_addr, i2c_read_byte_func_t read_func, i2c_write_byte_func_t write_func);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif