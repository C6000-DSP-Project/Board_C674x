/****************************************************************************/
/*                                                                          */
/*    新核科技(广州)有限公司                                                */
/*                                                                          */
/*    Copyright (C) 2022 CoreKernel Technology (Guangzhou) Co., Ltd         */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/*    SPI NOR FLASH                                                         */
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

/****************************************************************************/
/*                                                                          */
/*              宏定义                                                      */
/*                                                                          */
/****************************************************************************/
// SPI 管脚配置
#define SIMO_SOMI_CLK_CS        0x00000E03
#define CHAR_LENGTH             0x8

// FLASH 地址
#define SPI_FLASH_ADDR_MSB1     0x0A
#define SPI_FLASH_ADDR_MSB0     0x00
#define SPI_FLASH_ADDR_LSB      0x00

// 扇区擦除命令
#define SPI_FLASH_SECTOR_ERASE  0xD8

// 页写入命令
#define SPI_FLASH_PAGE_WRITE    0x02

// 读状态寄存器命令
#define SPI_FLASH_STATUS_RX     0x05

// 写使能命令
#define SPI_FLASH_WRITE_EN      0x06

// 读命令
#define SPI_FLASH_READ          0x03

// 写操作执行中
#define WRITE_IN_PROGRESS       0x01

/****************************************************************************/
/*                                                                          */
/*              全局变量                                                    */
/*                                                                          */
/****************************************************************************/
volatile unsigned int spi_flag = 1;

unsigned int tx_len;
unsigned int rx_len;
unsigned char vrf_data[260];
unsigned char tx_data[260];
unsigned char rx_data[260];
unsigned char *p_tx;
unsigned char *p_rx;

/****************************************************************************/
/*                                                                          */
/*              SPI 传输                                                    */
/*                                                                          */
/****************************************************************************/
static void SpiTransfer()
{
    p_tx = &tx_data[0];
    p_rx = &rx_data[0];
    SPIIntEnable(SOC_SPI_1_REGS, (SPI_RECV_INT | SPI_TRANSMIT_INT));
    while(spi_flag);
    spi_flag = 1;

    SPIDat1Config(SOC_SPI_1_REGS, SPI_DATA_FORMAT0, 0x1);
}

/****************************************************************************/
/*                                                                          */
/*              GPIO 管脚复用配置                                           */
/*                                                                          */
/****************************************************************************/
void SPI1CSPinMuxSet(unsigned int csPinNum)
{
     switch(csPinNum)
     {
          case 0:
              HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(5)) = (HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(5)) & (~(SYSCFG_PINMUX5_PINMUX5_7_4))) |
                                                             (SYSCFG_PINMUX5_PINMUX5_7_4_NSPI1_SCS0 << SYSCFG_PINMUX5_PINMUX5_7_4_SHIFT);

              break;

          case 1:
              HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(5)) = (HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(5)) & (~(SYSCFG_PINMUX5_PINMUX5_3_0))) |
                                                             (SYSCFG_PINMUX5_PINMUX5_3_0_NSPI1_SCS1 << SYSCFG_PINMUX5_PINMUX5_3_0_SHIFT);

              break;

          case 2:
              HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(4)) = (HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(4)) & (~(SYSCFG_PINMUX4_PINMUX4_31_28))) |
                                                             (SYSCFG_PINMUX4_PINMUX4_31_28_NSPI1_SCS2 << SYSCFG_PINMUX4_PINMUX4_31_28_SHIFT);

              break;

          case 3:
              HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(4)) = (HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(4)) & (~(SYSCFG_PINMUX4_PINMUX4_27_24))) |
                                                             (SYSCFG_PINMUX4_PINMUX4_27_24_NSPI1_SCS3 << SYSCFG_PINMUX4_PINMUX4_27_24_SHIFT);

              break;

          case 4:
              HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(4)) = (HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(4)) & (~(SYSCFG_PINMUX4_PINMUX4_23_20))) |
                                                             (SYSCFG_PINMUX4_PINMUX4_23_20_NSPI1_SCS4 << SYSCFG_PINMUX4_PINMUX4_23_20_SHIFT);

              break;

          case 5:
              HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(4)) = (HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(4)) & (~(SYSCFG_PINMUX4_PINMUX4_19_16))) |
                                                             (SYSCFG_PINMUX4_PINMUX4_19_16_NSPI1_SCS5 << SYSCFG_PINMUX4_PINMUX4_19_16_SHIFT);

              break;

          case 6:
              HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(4)) = (HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(4)) & (~(SYSCFG_PINMUX4_PINMUX4_15_12))) |
                                                             (SYSCFG_PINMUX4_PINMUX4_15_12_NSPI1_SCS6 << SYSCFG_PINMUX4_PINMUX4_15_12_SHIFT);

              break;

          case 7:
              HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(4)) = (HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(4)) & (~(SYSCFG_PINMUX4_PINMUX4_11_8))) |
                                                             (SYSCFG_PINMUX4_PINMUX4_11_8_NSPI1_SCS7 << SYSCFG_PINMUX4_PINMUX4_11_8_SHIFT);

              break;

          default:
              break;
     }
}

static void GPIOBankPinMuxSet()
{
    // SPI1 CLK SIMO SOMI
    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(5)) = (HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(5)) & (~(SYSCFG_PINMUX5_PINMUX5_23_20 | SYSCFG_PINMUX5_PINMUX5_19_16 | SYSCFG_PINMUX5_PINMUX5_15_12 | SYSCFG_PINMUX5_PINMUX5_11_8))) |
                                                   ((SYSCFG_PINMUX5_PINMUX5_11_8_SPI1_CLK << SYSCFG_PINMUX5_PINMUX5_11_8_SHIFT) |
                                                    (SYSCFG_PINMUX5_PINMUX5_23_20_SPI1_SIMO0 << SYSCFG_PINMUX5_PINMUX5_23_20_SHIFT) |
                                                    (SYSCFG_PINMUX5_PINMUX5_19_16_SPI1_SOMI0 << SYSCFG_PINMUX5_PINMUX5_19_16_SHIFT));

    // SPI1 CS
    SPI1CSPinMuxSet(0);
}

/****************************************************************************/
/*                                                                          */
/*              读状态寄存器                                                */
/*                                                                          */
/****************************************************************************/
static void StatusGet()
{
    tx_data[0] = SPI_FLASH_STATUS_RX;
    tx_len = rx_len = 2;
    SPIDat1Config(SOC_SPI_1_REGS, (SPI_CSHOLD | SPI_DATA_FORMAT0), 0x1);
    SpiTransfer();
}

/****************************************************************************/
/*                                                                          */
/*              写使能                                                      */
/*                                                                          */
/****************************************************************************/
static void WriteEnable(void)
{
    tx_data[0] = SPI_FLASH_WRITE_EN;
    tx_len = rx_len = 1;
    SPIDat1Config(SOC_SPI_1_REGS, (SPI_CSHOLD | SPI_DATA_FORMAT0), 0x1);
    SpiTransfer();
}

/****************************************************************************/
/*                                                                          */
/*              忙检测                                                      */
/*                                                                          */
/****************************************************************************/
static void IsFlashBusy(void)
{
    do{
         StatusGet();

      } while(rx_data[1] & WRITE_IN_PROGRESS);
}

/****************************************************************************/
/*                                                                          */
/*              扇区擦除                                                    */
/*                                                                          */
/****************************************************************************/
static void SectorErase(void)
{
    tx_data[0] =  SPI_FLASH_SECTOR_ERASE;
    tx_data[1] =  SPI_FLASH_ADDR_MSB1;
    tx_data[2] =  SPI_FLASH_ADDR_MSB0;
    tx_data[3] =  SPI_FLASH_ADDR_LSB;

    tx_len = rx_len = 4;
    SPIDat1Config(SOC_SPI_1_REGS, (SPI_CSHOLD | SPI_DATA_FORMAT0), 0x1);
    SpiTransfer();

    IsFlashBusy();
}

/****************************************************************************/
/*                                                                          */
/*              写扇区(256字节)                                             */
/*                                                                          */
/****************************************************************************/
static void WritetoFlash(void)
{
    unsigned int index;

    tx_data[0] =  SPI_FLASH_PAGE_WRITE;
    tx_data[1] =  SPI_FLASH_ADDR_MSB1;
    tx_data[2] =  SPI_FLASH_ADDR_MSB0;
    tx_data[3] =  SPI_FLASH_ADDR_LSB;

    // 准备要写入的数据
    for (index = 4; index < 260; index++)
    {
        tx_data[index] =  index;
    }

    for(index = 4; index < 260; index++)
    {
         vrf_data[index] = index;
    }

    tx_len = rx_len = index;
    SPIDat1Config(SOC_SPI_1_REGS, (SPI_CSHOLD | SPI_DATA_FORMAT0), 0x1);
    SpiTransfer();

    IsFlashBusy();
}

/****************************************************************************/
/*                                                                          */
/*              读数据                                                      */
/*                                                                          */
/****************************************************************************/
static void ReadFromFlash(void)
{
    unsigned int index;

    tx_data[0] =  SPI_FLASH_READ;
    tx_data[1] =  SPI_FLASH_ADDR_MSB1;
    tx_data[2] =  SPI_FLASH_ADDR_MSB0;
    tx_data[3] =  SPI_FLASH_ADDR_LSB;

    // 情况变量
    for (index = 4; index < 260; index++)
    {
        tx_data[index] =  0;
    }

    tx_len = rx_len = index;
    SPIDat1Config(SOC_SPI_1_REGS, (SPI_CSHOLD | SPI_DATA_FORMAT0), 0x1);
    SpiTransfer();
}

/****************************************************************************/
/*                                                                          */
/*              数据校验                                                    */
/*                                                                          */
/****************************************************************************/
static int VerifyData(void)
{
    unsigned int index;

    for(index = 4; index < 260; index++)
    {
        if(vrf_data[index] != rx_data[index])
        {
        	UARTprintf("\r\n");
        	UARTprintf("VerifyData: Comparing the data written to and read");
        	UARTprintf(" from Flash.\r\nThe two data blocks are unequal.");
        	UARTprintf(" Mismatch found at index ");
        	UARTprintf("%d", (int)index - 3);
            UARTprintf("\r\n");
            UARTprintf("Verify Failed.\r\n\r\n");
            return 0;
        }
    }

    if (index == 260)
    {
    	UARTprintf("\r\nThe data in the Flash and the one written ");
    	UARTprintf("to it are equal.\r\n");
    	UARTprintf("Verify successfully.\r\n\r\n");
        return 1;
    }

    return 0;
}

/****************************************************************************/
/*                                                                          */
/*              配置 SPI 数据格式                                           */
/*                                                                          */
/****************************************************************************/
static void SPIDataFormatConfig(unsigned int dataFormat)
{
    // 配置 SPI 时钟
    SPIConfigClkFormat(SOC_SPI_1_REGS,
                        (SPI_CLK_POL_HIGH | SPI_CLK_INPHASE),
                         dataFormat);

    // 配置 SPI 发送时 MSB 优先
    SPIShiftMsbFirst(SOC_SPI_1_REGS, dataFormat);

    // 设置字符长度
    SPICharLengthSet(SOC_SPI_1_REGS, CHAR_LENGTH, dataFormat);
}

/****************************************************************************/
/*                                                                          */
/*              SPI 初始化  		 	      	                            */
/*                                                                          */
/****************************************************************************/
static void SPIInit(void)
{
    unsigned char cs  = 0x01;
    unsigned char dcs = 0x01;
    unsigned int  val = SIMO_SOMI_CLK_CS;

    SPIReset(SOC_SPI_1_REGS);

    SPIOutOfReset(SOC_SPI_1_REGS);

    SPIModeConfigure(SOC_SPI_1_REGS, SPI_MASTER_MODE);

    SPIClkConfigure(SOC_SPI_1_REGS, 228000000, 20000000, SPI_DATA_FORMAT0);

    SPIPinControl(SOC_SPI_1_REGS, 0, 0, &val);

    SPIDefaultCSSet(SOC_SPI_1_REGS, dcs);

    // 配置 SPI 数据格式
    SPIDataFormatConfig(SPI_DATA_FORMAT0);

    // 配置 SPI 数据格式及片选信号
    SPIDat1Config(SOC_SPI_1_REGS, (SPI_CSHOLD | SPI_DATA_FORMAT0), cs);

	// 映射中断到 INT1
    SPIIntLevelSet(SOC_SPI_1_REGS, SPI_RECV_INTLVL | SPI_TRANSMIT_INTLVL);

    // 使能 SPI
    SPIEnable(SOC_SPI_1_REGS);
}

/****************************************************************************/
/*                                                                          */
/*              硬件中断线程                                                */
/*                                                                          */
/****************************************************************************/
extern volatile unsigned int spi_flag;
extern unsigned int tx_len;
extern unsigned int rx_len;
extern unsigned char *p_tx;
extern unsigned char *p_rx;

Void SPIHwi(UArg arg)
{
    unsigned int intCode = 0;

    intCode = SPIInterruptVectorGet(SOC_SPI_1_REGS);

    while (intCode)
    {
        if(intCode == SPI_TX_BUF_EMPTY)
        {
            tx_len--;
            SPITransmitData1(SOC_SPI_1_REGS, *p_tx);
            p_tx++;
            if (!tx_len)
            {
                SPIIntDisable(SOC_SPI_1_REGS, SPI_TRANSMIT_INT);
            }
        }

        if(intCode == SPI_RECV_FULL)
        {
            rx_len--;
            *p_rx = (char)SPIDataReceive(SOC_SPI_1_REGS);
            p_rx++;
            if (!rx_len)
            {
                spi_flag = 0;
                SPIIntDisable(SOC_SPI_1_REGS, SPI_RECV_INT);
            }
        }

        intCode = SPIInterruptVectorGet(SOC_SPI_1_REGS);
    }
}

static Void HwiInit()
{
    /* 映射到组合事件中断 */
    // EVT0  1 - 31
    // EVT1 32 - 63
    // EVT2 64 - 95
    // EVT3 96 - 127

    EventCombiner_dispatchPlug(SYS_INT_SPI1_INT, &SPIHwi, 0, TRUE);
}

void SPIFlashTest()
{
    SPIFlashInit();

    // 写使能
    WriteEnable();

    UARTprintf("Erase a sector of the flash.\r\n");

    // 擦除 Flash
    SectorErase();

    WriteEnable();
    // 写 Flash
    WritetoFlash();
    // 读 Flash
    ReadFromFlash();

    // 数据校验
    VerifyData();
}

/****************************************************************************/
/*                                                                          */
/*              初始化                                                      */
/*                                                                          */
/****************************************************************************/
void SPIFlashInit()
{
    // 使能外设
    PSCModuleControl(SOC_PSC_1_REGS, HW_PSC_SPI1, PSC_POWERDOMAIN_ALWAYS_ON, PSC_MDCTL_NEXT_ENABLE);

    // 管脚复用配置
    GPIOBankPinMuxSet();

    // SPI 初始化
    SPIInit();

    // 硬件中断线程初始化
    HwiInit();
}
