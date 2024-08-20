#include "common_def.h"
#include "osal_debug.h"
#include "soc_osal.h"
#include "cmsis_os2.h"
#include "app_init.h"
#include "pinctrl.h"

#include "gpio.h"
#include "i2c.h"

#include "ssd1306_fonts.h"
#include "ssd1306.h"

#define DEAFULT_TASK_STACK_SIZE    0x1000
#define DEAFULT_TASK_PRIO          (osPriority_t)(17)


#define PIN_BLINKY 13
#define PIN_I2C1_SCL 15
#define PIN_I2C1_SDA 16
#define PIN_BUTTON_LEFT 0
#define PIN_BUTTON_UP 1
#define PIN_BUTTON_DOWN 2
#define PIN_BUTTON_RIGHT 3

#define I2C_MASTER_ADDR                   0x0
#define I2C_SET_BAUDRATE                  400000
#define I2C_MASTER_PIN_MODE               2
#define I2C_TASK_DURATION_MS              20

static uint8_t cur_x=0;
static uint8_t cur_y=0;

static void *main_task(const char *arg)
{
    unused(arg);
    uapi_pin_set_mode(PIN_BLINKY, PIN_MODE_0);
    uapi_gpio_set_dir(PIN_BLINKY, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(PIN_BLINKY, GPIO_LEVEL_LOW);

    while (1) {
        
        // osal_printk("[%d]Hello World  BearPi\r\n",osKernelGetTickCount());
        osal_msleep(500);
        uapi_gpio_toggle(PIN_BLINKY);
    }

    return NULL;
}

static void gpio_callback_func(pin_t pin, uintptr_t param)
{
    UNUSED(param);
    osal_printk("[%d]:",osKernelGetTickCount());
    switch (pin)
    {
        case 0: //key_left
            osal_printk("←");
            if(cur_x-10>=0)
                cur_x-=10;
            break;
        case 1: //key_up
            osal_printk("↑");
            if(cur_y-10>0)
                cur_y-=10;
            break;
        case 2: //key_down
            osal_printk("↓");
            if(cur_y+10<63)
                cur_y+=10;
            break;
        case 3: //key_right
            osal_printk("→");
            if(cur_x+10<127)
                cur_x+=10;
            break;
        default:
            break;
    }
    osal_printk("\r\n");
}

static void gpio_btn_init(pin_t pin)
{
    uapi_pin_set_mode(pin, HAL_PIO_FUNC_GPIO);
    uapi_pin_set_pull(pin, PIN_PULL_TYPE_UP);
    uapi_gpio_set_dir(pin, GPIO_DIRECTION_INPUT);    
    uapi_gpio_register_isr_func(pin, GPIO_INTERRUPT_FALLING_EDGE, gpio_callback_func);
}



static void app_i2c_init_pin(void)
{
    /* I2C pinmux. */
    uapi_pin_set_mode(PIN_I2C1_SCL, I2C_MASTER_PIN_MODE);
    uapi_pin_set_mode(PIN_I2C1_SDA, I2C_MASTER_PIN_MODE);       
    uapi_pin_set_pull(PIN_I2C1_SCL, PIN_PULL_TYPE_UP);
    uapi_pin_set_pull(PIN_I2C1_SDA, PIN_PULL_TYPE_UP);
}

static void *i2c_master_task(const char *arg)
{
    unused(arg);

    uint32_t baudrate = I2C_SET_BAUDRATE;
    uint8_t hscode = I2C_MASTER_ADDR;

    /* I2C master init config. */

    app_i2c_init_pin();
    uapi_i2c_master_init(1, baudrate, hscode);

    ssd1306_Init();
    ssd1306_Fill(Black);

    while (1) {
            osal_msleep(1000/30);
            ssd1306_SetCursor(cur_x, cur_y);
            ssd1306_ClearOLED();
            ssd1306_DrawString("H3863 ", Font_7x10, White);
            ssd1306_UpdateScreen();
    }
    return NULL;
}

static void tasks_entry(void)
{
    osThreadAttr_t attr;

    attr.name = "mainTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = DEAFULT_TASK_STACK_SIZE;
    attr.priority = DEAFULT_TASK_PRIO;

    if (osThreadNew((osThreadFunc_t)main_task, NULL, &attr) == NULL) {
        /* Create task fail. */
    }

    attr.name = "i2cTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = DEAFULT_TASK_STACK_SIZE;
    attr.priority = DEAFULT_TASK_PRIO;

    if (osThreadNew((osThreadFunc_t)i2c_master_task, NULL, &attr) == NULL) {
        /* Create task fail. */
    }

    gpio_btn_init(PIN_BUTTON_LEFT);
    gpio_btn_init(PIN_BUTTON_UP);
    gpio_btn_init(PIN_BUTTON_DOWN);
    gpio_btn_init(PIN_BUTTON_RIGHT);
}

/* Run the tasks_test_entry. */
app_run(tasks_entry);
