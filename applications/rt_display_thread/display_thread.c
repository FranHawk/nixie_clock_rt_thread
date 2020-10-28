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
#define THREAD_STACK_SIZE       1024
#define THREAD_TIMESLICE        5

static rt_thread_t tid1 = RT_NULL;


uint8_t display_num;
uint8_t alarm_hour;
uint8_t alarm_min;


static void display_thread_entry(void *parameter)
{
    time_t now;
    struct tm *p;
    rt_uint16_t count = 0;
    int min = 0, hour = 0,sec = 0;
    rt_uint8_t data[6] = {1, 2, 3, 4, 5, 6};
    while(1)
    {
        count++;
        if(count%200)
        {
            p=gmtime((const time_t*) &now);
                    hour = p->tm_hour;
                    min = p->tm_min;
                    sec = p->tm_sec;
                    now = time(RT_NULL);

                    //显示时分还是分秒
                    if(display_num==0){
                        data[0] = hour/10;
                        data[1] = hour%10;
                        data[2] = min/10;
                        data[3] = min%10;
                    }
                    else{
                        data[0] = min/10;
                        data[1] = min%10;
                        data[2] = sec/10;
                        data[3] = sec%10;
                    }

                    if (data[2]==0) {
                        data[2] = 9;
                    }
                    if (data[2]==2) {
                        data[2] = 8;
                    }
                    //飞线
                    HV57708_Display(data);
                    //LOG_D("display num:%d",display_num);
        }
        else {
            //阴极保护
            HV57708_Protection();
        }



        rt_thread_mdelay(250);
    }

}

INIT_DEVICE_EXPORT(HV57708_Init);
int display_thread_startup(void)
{



    //HV57708_Init();//hv57708初始化


    rt_uint8_t data[6] = {1, 2, 3, 4, 5, 6};
    HV57708_Display(data);


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
