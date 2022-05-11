/****************************************************************************/
/*                                                                          */
/*    新核科技(广州)有限公司                                                */
/*                                                                          */
/*    Copyright (C) 2022 CoreKernel Technology Guangzhou Co., Ltd           */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/*    LVGL 页面                                                             */
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

#include "zh-cn.h"  // 汉语文本字符串

/****************************************************************************/
/*                                                                          */
/*              全局变量                                                    */
/*                                                                          */
/****************************************************************************/
// 标签
lv_obj_t *time_label;  // 时间标签
lv_obj_t *temp_label;  // 温度湿度标签

// 旋转角度
uint8_t roate = LV_DISP_ROT_90;

/****************************************************************************/
/*                                                                          */
/*              函数声明                                                    */
/*                                                                          */
/****************************************************************************/
extern void CalculatorWin();
extern void SystemInfoWin();
extern void LANWin();
extern void WLANWin();
extern void LCDBrightnessWin();
extern void ClockWin();
extern void CalendarWin();
extern void FanWin();

/****************************************************************************/
/*                                                                          */
/*              事件                                                        */
/*                                                                          */
/****************************************************************************/
static void RoateEvent(lv_event_t *e)
{
    lv_event_code_t event = lv_event_get_code(e);

    if(event == LV_EVENT_CLICKED)
    {
        lv_disp_set_rotation(lv_scr_act(), roate);
        (roate == 3) ? roate = 0 : roate++;
    }
}

static void CalculatorEvent(lv_event_t *e)
{
    lv_event_code_t event = lv_event_get_code(e);

    if(event == LV_EVENT_CLICKED)
    {
        CalculatorWin();
    }
}

static void SysteminfoEvent(lv_event_t *e)
{
    lv_event_code_t event = lv_event_get_code(e);

    if(event == LV_EVENT_CLICKED)
    {
        SystemInfoWin();
    }
}

static void LANEvent(lv_event_t *e)
{
    lv_event_code_t event = lv_event_get_code(e);

    if(event == LV_EVENT_CLICKED)
    {
        LANWin();
    }
}

static void WLANEvent(lv_event_t *e)
{
    lv_event_code_t event = lv_event_get_code(e);

    if(event == LV_EVENT_CLICKED)
    {
        WLANWin();
    }
}

static void FileEvent(lv_event_t *e)
{
    lv_event_code_t event = lv_event_get_code(e);

    if(event == LV_EVENT_CLICKED)
    {
        WLANWin();
    }
}

static void LCDBrightnessEvent(lv_event_t *e)
{
    lv_event_code_t event = lv_event_get_code(e);

    if(event == LV_EVENT_CLICKED)
    {
        LCDBrightnessWin();
    }
}

static void ClockEvent(lv_event_t *e)
{
    lv_event_code_t event = lv_event_get_code(e);

    if(event == LV_EVENT_CLICKED)
    {
        ClockWin();
    }
}

static void CalendarEvent(lv_event_t *e)
{
    lv_event_code_t event = lv_event_get_code(e);

    if(event == LV_EVENT_CLICKED)
    {
        CalendarWin();
    }
}

static void FanEvent(lv_event_t *e)
{
    lv_event_code_t event = lv_event_get_code(e);

    if(event == LV_EVENT_CLICKED)
    {
        FanWin();
    }
}

/****************************************************************************/
/*                                                                          */
/*              页面                                                        */
/*                                                                          */
/****************************************************************************/
// 桌面图标
typedef struct
{
    // 位置坐标
    unsigned short x;
    unsigned short y;
    // 按下状态图标
    const lv_img_dsc_t *imgPressed;
    // 释放状态图标
    const lv_img_dsc_t *imgRelesed;
    // 图标文字
    char *text;
    // 回调函数
    lv_event_cb_t event_cb;
} DeskIcon;

LV_IMG_DECLARE(imgSystemInfo);
LV_IMG_DECLARE(imgRoate);
LV_IMG_DECLARE(imgCalculator);
LV_IMG_DECLARE(imgLAN);
LV_IMG_DECLARE(imgWLAN);
LV_IMG_DECLARE(imgCalendar);
LV_IMG_DECLARE(imgFile);
LV_IMG_DECLARE(imgSnake);
LV_IMG_DECLARE(imgTetris);
LV_IMG_DECLARE(imgTouch);
LV_IMG_DECLARE(imgGame);
LV_IMG_DECLARE(imgClock);
LV_IMG_DECLARE(imgBrightness);
LV_IMG_DECLARE(imgFan);

DeskIcon icon[] =
{
    { 80,  100, &imgSystemInfo, &imgSystemInfo, SystemInfoStr, SysteminfoEvent},
    {200,  100, &imgRoate, &imgRoate, RoateStr, RoateEvent},
    {320,  100, &imgCalculator, &imgCalculator, CalculatorStr, CalculatorEvent},
    {440,  100, &imgLAN, &imgLAN, LANStr, LANEvent},
    {560,  100, &imgWLAN, &imgWLAN, WLANStr, WLANEvent},
    {680,  100, &imgFile, &imgFile, FileStr, FileEvent},
    { 80,  220, &imgCalendar, &imgCalendar, CalendarStr, CalendarEvent},
    {200,  220, &imgClock, &imgClock, ClockStr, ClockEvent},
    {320,  220, &imgBrightness, &imgBrightness, LCDBrightnessStr, LCDBrightnessEvent},
    {440,  220, &imgTouch, &imgTouch, TouchStr, FileEvent},
    {560,  220, &imgFan, &imgFan, FanStr, FanEvent},
    { 80,  340, &imgGame, &imgGame, GameStr, FileEvent},
    {200,  340, &imgSnake, &imgSnake, SnakeStr, FileEvent},
    {320,  340, &imgTetris, &imgTetris, TetrisStr, FileEvent},
};

void HomePage()
{
    /* LVGL 对象 */
    // 背景图片
    lv_obj_t *img = lv_img_create(lv_scr_act());
    LV_IMG_DECLARE(imgBackground);
    lv_img_set_src(img, &imgBackground);
    lv_obj_align(img, LV_ALIGN_TOP_LEFT, 0, 0);

    // 显示时间
    time_label = lv_label_create(lv_scr_act());                                    // 创建标签
    lv_obj_set_style_text_font(time_label, &SourceHanSansCN_Normal_16, 0);
    lv_obj_set_style_text_align(time_label, LV_TEXT_ALIGN_LEFT, 0);                // 左对齐
    lv_label_set_long_mode(time_label, LV_LABEL_LONG_SCROLL);                      // 长文本滚动
    lv_obj_set_width(time_label, 230);                                             // 文本宽度
    lv_obj_align(time_label, LV_ALIGN_TOP_RIGHT, 15, 15);                          // 位置
    lv_label_set_recolor(time_label, true);                                        // 允许修改文本颜色

    // 显示温湿度
    temp_label = lv_label_create(lv_scr_act());
    lv_obj_set_style_text_font(temp_label, &SourceHanSansCN_Normal_16, 0);
    lv_obj_set_width(temp_label, 100);
    lv_obj_align(temp_label, LV_ALIGN_BOTTOM_LEFT, 5, -5);
    lv_label_set_recolor(temp_label, true);

    // 桌面图标
    int i;

    lv_obj_t *imgbtn;
    lv_obj_t *label;

    for(i = 0; i < sizeof(icon) / sizeof(DeskIcon); i++)
    {
        imgbtn = lv_imgbtn_create(lv_scr_act());
        lv_obj_set_size(imgbtn, icon[i].imgRelesed->header.w, icon[i].imgRelesed->header.h);
        lv_obj_set_pos(imgbtn, icon[i].x, icon[i].y);
        lv_imgbtn_set_src(imgbtn, LV_IMGBTN_STATE_RELEASED, NULL, icon[i].imgRelesed, NULL);
        lv_imgbtn_set_src(imgbtn, LV_IMGBTN_STATE_PRESSED, NULL, icon[i].imgPressed, NULL);
        lv_obj_add_event_cb(imgbtn, icon[i].event_cb, LV_EVENT_CLICKED, NULL);

        label = lv_label_create(lv_scr_act());
        lv_obj_set_style_text_font(label, &SourceHanSansCN_Normal_16, 0);
        lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_LEFT, 0);
        lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL);
        lv_obj_set_width(label, 230);
        lv_obj_align_to(label, imgbtn, LV_ALIGN_OUT_BOTTOM_LEFT, -8, 10);
        lv_label_set_recolor(label, true);
        lv_label_set_text_fmt(label, "#000000 %s#", icon[i].text);
    }
}
