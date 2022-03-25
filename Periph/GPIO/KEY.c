/****************************************************************************/
/*                                                                          */
/*    新核科技(广州)有限公司                                                */
/*                                                                          */
/*    Copyright (C) 2022 CoreKernel Technology (Guangzhou) Co., Ltd         */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/*    按键                                                                  */
/*                                                                          */
/*    2014年07月03日                                                        */
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
	// 配置相应的 GPIO 口功能为普通输入输出口
    unsigned int savePinMux = 0;

    // EVM-CKL138PKT 底板
    // GPIO0[8]
    savePinMux = (HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(0)) & ~(SYSCFG_PINMUX0_PINMUX0_31_28));
    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(0)) = ((SYSCFG_PINMUX0_PINMUX0_31_28_GPIO0_8 << SYSCFG_PINMUX0_PINMUX0_31_28_SHIFT) | savePinMux);

    // GPIO8[12]
    savePinMux = (HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(18)) & ~(SYSCFG_PINMUX18_PINMUX18_23_20));
    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(18)) = ((SYSCFG_PINMUX18_PINMUX18_23_20_GPIO8_12  << SYSCFG_PINMUX18_PINMUX18_23_20_SHIFT) | savePinMux);
}

/****************************************************************************/
/*                                                                          */
/*              GPIO 管脚初始化                                             */
/*                                                                          */
/****************************************************************************/
static void GPIOBankPinInit()
{
    // EVM-CKL138PKT 底板
	// 配置 LED 对应管脚为输入管脚
    GPIODirModeSet(SOC_GPIO_0_REGS, EVM_KEY6, GPIO_DIR_INPUT);          // GPIO0[08] SW6
    GPIODirModeSet(SOC_GPIO_0_REGS, EVM_KEY4, GPIO_DIR_INPUT);          // GPIO8[12] SW4

    // 配置中断触发方式
    GPIOIntTypeSet(SOC_GPIO_0_REGS, EVM_KEY6, GPIO_INT_TYPE_FALLEDGE);  // 下降沿
    GPIOIntTypeSet(SOC_GPIO_0_REGS, EVM_KEY4, GPIO_INT_TYPE_FALLEDGE);  // 上升沿及下降沿

    // 使能 GPIO BANK 中断
    GPIOBankIntEnable(SOC_GPIO_0_REGS, 0);                              // GPIO BANK0
    GPIOBankIntEnable(SOC_GPIO_0_REGS, 8);                              // GPIO BANK8
}

/****************************************************************************/
/*                                                                          */
/*              软件中断线程                                                */
/*                                                                          */
/****************************************************************************/
// 指令延时（非精确）
static Void Delay(UInt n)
{
    UInt i;

    for(i = n; i > 0; i--);
}

Swi_Handle KEY4SwiHandle;
Swi_Handle KEY6SwiHandle;

Void KEY4Swi(UArg a0, UArg a1)
{
    LEDControl(EVM_LED3, LED_ON);
    Delay(0x00FFFFFF);
    LEDControl(EVM_LED3, LED_OFF);
}

Void KEY6Swi(UArg a0, UArg a1)
{
    LEDControl(EVM_LED4, LED_ON);
    Delay(0x00FFFFFF);
    LEDControl(EVM_LED4, LED_OFF);
}

static Void SwiInit()
{
    Swi_Params SwiParams;
    Swi_Params_init(&SwiParams);

    SwiParams.priority = 2;      // 优先级
    SwiParams.trigger = 0;       // 触发变量初始值
    KEY4SwiHandle = Swi_create(KEY4Swi, &SwiParams, NULL);
    KEY6SwiHandle = Swi_create(KEY6Swi, &SwiParams, NULL);
}

/****************************************************************************/
/*                                                                          */
/*              硬件中断线程                                                */
/*                                                                          */
/****************************************************************************/
Void KEY4Hwi(UArg arg)
{
    if(GPIOPinIntStatus(SOC_GPIO_0_REGS, EVM_KEY4) == GPIO_INT_PEND)
    {
        // 发布软件中断
        Swi_post(KEY4SwiHandle);
    }

    // 清除 GPIO 中断状态
    GPIOPinIntClear(SOC_GPIO_0_REGS, EVM_KEY4);
}

Void KEY6Hwi(UArg arg)
{
    if(GPIOPinIntStatus(SOC_GPIO_0_REGS, EVM_KEY6) == GPIO_INT_PEND)
    {
        // 发布软件中断
        Swi_post(KEY6SwiHandle);
    }

    // 清除 GPIO 中断状态
    GPIOPinIntClear(SOC_GPIO_0_REGS, EVM_KEY6);
}

static Void HwiInit()
{
    /* 映射到组合事件中断 */
    // EVT0  1 - 31
    // EVT1 32 - 63
    // EVT2 64 - 95
    // EVT3 96 - 127

    // 按键 SW4
    EventCombiner_dispatchPlug(SYS_INT_GPIO_B8INT, &KEY4Hwi, 0, TRUE);

    // 按键 SW6
    EventCombiner_dispatchPlug(SYS_INT_GPIO_B0INT, &KEY6Hwi, 0, TRUE);
}

/****************************************************************************/
/*                                                                          */
/*              初始化                                                      */
/*                                                                          */
/****************************************************************************/
void KEYInit()
{
    // 使能外设
    PSCModuleControl(SOC_PSC_1_REGS, HW_PSC_GPIO, PSC_POWERDOMAIN_ALWAYS_ON, PSC_MDCTL_NEXT_ENABLE);

    // 管脚复用配置
    GPIOBankPinMuxSet();

    // GPIO 管脚初始化
    GPIOBankPinInit();

    // 软件中断线程初始化
    SwiInit();

    // 硬件中断线程初始化
    HwiInit();
}
