/****************************************************************************/
/*                                                                          */
/*    �º˿Ƽ�(����)���޹�˾                                                */
/*                                                                          */
/*    Copyright (C) 2022 CoreKernel Technology (Guangzhou) Co., Ltd         */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/*    EHRPWM �߾�������������                                             */
/*                                                                          */
/*    2014��06��07��                                                        */
/*                                                                          */
/****************************************************************************/
/*
 *    �߾��� PWM ������� LED �����
 *
 *    - ϣ����Ĭ(bin wang)
 *    - bin@corekernel.net
 *
 *    ���� corekernel.net/.org/.cn
 *    ���� fpga.net.cn
 *
 */
#include <App.h>

#include "ehrpwm.h"

/* ע�⣺DSP ports, Shared RAM, UART0, EDMA, SPI0, MMC/SD0/1,
         VPIF, LCDC, SATA, uPP, DDR2/mDDR (bus ports), USB2.0, HPI, PRU
         ��Щ����ʹ�õ�ʱ����ԴΪ PLL0_SYSCLK2 Ĭ��Ƶ��Ϊ CPU Ƶ�ʵĶ���֮һ
         ���� ECAP0/1/2, UART1/2, Timer64P2/3, eHRPWM0/1, McBSP0/1, McASP0, SPI1
         ��Щ�����ʱ����Դ������ PLL0_SYSCLK2 �� PLL1_SYSCLK2 ��ѡ��
         ͨ���޸� System Configuration (SYSCFG) Module
         �Ĵ��� Chip Configuration 3 Register (CFGCHIP3) ����λ ASYNC3_CLKSRC
         ����ʱ����Դ
         ��Ĭ��ֵ�� 0 ��Դ�� PLL0_SYSCLK2
                    1 ��Դ�� PLL1_SYSCLK2
         �������Ϊ�˽��͹��ģ��������޸����ֵ������Ӱ��������������ʱ��Ƶ��
*/

/****************************************************************************/
/*                                                                          */
/*              �궨��                                                      */
/*                                                                          */
/****************************************************************************/
// ʱ�ӷ�Ƶ
#define CLOCK_DIV_VAL     228

/****************************************************************************/
/*                                                                          */
/*              GPIO �ܽŸ�������                                           */
/*                                                                          */
/****************************************************************************/
void GPIOBankPinMuxSet()
{
    // EPWM1A
    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(5)) = (HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(5)) & (~(SYSCFG_PINMUX5_PINMUX5_3_0))) |
                                                   ((SYSCFG_PINMUX5_PINMUX5_3_0_EPWM1A << SYSCFG_PINMUX5_PINMUX5_3_0_SHIFT));

    // EPWM1B
    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(5)) = (HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(5)) & (~(SYSCFG_PINMUX5_PINMUX5_7_4))) |
                                                   ((SYSCFG_PINMUX5_PINMUX5_7_4_EPWM1B << SYSCFG_PINMUX5_PINMUX5_7_4_SHIFT));

    // EPWM1TZ[0]
    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(2)) = (HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(2)) & (~(SYSCFG_PINMUX2_PINMUX2_3_0))) |
                                                   ((SYSCFG_PINMUX2_PINMUX2_3_0_EPWM1TZ0 << SYSCFG_PINMUX2_PINMUX2_3_0_SHIFT));

    // ʹ�� PWM ʱ��
    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_CFGCHIP1) |= SYSCFG_CFGCHIP1_TBCLKSYNC;
}

/****************************************************************************/
/*                                                                          */
/*              PWM ��ʼ��                                                  */
/*                                                                          */
/****************************************************************************/
void PWMSet(unsigned int pwm_clk, unsigned short duty_ratio)
{
    // ʱ���׼����
    // ʱ������
    EHRPWMTimebaseClkConfig(SOC_EHRPWM_1_REGS, SOC_EHRPWM_1_MODULE_FREQ / CLOCK_DIV_VAL, SOC_EHRPWM_1_MODULE_FREQ);

    // ��������
    EHRPWMPWMOpFreqSet(SOC_EHRPWM_1_REGS, SOC_EHRPWM_1_MODULE_FREQ / CLOCK_DIV_VAL, pwm_clk, EHRPWM_COUNT_UP, EHRPWM_SHADOW_WRITE_DISABLE);

    // ��������ͬ���ź�
    EHRPWMTimebaseSyncDisable(SOC_EHRPWM_1_REGS);

    // �������ͬ���ź�
    EHRPWMSyncOutModeSet(SOC_EHRPWM_1_REGS, EHRPWM_SYNCOUT_DISABLE);

    // ����ģʽ��Ϊ����
    EHRPWMTBEmulationModeSet(SOC_EHRPWM_1_REGS, EHRPWM_STOP_AFTER_NEXT_TB_INCREMENT);

    // ���ü����Ƚ�����ģ��
    // ���رȽ��� A ֵ
    EHRPWMLoadCMPA(SOC_EHRPWM_1_REGS, (SOC_EHRPWM_1_MODULE_FREQ / CLOCK_DIV_VAL / pwm_clk) * duty_ratio / 100, EHRPWM_SHADOW_WRITE_DISABLE, EHRPWM_COMPA_NO_LOAD, EHRPWM_CMPCTL_OVERWR_SH_FL);

    // ���رȽ��� B ֵ
    EHRPWMLoadCMPB(SOC_EHRPWM_1_REGS, 0, EHRPWM_SHADOW_WRITE_DISABLE, EHRPWM_COMPB_NO_LOAD, EHRPWM_CMPCTL_OVERWR_SH_FL);

    // �����޶����ã�������Ŵ�����ʽ�趨��
    // ʱ���׼����������Ч�����ȽϼĴ��� A/B ֵʱ EPWM1_A ��ת ������ EPWM1_A ���
    EHRPWMConfigureAQActionOnA(SOC_EHRPWM_1_REGS, EHRPWM_AQCTLA_ZRO_DONOTHING, EHRPWM_AQCTLA_PRD_DONOTHING,
                                 EHRPWM_AQCTLA_CAU_EPWMXATOGGLE, EHRPWM_AQCTLA_CAD_DONOTHING, EHRPWM_AQCTLA_CBU_EPWMXATOGGLE,
                                 EHRPWM_AQCTLA_CBD_DONOTHING, EHRPWM_AQSFRC_ACTSFA_DONOTHING);

    // ��������ģ�飨��·��ģ�� �ź�ֱ�������ն����ģ�飩
    EHRPWMDBOutput(SOC_EHRPWM_1_REGS, EHRPWM_DBCTL_OUT_MODE_BYPASS);

    // ����ն����ģ��
    EHRPWMChopperDisable(SOC_EHRPWM_1_REGS);

    // ���ô�������¼�
    EHRPWMTZTripEventDisable(SOC_EHRPWM_1_REGS, EHRPWM_TZ_ONESHOT);
    EHRPWMTZTripEventDisable(SOC_EHRPWM_1_REGS, EHRPWM_TZ_CYCLEBYCYCLE);

    // �¼���������
    // ÿ�����¼����������ж�
    EHRPWMETIntPrescale(SOC_EHRPWM_1_REGS, EHRPWM_ETPS_INTPRD_THIRDEVENT);

    // ʱ���׼����������Ч�����ȽϼĴ��� B ֵ �����¼�
    EHRPWMETIntSourceSelect(SOC_EHRPWM_1_REGS, EHRPWM_ETSEL_INTSEL_TBCTREQUCMPBINC);

    // ʹ���ж�
    EHRPWMETIntEnable(SOC_EHRPWM_1_REGS);

    // ���ø߾�����ģ��
    EHRPWMHRDisable(SOC_EHRPWM_1_REGS);
}

/****************************************************************************/
/*                                                                          */
/*              Ӳ���ж��߳�                                                */
/*                                                                          */
/****************************************************************************/
/*
Void PWMEventHwi(UArg arg)
{
    EHRPWMETIntClear(SOC_EHRPWM_1_REGS);
}

Void PWMTZHwi(UArg arg)
{
    EHRPWMETIntClear(SOC_EHRPWM_1_REGS);
}

static Void HwiInit()
{
    EventCombiner_dispatchPlug(SYS_INT_EHRPWM1, &PWMEventHwi, 1, TRUE);
    EventCombiner_dispatchPlug(SYS_INT_EHRPWM1TZ, &PWMTZHwi, 1, TRUE);
}
*/

/****************************************************************************/
/*                                                                          */
/*              EHRPWM ��ʼ��                                               */
/*                                                                          */
/****************************************************************************/
void EHRPWMInit()
{
    // ʹ������
    PSCModuleControl(SOC_PSC_1_REGS, HW_PSC_EHRPWM, PSC_POWERDOMAIN_ALWAYS_ON, PSC_MDCTL_NEXT_ENABLE);

    // �ܽŸ�������
    GPIOBankPinMuxSet();

    // EHRPWM ��ʼ��
    PWMSet(10000, 80);

    // Ӳ���ж��̳߳�ʼ��
//  HwiInit();
}
