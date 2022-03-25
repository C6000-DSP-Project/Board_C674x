/****************************************************************************/
/*                                                                          */
/*    新核科技(广州)有限公司                                                */
/*                                                                          */
/*    Copyright (C) 2022 CoreKernel Technology (Guangzhou) Co., Ltd         */
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
/*              异常处理                                                    */
/*                                                                          */
/****************************************************************************/
Void ExceptionHook()
{
	// 输出信息
	UARTprintf("Fatal ERROR! System must be RESET.\n");

	Exception_Status status;
	Exception_getLastStatus(&status);

	UARTprintf("-------------------------------------------------------------------\n");
	UARTprintf("Exception Information\n");
	UARTprintf("- Exception Flag Register                        0x%08x\n", status.efr);
	UARTprintf("- NMI Return Pointer Register                    0x%08x\n", status.nrp);
	UARTprintf("- NMI/Exception Task State Register              0x%08x\n", status.ntsr);
	UARTprintf("- Internal Exception Report Register             0x%08x\n", status.ierr);
	UARTprintf("- Exception Flag Register                        0x%08x\n",  status.efr);
	UARTprintf("- DSP Core Number(DNUM) Register                 0x%08x\n", DNUM);
	UARTprintf("- Timestamp Counter Low 32(TSCL) Register        0x%08x\n", TSCL);
	UARTprintf("- Timestamp Counter High 32(TSCH) Register       0x%08x\n", TSCH);
	UARTprintf("- Addressing Mode(AMR) Register                  0x%08x\n", AMR);
	UARTprintf("- Control Status(CSR) Register                   0x%08x\n", CSR);
	UARTprintf("- Interrupt Enable(IER) Register                 0x%08x\n", IER);
	UARTprintf("- Interrupt Service Table Pointer(ISTP) Register 0x%08x\n", ISTP);
	UARTprintf("- Interrupt Return Pointer(IRP) Register         0x%08x\n", IRP);
	UARTprintf("-------------------------------------------------------------------\n");
}

Void InternalExceptionHook()
{
	// 输出信息
	UARTprintf("Internal Fatal ERROR!\n");
}

Void ExternalExceptionHook()
{
	// 输出信息
	UARTprintf("External Fatal ERROR!\n");
}

#pragma NMI_INTERRUPT(NMIHook);
Void NMIHook()
{
	// 输出信息
	UARTprintf("NMI Event!\n");
}

Void ExceptionReturnHook()
{
	// 输出信息
//	UARTprintf("Please Reset!\n");
}
