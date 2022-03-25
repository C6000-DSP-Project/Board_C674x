/****************************************************************************/
/*                                                                          */
/*              LCD 触摸屏                                                  */
/*                                                                          */
/*              2014年10月14日                                              */
/*                                                                          */
/****************************************************************************/
#include <App.h>

/****************************************************************************/
/*                                                                          */
/*              宏定义                                                      */
/*                                                                          */
/****************************************************************************/

/****************************************************************************/
/*                                                                          */
/*              全局变量                                                    */
/*                                                                          */
/****************************************************************************/


/****************************************************************************/
/*                                                                          */
/*              函数声明                                                    */
/*                                                                          */
/****************************************************************************/

/****************************************************************************/
/*                                                                          */
/*              任务                                                        */
/*                                                                          */
/****************************************************************************/
void Display(void)
{
	// 背景图
//    GrImageDraw(&g_sContext, image, 0, 0);

    // 显示静态文本
  	GrContextFontSet(&g_sContext, TEXT_FONT);
  	GrContextForegroundSet(&g_sContext, ClrSteelBlue);
    GrStringDraw(&g_sContext, "I Love C6748", -1, 50, 50, false);
}

void LCDTouchTest(void)
{
	// 触摸坐标
	extern int x;
	extern int y;
	extern char touch_flag;
	char String[64];

	RasterDisable(SOC_LCDC_0_REGS);
	RasterEnable(SOC_LCDC_0_REGS);

	Display();

	// 设置文字显示
	// 设置画笔为黄色
	GrContextForegroundSet(&g_sContext, ClrYellow);
	// 设置字体 字号
	GrContextFontSet(&g_sContext, &g_sFontCm18);
	// 设置背景色
	GrContextBackgroundSet(&g_sContext, ClrBlue);

	for(;;)
	{
//		RasterDisable(SOC_LCDC_0_REGS);
//		RasterEnable(SOC_LCDC_0_REGS);

		// 显示 CPU 负载
		sprintf(String, " CPU Load %d%% ", Load_getCPULoad());
		GrStringDraw(&g_sContext, String, -1, 650, 430, true);

		if(TouchDetect() == TRUE)
		{
			// 取得坐标值
			TouchCoOrdGet(&x, &y);

			// 显示在 LCD
			sprintf(String, " X %d Y %d ", x, y);
			GrStringDraw(&g_sContext, String, -1, 650, 50, true);

			touch_flag = 0;
		}
		Task_sleep(20);
	}
}
