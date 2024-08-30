#include "main.h"
#include "soft_i2c.h"
#include "csps.h"

#define PIN_CSPS_SCL GPIO_14
#define PIN_CSPS_SDA GPIO_13

// 定义自定义 TAG
#define TAG "[CSPS_TASK] "

static void LOGGG(const char *format, ...)
{

    char buffer[1024];

    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    // 打印 TAG 和格式化后的字符串
    osal_printk("%s%s\r\n", TAG, buffer);
}

static soft_i2c_t soft_i2c;

static void _i2c_delay(void)
{
    uint32_t i = 50;
    while (i > 0)
        i--;
}

static void _i2c_scl_out(uint8_t value)
{
    uapi_gpio_set_val(PIN_CSPS_SCL, value == 1 ? GPIO_LEVEL_HIGH : GPIO_LEVEL_LOW);
}

static void _i2c_sda_dir(uint8_t value)
{
    uapi_gpio_set_dir(PIN_CSPS_SDA, value == 1 ? GPIO_DIRECTION_OUTPUT : GPIO_DIRECTION_INPUT);
}

static void _i2c_sda_out(uint8_t value)
{
    uapi_gpio_set_val(PIN_CSPS_SDA, value == 1 ? GPIO_LEVEL_HIGH : GPIO_LEVEL_LOW);
}

static uint8_t _i2c_sda_in(void)
{
    return uapi_gpio_get_val(PIN_CSPS_SDA);
}

static void _i2c_write_byte(uint8_t addr, uint8_t *data, uint32_t datalen)
{
    SoftI2cWrite(&soft_i2c, addr, data, datalen);
    return;
}
static void _i2c_read_byte(uint8_t addr, uint8_t *data, uint32_t datalen)
{
    SoftI2cRead(&soft_i2c, addr, data, datalen);
    return;
}

void *task_csps(const char *arg)
{
    unused(arg);

    // 软件i2c初始化
    LOGGG("soft_i2c init");
    soft_i2c.delay = _i2c_delay;
    soft_i2c.scl_out = _i2c_scl_out;
    soft_i2c.sda_dir = _i2c_sda_dir;
    soft_i2c.sda_out = _i2c_sda_out;
    soft_i2c.sda_in = _i2c_sda_in;
    SoftI2cInit(&soft_i2c);
    // CSPS初始化
    LOGGG("csps init");
    csps_t csps;
    csps_init(&csps, 0x5E, 0x56, _i2c_read_byte, _i2c_write_byte);

    while (1)
    {
        // osal_printk("[%d]Hello World  BearPi\r\n",osKernelGetTickCount());
        osal_msleep(500);
        LOGGG("START TEST");
        LOGGG("Power Name: %s", csps.getName());
    }

    return NULL;
}
