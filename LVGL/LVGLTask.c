/****************************************************************************/
/*                                                                          */
/*    �º˿Ƽ�(����)���޹�˾                                                */
/*                                                                          */
/*    Copyright (C) 2022 CoreKernel Technology Guangzhou Co., Ltd           */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/*    LVGL ��ʾ����                                                         */
/*                                                                          */
/*    2022��05��02��                                                        */
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

/****************************************************************************/
/*                                                                          */
/*              ��������                                                    */
/*                                                                          */
/****************************************************************************/
extern void UARTprintf(const char *fmt, ...);

/****************************************************************************/
/*                                                                          */
/*              �����߳�                                                    */
/*                                                                          */
/****************************************************************************/
Void LVGLTask(UArg a0, UArg a1)
{
    // ������־���
    lv_log_register_print_cb((lv_log_print_g_cb_t)UARTprintf);

    // LVGL ����
    lv_obj_t * cont = lv_obj_create(lv_scr_act());
    lv_obj_set_style_base_dir(cont, LV_BASE_DIR_RTL, 0);
    lv_obj_set_size(cont, 300, 220);
    lv_obj_center(cont);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW_WRAP);

    uint32_t i;
    for(i = 0; i < 20; i++) {
        lv_obj_t * obj = lv_obj_create(cont);
        lv_obj_set_size(obj, 70, LV_SIZE_CONTENT);

        lv_obj_t * label = lv_label_create(obj);
        lv_label_set_text_fmt(label, "%"LV_PRIu32, i);
        lv_obj_center(label);
    }

//    your_test_function();
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
