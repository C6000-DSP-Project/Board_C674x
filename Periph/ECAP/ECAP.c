/****************************************************************************/
/*                                                                          */
/*    新核科技(广州)有限公司                                                */
/*                                                                          */
/*    Copyright (C) 2022 CoreKernel Technology (Guangzhou) Co., Ltd         */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/*    ECAP 捕获                                                             */
/*                                                                          */
/*    2015年05月11日                                                        */
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

#include "ecap.h"

/****************************************************************************/
/*                                                                          */
/*              全局变量                                                    */
/*                                                                          */
/****************************************************************************/
Semaphore_Handle ECAPSemHandle;

unsigned int ECAPValue;

/****************************************************************************/
/*                                                                          */
/*              GPIO 管脚复用配置                                           */
/*                                                                          */
/****************************************************************************/
static void GPIOBankPinMuxSet()
{
	// 作为捕获功能时管脚方向为输入
	// 作为辅助脉宽调制时管脚方向为输出
    // ECAP1 / APWM1
    unsigned int savePinMux = 0;

    savePinMux = (HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(1)) & ~(SYSCFG_PINMUX1_PINMUX1_31_28));
    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(1)) = ((SYSCFG_PINMUX1_PINMUX1_31_28_ECAP1 << SYSCFG_PINMUX1_PINMUX1_31_28_SHIFT) | savePinMux);
}

/****************************************************************************/
/*                                                                          */
/*              ECAP 配置                                                   */
/*                                                                          */
/****************************************************************************/
static void ECAPSetup(unsigned int baseAddr)
{
    // 停止计数
    ECAPCounterControl(baseAddr, ECAP_COUNTER_STOP);

    // 设置捕获边缘为 0上升沿  1下降沿
    ECAPCapeEvtPolarityConfig(baseAddr, 0, 1, 0, 1);

    // 捕获时间重置计数器(DELTA MODE)
    ECAPCaptureEvtCntrRstConfig(baseAddr, 1, 1, 1, 1);

    // 使能捕获事件装载计数值
    ECAPCaptureLoadingEnable(baseAddr);

    // 预分频 可以写入值 x = 0~31，则分频系数为2x，写入0是1分频
    ECAPPrescaleConfig(baseAddr, 0);

    // 禁用同步输入输出信号
    ECAPSyncInOutSelect(baseAddr, ECAP_SYNC_IN_DISABLE, ECAP_SYNC_OUT_DISABLE);

    // 设置操作模式为捕获模式
    ECAPOperatingModeSelect(baseAddr, ECAP_CAPTURE_MODE);

    // 初始化 ECAP 为连续方式
    ECAPContinousModeConfig(baseAddr);

    // 设置计数器的值 100ms 溢出
    ECAPCounterConfig(baseAddr, 0xffffffff - 22800000);

    // 启动计数器
    ECAPCounterControl(baseAddr, ECAP_COUNTER_FREE_RUNNING);
}

/****************************************************************************/
/*                                                                          */
/*              ECAP 中断初始化                                             */
/*                                                                          */
/****************************************************************************/
static void ECAPInterruptInit(unsigned int baseAddr)
{
	// 使能中断源
	ECAPIntEnable(baseAddr, ECAP_CNTOVF_INT | ECAP_CEVT1_INT | ECAP_CEVT2_INT | ECAP_CEVT3_INT | ECAP_CEVT4_INT);
	ECAPIntStatusClear(baseAddr, ECAP_CNTOVF_INT | ECAP_CEVT1_INT | ECAP_CEVT2_INT | ECAP_CEVT3_INT | ECAP_CEVT4_INT);

	// 使能 ECAP 全局中断
	ECAPGlobalIntEnable(baseAddr);
}

/****************************************************************************/
/*                                                                          */
/*              硬件中断线程                                                */
/*                                                                          */
/****************************************************************************/
Void ECAPHwi(UArg arg)
{
    unsigned int intFlag;

    intFlag = ECAPIntStatus(SOC_ECAP_2_REGS,ECAP_CNTOVF_INT | ECAP_CEVT1_INT | ECAP_CEVT2_INT |
                                                ECAP_CEVT3_INT | ECAP_CEVT4_INT);

    ECAPIntStatusClear(SOC_ECAP_2_REGS, intFlag);

    if(intFlag & ECAP_CEVT1_INT)
    {
//      ECAPValue = ECAPTimeStampRead(SOC_ECAP_1_REGS, ECAP_CAPTURE_EVENT_1);
    }
    if(intFlag & ECAP_CEVT2_INT)
    {
        ECAPValue = ECAPTimeStampRead(SOC_ECAP_2_REGS, ECAP_CAPTURE_EVENT_2);
        ECAPValue = ECAPValue / 228;   //将值转换为微秒us
        // 发布信号量
        Semaphore_post(ECAPSemHandle);
    }
    if(intFlag & ECAP_CEVT3_INT)
    {
//      ECAPValue = ECAPTimeStampRead(SOC_ECAP_1_REGS, ECAP_CAPTURE_EVENT_3);
    }
    if(intFlag & ECAP_CEVT4_INT)
    {
        ECAPValue = ECAPTimeStampRead(SOC_ECAP_2_REGS, ECAP_CAPTURE_EVENT_4);
        ECAPValue = ECAPValue/228;   //将值转换为微秒us
        // 发布信号量
        Semaphore_post(ECAPSemHandle);
    }
    if(intFlag & ECAP_CNTOVF_INT) // 溢出中断
    {
        ECAPCounterConfig(SOC_ECAP_2_REGS, 0xffffffff - 22800000);
    }
    ECAPIntStatusClear(SOC_ECAP_2_REGS, ECAP_GLOBAL_INT);
}

static Void HwiInit()
{
    EventCombiner_dispatchPlug(SYS_INT_ECAP2, &ECAPHwi, 1, TRUE);
}

/****************************************************************************/
/*                                                                          */
/*              任务线程                                                    */
/*                                                                          */
/****************************************************************************/
Void ECAPTask(UArg a0, UArg a1)
{
    while(1)
    {
         Semaphore_pend(ECAPSemHandle, BIOS_WAIT_FOREVER);

        if(ECAPValue != 0)
        {
            UARTprintf("ECAPValue = %d us\n", ECAPValue);
            ECAPValue = 0;
        }
    }
}

static Void TaskInit()
{
    Task_Params taskParams;
    Task_Params_init(&taskParams);
    taskParams.priority = 4;
    Task_create(ECAPTask, &taskParams, NULL);
}

/****************************************************************************/
/*                                                                          */
/*              信号量                                                      */
/*                                                                          */
/****************************************************************************/
Void SemInit()
{
    Semaphore_Params params;
    Semaphore_Params_init(&params);
    params.mode = Semaphore_Mode_BINARY;
    ECAPSemHandle = Semaphore_create(0, &params, NULL);
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

    // 捕获配置
    ECAPSetup(SOC_ECAP_2_REGS);

    // ECAP中断初始化
    ECAPInterruptInit(SOC_ECAP_2_REGS);

    // 硬件中断线程初始化
    HwiInit();

    // 任务线程初始化
    TaskInit();

    // 信号量初始化
    SemInit();
}
