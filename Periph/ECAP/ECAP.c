/****************************************************************************/
/*                                                                          */
/*    �º˿Ƽ�(����)���޹�˾                                                */
/*                                                                          */
/*    Copyright (C) 2022 CoreKernel Technology (Guangzhou) Co., Ltd         */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/*    ECAP ���� PWM ���                                                    */
/*                                                                          */
/*    2022��05��10��                                                        */
/*                                                                          */
/****************************************************************************/
/*
 *    ECAP ���� PWM ������� LCD ����
 *
 *    - ϣ����Ĭ(bin wang)
 *    - bin@corekernel.net
 *
 *    ���� corekernel.net/.org/.cn
 *    ���� fpga.net.cn
 *
 */
#include <App.h>

#include "ecap.h"

/****************************************************************************/
/*                                                                          */
/*              �궨��                                                      */
/*                                                                          */
/****************************************************************************/
// ���ڼ���
#define Period            22800     // 10KHz

// ռ�ձ�
#define DutyCycle         0.5

/****************************************************************************/
/*                                                                          */
/*              GPIO �ܽŸ�������                                           */
/*                                                                          */
/****************************************************************************/
static void GPIOBankPinMuxSet()
{
    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(1)) = (HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(1)) & (~(SYSCFG_PINMUX1_PINMUX1_3_0))) |
                                                    ((SYSCFG_PINMUX1_PINMUX1_3_0_ECAP2 << SYSCFG_PINMUX1_PINMUX1_3_0_SHIFT));
}

/****************************************************************************/
/*                                                                          */
/*              ECAP ��ʼ��                                                 */
/*                                                                          */
/****************************************************************************/
void ECAPAPWMInit()
{
    // ���� ECAP 2 Ϊ APWM ģʽ
    ECAPOperatingModeSelect(SOC_ECAP_2_REGS, ECAP_APWM_MODE);

    // �������ڼ�ռ�ձȣ��Ƚϼ�����ֵ��
    ECAPAPWMCaptureConfig(SOC_ECAP_2_REGS, Period * DutyCycle, Period);

    // �����λ����
    ECAPAPWMPolarityConfig(SOC_ECAP_2_REGS, ECAP_APWM_ACTIVE_HIGH);

    // �����Ƚϼ�����
    ECAPCounterControl(SOC_ECAP_2_REGS, ECAP_COUNTER_FREE_RUNNING);

    // ����ֵ��Ƚ���ֵ���ʱ�����ж�
    ECAPIntEnable(SOC_ECAP_2_REGS, ECAP_CMPEQ_INT);

    // ʹ�� ECAP ȫ���ж�
    ECAPGlobalIntEnable(SOC_ECAP_2_REGS);
}

/****************************************************************************/
/*                                                                          */
/*              Ӳ���ж��߳�                                                */
/*                                                                          */
/****************************************************************************/
Void ECAPHwi(UArg arg)
{
    // �����ж�
    ECAPIntDisable(SOC_ECAP_2_REGS, ECAP_CMPEQ_INT);

    // ECAP �ж�״̬���
    ECAPIntStatusClear(SOC_ECAP_2_REGS, ECAP_CMPEQ_INT);

    // ʹ���ж�
    ECAPIntEnable(SOC_ECAP_2_REGS, ECAP_CMPEQ_INT);
}

static Void HwiInit()
{
    EventCombiner_dispatchPlug(SYS_INT_ECAP2, &ECAPHwi, 1, TRUE);
}

/****************************************************************************/
/*                                                                          */
/*              ECAP ��ʼ��                                                 */
/*                                                                          */
/****************************************************************************/
void ECAPInit()
{
    // ʹ������
    PSCModuleControl(SOC_PSC_1_REGS, HW_PSC_ECAP0_1_2, PSC_POWERDOMAIN_ALWAYS_ON, PSC_MDCTL_NEXT_ENABLE);

    // GPIO �ܽŸ�������
    GPIOBankPinMuxSet();

    // ECAP ��ʼ��
    ECAPAPWMInit();

    // Ӳ���ж��̳߳�ʼ��
    HwiInit();
}
