/****************************************************************************/
/*                                                                          */
/*    �º˿Ƽ�(����)���޹�˾                                                */
/*                                                                          */
/*    Copyright (C) 2022 CoreKernel Technology Guangzhou Co., Ltd           */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/*    LCD ��Ļ����                                                          */
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

#include "ecap.h"

/****************************************************************************/
/*                                                                          */
/*              ȫ�ֱ���                                                    */
/*                                                                          */
/****************************************************************************/
static lv_obj_t *win;
static lv_obj_t *slider_label;
static unsigned int Brightness = 50;

/****************************************************************************/
/*                                                                          */
/*              �¼�                                                        */
/*                                                                          */
/****************************************************************************/
static void WinEvent(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_target(e);
    LV_LOG_USER("Button %d clicked", (int)lv_obj_get_index(obj));

    lv_obj_del(win);
}

static void SliderEvent(lv_event_t *e)
{
    lv_obj_t *slider = lv_event_get_target(e);
    Brightness = lv_slider_get_value(slider);

    char buf[8];
    lv_snprintf(buf, sizeof(buf), "%d%%", (int)Brightness);
    lv_label_set_text(slider_label, buf);
    lv_obj_align_to(slider_label, slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    // �޸���Ļ����(����ռ�ձ�)
    ECAPAPWMCaptureConfig(SOC_ECAP_2_REGS, 228 * Brightness, 22800);
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
    label = lv_win_add_title(win, LCDBrightnessStr);

    lv_obj_t *btn;
    btn = lv_win_add_btn(win, LV_SYMBOL_CLOSE, 40);
    lv_obj_add_event_cb(btn, WinEvent, LV_EVENT_CLICKED, NULL);

    lv_obj_t *cont = lv_win_get_content(win);

    // ���ڻ���
    lv_obj_t *slider = lv_slider_create(cont);
    lv_obj_center(slider);
    lv_obj_add_event_cb(slider, SliderEvent, LV_EVENT_VALUE_CHANGED, NULL);
    lv_slider_set_range(slider, 1, 100);
    lv_slider_set_value(slider, Brightness, LV_ANIM_ON);

    // ��ǩ
    slider_label = lv_label_create(cont);
    lv_label_set_text_fmt(slider_label, "%d%%", Brightness);

    lv_obj_align_to(slider_label, slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
}
