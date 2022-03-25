/****************************************************************************/
/*                                                                          */
/*    �º˿Ƽ�(����)���޹�˾                                                */
/*                                                                          */
/*    Copyright (C) 2022 CoreKernel Technology (Guangzhou) Co., Ltd         */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/*    LED                                                                   */
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

    // ���İ�
    // GPIO6[12]
    savePinMux = (HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(13)) & ~(SYSCFG_PINMUX13_PINMUX13_15_12));
    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(13)) = ((SYSCFG_PINMUX13_PINMUX13_15_12_GPIO6_12 << SYSCFG_PINMUX13_PINMUX13_15_12_SHIFT) | savePinMux);

    // GPIO6[13]
    savePinMux = (HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(13)) & ~(SYSCFG_PINMUX13_PINMUX13_11_8));
    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(13)) = ((SYSCFG_PINMUX13_PINMUX13_11_8_GPIO6_13  << SYSCFG_PINMUX13_PINMUX13_11_8_SHIFT) | savePinMux);

	// �װ�
    // GPIO2[15]
    savePinMux = (HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(5)) & ~(SYSCFG_PINMUX5_PINMUX5_3_0));
    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(5)) = ((SYSCFG_PINMUX5_PINMUX5_3_0_GPIO2_15 << SYSCFG_PINMUX5_PINMUX5_3_0_SHIFT) | savePinMux);

    // GPIO4[00]
    savePinMux = (HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(10)) & ~(SYSCFG_PINMUX10_PINMUX10_31_28));
    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(10)) = ((SYSCFG_PINMUX10_PINMUX10_31_28_GPIO4_0 << SYSCFG_PINMUX10_PINMUX10_31_28_SHIFT) | savePinMux);
}

/****************************************************************************/
/*                                                                          */
/*              GPIO �ܽų�ʼ��                                             */
/*                                                                          */
/****************************************************************************/
static void GPIOBankPinInit()
{
	// ���� LED ��Ӧ�ܽ�Ϊ����ܽ�
    // OMAP-L138 �� TMS320C6748 ���� 144 �� GPIO
	// ����Ϊ���� GPIO BANK ��ʼ�ܽŶ�Ӧֵ
    // ��Χ 1-144
	// GPIO0[0] 1
    // GPIO1[0] 17
	// GPIO2[0] 33
    // GPIO3[0] 49
	// GPIO4[0] 65
    // GPIO5[0] 81
	// GPIO6[0] 97
	// GPIO7[0] 113
	// GPIO8[0] 129

	// ���İ�
    GPIODirModeSet(SOC_GPIO_0_REGS, SOM_LED3, GPIO_DIR_OUTPUT);  // GPIO6[12] LED3
    GPIODirModeSet(SOC_GPIO_0_REGS, SOM_LED2, GPIO_DIR_OUTPUT);  // GPIO6[13] LED2

    // �װ�
    GPIODirModeSet(SOC_GPIO_0_REGS, EVM_LED4, GPIO_DIR_OUTPUT);  // GPIO2[15] LED4
    GPIODirModeSet(SOC_GPIO_0_REGS, EVM_LED3, GPIO_DIR_OUTPUT);  // GPIO4[00] LED3
}

/****************************************************************************/
/*                                                                          */
/*              LED ����                                                    */
/*                                                                          */
/****************************************************************************/
void LEDControl(unsigned char LED, char Status)
{
	switch(LED)
	{
        case SOM_LED2 : Status ? GPIOPinWrite(SOC_GPIO_0_REGS, SOM_LED2, GPIO_PIN_HIGH) : \
                                  GPIOPinWrite(SOC_GPIO_0_REGS, SOM_LED2, GPIO_PIN_LOW); break;   // GPIO6[13] LED2

		case SOM_LED3 : Status ? GPIOPinWrite(SOC_GPIO_0_REGS, SOM_LED3, GPIO_PIN_HIGH) : \
						           GPIOPinWrite(SOC_GPIO_0_REGS, SOM_LED3, GPIO_PIN_LOW); break;  // GPIO6[12] LED3

		case EVM_LED3  : Status ? GPIOPinWrite(SOC_GPIO_0_REGS, EVM_LED3, GPIO_PIN_HIGH) : \
				                   GPIOPinWrite(SOC_GPIO_0_REGS, EVM_LED3, GPIO_PIN_LOW); break;  // GPIO4[00] LED3

		case EVM_LED4  : Status ? GPIOPinWrite(SOC_GPIO_0_REGS, EVM_LED4, GPIO_PIN_HIGH) : \
				                  GPIOPinWrite(SOC_GPIO_0_REGS, EVM_LED4, GPIO_PIN_LOW); break;   // GPIO2[15] LED4
	}
}

/****************************************************************************/
/*                                                                          */
/*              �����߳�                                                    */
/*                                                                          */
/****************************************************************************/
unsigned int SoftPWM = 20;

Void LEDTask(UArg a0, UArg a1)
{
    unsigned int p, q;

    for(;;)
    {
        for(p = 0; p < SoftPWM; p++)
        {
            q = SoftPWM - p;

            LEDControl(SOM_LED2, GPIO_PIN_LOW);
            Task_sleep(p);

            LEDControl(SOM_LED2, GPIO_PIN_HIGH);
            Task_sleep(q);
        }

        for(p = SoftPWM; p > 0; p--)
        {
            q = SoftPWM - p;

            LEDControl(SOM_LED2, GPIO_PIN_LOW);
            Task_sleep(p);

            LEDControl(SOM_LED2, GPIO_PIN_HIGH);
            Task_sleep(q);
        }
    }
}

static Void TaskInit()
{
    Task_Params taskParams;
    Task_Params_init(&taskParams);
    taskParams.priority = 1;
    Task_create(LEDTask, &taskParams, NULL);
}

/****************************************************************************/
/*                                                                          */
/*              ��ʼ��                                                      */
/*                                                                          */
/****************************************************************************/
void LEDInit()
{
	// ʹ������
    PSCModuleControl(SOC_PSC_1_REGS, HW_PSC_GPIO, PSC_POWERDOMAIN_ALWAYS_ON, PSC_MDCTL_NEXT_ENABLE);

    // �ܽŸ�������
    GPIOBankPinMuxSet();

    // GPIO �ܽų�ʼ��
    GPIOBankPinInit();

    // �����̳߳�ʼ��
    TaskInit();
}
