/****************************************************************************/
/*                                                                          */
/*              LCD 触摸屏                                                  */
/*                                                                          */
/*              2014年07月29日                                              */
/*                                                                          */
/****************************************************************************/
#ifndef _TOUCH_H_
#define _TOUCH_H_

// 库
#include "gpio.h"                    // 通用输入输出口宏及设备抽象层函数声明

#define PEN GPIOPinRead(SOC_GPIO_0_REGS, 45)

/****************************************************************************/
/*                                                                          */
/*              函数声明                                                    */
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
