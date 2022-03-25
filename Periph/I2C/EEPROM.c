/****************************************************************************/
/*                                                                          */
/*    新核科技(广州)有限公司                                                */
/*                                                                          */
/*    Copyright (C) 2022 CoreKernel Technology (Guangzhou) Co., Ltd         */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/*    I2C EEPROM 读写                                                       */
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
/// EEPROM 地址
#define ADDRESS   0x50

/****************************************************************************/
/*                                                                          */
/*              全局变量                                                    */
/*                                                                          */
/****************************************************************************/

/****************************************************************************/
/*                                                                          */
/*              函数声明                                                    */
/*                                                                          */
/****************************************************************************/
void EEPROMByteWrite(unsigned int Address, unsigned char Data);
void EEPROMPageWrite(unsigned int Address, unsigned char *Data, unsigned int Size);
void EEPROMPageRead(unsigned int Address, unsigned char *Data, unsigned int Size);
unsigned char EEPROMRandomRead(unsigned int Address);

/****************************************************************************/
/*                                                                          */
/*              测试函数                                                    */
/*                                                                          */
/****************************************************************************/
void EEPROMTest(void)
{
	int i,result;
	unsigned char buf_send[8];
	unsigned char buf_recv[8];

	// I2C 配置
	I2CInit(SOC_I2C_0_REGS, ADDRESS);

    // 写一个字节
	ConsoleWrite("Write single byte to address 0x0, value is 0x55.\r\n\r\n");
    EEPROMByteWrite(0x0, 0x55);

    // 读一个字节
    memset(buf_recv,0,8);
    ConsoleWrite("Read one byte at a address 0x0, the value is ");
    buf_recv[0] = EEPROMRandomRead(0);

    UARTprintf("%d", buf_recv[0]);
    ConsoleWrite(".\r\n\r\n");

    for(i=0;i<8;i++)
    {
    	if(i%2 ==0)
    		buf_send[i]=0xaa;
    	else
    		buf_send[i]=0x55;
    }
    // 连续写指定长度（一个页面）
    ConsoleWrite("Write one page (8 bytes) to address 0x0.\r\n\r\n");
    EEPROMPageWrite(0x00,buf_send,8);

    memset(buf_recv,0,8);

    // 连续读指定长度（一个页面）
    ConsoleWrite("Read one page (8 bytes) at address 0x0.\r\n\r\n",);
	EEPROMPageRead(0x00,buf_recv,8);

    result = 1;
    for (i=0;i<8;i++)
    {
    	if (buf_send[i]!=buf_recv[i])
    	{
    		result = 0;
    		break;
    	}
    }

    // 如果 result 等于 1 ，说明写入的字节值与读出的字节值全部相同。否则，写入与读取的不一致
	if (result)
	    ConsoleWrite("Verify successfully.\r\n");
	else
	    ConsoleWrite("Verify failed.\r\n\r\n");
}

// 指令延时（非精确）
static Void Delay(UInt n)
{
	UInt i;

	for(i = n; i > 0; i--);
}
/****************************************************************************/
/*                                                                          */
/*              EEPROM 写一个字节到指定地址                                 */
/*                                                                          */
/****************************************************************************/
void EEPROMByteWrite(unsigned int Address, unsigned char Data)
{
	I2CRegWrite(SOC_I2C_0_REGS, Address, Data);
	Delay(0x1ff);
}

/****************************************************************************/
/*                                                                          */
/*              EEPROM 写多个字节到指定起始地址                             */
/*                                                                          */
/****************************************************************************/
void EEPROMPageWrite(unsigned int Address, unsigned char *Data, unsigned int Size)
{
	int i;

	for (i=0;i<Size;i++)
	{
		I2CRegWrite(SOC_I2C_0_REGS, Address+i, Data[i]);
		Delay(0x1ffff);
	}
}

/****************************************************************************/
/*                                                                          */
/*              EEPROM 写多个字节到指定起始地址                             */
/*                                                                          */
/****************************************************************************/
void EEPROMPageRead(unsigned int Address, unsigned char *Data, unsigned int Size)
{
	int i;

	for (i=0;i<Size;i++)
	{
		Data[i] = I2CRegRead(SOC_I2C_0_REGS, Address+i);
		Delay(0x1ff);
	}
}

/****************************************************************************/
/*                                                                          */
/*              EEPROM 从指定地址读一个字节                                 */
/*                                                                          */
/****************************************************************************/
unsigned char EEPROMRandomRead(unsigned int Address)
{
	unsigned char ret;

	ret = I2CRegRead(SOC_I2C_0_REGS, Address);
	Delay(0x1ff);

   	return ret;
}
