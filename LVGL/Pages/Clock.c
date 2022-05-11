/****************************************************************************/
/*                                                                          */
/*    新核科技(广州)有限公司                                                */
/*                                                                          */
/*    Copyright (C) 2022 CoreKernel Technology Guangzhou Co., Ltd           */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/*    时钟                                                                  */
/*                                                                          */
/*    2022年05月10日                                                        */
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

#include "zh-cn.h"  // 汉语文本字符串

/****************************************************************************/
/*                                                                          */
/*              全局变量                                                    */
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
/*              事件                                                        */
/*                                                                          */
/****************************************************************************/
static void WinEvent(lv_event_t *e)
{
    // 删除定时器
    lv_timer_del(timer);

    lv_obj_t *obj = lv_event_get_target(e);
    LV_LOG_USER("Button %d clicked", (int)lv_obj_get_index(obj));

    lv_obj_del(win);
}

/****************************************************************************/
/*                                                                          */
/*              更新时间                                                    */
/*                                                                          */
/****************************************************************************/
static void UpdateTime(lv_timer_t *timer)
{
    lv_label_set_text_fmt(timelabel, "#00BFFF %02d:%02d:%02d", RTCTime.tm_hour, RTCTime.tm_min, RTCTime.tm_sec);
}

/****************************************************************************/
/*                                                                          */
/*              窗口                                                        */
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

    // 模拟时钟
    meter = lv_meter_create(cont);
    lv_obj_set_size(meter, 350, 350);
    lv_obj_align(meter, LV_ALIGN_LEFT_MID, 0, 0);

    // 时刻度
    lv_meter_scale_t *scale_hour = lv_meter_add_scale(meter);
    lv_meter_set_scale_ticks(meter, scale_hour, 12, 0, 0, lv_palette_main(LV_PALETTE_GREY));  // 12 刻度
    lv_meter_set_scale_major_ticks(meter, scale_hour, 1, 4, 20, lv_color_black(), 15);
    lv_meter_set_scale_range(meter, scale_hour, 1, 12, 330, 300);                             // 刻度值

    // 分刻度
    // 360°61 刻度(开始和最后刻度重合)
    lv_meter_scale_t *scale_min = lv_meter_add_scale(meter);
    lv_meter_set_scale_ticks(meter, scale_min, 61, 1, 10, lv_palette_main(LV_PALETTE_GREY));
    lv_meter_set_scale_range(meter, scale_min, 0, 60, 360, 270);

    // 秒刻度
    // 360°61 刻度(开始和最后刻度重合)
    lv_meter_scale_t *scale_sec = lv_meter_add_scale(meter);
    lv_meter_set_scale_ticks(meter, scale_sec, 61, 1, 10, lv_palette_main(LV_PALETTE_GREY));
    lv_meter_set_scale_range(meter, scale_sec, 0, 60, 360, 270);

    // 时针 分针 秒针
    LV_IMG_DECLARE(imgHandHour);
    LV_IMG_DECLARE(imgHandMin);
    LV_IMG_DECLARE(imgHandSec);

    lv_meter_indicator_t *indic_hour = lv_meter_add_needle_img(meter, scale_hour, &imgHandHour, 5, 5);
    lv_meter_indicator_t *indic_min = lv_meter_add_needle_img(meter, scale_min, &imgHandMin, 5, 5);
    lv_meter_indicator_t *indic_sec = lv_meter_add_needle_img(meter, scale_sec, &imgHandSec, 5, 5);

    // 定时更新时间
    lv_meter_set_indicator_end_value(meter, indic_hour, RTCTime.tm_hour);
    lv_meter_set_indicator_end_value(meter, indic_min, RTCTime.tm_min);
    lv_meter_set_indicator_end_value(meter, indic_sec, RTCTime.tm_sec);

    // 数字时钟
    timelabel = lv_label_create(cont);
    lv_obj_set_style_text_font(timelabel, &lv_font_montserrat_48, 0);
    lv_obj_set_style_text_align(timelabel, LV_TEXT_ALIGN_LEFT, 0);                            // 左对齐
    lv_obj_set_width(timelabel, 200);                                                         // 文本宽度
    lv_obj_align(timelabel, LV_ALIGN_TOP_RIGHT, -75, 170);                                    // 位置
    lv_label_set_recolor(timelabel, true);                                                    // 允许修改文本颜色
    lv_label_set_text_fmt(timelabel, "#00BFFF %02d:%02d:%02d", RTCTime.tm_hour, RTCTime.tm_min, RTCTime.tm_sec);

    // 创建定时器
    timer = lv_timer_create(UpdateTime, 1000, NULL);
}
