/****************************************************************************/
/*                                                                          */
/*              LCD                                                         */
/*                                                                          */
/*              2014��07��29��                                              */
/*                                                                          */
/****************************************************************************/
#ifndef _LCD_H_
#define _LCD_H_

// ��
#include "soc_C6748.h"               // DSP C6748 ����Ĵ���

// LCD ��غ�������
#include "raster.h"
#include "grlib.h"
#include "widget.h"
#include "canvas.h"
#include "pushbutton.h"
#include "checkbox.h"
#include "radiobutton.h"
#include "container.h"
#include "slider.h"

/****************************************************************************/
/*                                                                          */
/*              �궨��                                                      */
/*                                                                          */
/****************************************************************************/
// LCD ��ʾ����
#define TEXT_FONT               &g_sFontCmss22b
#define TEXT_HEIGHT             (GrFontHeightGet(TEXT_FONT))
#define BUFFER_METER_HEIGHT     TEXT_HEIGHT
#define BUFFER_METER_WIDTH      150

// LCD ʱ��
#define LCD_CLK                 228000000

#define PALETTE_SIZE            32

// LCD �ֱ���
#define LCD_WIDTH               800
#define LCD_HEIGHT              480

#define PALETTE_OFFSET          4

extern tContext g_sContext;
extern unsigned char g_pucBuffer[GrOffScreen16BPPSize(LCD_WIDTH, LCD_HEIGHT)];

/****************************************************************************/
/*                                                                          */
/*              ��������                                                    */
/*                                                                          */
/****************************************************************************/
void LCDInit(void);
void LCDBacklightEnable(void);
void LCDBacklightDisable(void);

#endif
