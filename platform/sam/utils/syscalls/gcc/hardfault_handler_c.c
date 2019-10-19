/**
 * \file
 *
 * Copyright (c) 2012-2013 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#include <board.h>

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/// @endcond

/*
http://blog.frankvh.com/

Hard Fault Handler Usage

The big thing the above handler gives you is the program counter, which is the
address where the processor was executing when the hard fault occurred. You can
then look at your listing file (or map file) to see what function and
instruction that was. Also useful is LR the Link Register, which contains the
return address of the last function call – it can show you where you came from
to get to this point.

A few tips.

The typical reason for a hardfault is actually a bus error (which was promoted
to a hard fault), because software tried to access an invalid region of memory.
There are a couple of common ways of doing this.
One is a bad pointer. It might be uninitialized, or not properly bounded (you
ran off the end of an array for example).
Another, more subtle, way of getting a bad pointer is by being sloppy with the
scope of variables. This is quite common with main() but can happen anywhere.
For example, you define a buffer or a structure as a local variable within
main(), but then you access that buffer or structure from some other function
(by passing a pointer to it). This is dangerous – you need to declare the
buffer or structure as “static” if you want to do that. Otherwise the buffer
or structure might be optimized away and not actually exist when that other
function tries to access it. If that structure contained a pointer for example,
that would become a bad pointer.

Imprecise Bus Fault

For the Cortex M3, an imprecise bus fault (as indicated by bit 10 in the CFSR
register) means that a write to an invalid address was attempted. If you look
at the program counter, the naughty naughty write is usually present in the 3
or so instructions leading up to the program counter address. Because of the
Cortex M3 write buffer system, the program counter might have advanced slightly
before the actual bus write took place, hence you need to look back slightly to
find the erroneous write.

*/

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"

// From Joseph Yiu, minor edits by FVH
// hard fault handler in C,
// with stack frame location as input parameter
// called from HardFault_Handler in file xxx.s
void HardFault_Handler_C(unsigned int * hardfault_args)
{
	static unsigned int stacked_r0;
	static unsigned int stacked_r1;
	static unsigned int stacked_r2;
	static unsigned int stacked_r3;
	static unsigned int stacked_r12;
	static unsigned int stacked_lr;
	static unsigned int stacked_pc;
	static unsigned int stacked_psr;
	
	stacked_r0 = ((unsigned long) hardfault_args[0]);
	stacked_r1 = ((unsigned long) hardfault_args[1]);
	stacked_r2 = ((unsigned long) hardfault_args[2]);
	stacked_r3 = ((unsigned long) hardfault_args[3]);
	
	stacked_r12 = ((unsigned long) hardfault_args[4]);
	stacked_lr = ((unsigned long) hardfault_args[5]);
	stacked_pc = ((unsigned long) hardfault_args[6]);
	stacked_psr = ((unsigned long) hardfault_args[7]);
	
	//printf ("\n\n[Hard fault handler - all numbers in hex]\n");
	//printf ("R0 = %x\n", stacked_r0);
	//printf ("R1 = %x\n", stacked_r1);
	//printf ("R2 = %x\n", stacked_r2);
	//printf ("R3 = %x\n", stacked_r3);
	//printf ("R12 = %x\n", stacked_r12);
	//printf ("LR [R14] = %x  subroutine call return address\n", stacked_lr);
	//printf ("PC [R15] = %x  program counter\n", stacked_pc);
	//printf ("PSR = %x\n", stacked_psr);
	//printf ("BFAR = %x\n", (*((volatile unsigned long *)(0xE000ED38))));
	//printf ("CFSR = %x\n", (*((volatile unsigned long *)(0xE000ED28))));
	//printf ("HFSR = %x\n", (*((volatile unsigned long *)(0xE000ED2C))));
	//printf ("DFSR = %x\n", (*((volatile unsigned long *)(0xE000ED30))));
	//printf ("AFSR = %x\n", (*((volatile unsigned long *)(0xE000ED3C))));
	//printf ("SCB_SHCSR = %x\n", SCB->SHCSR);
	//
	while(1);
}

#pragma GCC diagnostic pop

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/// @endcond
