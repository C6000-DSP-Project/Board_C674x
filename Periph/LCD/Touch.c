/****************************************************************************/
/*                                                                          */
/*              LCD ������                                                  */
/*                                                                          */
/*              2014��07��29��                                              */
/*                                                                          */
/****************************************************************************/
#include <App.h>

/****************************************************************************/
/*                                                                          */
/*              �궨��                                                      */
/*                                                                          */
/****************************************************************************/
#define PINMUX5_GPIO2_12_ENABLE    (SYSCFG_PINMUX5_PINMUX5_15_12_GPIO2_12 << \
								    SYSCFG_PINMUX5_PINMUX5_15_12_SHIFT)

#define SIMO_SOMI_CLK_CS1          0x00000E02

#define DEFAULT_XSCALE             (0.5096)
#define DEFAULT_YSCALE             (0.260162)

#define XOFFSET                    (-22.73)
#define YOFFSET                    (-76.84)

#define TOUCH_AD_LEFT              60
#define TOUCH_AD_RIGHT             1992 // A/D ת�������ص����ֵ
#define TOUCH_AD_TOP               1927
#define TOUCH_AD_BOTTOM            85

/****************************************************************************/
/*                                                                          */
/*              ȫ�ֱ���                                                    */
/*                                                                          */
/****************************************************************************/
float xscale = DEFAULT_XSCALE;
float yscale = DEFAULT_YSCALE;
float xoffset = XOFFSET;
float yoffset = YOFFSET;

// ��������
int x;
int y;

// ������־
char touch_flag;

/****************************************************************************/
/*                                                                          */
/*              ��������                                                    */
/*                                                                          */
/****************************************************************************/
void tsc2046ReadAxis(char mode, int*p1);

/****************************************************************************/
/*                                                                          */
/*              ��ʱ                                                        */
/*                                                                          */
/****************************************************************************/
static void Delay(volatile unsigned int delay)
{
    while(delay--);
}

/****************************************************************************/
/*                                                                          */
/*              �������жϷ�����                                          */
/*                                                                          */
/****************************************************************************/
Void TouchIsr(UArg arg)
{
	GPIOBankIntDisable(SOC_GPIO_0_REGS, 2);

	if (GPIOPinIntStatus(SOC_GPIO_0_REGS, 45) == GPIO_INT_PEND)
	{
		touch_flag = 1;
	}

	GPIOPinIntClear(SOC_GPIO_0_REGS, 45);

	GPIOBankIntEnable(SOC_GPIO_0_REGS, 2);
}

/****************************************************************************/
/*                                                                          */
/*              �������ܽų�ʼ��                                            */
/*                                                                          */
/****************************************************************************/
void TouchGPIOInit(void)
{
	volatile unsigned int savePinMux = 0;

	savePinMux = HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(5)) & \
						 ~(SYSCFG_PINMUX5_PINMUX5_15_12);

	HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(5)) = \
    		 (PINMUX5_GPIO2_12_ENABLE | savePinMux);

	// ����GPIO2[12]Ϊ����ģʽ
	GPIODirModeSet(SOC_GPIO_0_REGS, 45, GPIO_DIR_INPUT);

	// ����GPIO2[12]Ϊ�½��ش����ж�ģʽ
	GPIOIntTypeSet(SOC_GPIO_0_REGS, 45, GPIO_INT_TYPE_FALLEDGE);

	// ��������GPIO2[15��0]�����ж�
	GPIOBankIntEnable(SOC_GPIO_0_REGS, 2);
}

/****************************************************************************/
/*                                                                          */
/*              �����������ʼ��                                            */
/*                                                                          */
/****************************************************************************/
void TouchInitXY(void)
{
	xscale=(float)800.0 / (TOUCH_AD_RIGHT - TOUCH_AD_LEFT); // �õ�xscale
	xoffset=(float)(800.0 - xscale * TOUCH_AD_RIGHT);       // �õ�xoff

	yscale=(float)480.0 / (TOUCH_AD_TOP - TOUCH_AD_BOTTOM); // �õ�yscale
	yoffset=(float)(480.0 - yscale * TOUCH_AD_TOP);         // �õ�yoff
}

/****************************************************************************/
/*                                                                          */
/*              ��������ʼ��                                                */
/*                                                                          */
/****************************************************************************/
void TouchInit(void)
{
	unsigned int  val = SIMO_SOMI_CLK_CS1;
	unsigned char dcs = 0x01;
	unsigned char cs  = 0x01;

	TouchInitXY();

	TouchGPIOInit();

	SPIReset(SOC_SPI_1_REGS);

	SPIOutOfReset(SOC_SPI_1_REGS);

	// ����ģʽ
	SPIModeConfigure(SOC_SPI_1_REGS, SPI_MASTER_MODE);

	SPIClkConfigure(SOC_SPI_1_REGS, 228000000, 1000000, SPI_DATA_FORMAT0);

	// ʹ��SIMO_SOMI_CLK_CS1����
	SPIPinControl(SOC_SPI_1_REGS, 0, 0, &val);

	// ����CS1����ʱΪ�ߵ�ƽ
	SPIDefaultCSSet(SOC_SPI_1_REGS, (dcs<<1));

	// ���� SPI ���ݸ�ʽ
	SPIConfigClkFormat(SOC_SPI_1_REGS,
					 (SPI_CLK_POL_LOW | SPI_CLK_OUTOFPHASE),  // SPI_CLK_POL_LOW     - �����ݴ���ǰ��ʱ�ӱ��ֵ�
					 SPI_DATA_FORMAT0);					      // SPI_CLK_INPHASE     - ʱ�Ӳ���ʱ
															  // SPI_CLK_OUTOFPHASE  - ʱ����ʱ���ʱ������

	SPIShiftMsbFirst(SOC_SPI_1_REGS, SPI_DATA_FORMAT0);

	// �����ַ�����
	SPICharLengthSet(SOC_SPI_1_REGS, 8, SPI_DATA_FORMAT0);

	SPIDat1Config(SOC_SPI_1_REGS, (SPI_CSHOLD | SPI_DATA_FORMAT0), (cs<<1));

	// ʹ�� SPI
	SPIEnable(SOC_SPI_1_REGS);
}

/****************************************************************************/
/*                                                                          */
/*              ȡ�� AD ֵ                                                  */
/*                                                                          */
/****************************************************************************/
int Read_touch_ad(void)
{
	int result=0, re1, re2;

	SPITransmitData1(SOC_SPI_1_REGS, 0);
	while( (HWREG(SOC_SPI_1_REGS + SPI_SPIBUF) & 0x80000000 ));
	re1 = SPIDataReceive(SOC_SPI_1_REGS);

	SPITransmitData1(SOC_SPI_1_REGS, 0);
	while( (HWREG(SOC_SPI_1_REGS + SPI_SPIBUF) & 0x80000000 ));
	re2 = SPIDataReceive(SOC_SPI_1_REGS);

	result = (re1<<4) + (re2>>4);

	return result;
}

/****************************************************************************/
/*                                                                          */
/*              �������                                                    */
/*                                                                          */
/****************************************************************************/
unsigned int TouchDetect(void)
{
    return ((0 == touch_flag) ? FALSE : TRUE);
}

/****************************************************************************/
/*                                                                          */
/*              ȡ������ֵ                                                  */
/*                                                                          */
/****************************************************************************/
void tsc2046ReadAxis(char mode, int*p1)
{
    unsigned int result1=0;
    unsigned char count=0,t,t1;
    unsigned int databuffer[30],temp;

    switch(mode)
    {
    	case 0:
			do{
				SPIDat1Config(SOC_SPI_1_REGS, (SPI_CSHOLD | SPI_DATA_FORMAT0), (1<<1));
				SPITransmitData1(SOC_SPI_1_REGS, 0x93);
				Delay(0xf);

				while((HWREG(SOC_SPI_1_REGS + SPI_SPIBUF) & 0x80000000 ));

				databuffer[count] = Read_touch_ad();
				count++;
			}while(count<10);

			SPIDat1Config(SOC_SPI_1_REGS, SPI_DATA_FORMAT0, (1<<1));
			if(count == 10)
			{
				do                                            // ������X��������
				{
					t1=0;
					for(t=0;t<count-1;t++)
					{
						if(databuffer[t]>databuffer[t+1])     // ��������
						{
							temp=databuffer[t+1];
							databuffer[t+1]=databuffer[t];
							databuffer[t]=temp;
							t1=1;
						}
					}
				}while(t1);
			}

			break;
		case 1:
			do{
				SPIDat1Config(SOC_SPI_1_REGS, (SPI_CSHOLD | SPI_DATA_FORMAT0), (1<<1));
				SPITransmitData1(SOC_SPI_1_REGS, 0xd3);
				Delay(0xf);

				while( (HWREG(SOC_SPI_1_REGS + SPI_SPIBUF) & 0x80000000 ) );
				databuffer[count] = Read_touch_ad();
				count++;
				//Delay(0xf);
			}while(count<10);

			SPIDat1Config(SOC_SPI_1_REGS, SPI_DATA_FORMAT0, (1<<1));
			if(count == 10)
			{
				 do                                            // ������X��������
				{
					t1=0;
					for(t=0;t<count-1;t++)
					{
						if(databuffer[t]>databuffer[t+1])      // ��������
						{
							temp=databuffer[t+1];
							databuffer[t+1]=databuffer[t];
							databuffer[t]=temp;
							t1=1;
						}
					}
				}while(t1);
			}

			break;
    }

    for(count=3;count<8;count++)
    {
    	result1 = result1 + databuffer[count];
    }
    *p1 = result1/5;
}

void TouchCoOrdGet(int *pX, int *pY)
{
    unsigned short xpos, ypos;
    char x = 1, y =0;
    int rx1 = 0;
    int ry1 = 0;
    unsigned int xDpos = 0;
    int yDpos = 0;

    tsc2046ReadAxis(x, &rx1);

    tsc2046ReadAxis(y, &ry1);

    SPITransmitData1(SOC_SPI_1_REGS, 0x90);
    while( (HWREG(SOC_SPI_1_REGS + SPI_SPIBUF) & 0x80000000 ) );

    xpos = rx1;

    ypos = ry1;

    xDpos = xpos * xscale + xoffset;
    yDpos = ypos * yscale + yoffset;

    yDpos = LCD_HEIGHT - yDpos;

    if(yDpos < 0)
    {
        yDpos = 0;
    }

    *pX =(int)xDpos;
    *pY = yDpos;
}
