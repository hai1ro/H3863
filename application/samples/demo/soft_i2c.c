#include "main.h"
#include "soft_i2c.h"

// 定义自定义 TAG
#define TAG "[SOFT_IIC] "

static void LOGGG(const char *format, ...) {

    char buffer[1024];
    
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    // 打印 TAG 和格式化后的字符串
    osal_printk("%s%s\r\n", TAG, buffer);
}

/**
 * @brief Start signal
 *
 * @param [in] soft_i2c_t *i2c
 *
 * @return 1
 */
static uint8_t i2cStart(soft_i2c_t *i2c)
{
    i2c->sda_out(SOFT_I2C_GPIO_H);
    i2c->delay();
    i2c->scl_out(SOFT_I2C_GPIO_H);
    i2c->delay();
    i2c->sda_out(SOFT_I2C_GPIO_L);
    i2c->delay();
    i2c->scl_out(SOFT_I2C_GPIO_L);
    i2c->delay();

    return 1;
}

/**
 * @brief Stop signal
 *
 * @param [in] soft_i2c_t *i2c
 *
 * @return

 */
static void i2cStop(soft_i2c_t *i2c)
{
    i2c->scl_out(SOFT_I2C_GPIO_L);
    i2c->delay();
    i2c->sda_out(SOFT_I2C_GPIO_L);
    i2c->delay();
    i2c->scl_out(SOFT_I2C_GPIO_H);
    i2c->delay();
    i2c->sda_out(SOFT_I2C_GPIO_H);
    i2c->delay();
}

/**
 * @brief Ack signal
 *
 * @param [in] soft_i2c_t *i2c
 *
 * @return

 */
static void i2cAck(soft_i2c_t *i2c)
{
    i2c->scl_out(SOFT_I2C_GPIO_L);
    i2c->delay();
    i2c->sda_out(SOFT_I2C_GPIO_L);
    i2c->delay();
    i2c->scl_out(SOFT_I2C_GPIO_H);
    i2c->delay();
}

/**
 * @brief No ack signal
 *
 * @param [in] soft_i2c_t *i2c
 *
 * @return

 */
static void i2cNack(soft_i2c_t *i2c)
{
    i2c->scl_out(SOFT_I2C_GPIO_L);
    i2c->delay();
    i2c->sda_out(SOFT_I2C_GPIO_H);
    i2c->delay();
    i2c->scl_out(SOFT_I2C_GPIO_H);
    i2c->delay();
}

// 返回为:
/**
 * @brief Wait ack
 *
 * @param [in] soft_i2c_t *i2c
 *
 * @return =1 ACK, =0 NACK
 */
static uint8_t i2cWaitAck(soft_i2c_t *i2c)
{
    uint8_t ret;

    i2c->scl_out(SOFT_I2C_GPIO_L);
    // i2c->delay();
    i2c->sda_out(SOFT_I2C_GPIO_H);
    i2c->delay();
    i2c->scl_out(SOFT_I2C_GPIO_H);
    i2c->delay();

    i2c->sda_dir(SOFT_I2C_DIR_IN);
    if (i2c->sda_in())
    {
        ret = 0;
    }
    else
    {
        ret = 1;
    }
    i2c->sda_dir(SOFT_I2C_DIR_OUT);
    i2c->scl_out(SOFT_I2C_GPIO_L);

    return ret;
}

/**
 * @brief Write 1 byte
 *
 * @param [in] soft_i2c_t *i2c
 * @param [in] uint8_t byte: Data from high to low
 *
 * @return =1 ACK, =0 NACK
 */
static uint8_t i2cWriteByte(soft_i2c_t *i2c, uint8_t byte)
{
    uint8_t i = 8;

    while (i-- > 0)
    {
        i2c->scl_out(SOFT_I2C_GPIO_L);
        i2c->delay();
        if (byte & 0x80)
        {
            i2c->sda_out(SOFT_I2C_GPIO_H);
        }
        else
        {
            i2c->sda_out(SOFT_I2C_GPIO_L);
        }
        byte <<= 1;
        i2c->delay();
        i2c->scl_out(SOFT_I2C_GPIO_H);
        i2c->delay();
    }
    i2c->scl_out(SOFT_I2C_GPIO_L);

    return i2cWaitAck(i2c);
}

/**
 * @brief Read 1 byte
 *
 * @param [in] soft_i2c_t *i2c
 * @param [in] uint8_t ack: =1 send ACK, =0 send nACK
 *
 * @return read byte
 */
static uint8_t i2cReadByte(soft_i2c_t *i2c, uint8_t ack)
{
    uint8_t i = 8;
    uint8_t read_byte = 0;

    i2c->scl_out(SOFT_I2C_GPIO_L);
    i2c->sda_dir(SOFT_I2C_DIR_IN);
    while (i-- > 0)
    {
        read_byte <<= 1;
        i2c->scl_out(SOFT_I2C_GPIO_L);
        i2c->delay();
        i2c->scl_out(SOFT_I2C_GPIO_H);
        i2c->delay();
        if (i2c->sda_in())
        {
            read_byte |= 0x01;
        }
    }

    /* 切换SDA方向前，必须先把SCL拉低！！！因为SDA切换为输出，变为低电平，导致从机误检测到：开始信号 */
    i2c->scl_out(SOFT_I2C_GPIO_L);
    i2c->sda_dir(SOFT_I2C_DIR_OUT);

    if (!ack)
    {
        i2cNack(i2c);
    }
    else
    {
        i2cAck(i2c);
    }
    return read_byte;
}

void SoftI2cInit(soft_i2c_t *i2c)
{
    i2c->sda_dir(SOFT_I2C_DIR_OUT);

    i2c->scl_out(SOFT_I2C_GPIO_H);
    i2c->sda_out(SOFT_I2C_GPIO_H);
}

int32_t SoftI2cWrite(soft_i2c_t *i2c, uint8_t addr, uint8_t *data, uint32_t datalen)
{
    int32_t writelen = 0;

    /* 地址默认是7位，bit0: 0:表示主机写 */
    /* 流程:先写入从机地址，后面是连续写入数据给从机 */
    addr = (addr << 1) | 0x00;
    i2cStart(i2c);

    if (i2cWriteByte(i2c, addr))
    {
        while (datalen--)
        {
            writelen++;
            if (!i2cWriteByte(i2c, *data++))
            {
                break;
            }
        }
    }
    i2cStop(i2c);
    // LOGGG("writelen %d",writelen);
    return writelen;
}

int32_t SoftI2cRead(soft_i2c_t *i2c, uint8_t addr, uint8_t *data, uint32_t datalen)
{
    int32_t readlen = 0;

    /* 地址默认是7位，bit0: 1:表示主机读 */
    /* 流程:先写入从机地址，后面是连续从 从机读取数据 */
    addr = (addr << 1) | 0x01;
    i2cStart(i2c);
    if (i2cWriteByte(i2c, addr))
    {
        while (datalen--)
        {
            if (datalen == 0)
            {
                *data = i2cReadByte(i2c, 0);
                LOGGG("%c",*data);
            }
            else
            {
                *data = i2cReadByte(i2c, 1);
                LOGGG("%c",*data);
            }
            data++;
            readlen++;
        }
    }
    i2cStop(i2c);
    // LOGGG("readlen %d",readlen);
    return readlen;
}

int32_t SoftI2cWriteAndRead(soft_i2c_t *i2c, uint8_t addr,
                            uint8_t *wdata, uint32_t wdatalen,
                            uint8_t *rdata, uint32_t rdatalen)
{
    uint8_t waddr = (addr << 1) | 0x00;

    i2cStart(i2c);
    if (!i2cWriteByte(i2c, waddr))
    {
        goto ERROR;
    }

    while (wdatalen--)
    {
        if (!i2cWriteByte(i2c, *wdata++))
        {
            break;
        }
    }

    if (rdatalen == 0)
    {
        goto ERROR;
    }

    return SoftI2cRead(i2c, addr, rdata, rdatalen);
ERROR:
    i2cStop(i2c);

    return 0;
}
