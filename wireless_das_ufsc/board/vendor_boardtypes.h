/**
 * @file
 *
 * @brief Atmel vendor specific board types for ATmega1281
 *
 * This header file is an example implementation for customer specfic board types
 * based on ATmega1281.
 *
 * $Id: vendor_boardtypes.h 21936 2010-04-28 09:23:25Z sschneid $
 */

/* Prevent double inclusion */
#ifndef VENDOR_BOARDTYPES_H
#define VENDOR_BOARDTYPES_H

#include <platform.h>

/* Board types */
#define KITRFA1					(0x31)
#define MINIKIT_ATXMEGA_V3R7	(0x32)
#define MINIKIT2				(0x33)
//#define RZ600					(0x34)
#define RZ600_SAM4S_XPLAINED	(0x35)
#define STK500					(0x36)
#define STK500_RZ600			(0x37)
#define STK525_RZ600			(0x38)
#define STK600_QT600			(0x39)
#define XPLD_XMEGAA1			(0x3A)
//#define SAM4S_XPLAINED			(0x3B)
//#define SAM4S_EK				(0x3C)
//#define SAM4L_EK				(0x3D)
#define SAMD20_PRO				(0x3E)

#define WM100					(0x40)
#define WM100_DUINO				(0x41)
#define WM200					(0x42)
#define WM400					(0x43)

#define WM400_BBBS				(0x60)

#if BOARD_TYPE == WM100
	#include "WM100/inc/wm100.h"
#elif BOARD_TYPE == WM100_DUINO
	#if PLATFORM == xmega
		#include "WM100_DUINO/inc/wm100_duino_cop.h"		// Include Co-Processor
	#else
		#include "WM100_DUINO/inc/wm100_duino.h"			// Include Main Processor
	#endif
#elif BOARD_TYPE == WM200
	#include "WM200/inc/wm200.h"
#elif BOARD_TYPE == WM400
	#include "WM400/inc/wm400.h"
#elif BOARD_TYPE == WM400_BBBS
	#include "WM400_BBBS/inc/wm400_bbbs.h"
#elif BOARD_TYPE == KITRFA1
	#include "KITRFA1/inc/KITRFA1.h"
#elif BOARD_TYPE == MINIKIT2
	#include "MINIKIT2/inc/MINIKIT2.h"
#elif BOARD_TYPE == SAM4S_XPLAINED
	#include "SAM4S_XPLAINED/inc/sam4s_xplained.h"
#elif BOARD_TYPE == SAMD20_PRO
	#include "SAMD20_PRO/inc/samd20_xplained_pro.h"
#elif BOARD_TYPE == SAM4S_EK
	#include "SAM4S_EK/inc/sam4s_ek.h"
#elif BOARD_TYPE == SAM4L_EK
	#include "SAM4L_EK/inc/sam4l_ek.h"
#endif


#endif  /* VENDOR_BOARDTYPES_H */
/* EOF */
