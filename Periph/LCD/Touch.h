/****************************************************************************/
/*                                                                          */
/*              LCD ������                                                  */
/*                                                                          */
/*              2014��07��29��                                              */
/*                                                                          */
/****************************************************************************/
#ifndef _TOUCH_H_
#define _TOUCH_H_

// ��
#include "gpio.h"                    // ͨ����������ں꼰�豸����㺯������

#define PEN GPIOPinRead(SOC_GPIO_0_REGS, 45)

/****************************************************************************/
/*                                                                          */
/*              ��������                                                    */
/*                                                                          */
/****************************************************************************/
void TouchInit(void);
unsigned int TouchDetect(void);
void TouchCoOrdGet(int *pX, int *pY);
void ReadAxis(char mode, char*p1,char*p2);
void tsc2046ReadAxis(char mode, int*p1);
void TouchGPIOInit(void);
Void TouchIsr(UArg arg);

#endif
