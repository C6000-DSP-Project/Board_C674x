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
/*              函数声明                                                    */
/*                                                                          */
/****************************************************************************/
extern void UARTprintf(const char *fmt, ...);

// 页面
void HomePage();

extern lv_obj_t *time_label;  // 时间标签

/****************************************************************************/
/*                                                                          */
/*              任务线程                                                    */
/*                                                                          */
/****************************************************************************/
Void LVGLTask(UArg a0, UArg a1)
{
    // 配置日志输出
    lv_log_register_print_cb((lv_log_print_g_cb_t)UARTprintf);

    // 页面
    // 主页
    HomePage();

    for(;;)
    {
        // 更新时间
        lv_label_set_text_fmt(time_label, "#ffffff %04d/%02d/%02d %s %02d:%02d:%02d#", RTCTime.tm_year + 1920, RTCTime.tm_mon, RTCTime.tm_mday, WeekDayStr[RTCTime.tm_wday],
                                                                                       RTCTime.tm_hour, RTCTime.tm_min, RTCTime.tm_sec);

        lv_timer_handler();
        Task_sleep(1000);
    }
}
