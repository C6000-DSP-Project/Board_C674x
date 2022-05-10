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

// ��������
static lv_calendar_date_t highlighted_days[2];

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

static void CalendarEvent(lv_event_t *e)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_current_target(e);

    if(event == LV_EVENT_VALUE_CHANGED)
    {
        lv_calendar_date_t date;

        if(lv_calendar_get_pressed_date(obj, &date))
        {
            highlighted_days[1].year = date.year;
            highlighted_days[1].month = date.month;
            highlighted_days[1].day = date.day;

            lv_calendar_set_highlighted_dates(obj, highlighted_days, 2);
        }
    }
}

/****************************************************************************/
/*                                                                          */
/*              ����                                                        */
/*                                                                          */
/****************************************************************************/
void CalendarWin()
{
    win = lv_win_create(lv_scr_act(), 40);

    lv_obj_t *label;
    label = lv_win_add_title(win, CalendarTitleStr);

    lv_obj_t *btn;
    btn = lv_win_add_btn(win, LV_SYMBOL_CLOSE, 40);
    lv_obj_add_event_cb(btn, WinEvent, LV_EVENT_CLICKED, NULL);

    lv_obj_t *cont = lv_win_get_content(win);

    // ����
    lv_obj_t *calendar = lv_calendar_create(cont);
    lv_obj_set_size(calendar, 500, 400);
    lv_obj_align(calendar, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_add_event_cb(calendar, CalendarEvent, LV_EVENT_ALL, NULL);
    lv_calendar_header_dropdown_create(calendar);

    // ��������
    static const char *day_names[7] = {MondayStr, TuesdayStr, WednesdayStr, ThurdayStr, FridayStr, SaturdayStr, SundayStr};
    lv_calendar_set_day_names(calendar, day_names);

    // ��ʾ����
    lv_calendar_set_today_date(calendar, RTCTime.tm_year + 1920, RTCTime.tm_mon, RTCTime.tm_mday);
    lv_calendar_set_showed_date(calendar, RTCTime.tm_year + 1920, RTCTime.tm_mon);

    // ��������
    highlighted_days[0].year = RTCTime.tm_year + 1920;
    highlighted_days[0].month = RTCTime.tm_mon;
    highlighted_days[0].day = RTCTime.tm_mday;

    lv_calendar_set_highlighted_dates(calendar, highlighted_days, 1);

    // ��ǩ
    label = lv_label_create(cont);

    LV_FONT_DECLARE(SourceHanSansCN_Normal_32)
    lv_obj_set_style_text_font(label, &SourceHanSansCN_Normal_32, 0);

    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);              // �����
    lv_obj_set_width(label, 100);                                             // �ı����
    lv_obj_align(label, LV_ALIGN_TOP_RIGHT, -70, 50);                         // λ��
    lv_label_set_recolor(label, true);                                        // �����޸��ı���ɫ
    lv_label_set_text_fmt(label, "#000000 %s", TodayStr);

    label = lv_label_create(cont);
    lv_obj_set_style_text_font(label, &SourceHanSansCN_Normal_16, 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);              // �����
    lv_obj_set_width(label, 100);                                             // �ı����
    lv_obj_align(label, LV_ALIGN_TOP_RIGHT, -70, 100);                        // λ��
    lv_label_set_recolor(label, true);                                        // �����޸��ı���ɫ
    lv_label_set_text_fmt(label, "#000000 %d%s%d%s", RTCTime.tm_year + 1920, YearStr, RTCTime.tm_mon, MonthStr);

    label = lv_label_create(cont);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_48, 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);              // �����
    lv_obj_set_width(label, 100);                                             // �ı����
    lv_obj_align(label, LV_ALIGN_TOP_RIGHT, -70, 130);                        // λ��
    lv_label_set_recolor(label, true);                                        // �����޸��ı���ɫ
    lv_label_set_text_fmt(label, "#000000 %d", RTCTime.tm_mday);

    label = lv_label_create(cont);
    lv_obj_set_style_text_font(label, &SourceHanSansCN_Normal_32, 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);              // �����
    lv_obj_set_width(label, 100);                                             // �ı����
    lv_obj_align(label, LV_ALIGN_TOP_RIGHT, -70, 200);                        // λ��
    lv_label_set_recolor(label, true);                                        // �����޸��ı���ɫ

    // ����
    char *WeekDayStr[] = {SundayStr, MondayStr, TuesdayStr, WednesdayStr, ThurdayStr, FridayStr, SaturdayStr};
    lv_label_set_text_fmt(label, "#000000 %s", WeekDayStr[RTCTime.tm_wday]);
}
