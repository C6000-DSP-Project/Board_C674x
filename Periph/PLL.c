/****************************************************************************/
/*                                                                          */
/*    �º˿Ƽ�(����)���޹�˾                                                */
/*                                                                          */
/*    Copyright (C) 2022 CoreKernel Technology (Guangzhou) Co., Ltd         */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/*    PLL                                                                   */
/*                                                                          */
/*    2022��03��27��                                                        */
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
/*              ȫ�ֱ���                                                    */
/*                                                                          */
/****************************************************************************/
unsigned int OSCINFreq = 24;  // 24MHz ����ʱ��

PLLClock pllcfg;

// ʱ�����
typedef enum
{
    OSCIN = 0x14,
    PLL0_SYSCLK1 = 0x17,
    PLL0_SYSCLK2,
    PLL0_SYSCLK3,
    PLL0_SYSCLK4,
    PLL0_SYSCLK5,
    PLL0_SYSCLK6,
    PLL0_SYSCLK7,
    PLLC1_OBSCLK
} OBSCLKSrc0;

typedef enum
{
    PLL1_SYSCLK1 = 0x17,
    PLL1_SYSCLK2,
    PLL1_SYSCLK3
} OBSCLKSrc1;

/****************************************************************************/
/*                                                                          */
/*              PLL ʱ��                                                    */
/*                                                                          */
/****************************************************************************/
void PLLClockGet()
{
    unsigned int PLLCTL;
    unsigned int PLLPreDiv, PLLM, PLLPostDiv;
    unsigned int PLLDiv1, PLLDiv2, PLLDiv3, PLLDiv4, PLLDiv6, PLLDiv7;
    unsigned int PLLOut;

    // PLLC1
    PLLCTL = HWREG(SOC_PLLC_1_REGS + PLLC_PLLCTL);
    PLLM = HWREG(SOC_PLLC_1_REGS + PLLC_PLLM);
    PLLPostDiv = HWREG(SOC_PLLC_1_REGS + PLLC_POSTDIV);

    PLLDiv1 = HWREG(SOC_PLLC_1_REGS + PLLC_PLLDIV1);
    PLLDiv2 = HWREG(SOC_PLLC_1_REGS + PLLC_PLLDIV2);
    PLLDiv3 = HWREG(SOC_PLLC_1_REGS + PLLC_PLLDIV3);

    // PLL1 ���ʱ��
    if(PLLCTL & 0x01)          // PLL ʹ��
    {
        PLLOut = OSCINFreq * ((PLLM & 0x1F) + 1);

        if(PLLPostDiv & (1 << 15))
        {
            PLLOut /= ((PLLPostDiv & 0x1F) + 1);
        }
    }
    else                       // PLL ��·
    {
        PLLOut = OSCINFreq;
    }

    pllcfg.PLL1_SYSCLK1 = (PLLDiv1 & (1 << 15)) ? PLLOut / ((PLLDiv1 & 0x1F) + 1) : PLLOut;
    pllcfg.PLL1_SYSCLK2 = (PLLDiv2 & (1 << 15)) ? PLLOut / ((PLLDiv2 & 0x1F) + 1) : PLLOut;
    pllcfg.PLL1_SYSCLK3 = (PLLDiv3 & (1 << 15)) ? PLLOut / ((PLLDiv3 & 0x1F) + 1) : PLLOut;

    // PLLC0
    PLLCTL = HWREG(SOC_PLLC_0_REGS + PLLC_PLLCTL);
    PLLPreDiv = HWREG(SOC_PLLC_0_REGS + PLLC_PREDIV);
    PLLM = HWREG(SOC_PLLC_0_REGS + PLLC_PLLM);
    PLLPostDiv = HWREG(SOC_PLLC_0_REGS + PLLC_POSTDIV);

    PLLDiv1 = HWREG(SOC_PLLC_0_REGS + PLLC_PLLDIV1);
    PLLDiv2 = HWREG(SOC_PLLC_0_REGS + PLLC_PLLDIV2);
    PLLDiv3 = HWREG(SOC_PLLC_0_REGS + PLLC_PLLDIV3);
    PLLDiv4 = HWREG(SOC_PLLC_0_REGS + PLLC_PLLDIV4);
    PLLDiv6 = HWREG(SOC_PLLC_0_REGS + PLLC_PLLDIV6);
    PLLDiv7 = HWREG(SOC_PLLC_0_REGS + PLLC_PLLDIV7);

    // PLL0 ���ʱ��
    if(PLLCTL & 0x01)          // PLL ʹ��
    {
        if(PLLPreDiv & (1 << 15))
        {
            PLLOut = OSCINFreq / ((PLLPreDiv & 0x1F) + 1);
        }
        else
        {
            PLLOut = OSCINFreq;
        }

        PLLOut *= ((PLLM & 0x1F) + 1);

        if(PLLPostDiv & (1 << 15))
        {
            PLLOut /= ((PLLPostDiv & 0x1F) + 1);
        }
    }
    else                       // PLL ��·
    {
        if(PLLCTL & (1 << 9))  // ʹ�� PLL1_SYSCLK3 ��Ϊʱ��Դ
        {
            PLLOut = pllcfg.PLL1_SYSCLK3;
        }
        else                   // ʹ���ⲿʱ��Դ
        {
            PLLOut = OSCINFreq;
        }
    }

    pllcfg.PLL0_AUXCLK = OSCINFreq;

    pllcfg.PLL0_SYSCLK1 = (PLLDiv1 & (1 << 15)) ? PLLOut / ((PLLDiv1 & 0x1F) + 1) : PLLOut;
    pllcfg.PLL0_SYSCLK2 = (PLLDiv2 & (1 << 15)) ? PLLOut / ((PLLDiv2 & 0x1F) + 1) : PLLOut;
    pllcfg.PLL0_SYSCLK3 = (PLLDiv3 & (1 << 15)) ? PLLOut / ((PLLDiv3 & 0x1F) + 1) : PLLOut;
    pllcfg.PLL0_SYSCLK4 = (PLLDiv4 & (1 << 15)) ? PLLOut / ((PLLDiv4 & 0x1F) + 1) : PLLOut;
    pllcfg.PLL0_SYSCLK6 = (PLLDiv6 & (1 << 15)) ? PLLOut / ((PLLDiv6 & 0x1F) + 1) : PLLOut;
    pllcfg.PLL0_SYSCLK7 = (PLLDiv7 & (1 << 15)) ? PLLOut / ((PLLDiv7 & 0x1F) + 1) : PLLOut;
}

/****************************************************************************/
/*                                                                          */
/*              PLL ʱ�����                                                */
/*                                                                          */
/****************************************************************************/
void PLLClockOut()
{
    // GPIO6[14] CLKOUT
    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(13)) = (HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(13)) & (~(SYSCFG_PINMUX13_PINMUX13_7_4))) |
                                                    (SYSCFG_PINMUX13_PINMUX13_7_4_OBSCLK0 << SYSCFG_PINMUX13_PINMUX13_7_4_SHIFT);



}
