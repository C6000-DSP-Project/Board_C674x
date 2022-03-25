/****************************************************************************/
/*                                                                          */
/*    �º˿Ƽ�(����)���޹�˾                                                */
/*                                                                          */
/*    Copyright (C) 2022 CoreKernel Technology (Guangzhou) Co., Ltd         */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/*    UART1 �첽����                                                        */
/*                                                                          */
/*    2014��10��14��                                                        */
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
/*              �궨��                                                      */
/*                                                                          */
/****************************************************************************/
// ����ϵ�
#define SW_BREAKPOINT     asm(" SWBP 0 ");

// �������ܼ����Ĵ���
#define EMUCNT0           *(volatile unsigned int*)0x01C111F0
#define EMUCNT1           *(volatile unsigned int*)0x01C111F4

// ʱ��
#define SYSCLK_1_FREQ     (456000000)
#define SYSCLK_2_FREQ     (SYSCLK_1_FREQ/2)
#define UART_1_FREQ       (SYSCLK_2_FREQ)

/****************************************************************************/
/*                                                                          */
/*              ȫ�ֱ���                                                    */
/*                                                                          */
/****************************************************************************/
char txArray[] = "UART1 Message...\n\r";

/****************************************************************************/
/*                                                                          */
/*              GPIO �ܽŸ�������                                           */
/*                                                                          */
/****************************************************************************/
static void GPIOBankPinMuxSet()
{
	// �����ƴ��� ��ʹ������
    #define PINMUX4_UART1_TXD_ENABLE    (SYSCFG_PINMUX4_PINMUX4_31_28_UART1_TXD << SYSCFG_PINMUX4_PINMUX4_31_28_SHIFT)
    #define PINMUX4_UART1_RXD_ENABLE    (SYSCFG_PINMUX4_PINMUX4_27_24_UART1_RXD << SYSCFG_PINMUX4_PINMUX4_27_24_SHIFT)

    unsigned int savePinmux = 0;

    savePinmux = (HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(4)) & ~(SYSCFG_PINMUX4_PINMUX4_31_28 | SYSCFG_PINMUX4_PINMUX4_27_24));
    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(4)) = ((PINMUX4_UART1_TXD_ENABLE | PINMUX4_UART1_RXD_ENABLE) | savePinmux);
}

/****************************************************************************/
/*                                                                          */
/*              UART ����                                                   */
/*                                                                          */
/****************************************************************************/
static void UARTConfig()
{
    // ������ 115200 ����λ 8 ֹͣλ 1 ��У��λ
    UARTConfigSetExpClk(SOC_UART_1_REGS, UART_1_FREQ, BAUD_115200, UART_WORDL_8BITS, UART_OVER_SAMP_RATE_16);

    // ʹ�� UART
    UARTEnable(SOC_UART_1_REGS);

    // ʹ�ܽ��� / ���� FIFO
    UARTFIFOEnable(SOC_UART_1_REGS);

    // ���� FIFO ����
    UARTFIFOLevelSet(SOC_UART_1_REGS, UART_RX_TRIG_LEVEL_1);
}

/****************************************************************************/
/*                                                                          */
/*              UART �жϳ�ʼ��                                             */
/*                                                                          */
/****************************************************************************/
static void UARTInterruptInit()
{
	// ʹ���ж�
	unsigned int intFlags = 0;
    intFlags |= (UART_INT_LINE_STAT  |  \
                 UART_INT_TX_EMPTY |    \
                 UART_INT_RXDATA_CTI);
    UARTIntEnable(SOC_UART_1_REGS, intFlags);
}

/****************************************************************************/
/*                                                                          */
/*              Ӳ���ж��߳�                                                */
/*                                                                          */
/****************************************************************************/
Void UART1Hwi(UArg arg)
{
    static unsigned int length = sizeof(txArray);
    static unsigned int count = 0;
    unsigned char rxData = 0;
    unsigned int int_id = 0;

    // ȷ���ж�Դ
    int_id = UARTIntStatus(SOC_UART_1_REGS);

    // �����ж�
    if(UART_INTID_TX_EMPTY == int_id)
    {
        if(0 < length)
        {
            // дһ���ֽڵ� THR
            UARTCharPutNonBlocking(SOC_UART_1_REGS, txArray[count]);
            length--;
            count++;
        }
        if(0 == length)
        {
            // ���÷����ж�
            UARTIntDisable(SOC_UART_1_REGS, UART_INT_TX_EMPTY);
        }
     }

    // �����ж�
    if(UART_INTID_RX_DATA == int_id)
    {
        rxData = UARTCharGetNonBlocking(SOC_UART_1_REGS);
        UARTCharPutNonBlocking(SOC_UART_1_REGS, rxData);
    }

    // ���մ���
    if(UART_INTID_RX_LINE_STAT == int_id)
    {
        while(UARTRxErrorGet(SOC_UART_1_REGS))
        {
            // �� RBR ��һ���ֽ�
            UARTCharGetNonBlocking(SOC_UART_1_REGS);
        }
    }
}

static Void HwiInit()
{
    /* ӳ�䵽����¼��ж� */
    // EVT0  1 - 31
    // EVT1 32 - 63
    // EVT2 64 - 95
    // EVT3 96 - 127

    EventCombiner_dispatchPlug(SYS_INT_UART1_INT, &UART1Hwi, 0, TRUE);
}

/****************************************************************************/
/*                                                                          */
/*              ��ʼ��                                                      */
/*                                                                          */
/****************************************************************************/
void UART1Init()
{
    // ����ʹ��
    PSCModuleControl(SOC_PSC_1_REGS, HW_PSC_UART1, PSC_POWERDOMAIN_ALWAYS_ON,PSC_MDCTL_NEXT_ENABLE);

    // GPIO �ܽŸ�������
    GPIOBankPinMuxSet();

    // ���ڲ�������
    UARTConfig();

    // �����ж�����
    UARTInterruptInit();

    // Ӳ���ж��̳߳�ʼ��
    HwiInit();
}
