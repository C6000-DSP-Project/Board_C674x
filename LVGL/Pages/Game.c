/****************************************************************************/
/*                                                                          */
/*    新核科技(广州)有限公司                                                */
/*                                                                          */
/*    Copyright (C) 2022 CoreKernel Technology Guangzhou Co., Ltd           */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/*    游戏手柄                                                              */
/*                                                                          */
/*    2022年05月20日                                                        */
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

#include "../Periph/SPI/GamePad/PS2.h"

/****************************************************************************/
/*                                                                          */
/*              全局变量                                                    */
/*                                                                          */
/****************************************************************************/
static lv_obj_t *win;

static lv_obj_t *SELECTLabel;
static lv_obj_t *STARTLabel;
static lv_obj_t *UPLabel;
static lv_obj_t *DOWNLabel;
static lv_obj_t *LEFTLabel;
static lv_obj_t *RIGHTLabel;
static lv_obj_t *L1Label;
static lv_obj_t *L2Label;
static lv_obj_t *L3Label;
static lv_obj_t *R1Label;
static lv_obj_t *R2Label;
static lv_obj_t *R3Label;
static lv_obj_t *GREENLabel;
static lv_obj_t *REDLabel;
static lv_obj_t *BLUELabel;
static lv_obj_t *PINKLabel;

/****************************************************************************/
/*                                                                          */
/*              事件                                                        */
/*                                                                          */
/****************************************************************************/
static void WinEvent(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_target(e);
    LV_LOG_USER("Button %d clicked", (int)lv_obj_get_index(obj));

    lv_obj_del(win);
}

/****************************************************************************/
/*                                                                          */
/*              窗口                                                        */
/*                                                                          */
/****************************************************************************/
void GameWin()
{
    win = lv_win_create(lv_scr_act(), 40);

    lv_obj_t *label;
    label = lv_win_add_title(win, GameStr);

    lv_obj_t *btn;
    btn = lv_win_add_btn(win, LV_SYMBOL_CLOSE, 40);
    lv_obj_add_event_cb(btn, WinEvent, LV_EVENT_CLICKED, NULL);

    lv_obj_t *cont = lv_win_get_content(win);

    // PS2 初始化
    PS2Init();

    // PS2 按键
    // 标签样式
    static lv_style_t LableStyle;
    lv_style_reset(&LableStyle);
    lv_style_init(&LableStyle);
    lv_style_set_bg_opa(&LableStyle, LV_OPA_COVER);
    lv_style_set_border_width(&LableStyle, 0);
    lv_style_set_radius(&LableStyle, 2);
    lv_style_set_bg_color(&LableStyle, lv_palette_main(LV_PALETTE_BLUE));
    lv_style_set_text_font(&LableStyle, &lv_font_montserrat_28);
    lv_style_set_text_color(&LableStyle, lv_color_white());
    lv_style_set_text_align(&LableStyle, LV_TEXT_ALIGN_CENTER);
    lv_style_set_width(&LableStyle, 150);
    lv_style_set_height(&LableStyle, LV_SIZE_CONTENT);

    // SELECT
    SELECTLabel = lv_label_create(cont);
    lv_obj_add_style(SELECTLabel, &LableStyle, LV_STATE_DEFAULT);
    lv_obj_align(SELECTLabel, LV_ALIGN_CENTER, 100, 200);
    lv_label_set_recolor(SELECTLabel, true);
    lv_label_set_text_fmt(SELECTLabel, "SELECT");
}
