/****************************************************************************/
/*                                                                          */
/*    新核科技(广州)有限公司                                                */
/*                                                                          */
/*    Copyright (C) 2022 CoreKernel Technology (Guangzhou) Co., Ltd         */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/*    内存测试                                                              */
/*                                                                          */
/*    2014年07月03日                                                        */
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

#define MAX_FILL_FAIL_COUNT 		(10)
#define MAX_ADDRESS_FAIL_COUNT 		(10)
#define MAX_BITWALKING_FAIL_COUNT 	(2)

#define BIT_PATTERN_FILLING_TEST 	1
#define ADDRESS_TEST 				1
#define BIT_WALKING_TEST 			1

#define UARTprintf_DETAILS 			1

unsigned long long ulDataPatternTable[] = {
	0x0000000000000000,
	0xffffffffffffffff,
	0xaaaaaaaaaaaaaaaa,
	0x5555555555555555,
	0xcccccccccccccccc,
	0x3333333333333333,
	0xf0f0f0f0f0f0f0f0,
	0x0f0f0f0f0f0f0f0f,
	0xff00ff00ff00ff00,
	0x00ff00ff00ff00ff
};

/*
This function fills the patterns into memory and reads back and verification
*/
unsigned int MEM_FillTest(unsigned int uiStartAddress,
                        unsigned int uiCount,
                        unsigned long long ulBitPattern,
                        unsigned int uiStep)
{
    unsigned int i, uiFailCount=0;
    volatile unsigned long long *ulpAddressPointer;
    volatile unsigned long long ulReadBack;

    /* Write the pattern */
	ulpAddressPointer = (unsigned long long *)uiStartAddress;
	for(i=0; i<uiCount; i++)
	{
        *ulpAddressPointer = ulBitPattern;	  /* fill with pattern */
        ulpAddressPointer += (uiStep);
    }

	ulpAddressPointer = (unsigned long long *)uiStartAddress;
	for(i=0; i<uiCount; i++)
	{
        ulReadBack = *ulpAddressPointer;
        if ( ulReadBack!= ulBitPattern)      /* verify pattern */
        {
			UARTprintf("Memory Test fails at 0x%8x, Write 0x%16llx, Readback 0x%16llx\n", ulpAddressPointer, ulBitPattern, ulReadBack);
            uiFailCount++;
			if(uiFailCount>=MAX_FILL_FAIL_COUNT)
				return uiFailCount;
        }
        ulpAddressPointer += (uiStep);
    }
	return uiFailCount;
}

/*
this function write the address to corresponding memory unit and readback for verification
*/
unsigned int MEM_AddrTest(unsigned int uiStartAddress,
						unsigned int uiCount,
                        int iStep)
{
    unsigned int i, uiFailCount=0;
    volatile unsigned long long *ulpAddressPointer;
    volatile unsigned long long ulReadBack;

	ulpAddressPointer = (unsigned long long *)uiStartAddress;
	for(i=0; i<uiCount; i++)
	{
		/* fill with address value */
        *ulpAddressPointer = _itoll(((unsigned int)ulpAddressPointer)+4,
        	(unsigned int)ulpAddressPointer);
        ulpAddressPointer += (iStep);
    }

	ulpAddressPointer = (unsigned long long *)uiStartAddress;
	for(i=0; i<uiCount; i++)
	{
        ulReadBack = *ulpAddressPointer;
        if ( ulReadBack != _itoll(((unsigned int)ulpAddressPointer)+4,
        	(unsigned int)ulpAddressPointer)) /* verify data */
        {
			UARTprintf("Memory Test fails at 0x%8x, Write 0x%16llx, Readback 0x%16llx\n", ulpAddressPointer, _itoll(((unsigned int)ulpAddressPointer)+4, (unsigned int)ulpAddressPointer), ulReadBack);
            uiFailCount++;
			if(uiFailCount>=MAX_ADDRESS_FAIL_COUNT)
				return uiFailCount;
        }
        ulpAddressPointer += (iStep);
    }
    return uiFailCount;              /* show no error */
}

unsigned int MEM_Bit_Walking(unsigned int uiStartAddress,
						unsigned int uiCount,
                        unsigned int uiStep)
{
    unsigned int uiFailCount=0;
    unsigned int j;
    unsigned int uiBitMask=1;
    for (j = 0; j < 32; j++)
    {
    	if(uiCount*8>1024*1024)
	  		UARTprintf("Memory Bit Walking at 0x%8x with pattern 0x%16llx\n",uiStartAddress, _itoll(uiBitMask, uiBitMask));
        if(MEM_FillTest(uiStartAddress, uiCount, _itoll(uiBitMask, uiBitMask), uiStep))
        	uiFailCount++;
    	if(uiCount*8>1024*1024)
	  		UARTprintf("Memory Bit Walking at 0x%8x with pattern 0x%16llx\n",uiStartAddress, _itoll(~uiBitMask, ~uiBitMask));
        if(MEM_FillTest(uiStartAddress, uiCount, _itoll(~uiBitMask, ~uiBitMask), uiStep))
        	uiFailCount++;

		if(uiFailCount>= MAX_BITWALKING_FAIL_COUNT)
			return uiFailCount;

        uiBitMask <<= 1;
    }
	return uiFailCount;
}

int DSP_core_MEM_Test(unsigned int uiStartAddress, unsigned int uiStopAddress, unsigned int uiStep)
{
    unsigned int uiCount, uiFailCount=0, uiTotalFailCount=0;
    int j;

    uiCount = ((uiStopAddress - uiStartAddress)/8)/uiStep;

#if BIT_PATTERN_FILLING_TEST
    // Fill with values from pattern table
    for (j = 0; j < sizeof(ulDataPatternTable)/8; j++)
    {
        uiFailCount = MEM_FillTest(uiStartAddress, uiCount, ulDataPatternTable[j], uiStep);
	    if (uiFailCount)
	    {
	    	UARTprintf("!!!Failed Memory Fill Test at %d Units with pattern 0x%16llx\n",uiFailCount, ulDataPatternTable[j]);
	        uiTotalFailCount+=uiFailCount;
	    }
		else
		   	UARTprintf("Passed Memory Fill Test from 0x%8x to 0x%8x with pattern 0x%16llx\n",uiStartAddress,uiStopAddress, ulDataPatternTable[j]);
    }

#endif
#if ADDRESS_TEST
    // Test the address range from low to high
    uiFailCount = MEM_AddrTest(uiStartAddress, uiCount, uiStep);
    if (uiFailCount)
    {
    	UARTprintf("!!!Failed Memory Address Test at %d units\n",uiFailCount);
        uiTotalFailCount+=uiFailCount;
    }
	else
	   	UARTprintf("Passed Memory Address Test from 0x%8x to 0x%8x\n",uiStartAddress,uiStopAddress);

#endif
#if BIT_WALKING_TEST
    // Do walking ones and zeroes test
    uiFailCount = MEM_Bit_Walking(uiStartAddress,uiCount, uiStep);
    if (uiFailCount)
    {
	   	UARTprintf("!!!Failed Memory Bit Walking at %d units\n",uiFailCount);
        uiTotalFailCount+=uiFailCount;
    }
	else
	  	UARTprintf("Passed Memory Bit Walking from 0x%8x to 0x%8x\n",uiStartAddress,uiStopAddress);
#endif
  	return uiTotalFailCount;
}
