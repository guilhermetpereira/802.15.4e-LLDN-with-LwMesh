/*
 * conf_resources.h
 *
 * This manage the memory pool resources shared by services and application. The user
 * must allocate resources dynamic from BMM. The user can use QMM for passing messages
 * or any other data type.
 *
 * The recommendation is use half SRAM.
 *
 * Created: 02/02/2013 19:50:05
 *  Author: nando
 */ 

#ifndef CONF_RESOURCES_H_
#define CONF_RESOURCES_H_

#include <interrupt.h>

#define				ENTER_CRITICAL_REGION()		irqflags_t flags; flags = cpu_irq_save()
#define				LEAVE_CRITICAL_REGION()		cpu_irq_restore(flags)

#define				SMALL_BUFFER_SIZE			64
#define				TOTAL_NUMBER_OF_SMALL_BUFS	64		// 4k

#define				LARGE_BUFFER_SIZE			256
#define				TOTAL_NUMBER_OF_LARGE_BUFS	16		// 4k
														// 8k
#define				TOTAL_NUMBER_OF_BUFS		(TOTAL_NUMBER_OF_SMALL_BUFS + TOTAL_NUMBER_OF_LARGE_BUFS)

#define				RING_BUFFER_SIZE			250

#endif /* CONF_RESOURCES_H_ */