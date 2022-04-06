/****************************************************************************/
/*                                                                          */
/*    新核科技(广州)有限公司                                                */
/*                                                                          */
/*    Copyright (C) 2022 CoreKernel Technology Guangzhou Co., Ltd           */
/*                                                                          */
/****************************************************************************/
/*
 *   - 希望缄默(bin wang)
 *   - bin@corekernel.net
 *
 *    官网 corekernel.net/.org/.cn
 *   社区 fpga.net.cn
 *
 */
#ifndef APP_H_
#define APP_H_

#include <c6x.h>

#include "Log.h"

// C 标准库
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

// XDCTools
#include <xdc/std.h>

#include <xdc/runtime/Error.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Log.h>

// SYS/BIOS
#include <ti/sysbios/BIOS.h>

#include <ti/sysbios/knl/Swi.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>

#include <ti/sysbios/utils/Load.h>

#include <ti/sysbios/family/c64p/Hwi.h>
#include <ti/sysbios/family/c64p/EventCombiner.h>
#include <ti/sysbios/family/c64p/cache.h>
#include <ti/sysbios/family/c64p/Exception.h>
#include <ti/sysbios/family/c64p/TimestampProvider.h>

#include <ti/sysbios/timers/timer64/Timer.h>

#include <ti/sysbios/heaps/HeapBuf.h>
#include <ti/sysbios/heaps/HeapMem.h>

#include <ti/uia/events/UIABenchmark.h>
#include <ti/uia/events/UIAProfile.h>

// 算法库
#include "math.h"

// StarterWare 库
#include "hw_types.h"
#include "hw_syscfg0_C6748.h"
#include "hw_pllc_C6748.h"
#include "hw_psc_C6748.h"

#include "soc_C6748.h"

#include "psc.h"
#include "gpio.h"
#include "uart.h"
#include "i2c.h"
#include "spi.h"

#include "pru.h"

#include "interrupt.h"

#include "Periph/LCD/LCD.h"

// LVGL 图形库
#include "LVGL/lvgl.h"
#include "LVGL/porting/lv_port_disp_c674x.h"

/****************************************************************************/
/*                                                                          */
/*              宏定义                                                      */
/*                                                                          */
/****************************************************************************/
/* 外设定义 */
// LED
#define SOM_LED2    110
#define SOM_LED3    109

#define EVM_LED3    65
#define EVM_LED4    48

// LED 状态
#define LED_OFF     0
#define LED_ON      1

// KEY
#define EVM_KEY4    141
#define EVM_KEY6    9

// 蜂鸣器
#define BUZZER      1

// 调试串口
#define UARTConsole SOC_UART_2_REGS

/****************************************************************************/
/*                                                                          */
/*              全局变量                                                    */
/*                                                                          */
/****************************************************************************/
// MAC/IP 地址
extern unsigned char MacAddr[8];
extern char StrIP[16];

// RTC 时间
extern struct tm RTCTime;

// PLL
typedef struct
{
    unsigned int PLL0_SYSCLK1;  // DSP
    unsigned int PLL0_SYSCLK2;  // ARM RAM/ROM, DSP ports, Shared RAM, UART0, EDMA, SPI0, MMC/SD0/1, VPIF, LCDC, SATA, uPP, DDR2/mDDR (bus ports), USB2.0, HPI, PRU
    unsigned int PLL0_SYSCLK3;  // EMIFA
    unsigned int PLL0_SYSCLK4;  // SYSCFG, GPIO, PLL0/1, PSC, I2C1, EMAC/MDIO, USB1.1, ARM INTC
    unsigned int PLL0_SYSCLK5;  // 未使用
    unsigned int PLL0_SYSCLK6;  // ARM
    unsigned int PLL0_SYSCLK7;  // EMAC RMII Clock
    unsigned int PLL0_AUXCLK;   // PLL 旁路时钟(24MHz)
                                 // I2C0, Timer64P0/P1, RTC, USB2.0 PHY, McASP0 Serial Clock
    unsigned int PLL0_OBSCLK;   // 时钟输出

    unsigned int PLL1_SYSCLK1;  // DDR2/mDDR PHY
    unsigned int PLL1_SYSCLK2;  // ECAP0/1/2, UART1/2, Timer64P2/3, eHRPWM0/1, McBSP0/1, McASP0, SPI1 (默认使用 PLL0_SYSCLK2)
    unsigned int PLL1_SYSCLK3;  // PLL0 输入参考时钟
} PLLClock;

extern PLLClock pllcfg;

// 启动模式
extern unsigned int BootMode;
extern char *BootModeStr[];

// 触摸屏坐标
typedef struct
{
    unsigned char Flag;         // 触摸标志
    unsigned char Num;          // 触摸点数
    unsigned short X[10];       // 横坐标
    unsigned short Y[10];       // 纵坐标
} stTouchInfo;

extern stTouchInfo TouchInfo;

/****************************************************************************/
/*                                                                          */
/*              函数声明                                                    */
/*                                                                          */
/****************************************************************************/
void BootModeGet();
void PLLClockGet();

void LEDInit();
void LEDControl(unsigned char LED, char Status);
void LEDNixieInit();
void LEDMatrixInit();

void KEYInit();

void BUZZERInit();
void BUZZERBeep(unsigned int t);

void UARTConsoleInit();

void UART1Init();
unsigned int UART1Puts(char *pTxBuffer, int numBytesToWrite);

void TimerInit();

void I2CInit(unsigned int baseAddr, unsigned int slaveAddr);
unsigned char I2CRegRead(unsigned int baseAddr , unsigned char regAddr);
void I2CRegWrite(unsigned int baseAddr, unsigned char regAddr, unsigned char regData);
unsigned char I2CHWRegRead(unsigned int baseAddr, unsigned short regAddr);
void I2CHWRegWrite(unsigned int baseAddr, unsigned short regAddr, unsigned char regData);

void TempSensorInit();
void TempSensorGet(float *t, float *rh);

void RTCInit();
void RTCSet();

void LCDInit();
void TouchInit();

Void ECAPInit();
Void PWMInit(unsigned int pwm_clk, unsigned short duty_ratio);

Void SPIFlashInit();
Void SPIFlashTest(Void);

Void EDMAInit();
Void EEPROMTest(Void);

// SPI Flash 中断服务函数
Void SPIIsr(UArg arg);

void PruLEDTest(void);
void TL2640Test(void);
void SATATest(void);

int DSP_core_MEM_Test(unsigned int uiStartAddress, unsigned int uiStopAddress, unsigned int uiStep);

void Delay(volatile unsigned int count);

#endif
