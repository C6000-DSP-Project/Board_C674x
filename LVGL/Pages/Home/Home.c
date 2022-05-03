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

/****************************************************************************/
/*                                                                          */
/*              全局变量                                                    */
/*                                                                          */
/****************************************************************************/
// 标签
lv_obj_t *time_label;  // 时间标签
lv_obj_t *temp_label;  // 温度湿度标签

/****************************************************************************/
/*                                                                          */
/*              事件                                                        */
/*                                                                          */
/****************************************************************************/
static void time_label_event_cb(lv_event_t *e)
{
    // 创建日历
    lv_obj_t *calendar = lv_calendar_create(lv_scr_act());
    lv_obj_set_size(calendar, 300, 300);                                           // 大小
    lv_obj_align(calendar, LV_ALIGN_CENTER, 0, 0);                                 // 位置

    // 设置日期和显示日期
    lv_calendar_set_today_date(calendar, RTCTime.tm_year + 1920, RTCTime.tm_mon, RTCTime.tm_mday);
    lv_calendar_set_showed_date(calendar, RTCTime.tm_year + 1920, RTCTime.tm_mon);

    // 删除日历
//    lv_obj_del(calendar);
}

/****************************************************************************/
/*                                                                          */
/*              页面                                                        */
/*                                                                          */
/****************************************************************************/
void HomePage()
{
    /* LVGL 对象 */
    // 背景图片
    lv_obj_t *img = lv_img_create(lv_scr_act());
    LV_IMG_DECLARE(img_bg);
    lv_img_set_src(img, &img_bg);
    lv_obj_align(img, LV_ALIGN_TOP_LEFT, 0, 0);

    // 显示时间
    time_label = lv_label_create(lv_scr_act());                                    // 创建标签
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
}
