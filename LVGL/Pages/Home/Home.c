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

/****************************************************************************/
/*                                                                          */
/*              ȫ�ֱ���                                                    */
/*                                                                          */
/****************************************************************************/
// ��ǩ
lv_obj_t *time_label;  // ʱ���ǩ
lv_obj_t *temp_label;  // �¶�ʪ�ȱ�ǩ

/****************************************************************************/
/*                                                                          */
/*              �¼�                                                        */
/*                                                                          */
/****************************************************************************/
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
    LV_IMG_DECLARE(img_bg);
    lv_img_set_src(img, &img_bg);
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
}
