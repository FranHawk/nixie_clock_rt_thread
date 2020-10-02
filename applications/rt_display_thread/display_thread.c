/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-10-02     Alpha       the first version
 */

#include <rtthread.h>

#define DBG_TAG "display"
#define DBG_LVL DBG_LOG

#include <rtdbg.h>
#include <rtdevice.h>
#include "drv_common.h"
#include "drv_hv57708.h"

#include "display_thread.h"


#define THREAD_PRIORITY         25
#define THREAD_STACK_SIZE       512
#define THREAD_TIMESLICE        5

static rt_thread_t tid1 = RT_NULL;


uint8_t display_num;



static void display_thread_entry(void *parameter)
{
    while(1)
    {
        rt_thread_mdelay(1000);

        LOG_D("display num:%d",display_num);
    }

}


int display_thread_startup(void)
{



    HV57708_Init();//hv57708初始化

    LOG_D("HV57708 device init success!");

    tid1 = rt_thread_create("display_thread",
                            display_thread_entry, RT_NULL,
                            THREAD_STACK_SIZE,
                            THREAD_PRIORITY, THREAD_TIMESLICE);

    /* 如果获得线程控制块，启动这个线程 */
    if (tid1 != RT_NULL)
        rt_thread_startup(tid1);

    return 0;
}
