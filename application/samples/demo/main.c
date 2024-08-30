#include "main.h"

#define DEAFULT_TASK_STACK_SIZE 0x1000
#define DEAFULT_TASK_PRIO osPriorityNormal

#define PIN_BUTTON_LEFT GPIO_00
#define PIN_BUTTON_UP GPIO_01
#define PIN_BUTTON_DOWN GPIO_02
#define PIN_BUTTON_RIGHT GPIO_03

static void gpio_callback_func(pin_t pin, uintptr_t param)
{
    UNUSED(param);
    osal_printk("[%d]:", osKernelGetTickCount());
    switch (pin)
    {
    case 0: // key_left
        osal_printk("←");
        if (cur_x - 10 >= 0)
            cur_x -= 10;
        break;
    case 1: // key_up
        osal_printk("↑");
        if (cur_y - 10 > 0)
            cur_y -= 10;
        break;
    case 2: // key_down
        osal_printk("↓");
        if (cur_y + 10 < 63)
            cur_y += 10;
        break;
    case 3: // key_right
        osal_printk("→");
        if (cur_x + 10 < 127)
            cur_x += 10;
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

void *task_main(const char *arg)
{
    unused(arg);
    gpio_btn_init(PIN_BUTTON_LEFT);
    gpio_btn_init(PIN_BUTTON_UP);
    gpio_btn_init(PIN_BUTTON_DOWN);
    gpio_btn_init(PIN_BUTTON_RIGHT);
    // uapi_pin_set_mode(PIN_BLINKY, PIN_MODE_0);
    // uapi_gpio_set_dir(PIN_BLINKY, GPIO_DIRECTION_OUTPUT);
    // uapi_gpio_set_val(PIN_BLINKY, GPIO_LEVEL_LOW);

    while (1)
    {
        // osal_printk("[%d]Hello World  BearPi\r\n",osKernelGetTickCount());
        osal_msleep(500);
        // uapi_gpio_toggle(PIN_BLINKY);
    }

    return NULL;
}

static void attr_init(osThreadAttr_t *attr)
{
    attr->name = "untitleTask";
    attr->attr_bits = 0U;
    attr->cb_mem = NULL;
    attr->cb_size = 0U;
    attr->stack_mem = NULL;
    attr->stack_size = DEAFULT_TASK_STACK_SIZE;
    attr->priority = DEAFULT_TASK_PRIO;
}

static void tasks_entry(void)
{
    osThreadAttr_t attr;

    // attr_init(&attr);
    // attr.name = "mainTask";
    // if (osThreadNew((osThreadFunc_t)task_main, NULL, &attr) == NULL)
    // {
    //     /* Create task fail. */
    // }

    // attr_init(&attr);
    // attr.name = "oledTask";
    // if (osThreadNew((osThreadFunc_t)task_oled, NULL, &attr) == NULL)
    // {
    //     /* Create task fail. */
    // }

    attr_init(&attr);
    attr.name = "cspsTask";
    if (osThreadNew((osThreadFunc_t)task_csps, NULL, &attr) == NULL)
    {
        /* Create task fail. */
    }
}

/* Run the tasks_test_entry. */
app_run(tasks_entry);
