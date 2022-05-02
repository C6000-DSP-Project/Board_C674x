/****************************************************************************/
/*                                                                          */
/*    �º˿Ƽ�(����)���޹�˾                                                */
/*                                                                          */
/*    Copyright (C) 2022 CoreKernel Technology (Guangzhou) Co., Ltd         */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/*    LCD                                                                   */
/*                                                                          */
/*    2014��10��14��                                                        */
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

#include <LCD/Logo.h>

/****************************************************************************/
/*                                                                          */
/*              ȫ�ֱ���                                                    */
/*                                                                          */
/****************************************************************************/
#pragma DATA_ALIGN(g_pucBuffer, 4);
unsigned char g_pucBuffer[4 + (16 * 2) + (LCD_WIDTH * LCD_HEIGHT * 2)];

// ��ɫ��
unsigned short palette_32b[PALETTE_SIZE / 2] =
{
    0x4000u, 0x0000u, 0x0000u, 0x0000u, 0x0000u, 0x0000u, 0x0000u, 0x0000u,
    0x0000u, 0x0000u, 0x0000u, 0x0000u, 0x0000u, 0x0000u, 0x0000u, 0x0000u
};

/****************************************************************************/
/*                                                                          */
/*              GPIO �ܽŸ�������                                           */
/*                                                                          */
/****************************************************************************/
static void GPIOBankPinMuxSet()
{
    // LCD �ź�
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

    // LCD ����
    savePinMux = (HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(1)) & ~(SYSCFG_PINMUX1_PINMUX1_3_0));
    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(1)) = ((SYSCFG_PINMUX1_PINMUX1_3_0_GPIO0_7 << SYSCFG_PINMUX1_PINMUX1_3_0_SHIFT) | savePinMux);
}

/****************************************************************************/
/*                                                                          */
/*              LCD ����                                                    */
/*                                                                          */
/****************************************************************************/
void LCDBacklightEnable()
{
    // ʹ�ܱ��� GPIO0[7]��Ҳ����ʹ�� ECAP APWM2 ���⣩
    GPIODirModeSet(SOC_GPIO_0_REGS, 8, GPIO_DIR_OUTPUT);
    GPIOPinWrite(SOC_GPIO_0_REGS, 8, 1);
}

void LCDBacklightDisable()
{
    // ���ñ��� GPIO0[7]
    GPIODirModeSet(SOC_GPIO_0_REGS, 8, GPIO_DIR_OUTPUT);
    GPIOPinWrite(SOC_GPIO_0_REGS, 8, 0);
}

/****************************************************************************/
/*                                                                          */
/*              Ӳ���ж��߳�                                                */
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
    /* ӳ�䵽����¼��ж� */
    // EVT0  1 - 31
    // EVT1 32 - 63
    // EVT2 64 - 95
    // EVT3 96 - 127

    EventCombiner_dispatchPlug(SYS_INT_LCDC_INT, &LCDHwi, 0, TRUE);
}

/****************************************************************************/
/*                                                                          */
/*              �����߳�                                                    */
/*                                                                          */
/****************************************************************************/
Void LCDTask(UArg a0, UArg a1)
{
//    // �رձ���
//    LCDBacklightDisable();
//
//    // ����ͼ
//    GrImageDraw(&g_sContext, image, 0, 0);
//
//    // ��ʾ��̬�ı�
//    GrContextFontSet(&g_sContext, TEXT_FONT);
//    GrContextForegroundSet(&g_sContext, ClrWhite);
//    GrStringDraw(&g_sContext, "corekernel.net/.org/.cn", -1, 525, 400, false);
//    GrStringDraw(&g_sContext, "fpga.net.cn", -1, 525, 425, false);
//
//    // �򿪱���
//    LCDBacklightEnable();
//
//    // �������ֱ���
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
//        // ����ͼ
////      GrImageDraw(&g_sContext, image, 0, 0);
//
//        // ��ʾʱ��
//        GrContextForegroundSet(&g_sContext, ClrSteelBlue);
//
//        sprintf(str, "%04d/%02d/%02d %s %02d:%02d:%02d", RTCTime.tm_year + 1920, RTCTime.tm_mon, RTCTime.tm_mday, WeekDayStr[RTCTime.tm_wday],
//                                                          RTCTime.tm_hour, RTCTime.tm_min, RTCTime.tm_sec);
//        GrStringDraw(&g_sContext, str, -1, 525, 25, true);
//
//        // ������Ϣ
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
//        // �¶�/ʪ��
//        TempSensorGet(&t, &rh);
//        sprintf(str, "Temperature %2.2f%sC", t, deg);
//        GrStringDraw(&g_sContext, str, -1, 25, 175, true);
//
//        sprintf(str, "Humidity %2.2f%%", rh);
//        GrStringDraw(&g_sContext, str, -1, 25, 200, true);
//
//        // CPU Ƶ��
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
//        // ����ģʽ
//        BootModeGet();
//        sprintf(str, "Boot From %s", BootModeStr[BootMode]);
//        GrStringDraw(&g_sContext, str, -1, 25, 300, false);
//
//        // MAC/IP ��ַ
//        GrContextForegroundSet(&g_sContext, ClrSteelBlue);
//
//        sprintf(str, "%02X-%02X-%02X-%02X-%02X-%02X", MacAddr[0], MacAddr[1], MacAddr[2], MacAddr[3], MacAddr[4], MacAddr[5]);
//        GrStringDraw(&g_sContext, str, -1, 25, 350, true);
//
//        sprintf(str, "%s", StrIP);
//        GrStringDraw(&g_sContext, str, -1, 25, 375, true);
//
//        // ��ʾ CPU ����
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
/*              �汾ʶ��                                                    */
/*                                                                          */
/****************************************************************************/
unsigned int LCDVersionGet()
{
    return 1;
}

/****************************************************************************/
/*                                                                          */
/*              LCD ��ʼ��                                                  */
/*                                                                          */
/****************************************************************************/
void LCDInit()
{
	// ʹ�� LCD ģ��
	PSCModuleControl(SOC_PSC_1_REGS, HW_PSC_LCDC, PSC_POWERDOMAIN_ALWAYS_ON, PSC_MDCTL_NEXT_ENABLE);

	// �ܽŸ�������
	GPIOBankPinMuxSet();

    // Ӳ���ж��̳߳�ʼ��
    HwiInit();

	// LCD ��ʼ��
    // ���ù�դ
    RasterDisable(SOC_LCDC_0_REGS);
    
    // ʱ������
    RasterClkConfig(SOC_LCDC_0_REGS, 30000000, LCD_CLK);

    // ���� LCD DMA ������
    RasterDMAConfig(SOC_LCDC_0_REGS, RASTER_DOUBLE_FRAME_BUFFER, RASTER_BURST_SIZE_16, RASTER_FIFO_THRESHOLD_8, RASTER_BIG_ENDIAN_DISABLE);

    // ģʽ����(����:TFT ���� STN,��ɫ���ߺڰ� �ȵ�)
    RasterModeConfig(SOC_LCDC_0_REGS, RASTER_DISPLAY_MODE_TFT, RASTER_PALETTE_DATA, RASTER_COLOR, RASTER_RIGHT_ALIGNED);

    // ֡���������� LSB ��ʽ����
    RasterLSBDataOrderSelect(SOC_LCDC_0_REGS);
    
    // ���� Nibble ģʽ
    RasterNibbleModeDisable(SOC_LCDC_0_REGS);
   
    // ���ù�դ����������
    RasterTiming2Configure(SOC_LCDC_0_REGS, RASTER_FRAME_CLOCK_LOW | RASTER_LINE_CLOCK_LOW | RASTER_PIXEL_CLOCK_HIGH | RASTER_SYNC_EDGE_RISING | RASTER_SYNC_CTRL_ACTIVE | RASTER_AC_BIAS_HIGH, 0, 255);

    // ����ˮƽ / ��ֱ����
    RasterHparamConfig(SOC_LCDC_0_REGS, 800, 30, 210, 45);
    RasterVparamConfig(SOC_LCDC_0_REGS, 480, 10, 21, 22);

    // ���� FIFO DMA ��ʱ
    RasterFIFODMADelayConfig(SOC_LCDC_0_REGS, 2);

    // �����Դ�
    RasterDMAFBConfig(SOC_LCDC_0_REGS, (unsigned int)(g_pucBuffer + PALETTE_OFFSET), (unsigned int)(g_pucBuffer + PALETTE_OFFSET) + sizeof(g_pucBuffer) - 2 - PALETTE_OFFSET, 0);
    RasterDMAFBConfig(SOC_LCDC_0_REGS, (unsigned int)(g_pucBuffer + PALETTE_OFFSET), (unsigned int)(g_pucBuffer + PALETTE_OFFSET) + sizeof(g_pucBuffer) - 2 - PALETTE_OFFSET, 1);

    // ���Ƶ�ɫ�嵽�����Դ���
    unsigned char *src, *dst;
    unsigned int i = 0;

    src = (unsigned char *)palette_32b;
    dst = (unsigned char *)(g_pucBuffer + PALETTE_OFFSET);

    for(i = 4; i < (PALETTE_SIZE + 4); i++)
    {
        *dst++ = *src++;
    }

    // ʹ�� LCD ֡�����ж�
    RasterEndOfFrameIntEnable(SOC_LCDC_0_REGS);
    RasterIntEnable(SOC_LCDC_0_REGS, RASTER_END_OF_FRAME0_INT | RASTER_END_OF_FRAME1_INT);

    // ʹ�ܹ�դ
    RasterEnable(SOC_LCDC_0_REGS);
}
