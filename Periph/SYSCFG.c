/****************************************************************************/
/*                                                                          */
/*    新核科技(广州)有限公司                                                */
/*                                                                          */
/*    Copyright (C) 2022 CoreKernel Technology (Guangzhou) Co., Ltd         */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/*    系统配置                                                              */
/*                                                                          */
/*    2022年03月27日                                                        */
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
/*              全局变量                                                    */
/*                                                                          */
/****************************************************************************/
unsigned int BootMode;
char *BootModeStr[] =
{
    "EMIF NOR",
    "EMIF NAND 8Bit",
    "EMIF NAND 16Bit",
    "MMC/SD0",
    "I2C0 EEPROM",
    "I2C1 EEPROM",
    "I2C0 Slave",
    "I2C1 Slave",
    "SPI0 EEPROM",
    "SPI1 EEPROM",
    "SPI0 Flash",
    "SPI1 Flash",
    "SPI0 Slave",
    "SPI1 Slave",
    "UART0",
    "UART1",
    "UART2",
    "HPI",
    "Emulation Debug",
    "INVALID MODE"
};

/****************************************************************************/
/*                                                                          */
/*              启动模式                                                    */
/*                                                                          */
/****************************************************************************/
void BootModeGet()
{
    unsigned int val = HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_BOOTCFG);

    switch(val & 0x1F)
    {
        case 0x02: BootMode = 0; break;
        case 0x0E: BootMode = 1; break;
        case 0x10: BootMode = 2; break;
        case 0x1C: BootMode = 3; break;
        case 0x00: BootMode = 4; break;
        case 0x06: BootMode = 5; break;
        case 0x01: BootMode = 6; break;
        case 0x07: BootMode = 7; break;
        case 0x08: BootMode = 8; break;
        case 0x09: BootMode = 9; break;
        case 0x0A: BootMode = 10; break;
        case 0x0C: BootMode = 11; break;
        case 0x12: BootMode = 12; break;
        case 0x13: BootMode = 13; break;
        case 0x16: BootMode = 14; break;
        case 0x17: BootMode = 15; break;
        case 0x14: BootMode = 16; break;
        case 0x04: BootMode = 17; break;
        case 0x1E: BootMode = 18; break;

        default:
            BootMode = 19;
            break;
    }
}
