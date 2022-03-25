/****************************************************************************/
/*                                                                          */
/*    新核科技(广州)有限公司                                                */
/*                                                                          */
/*    Copyright (C) 2022 CoreKernel Technology (Guangzhou) Co., Ltd         */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/*    RTC 实时时钟                                                          */
/*                                                                          */
/*    2014年06月05日                                                        */
/*                                                                          */
/****************************************************************************/
/*
 *    - 希望缄默(bin wang)
 *    - bin@corekernel.net
 *
 *    官网 corekernel.net/.org/.cn
 *    社区 fpga.net.cn
 *
 */
#include <App.h>

#include "rtc.h"

/****************************************************************************/
/*                                                                          */
/*              全局变量                                                    */
/*                                                                          */
/****************************************************************************/
struct tm RTCTime;

extern unsigned char LEDNixieVal[4];

/****************************************************************************/
/*                                                                          */
/*              设置日期/时间                                               */
/*                                                                          */
/****************************************************************************/
void RTCSet()
{
    // 设置日期
    RTCYearSet(SOC_RTC_0_REGS, RTCTime.tm_year);
    RTCMonthSet(SOC_RTC_0_REGS, RTCTime.tm_mon);
    RTCDayOfMonthSet(SOC_RTC_0_REGS, RTCTime.tm_mday);
    RTCDayOfTheWeekSet(SOC_RTC_0_REGS, RTCTime.tm_wday);   // 星期

    // 设置时间
    RTCHourSet(SOC_RTC_0_REGS, RTCTime.tm_hour);
    RTCMinuteSet(SOC_RTC_0_REGS, RTCTime.tm_min);
    RTCSecondSet(SOC_RTC_0_REGS, RTCTime.tm_sec);
}

/****************************************************************************/
/*                                                                          */
/*              硬件中断线程                                                */
/*                                                                          */
/****************************************************************************/
Void RTCHwi(UArg arg)
{
    // 获取日期
    RTCTime.tm_year = RTCYearGet(SOC_RTC_0_REGS);
    RTCTime.tm_mon = RTCMonthGet(SOC_RTC_0_REGS);
    RTCTime.tm_mday = RTCDayOfMonthGet(SOC_RTC_0_REGS);
    RTCTime.tm_wday = RTCDayOfTheWeekGet(SOC_RTC_0_REGS);  // 星期

    // 获取时间
    RTCTime.tm_hour = RTCHourGet(SOC_RTC_0_REGS);
    RTCTime.tm_min = RTCMinuteGet(SOC_RTC_0_REGS);
    RTCTime.tm_sec = RTCSecondGet(SOC_RTC_0_REGS);

    // 数码管显示时间
    LEDNixieVal[0] = (RTCTime.tm_min >> 4) & 0x0F;
    LEDNixieVal[1] = (RTCTime.tm_min) & 0x0F;

    LEDNixieVal[2] = (RTCTime.tm_sec >> 4) & 0x0F;
    LEDNixieVal[3] = (RTCTime.tm_sec) & 0x0F;

    if((LEDNixieVal[2] == 5) && (LEDNixieVal[3] == 9))
    {
        BUZZERBeep(0xFFFFFF);
    }
}

static Void HwiInit()
{
    Hwi_Params hwiParams;
    Hwi_Params_init(&hwiParams);

    // 中断事件
    // 可以直接使用中断事件序号也可以使用 StarterWare "interrupt.h" 中的宏定义
    // 详情参阅 TMS320C6748 DSP Technical Reference Manual 83-86页 DSP Interrupt Map
    hwiParams.eventId = SYS_INT_RTC_IRQS;
    Hwi_create(C674X_MASK_INT13, RTCHwi, &hwiParams, NULL);
}

/****************************************************************************/
/*                                                                          */
/*              延时（非精确）                                              */
/*                                                                          */
/****************************************************************************/
void Delay(volatile unsigned int count)
{
    while(count--);
}

/****************************************************************************/
/*                                                                          */
/*              返回识别码                                                  */
/*                                                                          */
/****************************************************************************/
unsigned int RtcVersionGet()
{
    // 返回 1 表示 AM1808
    // AM1808 使能 RTC 执行一次复位

    return 0;
}

/****************************************************************************/
/*                                                                          */
/*              初始化                                                      */
/*                                                                          */
/****************************************************************************/
void RTCInit()
{
    // 禁用 RTC 寄存器写保护
    RTCWriteProtectDisable(SOC_RTC_0_REGS);

    // 软件复位并使能 RTC
    RTCEnable(SOC_RTC_0_REGS);

    // 延时 最小3倍 32KH 时钟周期
    Delay(0xFFFF);

    // 使能 32KHz 计数器
    RTCRun(SOC_RTC_0_REGS);

    // 使能实时时钟中断 每秒产生一次中断
    RTCIntTimerEnable(SOC_RTC_0_REGS, RTC_INT_EVERY_SECOND);

    // 硬件中断线程初始化
    HwiInit();
}
