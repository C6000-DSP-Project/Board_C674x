/****************************************************************************/
/*                                                                          */
/*    �º˿Ƽ�(����)���޹�˾                                                */
/*                                                                          */
/*    Copyright (C) 2022 CoreKernel Technology Guangzhou Co., Ltd           */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/*    ��Ϸ�ֱ�                                                              */
/*                                                                          */
/*    2022��05��20��                                                        */
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

#include "../Periph/SPI/GamePad/PS2.h"

/****************************************************************************/
/*                                                                          */
/*              ȫ�ֱ���                                                    */
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
/*              �¼�                                                        */
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
/*              ����                                                        */
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

    // PS2 ��ʼ��
    PS2Init();

    // PS2 ����
    // ��ǩ��ʽ
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
