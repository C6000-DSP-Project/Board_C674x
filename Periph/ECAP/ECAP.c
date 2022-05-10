/****************************************************************************/
/*                                                                          */
/*    新核科技(广州)有限公司                                                */
/*                                                                          */
/*    Copyright (C) 2022 CoreKernel Technology (Guangzhou) Co., Ltd         */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/*    ECAP 辅助 PWM 输出                                                    */
/*                                                                          */
/*    2022年05月10日                                                        */
/*                                                                          */
/****************************************************************************/
/*
 *    ECAP 辅助 PWM 输出调整 LCD 亮度
 *
 *    - 希望缄默(bin wang)
 *    - bin@corekernel.net
 *
 *    官网 corekernel.net/.org/.cn
 *    社区 fpga.net.cn
 *
 */
#include <App.h>

#include "ecap.h"

/****************************************************************************/
/*                                                                          */
/*              宏定义                                                      */
/*                                                                          */
/****************************************************************************/
// 周期计数
#define Period            22800     // 10KHz

// 占空比
#define DutyCycle         0.5

/****************************************************************************/
/*                                                                          */
/*              GPIO 管脚复用配置                                           */
/*                                                                          */
/****************************************************************************/
static void GPIOBankPinMuxSet()
{
    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(1)) = (HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(1)) & (~(SYSCFG_PINMUX1_PINMUX1_3_0))) |
                                                    ((SYSCFG_PINMUX1_PINMUX1_3_0_ECAP2 << SYSCFG_PINMUX1_PINMUX1_3_0_SHIFT));
}

/****************************************************************************/
/*                                                                          */
/*              ECAP 初始化                                                 */
/*                                                                          */
/****************************************************************************/
void ECAPAPWMInit()
{
    // 配置 ECAP 2 为 APWM 模式
    ECAPOperatingModeSelect(SOC_ECAP_2_REGS, ECAP_APWM_MODE);

    // 配置周期及占空比（比较计数器值）
    ECAPAPWMCaptureConfig(SOC_ECAP_2_REGS, Period * DutyCycle, Period);

    // 输出相位配置
    ECAPAPWMPolarityConfig(SOC_ECAP_2_REGS, ECAP_APWM_ACTIVE_HIGH);

    // 启动比较计数器
    ECAPCounterControl(SOC_ECAP_2_REGS, ECAP_COUNTER_FREE_RUNNING);

    // 计数值与比较器值相等时产生中断
    ECAPIntEnable(SOC_ECAP_2_REGS, ECAP_CMPEQ_INT);

    // 使能 ECAP 全局中断
    ECAPGlobalIntEnable(SOC_ECAP_2_REGS);
}

/****************************************************************************/
/*                                                                          */
/*              硬件中断线程                                                */
/*                                                                          */
/****************************************************************************/
Void ECAPHwi(UArg arg)
{
    // 禁用中断
    ECAPIntDisable(SOC_ECAP_2_REGS, ECAP_CMPEQ_INT);

    // ECAP 中断状态清除
    ECAPIntStatusClear(SOC_ECAP_2_REGS, ECAP_CMPEQ_INT);

    // 使能中断
    ECAPIntEnable(SOC_ECAP_2_REGS, ECAP_CMPEQ_INT);
}

static Void HwiInit()
{
    EventCombiner_dispatchPlug(SYS_INT_ECAP2, &ECAPHwi, 1, TRUE);
}

/****************************************************************************/
/*                                                                          */
/*              ECAP 初始化                                                 */
/*                                                                          */
/****************************************************************************/
void ECAPInit()
{
    // 使能外设
    PSCModuleControl(SOC_PSC_1_REGS, HW_PSC_ECAP0_1_2, PSC_POWERDOMAIN_ALWAYS_ON, PSC_MDCTL_NEXT_ENABLE);

    // GPIO 管脚复用配置
    GPIOBankPinMuxSet();

    // ECAP 初始化
    ECAPAPWMInit();

    // 硬件中断线程初始化
    HwiInit();
}
