#include "csps.h"

// 定义自定义 TAG
#define TAG "[CSPS_HAL] "

static void LOGGG(const char *format, ...) {

    char buffer[1024];
    
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    // 打印 TAG 和格式化后的字符串
    osal_printk("%s%s\r\n", TAG, buffer);
}

static csps_t *csps_g;

static uint32_t readCSPSword_Impl(uint8_t dataAddr)
{
    uint8_t regCS = ((0xFF - (dataAddr + (csps_g->i2c_addr << 1))) + 1) & 0xFF;
    uint32_t rec = 0xFFFF;
    uint8_t buff_w[2];
    buff_w[0] = dataAddr;
    buff_w[1] = regCS;
    csps_g->i2c_write_byte(csps_g->i2c_addr, buff_w, 2);
    uint8_t buff_r[3];
    csps_g->i2c_read_byte(csps_g->i2c_addr, buff_r, 3);
    rec = buff_r[0];
    rec |= buff_r[1] << 8;
    rec |= buff_r[2] << 16;
    return rec;
}

static uint8_t *getROM_Impl(uint8_t addr, uint8_t len)
{
    // 为字符串分配内存（len + 1 是因为要包含字符串的终止符 '\0'）
    LOGGG("getROM_Impl addr %x len %d",addr,len);
    uint8_t *rec = (uint8_t *)malloc(len + 1);
    if (rec == NULL)
    {
        // 如果内存分配失败，返回 NULL
        LOGGG("getROM_Impl malloc fail");
        return NULL;
    }
    // 初始化字符数组
    for (uint8_t n = 0; n < len; n++)
    {
        csps_g->i2c_write_byte(csps_g->rom_addr, &addr, 1);
        addr++;
        csps_g->i2c_read_byte(csps_g->rom_addr, &rec[n], 1);
    }
    // 终止字符串
    rec[len] = '\0';
    return rec;
}

static uint8_t *getName_Impl(void)
{
    return csps_g->getROM(0x32, 0x1A);
}

void csps_init(csps_t *csps, uint8_t i2c_addr, uint8_t rom_addr, i2c_read_byte_func_t read_func, i2c_write_byte_func_t write_func)
{
    csps_g = csps;
    csps_g->i2c_addr = i2c_addr;
    csps_g->rom_addr = rom_addr;
    
    csps_g->i2c_read_byte = read_func;
    csps_g->i2c_write_byte = write_func;
    // 设置内部函数指针
    csps_g->readCSPSword = readCSPSword_Impl;
    csps_g->getROM = getROM_Impl;
    csps_g->getName = getName_Impl;
}
