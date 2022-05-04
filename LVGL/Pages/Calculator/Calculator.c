/****************************************************************************/
/*                                                                          */
/*    新核科技(广州)有限公司                                                */
/*                                                                          */
/*    Copyright (C) 2022 CoreKernel Technology Guangzhou Co., Ltd           */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/*    计算器                                                                */
/*                                                                          */
/*    2022年05月04日                                                        */
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

#include "../zh-cn.h"  // 汉语文本字符串

/****************************************************************************/
/*                                                                          */
/*              全局变量                                                    */
/*                                                                          */
/****************************************************************************/
static lv_obj_t *win;

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
void CalculatorWin()
{
    win = lv_win_create(lv_scr_act(), 40);

    lv_obj_t *lable;
    lable = lv_win_add_title(win, CalculatorStr);

    lv_obj_t *btn;
    btn = lv_win_add_btn(win, LV_SYMBOL_CLOSE, 40);
    lv_obj_add_event_cb(btn, WinEvent, LV_EVENT_CLICKED, NULL);

    lv_obj_t *cont = lv_win_get_content(win);
    lv_obj_t *label = lv_label_create(cont);
    lv_label_set_text(label, "This is\n"
                             "a pretty\n"
                             "long text\n"
                             "to see how\n"
                             "the window\n"
                             "becomes\n"
                             "scrollable.\n"
                             "\n"
                             "\n"
                             "Some more\n"
                             "text to be\n"
                             "sure it\n"
                             "overflows. :)");
}
