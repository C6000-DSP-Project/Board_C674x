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

#include "zh-cn.h"  // 汉语文本字符串

/****************************************************************************/
/*                                                                          */
/*              全局变量                                                    */
/*                                                                          */
/****************************************************************************/
// 窗口
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
void SystemInfoWin()
{
    win = lv_win_create(lv_scr_act(), 40);

    lv_obj_t *lable;
    lable = lv_win_add_title(win, SystemInfoStr);

    lv_obj_t *btn;
    btn = lv_win_add_btn(win, LV_SYMBOL_CLOSE, 40);
    lv_obj_add_event_cb(btn, WinEvent, LV_EVENT_CLICKED, NULL);

    // 信息表格
    lv_obj_t *cont = lv_win_get_content(win);
    lv_obj_t *table = lv_table_create(cont);
    lv_obj_set_pos(table, 0, 0);
    lv_table_set_col_width(table, 0, 150);
    lv_table_set_col_width(table, 1, 300);
    lv_obj_invalidate(table);

    // 第一列
    lv_table_set_cell_value(table, 0, 0, DSPFreqStr);
    lv_table_set_cell_value(table, 1, 0, ARMFreqStr);
    lv_table_set_cell_value(table, 2, 0, PRUFreqStr);
    lv_table_set_cell_value(table, 3, 0, DDRFreqStr);
    lv_table_set_cell_value(table, 4, 0, BootStr);
    lv_table_set_cell_value(table, 5, 0, TempStr);
    lv_table_set_cell_value(table, 6, 0, HumStr);
    lv_table_set_cell_value(table, 7, 0, HostNameStr);
    lv_table_set_cell_value(table, 8, 0, MACAddrStr);
    lv_table_set_cell_value(table, 9, 0, DHCPModeStr);
    lv_table_set_cell_value(table, 10, 0, IPAddrStr);
    lv_table_set_cell_value(table, 11, 0, SubMaskStr);
    lv_table_set_cell_value(table, 12, 0, GateWayStr);
    lv_table_set_cell_value(table, 13, 0, DomainNameStr);
    lv_table_set_cell_value(table, 14, 0, DNSServerStr);
    lv_table_set_cell_value(table, 15, 0, WIFISSIDStr);

    // 获取信息
    // 获取频率信息
    PLLClockGet();

    // 获取启动模式
    BootModeGet();

    // 温度湿度
    extern float t, rh;
    char TempValStr[32];
    char HumValStr[32];
    sprintf(TempValStr, "%2.2f%s", t, DegStr);
    sprintf(HumValStr, "%2.2f%%", rh);

    // 网络信息
    extern char DHCPMode;
    extern char *HostName;
    extern char *LocalIPAddr;          // DHCP IP 地址
    extern char StaticIPAddr[16];      // 静态 IP 地址
    extern char LocalIPMask[16];       // 静态 子网掩码
    extern char GatewayIP[16];         // 静态 网关
    extern char *DomainName;           // 静态 域名
    extern char *DNSServer;            // DNS 服务器

    char str[64];

    // 第二列
    sprintf(str, "%dMHz", pllcfg.PLL0_SYSCLK1);
    lv_table_set_cell_value(table, 0, 1, str);

    sprintf(str, "%dMHz(仅 OMAP-L138)", pllcfg.PLL0_SYSCLK6);
    lv_table_set_cell_value(table, 1, 1, str);

    sprintf(str, "%dMHz", pllcfg.PLL0_SYSCLK2);
    lv_table_set_cell_value(table, 2, 1, str);

    sprintf(str, "%dMT/s", pllcfg.PLL1_SYSCLK1);
    lv_table_set_cell_value(table, 3, 1, str);

    sprintf(str, "%s", BootModeStr[BootMode]);
    lv_table_set_cell_value(table, 4, 1, str);

    lv_table_set_cell_value(table, 5, 1, TempValStr);
    lv_table_set_cell_value(table, 6, 1, HumValStr);
    lv_table_set_cell_value(table, 7, 1, HostName);

    sprintf(str, "%02X-%02X-%02X-%02X-%02X-%02X", MacAddr[0], MacAddr[1], MacAddr[2], MacAddr[3], MacAddr[4], MacAddr[5]);
    lv_table_set_cell_value(table, 8, 1, str);

    sprintf(str, "%s", DHCPMode ? DHCPModeStaticStr : DHCPModeAutoStr);
    lv_table_set_cell_value(table, 9, 1, str);

    sprintf(str, "%s", StaticIPAddr);
    lv_table_set_cell_value(table, 10, 1, str);

    sprintf(str, "%s", LocalIPMask);
    lv_table_set_cell_value(table, 11, 1, str);

    sprintf(str, "%s", GatewayIP);
    lv_table_set_cell_value(table, 12, 1, str);

    sprintf(str, "%s", DomainName);
    lv_table_set_cell_value(table, 13, 1, str);

    sprintf(str, "%s", DNSServer);
    lv_table_set_cell_value(table, 14, 1, str);

    sprintf(str, "EndlessDream");
    lv_table_set_cell_value(table, 15, 1, str);

//  lv_table_set_cell_value_fmt(table, 15, 1, "EndlessDream");
}
