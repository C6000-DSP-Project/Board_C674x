/****************************************************************************/
/*                                                                          */
/*    �º˿Ƽ�(����)���޹�˾                                                */
/*                                                                          */
/*    Copyright (C) 2022 CoreKernel Technology Guangzhou Co., Ltd           */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/*    LVGL ҳ��                                                             */
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

#include "../zh-cn.h"  // �����ı��ַ���

/****************************************************************************/
/*                                                                          */
/*              ȫ�ֱ���                                                    */
/*                                                                          */
/****************************************************************************/
// ��ǩ
lv_obj_t *time_label;  // ʱ���ǩ
lv_obj_t *temp_label;  // �¶�ʪ�ȱ�ǩ

// ��ת�Ƕ�
uint8_t roate = LV_DISP_ROT_90;

/****************************************************************************/
/*                                                                          */
/*              ��������                                                    */
/*                                                                          */
/****************************************************************************/
extern void CalculatorWin();
extern void SystemInfoWin();

/****************************************************************************/
/*                                                                          */
/*              �¼�                                                        */
/*                                                                          */
/****************************************************************************/
static void roate_imgbtnEvent(lv_event_t *e)
{
    lv_event_code_t event = lv_event_get_code(e);

    if(event == LV_EVENT_CLICKED)
    {
        lv_disp_set_rotation(NULL, roate);
        (roate == 3) ? roate = 0 : roate++;
    }
}

static void calculator_imgbtnEvent(lv_event_t *e)
{
    lv_event_code_t event = lv_event_get_code(e);

    if(event == LV_EVENT_CLICKED)
    {
        CalculatorWin();
    }
}

static void systeminfo_imgbtnEvent(lv_event_t *e)
{
    lv_event_code_t event = lv_event_get_code(e);

    if(event == LV_EVENT_CLICKED)
    {
        SystemInfoWin();
    }
}

static void time_label_event_cb(lv_event_t *e)
{
    // ��������
    lv_obj_t *calendar = lv_calendar_create(lv_scr_act());
    lv_obj_set_size(calendar, 300, 300);                                           // ��С
    lv_obj_align(calendar, LV_ALIGN_CENTER, 0, 0);                                 // λ��

    // �������ں���ʾ����
    lv_calendar_set_today_date(calendar, RTCTime.tm_year + 1920, RTCTime.tm_mon, RTCTime.tm_mday);
    lv_calendar_set_showed_date(calendar, RTCTime.tm_year + 1920, RTCTime.tm_mon);

    // ɾ������
//    lv_obj_del(calendar);
}

/****************************************************************************/
/*                                                                          */
/*              ҳ��                                                        */
/*                                                                          */
/****************************************************************************/
void HomePage()
{
    /* LVGL ���� */
    // ����ͼƬ
    lv_obj_t *img = lv_img_create(lv_scr_act());
    LV_IMG_DECLARE(imgBackground);
    lv_img_set_src(img, &imgBackground);
    lv_obj_align(img, LV_ALIGN_TOP_LEFT, 0, 0);

    // ��ʾʱ��
    time_label = lv_label_create(lv_scr_act());                                    // ������ǩ
    lv_obj_set_style_text_align(time_label, LV_TEXT_ALIGN_LEFT, 0);                // �����
    lv_label_set_long_mode(time_label, LV_LABEL_LONG_SCROLL);                      // ���ı�����
    lv_obj_set_width(time_label, 230);                                             // �ı����
    lv_obj_align(time_label, LV_ALIGN_TOP_RIGHT, 15, 15);                          // λ��
    lv_label_set_recolor(time_label, true);                                        // �����޸��ı���ɫ

    // ��ʾ��ʪ��
    temp_label = lv_label_create(lv_scr_act());
    lv_obj_set_style_text_font(temp_label, &SourceHanSansCN_Normal_16, 0);
    lv_obj_set_width(temp_label, 100);
    lv_obj_align(temp_label, LV_ALIGN_BOTTOM_LEFT, 5, -5);
    lv_label_set_recolor(temp_label, true);

    // ����ͼ��
    // ϵͳ��Ϣ
    LV_IMG_DECLARE(imgSystemInfo);

    lv_obj_t *systeminfo_imgbtn = lv_imgbtn_create(lv_scr_act());
    lv_obj_set_size(systeminfo_imgbtn, imgSystemInfo.header.w, imgSystemInfo.header.h);
    lv_obj_set_pos(systeminfo_imgbtn, 50, 50);
    lv_imgbtn_set_src(systeminfo_imgbtn, LV_IMGBTN_STATE_RELEASED, NULL, &imgSystemInfo, NULL);
    lv_imgbtn_set_src(systeminfo_imgbtn, LV_IMGBTN_STATE_PRESSED, NULL, &imgSystemInfo, NULL);
    lv_obj_add_event_cb(systeminfo_imgbtn, systeminfo_imgbtnEvent, LV_EVENT_CLICKED, NULL);

    lv_obj_t *systeminfo_label;
    systeminfo_label = lv_label_create(lv_scr_act());
    lv_obj_set_style_text_align(systeminfo_label, LV_TEXT_ALIGN_LEFT, 0);
    lv_label_set_long_mode(systeminfo_label, LV_LABEL_LONG_SCROLL);
    lv_obj_set_width(systeminfo_label, 230);
    lv_obj_align_to(systeminfo_label, systeminfo_imgbtn, LV_ALIGN_OUT_BOTTOM_LEFT, -8, 10);
    lv_label_set_recolor(systeminfo_label, true);
    lv_label_set_text_fmt(systeminfo_label, "#000000 %s#", SystemInfoStr);

    // ��ת
    LV_IMG_DECLARE(imgRoate);

    lv_obj_t *roate_imgbtn = lv_imgbtn_create(lv_scr_act());
    lv_obj_set_size(roate_imgbtn, imgRoate.header.w, imgRoate.header.h);
    lv_obj_set_pos(roate_imgbtn, 130, 50);
    lv_imgbtn_set_src(roate_imgbtn, LV_IMGBTN_STATE_RELEASED, NULL, &imgRoate, NULL);
    lv_imgbtn_set_src(roate_imgbtn, LV_IMGBTN_STATE_PRESSED, NULL, &imgRoate, NULL);
    lv_obj_add_event_cb(roate_imgbtn, roate_imgbtnEvent, LV_EVENT_CLICKED, NULL);

    lv_obj_t *roate_label;
    roate_label = lv_label_create(lv_scr_act());
    lv_obj_set_style_text_align(roate_label, LV_TEXT_ALIGN_LEFT, 0);
    lv_label_set_long_mode(roate_label, LV_LABEL_LONG_SCROLL);
    lv_obj_set_width(roate_label, 230);
    lv_obj_align_to(roate_label, roate_imgbtn, LV_ALIGN_OUT_BOTTOM_LEFT, -8, 10);
    lv_label_set_recolor(roate_label, true);
    lv_label_set_text_fmt(roate_label, "#000000 %s#", RoateStr);

    // ������
    LV_IMG_DECLARE(imgCalculator);

    lv_obj_t *calculator_imgbtn = lv_imgbtn_create(lv_scr_act());
    lv_obj_set_size(calculator_imgbtn, imgCalculator.header.w, imgCalculator.header.h);
    lv_obj_set_pos(calculator_imgbtn, 210, 50);
    lv_imgbtn_set_src(calculator_imgbtn, LV_IMGBTN_STATE_RELEASED, NULL, &imgCalculator, NULL);
    lv_imgbtn_set_src(calculator_imgbtn, LV_IMGBTN_STATE_PRESSED, NULL, &imgCalculator, NULL);
    lv_obj_add_event_cb(calculator_imgbtn, calculator_imgbtnEvent, LV_EVENT_CLICKED, NULL);

    lv_obj_t *calculator_label;
    calculator_label = lv_label_create(lv_scr_act());
    lv_obj_set_style_text_align(calculator_label, LV_TEXT_ALIGN_LEFT, 0);
    lv_label_set_long_mode(calculator_label, LV_LABEL_LONG_SCROLL);
    lv_obj_set_width(calculator_label, 230);
    lv_obj_align_to(calculator_label, calculator_imgbtn, LV_ALIGN_OUT_BOTTOM_LEFT, -2, 10);
    lv_label_set_recolor(calculator_label, true);
    lv_label_set_text_fmt(calculator_label, "#000000 %s#", CalculatorStr);

    // ����


    // ̰����


    // ����

    // ����

    // ����


}
