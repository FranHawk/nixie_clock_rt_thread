/*******************************************************************************
* @file     --> hv57708.c
* @author   --> Li.Changchun
* @version  --> 1.0
* @date     --> 23-Nov-2019
* @brief    --> HV57708 驱动
*           芯片简介:
*           32MHz, 64通道推挽输出串入并出芯片,
*           内有 4 个 并行的 16 位移位寄存器
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "drv_hv57708.h"

/*******************************************************************************
  * @brief  HV57708 初始化
  * @param  None
  * @retval None
*******************************************************************************/
void HV57708_Init(void)
{
    /* 需要注意的是, HV57708 引脚的驱动电压为 5V, 而 STM32 推挽输出高电平
    仅为 3.3V, 为了匹配电平, STM32 引脚应使用开漏输出并上拉到 5V */

    /* CLK, LE, POL */
    rt_pin_mode(HV57708_CLK, PIN_MODE_OUTPUT_OD);
    rt_pin_mode(HV57708_LE, PIN_MODE_OUTPUT_OD);
    rt_pin_mode(HV57708_POL, PIN_MODE_OUTPUT_OD);
    /* 数据引脚 */
    rt_pin_mode(HV57708_DI1, PIN_MODE_OUTPUT_OD);
    rt_pin_mode(HV57708_DI2, PIN_MODE_OUTPUT_OD);
    rt_pin_mode(HV57708_DI3, PIN_MODE_OUTPUT_OD);
    rt_pin_mode(HV57708_DI4, PIN_MODE_OUTPUT_OD);


    /******************************************************************************/

    HV57708_CLK_L;
    HV57708_LE_H;
    HV57708_POL_L; // 反相输出
    HV57708_DIN1_L;
    HV57708_DIN2_L;
    HV57708_DIN3_L;
    HV57708_DIN4_L;

    HV57708_SendData(0, 0); // 所有引脚输出低电平
}




/*******************************************************************************
  * @brief  向 HV57708 发送 64 位数据
  * @param  datapart1 - 第一部分数据, 32 位
  *         datapart2 - 第二部分数据, 32 位
  * @retval None
*******************************************************************************/
void HV57708_SendData(rt_uint32_t datapart2, rt_uint32_t datapart1)
{
    rt_uint8_t i;
    rt_uint32_t tmp;
    tmp = datapart2; // 高位在前
    for (i = 0; i < 8; i++)
    {
        HV57708_CLK_L;

        if (tmp & 0x80000000)
            HV57708_DIN4_H;
        else
            HV57708_DIN4_L;
        tmp <<= 1;
        if (tmp & 0x80000000)
            HV57708_DIN3_H;
        else
            HV57708_DIN3_L;
        tmp <<= 1;
        if (tmp & 0x80000000)
            HV57708_DIN2_H;
        else
            HV57708_DIN2_L;
        tmp <<= 1;
        if (tmp & 0x80000000)
            HV57708_DIN1_H;
        else
            HV57708_DIN1_L;
        tmp <<= 1;

        HV57708_CLK_H;
        rt_hw_us_delay(75); /* 至少 62 ns */
        HV57708_CLK_L;
        rt_hw_us_delay(75);
    }
    tmp = datapart1;
    for (i = 0; i < 8; i++)
    {
        HV57708_CLK_L;

        if (tmp & 0x80000000)
            HV57708_DIN4_H;
        else
            HV57708_DIN4_L;
        tmp <<= 1;
        if (tmp & 0x80000000)
            HV57708_DIN3_H;
        else
            HV57708_DIN3_L;
        tmp <<= 1;
        if (tmp & 0x80000000)
            HV57708_DIN2_H;
        else
            HV57708_DIN2_L;
        tmp <<= 1;
        if (tmp & 0x80000000)
            HV57708_DIN1_H;
        else
            HV57708_DIN1_L;
        tmp <<= 1;

        HV57708_CLK_H;
        rt_hw_us_delay(75);  /* 至少 62 ns */
        HV57708_CLK_L;
        rt_hw_us_delay(75);
    }
}

/*******************************************************************************
  * @brief  将 HV57708 寄存器中的数据发送到引脚, 即锁存使能脉冲
  * @param  None
  * @retval None
*******************************************************************************/
void HV57708_OutputData(void)
{
    HV57708_LE_L;
    rt_hw_us_delay(35);
    HV57708_LE_H;
    /* 至少 25ns */
    rt_hw_us_delay(35);
    HV57708_LE_L;
}

/*******************************************************************************
  * @brief  将 HV57708 寄存器中的数据发送到引脚, 即锁存使能脉冲
  * @param  data: data0 ~ data5 表示辉光管从左到右
  * @retval None
*******************************************************************************/
void HV57708_Display(rt_uint8_t data[])
{
    RT_ASSERT(data != NULL);


    rt_uint32_t part2 = 0, part1 = 0;
    rt_uint32_t pos[6];
    rt_uint8_t  i;

    for (i = 0; i < 6; i++)
    {
        if (data[i] == 0)
            pos[i] = 0x0200;
        else
            pos[i] = 0x03FF & (1 << (data[i] - 1));
    }

    part1 = pos[0] | pos[1]<<10 | pos[2]<<20 | pos[3]<<30;
    part2 = pos[3]>>2 | pos[4]<<8 | pos[5]<<18;

    HV57708_SendData(part2, part1);
    HV57708_OutputData();
}

/*******************************************************************************
  * @brief  阴极保护, 顺序输出一遍所有数字
  * @param  None
  * @retval None
*******************************************************************************/
void HV57708_Protection(void)
{

    for (rt_uint8_t i = 0; i < 10; i++)
    {
        rt_uint8_t data[6] = {i, i, i, i, i, i};
        HV57708_Display(data);
        rt_thread_mdelay(75);
    }

}

/*******************************************************************************
  * @brief  依次设置 1 ~ 64 引脚输出高电平
  * @param  None
  * @retval None
*******************************************************************************/
void HV57708_Scan(void)
{
    rt_uint32_t part1=0x00000000, part2=0x00000000;
    for (int i = 0; i < 32; i++)
    {
        part1=0x00000000;
        part2=0x00000000;
        part1 |= (1 << i);
        HV57708_SendData(part2, part1);
        HV57708_OutputData();
    }
    for (int i = 0; i < 32; i++)
    {
        part1=0x00000000;
        part2=0x00000000;
        part1 |= (1 << i);
        HV57708_SendData(part2, part1);
        HV57708_OutputData();
        rt_thread_mdelay(500);
    }
    for (int i = 0; i < 32; i++)
    {
        part1=0x00000000;
        part2=0x00000000;
        part2 |= (1 << i);
        HV57708_SendData(part2, part1);
        HV57708_OutputData();
        rt_thread_mdelay(500);
    }
}

/*******************************************************************************
  * @brief  设置某个引脚输出高电平
  * @param  pin - 要输出高电平的引脚(1 ~ 64)
  * @retval None
*******************************************************************************/
void HV57708_SetPin(rt_uint8_t pin)
{
    rt_uint32_t temp = 0x00000000;
    if (pin > 64)
        return;
    if (pin <= 32)
    {
        temp |= (1 << (pin-1));
        HV57708_SendData(0, temp);
        HV57708_OutputData();
    }
    else
    {
        temp |= (1 << (pin-33));
        HV57708_SendData(temp, 0);
        HV57708_OutputData();
    }
}
