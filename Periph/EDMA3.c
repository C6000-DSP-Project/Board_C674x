/****************************************************************************/
/*                                                                          */
/*    �º˿Ƽ�(����)���޹�˾                                                */
/*                                                                          */
/*    Copyright (C) 2022 CoreKernel Technology (Guangzhou) Co., Ltd         */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/*    EDMA3                                                                 */
/*                                                                          */
/*    2014��07��03��                                                        */
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

#include "edma.h"

/****************************************************************************/
/*                                                                          */
/*              EDMA3 �ж���ɷ�����                                      */
/*                                                                          */
/****************************************************************************/
// �ص�����
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
                    // ���û��ָ���ص����� ���޷���� IPR ��Ӧλ
                    // д ICR ��� IPR ��Ӧλ
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
/*              EDMA3 �жϴ��������                                      */
/*                                                                          */
/****************************************************************************/
Void Edma3CCErrorHwi(UArg arg)
{
    volatile unsigned int pendingIrqs;
    unsigned int Cnt = 0u;
    unsigned int index;
    unsigned int evtqueNum = 0;  // �¼�������Ŀ

    pendingIrqs = 0u;
    index = 1u;

    if((EDMA3GetErrIntrStatus(SOC_EDMA30CC_0_REGS) != 0 )
        || (EDMA3QdmaGetErrIntrStatus(SOC_EDMA30CC_0_REGS) != 0)
        || (EDMA3GetCCErrStatus(SOC_EDMA30CC_0_REGS) != 0))
    {
        // ѭ�� EDMA3CC_ERR_HANDLER_RETRY_COUNT ��
        // ֱ��û�еȴ��е��ж�ʱ��ֹ
        while ((Cnt < EDMA3CC_ERR_HANDLER_RETRY_COUNT) && (index != 0u))
        {
            index = 0u;
            pendingIrqs = EDMA3GetErrIntrStatus(SOC_EDMA30CC_0_REGS);

            while (pendingIrqs)
            {
                // ִ�����еȴ��е��ж�
                if(TRUE == (pendingIrqs & 1u))
                {
                    // ��� SER
                    EDMA3ClrMissEvt(SOC_EDMA30CC_0_REGS, index);
                }
                    ++index;
                    pendingIrqs >>= 1u;
            }

            index = 0u;
            pendingIrqs = EDMA3QdmaGetErrIntrStatus(SOC_EDMA30CC_0_REGS);

            while (pendingIrqs)
            {
                // ִ�����еȴ��е��ж�
                if(TRUE == (pendingIrqs & 1u))
                {
                    // ��� SER
                    EDMA3QdmaClrMissEvt(SOC_EDMA30CC_0_REGS, index);
                }
                ++index;
                pendingIrqs >>= 1u;
            }

            index = 0u;
            pendingIrqs = EDMA3GetCCErrStatus(SOC_EDMA30CC_0_REGS);

            if (pendingIrqs != 0u)
            {
                // ִ�����еȴ��е� CC �����ж�
                // �¼����� ������ڴ���
                for (evtqueNum = 0u; evtqueNum < SOC_EDMA3_NUM_EVQUE; evtqueNum++)
                {
                    if((pendingIrqs & (1u << evtqueNum)) != 0u)
                    {
                        // ��������ж�
                        EDMA3ClrCCErr(SOC_EDMA30CC_0_REGS, (1u << evtqueNum));
                    }
                }

                // ������ɴ���
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
/*              Ӳ���ж��߳�                                                */
/*                                                                          */
/****************************************************************************/
static Void HwiInit()
{
    /* ӳ�䵽����¼��ж� */
    // EVT0  1 - 31
    // EVT1 32 - 63
    // EVT2 64 - 95
    // EVT3 96 - 127

    EventCombiner_dispatchPlug(SYS_INT_EDMA3_0_CC0_INT1, &Edma3CompletionHwi, 1, TRUE);
    EventCombiner_dispatchPlug(SYS_INT_EDMA3_0_CC0_ERRINT, &Edma3CCErrorHwi, 1, TRUE);

    // ע�� McASP Audio ���պͷ�������жϻص�����
//    cb_Fxn[0] = &McASPRxDMAComplCallback;
//    cb_Fxn[1] = &McASPTxDMAComplCallback;
}

/****************************************************************************/
/*                                                                          */
/*              �汾ƥ��                                                    */
/*                                                                          */
/****************************************************************************/
unsigned int EDMAVersionGet()
{
    return 1;
}

/****************************************************************************/
/*                                                                          */
/*              ��ʼ��                                                      */
/*                                                                          */
/****************************************************************************/
void EDMAInit()
{
    // ʹ������
    PSCModuleControl(SOC_PSC_0_REGS, HW_PSC_CC0, PSC_POWERDOMAIN_ALWAYS_ON, PSC_MDCTL_NEXT_ENABLE);
    PSCModuleControl(SOC_PSC_0_REGS, HW_PSC_TC0, PSC_POWERDOMAIN_ALWAYS_ON, PSC_MDCTL_NEXT_ENABLE);

    // EDMA3 ��ʼ�� */
    EDMA3Init(SOC_EDMA30CC_0_REGS, 0);

    // Ӳ���ж��̳߳�ʼ��
    HwiInit();
}
