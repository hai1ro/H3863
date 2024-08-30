#ifndef __DEMO_MAIN_H__
#define __DEMO_MAIN_H__


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

extern uint8_t cur_x;
extern uint8_t cur_y;

void *task_main(const char *arg);
void *task_oled(const char *arg);
void *task_csps(const char *arg);


#endif
