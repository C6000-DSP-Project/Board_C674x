/****************************************************************************/
/*                                                                          */
/*    新核科技(广州)有限公司                                                */
/*                                                                          */
/*    Copyright (C) 2022 CoreKernel Technology (Guangzhou) Co., Ltd         */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/*    LED 点阵                                                              */
/*                                                                          */
/*    2022年03月28日                                                        */
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

#include "PRULoader.h"
#include "PRU_Code.h"
#include "PRU_Data.h"

/****************************************************************************/
/*                                                                          */
/*              初始化                                                      */
/*                                                                          */
/****************************************************************************/
void LEDMatrixInit()
{
    /* 使用 RISC 核心 PRU 控制 */
    // 加载并运行 PRU 程序
    PRUEnable(PRU1);
    PRULoad(PRU1, (unsigned int*)PRU_Code, (sizeof(PRU_Code) / sizeof(unsigned int)), (unsigned int*)PRU_Data, (sizeof(PRU_Data) / sizeof(unsigned int)));
    PRURun(PRU1);
}
