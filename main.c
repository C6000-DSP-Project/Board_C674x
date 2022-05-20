/****************************************************************************/
/*                                                                          */
/*    新核科技(广州)有限公司                                                */
/*                                                                          */
/*    Copyright (C) 2022 CoreKernel Technology Guangzhou Co., Ltd           */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/*    EVM-CKL138（DSP 核心）/6748PKT/EDA 测试程序（SYS/BIOS）               */
/*                                                                          */
/*    2022年02月22日                                                        */
/*                                                                          */
/****************************************************************************/
/*
 *    - 希望缄默(bin wang)
 *    - bin@corekernel.net
 *
 *    官网 corekernel.net/.org/.cn
 *    社区 fpga.net.cn
 *
 *    说明
 *         由于管脚复用为 EMAC MII 下述外设无法使用
 *         1、eHRPWM0
 *         2、SPI0
 *         3、McBSP0
 *         4、UART0
 *         5、TIMER0/1 输入输出引脚
 *         6、McASP AXR00-AXR06
 *         7、ECAP0
 *
 *         SPI0_ENA/EPWM0_B/EMAC_MII_RXDV
 *         GPIO1[06]/SPI0_SCS0/TIMER64P1_IN12/TIMER64P1_OUT12/MDIO_DATA
 *         GPIO1[07]/SPI0_SCS1/TIMER64P0_IN12/TIMER64P0_OUT12/MDIO_CLK
 *         GPIO1[08]/SPI0_CLK/EPWM0_A/EMAC_MII_RXCLK
 *         GPIO1[09]/McASP_AXR01/McBSP0_DX0/EMAC_MII_TXD[1]
 *         GPIO1[10]/McASP_AXR02/McBSP0_DR0/EMAC_MII_TXD[2]
 *         GPIO1[11]/McASP_AXR03/McBSP0_FSX0/EMAC_MII_TXD[3]
 *         GPIO1[12]/McASP_AXR04/McBSP0_FSR0/EMAC_MII_COL
 *         GPIO1[13]/McASP_AXR05/McBSP0_CLKX0/EMAC_MII_TXCLK
 *         GPIO1[14]/McASP_AXR06/McBSP0_CLKR0/EMAC_MII_TXEN
 *         GPIO8[01]/SPI0_SCS2/UART0_RTS/SATA_CP_DET/EMAC_MII_RXD[0]
 *         GPIO8[02]/SPI0_SCS3/UART0_CTS/SATA_MP_SWITCH/EMAC_MII_RXD[1]
 *         GPIO8[03]/SPI0_SCS4/UART0_TXD/EMAC_MII_RXD[2]
 *         GPIO8[04]/SPI0_SCS5/UART0_RXD/EMAC_MII_RXD[3]
 *         GPIO8[05]/SPI0_SIMO/EPWM0_SYNCO/EMAC_MII_CRS
 *         GPIO8[06]/SPI0_SOMI/EPWM0_SYNCI/EMAC_MII_RXER
 *         GPIO8[07]/McASP_AXR00/McBSP0_CLKS0/ECAP0_APWM0/EMAC_MII_TXD[0]
 *
 *    版本历史
 *    2022年02月22日 - 版本 1.00.0222
 *                     初始版本
 *
 */
#include <App.h>

/****************************************************************************/
/*                                                                          */
/*              Hook                                                        */
/*                                                                          */
/****************************************************************************/
#pragma CODE_SECTION (FunEntryHook, ".text:FunctionHook")
void FunEntryHook(void (*addr)())
{
//	Log_write2(UIAProfile_enterFunctionAdrs, (IArg)0, (IArg)addr);
}

#pragma CODE_SECTION (FunExitHook, ".text:FunctionHook")
void FunExitHook(void (*addr)())
{
//	Log_write2(UIAProfile_exitFunctionAdrs, (IArg)0, (IArg)addr);
}

/****************************************************************************/
/*                                                                          */
/*              缓存配置                                                    */
/*                                                                          */
/****************************************************************************/
void CacheInit()
{
    Cache_Size cacheSize;

    cacheSize.l1pSize = Cache_L1Size_32K;
    cacheSize.l1dSize = Cache_L1Size_32K;
    cacheSize.l2Size  = Cache_L2Size_128K;
    Cache_setSize(&cacheSize);

    // 配置可缓存区域
    Cache_setMar((Ptr *)0xC0000000, 0x08000000, Cache_Mar_ENABLE);  // DDR2
}

/****************************************************************************/
/*                                                                          */
/*              硬件初始化                                                  */
/*                                                                          */
/****************************************************************************/
Void HardwareInit()
{
    // 获取 PLL 时钟
    PLLClockGet();

    // LED
    LEDInit();

    // 数码管
    LEDNixieInit();

    // 按键
    KEYInit();

    // 蜂鸣器
    BUZZERInit();

    // 串口终端
    UARTConsoleInit();

    // UART1
    UART1Init();

    // 定时器
    TimerInit();

    // 温湿度传感器
    TempSensorInit();

    // RTC
    RTCInit();

    // LCD
    LCDInit();

    // EHRPWM
    EHRPWMInit();

    // ECAP(LCD 屏幕亮度)
    ECAPInit();

    // 触摸屏
    TouchInit();

    // LVGL 图形库
    lv_init();
    lv_port_disp_init();
    lv_port_indev_init();

    // [PRU 核心] LED 点阵模块
    LEDMatrixInit();
}

/****************************************************************************/
/*                                                                          */
/*              主函数                                                      */
/*                                                                          */
/****************************************************************************/
Int main()
{
    // 缓存配置
    CacheInit();

	// 硬件初始化
    HardwareInit();

    // 输出提示
    ConsoleWrite("CoreKernel EVM-CKL138PKT/CK6748PKT Application...\r\n");
    ConsoleWrite("Build on %s - %s CGT Version %d\r\n", __DATE__, __TIME__, __TI_COMPILER_VERSION__);
    ConsoleWrite("CoreKernel > ");

    // 启动 SYS/BIOS 系统
    BIOS_start();

    return 0;
}
