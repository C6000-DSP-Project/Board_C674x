/****************************************************************************/
/*                                                                          */
/*    新核科技(广州)有限公司                                                */
/*                                                                          */
/*    Copyright (C) 2022 CoreKernel Technology (Guangzhou) Co., Ltd         */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/*    EMAC MII 百兆以太网                                                   */
/*                                                                          */
/*    2014年07月02日                                                        */
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
// MAC 地址
unsigned char MacAddr[8];

// 连接状态
char *LinkStr[] = {"No Link", "10Mb/s Half Duplex", "10Mb/s Full Duplex", "100Mb/s Half Duplex", "100Mb/s Full Duplex"};

/****************************************************************************/
/*                                                                          */
/*              回调函数 EMAC 初始化                                        */
/*                                                                          */
/****************************************************************************/
void MDIOGPIOBankPinMuxSet()
{
    #define PINMUX4_MDIO_ENABLE      (SYSCFG_PINMUX4_PINMUX4_7_4_MDIO_D << SYSCFG_PINMUX4_PINMUX4_7_4_SHIFT) | \
                                      (SYSCFG_PINMUX4_PINMUX4_3_0_MDIO_CLK << SYSCFG_PINMUX4_PINMUX4_3_0_SHIFT)
    unsigned int savePinMux = 0;

    savePinMux = HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(4)) & \
                       ~(SYSCFG_PINMUX4_PINMUX4_3_0 | \
                         SYSCFG_PINMUX4_PINMUX4_7_4);

    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(4)) = (PINMUX4_MDIO_ENABLE | savePinMux);
}

void EMACMIIGPIOBankPinMuxSet()
{
    #define PINMUX2_EMAC_ENABLE      (SYSCFG_PINMUX2_PINMUX2_31_28_MII_TXD0 << SYSCFG_PINMUX2_PINMUX2_31_28_SHIFT) | \
                                      (SYSCFG_PINMUX2_PINMUX2_27_24_MII_TXD1 << SYSCFG_PINMUX2_PINMUX2_27_24_SHIFT) | \
                                      (SYSCFG_PINMUX2_PINMUX2_23_20_MII_TXD2 << SYSCFG_PINMUX2_PINMUX2_23_20_SHIFT) | \
                                      (SYSCFG_PINMUX2_PINMUX2_19_16_MII_TXD3 << SYSCFG_PINMUX2_PINMUX2_19_16_SHIFT) | \
                                      (SYSCFG_PINMUX2_PINMUX2_15_12_MII_COL << SYSCFG_PINMUX2_PINMUX2_15_12_SHIFT) | \
                                      (SYSCFG_PINMUX2_PINMUX2_11_8_MII_TXCLK << SYSCFG_PINMUX2_PINMUX2_11_8_SHIFT) | \
                                      (SYSCFG_PINMUX2_PINMUX2_7_4_MII_TXEN << SYSCFG_PINMUX2_PINMUX2_7_4_SHIFT)


    #define PINMUX3_EMAC_ENABLE      (SYSCFG_PINMUX3_PINMUX3_31_28_MII_RXD0 << SYSCFG_PINMUX3_PINMUX3_31_28_SHIFT) | \
                                      (SYSCFG_PINMUX3_PINMUX3_27_24_MII_RXD1 << SYSCFG_PINMUX3_PINMUX3_27_24_SHIFT) | \
                                      (SYSCFG_PINMUX3_PINMUX3_23_20_MII_RXD2 << SYSCFG_PINMUX3_PINMUX3_23_20_SHIFT) | \
                                      (SYSCFG_PINMUX3_PINMUX3_19_16_MII_RXD3 << SYSCFG_PINMUX3_PINMUX3_19_16_SHIFT) | \
                                      (SYSCFG_PINMUX3_PINMUX3_15_12_MII_CRS << SYSCFG_PINMUX3_PINMUX3_15_12_SHIFT) | \
                                      (SYSCFG_PINMUX3_PINMUX3_11_8_MII_RXER << SYSCFG_PINMUX3_PINMUX3_11_8_SHIFT) | \
                                      (SYSCFG_PINMUX3_PINMUX3_7_4_MII_RXDV << SYSCFG_PINMUX3_PINMUX3_7_4_SHIFT) | \
                                      (SYSCFG_PINMUX3_PINMUX3_3_0_MII_RXCLK << SYSCFG_PINMUX3_PINMUX3_3_0_SHIFT)

    unsigned int savePinMux = 0;

    savePinMux =  HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(2)) & \
                        ~(SYSCFG_PINMUX2_PINMUX2_31_28 | \
                          SYSCFG_PINMUX2_PINMUX2_27_24 | \
                          SYSCFG_PINMUX2_PINMUX2_23_20 | \
                          SYSCFG_PINMUX2_PINMUX2_19_16 | \
                          SYSCFG_PINMUX2_PINMUX2_15_12 | \
                          SYSCFG_PINMUX2_PINMUX2_11_8 | \
                          SYSCFG_PINMUX2_PINMUX2_7_4);

    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(2)) = (PINMUX2_EMAC_ENABLE | savePinMux);

    savePinMux = HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(3)) & \
                       ~(SYSCFG_PINMUX3_PINMUX3_31_28 | \
                         SYSCFG_PINMUX3_PINMUX3_27_24 | \
                         SYSCFG_PINMUX3_PINMUX3_23_20 | \
                         SYSCFG_PINMUX3_PINMUX3_19_16 | \
                         SYSCFG_PINMUX3_PINMUX3_15_12 | \
                         SYSCFG_PINMUX3_PINMUX3_11_8 | \
                         SYSCFG_PINMUX3_PINMUX3_7_4 | \
                         SYSCFG_PINMUX3_PINMUX3_3_0);

    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(3)) = (PINMUX3_EMAC_ENABLE | savePinMux);

    // 使能 MII 模式
    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_CFGCHIP3) &= ~(SYSCFG_CFGCHIP3_RMII_SEL);
}

void EMACRMIIGPIOBankPinMuxSet()
{
    #define PINMUX14_EMAC_ENABLE    (SYSCFG_PINMUX14_PINMUX14_31_28_RMII_RXER << SYSCFG_PINMUX14_PINMUX14_31_28_SHIFT) | \
                                     (SYSCFG_PINMUX14_PINMUX14_27_24_RMII_RXD0 << SYSCFG_PINMUX14_PINMUX14_27_24_SHIFT) | \
                                     (SYSCFG_PINMUX14_PINMUX14_23_20_RMII_RXD1 << SYSCFG_PINMUX14_PINMUX14_23_20_SHIFT) | \
                                     (SYSCFG_PINMUX14_PINMUX14_19_16_RMII_TXEN << SYSCFG_PINMUX14_PINMUX14_19_16_SHIFT) | \
                                     (SYSCFG_PINMUX14_PINMUX14_15_12_RMII_TXD0 << SYSCFG_PINMUX14_PINMUX14_15_12_SHIFT) | \
                                     (SYSCFG_PINMUX14_PINMUX14_11_8_RMII_TXD1 << SYSCFG_PINMUX14_PINMUX14_11_8_SHIFT)

    #define PINMUX15_EMAC_ENABLE    (SYSCFG_PINMUX15_PINMUX15_7_4_RMII_CRS_DV << SYSCFG_PINMUX15_PINMUX15_7_4_SHIFT) | \
                                     (SYSCFG_PINMUX15_PINMUX15_3_0_DEFAULT << SYSCFG_PINMUX15_PINMUX15_3_0_SHIFT)

    unsigned int savePinMux = 0;

    savePinMux = HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(14)) & \
                       ~(SYSCFG_PINMUX14_PINMUX14_31_28 | \
                         SYSCFG_PINMUX14_PINMUX14_27_24 | \
                         SYSCFG_PINMUX14_PINMUX14_23_20 | \
                         SYSCFG_PINMUX14_PINMUX14_19_16 | \
                         SYSCFG_PINMUX14_PINMUX14_15_12 | \
                         SYSCFG_PINMUX14_PINMUX14_11_8);

    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(14)) = (PINMUX14_EMAC_ENABLE | savePinMux);

    savePinMux = HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(15)) & \
                       ~(SYSCFG_PINMUX15_PINMUX15_7_4 | \
                         SYSCFG_PINMUX15_PINMUX15_3_0);

    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(15)) = (PINMUX15_EMAC_ENABLE | savePinMux);

    // 使能 RMII 模式
    HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_CFGCHIP3) &= SYSCFG_CFGCHIP3_RMII_SEL;
}

// 这个函数被 NSP 驱动调用 请勿修改函数名
void EMAC_initialize()
{
    // 使能外设
    PSCModuleControl(SOC_PSC_1_REGS, HW_PSC_EMAC, PSC_POWERDOMAIN_ALWAYS_ON, PSC_MDCTL_NEXT_ENABLE);

    // 使能 MII 模式
    MDIOGPIOBankPinMuxSet();
    EMACMIIGPIOBankPinMuxSet();
}

/****************************************************************************/
/*                                                                          */
/*              回调函数 获取 MAC 地址                                      */
/*                                                                          */
/****************************************************************************/
// 这个函数被 NSP 驱动调用 请勿修改函数名
void EMAC_getConfig(unsigned char *pMacAddr)
{
    // 根据芯片 ID 生成 MAC 地址
    MacAddr[0] = 0x00;
    MacAddr[1] = (*(volatile unsigned int *)(0x01C14008) & 0x0000FF00) >> 8;
    MacAddr[2] = (*(volatile unsigned int *)(0x01C14008) & 0x000000FF) >> 0;
    MacAddr[3] = (*(volatile unsigned int *)(0x01C1400C) & 0x0000FF00) >> 8;
    MacAddr[4] = (*(volatile unsigned int *)(0x01C1400C) & 0x000000FF) >> 0;
    MacAddr[5] = (*(volatile unsigned int *)(0x01C14010) & 0x000000FF) >> 0;
    UARTprintf("Using MAC Address: %02X-%02X-%02X-%02X-%02X-%02X\n",
                MacAddr[0], MacAddr[1], MacAddr[2], MacAddr[3], MacAddr[4], MacAddr[5]);

    // 传递 MAC 地址
    memcpy(pMacAddr, MacAddr, 6);
}

/****************************************************************************/
/*                                                                          */
/*              回调函数 设置 MAC 地址                                      */
/*                                                                          */
/****************************************************************************/
// 这个函数被 NSP 驱动调用 请勿修改函数名
void EMAC_setConfig(unsigned char *pMacAddr)
{
    memcpy(MacAddr, pMacAddr, 6);
    UARTprintf("Setting MAC Addr to: %02x-%02x-%02x-%02x-%02x-%02x\n",
                MacAddr[0], MacAddr[1], MacAddr[2], MacAddr[3], MacAddr[4], MacAddr[5]);
}

/****************************************************************************/
/*                                                                          */
/*              回调函数 获取连接状态                                       */
/*                                                                          */
/****************************************************************************/
// 这个函数被 NSP 驱动调用 请勿修改函数名
void EMAC_linkStatus(unsigned int phy, unsigned int linkStatus)
{
    UARTprintf("Link Status: %s on PHY %d\n", LinkStr[linkStatus], phy);
}
