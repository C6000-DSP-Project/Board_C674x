/****************************************************************************/
/*                                                                          */
/*    �º˿Ƽ�(����)���޹�˾                                                */
/*                                                                          */
/*    Copyright (C) 2022 CoreKernel Technology Guangzhou Co., Ltd           */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/*    LVGL ��ʾ����                                                         */
/*                                                                          */
/*    2022��05��02��                                                        */
/*                                                                          */
/****************************************************************************/
/*
 *    - ϣ����Ĭ(bin wang)
 *    - bin@corekernel.net
 *
 *    ���� corekernel.net/.org/.cn
 *    ���� fpga.net.cn
 *
 */
#include <App.h>

#include "Pages/zh-cn.h"  // �����ı��ַ���

/****************************************************************************/
/*                                                                          */
/*              ȫ�ֱ���                                                    */
/*                                                                          */
/****************************************************************************/
// ����
char *WeekDayStr[] = {SundayStr, MondayStr, TuesdayStr, WednesdayStr, ThurdayStr, FridayStr, SaturdayStr};

// �¶�ʪ��
float t, rh;

/****************************************************************************/
/*                                                                          */
/*              ��������                                                    */
/*                                                                          */
/****************************************************************************/
extern void UARTprintf(const char *fmt, ...);

// ҳ��
void HomePage();

// ��ǩ
extern lv_obj_t *time_label;  // ʱ���ǩ
extern lv_obj_t *temp_label;  // �¶�ʪ�ȱ�ǩ

/****************************************************************************/
/*                                                                          */
/*              �����߳�                                                    */
/*                                                                          */
/****************************************************************************/
Void LVGLTask(UArg a0, UArg a1)
{
    // ������־���
    lv_log_register_print_cb((lv_log_print_g_cb_t)UARTprintf);

    for(;;)
    {
        lv_timer_handler();
        Task_sleep(5);
    }
}

Void LVGLAppTask(UArg a0, UArg a1)
{
    // ҳ��
    // ��ҳ
    HomePage();

    // ����ѭ��
    char str[64];

    for(;;)
    {
        // ʱ��
        lv_label_set_text_fmt(time_label, "#ffffff %04d/%02d/%02d %s %02d:%02d:%02d#", RTCTime.tm_year + 1920, RTCTime.tm_mon, RTCTime.tm_mday, WeekDayStr[RTCTime.tm_wday],
                                                                                       RTCTime.tm_hour, RTCTime.tm_min, RTCTime.tm_sec);

        // �¶�/ʪ��
        TempSensorGet(&t, &rh);
        sprintf(str, "%s %2.2f%s %s %2.2f%%", TempStr, t, DegStr, HumStr, rh);
        lv_label_set_text_fmt(temp_label, "#ffffff %s#", str);
        Task_sleep(1000);
    }
}

