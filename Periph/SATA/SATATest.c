/****************************************************************************/
/*                                                                          */
/*    �º˿Ƽ�(����)���޹�˾                                                */
/*                                                                          */
/*    Copyright (C) 2022 CoreKernel Technology (Guangzhou) Co., Ltd         */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/*    SATA �豸ö�ٲ���                                                     */
/*                                                                          */
/*    2014��03��19��                                                        */
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
// �����̽����� SATA �豸ö�ٲ���
// ��������� TMS320C674x OMAP-L1x Processor Serial ATA (SATA) Controller User's Guide

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
/*              PSC ��ʼ��                                                  */
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
	// ʹ�� SATA ����
    PSCModuleControlForce(SOC_PSC_1_REGS, HW_PSC_SATA, PSC_POWERDOMAIN_ALWAYS_ON, PSC_MDCTL_FORCE | PSC_MDCTL_NEXT_ENABLE);

    // ʹ��ʱ��
    HWREG(SOC_SYSCFG_1_REGS + SYSCFG1_PWRDN) = ~SYSCFG1_PWRDN_SATACLK_PWRDN;
}

/****************************************************************************/
/*                                                                          */
/*              SATA ����                                                   */
/*                                                                          */
/****************************************************************************/
void SATATest()
{
	// ����ʹ������
	PSCInit();

	// ��ʼ������
	swCtrlFeatures.capSMPS = 1;
	swCtrlFeatures.capSSS = 1;
	swCtrlFeatures.piPi = 1;
	swCtrlFeatures.p0cmdEsp = 0;
	swCtrlFeatures.p0cmdCpd = 1;
	swCtrlFeatures.p0cmdMpsp = 1;
	swCtrlFeatures.p0cmdHpcp = 1;

	// HBA ��λ
	invokeHBAReset();

	// ���� HBA Ϊ����
	if(placeHbaInIdle())
	{
		invokeHBAReset();
	}

	// �̼���ʼ��
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
