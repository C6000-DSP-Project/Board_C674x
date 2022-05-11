/****************************************************************************/
/*                                                                          */
/*    �º˿Ƽ�(����)���޹�˾                                                */
/*                                                                          */
/*    Copyright (C) 2022 CoreKernel Technology Guangzhou Co., Ltd           */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/*    ʱ��                                                                  */
/*                                                                          */
/*    2022��05��10��                                                        */
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

#include "zh-cn.h"  // �����ı��ַ���

/****************************************************************************/
/*                                                                          */
/*              ȫ�ֱ���                                                    */
/*                                                                          */
/****************************************************************************/
static lv_obj_t *win;

static lv_obj_t *meter;
lv_meter_indicator_t *indic_hour;
lv_meter_indicator_t *indic_min;
lv_meter_indicator_t *indic_sec;

lv_obj_t *timelabel;
lv_timer_t *timer;

/****************************************************************************/
/*                                                                          */
/*              �¼�                                                        */
/*                                                                          */
/****************************************************************************/
static void WinEvent(lv_event_t *e)
{
    // ɾ����ʱ��
    lv_timer_del(timer);

    lv_obj_t *obj = lv_event_get_target(e);
    LV_LOG_USER("Button %d clicked", (int)lv_obj_get_index(obj));

    lv_obj_del(win);
}

/****************************************************************************/
/*                                                                          */
/*              ����ʱ��                                                    */
/*                                                                          */
/****************************************************************************/
static void UpdateTime(lv_timer_t *timer)
{
    lv_label_set_text_fmt(timelabel, "#00BFFF %02d:%02d:%02d", RTCTime.tm_hour, RTCTime.tm_min, RTCTime.tm_sec);
}

/****************************************************************************/
/*                                                                          */
/*              ����                                                        */
/*                                                                          */
/****************************************************************************/
void ClockWin()
{
    win = lv_win_create(lv_scr_act(), 40);

    lv_obj_t *label;
    label = lv_win_add_title(win, ClockTitleStr);

    lv_obj_t *btn;
    btn = lv_win_add_btn(win, LV_SYMBOL_CLOSE, 40);
    lv_obj_add_event_cb(btn, WinEvent, LV_EVENT_CLICKED, NULL);

    lv_obj_t *cont = lv_win_get_content(win);

    // ģ��ʱ��
    meter = lv_meter_create(cont);
    lv_obj_set_size(meter, 350, 350);
    lv_obj_align(meter, LV_ALIGN_LEFT_MID, 0, 0);

    // ʱ�̶�
    lv_meter_scale_t *scale_hour = lv_meter_add_scale(meter);
    lv_meter_set_scale_ticks(meter, scale_hour, 12, 0, 0, lv_palette_main(LV_PALETTE_GREY));  // 12 �̶�
    lv_meter_set_scale_major_ticks(meter, scale_hour, 1, 4, 20, lv_color_black(), 15);
    lv_meter_set_scale_range(meter, scale_hour, 1, 12, 330, 300);                             // �̶�ֵ

    // �̶ֿ�
    // 360��61 �̶�(��ʼ�����̶��غ�)
    lv_meter_scale_t *scale_min = lv_meter_add_scale(meter);
    lv_meter_set_scale_ticks(meter, scale_min, 61, 1, 10, lv_palette_main(LV_PALETTE_GREY));
    lv_meter_set_scale_range(meter, scale_min, 0, 60, 360, 270);

    // ��̶�
    // 360��61 �̶�(��ʼ�����̶��غ�)
    lv_meter_scale_t *scale_sec = lv_meter_add_scale(meter);
    lv_meter_set_scale_ticks(meter, scale_sec, 61, 1, 10, lv_palette_main(LV_PALETTE_GREY));
    lv_meter_set_scale_range(meter, scale_sec, 0, 60, 360, 270);

    // ʱ�� ���� ����
    LV_IMG_DECLARE(imgHandHour);
    LV_IMG_DECLARE(imgHandMin);
    LV_IMG_DECLARE(imgHandSec);

    lv_meter_indicator_t *indic_hour = lv_meter_add_needle_img(meter, scale_hour, &imgHandHour, 5, 5);
    lv_meter_indicator_t *indic_min = lv_meter_add_needle_img(meter, scale_min, &imgHandMin, 5, 5);
    lv_meter_indicator_t *indic_sec = lv_meter_add_needle_img(meter, scale_sec, &imgHandSec, 5, 5);

    // ��ʱ����ʱ��
    lv_meter_set_indicator_end_value(meter, indic_hour, RTCTime.tm_hour);
    lv_meter_set_indicator_end_value(meter, indic_min, RTCTime.tm_min);
    lv_meter_set_indicator_end_value(meter, indic_sec, RTCTime.tm_sec);

    // ����ʱ��
    timelabel = lv_label_create(cont);
    lv_obj_set_style_text_font(timelabel, &lv_font_montserrat_48, 0);
    lv_obj_set_style_text_align(timelabel, LV_TEXT_ALIGN_LEFT, 0);                            // �����
    lv_obj_set_width(timelabel, 200);                                                         // �ı����
    lv_obj_align(timelabel, LV_ALIGN_TOP_RIGHT, -75, 170);                                    // λ��
    lv_label_set_recolor(timelabel, true);                                                    // �����޸��ı���ɫ
    lv_label_set_text_fmt(timelabel, "#00BFFF %02d:%02d:%02d", RTCTime.tm_hour, RTCTime.tm_min, RTCTime.tm_sec);

    // ������ʱ��
    timer = lv_timer_create(UpdateTime, 1000, NULL);
}
