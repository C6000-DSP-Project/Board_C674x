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
#include "hw_psc_C6748.h"

#include "soc_C6748.h"

#include "psc.h"
#include "gpio.h"
#include "uart.h"
#include "i2c.h"
#include "spi.h"

#include "interrupt.h"

#include "LCD/LCD.h"
#include "LCD/Touch.h"

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

/****************************************************************************/
/*                                                                          */
/*              函数声明                                                    */
/*                                                                          */
/****************************************************************************/
void LEDInit();
void LEDControl(unsigned char LED, char Status);
void LEDNixieInit();

void KEYInit();

void BUZZERInit();
void BUZZERBeep(unsigned int t);

void UARTConsoleInit();
void UART1Init();

void TimerInit();

void I2CInit(unsigned int baseAddr, unsigned int slaveAddr);
unsigned char I2CRegRead(unsigned int baseAddr , unsigned char regAddr);
void I2CRegWrite(unsigned int baseAddr, unsigned char regAddr, unsigned char regData);

void TempSensorInit();

void RTCInit();
void RTCSet();

Void ECAPInit();
Void PWMInit(unsigned int pwm_clk, unsigned short duty_ratio);

Void SPIFlashInit();
Void SPIFlashTest(Void);

Void LCDInit();
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
