/****************************************************************************/
/*                                                                          */
/*    新核科技(广州)有限公司                                                */
/*                                                                          */
/*    Copyright (C) 2022 CoreKernel Technology (Guangzhou) Co., Ltd         */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/*    EDMA3                                                                 */
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

#include "edma.h"

/****************************************************************************/
/*                                                                          */
/*              EDMA3 中断完成服务函数                                      */
/*                                                                          */
/****************************************************************************/
// 回调函数
void (*cb_Fxn[EDMA3_NUM_TCC])(unsigned int tcc, unsigned int status, void *appData);

Void Edma3CompletionHwi(UArg arg)
{
    volatile unsigned int pendingIrqs;
    volatile unsigned int isIPR = 0;

    unsigned int indexl;
    unsigned int Cnt = 0;
    indexl = 1u;

    isIPR = EDMA3GetIntrStatus(SOC_EDMA30CC_0_REGS);
    if(isIPR)
    {
        while ((Cnt < EDMA3CC_COMPL_HANDLER_RETRY_COUNT)&& (indexl != 0u))
        {
            indexl = 0u;
            pendingIrqs = EDMA3GetIntrStatus(SOC_EDMA30CC_0_REGS);
            while (pendingIrqs)
            {
                if(TRUE == (pendingIrqs & 1u))
                {
                    // 如果没有指定回调函数 就无法清除 IPR 相应位
                    // 写 ICR 清除 IPR 相应位
                    EDMA3ClrIntr(SOC_EDMA30CC_0_REGS, indexl);
                    (*cb_Fxn[indexl])(indexl, EDMA3_XFER_COMPLETE, NULL);
                }

                ++indexl;
                pendingIrqs >>= 1u;
            }

            Cnt++;
        }
    }
}

/****************************************************************************/
/*                                                                          */
/*              EDMA3 中断错误服务函数                                      */
/*                                                                          */
/****************************************************************************/
Void Edma3CCErrorHwi(UArg arg)
{
    volatile unsigned int pendingIrqs;
    unsigned int Cnt = 0u;
    unsigned int index;
    unsigned int evtqueNum = 0;  // 事件队列数目

    pendingIrqs = 0u;
    index = 1u;

    if((EDMA3GetErrIntrStatus(SOC_EDMA30CC_0_REGS) != 0 )
        || (EDMA3QdmaGetErrIntrStatus(SOC_EDMA30CC_0_REGS) != 0)
        || (EDMA3GetCCErrStatus(SOC_EDMA30CC_0_REGS) != 0))
    {
        // 循环 EDMA3CC_ERR_HANDLER_RETRY_COUNT 次
        // 直到没有等待中的中断时终止
        while ((Cnt < EDMA3CC_ERR_HANDLER_RETRY_COUNT) && (index != 0u))
        {
            index = 0u;
            pendingIrqs = EDMA3GetErrIntrStatus(SOC_EDMA30CC_0_REGS);

            while (pendingIrqs)
            {
                // 执行所有等待中的中断
                if(TRUE == (pendingIrqs & 1u))
                {
                    // 清除 SER
                    EDMA3ClrMissEvt(SOC_EDMA30CC_0_REGS, index);
                }
                    ++index;
                    pendingIrqs >>= 1u;
            }

            index = 0u;
            pendingIrqs = EDMA3QdmaGetErrIntrStatus(SOC_EDMA30CC_0_REGS);

            while (pendingIrqs)
            {
                // 执行所有等待中的中断
                if(TRUE == (pendingIrqs & 1u))
                {
                    // 清除 SER
                    EDMA3QdmaClrMissEvt(SOC_EDMA30CC_0_REGS, index);
                }
                ++index;
                pendingIrqs >>= 1u;
            }

            index = 0u;
            pendingIrqs = EDMA3GetCCErrStatus(SOC_EDMA30CC_0_REGS);

            if (pendingIrqs != 0u)
            {
                // 执行所有等待中的 CC 错误中断
                // 事件队列 队列入口错误
                for (evtqueNum = 0u; evtqueNum < SOC_EDMA3_NUM_EVQUE; evtqueNum++)
                {
                    if((pendingIrqs & (1u << evtqueNum)) != 0u)
                    {
                        // 清除错误中断
                        EDMA3ClrCCErr(SOC_EDMA30CC_0_REGS, (1u << evtqueNum));
                    }
                }

                // 传输完成错误
                if ((pendingIrqs & (1 << EDMA3CC_CCERR_TCCERR_SHIFT)) != 0u)
                {
                    EDMA3ClrCCErr(SOC_EDMA30CC_0_REGS, (0x01u << EDMA3CC_CCERR_TCCERR_SHIFT));
                }

                ++index;
            }

            Cnt++;
        }
    }
}

/****************************************************************************/
/*                                                                          */
/*              硬件中断线程                                                */
/*                                                                          */
/****************************************************************************/
static Void HwiInit()
{
    /* 映射到组合事件中断 */
    // EVT0  1 - 31
    // EVT1 32 - 63
    // EVT2 64 - 95
    // EVT3 96 - 127

    EventCombiner_dispatchPlug(SYS_INT_EDMA3_0_CC0_INT1, &Edma3CompletionHwi, 1, TRUE);
    EventCombiner_dispatchPlug(SYS_INT_EDMA3_0_CC0_ERRINT, &Edma3CCErrorHwi, 1, TRUE);

    // 注册 McASP Audio 接收和发送完成中断回掉函数
//    cb_Fxn[0] = &McASPRxDMAComplCallback;
//    cb_Fxn[1] = &McASPTxDMAComplCallback;
}

/****************************************************************************/
/*                                                                          */
/*              版本匹配                                                    */
/*                                                                          */
/****************************************************************************/
unsigned int EDMAVersionGet()
{
    return 1;
}

/****************************************************************************/
/*                                                                          */
/*              初始化                                                      */
/*                                                                          */
/****************************************************************************/
void EDMAInit()
{
    // 使能外设
    PSCModuleControl(SOC_PSC_0_REGS, HW_PSC_CC0, PSC_POWERDOMAIN_ALWAYS_ON, PSC_MDCTL_NEXT_ENABLE);
    PSCModuleControl(SOC_PSC_0_REGS, HW_PSC_TC0, PSC_POWERDOMAIN_ALWAYS_ON, PSC_MDCTL_NEXT_ENABLE);

    // EDMA3 初始化 */
    EDMA3Init(SOC_EDMA30CC_0_REGS, 0);

    // 硬件中断线程初始化
    HwiInit();
}
