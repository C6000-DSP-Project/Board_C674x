/****************************************************************************/
/*                                                                          */
/*    �º˿Ƽ�(����)���޹�˾                                                */
/*                                                                          */
/*    Copyright (C) 2022 CoreKernel Technology (Guangzhou) Co., Ltd         */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/*    ����                                                                  */
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

/****************************************************************************/
/*                                                                          */
/*              GPIO �ܽŸ�������                                           */
/*                                                                          */
/****************************************************************************/
static void GPIOBankPinMuxSet()
{
	// ������Ӧ�� GPIO �ڹ���Ϊ��ͨ���������
    unsigned int savePinMux = 0;

    // EVM-CKL138PKT �װ�
    // GPIO0[8]
    savePinMux = (HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(0)) & ~(SYSCFG_PINMUX0_PINMUX0_31_28));
    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(0)) = ((SYSCFG_PINMUX0_PINMUX0_31_28_GPIO0_8 << SYSCFG_PINMUX0_PINMUX0_31_28_SHIFT) | savePinMux);

    // GPIO8[12]
    savePinMux = (HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(18)) & ~(SYSCFG_PINMUX18_PINMUX18_23_20));
    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(18)) = ((SYSCFG_PINMUX18_PINMUX18_23_20_GPIO8_12  << SYSCFG_PINMUX18_PINMUX18_23_20_SHIFT) | savePinMux);
}

/****************************************************************************/
/*                                                                          */
/*              GPIO �ܽų�ʼ��                                             */
/*                                                                          */
/****************************************************************************/
static void GPIOBankPinInit()
{
    // EVM-CKL138PKT �װ�
	// ���� LED ��Ӧ�ܽ�Ϊ����ܽ�
    GPIODirModeSet(SOC_GPIO_0_REGS, EVM_KEY6, GPIO_DIR_INPUT);          // GPIO0[08] SW6
    GPIODirModeSet(SOC_GPIO_0_REGS, EVM_KEY4, GPIO_DIR_INPUT);          // GPIO8[12] SW4

    // �����жϴ�����ʽ
    GPIOIntTypeSet(SOC_GPIO_0_REGS, EVM_KEY6, GPIO_INT_TYPE_FALLEDGE);  // �½���
    GPIOIntTypeSet(SOC_GPIO_0_REGS, EVM_KEY4, GPIO_INT_TYPE_FALLEDGE);  // �����ؼ��½���

    // ʹ�� GPIO BANK �ж�
    GPIOBankIntEnable(SOC_GPIO_0_REGS, 0);                              // GPIO BANK0
    GPIOBankIntEnable(SOC_GPIO_0_REGS, 8);                              // GPIO BANK8
}

/****************************************************************************/
/*                                                                          */
/*              ����ж��߳�                                                */
/*                                                                          */
/****************************************************************************/
// ָ����ʱ���Ǿ�ȷ��
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

    SwiParams.priority = 2;      // ���ȼ�
    SwiParams.trigger = 0;       // ����������ʼֵ
    KEY4SwiHandle = Swi_create(KEY4Swi, &SwiParams, NULL);
    KEY6SwiHandle = Swi_create(KEY6Swi, &SwiParams, NULL);
}

/****************************************************************************/
/*                                                                          */
/*              Ӳ���ж��߳�                                                */
/*                                                                          */
/****************************************************************************/
Void KEY4Hwi(UArg arg)
{
    if(GPIOPinIntStatus(SOC_GPIO_0_REGS, EVM_KEY4) == GPIO_INT_PEND)
    {
        // ��������ж�
        Swi_post(KEY4SwiHandle);
    }

    // ��� GPIO �ж�״̬
    GPIOPinIntClear(SOC_GPIO_0_REGS, EVM_KEY4);
}

Void KEY6Hwi(UArg arg)
{
    if(GPIOPinIntStatus(SOC_GPIO_0_REGS, EVM_KEY6) == GPIO_INT_PEND)
    {
        // ��������ж�
        Swi_post(KEY6SwiHandle);
    }

    // ��� GPIO �ж�״̬
    GPIOPinIntClear(SOC_GPIO_0_REGS, EVM_KEY6);
}

static Void HwiInit()
{
    /* ӳ�䵽����¼��ж� */
    // EVT0  1 - 31
    // EVT1 32 - 63
    // EVT2 64 - 95
    // EVT3 96 - 127

    // ���� SW4
    EventCombiner_dispatchPlug(SYS_INT_GPIO_B8INT, &KEY4Hwi, 0, TRUE);

    // ���� SW6
    EventCombiner_dispatchPlug(SYS_INT_GPIO_B0INT, &KEY6Hwi, 0, TRUE);
}

/****************************************************************************/
/*                                                                          */
/*              ��ʼ��                                                      */
/*                                                                          */
/****************************************************************************/
void KEYInit()
{
    // ʹ������
    PSCModuleControl(SOC_PSC_1_REGS, HW_PSC_GPIO, PSC_POWERDOMAIN_ALWAYS_ON, PSC_MDCTL_NEXT_ENABLE);

    // �ܽŸ�������
    GPIOBankPinMuxSet();

    // GPIO �ܽų�ʼ��
    GPIOBankPinInit();

    // ����ж��̳߳�ʼ��
    SwiInit();

    // Ӳ���ж��̳߳�ʼ��
    HwiInit();
}
