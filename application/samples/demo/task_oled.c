#include "main.h"

#define I2C_MASTER_ADDR 0x0
#define I2C_SET_BAUDRATE 400000
#define I2C_MASTER_PIN_MODE PIN_MODE_2
#define I2C_TASK_DURATION_MS 20

#define PIN_I2C1_SCL GPIO_15
#define PIN_I2C1_SDA GPIO_16

static void oled_i2c_init_pin(void)
{
    /* I2C pinmux. */
    uapi_pin_set_mode(PIN_I2C1_SCL, I2C_MASTER_PIN_MODE);
    uapi_pin_set_mode(PIN_I2C1_SDA, I2C_MASTER_PIN_MODE);
    uapi_pin_set_pull(PIN_I2C1_SCL, PIN_PULL_TYPE_UP);
    uapi_pin_set_pull(PIN_I2C1_SDA, PIN_PULL_TYPE_UP);
}

uint8_t cur_x = 0;
uint8_t cur_y = 0;
void *task_oled(const char *arg)
{
    unused(arg);

    uint32_t baudrate = I2C_SET_BAUDRATE;
    uint8_t hscode = I2C_MASTER_ADDR;

    /* I2C master init config. */

    oled_i2c_init_pin();
    uapi_i2c_master_init(1, baudrate, hscode);

    ssd1306_Init();
    ssd1306_Fill(Black);

    while (1)
    {
        osal_msleep(1000 / 30);
        ssd1306_SetCursor(cur_x, cur_y);
        ssd1306_ClearOLED();
        ssd1306_DrawString("H3863 PMbus", Font_7x10, White);
        ssd1306_UpdateScreen();
    }
    return NULL;
}