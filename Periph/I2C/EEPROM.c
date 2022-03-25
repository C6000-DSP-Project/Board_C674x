/****************************************************************************/
/*                                                                          */
/*    �º˿Ƽ�(����)���޹�˾                                                */
/*                                                                          */
/*    Copyright (C) 2022 CoreKernel Technology (Guangzhou) Co., Ltd         */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/*    I2C EEPROM ��д                                                       */
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
/// EEPROM ��ַ
#define ADDRESS   0x50

/****************************************************************************/
/*                                                                          */
/*              ȫ�ֱ���                                                    */
/*                                                                          */
/****************************************************************************/

/****************************************************************************/
/*                                                                          */
/*              ��������                                                    */
/*                                                                          */
/****************************************************************************/
void EEPROMByteWrite(unsigned int Address, unsigned char Data);
void EEPROMPageWrite(unsigned int Address, unsigned char *Data, unsigned int Size);
void EEPROMPageRead(unsigned int Address, unsigned char *Data, unsigned int Size);
unsigned char EEPROMRandomRead(unsigned int Address);

/****************************************************************************/
/*                                                                          */
/*              ���Ժ���                                                    */
/*                                                                          */
/****************************************************************************/
void EEPROMTest(void)
{
	int i,result;
	unsigned char buf_send[8];
	unsigned char buf_recv[8];

	// I2C ����
	I2CInit(SOC_I2C_0_REGS, ADDRESS);

    // дһ���ֽ�
	ConsoleWrite("Write single byte to address 0x0, value is 0x55.\r\n\r\n");
    EEPROMByteWrite(0x0, 0x55);

    // ��һ���ֽ�
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
    // ����дָ�����ȣ�һ��ҳ�棩
    ConsoleWrite("Write one page (8 bytes) to address 0x0.\r\n\r\n");
    EEPROMPageWrite(0x00,buf_send,8);

    memset(buf_recv,0,8);

    // ������ָ�����ȣ�һ��ҳ�棩
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

    // ��� result ���� 1 ��˵��д����ֽ�ֵ��������ֽ�ֵȫ����ͬ������д�����ȡ�Ĳ�һ��
	if (result)
	    ConsoleWrite("Verify successfully.\r\n");
	else
	    ConsoleWrite("Verify failed.\r\n\r\n");
}

// ָ����ʱ���Ǿ�ȷ��
static Void Delay(UInt n)
{
	UInt i;

	for(i = n; i > 0; i--);
}
/****************************************************************************/
/*                                                                          */
/*              EEPROM дһ���ֽڵ�ָ����ַ                                 */
/*                                                                          */
/****************************************************************************/
void EEPROMByteWrite(unsigned int Address, unsigned char Data)
{
	I2CRegWrite(SOC_I2C_0_REGS, Address, Data);
	Delay(0x1ff);
}

/****************************************************************************/
/*                                                                          */
/*              EEPROM д����ֽڵ�ָ����ʼ��ַ                             */
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
/*              EEPROM д����ֽڵ�ָ����ʼ��ַ                             */
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
/*              EEPROM ��ָ����ַ��һ���ֽ�                                 */
/*                                                                          */
/****************************************************************************/
unsigned char EEPROMRandomRead(unsigned int Address)
{
	unsigned char ret;

	ret = I2CRegRead(SOC_I2C_0_REGS, Address);
	Delay(0x1ff);

   	return ret;
}
