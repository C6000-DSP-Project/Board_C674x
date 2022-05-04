/****************************************************************************/
/*                                                                          */
/*    新核科技(广州)有限公司                                                */
/*                                                                          */
/*    Copyright (C) 2022 CoreKernel Technology Guangzhou Co., Ltd           */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/*    LVGL 显示任务                                                         */
/*                                                                          */
/*    2022年05月02日                                                        */
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

#include "Pages/zh-cn.h"  // 汉语文本字符串

/****************************************************************************/
/*                                                                          */
/*              全局变量                                                    */
/*                                                                          */
/****************************************************************************/
// 星期
char *WeekDayStr[] = {SundayStr, MondayStr, TuesdayStr, WednesdayStr, ThurdayStr, FridayStr, SaturdayStr};

// 温度湿度
float t, rh;

/****************************************************************************/
/*                                                                          */
/*              函数声明                                                    */
/*                                                                          */
/****************************************************************************/
extern void UARTprintf(const char *fmt, ...);

// 页面
void HomePage();

// 标签
extern lv_obj_t *time_label;  // 时间标签
extern lv_obj_t *temp_label;  // 温度湿度标签

/****************************************************************************/
/*                                                                          */
/*              任务线程                                                    */
/*                                                                          */
/****************************************************************************/
Void LVGLTask(UArg a0, UArg a1)
{
    // 配置日志输出
    lv_log_register_print_cb((lv_log_print_g_cb_t)UARTprintf);

    for(;;)
    {
        lv_timer_handler();
        Task_sleep(5);
    }
}

Void LVGLAppTask(UArg a0, UArg a1)
{
    // 页面
    // 主页
    HomePage();

    // 任务循环
    char str[64];

    for(;;)
    {
        // 时间
        lv_label_set_text_fmt(time_label, "#ffffff %04d/%02d/%02d %s %02d:%02d:%02d#", RTCTime.tm_year + 1920, RTCTime.tm_mon, RTCTime.tm_mday, WeekDayStr[RTCTime.tm_wday],
                                                                                       RTCTime.tm_hour, RTCTime.tm_min, RTCTime.tm_sec);

        // 温度/湿度
        TempSensorGet(&t, &rh);
        sprintf(str, "%s %2.2f%s %s %2.2f%%", TempStr, t, DegStr, HumStr, rh);
        lv_label_set_text_fmt(temp_label, "#ffffff %s#", str);
        Task_sleep(1000);
    }
}

