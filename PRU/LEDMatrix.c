/****************************************************************************/
/*                                                                          */
/*    �º˿Ƽ�(����)���޹�˾                                                */
/*                                                                          */
/*    Copyright (C) 2022 CoreKernel Technology (Guangzhou) Co., Ltd         */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/*    LED ����                                                              */
/*                                                                          */
/*    2022��03��28��                                                        */
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

#include "PRULoader.h"
#include "PRU_Code.h"
#include "PRU_Data.h"

/****************************************************************************/
/*                                                                          */
/*              ��ʼ��                                                      */
/*                                                                          */
/****************************************************************************/
void LEDMatrixInit()
{
    /* ʹ�� RISC ���� PRU ���� */
    // ���ز����� PRU ����
    PRUEnable(PRU1);
    PRULoad(PRU1, (unsigned int*)PRU_Code, (sizeof(PRU_Code) / sizeof(unsigned int)), (unsigned int*)PRU_Data, (sizeof(PRU_Data) / sizeof(unsigned int)));
    PRURun(PRU1);
}
