/****************************************************************************/
/*                                                                          */
/*    新核科技(广州)有限公司                                                */
/*                                                                          */
/*    Copyright (C) 2022 CoreKernel Technology (Guangzhou) Co., Ltd         */
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

#include "cmdline.h"

/****************************************************************************/
/*                                                                          */
/*              宏定义                                                      */
/*                                                                          */
/****************************************************************************/
#define UART_FREQ       pllcfg.PLL0_SYSCLK2 * 1000000

// 命令字长度限制
#define CmdSize 64

/****************************************************************************/
/*                                                                          */
/*              全局变量                                                    */
/*                                                                          */
/****************************************************************************/
Semaphore_Handle UARTConsoleSemHandle;

Task_Handle UARTConsoleTaskHandle;

// 命令字缓存
char g_cCmdBuf[CmdSize];

// 命令历史
char CmdHistory[32][CmdSize];

/****************************************************************************/
/*                                                                          */
/*              函数声明                                                    */
/*                                                                          */
/****************************************************************************/
extern int ReadLine();

/****************************************************************************/
/*                                                                          */
/*              GPIO 管脚复用配置                                           */
/*                                                                          */
/****************************************************************************/
static void GPIOBankPinMuxSet()
{
    // 两线制串口 不使用流控
    #define PINMUX4_UART2_TXD_ENABLE    (SYSCFG_PINMUX4_PINMUX4_23_20_UART2_TXD << SYSCFG_PINMUX4_PINMUX4_23_20_SHIFT)
    #define PINMUX4_UART2_RXD_ENABLE    (SYSCFG_PINMUX4_PINMUX4_19_16_UART2_RXD << SYSCFG_PINMUX4_PINMUX4_19_16_SHIFT)

    unsigned int savePinmux = 0;

    savePinmux = (HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(4)) & ~(SYSCFG_PINMUX4_PINMUX4_23_20 | SYSCFG_PINMUX4_PINMUX4_19_16));
    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(4)) = ((PINMUX4_UART2_TXD_ENABLE | PINMUX4_UART2_RXD_ENABLE) | savePinmux);
}

/****************************************************************************/
/*                                                                          */
/*              UART 配置                                                   */
/*                                                                          */
/****************************************************************************/
static void UARTConfig()
{
	// 配置 UART 参数
	// 波特率 115200 数据位 8 停止位 1 无校验位
    UARTConfigSetExpClk(UARTConsole, UART_FREQ, BAUD_115200, UART_WORDL_8BITS, UART_OVER_SAMP_RATE_16);

	// 使能 UART
	UARTEnable(UARTConsole);

    // 使能接收 / 发送 FIFO
    UARTFIFOEnable(UARTConsole);

    // 设置 FIFO 级别
    UARTFIFOLevelSet(UARTConsole, UART_RX_TRIG_LEVEL_1);
}

/****************************************************************************/
/*                                                                          */
/*              UART 中断初始化                                             */
/*                                                                          */
/****************************************************************************/
static void UARTInterruptInit()
{
	// 使能中断
    UARTIntEnable(UARTConsole, UART_INT_LINE_STAT  | UART_INT_RXDATA_CTI);
}

/****************************************************************************/
/*                                                                          */
/*              硬件中断线程                                                */
/*                                                                          */
/****************************************************************************/
Void UARTConsoleHwi(UArg arg)
{
    // 接收中断
    if(UARTIntStatus(UARTConsole) == UART_INTID_RX_DATA)
    {
        // 发布信号量
        Semaphore_post(UARTConsoleSemHandle);
    }
}

static Void HwiInit()
{
    /* 映射到组合事件中断 */
    // EVT0  1 - 31
    // EVT1 32 - 63
    // EVT2 64 - 95
    // EVT3 96 - 127

    EventCombiner_dispatchPlug(SYS_INT_UART2_INT, &UARTConsoleHwi, 0, TRUE);
}

/****************************************************************************/
/*                                                                          */
/*              任务线程                                                    */
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
        /* 检查新行 */
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

    /* 返回字符数 */
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

    // 从命令起始开始读
    g_cCmdBuf[0] = '\0';
    ulIdx = 0;

    while(1)
    {
        // 回显输入字符
        while(UARTCharsAvail(UARTConsole))
        {
            // 读下一个字符
            ucChar = (unsigned char)UARTCharGet(UARTConsole);

            // 判断字符是否是空格
            if((ucChar == '\b') && (ulIdx != 0))
            {
                // 删除一个字符
                UARTprintf("\b \b");
                ulIdx--;
                g_cCmdBuf[ulIdx] = '\0';
            }
            // 判断换行符
            else if((ucChar == '\r') || (ucChar == '\n'))
            {
                UARTprintf("\n");

                if(ulIdx == 0)
                {
                    return 0;
                }
                else
                {
                    // 保存命令
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
            // 判断字符是否是 ESC 或者 Ctrl + U
            else if((ucChar == 0x1b) || (ucChar == 0x15))
            {
                // 删除所有字符
                while(ulIdx)
                {
                    UARTprintf("\b \b");
                    ulIdx--;
                }
                g_cCmdBuf[0] = '\0';
            }
            // 判断是否是一个可打印字符
            else if((ucChar >= ' ') && (ucChar <= '~') &&
                    (ulIdx < (sizeof(g_cCmdBuf) - 1)))
            {
                if((ucChar == '['))
                {
                    // 读下一个字符
                    ucChar = (unsigned char)UARTCharGet(UARTConsole);

                    if(ucChar == 'A' || ucChar == 'B' || ucChar == 'C' || ucChar == 'D')
                    {
                        switch(ucChar)
                        {
                            case 'A': // 上
                                    if(CmdHistoryDisplayIndex > 0 && CmdHistoryDisplayIndex < CmdHistorySaveNo)
                                    {
                                        CmdHistoryDisplayIndex--;
                                    }
                                    break;

                            case 'B': // 下
                                    if(CmdHistoryDisplayIndex <= 31 && CmdHistoryDisplayIndex < CmdHistorySaveNo - 1)
                                    {
                                        CmdHistoryDisplayIndex++;
                                    }
                                    break;

                            case 'C': // 左
                                    break;

                            case 'D': // 右
                                    break;
                        }

                        // 删除所有字符
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
                    // 添加字符到输入缓存
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
        // 挂起信号量
        Semaphore_pend(UARTConsoleSemHandle, BIOS_WAIT_FOREVER);

        // 禁用中断
        UARTIntDisable(UARTConsole, UART_INT_RXDATA_CTI);

        // 命令状态
        int iStatus;

        // 从终端读字符
        iStatus = ReadLine();

        if(iStatus)
        {
            // 处理命令
            iStatus = CmdLineProcess(g_cCmdBuf);

            // 未知命令
            if(iStatus == CMDLINE_BAD_CMD)
            {
                UARTprintf("Unknown command %s!\n", g_cCmdBuf);
            }
            // 参数太多
            else if(iStatus == CMDLINE_TOO_MANY_ARGS)
            {
                UARTprintf("Too many arguments for command processor!\n");
            }
            // 返回错误代码
            else if(iStatus != 0)
            {
                UARTprintf("Command returned error code %d\n", iStatus);
            }
        }

        // 输出提示符
        UARTprintf("CoreKernel > ");

        // 使能中断
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
/*              信号量                                                      */
/*                                                                          */
/****************************************************************************/
static Void SemInit()
{
    Semaphore_Params params;
    Semaphore_Params_init(&params);

    params.mode = Semaphore_Mode_BINARY;  // 二进制模式
    UARTConsoleSemHandle = Semaphore_create(0, &params, NULL);
}

/****************************************************************************/
/*                                                                          */
/*              串口终端初始化                                              */
/*                                                                          */
/****************************************************************************/
Void UARTConsoleInit()
{
    // 使能外设
    PSCModuleControl(SOC_PSC_1_REGS, HW_PSC_UART2, PSC_POWERDOMAIN_ALWAYS_ON,PSC_MDCTL_NEXT_ENABLE);

	// 管脚复用配置
    GPIOBankPinMuxSet();

	// 串口参数配置
	UARTConfig();

	// 串口中断配置
    UARTInterruptInit();

    // 硬件中断线程初始化
    HwiInit();

    // 信号量初始化
    SemInit();

    // 任务线程初始化
    TaskInit();
}
