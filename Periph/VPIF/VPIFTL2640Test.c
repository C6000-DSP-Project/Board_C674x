/****************************************************************************/
/*                                                                          */
/*              VPIF �ӿڼ� OV2640 CMOS ����������                          */
/*                                                                          */
/*              2014��07��17��                                              */
/*                                                                          */
/****************************************************************************/
#include <App.h>
#include <VPIF/OV2640.h>

#include "vpif.h"

/******************************************************************************
**                      �궨��
*******************************************************************************/

/******************************************************************************
**                      ����ԭ������
*******************************************************************************/
extern	void VPIFPinMuxSetUp(void);
static void VPIFSetup(void);

/******************************************************************************
**                      ȫ�ֱ�������
*******************************************************************************/

volatile char *srcBuff;
volatile char *dstBuff;

//#pragma DATA_ALIGN(rxBuf0, 4);
unsigned char rxBuf0[800*600*2];
unsigned char rxBuf1[800*600*2];

// ���� buffer ָ������
static unsigned int const rxBufPtr[2] =
       {
           (unsigned int) rxBuf0,
           (unsigned int) rxBuf1,
       };

static unsigned int buffcount = 0;
static unsigned int processe = 1;

// ���������������ܲ��ԣ�
long long t_start, t_stop, t_overhead, t_test;
int frame_count = 0;
float fps;

/******************************************************************************
**                       ��ں���
*******************************************************************************/
void TL2640Test(void)
{
	char String1[64]={0};
	char String2[64]={0};

	// ���������������ܲ��ԣ�
	TSCL = 0;
	TSCH = 0;
	t_start = _itoll(TSCH, TSCL);
	t_stop = _itoll(TSCH, TSCL);
	t_overhead = t_stop - t_start;

	ConsoleWrite("\r\n ============Test Start===========.\r\n");
	ConsoleWrite("Welcome to TL2640 Demo application.\r\n\r\n");

	// ʹ������
    PSCModuleControl(SOC_PSC_1_REGS, HW_PSC_VPIF, PSC_POWERDOMAIN_ALWAYS_ON, PSC_MDCTL_NEXT_ENABLE);

	// I2C ����
	I2CInit(SOC_I2C_0_REGS, 0x30);

	/* ��ʼ��ov2640 */
	TL2640Reset();
	Delay(0xffff);

	/*��ʼ��OV2640ΪSVGA 800*600*/
	TL2640Init();

    /*��ʼ��VPIF*/
    VPIFSetup();

	// ����������ʾ
	// ���û���Ϊ��ɫ
	GrContextForegroundSet(&g_sContext, ClrYellow);
	// �������� �ֺ�
	GrContextFontSet(&g_sContext, &g_sFontCm18);
	// ���ñ���ɫ
	GrContextBackgroundSet(&g_sContext, ClrBlue);

	while(1)
    {
		while(!VPIFInterruptStatus(SOC_VPIF_0_REGS, VPIF_FRAMEINT_CH0));
		VPIFInterruptStatusClear(SOC_VPIF_0_REGS, VPIF_FRAMEINT_CH0);

		processe = buffcount;
		buffcount++;
		if (buffcount == 2)
			buffcount = 0;

		// ��ʼ�����ݵ�ַ
		VPIFCaptureFBConfig(SOC_VPIF_0_REGS, VPIF_CHANNEL_0,
				VPIF_TOP_FIELD, VPIF_LUMA,(unsigned int)rxBufPtr[buffcount], 800*2);

		// ʧ�ܻ���
		Cache_inv((void *)rxBufPtr[processe], 800*600*2, Cache_Type_ALLD, TRUE);

		srcBuff = (char *)(rxBufPtr[processe]) + 800*60*2;
		dstBuff = (char *)(g_pucBuffer+PALETTE_OFFSET+PALETTE_SIZE);
		// ����ͼ�����ݵ�LCD BUFFER
		memcpy((void*)dstBuff,(void*)srcBuff,800*480*2);

		// ��ʾ CPU ����
		GrStringDraw(&g_sContext, String1, -1, 650, 450, true);
		// ��ʾ֡��
		GrStringDraw(&g_sContext, String2, -1, 550, 450, true);

		if(frame_count == 0)
		{
			// ������ʼֵ
			t_start = _itoll(TSCH, TSCL);
		}

		frame_count++;

		if(frame_count == 30)
		{
			// ��������ֵ
			t_test = 0;
			t_stop = _itoll(TSCH, TSCL);
			t_test = (t_stop - t_start) - t_overhead;
			fps = (float)frame_count / ((float)t_test/456000000);
			frame_count = 0;
			sprintf(String2, " fps = %0.1f ", fps);
		}
		sprintf(String1, " CPU Load %d%% ", Load_getCPULoad());

		Task_sleep(5);
    }
}

/****************************************************************************/
/*                                                                          */
/*                       ��ʼ��VPIF����               						*/
/*                                                                          */
/****************************************************************************/
static void VPIFSetup(void)
{
	/* ����VPIF pinmux*/
	VPIFPinMuxSetUp();

	/* Disable interrupts */
    VPIFInterruptDisable(SOC_VPIF_0_REGS, VPIF_FRAMEINT_CH1);
    VPIFInterruptDisable(SOC_VPIF_0_REGS, VPIF_FRAMEINT_CH0);

    /* Disable capture ports */
    VPIFCaptureChanenDisable(SOC_VPIF_0_REGS, VPIF_CHANNEL_1);
    VPIFCaptureChanenDisable(SOC_VPIF_0_REGS, VPIF_CHANNEL_0);

	/*����λ��8λ*/
	VPIFCaptureRawDatawidthConfig(SOC_VPIF_0_REGS,
				VPIF_C0CTRL_DATAWIDTH_EIGHT_BPS<<VPIF_C0CTRL_DATAWIDTH_SHIFT);

	/*����600��line*/
	VPIFCaptureRawIntlineConfig(SOC_VPIF_0_REGS, 600);

	/*���ô洢ģʽΪFrame-based storage*/
	VPIFCaptureFieldframeModeSelect(SOC_VPIF_0_REGS, VPIF_C0CTRL_FIELDFRAME);

	/*����progressiveģʽ��������*/
	VPIFCaptureIntrprogModeSelect(SOC_VPIF_0_REGS, VPIF_CHANNEL_0,
										VPIF_C0CTRL_INTRPROG);
//	VPIFCaptureIntrprogModeSelect(SOC_VPIF_0_REGS, VPIF_CHANNEL_1,
//										VPIF_C0CTRL_INTRPROG);

	/*ʹ�ܴ�ֱ��������*/
	VPIFCaptureVancEnable(SOC_VPIF_0_REGS, VPIF_CHANNEL_0);
//	VPIFCaptureVancEnable(SOC_VPIF_0_REGS, VPIF_CHANNEL_1);

	/*ʹ������������*/
	VPIFCaptureVancDisable(SOC_VPIF_0_REGS, VPIF_CHANNEL_0);
//	VPIFCaptureVancDisable(SOC_VPIF_0_REGS, VPIF_CHANNEL_1);

	/*�����жϲ�����ʽ��ֻ����top field��������*/
	VPIFCaptureIntframeConfig(SOC_VPIF_0_REGS, VPIF_CHANNEL_0,
										VPIF_FRAME_INTERRUPT_TOP);
//	VPIFCaptureIntframeConfig(SOC_VPIF_0_REGS, VPIF_CHANNEL_1,
//										VPIF_FRAME_INTERRUPT_TOP);

	/*���ò���ʽΪraw capture*/
	VPIFCaptureCapmodeModeSelect(SOC_VPIF_0_REGS, VPIF_CHANNEL_0,
										VPIF_CAPTURE_RAW);
//	VPIFCaptureCapmodeModeSelect(SOC_VPIF_0_REGS, VPIF_CHANNEL_1,
//										VPIF_CAPTURE_RAW);

	/*256�ֽ�DMA*/
	VPIFDMARequestSizeConfig(SOC_VPIF_0_REGS, VPIF_REQSIZE_TWO_FIFTY_SIX);

	/* ��ʼ��VPIF buffer��ַ*/
	VPIFCaptureFBConfig(SOC_VPIF_0_REGS, VPIF_CHANNEL_0,
			VPIF_TOP_FIELD, VPIF_LUMA,(unsigned int)rxBufPtr[buffcount], 800*2);
//    VPIFCaptureFBConfig(SOC_VPIF_0_REGS, VPIF_CHANNEL_1,
//    		VPIF_TOP_FIELD, VPIF_LUMA,(unsigned int)rxBufPtr[buffcount], 800*2);

	/*ʹ�ܴ����жϺ�frame0�ж�*/
	VPIFInterruptEnable(SOC_VPIF_0_REGS, VPIF_FRAMEINT_CH0 | VPIF_ERROR_INT);
	VPIFInterruptEnableSet(SOC_VPIF_0_REGS, VPIF_FRAMEINT_CH0);

	/* Enable capture */
	VPIFCaptureChanenEnable(SOC_VPIF_0_REGS, VPIF_CHANNEL_0);
	VPIFCaptureChanenEnable(SOC_VPIF_0_REGS, VPIF_CHANNEL_1);
}
