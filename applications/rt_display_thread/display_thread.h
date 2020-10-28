/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-10-02     Alpha       the first version
 */
#ifndef APPLICATIONS_RT_DISPLAY_THREAD_DISPLAY_THREAD_H_
#define APPLICATIONS_RT_DISPLAY_THREAD_DISPLAY_THREAD_H_

extern uint8_t display_num;
extern uint8_t alarm_hour;
extern uint8_t alarm_min;
int display_thread_startup(void);

#endif /* APPLICATIONS_RT_DISPLAY_THREAD_DISPLAY_THREAD_H_ */
