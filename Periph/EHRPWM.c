/****************************************************************************/
/*                                                                          */
/*    新核科技(广州)有限公司                                                */
/*                                                                          */
/*    Copyright (C) 2022 CoreKernel Technology (Guangzhou) Co., Ltd         */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/*    EHRPWM 高精度脉宽调制输出                                             */
/*                                                                          */
/*    2014年06月07日                                                        */
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

#include "ehrpwm.h"

/* 注意：DSP ports, Shared RAM, UART0, EDMA, SPI0, MMC/SD0/1,
         VPIF, LCDC, SATA, uPP, DDR2/mDDR (bus ports), USB2.0, HPI, PRU
         这些外设使用的时钟来源为 PLL0_SYSCLK2 默认频率为 CPU 频率的二分之一
         但是 ECAP0/1/2, UART1/2, Timer64P2/3, eHRPWM0/1, McBSP0/1, McASP0, SPI1
         这些外设的时钟来源可以在 PLL0_SYSCLK2 和 PLL1_SYSCLK2 中选择
         通过修改 System Configuration (SYSCFG) Module
         寄存器 Chip Configuration 3 Register (CFGCHIP3) 第四位 ASYNC3_CLKSRC
         配置时钟来源
         （默认值） 0 来源于 PLL0_SYSCLK2
                    1 来源于 PLL1_SYSCLK2
         如果不是为了降低功耗，不建议修改这个值，它会影响所有相关外设的时钟频率
*/

/****************************************************************************/
/*                                                                          */
/*              宏定义                                                      */
/*                                                                          */
/****************************************************************************/
// 软件断点
#define SW_BREAKPOINT     asm(" SWBP 0 ");

// 时钟分频
#define CLOCK_DIV_VAL     228

/****************************************************************************/
/*                                                                          */
/*              GPIO 管脚复用配置                                           */
/*                                                                          */
/****************************************************************************/
void GPIOBankPinMuxSet()
{
    // EPWM1A
    #define PINMUX5_EPWM1A_ENABLE   (SYSCFG_PINMUX5_PINMUX5_3_0_EPWM1A << SYSCFG_PINMUX5_PINMUX5_3_0_SHIFT)

    // EPWM1B
    #define PINMUX5_EPWM1B_ENABLE   (SYSCFG_PINMUX5_PINMUX5_7_4_EPWM1B << SYSCFG_PINMUX5_PINMUX5_7_4_SHIFT)

    // EPWM1TZ0
    #define PINMUX2_EPWM1TZ0_ENABLE (SYSCFG_PINMUX2_PINMUX2_3_0_EPWM1TZ0 << SYSCFG_PINMUX2_PINMUX2_3_0_SHIFT)

    // EPWM1A
    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(5)) =
            (HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(5)) & (~(SYSCFG_PINMUX5_PINMUX5_3_0))) |
            (PINMUX5_EPWM1A_ENABLE);

    // EPWM1B
    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(5)) =
            (HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(5)) & (~(SYSCFG_PINMUX5_PINMUX5_7_4))) |
            (PINMUX5_EPWM1B_ENABLE);

    // EPWM1TZ[0]
    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(2)) =
            (HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(2)) & (~(SYSCFG_PINMUX2_PINMUX2_3_0))) |
            (PINMUX2_EPWM1TZ0_ENABLE);

    // 使能 PWM 时钟
    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_CFGCHIP1) |= SYSCFG_CFGCHIP1_TBCLKSYNC;
}

/****************************************************************************/
/*                                                                          */
/*              PWM 输出波形                                                */
/*                                                                          */
/****************************************************************************/
void PWM1ABasic(unsigned int pwm_clk,unsigned short duty_ratio)
{
    // 时间基准配置
    // 时钟配置
    EHRPWMTimebaseClkConfig(SOC_EHRPWM_1_REGS, SOC_EHRPWM_1_MODULE_FREQ/CLOCK_DIV_VAL , SOC_EHRPWM_1_MODULE_FREQ);

    // 配置周期
    EHRPWMPWMOpFreqSet(SOC_EHRPWM_1_REGS, SOC_EHRPWM_1_MODULE_FREQ/CLOCK_DIV_VAL,
		    pwm_clk, EHRPWM_COUNT_UP, EHRPWM_SHADOW_WRITE_DISABLE);

    // 禁用输入同步信号
    EHRPWMTimebaseSyncDisable(SOC_EHRPWM_1_REGS);

    // 禁用输出同步信号
    EHRPWMSyncOutModeSet(SOC_EHRPWM_1_REGS, EHRPWM_SYNCOUT_DISABLE);

    // 仿真(DEBUG)模式行为配置
    EHRPWMTBEmulationModeSet(SOC_EHRPWM_1_REGS, EHRPWM_STOP_AFTER_NEXT_TB_INCREMENT);

    // 配置计数比较器子模块
    // 加载比较器 A 值
    EHRPWMLoadCMPA(SOC_EHRPWM_1_REGS, (SOC_EHRPWM_1_MODULE_FREQ/CLOCK_DIV_VAL/pwm_clk)*duty_ratio/100, EHRPWM_SHADOW_WRITE_DISABLE,
		   EHRPWM_COMPA_NO_LOAD, EHRPWM_CMPCTL_OVERWR_SH_FL);

    // 加载比较器 B 值
    EHRPWMLoadCMPB(SOC_EHRPWM_1_REGS, 0, EHRPWM_SHADOW_WRITE_DISABLE,
		   EHRPWM_COMPB_NO_LOAD, EHRPWM_CMPCTL_OVERWR_SH_FL);

    // 功能限定配置（输出引脚触发方式设定）
    // 时间基准计数等于有效计数比较寄存器 A/B 值时EPWM1_A翻转,波形由EPWM1_A输出
    EHRPWMConfigureAQActionOnA(SOC_EHRPWM_1_REGS, EHRPWM_AQCTLA_ZRO_DONOTHING, EHRPWM_AQCTLA_PRD_DONOTHING,
		EHRPWM_AQCTLA_CAU_EPWMXATOGGLE,  EHRPWM_AQCTLA_CAD_DONOTHING,  EHRPWM_AQCTLA_CBU_EPWMXATOGGLE,
		EHRPWM_AQCTLA_CBD_DONOTHING, EHRPWM_AQSFRC_ACTSFA_DONOTHING);

    // 禁用（旁路，信号直接输出到斩波子模块）死区模块
    EHRPWMDBOutput(SOC_EHRPWM_1_REGS, EHRPWM_DBCTL_OUT_MODE_BYPASS);

    // 禁用斩波子模块
    EHRPWMChopperDisable(SOC_EHRPWM_1_REGS);

    // 禁用错误控制事件
    EHRPWMTZTripEventDisable(SOC_EHRPWM_1_REGS, EHRPWM_TZ_ONESHOT);
    EHRPWMTZTripEventDisable(SOC_EHRPWM_1_REGS, EHRPWM_TZ_CYCLEBYCYCLE);

    // 事件触发配置
    // 每三次事件发生产生中断
    EHRPWMETIntPrescale(SOC_EHRPWM_1_REGS, EHRPWM_ETPS_INTPRD_THIRDEVENT);
    // 时间基准计数等于有效计数比较寄存器 B 值 产生事件
    EHRPWMETIntSourceSelect(SOC_EHRPWM_1_REGS, EHRPWM_ETSEL_INTSEL_TBCTREQUCMPBINC);
    // 使能中断
    EHRPWMETIntEnable(SOC_EHRPWM_1_REGS);

    // 禁用高精度子模块
    EHRPWMHRDisable(SOC_EHRPWM_1_REGS);
}

void ChopperWaveform()
{
    // 50% 占空比
    EHRPWMConfigureChopperDuty(SOC_EHRPWM_1_REGS, EHRPWM_CHP_DUTY_50_PER);
    // 4 分频
    EHRPWMConfigureChopperFreq(SOC_EHRPWM_1_REGS, EHRPWM_PCCTL_CHPFREQ_DIVBY4);
    // 单个脉冲宽度
    EHRPWMConfigureChopperOSPW(SOC_EHRPWM_1_REGS, 0xF);
    // 使能斩波子模块
    EHRPWMChopperEnable(SOC_EHRPWM_1_REGS);
}

/****************************************************************************/
/*                                                                          */
/*              EHRPWM 初始化                                               */
/*                                                                          */
/****************************************************************************/
void PWMInit(unsigned int pwm_clk, unsigned short duty_ratio)
{
    // 外设使能配置
    PSCModuleControl(SOC_PSC_1_REGS, HW_PSC_EHRPWM, PSC_POWERDOMAIN_ALWAYS_ON, PSC_MDCTL_NEXT_ENABLE);

    // 管脚复用配置
    GPIOBankPinMuxSet();

    // 产生波形
    PWM1ABasic(pwm_clk, duty_ratio);
}
