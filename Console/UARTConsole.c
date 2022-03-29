/****************************************************************************/
/*                                                                          */
/*    �º˿Ƽ�(����)���޹�˾                                                */
/*                                                                          */
/*    Copyright (C) 2022 CoreKernel Technology (Guangzhou) Co., Ltd         */
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

#include "cmdline.h"

/****************************************************************************/
/*                                                                          */
/*              �궨��                                                      */
/*                                                                          */
/****************************************************************************/
#define UART_FREQ       pllcfg.PLL0_SYSCLK2 * 1000000

// �����ֳ�������
#define CmdSize 64

/****************************************************************************/
/*                                                                          */
/*              ȫ�ֱ���                                                    */
/*                                                                          */
/****************************************************************************/
Semaphore_Handle UARTConsoleSemHandle;

Task_Handle UARTConsoleTaskHandle;

// �����ֻ���
char g_cCmdBuf[CmdSize];

// ������ʷ
char CmdHistory[32][CmdSize];

/****************************************************************************/
/*                                                                          */
/*              ��������                                                    */
/*                                                                          */
/****************************************************************************/
extern int ReadLine();

/****************************************************************************/
/*                                                                          */
/*              GPIO �ܽŸ�������                                           */
/*                                                                          */
/****************************************************************************/
static void GPIOBankPinMuxSet()
{
    // �����ƴ��� ��ʹ������
    #define PINMUX4_UART2_TXD_ENABLE    (SYSCFG_PINMUX4_PINMUX4_23_20_UART2_TXD << SYSCFG_PINMUX4_PINMUX4_23_20_SHIFT)
    #define PINMUX4_UART2_RXD_ENABLE    (SYSCFG_PINMUX4_PINMUX4_19_16_UART2_RXD << SYSCFG_PINMUX4_PINMUX4_19_16_SHIFT)

    unsigned int savePinmux = 0;

    savePinmux = (HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(4)) & ~(SYSCFG_PINMUX4_PINMUX4_23_20 | SYSCFG_PINMUX4_PINMUX4_19_16));
    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(4)) = ((PINMUX4_UART2_TXD_ENABLE | PINMUX4_UART2_RXD_ENABLE) | savePinmux);
}

/****************************************************************************/
/*                                                                          */
/*              UART ����                                                   */
/*                                                                          */
/****************************************************************************/
static void UARTConfig()
{
	// ���� UART ����
	// ������ 115200 ����λ 8 ֹͣλ 1 ��У��λ
    UARTConfigSetExpClk(UARTConsole, UART_FREQ, BAUD_115200, UART_WORDL_8BITS, UART_OVER_SAMP_RATE_16);

	// ʹ�� UART
	UARTEnable(UARTConsole);

    // ʹ�ܽ��� / ���� FIFO
    UARTFIFOEnable(UARTConsole);

    // ���� FIFO ����
    UARTFIFOLevelSet(UARTConsole, UART_RX_TRIG_LEVEL_1);
}

/****************************************************************************/
/*                                                                          */
/*              UART �жϳ�ʼ��                                             */
/*                                                                          */
/****************************************************************************/
static void UARTInterruptInit()
{
	// ʹ���ж�
    UARTIntEnable(UARTConsole, UART_INT_LINE_STAT  | UART_INT_RXDATA_CTI);
}

/****************************************************************************/
/*                                                                          */
/*              Ӳ���ж��߳�                                                */
/*                                                                          */
/****************************************************************************/
Void UARTConsoleHwi(UArg arg)
{
    // �����ж�
    if(UARTIntStatus(UARTConsole) == UART_INTID_RX_DATA)
    {
        // �����ź���
        Semaphore_post(UARTConsoleSemHandle);
    }
}

static Void HwiInit()
{
    /* ӳ�䵽����¼��ж� */
    // EVT0  1 - 31
    // EVT1 32 - 63
    // EVT2 64 - 95
    // EVT3 96 - 127

    EventCombiner_dispatchPlug(SYS_INT_UART2_INT, &UARTConsoleHwi, 0, TRUE);
}

/****************************************************************************/
/*                                                                          */
/*              �����߳�                                                    */
/*                                                                          */
/****************************************************************************/
static unsigned int UARTPuts(char *pTxBuffer, int numBytesToWrite)
{
    unsigned int count = 0;
    unsigned int flag = 0;

    if(numBytesToWrite < 0)
    {
      flag = 1;
    }

    while('\0' != *pTxBuffer)
    {
        /* ������� */
        if('\n' == *pTxBuffer)
        {
            UARTCharPut(UARTConsole, '\r');
            UARTCharPut(UARTConsole, '\n');
        }
        else
        {
            UARTCharPut(UARTConsole, (unsigned char)*pTxBuffer);
        }
        pTxBuffer++;
        count++;

        if((0 == flag) && (count == numBytesToWrite))
        {
           break;
        }

    }

    /* �����ַ��� */
    return count;
}

static char UARTWriteBuffer[128];
void UARTprintf(const char *fmt, ...)
{
    va_list arg_ptr;

    if(strlen(fmt) > 128)
    {
        return;
    }

    va_start(arg_ptr, fmt);
    vsprintf((char *)UARTWriteBuffer, fmt, arg_ptr);
    va_end(arg_ptr);

    UARTPuts(UARTWriteBuffer, -1);
}

unsigned char CmdHistorySaveNo = 0;
unsigned char CmdHistorySaveIndex = 0;
unsigned char CmdHistoryDisplayIndex = 0;

int ReadLine()
{
    unsigned int ulIdx;
    unsigned char ucChar;

    // ��������ʼ��ʼ��
    g_cCmdBuf[0] = '\0';
    ulIdx = 0;

    while(1)
    {
        // ���������ַ�
        while(UARTCharsAvail(UARTConsole))
        {
            // ����һ���ַ�
            ucChar = (unsigned char)UARTCharGet(UARTConsole);

            // �ж��ַ��Ƿ��ǿո�
            if((ucChar == '\b') && (ulIdx != 0))
            {
                // ɾ��һ���ַ�
                UARTprintf("\b \b");
                ulIdx--;
                g_cCmdBuf[ulIdx] = '\0';
            }
            // �жϻ��з�
            else if((ucChar == '\r') || (ucChar == '\n'))
            {
                UARTprintf("\n");

                if(ulIdx == 0)
                {
                    return 0;
                }
                else
                {
                    // ��������
                    strcpy(CmdHistory[CmdHistorySaveIndex], g_cCmdBuf);
                    if(CmdHistorySaveIndex >= 31)
                    {
                        CmdHistorySaveIndex = 0;
                    }
                    else
                    {
                        CmdHistorySaveIndex++;
                        if(CmdHistorySaveNo != 32)
                        {
                            CmdHistorySaveNo++;
                        }
                    }

                    return 1;
                }
            }
            // �ж��ַ��Ƿ��� ESC ���� Ctrl + U
            else if((ucChar == 0x1b) || (ucChar == 0x15))
            {
                // ɾ�������ַ�
                while(ulIdx)
                {
                    UARTprintf("\b \b");
                    ulIdx--;
                }
                g_cCmdBuf[0] = '\0';
            }
            // �ж��Ƿ���һ���ɴ�ӡ�ַ�
            else if((ucChar >= ' ') && (ucChar <= '~') &&
                    (ulIdx < (sizeof(g_cCmdBuf) - 1)))
            {
                if((ucChar == '['))
                {
                    // ����һ���ַ�
                    ucChar = (unsigned char)UARTCharGet(UARTConsole);

                    if(ucChar == 'A' || ucChar == 'B' || ucChar == 'C' || ucChar == 'D')
                    {
                        switch(ucChar)
                        {
                            case 'A': // ��
                                    if(CmdHistoryDisplayIndex > 0 && CmdHistoryDisplayIndex < CmdHistorySaveNo)
                                    {
                                        CmdHistoryDisplayIndex--;
                                    }
                                    break;

                            case 'B': // ��
                                    if(CmdHistoryDisplayIndex <= 31 && CmdHistoryDisplayIndex < CmdHistorySaveNo - 1)
                                    {
                                        CmdHistoryDisplayIndex++;
                                    }
                                    break;

                            case 'C': // ��
                                    break;

                            case 'D': // ��
                                    break;
                        }

                        // ɾ�������ַ�
                        while(ulIdx)
                        {
                            UARTprintf("\b \b");
                            ulIdx--;
                        }
                        g_cCmdBuf[0] = '\0';
                    }

                    strcpy(g_cCmdBuf, CmdHistory[CmdHistoryDisplayIndex]);
                    ulIdx = strlen(g_cCmdBuf);
                    UARTprintf("%s", g_cCmdBuf);
                }
                else
                {
                    // ����ַ������뻺��
                    g_cCmdBuf[ulIdx++] = ucChar;
                    g_cCmdBuf[ulIdx] = '\0';

                    UARTprintf("%c", ucChar);
                }
            }
        }
    }
}

Void UARTConsoleTask(UArg a0, UArg a1)
{
    for(;;)
    {
        // �����ź���
        Semaphore_pend(UARTConsoleSemHandle, BIOS_WAIT_FOREVER);

        // �����ж�
        UARTIntDisable(UARTConsole, UART_INT_RXDATA_CTI);

        // ����״̬
        int iStatus;

        // ���ն˶��ַ�
        iStatus = ReadLine();

        if(iStatus)
        {
            // ��������
            iStatus = CmdLineProcess(g_cCmdBuf);

            // δ֪����
            if(iStatus == CMDLINE_BAD_CMD)
            {
                UARTprintf("Unknown command %s!\n", g_cCmdBuf);
            }
            // ����̫��
            else if(iStatus == CMDLINE_TOO_MANY_ARGS)
            {
                UARTprintf("Too many arguments for command processor!\n");
            }
            // ���ش������
            else if(iStatus != 0)
            {
                UARTprintf("Command returned error code %d\n", iStatus);
            }
        }

        // �����ʾ��
        UARTprintf("CoreKernel > ");

        // ʹ���ж�
        UARTIntEnable(UARTConsole, UART_INT_RXDATA_CTI);
    }
}

static Void TaskInit()
{
    Task_Params taskParams;
    Task_Params_init(&taskParams);

    taskParams.priority   = 15;  // 4
    UARTConsoleTaskHandle = Task_create(UARTConsoleTask, &taskParams, NULL);
}

/****************************************************************************/
/*                                                                          */
/*              �ź���                                                      */
/*                                                                          */
/****************************************************************************/
static Void SemInit()
{
    Semaphore_Params params;
    Semaphore_Params_init(&params);

    params.mode = Semaphore_Mode_BINARY;  // ������ģʽ
    UARTConsoleSemHandle = Semaphore_create(0, &params, NULL);
}

/****************************************************************************/
/*                                                                          */
/*              �����ն˳�ʼ��                                              */
/*                                                                          */
/****************************************************************************/
Void UARTConsoleInit()
{
    // ʹ������
    PSCModuleControl(SOC_PSC_1_REGS, HW_PSC_UART2, PSC_POWERDOMAIN_ALWAYS_ON,PSC_MDCTL_NEXT_ENABLE);

	// �ܽŸ�������
    GPIOBankPinMuxSet();

	// ���ڲ�������
	UARTConfig();

	// �����ж�����
    UARTInterruptInit();

    // Ӳ���ж��̳߳�ʼ��
    HwiInit();

    // �ź�����ʼ��
    SemInit();

    // �����̳߳�ʼ��
    TaskInit();
}
