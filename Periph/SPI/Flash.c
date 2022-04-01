/****************************************************************************/
/*                                                                          */
/*    �º˿Ƽ�(����)���޹�˾                                                */
/*                                                                          */
/*    Copyright (C) 2022 CoreKernel Technology (Guangzhou) Co., Ltd         */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/*    SPI NOR FLASH                                                         */
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
/*              �궨��                                                      */
/*                                                                          */
/****************************************************************************/
// SPI �ܽ�����
#define SIMO_SOMI_CLK_CS        0x00000E03
#define CHAR_LENGTH             0x8

// FLASH ��ַ
#define SPI_FLASH_ADDR_MSB1     0x0A
#define SPI_FLASH_ADDR_MSB0     0x00
#define SPI_FLASH_ADDR_LSB      0x00

// ������������
#define SPI_FLASH_SECTOR_ERASE  0xD8

// ҳд������
#define SPI_FLASH_PAGE_WRITE    0x02

// ��״̬�Ĵ�������
#define SPI_FLASH_STATUS_RX     0x05

// дʹ������
#define SPI_FLASH_WRITE_EN      0x06

// ������
#define SPI_FLASH_READ          0x03

// д����ִ����
#define WRITE_IN_PROGRESS       0x01

/****************************************************************************/
/*                                                                          */
/*              ȫ�ֱ���                                                    */
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
/*              SPI ����                                                    */
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
/*              GPIO �ܽŸ�������                                           */
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
/*              ��״̬�Ĵ���                                                */
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
/*              дʹ��                                                      */
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
/*              æ���                                                      */
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
/*              ��������                                                    */
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
/*              д����(256�ֽ�)                                             */
/*                                                                          */
/****************************************************************************/
static void WritetoFlash(void)
{
    unsigned int index;

    tx_data[0] =  SPI_FLASH_PAGE_WRITE;
    tx_data[1] =  SPI_FLASH_ADDR_MSB1;
    tx_data[2] =  SPI_FLASH_ADDR_MSB0;
    tx_data[3] =  SPI_FLASH_ADDR_LSB;

    // ׼��Ҫд�������
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
/*              ������                                                      */
/*                                                                          */
/****************************************************************************/
static void ReadFromFlash(void)
{
    unsigned int index;

    tx_data[0] =  SPI_FLASH_READ;
    tx_data[1] =  SPI_FLASH_ADDR_MSB1;
    tx_data[2] =  SPI_FLASH_ADDR_MSB0;
    tx_data[3] =  SPI_FLASH_ADDR_LSB;

    // �������
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
/*              ����У��                                                    */
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
/*              ���� SPI ���ݸ�ʽ                                           */
/*                                                                          */
/****************************************************************************/
static void SPIDataFormatConfig(unsigned int dataFormat)
{
    // ���� SPI ʱ��
    SPIConfigClkFormat(SOC_SPI_1_REGS,
                        (SPI_CLK_POL_HIGH | SPI_CLK_INPHASE),
                         dataFormat);

    // ���� SPI ����ʱ MSB ����
    SPIShiftMsbFirst(SOC_SPI_1_REGS, dataFormat);

    // �����ַ�����
    SPICharLengthSet(SOC_SPI_1_REGS, CHAR_LENGTH, dataFormat);
}

/****************************************************************************/
/*                                                                          */
/*              SPI ��ʼ��  		 	      	                            */
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

    // ���� SPI ���ݸ�ʽ
    SPIDataFormatConfig(SPI_DATA_FORMAT0);

    // ���� SPI ���ݸ�ʽ��Ƭѡ�ź�
    SPIDat1Config(SOC_SPI_1_REGS, (SPI_CSHOLD | SPI_DATA_FORMAT0), cs);

	// ӳ���жϵ� INT1
    SPIIntLevelSet(SOC_SPI_1_REGS, SPI_RECV_INTLVL | SPI_TRANSMIT_INTLVL);

    // ʹ�� SPI
    SPIEnable(SOC_SPI_1_REGS);
}

/****************************************************************************/
/*                                                                          */
/*              Ӳ���ж��߳�                                                */
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
    /* ӳ�䵽����¼��ж� */
    // EVT0  1 - 31
    // EVT1 32 - 63
    // EVT2 64 - 95
    // EVT3 96 - 127

    EventCombiner_dispatchPlug(SYS_INT_SPI1_INT, &SPIHwi, 0, TRUE);
}

void SPIFlashTest()
{
    SPIFlashInit();

    // дʹ��
    WriteEnable();

    UARTprintf("Erase a sector of the flash.\r\n");

    // ���� Flash
    SectorErase();

    WriteEnable();
    // д Flash
    WritetoFlash();
    // �� Flash
    ReadFromFlash();

    // ����У��
    VerifyData();
}

/****************************************************************************/
/*                                                                          */
/*              ��ʼ��                                                      */
/*                                                                          */
/****************************************************************************/
void SPIFlashInit()
{
    // ʹ������
    PSCModuleControl(SOC_PSC_1_REGS, HW_PSC_SPI1, PSC_POWERDOMAIN_ALWAYS_ON, PSC_MDCTL_NEXT_ENABLE);

    // �ܽŸ�������
    GPIOBankPinMuxSet();

    // SPI ��ʼ��
    SPIInit();

    // Ӳ���ж��̳߳�ʼ��
    HwiInit();
}
