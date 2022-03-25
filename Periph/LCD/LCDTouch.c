/****************************************************************************/
/*                                                                          */
/*              LCD ������                                                  */
/*                                                                          */
/*              2014��10��14��                                              */
/*                                                                          */
/****************************************************************************/
#include <App.h>

/****************************************************************************/
/*                                                                          */
/*              �궨��                                                      */
/*                                                                          */
/****************************************************************************/

/****************************************************************************/
/*                                                                          */
/*              ȫ�ֱ���                                                    */
/*                                                                          */
/****************************************************************************/


/****************************************************************************/
/*                                                                          */
/*              ��������                                                    */
/*                                                                          */
/****************************************************************************/

/****************************************************************************/
/*                                                                          */
/*              ����                                                        */
/*                                                                          */
/****************************************************************************/
void Display(void)
{
	// ����ͼ
//    GrImageDraw(&g_sContext, image, 0, 0);

    // ��ʾ��̬�ı�
  	GrContextFontSet(&g_sContext, TEXT_FONT);
  	GrContextForegroundSet(&g_sContext, ClrSteelBlue);
    GrStringDraw(&g_sContext, "I Love C6748", -1, 50, 50, false);
}

void LCDTouchTest(void)
{
	// ��������
	extern int x;
	extern int y;
	extern char touch_flag;
	char String[64];

	RasterDisable(SOC_LCDC_0_REGS);
	RasterEnable(SOC_LCDC_0_REGS);

	Display();

	// ����������ʾ
	// ���û���Ϊ��ɫ
	GrContextForegroundSet(&g_sContext, ClrYellow);
	// �������� �ֺ�
	GrContextFontSet(&g_sContext, &g_sFontCm18);
	// ���ñ���ɫ
	GrContextBackgroundSet(&g_sContext, ClrBlue);

	for(;;)
	{
//		RasterDisable(SOC_LCDC_0_REGS);
//		RasterEnable(SOC_LCDC_0_REGS);

		// ��ʾ CPU ����
		sprintf(String, " CPU Load %d%% ", Load_getCPULoad());
		GrStringDraw(&g_sContext, String, -1, 650, 430, true);

		if(TouchDetect() == TRUE)
		{
			// ȡ������ֵ
			TouchCoOrdGet(&x, &y);

			// ��ʾ�� LCD
			sprintf(String, " X %d Y %d ", x, y);
			GrStringDraw(&g_sContext, String, -1, 650, 50, true);

			touch_flag = 0;
		}
		Task_sleep(20);
	}
}
