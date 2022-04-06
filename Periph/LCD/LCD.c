/****************************************************************************/
/*                                                                          */
/*    新核科技(广州)有限公司                                                */
/*                                                                          */
/*    Copyright (C) 2022 CoreKernel Technology (Guangzhou) Co., Ltd         */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/*    LCD                                                                   */
/*                                                                          */
/*    2014年10月14日                                                        */
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

#include <LCD/Logo.h>

/****************************************************************************/
/*                                                                          */
/*              全局变量                                                    */
/*                                                                          */
/****************************************************************************/
#pragma DATA_ALIGN(g_pucBuffer, 4);
unsigned char g_pucBuffer[4 + (16 * 2) + (LCD_WIDTH * LCD_HEIGHT *2)];

// 调色板
unsigned short palette_32b[PALETTE_SIZE / 2] =
{
    0x4000u, 0x0000u, 0x0000u, 0x0000u, 0x0000u, 0x0000u, 0x0000u, 0x0000u,
    0x0000u, 0x0000u, 0x0000u, 0x0000u, 0x0000u, 0x0000u, 0x0000u, 0x0000u
};

/****************************************************************************/
/*                                                                          */
/*              GPIO 管脚复用配置                                           */
/*                                                                          */
/****************************************************************************/
static void GPIOBankPinMuxSet()
{
    // LCD 信号
    #define PINMUX16_LCD_ENABLE     (SYSCFG_PINMUX16_PINMUX16_31_28_LCD_D2 << SYSCFG_PINMUX16_PINMUX16_31_28_SHIFT) | \
                                     (SYSCFG_PINMUX16_PINMUX16_27_24_LCD_D3 << SYSCFG_PINMUX16_PINMUX16_27_24_SHIFT) | \
                                     (SYSCFG_PINMUX16_PINMUX16_23_20_LCD_D4 << SYSCFG_PINMUX16_PINMUX16_23_20_SHIFT) | \
                                     (SYSCFG_PINMUX16_PINMUX16_19_16_LCD_D5 << SYSCFG_PINMUX16_PINMUX16_19_16_SHIFT) | \
                                     (SYSCFG_PINMUX16_PINMUX16_15_12_LCD_D6 << SYSCFG_PINMUX16_PINMUX16_15_12_SHIFT) | \
                                     (SYSCFG_PINMUX16_PINMUX16_11_8_LCD_D7 << SYSCFG_PINMUX16_PINMUX16_11_8_SHIFT)

    #define PINMUX17_LCD_ENABLE     (SYSCFG_PINMUX17_PINMUX17_31_28_LCD_D10 << SYSCFG_PINMUX17_PINMUX17_31_28_SHIFT) | \
                                     (SYSCFG_PINMUX17_PINMUX17_27_24_LCD_D11 << SYSCFG_PINMUX17_PINMUX17_27_24_SHIFT) | \
                                     (SYSCFG_PINMUX17_PINMUX17_23_20_LCD_D12 << SYSCFG_PINMUX17_PINMUX17_23_20_SHIFT) | \
                                     (SYSCFG_PINMUX17_PINMUX17_19_16_LCD_D13 << SYSCFG_PINMUX17_PINMUX17_19_16_SHIFT) | \
                                     (SYSCFG_PINMUX17_PINMUX17_15_12_LCD_D14 << SYSCFG_PINMUX17_PINMUX17_15_12_SHIFT) | \
                                     (SYSCFG_PINMUX17_PINMUX17_11_8_LCD_D15 << SYSCFG_PINMUX17_PINMUX17_11_8_SHIFT) | \
                                     (SYSCFG_PINMUX17_PINMUX17_7_4_LCD_D0 << SYSCFG_PINMUX17_PINMUX17_7_4_SHIFT) | \
                                     (SYSCFG_PINMUX17_PINMUX17_3_0_LCD_D1 << SYSCFG_PINMUX17_PINMUX17_3_0_SHIFT)

    #define PINMUX18_LCD_ENABLE     (SYSCFG_PINMUX18_PINMUX18_31_28_LCD_MCLK << SYSCFG_PINMUX18_PINMUX18_31_28_SHIFT) | \
                                     (SYSCFG_PINMUX18_PINMUX18_27_24_LCD_PCLK << SYSCFG_PINMUX18_PINMUX18_27_24_SHIFT) | \
                                     (SYSCFG_PINMUX18_PINMUX18_7_4_LCD_D8 << SYSCFG_PINMUX18_PINMUX18_7_4_SHIFT) | \
                                     (SYSCFG_PINMUX18_PINMUX18_3_0_LCD_D9 << SYSCFG_PINMUX18_PINMUX18_3_0_SHIFT)

    #define PINMUX19_LCD_ENABLE     (SYSCFG_PINMUX19_PINMUX19_27_24_NLCD_AC_ENB_CS << SYSCFG_PINMUX19_PINMUX19_27_24_SHIFT) | \
                                     (SYSCFG_PINMUX19_PINMUX19_7_4_LCD_VSYNC << SYSCFG_PINMUX19_PINMUX19_7_4_SHIFT) | \
                                     (SYSCFG_PINMUX19_PINMUX19_3_0_LCD_HSYNC << SYSCFG_PINMUX19_PINMUX19_3_0_SHIFT)

    unsigned int savePinMux = 0;

    savePinMux = HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(16)) & \
                       ~(SYSCFG_PINMUX16_PINMUX16_31_28 | \
                         SYSCFG_PINMUX16_PINMUX16_27_24 | \
                         SYSCFG_PINMUX16_PINMUX16_23_20 | \
                         SYSCFG_PINMUX16_PINMUX16_19_16 | \
                         SYSCFG_PINMUX16_PINMUX16_15_12 | \
                         SYSCFG_PINMUX16_PINMUX16_11_8);

    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(16)) = (PINMUX16_LCD_ENABLE | savePinMux);

    savePinMux = HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(17)) & \
                       ~(SYSCFG_PINMUX17_PINMUX17_31_28 | \
                         SYSCFG_PINMUX17_PINMUX17_27_24 | \
                         SYSCFG_PINMUX17_PINMUX17_23_20 | \
                         SYSCFG_PINMUX17_PINMUX17_19_16 | \
                         SYSCFG_PINMUX17_PINMUX17_15_12 | \
                         SYSCFG_PINMUX17_PINMUX17_11_8 | \
                         SYSCFG_PINMUX17_PINMUX17_7_4 | \
                         SYSCFG_PINMUX17_PINMUX17_3_0);

    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(17)) = (PINMUX17_LCD_ENABLE | savePinMux);

    savePinMux = HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(18)) & \
                       ~(SYSCFG_PINMUX18_PINMUX18_31_28 | \
                         SYSCFG_PINMUX18_PINMUX18_27_24 | \
                         SYSCFG_PINMUX18_PINMUX18_7_4 |  \
                         SYSCFG_PINMUX18_PINMUX18_3_0);

    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(18)) = (PINMUX18_LCD_ENABLE | savePinMux);

    savePinMux = HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(19)) & \
                       ~(SYSCFG_PINMUX19_PINMUX19_27_24 | \
                         SYSCFG_PINMUX19_PINMUX19_7_4 | \
                         SYSCFG_PINMUX19_PINMUX19_3_0);

    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(19)) = (PINMUX19_LCD_ENABLE | savePinMux);

    // LCD 背光
    savePinMux = (HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(1)) & ~(SYSCFG_PINMUX1_PINMUX1_3_0));
    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(1)) = ((SYSCFG_PINMUX1_PINMUX1_3_0_GPIO0_7 << SYSCFG_PINMUX1_PINMUX1_3_0_SHIFT) | savePinMux);
}

/****************************************************************************/
/*                                                                          */
/*              LCD 背光                                                    */
/*                                                                          */
/****************************************************************************/
void LCDBacklightEnable()
{
    // 使能背光 GPIO0[7]（也可以使用 ECAP APWM2 调光）
    GPIODirModeSet(SOC_GPIO_0_REGS, 8, GPIO_DIR_OUTPUT);
    GPIOPinWrite(SOC_GPIO_0_REGS, 8, 1);
}

void LCDBacklightDisable()
{
    // 禁用背光 GPIO0[7]
    GPIODirModeSet(SOC_GPIO_0_REGS, 8, GPIO_DIR_OUTPUT);
    GPIOPinWrite(SOC_GPIO_0_REGS, 8, 0);
}

/****************************************************************************/
/*                                                                          */
/*              硬件中断线程                                                */
/*                                                                          */
/****************************************************************************/
Void LCDHwi(UArg arg)
{
    unsigned int  status;

    status = RasterIntStatus(SOC_LCDC_0_REGS,  RASTER_FIFO_UNDERFLOW_INT_STAT |
                                               RASTER_END_OF_FRAME0_INT_STAT  |
                                               RASTER_END_OF_FRAME1_INT_STAT );

    if(status & RASTER_FIFO_UNDERFLOW_INT_STAT)
    {
        RasterDisable(SOC_LCDC_0_REGS);
        RasterEnable(SOC_LCDC_0_REGS);
    }

    status = RasterClearGetIntStatus(SOC_LCDC_0_REGS, status);
}

static Void HwiInit()
{
    /* 映射到组合事件中断 */
    // EVT0  1 - 31
    // EVT1 32 - 63
    // EVT2 64 - 95
    // EVT3 96 - 127

    EventCombiner_dispatchPlug(SYS_INT_LCDC_INT, &LCDHwi, 0, TRUE);
}

/****************************************************************************/
/*                                                                          */
/*              任务线程                                                    */
/*                                                                          */
/****************************************************************************/
Void LCDTask(UArg a0, UArg a1)
{
//    // 关闭背光
//    LCDBacklightDisable();
//
//    // 背景图
//    GrImageDraw(&g_sContext, image, 0, 0);
//
//    // 显示静态文本
//    GrContextFontSet(&g_sContext, TEXT_FONT);
//    GrContextForegroundSet(&g_sContext, ClrWhite);
//    GrStringDraw(&g_sContext, "corekernel.net/.org/.cn", -1, 525, 400, false);
//    GrStringDraw(&g_sContext, "fpga.net.cn", -1, 525, 425, false);
//
//    // 打开背光
//    LCDBacklightEnable();
//
//    // 设置文字背景
//    GrContextBackgroundSet(&g_sContext, ClrWhite);
//
//    char str[64];
//
//    char *WeekDayStr[64] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
//
//    float t, rh;
//    char deg[] = {0xA1, 0xE3, 0};
//
//    for(;;)
//    {
//        // 背景图
////      GrImageDraw(&g_sContext, image, 0, 0);
//
//        // 显示时间
//        GrContextForegroundSet(&g_sContext, ClrSteelBlue);
//
//        sprintf(str, "%04d/%02d/%02d %s %02d:%02d:%02d", RTCTime.tm_year + 1920, RTCTime.tm_mon, RTCTime.tm_mday, WeekDayStr[RTCTime.tm_wday],
//                                                          RTCTime.tm_hour, RTCTime.tm_min, RTCTime.tm_sec);
//        GrStringDraw(&g_sContext, str, -1, 525, 25, true);
//
//        // 触摸信息
//        GrContextForegroundSet(&g_sContext, ClrSteelBlue);
//        sprintf(str, "%d Point Touch", TouchInfo.Num);
//        GrStringDraw(&g_sContext, str, -1, 525, 50, true);
//
//        sprintf(str, "X0 %3d Y0 %3d", TouchInfo.X[0], TouchInfo.Y[0]);
//        GrStringDraw(&g_sContext, str, -1, 525, 75, true);
//
//        sprintf(str, "X1 %3d Y1 %3d", TouchInfo.X[1], TouchInfo.Y[1]);
//        GrStringDraw(&g_sContext, str, -1, 525, 100, true);
//
//        sprintf(str, "X2 %3d Y2 %3d", TouchInfo.X[2], TouchInfo.Y[2]);
//        GrStringDraw(&g_sContext, str, -1, 525, 125, true);
//
//        sprintf(str, "X3 %3d Y3 %3d", TouchInfo.X[3], TouchInfo.Y[3]);
//        GrStringDraw(&g_sContext, str, -1, 525, 150, true);
//
//        sprintf(str, "X4 %3d Y4 %3d", TouchInfo.X[4], TouchInfo.Y[4]);
//        GrStringDraw(&g_sContext, str, -1, 525, 175, true);
//
//        // 温度/湿度
//        TempSensorGet(&t, &rh);
//        sprintf(str, "Temperature %2.2f%sC", t, deg);
//        GrStringDraw(&g_sContext, str, -1, 25, 175, true);
//
//        sprintf(str, "Humidity %2.2f%%", rh);
//        GrStringDraw(&g_sContext, str, -1, 25, 200, true);
//
//        // CPU 频率
//        GrContextForegroundSet(&g_sContext, ClrBlack);
//
//        PLLClockGet();
//
//        sprintf(str, "DSP/ARM %dMHz", pllcfg.PLL0_SYSCLK1);
//        GrStringDraw(&g_sContext, str, -1, 25, 250, false);
//
//        sprintf(str, "DDR2 %dMT/s", pllcfg.PLL1_SYSCLK1);
//        GrStringDraw(&g_sContext, str, -1, 25, 275, false);
//
//        // 启动模式
//        BootModeGet();
//        sprintf(str, "Boot From %s", BootModeStr[BootMode]);
//        GrStringDraw(&g_sContext, str, -1, 25, 300, false);
//
//        // MAC/IP 地址
//        GrContextForegroundSet(&g_sContext, ClrSteelBlue);
//
//        sprintf(str, "%02X-%02X-%02X-%02X-%02X-%02X", MacAddr[0], MacAddr[1], MacAddr[2], MacAddr[3], MacAddr[4], MacAddr[5]);
//        GrStringDraw(&g_sContext, str, -1, 25, 350, true);
//
//        sprintf(str, "%s", StrIP);
//        GrStringDraw(&g_sContext, str, -1, 25, 375, true);
//
//        // 显示 CPU 负载
//        GrContextForegroundSet(&g_sContext, ClrSteelBlue);
//
//        sprintf(str, "CPU Load %2d%%", Load_getCPULoad());
//        GrStringDraw(&g_sContext, str, -1, 25, 425, true);
//
//        Task_sleep(1000);
//    }
}

/****************************************************************************/
/*                                                                          */
/*              版本识别                                                    */
/*                                                                          */
/****************************************************************************/
unsigned int LCDVersionGet()
{
    return 1;
}

/****************************************************************************/
/*                                                                          */
/*              LCD 初始化                                                  */
/*                                                                          */
/****************************************************************************/
void LCDInit()
{
	// 使能 LCD 模块
	PSCModuleControl(SOC_PSC_1_REGS, HW_PSC_LCDC, PSC_POWERDOMAIN_ALWAYS_ON, PSC_MDCTL_NEXT_ENABLE);

	// 管脚复用配置
	GPIOBankPinMuxSet();

    // 硬件中断线程初始化
    HwiInit();

	// LCD 初始化
    // 禁用光栅
    RasterDisable(SOC_LCDC_0_REGS);
    
    // 时钟配置
    RasterClkConfig(SOC_LCDC_0_REGS, 30000000, LCD_CLK);

    // 配置 LCD DMA 控制器
    RasterDMAConfig(SOC_LCDC_0_REGS, RASTER_DOUBLE_FRAME_BUFFER, RASTER_BURST_SIZE_16, RASTER_FIFO_THRESHOLD_8, RASTER_BIG_ENDIAN_DISABLE);

    // 模式配置(例如:TFT 或者 STN,彩色或者黑白 等等)
    RasterModeConfig(SOC_LCDC_0_REGS, RASTER_DISPLAY_MODE_TFT, RASTER_PALETTE_DATA, RASTER_COLOR, RASTER_RIGHT_ALIGNED);

    // 帧缓存数据以 LSB 方式排列
    RasterLSBDataOrderSelect(SOC_LCDC_0_REGS);
    
    // 禁用 Nibble 模式
    RasterNibbleModeDisable(SOC_LCDC_0_REGS);
   
    // 配置光栅控制器极性
    RasterTiming2Configure(SOC_LCDC_0_REGS, RASTER_FRAME_CLOCK_LOW | RASTER_LINE_CLOCK_LOW | RASTER_PIXEL_CLOCK_HIGH | RASTER_SYNC_EDGE_RISING | RASTER_SYNC_CTRL_ACTIVE | RASTER_AC_BIAS_HIGH, 0, 255);

    // 配置水平 / 垂直参数
    RasterHparamConfig(SOC_LCDC_0_REGS, 800, 30, 210, 45);
    RasterVparamConfig(SOC_LCDC_0_REGS, 480, 10, 21, 22);

    // 配置 FIFO DMA 延时
    RasterFIFODMADelayConfig(SOC_LCDC_0_REGS, 2);

    // 配置显存
    RasterDMAFBConfig(SOC_LCDC_0_REGS, (unsigned int)(g_pucBuffer + PALETTE_OFFSET), (unsigned int)(g_pucBuffer + PALETTE_OFFSET) + sizeof(g_pucBuffer) - 2 - PALETTE_OFFSET, 0);
    RasterDMAFBConfig(SOC_LCDC_0_REGS, (unsigned int)(g_pucBuffer + PALETTE_OFFSET), (unsigned int)(g_pucBuffer + PALETTE_OFFSET) + sizeof(g_pucBuffer) - 2 - PALETTE_OFFSET, 1);

    // 复制调色板到离屏显存中
    unsigned char *src, *dst;
    unsigned int i = 0;

    src = (unsigned char *)palette_32b;
    dst = (unsigned char *)(g_pucBuffer + PALETTE_OFFSET);

    for(i = 4; i < (PALETTE_SIZE + 4); i++)
    {
        *dst++ = *src++;
    }

    // 使能 LCD 帧结束中断
    RasterEndOfFrameIntEnable(SOC_LCDC_0_REGS);
    RasterIntEnable(SOC_LCDC_0_REGS, RASTER_END_OF_FRAME0_INT | RASTER_END_OF_FRAME1_INT);

    // 使能光栅
    RasterEnable(SOC_LCDC_0_REGS);
}
