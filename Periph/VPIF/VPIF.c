/****************************************************************************/
/*                                                                          */
/*              VPIF 接口及 OV2640 CMOS 传感器测试                          */
/*                                                                          */
/*              2014年07月17日                                              */
/*                                                                          */
/****************************************************************************/
#include <App.h>
#include <VPIF/OV2640.h>

#include "vpif.h"

/*****************************************************************************
**                      MACRO DEFINITIONS
*****************************************************************************/
#define PINMUX14_VPIF_ENABLE    (SYSCFG_PINMUX14_PINMUX14_31_28_DIN2 << \
                                 SYSCFG_PINMUX14_PINMUX14_31_28_SHIFT) | \
                                (SYSCFG_PINMUX14_PINMUX14_27_24_DIN3 << \
                                 SYSCFG_PINMUX14_PINMUX14_27_24_SHIFT) | \
                                (SYSCFG_PINMUX14_PINMUX14_23_20_DIN4 << \
                                 SYSCFG_PINMUX14_PINMUX14_23_20_SHIFT) | \
                                (SYSCFG_PINMUX14_PINMUX14_19_16_DIN5 << \
                                 SYSCFG_PINMUX14_PINMUX14_19_16_SHIFT) | \
                                (SYSCFG_PINMUX14_PINMUX14_15_12_DIN6 << \
                                 SYSCFG_PINMUX14_PINMUX14_15_12_SHIFT) | \
                                (SYSCFG_PINMUX14_PINMUX14_11_8_DIN7 << \
                                 SYSCFG_PINMUX14_PINMUX14_11_8_SHIFT) | \
                                (SYSCFG_PINMUX14_PINMUX14_7_4_CLKIN1 << \
                                 SYSCFG_PINMUX14_PINMUX14_7_4_SHIFT) | \
                                (SYSCFG_PINMUX14_PINMUX14_3_0_CLKIN0 << \
                                 SYSCFG_PINMUX14_PINMUX14_3_0_SHIFT)


#define PINMUX15_VPIF_ENABLE    (SYSCFG_PINMUX15_PINMUX15_31_28_DIN10 << \
                                 SYSCFG_PINMUX15_PINMUX15_31_28_SHIFT) | \
                                (SYSCFG_PINMUX15_PINMUX15_27_24_DIN11 << \
                                 SYSCFG_PINMUX15_PINMUX15_27_24_SHIFT) | \
                                (SYSCFG_PINMUX15_PINMUX15_23_20_DIN12 << \
                                 SYSCFG_PINMUX15_PINMUX15_23_20_SHIFT) | \
                                (SYSCFG_PINMUX15_PINMUX15_19_16_DIN13_FIELD << \
                                 SYSCFG_PINMUX15_PINMUX15_19_16_SHIFT) | \
                                (SYSCFG_PINMUX15_PINMUX15_15_12_DIN14_HSYNC << \
                                 SYSCFG_PINMUX15_PINMUX15_15_12_SHIFT) | \
                                (SYSCFG_PINMUX15_PINMUX15_11_8_DIN15_VSYNC << \
                                 SYSCFG_PINMUX15_PINMUX15_11_8_SHIFT) | \
                                (SYSCFG_PINMUX15_PINMUX15_7_4_DIN0 << \
                                 SYSCFG_PINMUX15_PINMUX15_7_4_SHIFT) | \
                                (SYSCFG_PINMUX15_PINMUX15_3_0_DIN1 << \
                                 SYSCFG_PINMUX15_PINMUX15_3_0_SHIFT)



#define PINMUX16_VPIF_ENABLE    (SYSCFG_PINMUX16_PINMUX16_31_28_DOUT2 << \
                                 SYSCFG_PINMUX16_PINMUX16_31_28_SHIFT) | \
                                (SYSCFG_PINMUX16_PINMUX16_27_24_DOUT3 << \
                                 SYSCFG_PINMUX16_PINMUX16_27_24_SHIFT) | \
                                (SYSCFG_PINMUX16_PINMUX16_23_20_DOUT4 << \
                                 SYSCFG_PINMUX16_PINMUX16_23_20_SHIFT) | \
                                (SYSCFG_PINMUX16_PINMUX16_19_16_DOUT5 << \
                                 SYSCFG_PINMUX16_PINMUX16_19_16_SHIFT) | \
                                (SYSCFG_PINMUX16_PINMUX16_15_12_DOUT6 << \
                                 SYSCFG_PINMUX16_PINMUX16_15_12_SHIFT) | \
                                (SYSCFG_PINMUX16_PINMUX16_11_8_DOUT7 << \
                                 SYSCFG_PINMUX16_PINMUX16_11_8_SHIFT) | \
                                (SYSCFG_PINMUX16_PINMUX16_7_4_DIN8 << \
                                 SYSCFG_PINMUX16_PINMUX16_7_4_SHIFT) | \
                                (SYSCFG_PINMUX16_PINMUX16_3_0_DIN9 << \
                                 SYSCFG_PINMUX16_PINMUX16_3_0_SHIFT)

#define PINMUX17_VPIF_ENABLE    (SYSCFG_PINMUX17_PINMUX17_31_28_DOUT10 << \
                                 SYSCFG_PINMUX17_PINMUX17_31_28_SHIFT) | \
                                (SYSCFG_PINMUX17_PINMUX17_27_24_DOUT11 << \
                                 SYSCFG_PINMUX17_PINMUX17_27_24_SHIFT) | \
                                (SYSCFG_PINMUX17_PINMUX17_23_20_DOUT12 << \
                                 SYSCFG_PINMUX17_PINMUX17_23_20_SHIFT) | \
                                (SYSCFG_PINMUX17_PINMUX17_19_16_DOUT13 << \
                                 SYSCFG_PINMUX17_PINMUX17_19_16_SHIFT) | \
                                (SYSCFG_PINMUX17_PINMUX17_15_12_DOUT14 << \
                                 SYSCFG_PINMUX17_PINMUX17_15_12_SHIFT) | \
                                (SYSCFG_PINMUX17_PINMUX17_11_8_DOUT15 << \
                                 SYSCFG_PINMUX17_PINMUX17_11_8_SHIFT) | \
                                (SYSCFG_PINMUX17_PINMUX17_7_4_DOUT0 << \
                                 SYSCFG_PINMUX17_PINMUX17_7_4_SHIFT) | \
                                (SYSCFG_PINMUX17_PINMUX17_3_0_DOUT1 << \
                                 SYSCFG_PINMUX17_PINMUX17_3_0_SHIFT)

#define PINMUX18_VPIF_ENABLE    (SYSCFG_PINMUX18_PINMUX18_7_4_DOUT8 << \
                                 SYSCFG_PINMUX18_PINMUX18_7_4_SHIFT) | \
                                (SYSCFG_PINMUX18_PINMUX18_3_0_DOUT9 << \
                                 SYSCFG_PINMUX18_PINMUX18_3_0_SHIFT)

#define PINMUX19_VPIF_ENABLE    (SYSCFG_PINMUX19_PINMUX19_23_20_CLKO3 << \
                                 SYSCFG_PINMUX19_PINMUX19_23_20_SHIFT) | \
                                (SYSCFG_PINMUX19_PINMUX19_19_16_CLKIN3 << \
                                 SYSCFG_PINMUX19_PINMUX19_19_16_SHIFT) | \
                                (SYSCFG_PINMUX19_PINMUX19_15_12_CLKO2 << \
                                 SYSCFG_PINMUX19_PINMUX19_15_12_SHIFT) | \
                                (SYSCFG_PINMUX19_PINMUX19_11_8_CLKIN2 << \
                                 SYSCFG_PINMUX19_PINMUX19_11_8_SHIFT)



/******************************************************************************
**                      INTERNAL FUNCTION PROTOTYPES
*******************************************************************************/

/*****************************************************************************
**                       FUNCTION DEFINITION
*****************************************************************************/

/**
 * \brief   This function selects the VPIF pins for use. The VPIF pins
 *          are multiplexed with pins of other peripherals in the
 *          System on Chip(SoC).
 *
 * \param   None.
 *
 * \return  None.
 */

void VPIFPinMuxSetUp(void)
{
     unsigned int savePinMux = 0;

     /* Programming the PINMUX14 register. */

     /*
     ** Clearing the pins in context and retaining the other pin values
     ** of PINMUX14 register.
     */
     savePinMux = HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(14)) & \
                        ~(SYSCFG_PINMUX14_PINMUX14_31_28 | \
                          SYSCFG_PINMUX14_PINMUX14_27_24 | \
                          SYSCFG_PINMUX14_PINMUX14_23_20 | \
                          SYSCFG_PINMUX14_PINMUX14_19_16 | \
                          SYSCFG_PINMUX14_PINMUX14_15_12 | \
                          SYSCFG_PINMUX14_PINMUX14_11_8 | \
                          SYSCFG_PINMUX14_PINMUX14_7_4 | \
                          SYSCFG_PINMUX14_PINMUX14_3_0);


     /*
     ** Performing the actual Pin Multiplexing to select relevant pins in
     ** PINMUX14  register.
     */
     HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(14)) = \
          (PINMUX14_VPIF_ENABLE | savePinMux);

     /* Programming the PINMUX15 register. */

     /*
     ** Clearing the pins in context and retaining the other pin values
     ** of PINMUX15 register.
     */
     savePinMux = HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(15)) & \
                        ~(SYSCFG_PINMUX15_PINMUX15_31_28 | \
                          SYSCFG_PINMUX15_PINMUX15_27_24 | \
                          SYSCFG_PINMUX15_PINMUX15_23_20 | \
                          SYSCFG_PINMUX15_PINMUX15_19_16 | \
                          SYSCFG_PINMUX15_PINMUX15_15_12 | \
                          SYSCFG_PINMUX15_PINMUX15_11_8 | \
                          SYSCFG_PINMUX15_PINMUX15_7_4 | \
                          SYSCFG_PINMUX15_PINMUX15_3_0);


     /*
     ** Performing the actual Pin Multiplexing to select relevant pins in
     ** PINMUX15  register.
     */
     HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(15)) = \
          (PINMUX15_VPIF_ENABLE | savePinMux);

     /* Programming the PINMUX16 register. */

     /*
     ** Clearing the pins in context and retaining the other pin values
     ** of PINMUX16 register.
     */
     //savePinMux = HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(16)) & \
                        ~(SYSCFG_PINMUX16_PINMUX16_31_28 | \
                          SYSCFG_PINMUX16_PINMUX16_27_24 | \
                          SYSCFG_PINMUX16_PINMUX16_23_20 | \
                          SYSCFG_PINMUX16_PINMUX16_19_16 | \
                          SYSCFG_PINMUX16_PINMUX16_15_12 | \
                          SYSCFG_PINMUX16_PINMUX16_11_8 | \
                          SYSCFG_PINMUX16_PINMUX16_7_4 | \
                          SYSCFG_PINMUX16_PINMUX16_3_0);


     /*
     ** Performing the actual Pin Multiplexing to select relevant pins in
     ** PINMUX16  register.
     */
     //HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(16)) = \
          (PINMUX16_VPIF_ENABLE | savePinMux);

     /* Programming the PINMUX17 register. */

     /*
     ** Clearing the pins in context and retaining the other pin values
     ** of PINMUX17 register.
     */
     //savePinMux = HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(17)) & \
                        ~(SYSCFG_PINMUX17_PINMUX17_31_28 | \
                          SYSCFG_PINMUX17_PINMUX17_27_24 | \
                          SYSCFG_PINMUX17_PINMUX17_23_20 | \
                          SYSCFG_PINMUX17_PINMUX17_19_16 | \
                          SYSCFG_PINMUX17_PINMUX17_15_12 | \
                          SYSCFG_PINMUX17_PINMUX17_11_8 | \
                          SYSCFG_PINMUX17_PINMUX17_7_4 | \
                          SYSCFG_PINMUX17_PINMUX17_3_0);

     /*
     ** Performing the actual Pin Multiplexing to select relevant pins in
     ** PINMUX17  register.
     */
     //HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(17)) = \
          (PINMUX17_VPIF_ENABLE | savePinMux);

     /* Programming the PINMUX18 register. */

     /*
     ** Clearing the pins in context and retaining the other pin values
     ** of PINMUX18 register.
     */
     //savePinMux = HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(18)) & \
                       ~(SYSCFG_PINMUX18_PINMUX18_7_4 |  \
                         SYSCFG_PINMUX18_PINMUX18_3_0);

     /*
     ** Performing the actual Pin Multiplexing to select relevant pins in
     ** PINMUX18  register.
     */
     //HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(18)) = \
           (PINMUX18_VPIF_ENABLE | savePinMux);


     /* Programming the PINMUX19 register. */

     /*
     ** Clearing the pins in context and retaining the other pin values
     ** of PINMUX19 register.
     */
     savePinMux = HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(19)) & \
                       ~(SYSCFG_PINMUX19_PINMUX19_23_20 | \
                         SYSCFG_PINMUX19_PINMUX19_19_16 | \
                         SYSCFG_PINMUX19_PINMUX19_15_12 | \
                         SYSCFG_PINMUX19_PINMUX19_11_8  | \
                         SYSCFG_PINMUX19_PINMUX19_7_4   | \
                         SYSCFG_PINMUX19_PINMUX19_3_0);



     /*
     ** Performing the actual Pin Multiplexing to select relevant pins in
     ** PINMUX19 register.
     */
     HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(19)) = \
          (PINMUX19_VPIF_ENABLE | savePinMux);

}


static void VPIFSetup(void);

/******************************************************************************
**                      全局变量定义
*******************************************************************************/

volatile char *srcBuff;
volatile char *dstBuff;

//#pragma DATA_ALIGN(rxBuf0, 4);
unsigned char rxBuf0[800*600*2];
unsigned char rxBuf1[800*600*2];

// 接收 buffer 指针数组
static unsigned int const rxBufPtr[2] =
       {
           (unsigned int) rxBuf0,
           (unsigned int) rxBuf1,
       };

static unsigned int buffcount = 0;
//static unsigned int processe = 1;

// 计数器（用于性能测试）
long long t_start, t_stop, t_overhead, t_test;
int frame_count = 0;
float fps;

/******************************************************************************
**                       入口函数
*******************************************************************************/
void TL2640Test(void)
{
//	char String1[64]={0};
//	char String2[64]={0};

	// 计数器（用于性能测试）
	TSCL = 0;
	TSCH = 0;
	t_start = _itoll(TSCH, TSCL);
	t_stop = _itoll(TSCH, TSCL);
	t_overhead = t_stop - t_start;

	ConsoleWrite("\r\n ============Test Start===========.\r\n");
	ConsoleWrite("Welcome to TL2640 Demo application.\r\n\r\n");

	// 使能外设
    PSCModuleControl(SOC_PSC_1_REGS, HW_PSC_VPIF, PSC_POWERDOMAIN_ALWAYS_ON, PSC_MDCTL_NEXT_ENABLE);

	// I2C 配置
	I2CInit(SOC_I2C_0_REGS, 0x30);

	/* 初始化ov2640 */
	OV2640Reset();
	Delay(0xffff);

	/*初始化OV2640为SVGA 800*600*/
	OV2640Init();

    /*初始化VPIF*/
    VPIFSetup();

//	// 设置文字显示
//	// 设置画笔为黄色
//	GrContextForegroundSet(&g_sContext, ClrYellow);
//	// 设置字体 字号
//	GrContextFontSet(&g_sContext, &g_sFontCm18);
//	// 设置背景色
//	GrContextBackgroundSet(&g_sContext, ClrBlue);
//
//	while(1)
//    {
//		while(!VPIFInterruptStatus(SOC_VPIF_0_REGS, VPIF_FRAMEINT_CH0));
//		VPIFInterruptStatusClear(SOC_VPIF_0_REGS, VPIF_FRAMEINT_CH0);
//
//		processe = buffcount;
//		buffcount++;
//		if (buffcount == 2)
//			buffcount = 0;
//
//		// 初始化数据地址
//		VPIFCaptureFBConfig(SOC_VPIF_0_REGS, VPIF_CHANNEL_0,
//				VPIF_TOP_FIELD, VPIF_LUMA,(unsigned int)rxBufPtr[buffcount], 800*2);
//
//		// 失能缓存
//		Cache_inv((void *)rxBufPtr[processe], 800*600*2, Cache_Type_ALLD, TRUE);
//
//		srcBuff = (char *)(rxBufPtr[processe]) + 800*60*2;
//		dstBuff = (char *)(g_pucBuffer+PALETTE_OFFSET+PALETTE_SIZE);
//		// 复制图像数据到LCD BUFFER
//		memcpy((void*)dstBuff,(void*)srcBuff,800*480*2);
//
//		// 显示 CPU 负载
//		GrStringDraw(&g_sContext, String1, -1, 650, 450, true);
//		// 显示帧率
//		GrStringDraw(&g_sContext, String2, -1, 550, 450, true);
//
//		if(frame_count == 0)
//		{
//			// 计数开始值
//			t_start = _itoll(TSCH, TSCL);
//		}
//
//		frame_count++;
//
//		if(frame_count == 30)
//		{
//			// 计数结束值
//			t_test = 0;
//			t_stop = _itoll(TSCH, TSCL);
//			t_test = (t_stop - t_start) - t_overhead;
//			fps = (float)frame_count / ((float)t_test/456000000);
//			frame_count = 0;
//			sprintf(String2, " fps = %0.1f ", fps);
//		}
//		sprintf(String1, " CPU Load %d%% ", Load_getCPULoad());
//
//		Task_sleep(5);
//    }
}

/****************************************************************************/
/*                                                                          */
/*                       初始化VPIF捕获               						*/
/*                                                                          */
/****************************************************************************/
static void VPIFSetup(void)
{
	/* 设置VPIF pinmux*/
	VPIFPinMuxSetUp();

	/* Disable interrupts */
    VPIFInterruptDisable(SOC_VPIF_0_REGS, VPIF_FRAMEINT_CH1);
    VPIFInterruptDisable(SOC_VPIF_0_REGS, VPIF_FRAMEINT_CH0);

    /* Disable capture ports */
    VPIFCaptureChanenDisable(SOC_VPIF_0_REGS, VPIF_CHANNEL_1);
    VPIFCaptureChanenDisable(SOC_VPIF_0_REGS, VPIF_CHANNEL_0);

	/*数据位宽8位*/
	VPIFCaptureRawDatawidthConfig(SOC_VPIF_0_REGS,
				VPIF_C0CTRL_DATAWIDTH_EIGHT_BPS<<VPIF_C0CTRL_DATAWIDTH_SHIFT);

	/*产生600个line*/
	VPIFCaptureRawIntlineConfig(SOC_VPIF_0_REGS, 600);

	/*设置存储模式为Frame-based storage*/
	VPIFCaptureFieldframeModeSelect(SOC_VPIF_0_REGS, VPIF_C0CTRL_FIELDFRAME);

	/*设置progressive模式接收数据*/
	VPIFCaptureIntrprogModeSelect(SOC_VPIF_0_REGS, VPIF_CHANNEL_0,
										VPIF_C0CTRL_INTRPROG);
//	VPIFCaptureIntrprogModeSelect(SOC_VPIF_0_REGS, VPIF_CHANNEL_1,
//										VPIF_C0CTRL_INTRPROG);

	/*使能垂直消隐捕获*/
	VPIFCaptureVancEnable(SOC_VPIF_0_REGS, VPIF_CHANNEL_0);
//	VPIFCaptureVancEnable(SOC_VPIF_0_REGS, VPIF_CHANNEL_1);

	/*使能行消隐捕获*/
	VPIFCaptureVancDisable(SOC_VPIF_0_REGS, VPIF_CHANNEL_0);
//	VPIFCaptureVancDisable(SOC_VPIF_0_REGS, VPIF_CHANNEL_1);

	/*设置中断产生方式，只是用top field接收数据*/
	VPIFCaptureIntframeConfig(SOC_VPIF_0_REGS, VPIF_CHANNEL_0,
										VPIF_FRAME_INTERRUPT_TOP);
//	VPIFCaptureIntframeConfig(SOC_VPIF_0_REGS, VPIF_CHANNEL_1,
//										VPIF_FRAME_INTERRUPT_TOP);

	/*设置捕获方式为raw capture*/
	VPIFCaptureCapmodeModeSelect(SOC_VPIF_0_REGS, VPIF_CHANNEL_0,
										VPIF_CAPTURE_RAW);
//	VPIFCaptureCapmodeModeSelect(SOC_VPIF_0_REGS, VPIF_CHANNEL_1,
//										VPIF_CAPTURE_RAW);

	/*256字节DMA*/
	VPIFDMARequestSizeConfig(SOC_VPIF_0_REGS, VPIF_REQSIZE_TWO_FIFTY_SIX);

	/* 初始化VPIF buffer地址*/
	VPIFCaptureFBConfig(SOC_VPIF_0_REGS, VPIF_CHANNEL_0,
			VPIF_TOP_FIELD, VPIF_LUMA,(unsigned int)rxBufPtr[buffcount], 800*2);
//    VPIFCaptureFBConfig(SOC_VPIF_0_REGS, VPIF_CHANNEL_1,
//    		VPIF_TOP_FIELD, VPIF_LUMA,(unsigned int)rxBufPtr[buffcount], 800*2);

	/*使能错误中断和frame0中断*/
	VPIFInterruptEnable(SOC_VPIF_0_REGS, VPIF_FRAMEINT_CH0 | VPIF_ERROR_INT);
	VPIFInterruptEnableSet(SOC_VPIF_0_REGS, VPIF_FRAMEINT_CH0);

	/* Enable capture */
	VPIFCaptureChanenEnable(SOC_VPIF_0_REGS, VPIF_CHANNEL_0);
	VPIFCaptureChanenEnable(SOC_VPIF_0_REGS, VPIF_CHANNEL_1);
}
