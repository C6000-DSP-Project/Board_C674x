/****************************************************************************/
/*                                                                          */
/*    �º˿Ƽ�(����)���޹�˾                                                */
/*                                                                          */
/*    Copyright (C) 2022 CoreKernel Technology (Guangzhou) Co., Ltd         */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/*    PRU                                                                   */
/*                                                                          */
/*    2015��12��07��                                                        */
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
/*
 *  CCS �м���Ԥ����������빤��ʱ���Զ����� PRU ��������
 * 	���� xx.h �ļ�����ʹ��PRU��C��������
 */
#include <App.h>

#include "pru.h"
#include "PRU_GPIO_LED_bin.h"

void PruLEDTest(void)
{
	UARTprintf("Starting example.\r\n");

	PruDisable(1);

	UARTprintf("\tINFO: Loading example.\r\n");

	PruEnable (1);

	PruLoad(1, (unsigned int*)PRU0_Code, (sizeof(PRU0_Code)/sizeof(unsigned int)));

	UARTprintf("\tINFO: Executing example.\r\n");
	PruRun(1);
}
