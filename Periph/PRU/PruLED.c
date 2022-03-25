/****************************************************************************/
/*                                                                          */
/*    新核科技(广州)有限公司                                                */
/*                                                                          */
/*    Copyright (C) 2022 CoreKernel Technology (Guangzhou) Co., Ltd         */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/*    PRU                                                                   */
/*                                                                          */
/*    2015年12月07日                                                        */
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
/*
 *  CCS 中加入预编译命令，编译工程时会自动调用 PRU 编译器，
 * 	生成 xx.h 文件，供使用PRU的C函数调用
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
