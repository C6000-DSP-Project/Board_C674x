//../Periph/PRU/pasm -c -CPRU0_Code ../Periph/PRU/PRU_GPIO_LED.p ../Periph/PRU_GPIO_LED

.origin 0
.entrypoint GPIO_LED

#include "PRU_GPIO_LED.hp"

#define	GPIO_BASE			0x01E26000		// the base address of GPIO registers
#define GPIO01_DIRECT		0x10			// the offset address of DIR01 register
#define	GPIO0_SETDATAOUT	0x18			// the offset address of SET_DATA01  registers
#define	GPIO0_CLEARDATAOUT	0x1C			// the offset address of CLR_DATA01 registers


// GPIO0_0  GPIO0_1  GPIO0_2  GPIO0_5
#define GPIO_LED_VALUE			((1 << 0) | (1 << 1) | (1 << 2) | (1 << 5))


////////////////////////////////������//////////////////////////////	
GPIO_LED:
						
	MOV		r10, 10
LOOP1:
	CALL	GPIO_LED_ON
	CALL	DELAY_500MS		// Call to delay some time

	CALL	GPIO_LED_OFF
	CALL	DELAY_500MS		// Call to delay some time

	SUB		r10, r10, 1				// 1 CPI
	QBNE	LOOP1, r10, 0		// 1 CPI��r10 != 0 ����ת�� LOOP1 ��ֱ�� r10 = 0 ˳��ִ��
    HALT

///////////////////////////////�ӳ���///////////////////////////////	

// gpio led on funcation
GPIO_LED_ON:
	MOV 	r2, GPIO_LED_VALUE
	MOV 	r3, GPIO_BASE + GPIO0_SETDATAOUT
	SBBO	r2, r3, 0, 4
	RET

// gpio led off funcation
GPIO_LED_OFF:
	MOV 	r2, GPIO_LED_VALUE
	MOV 	r3, GPIO_BASE + GPIO0_CLEARDATAOUT
	SBBO	r2, r3, 0, 4
	RET

// ��ʱ�ӳ������1������ָ���1��������תָ��
// PRU CORE CLK: 228MHz, cycle:8.77ns
// 1s��ʱ:    228000000/2 = 114000000	
// 500ms��ʱ: 114000000/2 =  57000000	
DELAY_500MS:
	MOV		r0, 57000000			
LOOP_DELAY:							
	SUB		r0, r0, 1				// 1 CPI
	QBNE	LOOP_DELAY, r0, 0		// 1 CPI��r0 != 0 ����ת�� LOOP_DELAY ��ֱ�� r0 = 0 ˳��ִ��
	RET	
	