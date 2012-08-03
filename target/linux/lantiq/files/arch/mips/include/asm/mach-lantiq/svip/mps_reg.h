/******************************************************************************

  Copyright (c) 2007
  Infineon Technologies AG
  St. Martin Strasse 53; 81669 Munich, Germany

  Any use of this Software is subject to the conclusion of a respective
  License Agreement. Without such a License Agreement no rights to the
  Software are granted.

 ******************************************************************************/

#ifndef __MPS_REG_H
#define __MPS_REG_H

#define mbs_r32(reg) ltq_r32(&mbs->reg)
#define mbs_w32(val, reg) ltq_w32(val, &mbs->reg)
#define mbs_w32_mask(clear, set, reg) ltq_w32_mask(clear, set, &mbs->reg)

/** MBS register structure */
struct svip_reg_mbs {
	unsigned long reserved0[4];
	unsigned long mbsr0; /* 0x0010 */
	unsigned long mbsr1; /* 0x0014 */
	unsigned long mbsr2; /* 0x0018 */
	unsigned long mbsr3; /* 0x001c */
	unsigned long mbsr4; /* 0x0020 */
	unsigned long mbsr5; /* 0x0024 */
	unsigned long mbsr6; /* 0x0028 */
	unsigned long mbsr7; /* 0x002c */
	unsigned long mbsr8; /* 0x0030 */
	unsigned long mbsr9; /* 0x0034 */
	unsigned long mbsr10; /* 0x0038 */
	unsigned long mbsr11; /* 0x003c */
	unsigned long mbsr12; /* 0x0040 */
	unsigned long mbsr13; /* 0x0044 */
	unsigned long mbsr14; /* 0x0048 */
	unsigned long mbsr15; /* 0x004c */
	unsigned long mbsr16; /* 0x0050 */
	unsigned long mbsr17; /* 0x0054 */
	unsigned long mbsr18; /* 0x0058 */
	unsigned long mbsr19; /* 0x005c */
	unsigned long mbsr20; /* 0x0060 */
	unsigned long mbsr21; /* 0x0064 */
	unsigned long mbsr22; /* 0x0068 */
	unsigned long mbsr23; /* 0x006c */
	unsigned long mbsr24; /* 0x0070 */
	unsigned long mbsr25; /* 0x0074 */
	unsigned long mbsr26; /* 0x0078 */
	unsigned long mbsr27; /* 0x007c */
	unsigned long mbsr28; /* 0x0080 */
};

/** MPS register structure */
struct svip_reg_mps {
	volatile unsigned long  mps_swirn0set;  /*  0x0000 */
	volatile unsigned long  mps_swirn0en;  /*  0x0004 */
	volatile unsigned long  mps_swirn0cr;  /*  0x0008 */
	volatile unsigned long  mps_swirn0icr;  /*  0x000C */
	volatile unsigned long  mps_swirn1set;  /*  0x0010 */
	volatile unsigned long  mps_swirn1en;  /*  0x0014 */
	volatile unsigned long  mps_swirn1cr;  /*  0x0018 */
	volatile unsigned long  mps_swirn1icr;  /*  0x001C */
	volatile unsigned long  mps_swirn2set;  /*  0x0020 */
	volatile unsigned long  mps_swirn2en;  /*  0x0024 */
	volatile unsigned long  mps_swirn2cr;  /*  0x0028 */
	volatile unsigned long  mps_swirn2icr;  /*  0x002C */
	volatile unsigned long  mps_swirn3set;  /*  0x0030 */
	volatile unsigned long  mps_swirn3en;  /*  0x0034 */
	volatile unsigned long  mps_swirn3cr;  /*  0x0038 */
	volatile unsigned long  mps_swirn3icr;  /*  0x003C */
	volatile unsigned long  mps_swirn4set;  /*  0x0040 */
	volatile unsigned long  mps_swirn4en;  /*  0x0044 */
	volatile unsigned long  mps_swirn4cr;  /*  0x0048 */
	volatile unsigned long  mps_swirn4icr;  /*  0x004C */
	volatile unsigned long  mps_swirn5set;  /*  0x0050 */
	volatile unsigned long  mps_swirn5en;  /*  0x0054 */
	volatile unsigned long  mps_swirn5cr;  /*  0x0058 */
	volatile unsigned long  mps_swirn5icr;  /*  0x005C */
	volatile unsigned long  mps_swirn6set;  /*  0x0060 */
	volatile unsigned long  mps_swirn6en;  /*  0x0064 */
	volatile unsigned long  mps_swirn6cr;  /*  0x0068 */
	volatile unsigned long  mps_swirn6icr;  /*  0x006C */
	volatile unsigned long  mps_swirn7set;  /*  0x0070 */
	volatile unsigned long  mps_swirn7en;  /*  0x0074 */
	volatile unsigned long  mps_swirn7cr;  /*  0x0078 */
	volatile unsigned long  mps_swirn7icr;  /*  0x007C */
	volatile unsigned long  mps_swirn8set;  /*  0x0080 */
	volatile unsigned long  mps_swirn8en;  /*  0x0084 */
	volatile unsigned long  mps_swirn8cr;  /*  0x0088 */
	volatile unsigned long  mps_swirn8icr;  /*  0x008C */
};

/* Software Interrupt */
#define IFX_MPS_SWIRN0SET   ((volatile unsigned int*)(LTQ_SWINT_BASE + 0x0000))
#define IFX_MPS_SWIRN0EN   ((volatile unsigned int*)(LTQ_SWINT_BASE + 0x0004))
#define IFX_MPS_SWIRN0CR   ((volatile unsigned int*)(LTQ_SWINT_BASE + 0x0008))
#define IFX_MPS_SWIRN0ICR   ((volatile unsigned int*)(LTQ_SWINT_BASE + 0x000C))
#define IFX_MPS_SWIRN1SET   ((volatile unsigned int*)(LTQ_SWINT_BASE + 0x0010))
#define IFX_MPS_SWIRN1EN   ((volatile unsigned int*)(LTQ_SWINT_BASE + 0x0014))
#define IFX_MPS_SWIRN1CR   ((volatile unsigned int*)(LTQ_SWINT_BASE + 0x0018))
#define IFX_MPS_SWIRN1ICR   ((volatile unsigned int*)(LTQ_SWINT_BASE + 0x001C))
#define IFX_MPS_SWIRN2SET   ((volatile unsigned int*)(LTQ_SWINT_BASE + 0x0020))
#define IFX_MPS_SWIRN2EN   ((volatile unsigned int*)(LTQ_SWINT_BASE + 0x0024))
#define IFX_MPS_SWIRN2CR   ((volatile unsigned int*)(LTQ_SWINT_BASE + 0x0028))
#define IFX_MPS_SWIRN2ICR   ((volatile unsigned int*)(LTQ_SWINT_BASE + 0x002C))
#define IFX_MPS_SWIRN3SET   ((volatile unsigned int*)(LTQ_SWINT_BASE + 0x0030))
#define IFX_MPS_SWIRN3EN   ((volatile unsigned int*)(LTQ_SWINT_BASE + 0x0034))
#define IFX_MPS_SWIRN3CR   ((volatile unsigned int*)(LTQ_SWINT_BASE + 0x0038))
#define IFX_MPS_SWIRN3ICR   ((volatile unsigned int*)(LTQ_SWINT_BASE + 0x003C))
#define IFX_MPS_SWIRN4SET   ((volatile unsigned int*)(LTQ_SWINT_BASE + 0x0040))
#define IFX_MPS_SWIRN4EN   ((volatile unsigned int*)(LTQ_SWINT_BASE + 0x0044))
#define IFX_MPS_SWIRN4CR   ((volatile unsigned int*)(LTQ_SWINT_BASE + 0x0048))
#define IFX_MPS_SWIRN4ICR   ((volatile unsigned int*)(LTQ_SWINT_BASE + 0x004C))
#define IFX_MPS_SWIRN5SET   ((volatile unsigned int*)(LTQ_SWINT_BASE + 0x0050))
#define IFX_MPS_SWIRN5EN   ((volatile unsigned int*)(LTQ_SWINT_BASE + 0x0054))
#define IFX_MPS_SWIRN5CR   ((volatile unsigned int*)(LTQ_SWINT_BASE + 0x0058))
#define IFX_MPS_SWIRN5ICR   ((volatile unsigned int*)(LTQ_SWINT_BASE + 0x005C))
#define IFX_MPS_SWIRN6SET   ((volatile unsigned int*)(LTQ_SWINT_BASE + 0x0060))
#define IFX_MPS_SWIRN6EN   ((volatile unsigned int*)(LTQ_SWINT_BASE + 0x0064))
#define IFX_MPS_SWIRN6CR   ((volatile unsigned int*)(LTQ_SWINT_BASE + 0x0068))
#define IFX_MPS_SWIRN6ICR   ((volatile unsigned int*)(LTQ_SWINT_BASE + 0x006C))
#define IFX_MPS_SWIRN7SET   ((volatile unsigned int*)(LTQ_SWINT_BASE + 0x0070))
#define IFX_MPS_SWIRN7EN   ((volatile unsigned int*)(LTQ_SWINT_BASE + 0x0074))
#define IFX_MPS_SWIRN7CR   ((volatile unsigned int*)(LTQ_SWINT_BASE + 0x0078))
#define IFX_MPS_SWIRN7ICR   ((volatile unsigned int*)(LTQ_SWINT_BASE + 0x007C))
#define IFX_MPS_SWIRN8SET   ((volatile unsigned int*)(LTQ_SWINT_BASE + 0x0080))
#define IFX_MPS_SWIRN8EN   ((volatile unsigned int*)(LTQ_SWINT_BASE + 0x0084))
#define IFX_MPS_SWIRN8ICR   ((volatile unsigned int*)(LTQ_SWINT_BASE + 0x008C))
#define IFX_MPS_SWIRN8CR   ((volatile unsigned int*)(LTQ_SWINT_BASE + 0x0088))

/*******************************************************************************
 * MPS_SWIRNSET Register
 ******************************************************************************/

/* Software Interrupt Request IR5 (5) */
#define IFX_MPS_SWIRNSET_IR5   (0x1 << 5)
#define IFX_MPS_SWIRNSET_IR5_VAL(val)   (((val) & 0x1) << 5)
#define IFX_MPS_SWIRNSET_IR5_SET(reg,val) (reg) = (((reg & ~IFX_MPS_SWIRNSET_IR5) | (val) & 1) << 5)
/* Software Interrupt Request IR4 (4) */
#define IFX_MPS_SWIRNSET_IR4   (0x1 << 4)
#define IFX_MPS_SWIRNSET_IR4_VAL(val)   (((val) & 0x1) << 4)
#define IFX_MPS_SWIRNSET_IR4_SET(reg,val) (reg) = (((reg & ~IFX_MPS_SWIRNSET_IR4) | (val) & 1) << 4)
/* Software Interrupt Request IR3 (3) */
#define IFX_MPS_SWIRNSET_IR3   (0x1 << 3)
#define IFX_MPS_SWIRNSET_IR3_VAL(val)   (((val) & 0x1) << 3)
#define IFX_MPS_SWIRNSET_IR3_SET(reg,val) (reg) = (((reg & ~IFX_MPS_SWIRNSET_IR3) | (val) & 1) << 3)
/* Software Interrupt Request IR2 (2) */
#define IFX_MPS_SWIRNSET_IR2   (0x1 << 2)
#define IFX_MPS_SWIRNSET_IR2_VAL(val)   (((val) & 0x1) << 2)
#define IFX_MPS_SWIRNSET_IR2_SET(reg,val) (reg) = (((reg & ~IFX_MPS_SWIRNSET_IR2) | (val) & 1) << 2)
/* Software Interrupt Request IR1 (1) */
#define IFX_MPS_SWIRNSET_IR1   (0x1 << 1)
#define IFX_MPS_SWIRNSET_IR1_VAL(val)   (((val) & 0x1) << 1)
#define IFX_MPS_SWIRNSET_IR1_SET(reg,val) (reg) = (((reg & ~IFX_MPS_SWIRNSET_IR1) | (val) & 1) << 1)
/* Software Interrupt Request IR0 (0) */
#define IFX_MPS_SWIRNSET_IR0   (0x1)
#define IFX_MPS_SWIRNSET_IR0_VAL(val)   (((val) & 0x1) << 0)
#define IFX_MPS_SWIRNSET_IR0_SET(reg,val) (reg) = (((reg & ~IFX_MPS_SWIRNSET_IR0) | (val) & 1) << 0)

/*******************************************************************************
 * MPS_SWIRNEN Register
 ******************************************************************************/

/* Software Interrupt Request IR5 (5) */
#define IFX_MPS_SWIRNEN_IR5   (0x1 << 5)
#define IFX_MPS_SWIRNEN_IR5_VAL(val)   (((val) & 0x1) << 5)
#define IFX_MPS_SWIRNEN_IR5_GET(val)   ((((val) & IFX_MPS_SWIRNEN_IR5) >> 5) & 0x1)
#define IFX_MPS_SWIRNEN_IR5_SET(reg,val) (reg) = ((reg & ~IFX_MPS_SWIRNEN_IR5) | (((val) & 0x1) << 5))
/* Software Interrupt Request IR4 (4) */
#define IFX_MPS_SWIRNEN_IR4   (0x1 << 4)
#define IFX_MPS_SWIRNEN_IR4_VAL(val)   (((val) & 0x1) << 4)
#define IFX_MPS_SWIRNEN_IR4_GET(val)   ((((val) & IFX_MPS_SWIRNEN_IR4) >> 4) & 0x1)
#define IFX_MPS_SWIRNEN_IR4_SET(reg,val) (reg) = ((reg & ~IFX_MPS_SWIRNEN_IR4) | (((val) & 0x1) << 4))
/* Software Interrupt Request IR3 (3) */
#define IFX_MPS_SWIRNEN_IR3   (0x1 << 3)
#define IFX_MPS_SWIRNEN_IR3_VAL(val)   (((val) & 0x1) << 3)
#define IFX_MPS_SWIRNEN_IR3_GET(val)   ((((val) & IFX_MPS_SWIRNEN_IR3) >> 3) & 0x1)
#define IFX_MPS_SWIRNEN_IR3_SET(reg,val) (reg) = ((reg & ~IFX_MPS_SWIRNEN_IR3) | (((val) & 0x1) << 3))
/* Software Interrupt Request IR2 (2) */
#define IFX_MPS_SWIRNEN_IR2   (0x1 << 2)
#define IFX_MPS_SWIRNEN_IR2_VAL(val)   (((val) & 0x1) << 2)
#define IFX_MPS_SWIRNEN_IR2_GET(val)   ((((val) & IFX_MPS_SWIRNEN_IR2) >> 2) & 0x1)
#define IFX_MPS_SWIRNEN_IR2_SET(reg,val) (reg) = ((reg & ~IFX_MPS_SWIRNEN_IR2) | (((val) & 0x1) << 2))
/* Software Interrupt Request IR1 (1) */
#define IFX_MPS_SWIRNEN_IR1   (0x1 << 1)
#define IFX_MPS_SWIRNEN_IR1_VAL(val)   (((val) & 0x1) << 1)
#define IFX_MPS_SWIRNEN_IR1_GET(val)   ((((val) & IFX_MPS_SWIRNEN_IR1) >> 1) & 0x1)
#define IFX_MPS_SWIRNEN_IR1_SET(reg,val) (reg) = ((reg & ~IFX_MPS_SWIRNEN_IR1) | (((val) & 0x1) << 1))
/* Software Interrupt Request IR0 (0) */
#define IFX_MPS_SWIRNEN_IR0   (0x1)
#define IFX_MPS_SWIRNEN_IR0_VAL(val)   (((val) & 0x1) << 0)
#define IFX_MPS_SWIRNEN_IR0_GET(val)   ((((val) & IFX_MPS_SWIRNEN_IR0) >> 0) & 0x1)
#define IFX_MPS_SWIRNEN_IR0_SET(reg,val) (reg) = ((reg & ~IFX_MPS_SWIRNEN_IR0) | (((val) & 0x1) << 0))

/*******************************************************************************
 * MPS_SWIRNICR Register
 ******************************************************************************/

/* Software Interrupt Request IR5 (5) */
#define IFX_MPS_SWIRNICR_IR5   (0x1 << 5)
#define IFX_MPS_SWIRNICR_IR5_GET(val)   ((((val) & IFX_MPS_SWIRNICR_IR5) >> 5) & 0x1)
/* Software Interrupt Request IR4 (4) */
#define IFX_MPS_SWIRNICR_IR4   (0x1 << 4)
#define IFX_MPS_SWIRNICR_IR4_GET(val)   ((((val) & IFX_MPS_SWIRNICR_IR4) >> 4) & 0x1)
/* Software Interrupt Request IR3 (3) */
#define IFX_MPS_SWIRNICR_IR3   (0x1 << 3)
#define IFX_MPS_SWIRNICR_IR3_GET(val)   ((((val) & IFX_MPS_SWIRNICR_IR3) >> 3) & 0x1)
/* Software Interrupt Request IR2 (2) */
#define IFX_MPS_SWIRNICR_IR2   (0x1 << 2)
#define IFX_MPS_SWIRNICR_IR2_GET(val)   ((((val) & IFX_MPS_SWIRNICR_IR2) >> 2) & 0x1)
/* Software Interrupt Request IR1 (1) */
#define IFX_MPS_SWIRNICR_IR1   (0x1 << 1)
#define IFX_MPS_SWIRNICR_IR1_GET(val)   ((((val) & IFX_MPS_SWIRNICR_IR1) >> 1) & 0x1)
/* Software Interrupt Request IR0 (0) */
#define IFX_MPS_SWIRNICR_IR0   (0x1)
#define IFX_MPS_SWIRNICR_IR0_GET(val)   ((((val) & IFX_MPS_SWIRNICR_IR0) >> 0) & 0x1)

/*******************************************************************************
 * MPS_SWIRNCR Register
 ******************************************************************************/

/* Software Interrupt Request IR5 (5) */
#define IFX_MPS_SWIRNCR_IR5   (0x1 << 5)
#define IFX_MPS_SWIRNCR_IR5_GET(val)   ((((val) & IFX_MPS_SWIRNCR_IR5) >> 5) & 0x1)
/* Software Interrupt Request IR4 (4) */
#define IFX_MPS_SWIRNCR_IR4   (0x1 << 4)
#define IFX_MPS_SWIRNCR_IR4_GET(val)   ((((val) & IFX_MPS_SWIRNCR_IR4) >> 4) & 0x1)
/* Software Interrupt Request IR3 (3) */
#define IFX_MPS_SWIRNCR_IR3   (0x1 << 3)
#define IFX_MPS_SWIRNCR_IR3_GET(val)   ((((val) & IFX_MPS_SWIRNCR_IR3) >> 3) & 0x1)
/* Software Interrupt Request IR2 (2) */
#define IFX_MPS_SWIRNCR_IR2   (0x1 << 2)
#define IFX_MPS_SWIRNCR_IR2_GET(val)   ((((val) & IFX_MPS_SWIRNCR_IR2) >> 2) & 0x1)
/* Software Interrupt Request IR1 (1) */
#define IFX_MPS_SWIRNCR_IR1   (0x1 << 1)
#define IFX_MPS_SWIRNCR_IR1_GET(val)   ((((val) & IFX_MPS_SWIRNCR_IR1) >> 1) & 0x1)
/* Software Interrupt Request IR0 (0) */
#define IFX_MPS_SWIRNCR_IR0   (0x1)
#define IFX_MPS_SWIRNCR_IR0_GET(val)   ((((val) & IFX_MPS_SWIRNCR_IR0) >> 0) & 0x1)

#endif
