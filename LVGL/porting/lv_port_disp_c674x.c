/****************************************************************************/
/*                                                                          */
/*    新核科技(广州)有限公司                                                */
/*                                                                          */
/*    Copyright (C) 2022 CoreKernel Technology (Guangzhou) Co., Ltd         */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/*    LVGL DSP C6748 底层接口移植                                           */
/*                                                                          */
/*    2022年04月06日                                                        */
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
#if 1

#include "lv_port_disp_c674x.h"

#include "LCD/LCD.h"

#include <xdc/std.h>

#include <ti/sysbios/family/c64p/cache.h>

#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>

/****************************************************************************/
/*                                                                          */
/*              函数声明                                                    */
/*                                                                          */
/****************************************************************************/
static void disp_init(void);
static void disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p);
static void disp_monitor_cb(lv_disp_drv_t *disp_drv, uint32_t time, uint32_t px);

/****************************************************************************/
/*                                                                          */
/*              线程                                                        */
/*                                                                          */
/****************************************************************************/
Void LVGLTick(UArg arg)
{
    // 1ms 周期
    lv_tick_inc(1);
}

/*声明字库*/
LV_FONT_DECLARE(lv_font_source_han_sans_bold_14);

static void ta_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);
    lv_obj_t * kb = lv_event_get_user_data(e);

    if(code == LV_EVENT_FOCUSED) {
        if(lv_indev_get_type(lv_indev_get_act()) != LV_INDEV_TYPE_KEYPAD) {
            lv_keyboard_set_textarea(kb, ta);
            lv_obj_set_style_max_height(kb, LV_HOR_RES * 2 / 3, 0);
            lv_obj_clear_flag(kb, LV_OBJ_FLAG_HIDDEN);
            lv_obj_scroll_to_view_recursive(ta, LV_ANIM_OFF);
        }
    }
    else if(code == LV_EVENT_READY || code == LV_EVENT_CANCEL) {
        lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_state(ta, LV_STATE_FOCUSED);
        lv_indev_reset(NULL, ta);   /*To forget the last clicked object to make it focusable again*/
    }
}

void your_test_function(void)
{
    lv_obj_t * kb = lv_chinese_ime_create(lv_scr_act());
    lv_chinese_ime_set_text_font(&lv_font_source_han_sans_bold_14, 0);   // 使用自定义的字库
    //lv_chinese_ime_set_dict(your_dict); // 使用自定义的词库，如果不设置将使用内置词库


    /* ta1 */
    lv_obj_t * ta1 = lv_textarea_create(lv_scr_act());
    lv_obj_set_style_text_font(ta1, &lv_font_source_han_sans_bold_14, 0);   // 使用自定义的字库
    lv_obj_align(ta1, LV_ALIGN_TOP_LEFT, 10, 10);

    lv_keyboard_set_textarea(kb, ta1);

    lv_obj_add_event_cb(ta1, ta_event_cb, LV_EVENT_ALL, kb);

    /* ta2 */
    lv_obj_t * ta2 = lv_textarea_create(lv_scr_act());
    lv_obj_set_style_text_font(ta2, &lv_font_source_han_sans_bold_14, 0);   // 使用自定义的字库
    lv_obj_align_to(ta2, ta1,LV_ALIGN_OUT_RIGHT_MID, 10, 0);

    lv_obj_add_event_cb(ta2, ta_event_cb, LV_EVENT_ALL, kb);
}

extern void UARTprintf(const char *fmt, ...);

Void LVGLTask(UArg a0, UArg a1)
{
    lv_log_register_print_cb(UARTprintf);

    your_test_function();
//    lv_example_get_started_1();
//    lv_example_keyboard_1();
//    lv_demo_benchmark();
//    lv_demo_music();

    for(;;)
    {
        lv_timer_handler();
        Task_sleep(5);
    }
}

/****************************************************************************/
/*                                                                          */
/*              LVGL 显示初始化                                             */
/*                                                                          */
/****************************************************************************/
void lv_port_disp_init(void)
{
    /*-------------------------
     * LCD 控制器初始化
     * -----------------------*/
    disp_init();

    /*-----------------------------
     * 创建显示缓冲区
     *----------------------------*/

    /**
     * LVGL requires a buffer where it internally draws the widgets.
     * Later this buffer will passed to your display driver's `flush_cb` to copy its content to your display.
     * The buffer has to be greater than 1 display row
     *
     * There are 3 buffering configurations:
     * 1. Create ONE buffer:
     *      LVGL will draw the display's content here and writes it to your display
     *
     * 2. Create TWO buffer:
     *      LVGL will draw the display's content to a buffer and writes it your display.
     *      You should use DMA to write the buffer's content to the display.
     *      It will enable LVGL to draw the next part of the screen to the other buffer while
     *      the data is being sent form the first buffer. It makes rendering and flushing parallel.
     *
     * 3. Double buffering
     *      Set 2 screens sized buffers and set disp_drv.full_refresh = 1.
     *      This way LVGL will always provide the whole rendered screen in `flush_cb`
     *      and you only need to change the frame buffer's address.
     */

    /* 双显示缓冲区 */
    static lv_disp_draw_buf_t draw_buf_dsc;
    static lv_color_t buf_1[LCD_WIDTH * 10];  /* 10 行 */
    static lv_color_t buf_2[LCD_WIDTH * 10];  /* 10 行 */
    lv_disp_draw_buf_init(&draw_buf_dsc, buf_1, buf_2, LCD_WIDTH * 10);

    /*-----------------------------------
     * 注册 LVGL 显示设备
     *----------------------------------*/
    static lv_disp_drv_t disp_drv;            /* 显示驱动描述符 */
    lv_disp_drv_init(&disp_drv);

    /* 设置屏幕分辨率 */
    disp_drv.hor_res = LCD_WIDTH;
    disp_drv.ver_res = LCD_HEIGHT;

    /* 回调函数 */
    disp_drv.flush_cb = disp_flush;

    /* 配置显示缓冲区 */
    disp_drv.draw_buf = &draw_buf_dsc;

    /* 强制全屏刷新 */
    //disp_drv.full_refresh = 1;

    /* 旋转 */
    //disp_drv.rotated = LV_DISP_ROT_180;


    /* 注册驱动 */
    lv_disp_drv_register(&disp_drv);
}

/****************************************************************************/
/*                                                                          */
/*              显示控制器初始化                                            */
/*                                                                          */
/****************************************************************************/
static void disp_init(void)
{
    /* TODO 在外部初始化 LCD 控制器 */

    /* 时钟初始化 */
    Clock_Params ClkParams;
    Clock_Params_init(&ClkParams);

    ClkParams.period = 1;
    ClkParams.startFlag = TRUE;
    Clock_create(LVGLTick, 1000, &ClkParams, NULL);

    /* 任务初始化 */
    Task_Params taskParams;
    Task_Params_init(&taskParams);

    taskParams.stackSize = 128 * 1024;
    Task_create(LVGLTask, &taskParams, NULL);
}

/****************************************************************************/
/*                                                                          */
/*              回调函数                                                    */
/*                                                                          */
/****************************************************************************/
static void disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
    int32_t len = (area->y2 - area->y1 + 1) * (area->x2 - area->x1 + 1) * sizeof(unsigned short);

    /* 维护缓存一致性 */
//    Cache_inv(color_p, len, Cache_Type_ALLD, true);
    Cache_wbInvAll();

    unsigned short *LCDBuf = (unsigned short *)(g_pucBuffer + (PALETTE_OFFSET + PALETTE_SIZE));

    int32_t y;
    for(y = area->y1; y <= area->y2; y++)
    {
        memcpy((void *)&LCDBuf[y * LCD_WIDTH + area->x1], color_p, (area->x2 - area->x1 + 1) * sizeof(unsigned short));
        color_p += area->x2 - area->x1 + 1;
    }

    /* 维护缓存一致性 */
    Cache_wbInvAll();

    /* 修复画面错位 */
    RasterDisable(SOC_LCDC_0_REGS);
    RasterEnable(SOC_LCDC_0_REGS);

//    Cache_wb(&LCDBuf, (LCD_WIDTH * LCD_HEIGHT) * sizeof(unsigned short), Cache_Type_ALLD, true);

    /* 重要!!!
     * 通知图形库显示更新就绪 */
    lv_disp_flush_ready(disp_drv);
}

#else /*Enable this file at the top*/

/*This dummy typedef exists purely to silence -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif
