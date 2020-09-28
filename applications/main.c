/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-09-25     RT-Thread    first version
 */

#include <rtthread.h>

#define DBG_TAG "main"
#define DBG_LVL DBG_LOG



#include <rtdbg.h>
#include <rtdevice.h>
#include "drv_common.h"

#define LED0_PIN  GET_PIN(A,8)

int main(void)
{
    int count = 1;

    rt_pin_mode(LED0_PIN, PIN_MODE_OUTPUT);

    while (count++)
    {
        //LOG_D("Hello RT-Thread!");
        rt_pin_write(LED0_PIN, !rt_pin_read(LED0_PIN));
        rt_thread_mdelay(1000);
    }

    return RT_EOK;
}
