/****************************************************************************/
/*                                                                          */
/*    新核科技(广州)有限公司                                                */
/*                                                                          */
/*    Copyright (C) 2022 CoreKernel Technology (Guangzhou) Co., Ltd         */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/*    SATA 设备枚举测试                                                     */
/*                                                                          */
/*    2014年03月19日                                                        */
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
// 该例程仅用于 SATA 设备枚举测试
// 详情请参阅 TMS320C674x OMAP-L1x Processor Serial ATA (SATA) Controller User's Guide

#include "hw_types.h"
#include "hw_syscfg1_C6748.h"
#include "soc_C6748.h"

#include "hw_psc_C6748.h"
#include "psc.h"
#include "uartStdio.h"
#include "sata.h"

#include <stdio.h>

/****************************************************************************/
/*                                                                          */
/*              PSC 初始化                                                  */
/*                                                                          */
/****************************************************************************/
static int PSCModuleControlForce(unsigned int baseAdd, unsigned int moduleId, unsigned int powerDomain, unsigned int flags)
{
    volatile unsigned int timeout = 0xFFFFFF;
    int    retVal = 0;
    unsigned int    status = 0;

    HWREG(baseAdd +  PSC_MDCTL(moduleId)) = (flags & 0x8000001Fu);

    if (powerDomain == 0)
    {
        HWREG(baseAdd + PSC_PTCMD) = PSC_PTCMD_GO0;
    }
    else
    {
        HWREG(baseAdd + PSC_PTCMD) = PSC_PTCMD_GO1;
    }

    if (powerDomain == 0)
    {
        do {
            status = HWREG(baseAdd + PSC_PTSTAT) & PSC_PTSTAT_GOSTAT0;
        } while (status && timeout--);
    }
    else
    {
        do {
            status = HWREG(baseAdd + PSC_PTSTAT) & PSC_PTSTAT_GOSTAT1;
        } while (status && timeout--);
    }

    if (timeout != 0)
    {
        timeout = 0xFFFFFF;
        status = flags & PSC_MDCTL_NEXT;
        do {
            timeout--;
        } while(timeout &&
                (HWREG(baseAdd + PSC_MDSTAT(moduleId)) & PSC_MDSTAT_STATE) != status);
    }

    if (timeout == 0)
    {
        retVal = -1;
    }

    return retVal;
}

static void PSCInit(void)
{
	// 使能 SATA 外设
    PSCModuleControlForce(SOC_PSC_1_REGS, HW_PSC_SATA, PSC_POWERDOMAIN_ALWAYS_ON, PSC_MDCTL_FORCE | PSC_MDCTL_NEXT_ENABLE);

    // 使能时钟
    HWREG(SOC_SYSCFG_1_REGS + SYSCFG1_PWRDN) = ~SYSCFG1_PWRDN_SATACLK_PWRDN;
}

/****************************************************************************/
/*                                                                          */
/*              SATA 测试                                                   */
/*                                                                          */
/****************************************************************************/
void SATATest()
{
	// 外设使能配置
	PSCInit();

	// 初始化配置
	swCtrlFeatures.capSMPS = 1;
	swCtrlFeatures.capSSS = 1;
	swCtrlFeatures.piPi = 1;
	swCtrlFeatures.p0cmdEsp = 0;
	swCtrlFeatures.p0cmdCpd = 1;
	swCtrlFeatures.p0cmdMpsp = 1;
	swCtrlFeatures.p0cmdHpcp = 1;

	// HBA 复位
	invokeHBAReset();

	// 设置 HBA 为空闲
	if(placeHbaInIdle())
	{
		invokeHBAReset();
	}

	// 固件初始化
	performFirmwareInit();

	if (!spinUpDeviceAndWaitForInitToComplete())
	{
		UARTprintf("\r\nDisk spin-up success. \r\n");
	}
	else
	{
		UARTprintf("\r\nDisk spin-up failed. \r\n");
	}
}
