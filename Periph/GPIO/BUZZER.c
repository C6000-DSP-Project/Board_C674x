/****************************************************************************/
/*                                                                          */
/*    �º˿Ƽ�(����)���޹�˾                                                */
/*                                                                          */
/*    Copyright (C) 2022 CoreKernel Technology (Guangzhou) Co., Ltd         */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/*    ������                                                                */
/*                                                                          */
/*    2022��03��25��                                                        */
/*                                                                          */
/****************************************************************************/
/*
 *    - ϣ����Ĭ(bin wang)
 *    - bin@corekernel.net
 *
 *    ���� corekernel.net/.org/.cn
 *    ���� fpga.net.cn
 *
 */
#include <App.h>

/****************************************************************************/
/*                                                                          */
/*              GPIO �ܽŸ�������                                           */
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
/*              GPIO �ܽų�ʼ��                                             */
/*                                                                          */
/****************************************************************************/
static void GPIOBankPinInit()
{
    GPIODirModeSet(SOC_GPIO_0_REGS, BUZZER, GPIO_DIR_OUTPUT);    // GPIO0[00]
}

/****************************************************************************/
/*                                                                          */
/*              ����������                                                  */
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
/*              ��ʼ��                                                      */
/*                                                                          */
/****************************************************************************/
void BUZZERInit()
{
	// ʹ������
    PSCModuleControl(SOC_PSC_1_REGS, HW_PSC_GPIO, PSC_POWERDOMAIN_ALWAYS_ON, PSC_MDCTL_NEXT_ENABLE);

    // �ܽŸ�������
    GPIOBankPinMuxSet();

    // GPIO �ܽų�ʼ��
    GPIOBankPinInit();
}
