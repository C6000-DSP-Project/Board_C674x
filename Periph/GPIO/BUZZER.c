/****************************************************************************/
/*                                                                          */
/*    新核科技(广州)有限公司                                                */
/*                                                                          */
/*    Copyright (C) 2022 CoreKernel Technology (Guangzhou) Co., Ltd         */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/*    蜂鸣器                                                                */
/*                                                                          */
/*    2022年03月25日                                                        */
/*                                                                          */
/****************************************************************************/
/*
 *    - 希望缄默(bin wang)
 *    - bin@corekernel.net
 *
 *    官网 corekernel.net/.org/.cn
 *    社区 fpga.net.cn
 *
 */
#include <App.h>

/****************************************************************************/
/*                                                                          */
/*              GPIO 管脚复用配置                                           */
/*                                                                          */
/****************************************************************************/
static void GPIOBankPinMuxSet()
{
    unsigned int savePinMux = 0;

    savePinMux = (HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(1)) & ~(SYSCFG_PINMUX1_PINMUX1_31_28));
    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(1)) = ((SYSCFG_PINMUX1_PINMUX1_31_28_GPIO0_0 << SYSCFG_PINMUX1_PINMUX1_31_28_SHIFT) | savePinMux);
}

/****************************************************************************/
/*                                                                          */
/*              GPIO 管脚初始化                                             */
/*                                                                          */
/****************************************************************************/
static void GPIOBankPinInit()
{
    GPIODirModeSet(SOC_GPIO_0_REGS, BUZZER, GPIO_DIR_OUTPUT);    // GPIO0[00]
}

/****************************************************************************/
/*                                                                          */
/*              蜂鸣器控制                                                  */
/*                                                                          */
/****************************************************************************/
void BUZZERBeep(unsigned int t)
{
	GPIOPinWrite(SOC_GPIO_0_REGS, BUZZER, GPIO_PIN_HIGH);
	Delay(t);
	GPIOPinWrite(SOC_GPIO_0_REGS, BUZZER, GPIO_PIN_LOW);
}

/****************************************************************************/
/*                                                                          */
/*              初始化                                                      */
/*                                                                          */
/****************************************************************************/
void BUZZERInit()
{
	// 使能外设
    PSCModuleControl(SOC_PSC_1_REGS, HW_PSC_GPIO, PSC_POWERDOMAIN_ALWAYS_ON, PSC_MDCTL_NEXT_ENABLE);

    // 管脚复用配置
    GPIOBankPinMuxSet();

    // GPIO 管脚初始化
    GPIOBankPinInit();
}
