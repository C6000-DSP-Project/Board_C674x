/****************************************************************************/
/*                                                                          */
/*    新核科技(广州)有限公司                                                */
/*                                                                          */
/*    Copyright (C) 2022 CoreKernel Technology (Guangzhou) Co., Ltd         */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/*    UART1 异步串口                                                        */
/*                                                                          */
/*    2014年10月14日                                                        */
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
/*              宏定义                                                      */
/*                                                                          */
/****************************************************************************/
// 软件断点
#define SW_BREAKPOINT     asm(" SWBP 0 ");

// 仿真性能计数寄存器
#define EMUCNT0           *(volatile unsigned int*)0x01C111F0
#define EMUCNT1           *(volatile unsigned int*)0x01C111F4

// 时钟
#define UART_1_FREQ       (pllcfg.PLL0_SYSCLK2 * 1000000)

/****************************************************************************/
/*                                                                          */
/*              GPIO 管脚复用配置                                           */
/*                                                                          */
/****************************************************************************/
static void GPIOBankPinMuxSet()
{
	// 两线制串口 不使用流控
    #define PINMUX4_UART1_TXD_ENABLE    (SYSCFG_PINMUX4_PINMUX4_31_28_UART1_TXD << SYSCFG_PINMUX4_PINMUX4_31_28_SHIFT)
    #define PINMUX4_UART1_RXD_ENABLE    (SYSCFG_PINMUX4_PINMUX4_27_24_UART1_RXD << SYSCFG_PINMUX4_PINMUX4_27_24_SHIFT)

    unsigned int savePinmux = 0;

    savePinmux = (HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(4)) & ~(SYSCFG_PINMUX4_PINMUX4_31_28 | SYSCFG_PINMUX4_PINMUX4_27_24));
    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(4)) = ((PINMUX4_UART1_TXD_ENABLE | PINMUX4_UART1_RXD_ENABLE) | savePinmux);
}

/****************************************************************************/
/*                                                                          */
/*              UART 配置                                                   */
/*                                                                          */
/****************************************************************************/
static void UARTConfig()
{
    // 波特率 115200 数据位 8 停止位 1 无校验位
    UARTConfigSetExpClk(SOC_UART_1_REGS, UART_1_FREQ, BAUD_115200, UART_WORDL_8BITS, UART_OVER_SAMP_RATE_16);

    // 使能 UART
    UARTEnable(SOC_UART_1_REGS);

    // 使能接收 / 发送 FIFO
    UARTFIFOEnable(SOC_UART_1_REGS);

    // 设置 FIFO 级别
    UARTFIFOLevelSet(SOC_UART_1_REGS, UART_RX_TRIG_LEVEL_1);
}

/****************************************************************************/
/*                                                                          */
/*              UART 中断初始化                                             */
/*                                                                          */
/****************************************************************************/
static void UARTInterruptInit()
{
	// 使能中断
	unsigned int intFlags = 0;
    intFlags |= (UART_INT_LINE_STAT | UART_INT_RXDATA_CTI);
    UARTIntEnable(SOC_UART_1_REGS, intFlags);
}

/****************************************************************************/
/*                                                                          */
/*              硬件中断线程                                                */
/*                                                                          */
/****************************************************************************/
Void UART1Hwi(UArg arg)
{
    unsigned int int_id = 0;
    unsigned char rxData = 0;

    // 确定中断源
    int_id = UARTIntStatus(SOC_UART_1_REGS);

    // 接收中断
    if(UART_INTID_RX_DATA == int_id)
    {
        rxData = UARTCharGetNonBlocking(SOC_UART_1_REGS);
        UARTCharPutNonBlocking(UARTConsole, rxData);
    }

    // 接收错误
    if(UART_INTID_RX_LINE_STAT == int_id)
    {
        while(UARTRxErrorGet(SOC_UART_1_REGS))
        {
            // 从 RBR 读一个字节
            UARTCharGetNonBlocking(SOC_UART_1_REGS);
        }
    }
}

static Void HwiInit()
{
    Hwi_Params hwiParams;
    Hwi_Params_init(&hwiParams);
    hwiParams.eventId = SYS_INT_UART1_INT;
    Hwi_create(C674X_MASK_INT11, UART1Hwi, &hwiParams, NULL);
}

/****************************************************************************/
/*                                                                          */
/*              串口输出                                                    */
/*                                                                          */
/****************************************************************************/
unsigned int UART1Puts(char *pTxBuffer, int numBytesToWrite)
{
    unsigned int count = 0;
    unsigned int flag = 0;

    if(numBytesToWrite < 0)
    {
        flag = 1;
    }

    while('\0' != *pTxBuffer)
    {
        UARTCharPut(SOC_UART_1_REGS, (unsigned char)*pTxBuffer);
        pTxBuffer++;
        count++;

        if((0 == flag) && (count == numBytesToWrite))
        {
            break;
        }
    }

    return count;
}

/****************************************************************************/
/*                                                                          */
/*              初始化                                                      */
/*                                                                          */
/****************************************************************************/
void UART1Init()
{
    // 外设使能
    PSCModuleControl(SOC_PSC_1_REGS, HW_PSC_UART1, PSC_POWERDOMAIN_ALWAYS_ON,PSC_MDCTL_NEXT_ENABLE);

    // GPIO 管脚复用配置
    GPIOBankPinMuxSet();

    // 串口参数配置
    UARTConfig();

    // 串口中断配置
    UARTInterruptInit();

    // 硬件中断线程初始化
    HwiInit();
}
