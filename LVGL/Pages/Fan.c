/****************************************************************************/
/*                                                                          */
/*    �º˿Ƽ�(����)���޹�˾                                                */
/*                                                                          */
/*    Copyright (C) 2022 CoreKernel Technology Guangzhou Co., Ltd           */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/*    ����                                                                  */
/*                                                                          */
/*    2022��05��11��                                                        */
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

#include "ehrpwm.h"

/****************************************************************************/
/*                                                                          */
/*              �궨��                                                      */
/*                                                                          */
/****************************************************************************/
// ʱ�ӷ�Ƶ
#define CLOCK_DIV_VAL     228

/****************************************************************************/
/*                                                                          */
/*              ȫ�ֱ���                                                    */
/*                                                                          */
/****************************************************************************/
static lv_obj_t *win;
static lv_obj_t *slider_label;
static lv_obj_t *spinbox;

static unsigned int PWMCLK = 10000;
static unsigned int PWMDutyRatio = 80;

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

static void PWMConfig()
{
    // ��������
    EHRPWMPWMOpFreqSet(SOC_EHRPWM_1_REGS, SOC_EHRPWM_1_MODULE_FREQ / CLOCK_DIV_VAL, PWMCLK, EHRPWM_COUNT_UP, EHRPWM_SHADOW_WRITE_DISABLE);

    // ���ü����Ƚ�����ģ��
    // ���رȽ��� A ֵ
    EHRPWMLoadCMPA(SOC_EHRPWM_1_REGS, (SOC_EHRPWM_1_MODULE_FREQ / CLOCK_DIV_VAL / PWMCLK) * PWMDutyRatio / 100, EHRPWM_SHADOW_WRITE_DISABLE, EHRPWM_COMPA_NO_LOAD, EHRPWM_CMPCTL_OVERWR_SH_FL);
}

static void SpinboxIncrementEvent(lv_event_t *e)
{
    lv_event_code_t event = lv_event_get_code(e);

    if(event == LV_EVENT_SHORT_CLICKED || event  == LV_EVENT_LONG_PRESSED_REPEAT)
    {
        lv_spinbox_increment(spinbox);

        PWMCLK = lv_spinbox_get_value(spinbox);
        PWMConfig();
    }
}

static void SpinboxDecrementEvent(lv_event_t *e)
{
    lv_event_code_t event = lv_event_get_code(e);

    if(event == LV_EVENT_SHORT_CLICKED || event == LV_EVENT_LONG_PRESSED_REPEAT)
    {
        lv_spinbox_decrement(spinbox);

        PWMCLK = lv_spinbox_get_value(spinbox);
        PWMConfig();
    }
}

static void SliderEvent(lv_event_t *e)
{
    lv_obj_t *slider = lv_event_get_target(e);
    PWMDutyRatio = lv_slider_get_value(slider);

    char buf[8];
    lv_snprintf(buf, sizeof(buf), "%d%%", (int)PWMDutyRatio);
    lv_label_set_text(slider_label, buf);
    lv_obj_align_to(slider_label, slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    PWMConfig();
}

/****************************************************************************/
/*                                                                          */
/*              ����                                                        */
/*                                                                          */
/****************************************************************************/
void FanWin()
{
    win = lv_win_create(lv_scr_act(), 40);

    lv_obj_t *label;
    label = lv_win_add_title(win, FanTitleStr);

    lv_obj_t *btn;
    btn = lv_win_add_btn(win, LV_SYMBOL_CLOSE, 40);
    lv_obj_add_event_cb(btn, WinEvent, LV_EVENT_CLICKED, NULL);

    lv_obj_t *cont = lv_win_get_content(win);

    // GIF ͼƬ
    LV_IMG_DECLARE(imgFanGif);
    lv_obj_t *img;

    img = lv_gif_create(cont);
    lv_gif_set_src(img, &imgFanGif);
    lv_obj_align(img, LV_ALIGN_LEFT_MID, 20, 0);

    // ��ǩ
    label = lv_label_create(cont);
    lv_obj_set_style_text_font(label, &SourceHanSansCN_Normal_16, 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_set_width(label, 150);
    lv_obj_align_to(label, img, LV_ALIGN_OUT_RIGHT_TOP, 15, 0);
    lv_label_set_recolor(label, true);
    lv_label_set_text_fmt(label, "#FF0000 %s", FREQStr);

    // Ƶ�ʵ���
    spinbox = lv_spinbox_create(cont);
    lv_spinbox_set_range(spinbox, 0, 50000000);
    lv_spinbox_set_digit_format(spinbox, 8, 0);
    lv_spinbox_step_prev(spinbox);
    lv_obj_set_width(spinbox, 100);
    lv_obj_align_to(spinbox, label, LV_ALIGN_OUT_BOTTOM_LEFT, 50, 10);
    lv_spinbox_set_value(spinbox, PWMCLK);

    lv_coord_t h = lv_obj_get_height(spinbox);

    // ���Ӱ�ť
    btn = lv_btn_create(cont);
    lv_obj_set_size(btn, h, h);
    lv_obj_align_to(btn, spinbox, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
    lv_obj_set_style_bg_img_src(btn, LV_SYMBOL_PLUS, 0);
    lv_obj_add_event_cb(btn, SpinboxIncrementEvent, LV_EVENT_ALL,  NULL);

    // ���ٰ�ť
    btn = lv_btn_create(cont);
    lv_obj_set_size(btn, h, h);
    lv_obj_align_to(btn, spinbox, LV_ALIGN_OUT_LEFT_MID, -5, 0);
    lv_obj_set_style_bg_img_src(btn, LV_SYMBOL_MINUS, 0);
    lv_obj_add_event_cb(btn, SpinboxDecrementEvent, LV_EVENT_ALL, NULL);

    // ��ǩ
    label = lv_label_create(cont);
    lv_obj_set_style_text_font(label, &SourceHanSansCN_Normal_16, 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_set_width(label, 150);
    lv_obj_align_to(label, img, LV_ALIGN_OUT_RIGHT_TOP, 15, 130);
    lv_label_set_recolor(label, true);
    lv_label_set_text_fmt(label, "#000000 %s", DutyStr);

    // ռ�ձȵ���
    lv_obj_t *slider = lv_slider_create(cont);
    lv_obj_align_to(slider, label, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 10);
    lv_obj_add_event_cb(slider, SliderEvent, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_set_width(slider, 200);
    lv_slider_set_range(slider, 1, 100);
    lv_slider_set_value(slider, PWMDutyRatio, LV_ANIM_ON);

    // ��ǩ
    slider_label = lv_label_create(cont);
    lv_label_set_text_fmt(slider_label, "%d%%", PWMDutyRatio);

    lv_obj_align_to(slider_label, slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
}
