.syntax unified
.cpu cortex-m4
.thumb
 
.global HardFault_Handler
.extern HardFault_Handler_C
 
HardFault_Handler:
	TST		LR, #4
	ITE		EQ
	MRSEQ	R0, MSP
	MRSNE	R0, PSP
	B		HardFault_Handler_C
