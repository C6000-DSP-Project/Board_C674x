/****************************************************************************/
/*                                                                          */
/*    �º˿Ƽ�(����)���޹�˾                                                */
/*                                                                          */
/*    Copyright (C) 2022 CoreKernel Technology Guangzhou Co., Ltd           */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/*    ������                                                                */
/*                                                                          */
/*    2022��05��04��                                                        */
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
/*              �궨��                                                      */
/*                                                                          */
/****************************************************************************/
#define WinHeaderHeight     40

/****************************************************************************/
/*                                                                          */
/*              ȫ�ֱ���                                                    */
/*                                                                          */
/****************************************************************************/
// ����
static lv_obj_t *win;

// DHCP ģʽ
extern char DHCPMode;              // DHCP ģʽ
extern char *LocalIPAddr;          // DHCP IP ��ַ
extern char StaticIPAddr[16];      // ��̬ IP ��ַ
extern char LocalIPMask[16];       // ��̬ ��������
extern char GatewayIP[16];         // ��̬ ����
extern char *DNSServer;            // DNS ������

lv_obj_t *DHCPSw;

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

static void DHCPSwEvent(lv_event_t *e)
{
    DHCPMode = lv_obj_has_state(DHCPSw, LV_STATE_CHECKED);
}

static void taEvent(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);            // ��ȡ�¼�����
    lv_obj_t *ta = lv_event_get_target(e);                  // ��ȡ�¼����ָ��Ķ���
    lv_obj_t *kb = lv_event_get_user_data(e);               // ��ȡ�ڶ�����ע���¼�ʱ���ݵ��û�����(user_data)

    // �ж��¼�����(�������ڼ�����ʱ)
    if(code == LV_EVENT_FOCUSED)
    {
        // ��ȡ�����豸������
        if(lv_indev_get_type(lv_indev_get_act()) != LV_INDEV_TYPE_KEYPAD)
        {
            lv_keyboard_set_textarea(kb, ta);
            lv_obj_set_style_max_height(kb, LV_HOR_RES, 0);
            lv_obj_update_layout(win);
            lv_obj_set_height(win, LV_VER_RES - lv_obj_get_height(kb));
            lv_obj_clear_flag(kb, LV_OBJ_FLAG_HIDDEN);
            lv_obj_scroll_to_view_recursive(ta, LV_ANIM_ON);
        }
    }
    // �ж��¼�����(�����㲻�ڼ�����ʱ)
    else if(code == LV_EVENT_DEFOCUSED)
    {
        lv_keyboard_set_textarea(kb, NULL);
        lv_obj_set_height(win, LV_VER_RES);
        lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
    }
    // �ж��¼�����(�����̵��ȷ����ȡ����)
    else if(code == LV_EVENT_READY || code == LV_EVENT_CANCEL)
    {
        lv_obj_set_height(win, LV_VER_RES);
        lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);            // ��־Ϊ���ض���
        lv_obj_clear_state(e->target, LV_STATE_FOCUSED);    // ɾ�������һ�ֻ����״̬ ����״̬λ�����ֲ���
        lv_indev_reset(NULL, e->target);                    // �������һ�ε���Ķ��� ʹ���ٴγ�Ϊ�ɹ�ע�Ķ���
    }
}

static void NETbtnEvent(lv_event_t *e)
{

}

/****************************************************************************/
/*                                                                          */
/*              ����                                                        */
/*                                                                          */
/****************************************************************************/
void LANWin()
{
    // ����
    win = lv_win_create(lv_scr_act(), WinHeaderHeight);

    lv_obj_t *label;
    label = lv_win_add_title(win, LANStr);

    lv_obj_t *btn;
    btn = lv_win_add_btn(win, LV_SYMBOL_CLOSE, 40);
    lv_obj_add_event_cb(btn, WinEvent, LV_EVENT_CLICKED, NULL);

    // ��ǩҳ
    lv_obj_t *tabview;
    tabview = lv_tabview_create(win, LV_DIR_LEFT, 100);
    lv_obj_set_size(tabview, LV_HOR_RES, LV_VER_RES - WinHeaderHeight);

    // ��ӱ�ǩ
    lv_obj_t *tab1 = lv_tabview_add_tab(tabview, LANCFGStr);
    lv_obj_t *tab2 = lv_tabview_add_tab(tabview, LANTCPStr);
    lv_obj_t *tab3 = lv_tabview_add_tab(tabview, LANUDPStr);

    // ��ǩҳ����
    // �������ñ�ǩҳ
    // ���񲼾�
    static lv_coord_t col_dsc[] = {100, 200, LV_GRID_TEMPLATE_LAST};
    static lv_coord_t row_dsc[] = {40, 40, 40, 40, 40, 40, LV_GRID_TEMPLATE_LAST};

    lv_obj_t *cont = lv_obj_create(tab1);
    lv_obj_set_style_grid_column_dsc_array(cont, col_dsc, 0);
    lv_obj_set_style_grid_row_dsc_array(cont, row_dsc, 0);
    lv_obj_set_size(cont, 500, 350);
    lv_obj_set_layout(cont, LV_LAYOUT_GRID);
    lv_obj_set_grid_align(cont, LV_GRID_ALIGN_START, LV_GRID_ALIGN_START);

    // ���񲼾�����
    // ��ǩҳ 1
    // DHCP ģʽ
    label = lv_label_create(cont);
    lv_obj_center(label);
    lv_label_set_text(label, DHCPStr);
    lv_obj_set_grid_cell(label, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 0, 1);

    DHCPSw = lv_switch_create(cont);
    lv_obj_set_pos(DHCPSw, 75, 8);
    lv_obj_add_event_cb(DHCPSw, DHCPSwEvent, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_set_grid_cell(DHCPSw, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 0, 1);

    // IP ��ַ
    //  ��������
    lv_obj_t *kb = lv_keyboard_create(lv_scr_act());
    lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
//    lv_keyboard_set_mode(kb, LV_KEYBOARD_MODE_NUMBER);
//    lv_obj_set_size(kb, 400, 300);

    label = lv_label_create(cont);
    lv_obj_center(label);
    lv_label_set_text(label, IPAddrStr);
    lv_obj_set_grid_cell(label, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 1, 1);

    lv_obj_t *ip_ta = lv_textarea_create(cont);
    lv_obj_set_grid_cell(ip_ta, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
    lv_textarea_set_one_line(ip_ta, true);
    lv_textarea_set_password_mode(ip_ta, false);
    lv_obj_set_width(ip_ta, lv_pct(40));
    lv_obj_align(ip_ta, LV_ALIGN_TOP_RIGHT, -5, 20);
    lv_textarea_set_text(ip_ta, StaticIPAddr);
    lv_obj_add_event_cb(ip_ta, taEvent, LV_EVENT_ALL, kb);

    // ��������
    label = lv_label_create(cont);
    lv_obj_center(label);
    lv_label_set_text(label, SubMaskStr);
    lv_obj_set_grid_cell(label, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 2, 1);

    lv_obj_t *submask_ta = lv_textarea_create(cont);
    lv_obj_set_grid_cell(submask_ta, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 2, 1);
    lv_textarea_set_one_line(submask_ta, true);
    lv_textarea_set_password_mode(submask_ta, false);
    lv_obj_set_width(submask_ta, lv_pct(40));
    lv_obj_align(submask_ta, LV_ALIGN_TOP_RIGHT, -5, 20);
    lv_textarea_set_text(submask_ta, LocalIPMask);
    lv_obj_add_event_cb(submask_ta, taEvent, LV_EVENT_ALL, kb);

    // ����
    label = lv_label_create(cont);
    lv_obj_center(label);
    lv_label_set_text(label, GateWayStr);
    lv_obj_set_grid_cell(label, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 3, 1);

    lv_obj_t *gateway_ta = lv_textarea_create(cont);
    lv_obj_set_grid_cell(gateway_ta, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 3, 1);
    lv_textarea_set_one_line(gateway_ta, true);
    lv_textarea_set_password_mode(gateway_ta, false);
    lv_obj_set_width(gateway_ta, lv_pct(40));
    lv_obj_align(gateway_ta, LV_ALIGN_TOP_RIGHT, -5, 20);
    lv_textarea_set_text(gateway_ta, GatewayIP);
    lv_obj_add_event_cb(gateway_ta, taEvent, LV_EVENT_ALL, kb);

    // DNS ������
    label = lv_label_create(cont);
    lv_obj_center(label);
    lv_label_set_text(label, DNSServerStr);
    lv_obj_set_grid_cell(label, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 4, 1);

    lv_obj_t *dnsserver_ta = lv_textarea_create(cont);
    lv_obj_set_grid_cell(dnsserver_ta, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 4, 1);
    lv_textarea_set_one_line(dnsserver_ta, true);
    lv_textarea_set_password_mode(dnsserver_ta, false);
    lv_obj_set_width(dnsserver_ta, lv_pct(40));
    lv_obj_align(dnsserver_ta, LV_ALIGN_TOP_RIGHT, -5, 20);
    lv_textarea_set_text(dnsserver_ta, DNSServer);
    lv_obj_add_event_cb(dnsserver_ta, taEvent, LV_EVENT_ALL, kb);

    // �����ʼ����ť
    lv_obj_t *NETbtn = lv_btn_create(cont);
    lv_obj_set_grid_cell(NETbtn, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 5, 1);
    lv_obj_add_event_cb(NETbtn, NETbtnEvent, LV_EVENT_CLICKED, NULL);
    lv_obj_align(NETbtn, LV_ALIGN_CENTER, 0, -40);

    label = lv_label_create(NETbtn);
    lv_label_set_text(label, NETInitStr);
    lv_obj_center(label);

    // ��ǩҳ 2
    label = lv_label_create(tab2);
    lv_label_set_text(label, "None");

    // ��ǩҳ 3
    label = lv_label_create(tab3);
    lv_label_set_text(label, "None");

//    lv_obj_scroll_to_view_recursive(label, LV_ANIM_ON);
}

void WLANWin()
{
    win = lv_win_create(lv_scr_act(), 40);

    lv_obj_t *lable;
    lable = lv_win_add_title(win, WLANStr);

    lv_obj_t *btn;
    btn = lv_win_add_btn(win, LV_SYMBOL_CLOSE, 40);
    lv_obj_add_event_cb(btn, WinEvent, LV_EVENT_CLICKED, NULL);
}
