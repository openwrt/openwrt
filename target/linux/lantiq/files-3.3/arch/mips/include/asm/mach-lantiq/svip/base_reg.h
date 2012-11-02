/******************************************************************************

  Copyright (c) 2007
  Infineon Technologies AG
  St. Martin Strasse 53; 81669 Munich, Germany

  Any use of this Software is subject to the conclusion of a respective
  License Agreement. Without such a License Agreement no rights to the
  Software are granted.

 ******************************************************************************/

#ifndef __BASE_REG_H
#define __BASE_REG_H

#ifndef KSEG1
#define KSEG1 0xA0000000
#endif

#define LTQ_EBU_SEG1_BASE		(KSEG1 + 0x10000000)
#define LTQ_EBU_SEG2_BASE		(KSEG1 + 0x11000000)
#define LTQ_EBU_SEG3_BASE		(KSEG1 + 0x12000000)
#define LTQ_EBU_SEG4_BASE		(KSEG1 + 0x13000000)

#define LTQ_ASC0_BASE			(KSEG1 + 0x14100100)
#define LTQ_ASC1_BASE			(KSEG1 + 0x14100200)

#define LTQ_SSC0_BASE			(0x14100300)
#define LTQ_SSC1_BASE			(0x14100400)

#define LTQ_PORT_P0_BASE		(KSEG1 + 0x14100600)
#define LTQ_PORT_P1_BASE		(KSEG1 + 0x14108100)
#define LTQ_PORT_P2_BASE		(KSEG1 + 0x14100800)
#define LTQ_PORT_P3_BASE		(KSEG1 + 0x14100900)
#define LTQ_PORT_P4_BASE		(KSEG1 + 0x1E000400)

#define LTQ_EBU_BASE			(KSEG1 + 0x14102000)
#define LTQ_DMA_BASE			(KSEG1 + 0x14104000)

#define LTQ_ICU0_IM3_IM2_BASE		(KSEG1 + 0x1E016000)
#define LTQ_ICU0_IM5_IM4_IM1_IM0_BASE	(KSEG1 + 0x14106000)

#define LTQ_ES_BASE			(KSEG1 + 0x18000000)

#define LTQ_SYS0_BASE			(KSEG1 + 0x1C000000)
#define LTQ_SYS1_BASE			(KSEG1 + 0x1C000800)
#define LTQ_SYS2_BASE			(KSEG1 + 0x1E400000)

#define LTQ_L2_SPRAM_BASE		(KSEG1 + 0x1F1E8000)

#define LTQ_SWINT_BASE			(KSEG1 + 0x1E000100)
#define LTQ_MBS_BASE			(KSEG1 + 0x1E000200)

#define LTQ_STATUS_BASE			(KSEG1 + 0x1E000500)

#endif
