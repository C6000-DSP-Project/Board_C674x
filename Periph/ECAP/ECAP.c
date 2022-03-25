/****************************************************************************/
/*                                                                          */
/*    �º˿Ƽ�(����)���޹�˾                                                */
/*                                                                          */
/*    Copyright (C) 2022 CoreKernel Technology (Guangzhou) Co., Ltd         */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/*    ECAP ����                                                             */
/*                                                                          */
/*    2015��05��11��                                                        */
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

#include "ecap.h"

/****************************************************************************/
/*                                                                          */
/*              ȫ�ֱ���                                                    */
/*                                                                          */
/****************************************************************************/
Semaphore_Handle ECAPSemHandle;

unsigned int ECAPValue;

/****************************************************************************/
/*                                                                          */
/*              GPIO �ܽŸ�������                                           */
/*                                                                          */
/****************************************************************************/
static void GPIOBankPinMuxSet()
{
	// ��Ϊ������ʱ�ܽŷ���Ϊ����
	// ��Ϊ�����������ʱ�ܽŷ���Ϊ���
    // ECAP1 / APWM1
    unsigned int savePinMux = 0;

    savePinMux = (HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(1)) & ~(SYSCFG_PINMUX1_PINMUX1_31_28));
    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(1)) = ((SYSCFG_PINMUX1_PINMUX1_31_28_ECAP1 << SYSCFG_PINMUX1_PINMUX1_31_28_SHIFT) | savePinMux);
}

/****************************************************************************/
/*                                                                          */
/*              ECAP ����                                                   */
/*                                                                          */
/****************************************************************************/
static void ECAPSetup(unsigned int baseAddr)
{
    // ֹͣ����
    ECAPCounterControl(baseAddr, ECAP_COUNTER_STOP);

    // ���ò����ԵΪ 0������  1�½���
    ECAPCapeEvtPolarityConfig(baseAddr, 0, 1, 0, 1);

    // ����ʱ�����ü�����(DELTA MODE)
    ECAPCaptureEvtCntrRstConfig(baseAddr, 1, 1, 1, 1);

    // ʹ�ܲ����¼�װ�ؼ���ֵ
    ECAPCaptureLoadingEnable(baseAddr);

    // Ԥ��Ƶ ����д��ֵ x = 0~31�����Ƶϵ��Ϊ2x��д��0��1��Ƶ
    ECAPPrescaleConfig(baseAddr, 0);

    // ����ͬ����������ź�
    ECAPSyncInOutSelect(baseAddr, ECAP_SYNC_IN_DISABLE, ECAP_SYNC_OUT_DISABLE);

    // ���ò���ģʽΪ����ģʽ
    ECAPOperatingModeSelect(baseAddr, ECAP_CAPTURE_MODE);

    // ��ʼ�� ECAP Ϊ������ʽ
    ECAPContinousModeConfig(baseAddr);

    // ���ü�������ֵ 100ms ���
    ECAPCounterConfig(baseAddr, 0xffffffff - 22800000);

    // ����������
    ECAPCounterControl(baseAddr, ECAP_COUNTER_FREE_RUNNING);
}

/****************************************************************************/
/*                                                                          */
/*              ECAP �жϳ�ʼ��                                             */
/*                                                                          */
/****************************************************************************/
static void ECAPInterruptInit(unsigned int baseAddr)
{
	// ʹ���ж�Դ
	ECAPIntEnable(baseAddr, ECAP_CNTOVF_INT | ECAP_CEVT1_INT | ECAP_CEVT2_INT | ECAP_CEVT3_INT | ECAP_CEVT4_INT);
	ECAPIntStatusClear(baseAddr, ECAP_CNTOVF_INT | ECAP_CEVT1_INT | ECAP_CEVT2_INT | ECAP_CEVT3_INT | ECAP_CEVT4_INT);

	// ʹ�� ECAP ȫ���ж�
	ECAPGlobalIntEnable(baseAddr);
}

/****************************************************************************/
/*                                                                          */
/*              Ӳ���ж��߳�                                                */
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
        ECAPValue = ECAPValue / 228;   //��ֵת��Ϊ΢��us
        // �����ź���
        Semaphore_post(ECAPSemHandle);
    }
    if(intFlag & ECAP_CEVT3_INT)
    {
//      ECAPValue = ECAPTimeStampRead(SOC_ECAP_1_REGS, ECAP_CAPTURE_EVENT_3);
    }
    if(intFlag & ECAP_CEVT4_INT)
    {
        ECAPValue = ECAPTimeStampRead(SOC_ECAP_2_REGS, ECAP_CAPTURE_EVENT_4);
        ECAPValue = ECAPValue/228;   //��ֵת��Ϊ΢��us
        // �����ź���
        Semaphore_post(ECAPSemHandle);
    }
    if(intFlag & ECAP_CNTOVF_INT) // ����ж�
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
/*              �����߳�                                                    */
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
/*              �ź���                                                      */
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
/*              ECAP ��ʼ��                                                 */
/*                                                                          */
/****************************************************************************/
void ECAPInit()
{
    // ʹ������
    PSCModuleControl(SOC_PSC_1_REGS, HW_PSC_ECAP0_1_2, PSC_POWERDOMAIN_ALWAYS_ON, PSC_MDCTL_NEXT_ENABLE);

    // GPIO �ܽŸ�������
    GPIOBankPinMuxSet();

    // ��������
    ECAPSetup(SOC_ECAP_2_REGS);

    // ECAP�жϳ�ʼ��
    ECAPInterruptInit(SOC_ECAP_2_REGS);

    // Ӳ���ж��̳߳�ʼ��
    HwiInit();

    // �����̳߳�ʼ��
    TaskInit();

    // �ź�����ʼ��
    SemInit();
}
