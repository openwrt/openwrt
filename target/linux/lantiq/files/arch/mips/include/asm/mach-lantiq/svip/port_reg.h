/******************************************************************************

  Copyright (c) 2007
  Infineon Technologies AG
  St. Martin Strasse 53; 81669 Munich, Germany

  Any use of this Software is subject to the conclusion of a respective
  License Agreement. Without such a License Agreement no rights to the
  Software are granted.

 ******************************************************************************/

#ifndef __PORT_REG_H
#define __PORT_REG_H

#define port_r32(reg)			__raw_readl(&reg)
#define port_w32(val, reg)		__raw_writel(val, &reg)

/** PORT register structure */
struct svip_reg_port {
	volatile u32 out;      /*  0x0000 */
	volatile u32 in;       /*  0x0004 */
	volatile u32 dir;      /*  0x0008 */
	volatile u32 altsel0;  /*  0x000C */
	volatile u32 altsel1;  /*  0x0010 */
	volatile u32 puen;     /*  0x0014 */
	volatile u32 exintcr0; /*  0x0018 */
	volatile u32 exintcr1; /*  0x001C */
	volatile u32 irncr;    /*  0x0020 */
	volatile u32 irnicr;   /*  0x0024 */
	volatile u32 irnen;    /*  0x0028 */
	volatile u32 irncfg;   /*  0x002C */
	volatile u32 irnenset; /*  0x0030 */
	volatile u32 irnenclr; /*  0x0034 */
};

/*******************************************************************************
 * Port 0 Data Output Register
 ******************************************************************************/

/* Port 0 Pin # Output Value (19) */
#define PORT_P0_OUT_P19   (0x1 << 19)
#define PORT_P0_OUT_P19_VAL(val)   (((val) & 0x1) << 19)
#define PORT_P0_OUT_P19_GET(val)   ((((val) & PORT_P0_OUT_P19) >> 19) & 0x1)
#define PORT_P0_OUT_P19_SET(reg,val) (reg) = ((reg & ~PORT_P0_OUT_P19) | (((val) & 0x1) << 19))
/* Port 0 Pin # Output Value (18) */
#define PORT_P0_OUT_P18   (0x1 << 18)
#define PORT_P0_OUT_P18_VAL(val)   (((val) & 0x1) << 18)
#define PORT_P0_OUT_P18_GET(val)   ((((val) & PORT_P0_OUT_P18) >> 18) & 0x1)
#define PORT_P0_OUT_P18_SET(reg,val) (reg) = ((reg & ~PORT_P0_OUT_P18) | (((val) & 0x1) << 18))
/* Port 0 Pin # Output Value (17) */
#define PORT_P0_OUT_P17   (0x1 << 17)
#define PORT_P0_OUT_P17_VAL(val)   (((val) & 0x1) << 17)
#define PORT_P0_OUT_P17_GET(val)   ((((val) & PORT_P0_OUT_P17) >> 17) & 0x1)
#define PORT_P0_OUT_P17_SET(reg,val) (reg) = ((reg & ~PORT_P0_OUT_P17) | (((val) & 0x1) << 17))
/* Port 0 Pin # Output Value (16) */
#define PORT_P0_OUT_P16   (0x1 << 16)
#define PORT_P0_OUT_P16_VAL(val)   (((val) & 0x1) << 16)
#define PORT_P0_OUT_P16_GET(val)   ((((val) & PORT_P0_OUT_P16) >> 16) & 0x1)
#define PORT_P0_OUT_P16_SET(reg,val) (reg) = ((reg & ~PORT_P0_OUT_P16) | (((val) & 0x1) << 16))
/* Port 0 Pin # Output Value (15) */
#define PORT_P0_OUT_P15   (0x1 << 15)
#define PORT_P0_OUT_P15_VAL(val)   (((val) & 0x1) << 15)
#define PORT_P0_OUT_P15_GET(val)   ((((val) & PORT_P0_OUT_P15) >> 15) & 0x1)
#define PORT_P0_OUT_P15_SET(reg,val) (reg) = ((reg & ~PORT_P0_OUT_P15) | (((val) & 0x1) << 15))
/* Port 0 Pin # Output Value (14) */
#define PORT_P0_OUT_P14   (0x1 << 14)
#define PORT_P0_OUT_P14_VAL(val)   (((val) & 0x1) << 14)
#define PORT_P0_OUT_P14_GET(val)   ((((val) & PORT_P0_OUT_P14) >> 14) & 0x1)
#define PORT_P0_OUT_P14_SET(reg,val) (reg) = ((reg & ~PORT_P0_OUT_P14) | (((val) & 0x1) << 14))
/* Port 0 Pin # Output Value (13) */
#define PORT_P0_OUT_P13   (0x1 << 13)
#define PORT_P0_OUT_P13_VAL(val)   (((val) & 0x1) << 13)
#define PORT_P0_OUT_P13_GET(val)   ((((val) & PORT_P0_OUT_P13) >> 13) & 0x1)
#define PORT_P0_OUT_P13_SET(reg,val) (reg) = ((reg & ~PORT_P0_OUT_P13) | (((val) & 0x1) << 13))
/* Port 0 Pin # Output Value (12) */
#define PORT_P0_OUT_P12   (0x1 << 12)
#define PORT_P0_OUT_P12_VAL(val)   (((val) & 0x1) << 12)
#define PORT_P0_OUT_P12_GET(val)   ((((val) & PORT_P0_OUT_P12) >> 12) & 0x1)
#define PORT_P0_OUT_P12_SET(reg,val) (reg) = ((reg & ~PORT_P0_OUT_P12) | (((val) & 0x1) << 12))
/* Port 0 Pin # Output Value (11) */
#define PORT_P0_OUT_P11   (0x1 << 11)
#define PORT_P0_OUT_P11_VAL(val)   (((val) & 0x1) << 11)
#define PORT_P0_OUT_P11_GET(val)   ((((val) & PORT_P0_OUT_P11) >> 11) & 0x1)
#define PORT_P0_OUT_P11_SET(reg,val) (reg) = ((reg & ~PORT_P0_OUT_P11) | (((val) & 0x1) << 11))
/* Port 0 Pin # Output Value (10) */
#define PORT_P0_OUT_P10   (0x1 << 10)
#define PORT_P0_OUT_P10_VAL(val)   (((val) & 0x1) << 10)
#define PORT_P0_OUT_P10_GET(val)   ((((val) & PORT_P0_OUT_P10) >> 10) & 0x1)
#define PORT_P0_OUT_P10_SET(reg,val) (reg) = ((reg & ~PORT_P0_OUT_P10) | (((val) & 0x1) << 10))
/* Port 0 Pin # Output Value (9) */
#define PORT_P0_OUT_P9   (0x1 << 9)
#define PORT_P0_OUT_P9_VAL(val)   (((val) & 0x1) << 9)
#define PORT_P0_OUT_P9_GET(val)   ((((val) & PORT_P0_OUT_P9) >> 9) & 0x1)
#define PORT_P0_OUT_P9_SET(reg,val) (reg) = ((reg & ~PORT_P0_OUT_P9) | (((val) & 0x1) << 9))
/* Port 0 Pin # Output Value (8) */
#define PORT_P0_OUT_P8   (0x1 << 8)
#define PORT_P0_OUT_P8_VAL(val)   (((val) & 0x1) << 8)
#define PORT_P0_OUT_P8_GET(val)   ((((val) & PORT_P0_OUT_P8) >> 8) & 0x1)
#define PORT_P0_OUT_P8_SET(reg,val) (reg) = ((reg & ~PORT_P0_OUT_P8) | (((val) & 0x1) << 8))
/* Port 0 Pin # Output Value (7) */
#define PORT_P0_OUT_P7   (0x1 << 7)
#define PORT_P0_OUT_P7_VAL(val)   (((val) & 0x1) << 7)
#define PORT_P0_OUT_P7_GET(val)   ((((val) & PORT_P0_OUT_P7) >> 7) & 0x1)
#define PORT_P0_OUT_P7_SET(reg,val) (reg) = ((reg & ~PORT_P0_OUT_P7) | (((val) & 0x1) << 7))
/* Port 0 Pin # Output Value (6) */
#define PORT_P0_OUT_P6   (0x1 << 6)
#define PORT_P0_OUT_P6_VAL(val)   (((val) & 0x1) << 6)
#define PORT_P0_OUT_P6_GET(val)   ((((val) & PORT_P0_OUT_P6) >> 6) & 0x1)
#define PORT_P0_OUT_P6_SET(reg,val) (reg) = ((reg & ~PORT_P0_OUT_P6) | (((val) & 0x1) << 6))
/* Port 0 Pin # Output Value (5) */
#define PORT_P0_OUT_P5   (0x1 << 5)
#define PORT_P0_OUT_P5_VAL(val)   (((val) & 0x1) << 5)
#define PORT_P0_OUT_P5_GET(val)   ((((val) & PORT_P0_OUT_P5) >> 5) & 0x1)
#define PORT_P0_OUT_P5_SET(reg,val) (reg) = ((reg & ~PORT_P0_OUT_P5) | (((val) & 0x1) << 5))
/* Port 0 Pin # Output Value (4) */
#define PORT_P0_OUT_P4   (0x1 << 4)
#define PORT_P0_OUT_P4_VAL(val)   (((val) & 0x1) << 4)
#define PORT_P0_OUT_P4_GET(val)   ((((val) & PORT_P0_OUT_P4) >> 4) & 0x1)
#define PORT_P0_OUT_P4_SET(reg,val) (reg) = ((reg & ~PORT_P0_OUT_P4) | (((val) & 0x1) << 4))
/* Port 0 Pin # Output Value (3) */
#define PORT_P0_OUT_P3   (0x1 << 3)
#define PORT_P0_OUT_P3_VAL(val)   (((val) & 0x1) << 3)
#define PORT_P0_OUT_P3_GET(val)   ((((val) & PORT_P0_OUT_P3) >> 3) & 0x1)
#define PORT_P0_OUT_P3_SET(reg,val) (reg) = ((reg & ~PORT_P0_OUT_P3) | (((val) & 0x1) << 3))
/* Port 0 Pin # Output Value (2) */
#define PORT_P0_OUT_P2   (0x1 << 2)
#define PORT_P0_OUT_P2_VAL(val)   (((val) & 0x1) << 2)
#define PORT_P0_OUT_P2_GET(val)   ((((val) & PORT_P0_OUT_P2) >> 2) & 0x1)
#define PORT_P0_OUT_P2_SET(reg,val) (reg) = ((reg & ~PORT_P0_OUT_P2) | (((val) & 0x1) << 2))
/* Port 0 Pin # Output Value (1) */
#define PORT_P0_OUT_P1   (0x1 << 1)
#define PORT_P0_OUT_P1_VAL(val)   (((val) & 0x1) << 1)
#define PORT_P0_OUT_P1_GET(val)   ((((val) & PORT_P0_OUT_P1) >> 1) & 0x1)
#define PORT_P0_OUT_P1_SET(reg,val) (reg) = ((reg & ~PORT_P0_OUT_P1) | (((val) & 0x1) << 1))
/* Port 0 Pin # Output Value (0) */
#define PORT_P0_OUT_P0   (0x1)
#define PORT_P0_OUT_P0_VAL(val)   (((val) & 0x1) << 0)
#define PORT_P0_OUT_P0_GET(val)   ((((val) & PORT_P0_OUT_P0) >> 0) & 0x1)
#define PORT_P0_OUT_P0_SET(reg,val) (reg) = ((reg & ~PORT_P0_OUT_P0) | (((val) & 0x1) << 0))

/*******************************************************************************
 * Port 0 Data Input Register
 ******************************************************************************/

/* Port 0 Pin # Latched Input Value (19) */
#define PORT_P0_IN_P19   (0x1 << 19)
#define PORT_P0_IN_P19_GET(val)   ((((val) & PORT_P0_IN_P19) >> 19) & 0x1)
/* Port 0 Pin # Latched Input Value (18) */
#define PORT_P0_IN_P18   (0x1 << 18)
#define PORT_P0_IN_P18_GET(val)   ((((val) & PORT_P0_IN_P18) >> 18) & 0x1)
/* Port 0 Pin # Latched Input Value (17) */
#define PORT_P0_IN_P17   (0x1 << 17)
#define PORT_P0_IN_P17_GET(val)   ((((val) & PORT_P0_IN_P17) >> 17) & 0x1)
/* Port 0 Pin # Latched Input Value (16) */
#define PORT_P0_IN_P16   (0x1 << 16)
#define PORT_P0_IN_P16_GET(val)   ((((val) & PORT_P0_IN_P16) >> 16) & 0x1)
/* Port 0 Pin # Latched Input Value (15) */
#define PORT_P0_IN_P15   (0x1 << 15)
#define PORT_P0_IN_P15_GET(val)   ((((val) & PORT_P0_IN_P15) >> 15) & 0x1)
/* Port 0 Pin # Latched Input Value (14) */
#define PORT_P0_IN_P14   (0x1 << 14)
#define PORT_P0_IN_P14_GET(val)   ((((val) & PORT_P0_IN_P14) >> 14) & 0x1)
/* Port 0 Pin # Latched Input Value (13) */
#define PORT_P0_IN_P13   (0x1 << 13)
#define PORT_P0_IN_P13_GET(val)   ((((val) & PORT_P0_IN_P13) >> 13) & 0x1)
/* Port 0 Pin # Latched Input Value (12) */
#define PORT_P0_IN_P12   (0x1 << 12)
#define PORT_P0_IN_P12_GET(val)   ((((val) & PORT_P0_IN_P12) >> 12) & 0x1)
/* Port 0 Pin # Latched Input Value (11) */
#define PORT_P0_IN_P11   (0x1 << 11)
#define PORT_P0_IN_P11_GET(val)   ((((val) & PORT_P0_IN_P11) >> 11) & 0x1)
/* Port 0 Pin # Latched Input Value (10) */
#define PORT_P0_IN_P10   (0x1 << 10)
#define PORT_P0_IN_P10_GET(val)   ((((val) & PORT_P0_IN_P10) >> 10) & 0x1)
/* Port 0 Pin # Latched Input Value (9) */
#define PORT_P0_IN_P9   (0x1 << 9)
#define PORT_P0_IN_P9_GET(val)   ((((val) & PORT_P0_IN_P9) >> 9) & 0x1)
/* Port 0 Pin # Latched Input Value (8) */
#define PORT_P0_IN_P8   (0x1 << 8)
#define PORT_P0_IN_P8_GET(val)   ((((val) & PORT_P0_IN_P8) >> 8) & 0x1)
/* Port 0 Pin # Latched Input Value (7) */
#define PORT_P0_IN_P7   (0x1 << 7)
#define PORT_P0_IN_P7_GET(val)   ((((val) & PORT_P0_IN_P7) >> 7) & 0x1)
/* Port 0 Pin # Latched Input Value (6) */
#define PORT_P0_IN_P6   (0x1 << 6)
#define PORT_P0_IN_P6_GET(val)   ((((val) & PORT_P0_IN_P6) >> 6) & 0x1)
/* Port 0 Pin # Latched Input Value (5) */
#define PORT_P0_IN_P5   (0x1 << 5)
#define PORT_P0_IN_P5_GET(val)   ((((val) & PORT_P0_IN_P5) >> 5) & 0x1)
/* Port 0 Pin # Latched Input Value (4) */
#define PORT_P0_IN_P4   (0x1 << 4)
#define PORT_P0_IN_P4_GET(val)   ((((val) & PORT_P0_IN_P4) >> 4) & 0x1)
/* Port 0 Pin # Latched Input Value (3) */
#define PORT_P0_IN_P3   (0x1 << 3)
#define PORT_P0_IN_P3_GET(val)   ((((val) & PORT_P0_IN_P3) >> 3) & 0x1)
/* Port 0 Pin # Latched Input Value (2) */
#define PORT_P0_IN_P2   (0x1 << 2)
#define PORT_P0_IN_P2_GET(val)   ((((val) & PORT_P0_IN_P2) >> 2) & 0x1)
/* Port 0 Pin # Latched Input Value (1) */
#define PORT_P0_IN_P1   (0x1 << 1)
#define PORT_P0_IN_P1_GET(val)   ((((val) & PORT_P0_IN_P1) >> 1) & 0x1)
/* Port 0 Pin # Latched Input Value (0) */
#define PORT_P0_IN_P0   (0x1)
#define PORT_P0_IN_P0_GET(val)   ((((val) & PORT_P0_IN_P0) >> 0) & 0x1)

/*******************************************************************************
 * Port 0 Direction Register
 ******************************************************************************/

/* Port 0 Pin #Direction Control (19) */
#define PORT_P0_DIR_P19   (0x1 << 19)
#define PORT_P0_DIR_P19_VAL(val)   (((val) & 0x1) << 19)
#define PORT_P0_DIR_P19_GET(val)   ((((val) & PORT_P0_DIR_P19) >> 19) & 0x1)
#define PORT_P0_DIR_P19_SET(reg,val) (reg) = ((reg & ~PORT_P0_DIR_P19) | (((val) & 0x1) << 19))
/* Port 0 Pin #Direction Control (18) */
#define PORT_P0_DIR_P18   (0x1 << 18)
#define PORT_P0_DIR_P18_VAL(val)   (((val) & 0x1) << 18)
#define PORT_P0_DIR_P18_GET(val)   ((((val) & PORT_P0_DIR_P18) >> 18) & 0x1)
#define PORT_P0_DIR_P18_SET(reg,val) (reg) = ((reg & ~PORT_P0_DIR_P18) | (((val) & 0x1) << 18))
/* Port 0 Pin #Direction Control (17) */
#define PORT_P0_DIR_P17   (0x1 << 17)
#define PORT_P0_DIR_P17_VAL(val)   (((val) & 0x1) << 17)
#define PORT_P0_DIR_P17_GET(val)   ((((val) & PORT_P0_DIR_P17) >> 17) & 0x1)
#define PORT_P0_DIR_P17_SET(reg,val) (reg) = ((reg & ~PORT_P0_DIR_P17) | (((val) & 0x1) << 17))
/* Port 0 Pin #Direction Control (16) */
#define PORT_P0_DIR_P16   (0x1 << 16)
#define PORT_P0_DIR_P16_VAL(val)   (((val) & 0x1) << 16)
#define PORT_P0_DIR_P16_GET(val)   ((((val) & PORT_P0_DIR_P16) >> 16) & 0x1)
#define PORT_P0_DIR_P16_SET(reg,val) (reg) = ((reg & ~PORT_P0_DIR_P16) | (((val) & 0x1) << 16))
/* Port 0 Pin #Direction Control (15) */
#define PORT_P0_DIR_P15   (0x1 << 15)
#define PORT_P0_DIR_P15_VAL(val)   (((val) & 0x1) << 15)
#define PORT_P0_DIR_P15_GET(val)   ((((val) & PORT_P0_DIR_P15) >> 15) & 0x1)
#define PORT_P0_DIR_P15_SET(reg,val) (reg) = ((reg & ~PORT_P0_DIR_P15) | (((val) & 0x1) << 15))
/* Port 0 Pin #Direction Control (14) */
#define PORT_P0_DIR_P14   (0x1 << 14)
#define PORT_P0_DIR_P14_VAL(val)   (((val) & 0x1) << 14)
#define PORT_P0_DIR_P14_GET(val)   ((((val) & PORT_P0_DIR_P14) >> 14) & 0x1)
#define PORT_P0_DIR_P14_SET(reg,val) (reg) = ((reg & ~PORT_P0_DIR_P14) | (((val) & 0x1) << 14))
/* Port 0 Pin #Direction Control (13) */
#define PORT_P0_DIR_P13   (0x1 << 13)
#define PORT_P0_DIR_P13_VAL(val)   (((val) & 0x1) << 13)
#define PORT_P0_DIR_P13_GET(val)   ((((val) & PORT_P0_DIR_P13) >> 13) & 0x1)
#define PORT_P0_DIR_P13_SET(reg,val) (reg) = ((reg & ~PORT_P0_DIR_P13) | (((val) & 0x1) << 13))
/* Port 0 Pin #Direction Control (12) */
#define PORT_P0_DIR_P12   (0x1 << 12)
#define PORT_P0_DIR_P12_VAL(val)   (((val) & 0x1) << 12)
#define PORT_P0_DIR_P12_GET(val)   ((((val) & PORT_P0_DIR_P12) >> 12) & 0x1)
#define PORT_P0_DIR_P12_SET(reg,val) (reg) = ((reg & ~PORT_P0_DIR_P12) | (((val) & 0x1) << 12))
/* Port 0 Pin #Direction Control (11) */
#define PORT_P0_DIR_P11   (0x1 << 11)
#define PORT_P0_DIR_P11_VAL(val)   (((val) & 0x1) << 11)
#define PORT_P0_DIR_P11_GET(val)   ((((val) & PORT_P0_DIR_P11) >> 11) & 0x1)
#define PORT_P0_DIR_P11_SET(reg,val) (reg) = ((reg & ~PORT_P0_DIR_P11) | (((val) & 0x1) << 11))
/* Port 0 Pin #Direction Control (10) */
#define PORT_P0_DIR_P10   (0x1 << 10)
#define PORT_P0_DIR_P10_VAL(val)   (((val) & 0x1) << 10)
#define PORT_P0_DIR_P10_GET(val)   ((((val) & PORT_P0_DIR_P10) >> 10) & 0x1)
#define PORT_P0_DIR_P10_SET(reg,val) (reg) = ((reg & ~PORT_P0_DIR_P10) | (((val) & 0x1) << 10))
/* Port 0 Pin #Direction Control (9) */
#define PORT_P0_DIR_P9   (0x1 << 9)
#define PORT_P0_DIR_P9_VAL(val)   (((val) & 0x1) << 9)
#define PORT_P0_DIR_P9_GET(val)   ((((val) & PORT_P0_DIR_P9) >> 9) & 0x1)
#define PORT_P0_DIR_P9_SET(reg,val) (reg) = ((reg & ~PORT_P0_DIR_P9) | (((val) & 0x1) << 9))
/* Port 0 Pin #Direction Control (8) */
#define PORT_P0_DIR_P8   (0x1 << 8)
#define PORT_P0_DIR_P8_VAL(val)   (((val) & 0x1) << 8)
#define PORT_P0_DIR_P8_GET(val)   ((((val) & PORT_P0_DIR_P8) >> 8) & 0x1)
#define PORT_P0_DIR_P8_SET(reg,val) (reg) = ((reg & ~PORT_P0_DIR_P8) | (((val) & 0x1) << 8))
/* Port 0 Pin #Direction Control (7) */
#define PORT_P0_DIR_P7   (0x1 << 7)
#define PORT_P0_DIR_P7_VAL(val)   (((val) & 0x1) << 7)
#define PORT_P0_DIR_P7_GET(val)   ((((val) & PORT_P0_DIR_P7) >> 7) & 0x1)
#define PORT_P0_DIR_P7_SET(reg,val) (reg) = ((reg & ~PORT_P0_DIR_P7) | (((val) & 0x1) << 7))
/* Port 0 Pin #Direction Control (6) */
#define PORT_P0_DIR_P6   (0x1 << 6)
#define PORT_P0_DIR_P6_VAL(val)   (((val) & 0x1) << 6)
#define PORT_P0_DIR_P6_GET(val)   ((((val) & PORT_P0_DIR_P6) >> 6) & 0x1)
#define PORT_P0_DIR_P6_SET(reg,val) (reg) = ((reg & ~PORT_P0_DIR_P6) | (((val) & 0x1) << 6))
/* Port 0 Pin #Direction Control (5) */
#define PORT_P0_DIR_P5   (0x1 << 5)
#define PORT_P0_DIR_P5_VAL(val)   (((val) & 0x1) << 5)
#define PORT_P0_DIR_P5_GET(val)   ((((val) & PORT_P0_DIR_P5) >> 5) & 0x1)
#define PORT_P0_DIR_P5_SET(reg,val) (reg) = ((reg & ~PORT_P0_DIR_P5) | (((val) & 0x1) << 5))
/* Port 0 Pin #Direction Control (4) */
#define PORT_P0_DIR_P4   (0x1 << 4)
#define PORT_P0_DIR_P4_VAL(val)   (((val) & 0x1) << 4)
#define PORT_P0_DIR_P4_GET(val)   ((((val) & PORT_P0_DIR_P4) >> 4) & 0x1)
#define PORT_P0_DIR_P4_SET(reg,val) (reg) = ((reg & ~PORT_P0_DIR_P4) | (((val) & 0x1) << 4))
/* Port 0 Pin #Direction Control (3) */
#define PORT_P0_DIR_P3   (0x1 << 3)
#define PORT_P0_DIR_P3_VAL(val)   (((val) & 0x1) << 3)
#define PORT_P0_DIR_P3_GET(val)   ((((val) & PORT_P0_DIR_P3) >> 3) & 0x1)
#define PORT_P0_DIR_P3_SET(reg,val) (reg) = ((reg & ~PORT_P0_DIR_P3) | (((val) & 0x1) << 3))
/* Port 0 Pin #Direction Control (2) */
#define PORT_P0_DIR_P2   (0x1 << 2)
#define PORT_P0_DIR_P2_VAL(val)   (((val) & 0x1) << 2)
#define PORT_P0_DIR_P2_GET(val)   ((((val) & PORT_P0_DIR_P2) >> 2) & 0x1)
#define PORT_P0_DIR_P2_SET(reg,val) (reg) = ((reg & ~PORT_P0_DIR_P2) | (((val) & 0x1) << 2))
/* Port 0 Pin #Direction Control (1) */
#define PORT_P0_DIR_P1   (0x1 << 1)
#define PORT_P0_DIR_P1_VAL(val)   (((val) & 0x1) << 1)
#define PORT_P0_DIR_P1_GET(val)   ((((val) & PORT_P0_DIR_P1) >> 1) & 0x1)
#define PORT_P0_DIR_P1_SET(reg,val) (reg) = ((reg & ~PORT_P0_DIR_P1) | (((val) & 0x1) << 1))
/* Port 0 Pin #Direction Control (0) */
#define PORT_P0_DIR_P0   (0x1)
#define PORT_P0_DIR_P0_VAL(val)   (((val) & 0x1) << 0)
#define PORT_P0_DIR_P0_GET(val)   ((((val) & PORT_P0_DIR_P0) >> 0) & 0x1)
#define PORT_P0_DIR_P0_SET(reg,val) (reg) = ((reg & ~PORT_P0_DIR_P0) | (((val) & 0x1) << 0))

/*******************************************************************************
 * Port 0 Alternate Function Select Register 0
 ******************************************************************************/

/* Alternate Function at Port 0 Bit # (19) */
#define PORT_P0_ALTSEL0_P19   (0x1 << 19)
#define PORT_P0_ALTSEL0_P19_VAL(val)   (((val) & 0x1) << 19)
#define PORT_P0_ALTSEL0_P19_GET(val)   ((((val) & PORT_P0_ALTSEL0_P19) >> 19) & 0x1)
#define PORT_P0_ALTSEL0_P19_SET(reg,val) (reg) = ((reg & ~PORT_P0_ALTSEL0_P19) | (((val) & 0x1) << 19))
/* Alternate Function at Port 0 Bit # (18) */
#define PORT_P0_ALTSEL0_P18   (0x1 << 18)
#define PORT_P0_ALTSEL0_P18_VAL(val)   (((val) & 0x1) << 18)
#define PORT_P0_ALTSEL0_P18_GET(val)   ((((val) & PORT_P0_ALTSEL0_P18) >> 18) & 0x1)
#define PORT_P0_ALTSEL0_P18_SET(reg,val) (reg) = ((reg & ~PORT_P0_ALTSEL0_P18) | (((val) & 0x1) << 18))
/* Alternate Function at Port 0 Bit # (17) */
#define PORT_P0_ALTSEL0_P17   (0x1 << 17)
#define PORT_P0_ALTSEL0_P17_VAL(val)   (((val) & 0x1) << 17)
#define PORT_P0_ALTSEL0_P17_GET(val)   ((((val) & PORT_P0_ALTSEL0_P17) >> 17) & 0x1)
#define PORT_P0_ALTSEL0_P17_SET(reg,val) (reg) = ((reg & ~PORT_P0_ALTSEL0_P17) | (((val) & 0x1) << 17))
/* Alternate Function at Port 0 Bit # (16) */
#define PORT_P0_ALTSEL0_P16   (0x1 << 16)
#define PORT_P0_ALTSEL0_P16_VAL(val)   (((val) & 0x1) << 16)
#define PORT_P0_ALTSEL0_P16_GET(val)   ((((val) & PORT_P0_ALTSEL0_P16) >> 16) & 0x1)
#define PORT_P0_ALTSEL0_P16_SET(reg,val) (reg) = ((reg & ~PORT_P0_ALTSEL0_P16) | (((val) & 0x1) << 16))
/* Alternate Function at Port 0 Bit # (15) */
#define PORT_P0_ALTSEL0_P15   (0x1 << 15)
#define PORT_P0_ALTSEL0_P15_VAL(val)   (((val) & 0x1) << 15)
#define PORT_P0_ALTSEL0_P15_GET(val)   ((((val) & PORT_P0_ALTSEL0_P15) >> 15) & 0x1)
#define PORT_P0_ALTSEL0_P15_SET(reg,val) (reg) = ((reg & ~PORT_P0_ALTSEL0_P15) | (((val) & 0x1) << 15))
/* Alternate Function at Port 0 Bit # (14) */
#define PORT_P0_ALTSEL0_P14   (0x1 << 14)
#define PORT_P0_ALTSEL0_P14_VAL(val)   (((val) & 0x1) << 14)
#define PORT_P0_ALTSEL0_P14_GET(val)   ((((val) & PORT_P0_ALTSEL0_P14) >> 14) & 0x1)
#define PORT_P0_ALTSEL0_P14_SET(reg,val) (reg) = ((reg & ~PORT_P0_ALTSEL0_P14) | (((val) & 0x1) << 14))
/* Alternate Function at Port 0 Bit # (13) */
#define PORT_P0_ALTSEL0_P13   (0x1 << 13)
#define PORT_P0_ALTSEL0_P13_VAL(val)   (((val) & 0x1) << 13)
#define PORT_P0_ALTSEL0_P13_GET(val)   ((((val) & PORT_P0_ALTSEL0_P13) >> 13) & 0x1)
#define PORT_P0_ALTSEL0_P13_SET(reg,val) (reg) = ((reg & ~PORT_P0_ALTSEL0_P13) | (((val) & 0x1) << 13))
/* Alternate Function at Port 0 Bit # (12) */
#define PORT_P0_ALTSEL0_P12   (0x1 << 12)
#define PORT_P0_ALTSEL0_P12_VAL(val)   (((val) & 0x1) << 12)
#define PORT_P0_ALTSEL0_P12_GET(val)   ((((val) & PORT_P0_ALTSEL0_P12) >> 12) & 0x1)
#define PORT_P0_ALTSEL0_P12_SET(reg,val) (reg) = ((reg & ~PORT_P0_ALTSEL0_P12) | (((val) & 0x1) << 12))
/* Alternate Function at Port 0 Bit # (11) */
#define PORT_P0_ALTSEL0_P11   (0x1 << 11)
#define PORT_P0_ALTSEL0_P11_VAL(val)   (((val) & 0x1) << 11)
#define PORT_P0_ALTSEL0_P11_GET(val)   ((((val) & PORT_P0_ALTSEL0_P11) >> 11) & 0x1)
#define PORT_P0_ALTSEL0_P11_SET(reg,val) (reg) = ((reg & ~PORT_P0_ALTSEL0_P11) | (((val) & 0x1) << 11))
/* Alternate Function at Port 0 Bit # (10) */
#define PORT_P0_ALTSEL0_P10   (0x1 << 10)
#define PORT_P0_ALTSEL0_P10_VAL(val)   (((val) & 0x1) << 10)
#define PORT_P0_ALTSEL0_P10_GET(val)   ((((val) & PORT_P0_ALTSEL0_P10) >> 10) & 0x1)
#define PORT_P0_ALTSEL0_P10_SET(reg,val) (reg) = ((reg & ~PORT_P0_ALTSEL0_P10) | (((val) & 0x1) << 10))
/* Alternate Function at Port 0 Bit # (9) */
#define PORT_P0_ALTSEL0_P9   (0x1 << 9)
#define PORT_P0_ALTSEL0_P9_VAL(val)   (((val) & 0x1) << 9)
#define PORT_P0_ALTSEL0_P9_GET(val)   ((((val) & PORT_P0_ALTSEL0_P9) >> 9) & 0x1)
#define PORT_P0_ALTSEL0_P9_SET(reg,val) (reg) = ((reg & ~PORT_P0_ALTSEL0_P9) | (((val) & 0x1) << 9))
/* Alternate Function at Port 0 Bit # (8) */
#define PORT_P0_ALTSEL0_P8   (0x1 << 8)
#define PORT_P0_ALTSEL0_P8_VAL(val)   (((val) & 0x1) << 8)
#define PORT_P0_ALTSEL0_P8_GET(val)   ((((val) & PORT_P0_ALTSEL0_P8) >> 8) & 0x1)
#define PORT_P0_ALTSEL0_P8_SET(reg,val) (reg) = ((reg & ~PORT_P0_ALTSEL0_P8) | (((val) & 0x1) << 8))
/* Alternate Function at Port 0 Bit # (7) */
#define PORT_P0_ALTSEL0_P7   (0x1 << 7)
#define PORT_P0_ALTSEL0_P7_VAL(val)   (((val) & 0x1) << 7)
#define PORT_P0_ALTSEL0_P7_GET(val)   ((((val) & PORT_P0_ALTSEL0_P7) >> 7) & 0x1)
#define PORT_P0_ALTSEL0_P7_SET(reg,val) (reg) = ((reg & ~PORT_P0_ALTSEL0_P7) | (((val) & 0x1) << 7))
/* Alternate Function at Port 0 Bit # (6) */
#define PORT_P0_ALTSEL0_P6   (0x1 << 6)
#define PORT_P0_ALTSEL0_P6_VAL(val)   (((val) & 0x1) << 6)
#define PORT_P0_ALTSEL0_P6_GET(val)   ((((val) & PORT_P0_ALTSEL0_P6) >> 6) & 0x1)
#define PORT_P0_ALTSEL0_P6_SET(reg,val) (reg) = ((reg & ~PORT_P0_ALTSEL0_P6) | (((val) & 0x1) << 6))
/* Alternate Function at Port 0 Bit # (5) */
#define PORT_P0_ALTSEL0_P5   (0x1 << 5)
#define PORT_P0_ALTSEL0_P5_VAL(val)   (((val) & 0x1) << 5)
#define PORT_P0_ALTSEL0_P5_GET(val)   ((((val) & PORT_P0_ALTSEL0_P5) >> 5) & 0x1)
#define PORT_P0_ALTSEL0_P5_SET(reg,val) (reg) = ((reg & ~PORT_P0_ALTSEL0_P5) | (((val) & 0x1) << 5))
/* Alternate Function at Port 0 Bit # (4) */
#define PORT_P0_ALTSEL0_P4   (0x1 << 4)
#define PORT_P0_ALTSEL0_P4_VAL(val)   (((val) & 0x1) << 4)
#define PORT_P0_ALTSEL0_P4_GET(val)   ((((val) & PORT_P0_ALTSEL0_P4) >> 4) & 0x1)
#define PORT_P0_ALTSEL0_P4_SET(reg,val) (reg) = ((reg & ~PORT_P0_ALTSEL0_P4) | (((val) & 0x1) << 4))
/* Alternate Function at Port 0 Bit # (3) */
#define PORT_P0_ALTSEL0_P3   (0x1 << 3)
#define PORT_P0_ALTSEL0_P3_VAL(val)   (((val) & 0x1) << 3)
#define PORT_P0_ALTSEL0_P3_GET(val)   ((((val) & PORT_P0_ALTSEL0_P3) >> 3) & 0x1)
#define PORT_P0_ALTSEL0_P3_SET(reg,val) (reg) = ((reg & ~PORT_P0_ALTSEL0_P3) | (((val) & 0x1) << 3))
/* Alternate Function at Port 0 Bit # (2) */
#define PORT_P0_ALTSEL0_P2   (0x1 << 2)
#define PORT_P0_ALTSEL0_P2_VAL(val)   (((val) & 0x1) << 2)
#define PORT_P0_ALTSEL0_P2_GET(val)   ((((val) & PORT_P0_ALTSEL0_P2) >> 2) & 0x1)
#define PORT_P0_ALTSEL0_P2_SET(reg,val) (reg) = ((reg & ~PORT_P0_ALTSEL0_P2) | (((val) & 0x1) << 2))
/* Alternate Function at Port 0 Bit # (1) */
#define PORT_P0_ALTSEL0_P1   (0x1 << 1)
#define PORT_P0_ALTSEL0_P1_VAL(val)   (((val) & 0x1) << 1)
#define PORT_P0_ALTSEL0_P1_GET(val)   ((((val) & PORT_P0_ALTSEL0_P1) >> 1) & 0x1)
#define PORT_P0_ALTSEL0_P1_SET(reg,val) (reg) = ((reg & ~PORT_P0_ALTSEL0_P1) | (((val) & 0x1) << 1))
/* Alternate Function at Port 0 Bit # (0) */
#define PORT_P0_ALTSEL0_P0   (0x1)
#define PORT_P0_ALTSEL0_P0_VAL(val)   (((val) & 0x1) << 0)
#define PORT_P0_ALTSEL0_P0_GET(val)   ((((val) & PORT_P0_ALTSEL0_P0) >> 0) & 0x1)
#define PORT_P0_ALTSEL0_P0_SET(reg,val) (reg) = ((reg & ~PORT_P0_ALTSEL0_P0) | (((val) & 0x1) << 0))

/*******************************************************************************
 * Port 0 Alternate Function Select Register 1
 ******************************************************************************/

/* Alternate Function at Port 0 Bit # (13) */
#define PORT_P0_ALTSEL1_P13   (0x1 << 13)
#define PORT_P0_ALTSEL1_P13_VAL(val)   (((val) & 0x1) << 13)
#define PORT_P0_ALTSEL1_P13_GET(val)   ((((val) & PORT_P0_ALTSEL1_P13) >> 13) & 0x1)
#define PORT_P0_ALTSEL1_P13_SET(reg,val) (reg) = ((reg & ~PORT_P0_ALTSEL1_P13) | (((val) & 0x1) << 13))
/* Alternate Function at Port 0 Bit # (12) */
#define PORT_P0_ALTSEL1_P12   (0x1 << 12)
#define PORT_P0_ALTSEL1_P12_VAL(val)   (((val) & 0x1) << 12)
#define PORT_P0_ALTSEL1_P12_GET(val)   ((((val) & PORT_P0_ALTSEL1_P12) >> 12) & 0x1)
#define PORT_P0_ALTSEL1_P12_SET(reg,val) (reg) = ((reg & ~PORT_P0_ALTSEL1_P12) | (((val) & 0x1) << 12))
/* Alternate Function at Port 0 Bit # (11) */
#define PORT_P0_ALTSEL1_P11   (0x1 << 11)
#define PORT_P0_ALTSEL1_P11_VAL(val)   (((val) & 0x1) << 11)
#define PORT_P0_ALTSEL1_P11_GET(val)   ((((val) & PORT_P0_ALTSEL1_P11) >> 11) & 0x1)
#define PORT_P0_ALTSEL1_P11_SET(reg,val) (reg) = ((reg & ~PORT_P0_ALTSEL1_P11) | (((val) & 0x1) << 11))
/* Alternate Function at Port 0 Bit # (10) */
#define PORT_P0_ALTSEL1_P10   (0x1 << 10)
#define PORT_P0_ALTSEL1_P10_VAL(val)   (((val) & 0x1) << 10)
#define PORT_P0_ALTSEL1_P10_GET(val)   ((((val) & PORT_P0_ALTSEL1_P10) >> 10) & 0x1)
#define PORT_P0_ALTSEL1_P10_SET(reg,val) (reg) = ((reg & ~PORT_P0_ALTSEL1_P10) | (((val) & 0x1) << 10))
/* Alternate Function at Port 0 Bit # (9) */
#define PORT_P0_ALTSEL1_P9   (0x1 << 9)
#define PORT_P0_ALTSEL1_P9_VAL(val)   (((val) & 0x1) << 9)
#define PORT_P0_ALTSEL1_P9_GET(val)   ((((val) & PORT_P0_ALTSEL1_P9) >> 9) & 0x1)
#define PORT_P0_ALTSEL1_P9_SET(reg,val) (reg) = ((reg & ~PORT_P0_ALTSEL1_P9) | (((val) & 0x1) << 9))
/* Alternate Function at Port 0 Bit # (8) */
#define PORT_P0_ALTSEL1_P8   (0x1 << 8)
#define PORT_P0_ALTSEL1_P8_VAL(val)   (((val) & 0x1) << 8)
#define PORT_P0_ALTSEL1_P8_GET(val)   ((((val) & PORT_P0_ALTSEL1_P8) >> 8) & 0x1)
#define PORT_P0_ALTSEL1_P8_SET(reg,val) (reg) = ((reg & ~PORT_P0_ALTSEL1_P8) | (((val) & 0x1) << 8))
/* Alternate Function at Port 0 Bit # (7) */
#define PORT_P0_ALTSEL1_P7   (0x1 << 7)
#define PORT_P0_ALTSEL1_P7_VAL(val)   (((val) & 0x1) << 7)
#define PORT_P0_ALTSEL1_P7_GET(val)   ((((val) & PORT_P0_ALTSEL1_P7) >> 7) & 0x1)
#define PORT_P0_ALTSEL1_P7_SET(reg,val) (reg) = ((reg & ~PORT_P0_ALTSEL1_P7) | (((val) & 0x1) << 7))
/* Alternate Function at Port 0 Bit # (6) */
#define PORT_P0_ALTSEL1_P6   (0x1 << 6)
#define PORT_P0_ALTSEL1_P6_VAL(val)   (((val) & 0x1) << 6)
#define PORT_P0_ALTSEL1_P6_GET(val)   ((((val) & PORT_P0_ALTSEL1_P6) >> 6) & 0x1)
#define PORT_P0_ALTSEL1_P6_SET(reg,val) (reg) = ((reg & ~PORT_P0_ALTSEL1_P6) | (((val) & 0x1) << 6))
/* Alternate Function at Port 0 Bit # (3) */
#define PORT_P0_ALTSEL1_P3   (0x1 << 3)
#define PORT_P0_ALTSEL1_P3_VAL(val)   (((val) & 0x1) << 3)
#define PORT_P0_ALTSEL1_P3_GET(val)   ((((val) & PORT_P0_ALTSEL1_P3) >> 3) & 0x1)
#define PORT_P0_ALTSEL1_P3_SET(reg,val) (reg) = ((reg & ~PORT_P0_ALTSEL1_P3) | (((val) & 0x1) << 3))

/*******************************************************************************
 * Port 0 Pull Up Enable Register
 ******************************************************************************/

/* Pull Up Device Enable at Port 0 Bit # (19) */
#define PORT_P0_PUEN_P19   (0x1 << 19)
#define PORT_P0_PUEN_P19_VAL(val)   (((val) & 0x1) << 19)
#define PORT_P0_PUEN_P19_GET(val)   ((((val) & PORT_P0_PUEN_P19) >> 19) & 0x1)
#define PORT_P0_PUEN_P19_SET(reg,val) (reg) = ((reg & ~PORT_P0_PUEN_P19) | (((val) & 0x1) << 19))
/* Pull Up Device Enable at Port 0 Bit # (18) */
#define PORT_P0_PUEN_P18   (0x1 << 18)
#define PORT_P0_PUEN_P18_VAL(val)   (((val) & 0x1) << 18)
#define PORT_P0_PUEN_P18_GET(val)   ((((val) & PORT_P0_PUEN_P18) >> 18) & 0x1)
#define PORT_P0_PUEN_P18_SET(reg,val) (reg) = ((reg & ~PORT_P0_PUEN_P18) | (((val) & 0x1) << 18))
/* Pull Up Device Enable at Port 0 Bit # (17) */
#define PORT_P0_PUEN_P17   (0x1 << 17)
#define PORT_P0_PUEN_P17_VAL(val)   (((val) & 0x1) << 17)
#define PORT_P0_PUEN_P17_GET(val)   ((((val) & PORT_P0_PUEN_P17) >> 17) & 0x1)
#define PORT_P0_PUEN_P17_SET(reg,val) (reg) = ((reg & ~PORT_P0_PUEN_P17) | (((val) & 0x1) << 17))
/* Pull Up Device Enable at Port 0 Bit # (16) */
#define PORT_P0_PUEN_P16   (0x1 << 16)
#define PORT_P0_PUEN_P16_VAL(val)   (((val) & 0x1) << 16)
#define PORT_P0_PUEN_P16_GET(val)   ((((val) & PORT_P0_PUEN_P16) >> 16) & 0x1)
#define PORT_P0_PUEN_P16_SET(reg,val) (reg) = ((reg & ~PORT_P0_PUEN_P16) | (((val) & 0x1) << 16))
/* Pull Up Device Enable at Port 0 Bit # (15) */
#define PORT_P0_PUEN_P15   (0x1 << 15)
#define PORT_P0_PUEN_P15_VAL(val)   (((val) & 0x1) << 15)
#define PORT_P0_PUEN_P15_GET(val)   ((((val) & PORT_P0_PUEN_P15) >> 15) & 0x1)
#define PORT_P0_PUEN_P15_SET(reg,val) (reg) = ((reg & ~PORT_P0_PUEN_P15) | (((val) & 0x1) << 15))
/* Pull Up Device Enable at Port 0 Bit # (14) */
#define PORT_P0_PUEN_P14   (0x1 << 14)
#define PORT_P0_PUEN_P14_VAL(val)   (((val) & 0x1) << 14)
#define PORT_P0_PUEN_P14_GET(val)   ((((val) & PORT_P0_PUEN_P14) >> 14) & 0x1)
#define PORT_P0_PUEN_P14_SET(reg,val) (reg) = ((reg & ~PORT_P0_PUEN_P14) | (((val) & 0x1) << 14))
/* Pull Up Device Enable at Port 0 Bit # (13) */
#define PORT_P0_PUEN_P13   (0x1 << 13)
#define PORT_P0_PUEN_P13_VAL(val)   (((val) & 0x1) << 13)
#define PORT_P0_PUEN_P13_GET(val)   ((((val) & PORT_P0_PUEN_P13) >> 13) & 0x1)
#define PORT_P0_PUEN_P13_SET(reg,val) (reg) = ((reg & ~PORT_P0_PUEN_P13) | (((val) & 0x1) << 13))
/* Pull Up Device Enable at Port 0 Bit # (12) */
#define PORT_P0_PUEN_P12   (0x1 << 12)
#define PORT_P0_PUEN_P12_VAL(val)   (((val) & 0x1) << 12)
#define PORT_P0_PUEN_P12_GET(val)   ((((val) & PORT_P0_PUEN_P12) >> 12) & 0x1)
#define PORT_P0_PUEN_P12_SET(reg,val) (reg) = ((reg & ~PORT_P0_PUEN_P12) | (((val) & 0x1) << 12))
/* Pull Up Device Enable at Port 0 Bit # (11) */
#define PORT_P0_PUEN_P11   (0x1 << 11)
#define PORT_P0_PUEN_P11_VAL(val)   (((val) & 0x1) << 11)
#define PORT_P0_PUEN_P11_GET(val)   ((((val) & PORT_P0_PUEN_P11) >> 11) & 0x1)
#define PORT_P0_PUEN_P11_SET(reg,val) (reg) = ((reg & ~PORT_P0_PUEN_P11) | (((val) & 0x1) << 11))
/* Pull Up Device Enable at Port 0 Bit # (10) */
#define PORT_P0_PUEN_P10   (0x1 << 10)
#define PORT_P0_PUEN_P10_VAL(val)   (((val) & 0x1) << 10)
#define PORT_P0_PUEN_P10_GET(val)   ((((val) & PORT_P0_PUEN_P10) >> 10) & 0x1)
#define PORT_P0_PUEN_P10_SET(reg,val) (reg) = ((reg & ~PORT_P0_PUEN_P10) | (((val) & 0x1) << 10))
/* Pull Up Device Enable at Port 0 Bit # (9) */
#define PORT_P0_PUEN_P9   (0x1 << 9)
#define PORT_P0_PUEN_P9_VAL(val)   (((val) & 0x1) << 9)
#define PORT_P0_PUEN_P9_GET(val)   ((((val) & PORT_P0_PUEN_P9) >> 9) & 0x1)
#define PORT_P0_PUEN_P9_SET(reg,val) (reg) = ((reg & ~PORT_P0_PUEN_P9) | (((val) & 0x1) << 9))
/* Pull Up Device Enable at Port 0 Bit # (8) */
#define PORT_P0_PUEN_P8   (0x1 << 8)
#define PORT_P0_PUEN_P8_VAL(val)   (((val) & 0x1) << 8)
#define PORT_P0_PUEN_P8_GET(val)   ((((val) & PORT_P0_PUEN_P8) >> 8) & 0x1)
#define PORT_P0_PUEN_P8_SET(reg,val) (reg) = ((reg & ~PORT_P0_PUEN_P8) | (((val) & 0x1) << 8))
/* Pull Up Device Enable at Port 0 Bit # (7) */
#define PORT_P0_PUEN_P7   (0x1 << 7)
#define PORT_P0_PUEN_P7_VAL(val)   (((val) & 0x1) << 7)
#define PORT_P0_PUEN_P7_GET(val)   ((((val) & PORT_P0_PUEN_P7) >> 7) & 0x1)
#define PORT_P0_PUEN_P7_SET(reg,val) (reg) = ((reg & ~PORT_P0_PUEN_P7) | (((val) & 0x1) << 7))
/* Pull Up Device Enable at Port 0 Bit # (6) */
#define PORT_P0_PUEN_P6   (0x1 << 6)
#define PORT_P0_PUEN_P6_VAL(val)   (((val) & 0x1) << 6)
#define PORT_P0_PUEN_P6_GET(val)   ((((val) & PORT_P0_PUEN_P6) >> 6) & 0x1)
#define PORT_P0_PUEN_P6_SET(reg,val) (reg) = ((reg & ~PORT_P0_PUEN_P6) | (((val) & 0x1) << 6))
/* Pull Up Device Enable at Port 0 Bit # (5) */
#define PORT_P0_PUEN_P5   (0x1 << 5)
#define PORT_P0_PUEN_P5_VAL(val)   (((val) & 0x1) << 5)
#define PORT_P0_PUEN_P5_GET(val)   ((((val) & PORT_P0_PUEN_P5) >> 5) & 0x1)
#define PORT_P0_PUEN_P5_SET(reg,val) (reg) = ((reg & ~PORT_P0_PUEN_P5) | (((val) & 0x1) << 5))
/* Pull Up Device Enable at Port 0 Bit # (4) */
#define PORT_P0_PUEN_P4   (0x1 << 4)
#define PORT_P0_PUEN_P4_VAL(val)   (((val) & 0x1) << 4)
#define PORT_P0_PUEN_P4_GET(val)   ((((val) & PORT_P0_PUEN_P4) >> 4) & 0x1)
#define PORT_P0_PUEN_P4_SET(reg,val) (reg) = ((reg & ~PORT_P0_PUEN_P4) | (((val) & 0x1) << 4))
/* Pull Up Device Enable at Port 0 Bit # (3) */
#define PORT_P0_PUEN_P3   (0x1 << 3)
#define PORT_P0_PUEN_P3_VAL(val)   (((val) & 0x1) << 3)
#define PORT_P0_PUEN_P3_GET(val)   ((((val) & PORT_P0_PUEN_P3) >> 3) & 0x1)
#define PORT_P0_PUEN_P3_SET(reg,val) (reg) = ((reg & ~PORT_P0_PUEN_P3) | (((val) & 0x1) << 3))
/* Pull Up Device Enable at Port 0 Bit # (2) */
#define PORT_P0_PUEN_P2   (0x1 << 2)
#define PORT_P0_PUEN_P2_VAL(val)   (((val) & 0x1) << 2)
#define PORT_P0_PUEN_P2_GET(val)   ((((val) & PORT_P0_PUEN_P2) >> 2) & 0x1)
#define PORT_P0_PUEN_P2_SET(reg,val) (reg) = ((reg & ~PORT_P0_PUEN_P2) | (((val) & 0x1) << 2))
/* Pull Up Device Enable at Port 0 Bit # (1) */
#define PORT_P0_PUEN_P1   (0x1 << 1)
#define PORT_P0_PUEN_P1_VAL(val)   (((val) & 0x1) << 1)
#define PORT_P0_PUEN_P1_GET(val)   ((((val) & PORT_P0_PUEN_P1) >> 1) & 0x1)
#define PORT_P0_PUEN_P1_SET(reg,val) (reg) = ((reg & ~PORT_P0_PUEN_P1) | (((val) & 0x1) << 1))
/* Pull Up Device Enable at Port 0 Bit # (0) */
#define PORT_P0_PUEN_P0   (0x1)
#define PORT_P0_PUEN_P0_VAL(val)   (((val) & 0x1) << 0)
#define PORT_P0_PUEN_P0_GET(val)   ((((val) & PORT_P0_PUEN_P0) >> 0) & 0x1)
#define PORT_P0_PUEN_P0_SET(reg,val) (reg) = ((reg & ~PORT_P0_PUEN_P0) | (((val) & 0x1) << 0))

/*******************************************************************************
 * External Interrupt Control Register 0
 ******************************************************************************/

/* Type of Level or Edge Detection of EXINT16 (19) */
#define PORT_P0_EXINTCR0_EXINT16   (0x1 << 19)
#define PORT_P0_EXINTCR0_EXINT16_VAL(val)   (((val) & 0x1) << 19)
#define PORT_P0_EXINTCR0_EXINT16_GET(val)   ((((val) & PORT_P0_EXINTCR0_EXINT16) >> 19) & 0x1)
#define PORT_P0_EXINTCR0_EXINT16_SET(reg,val) (reg) = ((reg & ~PORT_P0_EXINTCR0_EXINT16) | (((val) & 0x1) << 19))
/* Type of Level or Edge Detection of EXINT10 (17) */
#define PORT_P0_EXINTCR0_EXINT10   (0x1 << 17)
#define PORT_P0_EXINTCR0_EXINT10_VAL(val)   (((val) & 0x1) << 17)
#define PORT_P0_EXINTCR0_EXINT10_GET(val)   ((((val) & PORT_P0_EXINTCR0_EXINT10) >> 17) & 0x1)
#define PORT_P0_EXINTCR0_EXINT10_SET(reg,val) (reg) = ((reg & ~PORT_P0_EXINTCR0_EXINT10) | (((val) & 0x1) << 17))
/* Type of Level or Edge Detection of EXINT9 (16) */
#define PORT_P0_EXINTCR0_EXINT9   (0x1 << 16)
#define PORT_P0_EXINTCR0_EXINT9_VAL(val)   (((val) & 0x1) << 16)
#define PORT_P0_EXINTCR0_EXINT9_GET(val)   ((((val) & PORT_P0_EXINTCR0_EXINT9) >> 16) & 0x1)
#define PORT_P0_EXINTCR0_EXINT9_SET(reg,val) (reg) = ((reg & ~PORT_P0_EXINTCR0_EXINT9) | (((val) & 0x1) << 16))
/* Type of Level or Edge Detection of EXINT8 (15) */
#define PORT_P0_EXINTCR0_EXINT8   (0x1 << 15)
#define PORT_P0_EXINTCR0_EXINT8_VAL(val)   (((val) & 0x1) << 15)
#define PORT_P0_EXINTCR0_EXINT8_GET(val)   ((((val) & PORT_P0_EXINTCR0_EXINT8) >> 15) & 0x1)
#define PORT_P0_EXINTCR0_EXINT8_SET(reg,val) (reg) = ((reg & ~PORT_P0_EXINTCR0_EXINT8) | (((val) & 0x1) << 15))
/* Type of Level or Edge Detection of EXINT7 (14) */
#define PORT_P0_EXINTCR0_EXINT7   (0x1 << 14)
#define PORT_P0_EXINTCR0_EXINT7_VAL(val)   (((val) & 0x1) << 14)
#define PORT_P0_EXINTCR0_EXINT7_GET(val)   ((((val) & PORT_P0_EXINTCR0_EXINT7) >> 14) & 0x1)
#define PORT_P0_EXINTCR0_EXINT7_SET(reg,val) (reg) = ((reg & ~PORT_P0_EXINTCR0_EXINT7) | (((val) & 0x1) << 14))
/* Type of Level or Edge Detection of EXINT6 (13) */
#define PORT_P0_EXINTCR0_EXINT6   (0x1 << 13)
#define PORT_P0_EXINTCR0_EXINT6_VAL(val)   (((val) & 0x1) << 13)
#define PORT_P0_EXINTCR0_EXINT6_GET(val)   ((((val) & PORT_P0_EXINTCR0_EXINT6) >> 13) & 0x1)
#define PORT_P0_EXINTCR0_EXINT6_SET(reg,val) (reg) = ((reg & ~PORT_P0_EXINTCR0_EXINT6) | (((val) & 0x1) << 13))
/* Type of Level or Edge Detection of EXINT5 (12) */
#define PORT_P0_EXINTCR0_EXINT5   (0x1 << 12)
#define PORT_P0_EXINTCR0_EXINT5_VAL(val)   (((val) & 0x1) << 12)
#define PORT_P0_EXINTCR0_EXINT5_GET(val)   ((((val) & PORT_P0_EXINTCR0_EXINT5) >> 12) & 0x1)
#define PORT_P0_EXINTCR0_EXINT5_SET(reg,val) (reg) = ((reg & ~PORT_P0_EXINTCR0_EXINT5) | (((val) & 0x1) << 12))
/* Type of Level or Edge Detection of EXINT4 (11) */
#define PORT_P0_EXINTCR0_EXINT4   (0x1 << 11)
#define PORT_P0_EXINTCR0_EXINT4_VAL(val)   (((val) & 0x1) << 11)
#define PORT_P0_EXINTCR0_EXINT4_GET(val)   ((((val) & PORT_P0_EXINTCR0_EXINT4) >> 11) & 0x1)
#define PORT_P0_EXINTCR0_EXINT4_SET(reg,val) (reg) = ((reg & ~PORT_P0_EXINTCR0_EXINT4) | (((val) & 0x1) << 11))
/* Type of Level or Edge Detection of EXINT3 (10) */
#define PORT_P0_EXINTCR0_EXINT3   (0x1 << 10)
#define PORT_P0_EXINTCR0_EXINT3_VAL(val)   (((val) & 0x1) << 10)
#define PORT_P0_EXINTCR0_EXINT3_GET(val)   ((((val) & PORT_P0_EXINTCR0_EXINT3) >> 10) & 0x1)
#define PORT_P0_EXINTCR0_EXINT3_SET(reg,val) (reg) = ((reg & ~PORT_P0_EXINTCR0_EXINT3) | (((val) & 0x1) << 10))
/* Type of Level or Edge Detection of EXINT2 (9) */
#define PORT_P0_EXINTCR0_EXINT2   (0x1 << 9)
#define PORT_P0_EXINTCR0_EXINT2_VAL(val)   (((val) & 0x1) << 9)
#define PORT_P0_EXINTCR0_EXINT2_GET(val)   ((((val) & PORT_P0_EXINTCR0_EXINT2) >> 9) & 0x1)
#define PORT_P0_EXINTCR0_EXINT2_SET(reg,val) (reg) = ((reg & ~PORT_P0_EXINTCR0_EXINT2) | (((val) & 0x1) << 9))
/* Type of Level or Edge Detection of EXINT1 (8) */
#define PORT_P0_EXINTCR0_EXINT1   (0x1 << 8)
#define PORT_P0_EXINTCR0_EXINT1_VAL(val)   (((val) & 0x1) << 8)
#define PORT_P0_EXINTCR0_EXINT1_GET(val)   ((((val) & PORT_P0_EXINTCR0_EXINT1) >> 8) & 0x1)
#define PORT_P0_EXINTCR0_EXINT1_SET(reg,val) (reg) = ((reg & ~PORT_P0_EXINTCR0_EXINT1) | (((val) & 0x1) << 8))
/* Type of Level or Edge Detection of EXINT0 (7) */
#define PORT_P0_EXINTCR0_EXINT0   (0x1 << 7)
#define PORT_P0_EXINTCR0_EXINT0_VAL(val)   (((val) & 0x1) << 7)
#define PORT_P0_EXINTCR0_EXINT0_GET(val)   ((((val) & PORT_P0_EXINTCR0_EXINT0) >> 7) & 0x1)
#define PORT_P0_EXINTCR0_EXINT0_SET(reg,val) (reg) = ((reg & ~PORT_P0_EXINTCR0_EXINT0) | (((val) & 0x1) << 7))

/*******************************************************************************
 * External Interrupt Control Register 1
 ******************************************************************************/

/* Type of Level or Edge Detection of EXINT16 (19) */
#define PORT_P0_EXINTCR1_EXINT16   (0x1 << 19)
#define PORT_P0_EXINTCR1_EXINT16_VAL(val)   (((val) & 0x1) << 19)
#define PORT_P0_EXINTCR1_EXINT16_GET(val)   ((((val) & PORT_P0_EXINTCR1_EXINT16) >> 19) & 0x1)
#define PORT_P0_EXINTCR1_EXINT16_SET(reg,val) (reg) = ((reg & ~PORT_P0_EXINTCR1_EXINT16) | (((val) & 0x1) << 19))
/* Type of Level or Edge Detection of EXINT10 (17) */
#define PORT_P0_EXINTCR1_EXINT10   (0x1 << 17)
#define PORT_P0_EXINTCR1_EXINT10_VAL(val)   (((val) & 0x1) << 17)
#define PORT_P0_EXINTCR1_EXINT10_GET(val)   ((((val) & PORT_P0_EXINTCR1_EXINT10) >> 17) & 0x1)
#define PORT_P0_EXINTCR1_EXINT10_SET(reg,val) (reg) = ((reg & ~PORT_P0_EXINTCR1_EXINT10) | (((val) & 0x1) << 17))
/* Type of Level or Edge Detection of EXINT9 (16) */
#define PORT_P0_EXINTCR1_EXINT9   (0x1 << 16)
#define PORT_P0_EXINTCR1_EXINT9_VAL(val)   (((val) & 0x1) << 16)
#define PORT_P0_EXINTCR1_EXINT9_GET(val)   ((((val) & PORT_P0_EXINTCR1_EXINT9) >> 16) & 0x1)
#define PORT_P0_EXINTCR1_EXINT9_SET(reg,val) (reg) = ((reg & ~PORT_P0_EXINTCR1_EXINT9) | (((val) & 0x1) << 16))
/* Type of Level or Edge Detection of EXINT8 (15) */
#define PORT_P0_EXINTCR1_EXINT8   (0x1 << 15)
#define PORT_P0_EXINTCR1_EXINT8_VAL(val)   (((val) & 0x1) << 15)
#define PORT_P0_EXINTCR1_EXINT8_GET(val)   ((((val) & PORT_P0_EXINTCR1_EXINT8) >> 15) & 0x1)
#define PORT_P0_EXINTCR1_EXINT8_SET(reg,val) (reg) = ((reg & ~PORT_P0_EXINTCR1_EXINT8) | (((val) & 0x1) << 15))
/* Type of Level or Edge Detection of EXINT7 (14) */
#define PORT_P0_EXINTCR1_EXINT7   (0x1 << 14)
#define PORT_P0_EXINTCR1_EXINT7_VAL(val)   (((val) & 0x1) << 14)
#define PORT_P0_EXINTCR1_EXINT7_GET(val)   ((((val) & PORT_P0_EXINTCR1_EXINT7) >> 14) & 0x1)
#define PORT_P0_EXINTCR1_EXINT7_SET(reg,val) (reg) = ((reg & ~PORT_P0_EXINTCR1_EXINT7) | (((val) & 0x1) << 14))
/* Type of Level or Edge Detection of EXINT6 (13) */
#define PORT_P0_EXINTCR1_EXINT6   (0x1 << 13)
#define PORT_P0_EXINTCR1_EXINT6_VAL(val)   (((val) & 0x1) << 13)
#define PORT_P0_EXINTCR1_EXINT6_GET(val)   ((((val) & PORT_P0_EXINTCR1_EXINT6) >> 13) & 0x1)
#define PORT_P0_EXINTCR1_EXINT6_SET(reg,val) (reg) = ((reg & ~PORT_P0_EXINTCR1_EXINT6) | (((val) & 0x1) << 13))
/* Type of Level or Edge Detection of EXINT5 (12) */
#define PORT_P0_EXINTCR1_EXINT5   (0x1 << 12)
#define PORT_P0_EXINTCR1_EXINT5_VAL(val)   (((val) & 0x1) << 12)
#define PORT_P0_EXINTCR1_EXINT5_GET(val)   ((((val) & PORT_P0_EXINTCR1_EXINT5) >> 12) & 0x1)
#define PORT_P0_EXINTCR1_EXINT5_SET(reg,val) (reg) = ((reg & ~PORT_P0_EXINTCR1_EXINT5) | (((val) & 0x1) << 12))
/* Type of Level or Edge Detection of EXINT4 (11) */
#define PORT_P0_EXINTCR1_EXINT4   (0x1 << 11)
#define PORT_P0_EXINTCR1_EXINT4_VAL(val)   (((val) & 0x1) << 11)
#define PORT_P0_EXINTCR1_EXINT4_GET(val)   ((((val) & PORT_P0_EXINTCR1_EXINT4) >> 11) & 0x1)
#define PORT_P0_EXINTCR1_EXINT4_SET(reg,val) (reg) = ((reg & ~PORT_P0_EXINTCR1_EXINT4) | (((val) & 0x1) << 11))
/* Type of Level or Edge Detection of EXINT3 (10) */
#define PORT_P0_EXINTCR1_EXINT3   (0x1 << 10)
#define PORT_P0_EXINTCR1_EXINT3_VAL(val)   (((val) & 0x1) << 10)
#define PORT_P0_EXINTCR1_EXINT3_GET(val)   ((((val) & PORT_P0_EXINTCR1_EXINT3) >> 10) & 0x1)
#define PORT_P0_EXINTCR1_EXINT3_SET(reg,val) (reg) = ((reg & ~PORT_P0_EXINTCR1_EXINT3) | (((val) & 0x1) << 10))
/* Type of Level or Edge Detection of EXINT2 (9) */
#define PORT_P0_EXINTCR1_EXINT2   (0x1 << 9)
#define PORT_P0_EXINTCR1_EXINT2_VAL(val)   (((val) & 0x1) << 9)
#define PORT_P0_EXINTCR1_EXINT2_GET(val)   ((((val) & PORT_P0_EXINTCR1_EXINT2) >> 9) & 0x1)
#define PORT_P0_EXINTCR1_EXINT2_SET(reg,val) (reg) = ((reg & ~PORT_P0_EXINTCR1_EXINT2) | (((val) & 0x1) << 9))
/* Type of Level or Edge Detection of EXINT1 (8) */
#define PORT_P0_EXINTCR1_EXINT1   (0x1 << 8)
#define PORT_P0_EXINTCR1_EXINT1_VAL(val)   (((val) & 0x1) << 8)
#define PORT_P0_EXINTCR1_EXINT1_GET(val)   ((((val) & PORT_P0_EXINTCR1_EXINT1) >> 8) & 0x1)
#define PORT_P0_EXINTCR1_EXINT1_SET(reg,val) (reg) = ((reg & ~PORT_P0_EXINTCR1_EXINT1) | (((val) & 0x1) << 8))
/* Type of Level or Edge Detection of EXINT0 (7) */
#define PORT_P0_EXINTCR1_EXINT0   (0x1 << 7)
#define PORT_P0_EXINTCR1_EXINT0_VAL(val)   (((val) & 0x1) << 7)
#define PORT_P0_EXINTCR1_EXINT0_GET(val)   ((((val) & PORT_P0_EXINTCR1_EXINT0) >> 7) & 0x1)
#define PORT_P0_EXINTCR1_EXINT0_SET(reg,val) (reg) = ((reg & ~PORT_P0_EXINTCR1_EXINT0) | (((val) & 0x1) << 7))

/*******************************************************************************
 * P0_IRNEN Register
 ******************************************************************************/

/* EXINT16 Interrupt Request Enable (19) */
#define PORT_P0_IRNEN_EXINT16   (0x1 << 19)
#define PORT_P0_IRNEN_EXINT16_VAL(val)   (((val) & 0x1) << 19)
#define PORT_P0_IRNEN_EXINT16_GET(val)   ((((val) & PORT_P0_IRNEN_EXINT16) >> 19) & 0x1)
#define PORT_P0_IRNEN_EXINT16_SET(reg,val) (reg) = ((reg & ~PORT_P0_IRNEN_EXINT16) | (((val) & 0x1) << 19))
/* EXINT10 Interrupt Request Enable (17) */
#define PORT_P0_IRNEN_EXINT10   (0x1 << 17)
#define PORT_P0_IRNEN_EXINT10_VAL(val)   (((val) & 0x1) << 17)
#define PORT_P0_IRNEN_EXINT10_GET(val)   ((((val) & PORT_P0_IRNEN_EXINT10) >> 17) & 0x1)
#define PORT_P0_IRNEN_EXINT10_SET(reg,val) (reg) = ((reg & ~PORT_P0_IRNEN_EXINT10) | (((val) & 0x1) << 17))
/* EXINT9 Interrupt Request Enable (16) */
#define PORT_P0_IRNEN_EXINT9   (0x1 << 16)
#define PORT_P0_IRNEN_EXINT9_VAL(val)   (((val) & 0x1) << 16)
#define PORT_P0_IRNEN_EXINT9_GET(val)   ((((val) & PORT_P0_IRNEN_EXINT9) >> 16) & 0x1)
#define PORT_P0_IRNEN_EXINT9_SET(reg,val) (reg) = ((reg & ~PORT_P0_IRNEN_EXINT9) | (((val) & 0x1) << 16))
/* EXINT8 Interrupt Request Enable (15) */
#define PORT_P0_IRNEN_EXINT8   (0x1 << 15)
#define PORT_P0_IRNEN_EXINT8_VAL(val)   (((val) & 0x1) << 15)
#define PORT_P0_IRNEN_EXINT8_GET(val)   ((((val) & PORT_P0_IRNEN_EXINT8) >> 15) & 0x1)
#define PORT_P0_IRNEN_EXINT8_SET(reg,val) (reg) = ((reg & ~PORT_P0_IRNEN_EXINT8) | (((val) & 0x1) << 15))
/* EXINT7 Interrupt Request Enable (14) */
#define PORT_P0_IRNEN_EXINT7   (0x1 << 14)
#define PORT_P0_IRNEN_EXINT7_VAL(val)   (((val) & 0x1) << 14)
#define PORT_P0_IRNEN_EXINT7_GET(val)   ((((val) & PORT_P0_IRNEN_EXINT7) >> 14) & 0x1)
#define PORT_P0_IRNEN_EXINT7_SET(reg,val) (reg) = ((reg & ~PORT_P0_IRNEN_EXINT7) | (((val) & 0x1) << 14))
/* EXINT6 Interrupt Request Enable (13) */
#define PORT_P0_IRNEN_EXINT6   (0x1 << 13)
#define PORT_P0_IRNEN_EXINT6_VAL(val)   (((val) & 0x1) << 13)
#define PORT_P0_IRNEN_EXINT6_GET(val)   ((((val) & PORT_P0_IRNEN_EXINT6) >> 13) & 0x1)
#define PORT_P0_IRNEN_EXINT6_SET(reg,val) (reg) = ((reg & ~PORT_P0_IRNEN_EXINT6) | (((val) & 0x1) << 13))
/* EXINT5 Interrupt Request Enable (12) */
#define PORT_P0_IRNEN_EXINT5   (0x1 << 12)
#define PORT_P0_IRNEN_EXINT5_VAL(val)   (((val) & 0x1) << 12)
#define PORT_P0_IRNEN_EXINT5_GET(val)   ((((val) & PORT_P0_IRNEN_EXINT5) >> 12) & 0x1)
#define PORT_P0_IRNEN_EXINT5_SET(reg,val) (reg) = ((reg & ~PORT_P0_IRNEN_EXINT5) | (((val) & 0x1) << 12))
/* EXINT4 Interrupt Request Enable (11) */
#define PORT_P0_IRNEN_EXINT4   (0x1 << 11)
#define PORT_P0_IRNEN_EXINT4_VAL(val)   (((val) & 0x1) << 11)
#define PORT_P0_IRNEN_EXINT4_GET(val)   ((((val) & PORT_P0_IRNEN_EXINT4) >> 11) & 0x1)
#define PORT_P0_IRNEN_EXINT4_SET(reg,val) (reg) = ((reg & ~PORT_P0_IRNEN_EXINT4) | (((val) & 0x1) << 11))
/* EXINT3 Interrupt Request Enable (10) */
#define PORT_P0_IRNEN_EXINT3   (0x1 << 10)
#define PORT_P0_IRNEN_EXINT3_VAL(val)   (((val) & 0x1) << 10)
#define PORT_P0_IRNEN_EXINT3_GET(val)   ((((val) & PORT_P0_IRNEN_EXINT3) >> 10) & 0x1)
#define PORT_P0_IRNEN_EXINT3_SET(reg,val) (reg) = ((reg & ~PORT_P0_IRNEN_EXINT3) | (((val) & 0x1) << 10))
/* EXINT2 Interrupt Request Enable (9) */
#define PORT_P0_IRNEN_EXINT2   (0x1 << 9)
#define PORT_P0_IRNEN_EXINT2_VAL(val)   (((val) & 0x1) << 9)
#define PORT_P0_IRNEN_EXINT2_GET(val)   ((((val) & PORT_P0_IRNEN_EXINT2) >> 9) & 0x1)
#define PORT_P0_IRNEN_EXINT2_SET(reg,val) (reg) = ((reg & ~PORT_P0_IRNEN_EXINT2) | (((val) & 0x1) << 9))
/* EXINT1 Interrupt Request Enable (8) */
#define PORT_P0_IRNEN_EXINT1   (0x1 << 8)
#define PORT_P0_IRNEN_EXINT1_VAL(val)   (((val) & 0x1) << 8)
#define PORT_P0_IRNEN_EXINT1_GET(val)   ((((val) & PORT_P0_IRNEN_EXINT1) >> 8) & 0x1)
#define PORT_P0_IRNEN_EXINT1_SET(reg,val) (reg) = ((reg & ~PORT_P0_IRNEN_EXINT1) | (((val) & 0x1) << 8))
/* EXINT0 Interrupt Request Enable (7) */
#define PORT_P0_IRNEN_EXINT0   (0x1 << 7)
#define PORT_P0_IRNEN_EXINT0_VAL(val)   (((val) & 0x1) << 7)
#define PORT_P0_IRNEN_EXINT0_GET(val)   ((((val) & PORT_P0_IRNEN_EXINT0) >> 7) & 0x1)
#define PORT_P0_IRNEN_EXINT0_SET(reg,val) (reg) = ((reg & ~PORT_P0_IRNEN_EXINT0) | (((val) & 0x1) << 7))

/*******************************************************************************
 * P0_IRNICR Register
 ******************************************************************************/

/* EXINT16 Interrupt Request (19) */
#define PORT_P0_IRNICR_EXINT16   (0x1 << 19)
#define PORT_P0_IRNICR_EXINT16_GET(val)   ((((val) & PORT_P0_IRNICR_EXINT16) >> 19) & 0x1)
/* EXINT10 Interrupt Request (17) */
#define PORT_P0_IRNICR_EXINT10   (0x1 << 17)
#define PORT_P0_IRNICR_EXINT10_GET(val)   ((((val) & PORT_P0_IRNICR_EXINT10) >> 17) & 0x1)
/* EXINT9 Interrupt Request (16) */
#define PORT_P0_IRNICR_EXINT9   (0x1 << 16)
#define PORT_P0_IRNICR_EXINT9_GET(val)   ((((val) & PORT_P0_IRNICR_EXINT9) >> 16) & 0x1)
/* EXINT8 Interrupt Request (15) */
#define PORT_P0_IRNICR_EXINT8   (0x1 << 15)
#define PORT_P0_IRNICR_EXINT8_GET(val)   ((((val) & PORT_P0_IRNICR_EXINT8) >> 15) & 0x1)
/* EXINT7 Interrupt Request (14) */
#define PORT_P0_IRNICR_EXINT7   (0x1 << 14)
#define PORT_P0_IRNICR_EXINT7_GET(val)   ((((val) & PORT_P0_IRNICR_EXINT7) >> 14) & 0x1)
/* EXINT6 Interrupt Request (13) */
#define PORT_P0_IRNICR_EXINT6   (0x1 << 13)
#define PORT_P0_IRNICR_EXINT6_GET(val)   ((((val) & PORT_P0_IRNICR_EXINT6) >> 13) & 0x1)
/* EXINT5 Interrupt Request (12) */
#define PORT_P0_IRNICR_EXINT5   (0x1 << 12)
#define PORT_P0_IRNICR_EXINT5_GET(val)   ((((val) & PORT_P0_IRNICR_EXINT5) >> 12) & 0x1)
/* EXINT4 Interrupt Request (11) */
#define PORT_P0_IRNICR_EXINT4   (0x1 << 11)
#define PORT_P0_IRNICR_EXINT4_GET(val)   ((((val) & PORT_P0_IRNICR_EXINT4) >> 11) & 0x1)
/* EXINT3 Interrupt Request (10) */
#define PORT_P0_IRNICR_EXINT3   (0x1 << 10)
#define PORT_P0_IRNICR_EXINT3_GET(val)   ((((val) & PORT_P0_IRNICR_EXINT3) >> 10) & 0x1)
/* EXINT2 Interrupt Request (9) */
#define PORT_P0_IRNICR_EXINT2   (0x1 << 9)
#define PORT_P0_IRNICR_EXINT2_GET(val)   ((((val) & PORT_P0_IRNICR_EXINT2) >> 9) & 0x1)
/* EXINT1 Interrupt Request (8) */
#define PORT_P0_IRNICR_EXINT1   (0x1 << 8)
#define PORT_P0_IRNICR_EXINT1_GET(val)   ((((val) & PORT_P0_IRNICR_EXINT1) >> 8) & 0x1)
/* EXINT0 Interrupt Request (7) */
#define PORT_P0_IRNICR_EXINT0   (0x1 << 7)
#define PORT_P0_IRNICR_EXINT0_GET(val)   ((((val) & PORT_P0_IRNICR_EXINT0) >> 7) & 0x1)

/*******************************************************************************
 * P0_IRNCR Register
 ******************************************************************************/

/* EXINT16 Interrupt Request (19) */
#define PORT_P0_IRNCR_EXINT16   (0x1 << 19)
#define PORT_P0_IRNCR_EXINT16_GET(val)   ((((val) & PORT_P0_IRNCR_EXINT16) >> 19) & 0x1)
/* EXINT10 Interrupt Request (17) */
#define PORT_P0_IRNCR_EXINT10   (0x1 << 17)
#define PORT_P0_IRNCR_EXINT10_GET(val)   ((((val) & PORT_P0_IRNCR_EXINT10) >> 17) & 0x1)
/* EXINT9 Interrupt Request (16) */
#define PORT_P0_IRNCR_EXINT9   (0x1 << 16)
#define PORT_P0_IRNCR_EXINT9_GET(val)   ((((val) & PORT_P0_IRNCR_EXINT9) >> 16) & 0x1)
/* EXINT8 Interrupt Request (15) */
#define PORT_P0_IRNCR_EXINT8   (0x1 << 15)
#define PORT_P0_IRNCR_EXINT8_GET(val)   ((((val) & PORT_P0_IRNCR_EXINT8) >> 15) & 0x1)
/* EXINT7 Interrupt Request (14) */
#define PORT_P0_IRNCR_EXINT7   (0x1 << 14)
#define PORT_P0_IRNCR_EXINT7_GET(val)   ((((val) & PORT_P0_IRNCR_EXINT7) >> 14) & 0x1)
/* EXINT6 Interrupt Request (13) */
#define PORT_P0_IRNCR_EXINT6   (0x1 << 13)
#define PORT_P0_IRNCR_EXINT6_GET(val)   ((((val) & PORT_P0_IRNCR_EXINT6) >> 13) & 0x1)
/* EXINT5 Interrupt Request (12) */
#define PORT_P0_IRNCR_EXINT5   (0x1 << 12)
#define PORT_P0_IRNCR_EXINT5_GET(val)   ((((val) & PORT_P0_IRNCR_EXINT5) >> 12) & 0x1)
/* EXINT4 Interrupt Request (11) */
#define PORT_P0_IRNCR_EXINT4   (0x1 << 11)
#define PORT_P0_IRNCR_EXINT4_GET(val)   ((((val) & PORT_P0_IRNCR_EXINT4) >> 11) & 0x1)
/* EXINT3 Interrupt Request (10) */
#define PORT_P0_IRNCR_EXINT3   (0x1 << 10)
#define PORT_P0_IRNCR_EXINT3_GET(val)   ((((val) & PORT_P0_IRNCR_EXINT3) >> 10) & 0x1)
/* EXINT2 Interrupt Request (9) */
#define PORT_P0_IRNCR_EXINT2   (0x1 << 9)
#define PORT_P0_IRNCR_EXINT2_GET(val)   ((((val) & PORT_P0_IRNCR_EXINT2) >> 9) & 0x1)
/* EXINT1 Interrupt Request (8) */
#define PORT_P0_IRNCR_EXINT1   (0x1 << 8)
#define PORT_P0_IRNCR_EXINT1_GET(val)   ((((val) & PORT_P0_IRNCR_EXINT1) >> 8) & 0x1)
/* EXINT0 Interrupt Request (7) */
#define PORT_P0_IRNCR_EXINT0   (0x1 << 7)
#define PORT_P0_IRNCR_EXINT0_GET(val)   ((((val) & PORT_P0_IRNCR_EXINT0) >> 7) & 0x1)

/*******************************************************************************
 * P0 External Event Detection Configuration Register
 ******************************************************************************/

/* EXINT16 configured for Edge or Level Detection (19) */
#define PORT_P0_IRNCFG_EXINT16   (0x1 << 19)
#define PORT_P0_IRNCFG_EXINT16_VAL(val)   (((val) & 0x1) << 19)
#define PORT_P0_IRNCFG_EXINT16_GET(val)   ((((val) & PORT_P0_IRNCFG_EXINT16) >> 19) & 0x1)
#define PORT_P0_IRNCFG_EXINT16_SET(reg,val) (reg) = ((reg & ~PORT_P0_IRNCFG_EXINT16) | (((val) & 0x1) << 19))
/* EXINT10 configured for Edge or Level Detection (17) */
#define PORT_P0_IRNCFG_EXINT10   (0x1 << 17)
#define PORT_P0_IRNCFG_EXINT10_VAL(val)   (((val) & 0x1) << 17)
#define PORT_P0_IRNCFG_EXINT10_GET(val)   ((((val) & PORT_P0_IRNCFG_EXINT10) >> 17) & 0x1)
#define PORT_P0_IRNCFG_EXINT10_SET(reg,val) (reg) = ((reg & ~PORT_P0_IRNCFG_EXINT10) | (((val) & 0x1) << 17))
/* EXINT9 configured for Edge or Level Detection (16) */
#define PORT_P0_IRNCFG_EXINT9   (0x1 << 16)
#define PORT_P0_IRNCFG_EXINT9_VAL(val)   (((val) & 0x1) << 16)
#define PORT_P0_IRNCFG_EXINT9_GET(val)   ((((val) & PORT_P0_IRNCFG_EXINT9) >> 16) & 0x1)
#define PORT_P0_IRNCFG_EXINT9_SET(reg,val) (reg) = ((reg & ~PORT_P0_IRNCFG_EXINT9) | (((val) & 0x1) << 16))
/* EXINT8 configured for Edge or Level Detection (15) */
#define PORT_P0_IRNCFG_EXINT8   (0x1 << 15)
#define PORT_P0_IRNCFG_EXINT8_VAL(val)   (((val) & 0x1) << 15)
#define PORT_P0_IRNCFG_EXINT8_GET(val)   ((((val) & PORT_P0_IRNCFG_EXINT8) >> 15) & 0x1)
#define PORT_P0_IRNCFG_EXINT8_SET(reg,val) (reg) = ((reg & ~PORT_P0_IRNCFG_EXINT8) | (((val) & 0x1) << 15))
/* EXINT7 configured for Edge or Level Detection (14) */
#define PORT_P0_IRNCFG_EXINT7   (0x1 << 14)
#define PORT_P0_IRNCFG_EXINT7_VAL(val)   (((val) & 0x1) << 14)
#define PORT_P0_IRNCFG_EXINT7_GET(val)   ((((val) & PORT_P0_IRNCFG_EXINT7) >> 14) & 0x1)
#define PORT_P0_IRNCFG_EXINT7_SET(reg,val) (reg) = ((reg & ~PORT_P0_IRNCFG_EXINT7) | (((val) & 0x1) << 14))
/* EXINT6 configured for Edge or Level Detection (13) */
#define PORT_P0_IRNCFG_EXINT6   (0x1 << 13)
#define PORT_P0_IRNCFG_EXINT6_VAL(val)   (((val) & 0x1) << 13)
#define PORT_P0_IRNCFG_EXINT6_GET(val)   ((((val) & PORT_P0_IRNCFG_EXINT6) >> 13) & 0x1)
#define PORT_P0_IRNCFG_EXINT6_SET(reg,val) (reg) = ((reg & ~PORT_P0_IRNCFG_EXINT6) | (((val) & 0x1) << 13))
/* EXINT5 configured for Edge or Level Detection (12) */
#define PORT_P0_IRNCFG_EXINT5   (0x1 << 12)
#define PORT_P0_IRNCFG_EXINT5_VAL(val)   (((val) & 0x1) << 12)
#define PORT_P0_IRNCFG_EXINT5_GET(val)   ((((val) & PORT_P0_IRNCFG_EXINT5) >> 12) & 0x1)
#define PORT_P0_IRNCFG_EXINT5_SET(reg,val) (reg) = ((reg & ~PORT_P0_IRNCFG_EXINT5) | (((val) & 0x1) << 12))
/* EXINT4 configured for Edge or Level Detection (11) */
#define PORT_P0_IRNCFG_EXINT4   (0x1 << 11)
#define PORT_P0_IRNCFG_EXINT4_VAL(val)   (((val) & 0x1) << 11)
#define PORT_P0_IRNCFG_EXINT4_GET(val)   ((((val) & PORT_P0_IRNCFG_EXINT4) >> 11) & 0x1)
#define PORT_P0_IRNCFG_EXINT4_SET(reg,val) (reg) = ((reg & ~PORT_P0_IRNCFG_EXINT4) | (((val) & 0x1) << 11))
/* EXINT3 configured for Edge or Level Detection (10) */
#define PORT_P0_IRNCFG_EXINT3   (0x1 << 10)
#define PORT_P0_IRNCFG_EXINT3_VAL(val)   (((val) & 0x1) << 10)
#define PORT_P0_IRNCFG_EXINT3_GET(val)   ((((val) & PORT_P0_IRNCFG_EXINT3) >> 10) & 0x1)
#define PORT_P0_IRNCFG_EXINT3_SET(reg,val) (reg) = ((reg & ~PORT_P0_IRNCFG_EXINT3) | (((val) & 0x1) << 10))
/* EXINT2 configured for Edge or Level Detection (9) */
#define PORT_P0_IRNCFG_EXINT2   (0x1 << 9)
#define PORT_P0_IRNCFG_EXINT2_VAL(val)   (((val) & 0x1) << 9)
#define PORT_P0_IRNCFG_EXINT2_GET(val)   ((((val) & PORT_P0_IRNCFG_EXINT2) >> 9) & 0x1)
#define PORT_P0_IRNCFG_EXINT2_SET(reg,val) (reg) = ((reg & ~PORT_P0_IRNCFG_EXINT2) | (((val) & 0x1) << 9))
/* EXINT1 configured for Edge or Level Detection (8) */
#define PORT_P0_IRNCFG_EXINT1   (0x1 << 8)
#define PORT_P0_IRNCFG_EXINT1_VAL(val)   (((val) & 0x1) << 8)
#define PORT_P0_IRNCFG_EXINT1_GET(val)   ((((val) & PORT_P0_IRNCFG_EXINT1) >> 8) & 0x1)
#define PORT_P0_IRNCFG_EXINT1_SET(reg,val) (reg) = ((reg & ~PORT_P0_IRNCFG_EXINT1) | (((val) & 0x1) << 8))
/* EXINT0 configured for Edge or Level Detection (7) */
#define PORT_P0_IRNCFG_EXINT0   (0x1 << 7)
#define PORT_P0_IRNCFG_EXINT0_VAL(val)   (((val) & 0x1) << 7)
#define PORT_P0_IRNCFG_EXINT0_GET(val)   ((((val) & PORT_P0_IRNCFG_EXINT0) >> 7) & 0x1)
#define PORT_P0_IRNCFG_EXINT0_SET(reg,val) (reg) = ((reg & ~PORT_P0_IRNCFG_EXINT0) | (((val) & 0x1) << 7))

/*******************************************************************************
 * P0_IRNENSET Register
 ******************************************************************************/

/* Set Interrupt Node Enable Flag EXINT16 (19) */
#define PORT_P0_IRNENSET_EXINT16   (0x1 << 19)
#define PORT_P0_IRNENSET_EXINT16_VAL(val)   (((val) & 0x1) << 19)
#define PORT_P0_IRNENSET_EXINT16_SET(reg,val) (reg) = (((reg & ~PORT_P0_IRNENSET_EXINT16) | (val) & 1) << 19)
/* Set Interrupt Node Enable Flag EXINT10 (17) */
#define PORT_P0_IRNENSET_EXINT10   (0x1 << 17)
#define PORT_P0_IRNENSET_EXINT10_VAL(val)   (((val) & 0x1) << 17)
#define PORT_P0_IRNENSET_EXINT10_SET(reg,val) (reg) = (((reg & ~PORT_P0_IRNENSET_EXINT10) | (val) & 1) << 17)
/* Set Interrupt Node Enable Flag EXINT9 (16) */
#define PORT_P0_IRNENSET_EXINT9   (0x1 << 16)
#define PORT_P0_IRNENSET_EXINT9_VAL(val)   (((val) & 0x1) << 16)
#define PORT_P0_IRNENSET_EXINT9_SET(reg,val) (reg) = (((reg & ~PORT_P0_IRNENSET_EXINT9) | (val) & 1) << 16)
/* Set Interrupt Node Enable Flag EXINT8 (15) */
#define PORT_P0_IRNENSET_EXINT8   (0x1 << 15)
#define PORT_P0_IRNENSET_EXINT8_VAL(val)   (((val) & 0x1) << 15)
#define PORT_P0_IRNENSET_EXINT8_SET(reg,val) (reg) = (((reg & ~PORT_P0_IRNENSET_EXINT8) | (val) & 1) << 15)
/* Set Interrupt Node Enable Flag EXINT7 (14) */
#define PORT_P0_IRNENSET_EXINT7   (0x1 << 14)
#define PORT_P0_IRNENSET_EXINT7_VAL(val)   (((val) & 0x1) << 14)
#define PORT_P0_IRNENSET_EXINT7_SET(reg,val) (reg) = (((reg & ~PORT_P0_IRNENSET_EXINT7) | (val) & 1) << 14)
/* Set Interrupt Node Enable Flag EXINT6 (13) */
#define PORT_P0_IRNENSET_EXINT6   (0x1 << 13)
#define PORT_P0_IRNENSET_EXINT6_VAL(val)   (((val) & 0x1) << 13)
#define PORT_P0_IRNENSET_EXINT6_SET(reg,val) (reg) = (((reg & ~PORT_P0_IRNENSET_EXINT6) | (val) & 1) << 13)
/* Set Interrupt Node Enable Flag EXINT5 (12) */
#define PORT_P0_IRNENSET_EXINT5   (0x1 << 12)
#define PORT_P0_IRNENSET_EXINT5_VAL(val)   (((val) & 0x1) << 12)
#define PORT_P0_IRNENSET_EXINT5_SET(reg,val) (reg) = (((reg & ~PORT_P0_IRNENSET_EXINT5) | (val) & 1) << 12)
/* Set Interrupt Node Enable Flag EXINT4 (11) */
#define PORT_P0_IRNENSET_EXINT4   (0x1 << 11)
#define PORT_P0_IRNENSET_EXINT4_VAL(val)   (((val) & 0x1) << 11)
#define PORT_P0_IRNENSET_EXINT4_SET(reg,val) (reg) = (((reg & ~PORT_P0_IRNENSET_EXINT4) | (val) & 1) << 11)
/* Set Interrupt Node Enable Flag EXINT3 (10) */
#define PORT_P0_IRNENSET_EXINT3   (0x1 << 10)
#define PORT_P0_IRNENSET_EXINT3_VAL(val)   (((val) & 0x1) << 10)
#define PORT_P0_IRNENSET_EXINT3_SET(reg,val) (reg) = (((reg & ~PORT_P0_IRNENSET_EXINT3) | (val) & 1) << 10)
/* Set Interrupt Node Enable Flag EXINT2 (9) */
#define PORT_P0_IRNENSET_EXINT2   (0x1 << 9)
#define PORT_P0_IRNENSET_EXINT2_VAL(val)   (((val) & 0x1) << 9)
#define PORT_P0_IRNENSET_EXINT2_SET(reg,val) (reg) = (((reg & ~PORT_P0_IRNENSET_EXINT2) | (val) & 1) << 9)
/* Set Interrupt Node Enable Flag EXINT1 (8) */
#define PORT_P0_IRNENSET_EXINT1   (0x1 << 8)
#define PORT_P0_IRNENSET_EXINT1_VAL(val)   (((val) & 0x1) << 8)
#define PORT_P0_IRNENSET_EXINT1_SET(reg,val) (reg) = (((reg & ~PORT_P0_IRNENSET_EXINT1) | (val) & 1) << 8)
/* Set Interrupt Node Enable Flag EXINT0 (7) */
#define PORT_P0_IRNENSET_EXINT0   (0x1 << 7)
#define PORT_P0_IRNENSET_EXINT0_VAL(val)   (((val) & 0x1) << 7)
#define PORT_P0_IRNENSET_EXINT0_SET(reg,val) (reg) = (((reg & ~PORT_P0_IRNENSET_EXINT0) | (val) & 1) << 7)

/*******************************************************************************
 * P0_IRNENCLR Register
 ******************************************************************************/

/* Clear Interrupt Node Enable Flag EXINT16 (19) */
#define PORT_P0_IRNENCLR_EXINT16   (0x1 << 19)
#define PORT_P0_IRNENCLR_EXINT16_VAL(val)   (((val) & 0x1) << 19)
#define PORT_P0_IRNENCLR_EXINT16_SET(reg,val) (reg) = (((reg & ~PORT_P0_IRNENCLR_EXINT16) | (val) & 1) << 19)
/* Clear Interrupt Node Enable Flag EXINT10 (17) */
#define PORT_P0_IRNENCLR_EXINT10   (0x1 << 17)
#define PORT_P0_IRNENCLR_EXINT10_VAL(val)   (((val) & 0x1) << 17)
#define PORT_P0_IRNENCLR_EXINT10_SET(reg,val) (reg) = (((reg & ~PORT_P0_IRNENCLR_EXINT10) | (val) & 1) << 17)
/* Clear Interrupt Node Enable Flag EXINT9 (16) */
#define PORT_P0_IRNENCLR_EXINT9   (0x1 << 16)
#define PORT_P0_IRNENCLR_EXINT9_VAL(val)   (((val) & 0x1) << 16)
#define PORT_P0_IRNENCLR_EXINT9_SET(reg,val) (reg) = (((reg & ~PORT_P0_IRNENCLR_EXINT9) | (val) & 1) << 16)
/* Clear Interrupt Node Enable Flag EXINT8 (15) */
#define PORT_P0_IRNENCLR_EXINT8   (0x1 << 15)
#define PORT_P0_IRNENCLR_EXINT8_VAL(val)   (((val) & 0x1) << 15)
#define PORT_P0_IRNENCLR_EXINT8_SET(reg,val) (reg) = (((reg & ~PORT_P0_IRNENCLR_EXINT8) | (val) & 1) << 15)
/* Clear Interrupt Node Enable Flag EXINT7 (14) */
#define PORT_P0_IRNENCLR_EXINT7   (0x1 << 14)
#define PORT_P0_IRNENCLR_EXINT7_VAL(val)   (((val) & 0x1) << 14)
#define PORT_P0_IRNENCLR_EXINT7_SET(reg,val) (reg) = (((reg & ~PORT_P0_IRNENCLR_EXINT7) | (val) & 1) << 14)
/* Clear Interrupt Node Enable Flag EXINT6 (13) */
#define PORT_P0_IRNENCLR_EXINT6   (0x1 << 13)
#define PORT_P0_IRNENCLR_EXINT6_VAL(val)   (((val) & 0x1) << 13)
#define PORT_P0_IRNENCLR_EXINT6_SET(reg,val) (reg) = (((reg & ~PORT_P0_IRNENCLR_EXINT6) | (val) & 1) << 13)
/* Clear Interrupt Node Enable Flag EXINT5 (12) */
#define PORT_P0_IRNENCLR_EXINT5   (0x1 << 12)
#define PORT_P0_IRNENCLR_EXINT5_VAL(val)   (((val) & 0x1) << 12)
#define PORT_P0_IRNENCLR_EXINT5_SET(reg,val) (reg) = (((reg & ~PORT_P0_IRNENCLR_EXINT5) | (val) & 1) << 12)
/* Clear Interrupt Node Enable Flag EXINT4 (11) */
#define PORT_P0_IRNENCLR_EXINT4   (0x1 << 11)
#define PORT_P0_IRNENCLR_EXINT4_VAL(val)   (((val) & 0x1) << 11)
#define PORT_P0_IRNENCLR_EXINT4_SET(reg,val) (reg) = (((reg & ~PORT_P0_IRNENCLR_EXINT4) | (val) & 1) << 11)
/* Clear Interrupt Node Enable Flag EXINT3 (10) */
#define PORT_P0_IRNENCLR_EXINT3   (0x1 << 10)
#define PORT_P0_IRNENCLR_EXINT3_VAL(val)   (((val) & 0x1) << 10)
#define PORT_P0_IRNENCLR_EXINT3_SET(reg,val) (reg) = (((reg & ~PORT_P0_IRNENCLR_EXINT3) | (val) & 1) << 10)
/* Clear Interrupt Node Enable Flag EXINT2 (9) */
#define PORT_P0_IRNENCLR_EXINT2   (0x1 << 9)
#define PORT_P0_IRNENCLR_EXINT2_VAL(val)   (((val) & 0x1) << 9)
#define PORT_P0_IRNENCLR_EXINT2_SET(reg,val) (reg) = (((reg & ~PORT_P0_IRNENCLR_EXINT2) | (val) & 1) << 9)
/* Clear Interrupt Node Enable Flag EXINT1 (8) */
#define PORT_P0_IRNENCLR_EXINT1   (0x1 << 8)
#define PORT_P0_IRNENCLR_EXINT1_VAL(val)   (((val) & 0x1) << 8)
#define PORT_P0_IRNENCLR_EXINT1_SET(reg,val) (reg) = (((reg & ~PORT_P0_IRNENCLR_EXINT1) | (val) & 1) << 8)
/* Clear Interrupt Node Enable Flag EXINT0 (7) */
#define PORT_P0_IRNENCLR_EXINT0   (0x1 << 7)
#define PORT_P0_IRNENCLR_EXINT0_VAL(val)   (((val) & 0x1) << 7)
#define PORT_P0_IRNENCLR_EXINT0_SET(reg,val) (reg) = (((reg & ~PORT_P0_IRNENCLR_EXINT0) | (val) & 1) << 7)

/*******************************************************************************
 * Port 1 Data Output Register
 ******************************************************************************/

/* Port 1 Pin # Output Value (19) */
#define PORT_P1_OUT_P19   (0x1 << 19)
#define PORT_P1_OUT_P19_VAL(val)   (((val) & 0x1) << 19)
#define PORT_P1_OUT_P19_GET(val)   ((((val) & PORT_P1_OUT_P19) >> 19) & 0x1)
#define PORT_P1_OUT_P19_SET(reg,val) (reg) = ((reg & ~PORT_P1_OUT_P19) | (((val) & 0x1) << 19))
/* Port 1 Pin # Output Value (18) */
#define PORT_P1_OUT_P18   (0x1 << 18)
#define PORT_P1_OUT_P18_VAL(val)   (((val) & 0x1) << 18)
#define PORT_P1_OUT_P18_GET(val)   ((((val) & PORT_P1_OUT_P18) >> 18) & 0x1)
#define PORT_P1_OUT_P18_SET(reg,val) (reg) = ((reg & ~PORT_P1_OUT_P18) | (((val) & 0x1) << 18))
/* Port 1 Pin # Output Value (17) */
#define PORT_P1_OUT_P17   (0x1 << 17)
#define PORT_P1_OUT_P17_VAL(val)   (((val) & 0x1) << 17)
#define PORT_P1_OUT_P17_GET(val)   ((((val) & PORT_P1_OUT_P17) >> 17) & 0x1)
#define PORT_P1_OUT_P17_SET(reg,val) (reg) = ((reg & ~PORT_P1_OUT_P17) | (((val) & 0x1) << 17))
/* Port 1 Pin # Output Value (16) */
#define PORT_P1_OUT_P16   (0x1 << 16)
#define PORT_P1_OUT_P16_VAL(val)   (((val) & 0x1) << 16)
#define PORT_P1_OUT_P16_GET(val)   ((((val) & PORT_P1_OUT_P16) >> 16) & 0x1)
#define PORT_P1_OUT_P16_SET(reg,val) (reg) = ((reg & ~PORT_P1_OUT_P16) | (((val) & 0x1) << 16))
/* Port 1 Pin # Output Value (15) */
#define PORT_P1_OUT_P15   (0x1 << 15)
#define PORT_P1_OUT_P15_VAL(val)   (((val) & 0x1) << 15)
#define PORT_P1_OUT_P15_GET(val)   ((((val) & PORT_P1_OUT_P15) >> 15) & 0x1)
#define PORT_P1_OUT_P15_SET(reg,val) (reg) = ((reg & ~PORT_P1_OUT_P15) | (((val) & 0x1) << 15))
/* Port 1 Pin # Output Value (14) */
#define PORT_P1_OUT_P14   (0x1 << 14)
#define PORT_P1_OUT_P14_VAL(val)   (((val) & 0x1) << 14)
#define PORT_P1_OUT_P14_GET(val)   ((((val) & PORT_P1_OUT_P14) >> 14) & 0x1)
#define PORT_P1_OUT_P14_SET(reg,val) (reg) = ((reg & ~PORT_P1_OUT_P14) | (((val) & 0x1) << 14))
/* Port 1 Pin # Output Value (13) */
#define PORT_P1_OUT_P13   (0x1 << 13)
#define PORT_P1_OUT_P13_VAL(val)   (((val) & 0x1) << 13)
#define PORT_P1_OUT_P13_GET(val)   ((((val) & PORT_P1_OUT_P13) >> 13) & 0x1)
#define PORT_P1_OUT_P13_SET(reg,val) (reg) = ((reg & ~PORT_P1_OUT_P13) | (((val) & 0x1) << 13))
/* Port 1 Pin # Output Value (12) */
#define PORT_P1_OUT_P12   (0x1 << 12)
#define PORT_P1_OUT_P12_VAL(val)   (((val) & 0x1) << 12)
#define PORT_P1_OUT_P12_GET(val)   ((((val) & PORT_P1_OUT_P12) >> 12) & 0x1)
#define PORT_P1_OUT_P12_SET(reg,val) (reg) = ((reg & ~PORT_P1_OUT_P12) | (((val) & 0x1) << 12))
/* Port 1 Pin # Output Value (11) */
#define PORT_P1_OUT_P11   (0x1 << 11)
#define PORT_P1_OUT_P11_VAL(val)   (((val) & 0x1) << 11)
#define PORT_P1_OUT_P11_GET(val)   ((((val) & PORT_P1_OUT_P11) >> 11) & 0x1)
#define PORT_P1_OUT_P11_SET(reg,val) (reg) = ((reg & ~PORT_P1_OUT_P11) | (((val) & 0x1) << 11))
/* Port 1 Pin # Output Value (10) */
#define PORT_P1_OUT_P10   (0x1 << 10)
#define PORT_P1_OUT_P10_VAL(val)   (((val) & 0x1) << 10)
#define PORT_P1_OUT_P10_GET(val)   ((((val) & PORT_P1_OUT_P10) >> 10) & 0x1)
#define PORT_P1_OUT_P10_SET(reg,val) (reg) = ((reg & ~PORT_P1_OUT_P10) | (((val) & 0x1) << 10))
/* Port 1 Pin # Output Value (9) */
#define PORT_P1_OUT_P9   (0x1 << 9)
#define PORT_P1_OUT_P9_VAL(val)   (((val) & 0x1) << 9)
#define PORT_P1_OUT_P9_GET(val)   ((((val) & PORT_P1_OUT_P9) >> 9) & 0x1)
#define PORT_P1_OUT_P9_SET(reg,val) (reg) = ((reg & ~PORT_P1_OUT_P9) | (((val) & 0x1) << 9))
/* Port 1 Pin # Output Value (8) */
#define PORT_P1_OUT_P8   (0x1 << 8)
#define PORT_P1_OUT_P8_VAL(val)   (((val) & 0x1) << 8)
#define PORT_P1_OUT_P8_GET(val)   ((((val) & PORT_P1_OUT_P8) >> 8) & 0x1)
#define PORT_P1_OUT_P8_SET(reg,val) (reg) = ((reg & ~PORT_P1_OUT_P8) | (((val) & 0x1) << 8))
/* Port 1 Pin # Output Value (7) */
#define PORT_P1_OUT_P7   (0x1 << 7)
#define PORT_P1_OUT_P7_VAL(val)   (((val) & 0x1) << 7)
#define PORT_P1_OUT_P7_GET(val)   ((((val) & PORT_P1_OUT_P7) >> 7) & 0x1)
#define PORT_P1_OUT_P7_SET(reg,val) (reg) = ((reg & ~PORT_P1_OUT_P7) | (((val) & 0x1) << 7))
/* Port 1 Pin # Output Value (6) */
#define PORT_P1_OUT_P6   (0x1 << 6)
#define PORT_P1_OUT_P6_VAL(val)   (((val) & 0x1) << 6)
#define PORT_P1_OUT_P6_GET(val)   ((((val) & PORT_P1_OUT_P6) >> 6) & 0x1)
#define PORT_P1_OUT_P6_SET(reg,val) (reg) = ((reg & ~PORT_P1_OUT_P6) | (((val) & 0x1) << 6))
/* Port 1 Pin # Output Value (5) */
#define PORT_P1_OUT_P5   (0x1 << 5)
#define PORT_P1_OUT_P5_VAL(val)   (((val) & 0x1) << 5)
#define PORT_P1_OUT_P5_GET(val)   ((((val) & PORT_P1_OUT_P5) >> 5) & 0x1)
#define PORT_P1_OUT_P5_SET(reg,val) (reg) = ((reg & ~PORT_P1_OUT_P5) | (((val) & 0x1) << 5))
/* Port 1 Pin # Output Value (4) */
#define PORT_P1_OUT_P4   (0x1 << 4)
#define PORT_P1_OUT_P4_VAL(val)   (((val) & 0x1) << 4)
#define PORT_P1_OUT_P4_GET(val)   ((((val) & PORT_P1_OUT_P4) >> 4) & 0x1)
#define PORT_P1_OUT_P4_SET(reg,val) (reg) = ((reg & ~PORT_P1_OUT_P4) | (((val) & 0x1) << 4))
/* Port 1 Pin # Output Value (3) */
#define PORT_P1_OUT_P3   (0x1 << 3)
#define PORT_P1_OUT_P3_VAL(val)   (((val) & 0x1) << 3)
#define PORT_P1_OUT_P3_GET(val)   ((((val) & PORT_P1_OUT_P3) >> 3) & 0x1)
#define PORT_P1_OUT_P3_SET(reg,val) (reg) = ((reg & ~PORT_P1_OUT_P3) | (((val) & 0x1) << 3))
/* Port 1 Pin # Output Value (2) */
#define PORT_P1_OUT_P2   (0x1 << 2)
#define PORT_P1_OUT_P2_VAL(val)   (((val) & 0x1) << 2)
#define PORT_P1_OUT_P2_GET(val)   ((((val) & PORT_P1_OUT_P2) >> 2) & 0x1)
#define PORT_P1_OUT_P2_SET(reg,val) (reg) = ((reg & ~PORT_P1_OUT_P2) | (((val) & 0x1) << 2))
/* Port 1 Pin # Output Value (1) */
#define PORT_P1_OUT_P1   (0x1 << 1)
#define PORT_P1_OUT_P1_VAL(val)   (((val) & 0x1) << 1)
#define PORT_P1_OUT_P1_GET(val)   ((((val) & PORT_P1_OUT_P1) >> 1) & 0x1)
#define PORT_P1_OUT_P1_SET(reg,val) (reg) = ((reg & ~PORT_P1_OUT_P1) | (((val) & 0x1) << 1))
/* Port 1 Pin # Output Value (0) */
#define PORT_P1_OUT_P0   (0x1)
#define PORT_P1_OUT_P0_VAL(val)   (((val) & 0x1) << 0)
#define PORT_P1_OUT_P0_GET(val)   ((((val) & PORT_P1_OUT_P0) >> 0) & 0x1)
#define PORT_P1_OUT_P0_SET(reg,val) (reg) = ((reg & ~PORT_P1_OUT_P0) | (((val) & 0x1) << 0))

/*******************************************************************************
 * Port 1 Data Input Register
 ******************************************************************************/

/* Port 1 Pin # Latched Input Value (19) */
#define PORT_P1_IN_P19   (0x1 << 19)
#define PORT_P1_IN_P19_GET(val)   ((((val) & PORT_P1_IN_P19) >> 19) & 0x1)
/* Port 1 Pin # Latched Input Value (18) */
#define PORT_P1_IN_P18   (0x1 << 18)
#define PORT_P1_IN_P18_GET(val)   ((((val) & PORT_P1_IN_P18) >> 18) & 0x1)
/* Port 1 Pin # Latched Input Value (17) */
#define PORT_P1_IN_P17   (0x1 << 17)
#define PORT_P1_IN_P17_GET(val)   ((((val) & PORT_P1_IN_P17) >> 17) & 0x1)
/* Port 1 Pin # Latched Input Value (16) */
#define PORT_P1_IN_P16   (0x1 << 16)
#define PORT_P1_IN_P16_GET(val)   ((((val) & PORT_P1_IN_P16) >> 16) & 0x1)
/* Port 1 Pin # Latched Input Value (15) */
#define PORT_P1_IN_P15   (0x1 << 15)
#define PORT_P1_IN_P15_GET(val)   ((((val) & PORT_P1_IN_P15) >> 15) & 0x1)
/* Port 1 Pin # Latched Input Value (14) */
#define PORT_P1_IN_P14   (0x1 << 14)
#define PORT_P1_IN_P14_GET(val)   ((((val) & PORT_P1_IN_P14) >> 14) & 0x1)
/* Port 1 Pin # Latched Input Value (13) */
#define PORT_P1_IN_P13   (0x1 << 13)
#define PORT_P1_IN_P13_GET(val)   ((((val) & PORT_P1_IN_P13) >> 13) & 0x1)
/* Port 1 Pin # Latched Input Value (12) */
#define PORT_P1_IN_P12   (0x1 << 12)
#define PORT_P1_IN_P12_GET(val)   ((((val) & PORT_P1_IN_P12) >> 12) & 0x1)
/* Port 1 Pin # Latched Input Value (11) */
#define PORT_P1_IN_P11   (0x1 << 11)
#define PORT_P1_IN_P11_GET(val)   ((((val) & PORT_P1_IN_P11) >> 11) & 0x1)
/* Port 1 Pin # Latched Input Value (10) */
#define PORT_P1_IN_P10   (0x1 << 10)
#define PORT_P1_IN_P10_GET(val)   ((((val) & PORT_P1_IN_P10) >> 10) & 0x1)
/* Port 1 Pin # Latched Input Value (9) */
#define PORT_P1_IN_P9   (0x1 << 9)
#define PORT_P1_IN_P9_GET(val)   ((((val) & PORT_P1_IN_P9) >> 9) & 0x1)
/* Port 1 Pin # Latched Input Value (8) */
#define PORT_P1_IN_P8   (0x1 << 8)
#define PORT_P1_IN_P8_GET(val)   ((((val) & PORT_P1_IN_P8) >> 8) & 0x1)
/* Port 1 Pin # Latched Input Value (7) */
#define PORT_P1_IN_P7   (0x1 << 7)
#define PORT_P1_IN_P7_GET(val)   ((((val) & PORT_P1_IN_P7) >> 7) & 0x1)
/* Port 1 Pin # Latched Input Value (6) */
#define PORT_P1_IN_P6   (0x1 << 6)
#define PORT_P1_IN_P6_GET(val)   ((((val) & PORT_P1_IN_P6) >> 6) & 0x1)
/* Port 1 Pin # Latched Input Value (5) */
#define PORT_P1_IN_P5   (0x1 << 5)
#define PORT_P1_IN_P5_GET(val)   ((((val) & PORT_P1_IN_P5) >> 5) & 0x1)
/* Port 1 Pin # Latched Input Value (4) */
#define PORT_P1_IN_P4   (0x1 << 4)
#define PORT_P1_IN_P4_GET(val)   ((((val) & PORT_P1_IN_P4) >> 4) & 0x1)
/* Port 1 Pin # Latched Input Value (3) */
#define PORT_P1_IN_P3   (0x1 << 3)
#define PORT_P1_IN_P3_GET(val)   ((((val) & PORT_P1_IN_P3) >> 3) & 0x1)
/* Port 1 Pin # Latched Input Value (2) */
#define PORT_P1_IN_P2   (0x1 << 2)
#define PORT_P1_IN_P2_GET(val)   ((((val) & PORT_P1_IN_P2) >> 2) & 0x1)
/* Port 1 Pin # Latched Input Value (1) */
#define PORT_P1_IN_P1   (0x1 << 1)
#define PORT_P1_IN_P1_GET(val)   ((((val) & PORT_P1_IN_P1) >> 1) & 0x1)
/* Port 1 Pin # Latched Input Value (0) */
#define PORT_P1_IN_P0   (0x1)
#define PORT_P1_IN_P0_GET(val)   ((((val) & PORT_P1_IN_P0) >> 0) & 0x1)

/*******************************************************************************
 * Port 1 Direction Register
 ******************************************************************************/

/* Port 1 Pin #Direction Control (19) */
#define PORT_P1_DIR_P19   (0x1 << 19)
#define PORT_P1_DIR_P19_VAL(val)   (((val) & 0x1) << 19)
#define PORT_P1_DIR_P19_GET(val)   ((((val) & PORT_P1_DIR_P19) >> 19) & 0x1)
#define PORT_P1_DIR_P19_SET(reg,val) (reg) = ((reg & ~PORT_P1_DIR_P19) | (((val) & 0x1) << 19))
/* Port 1 Pin #Direction Control (18) */
#define PORT_P1_DIR_P18   (0x1 << 18)
#define PORT_P1_DIR_P18_VAL(val)   (((val) & 0x1) << 18)
#define PORT_P1_DIR_P18_GET(val)   ((((val) & PORT_P1_DIR_P18) >> 18) & 0x1)
#define PORT_P1_DIR_P18_SET(reg,val) (reg) = ((reg & ~PORT_P1_DIR_P18) | (((val) & 0x1) << 18))
/* Port 1 Pin #Direction Control (17) */
#define PORT_P1_DIR_P17   (0x1 << 17)
#define PORT_P1_DIR_P17_VAL(val)   (((val) & 0x1) << 17)
#define PORT_P1_DIR_P17_GET(val)   ((((val) & PORT_P1_DIR_P17) >> 17) & 0x1)
#define PORT_P1_DIR_P17_SET(reg,val) (reg) = ((reg & ~PORT_P1_DIR_P17) | (((val) & 0x1) << 17))
/* Port 1 Pin #Direction Control (16) */
#define PORT_P1_DIR_P16   (0x1 << 16)
#define PORT_P1_DIR_P16_VAL(val)   (((val) & 0x1) << 16)
#define PORT_P1_DIR_P16_GET(val)   ((((val) & PORT_P1_DIR_P16) >> 16) & 0x1)
#define PORT_P1_DIR_P16_SET(reg,val) (reg) = ((reg & ~PORT_P1_DIR_P16) | (((val) & 0x1) << 16))
/* Port 1 Pin #Direction Control (15) */
#define PORT_P1_DIR_P15   (0x1 << 15)
#define PORT_P1_DIR_P15_VAL(val)   (((val) & 0x1) << 15)
#define PORT_P1_DIR_P15_GET(val)   ((((val) & PORT_P1_DIR_P15) >> 15) & 0x1)
#define PORT_P1_DIR_P15_SET(reg,val) (reg) = ((reg & ~PORT_P1_DIR_P15) | (((val) & 0x1) << 15))
/* Port 1 Pin #Direction Control (14) */
#define PORT_P1_DIR_P14   (0x1 << 14)
#define PORT_P1_DIR_P14_VAL(val)   (((val) & 0x1) << 14)
#define PORT_P1_DIR_P14_GET(val)   ((((val) & PORT_P1_DIR_P14) >> 14) & 0x1)
#define PORT_P1_DIR_P14_SET(reg,val) (reg) = ((reg & ~PORT_P1_DIR_P14) | (((val) & 0x1) << 14))
/* Port 1 Pin #Direction Control (13) */
#define PORT_P1_DIR_P13   (0x1 << 13)
#define PORT_P1_DIR_P13_VAL(val)   (((val) & 0x1) << 13)
#define PORT_P1_DIR_P13_GET(val)   ((((val) & PORT_P1_DIR_P13) >> 13) & 0x1)
#define PORT_P1_DIR_P13_SET(reg,val) (reg) = ((reg & ~PORT_P1_DIR_P13) | (((val) & 0x1) << 13))
/* Port 1 Pin #Direction Control (12) */
#define PORT_P1_DIR_P12   (0x1 << 12)
#define PORT_P1_DIR_P12_VAL(val)   (((val) & 0x1) << 12)
#define PORT_P1_DIR_P12_GET(val)   ((((val) & PORT_P1_DIR_P12) >> 12) & 0x1)
#define PORT_P1_DIR_P12_SET(reg,val) (reg) = ((reg & ~PORT_P1_DIR_P12) | (((val) & 0x1) << 12))
/* Port 1 Pin #Direction Control (11) */
#define PORT_P1_DIR_P11   (0x1 << 11)
#define PORT_P1_DIR_P11_VAL(val)   (((val) & 0x1) << 11)
#define PORT_P1_DIR_P11_GET(val)   ((((val) & PORT_P1_DIR_P11) >> 11) & 0x1)
#define PORT_P1_DIR_P11_SET(reg,val) (reg) = ((reg & ~PORT_P1_DIR_P11) | (((val) & 0x1) << 11))
/* Port 1 Pin #Direction Control (10) */
#define PORT_P1_DIR_P10   (0x1 << 10)
#define PORT_P1_DIR_P10_VAL(val)   (((val) & 0x1) << 10)
#define PORT_P1_DIR_P10_GET(val)   ((((val) & PORT_P1_DIR_P10) >> 10) & 0x1)
#define PORT_P1_DIR_P10_SET(reg,val) (reg) = ((reg & ~PORT_P1_DIR_P10) | (((val) & 0x1) << 10))
/* Port 1 Pin #Direction Control (9) */
#define PORT_P1_DIR_P9   (0x1 << 9)
#define PORT_P1_DIR_P9_VAL(val)   (((val) & 0x1) << 9)
#define PORT_P1_DIR_P9_GET(val)   ((((val) & PORT_P1_DIR_P9) >> 9) & 0x1)
#define PORT_P1_DIR_P9_SET(reg,val) (reg) = ((reg & ~PORT_P1_DIR_P9) | (((val) & 0x1) << 9))
/* Port 1 Pin #Direction Control (8) */
#define PORT_P1_DIR_P8   (0x1 << 8)
#define PORT_P1_DIR_P8_VAL(val)   (((val) & 0x1) << 8)
#define PORT_P1_DIR_P8_GET(val)   ((((val) & PORT_P1_DIR_P8) >> 8) & 0x1)
#define PORT_P1_DIR_P8_SET(reg,val) (reg) = ((reg & ~PORT_P1_DIR_P8) | (((val) & 0x1) << 8))
/* Port 1 Pin #Direction Control (7) */
#define PORT_P1_DIR_P7   (0x1 << 7)
#define PORT_P1_DIR_P7_VAL(val)   (((val) & 0x1) << 7)
#define PORT_P1_DIR_P7_GET(val)   ((((val) & PORT_P1_DIR_P7) >> 7) & 0x1)
#define PORT_P1_DIR_P7_SET(reg,val) (reg) = ((reg & ~PORT_P1_DIR_P7) | (((val) & 0x1) << 7))
/* Port 1 Pin #Direction Control (6) */
#define PORT_P1_DIR_P6   (0x1 << 6)
#define PORT_P1_DIR_P6_VAL(val)   (((val) & 0x1) << 6)
#define PORT_P1_DIR_P6_GET(val)   ((((val) & PORT_P1_DIR_P6) >> 6) & 0x1)
#define PORT_P1_DIR_P6_SET(reg,val) (reg) = ((reg & ~PORT_P1_DIR_P6) | (((val) & 0x1) << 6))
/* Port 1 Pin #Direction Control (5) */
#define PORT_P1_DIR_P5   (0x1 << 5)
#define PORT_P1_DIR_P5_VAL(val)   (((val) & 0x1) << 5)
#define PORT_P1_DIR_P5_GET(val)   ((((val) & PORT_P1_DIR_P5) >> 5) & 0x1)
#define PORT_P1_DIR_P5_SET(reg,val) (reg) = ((reg & ~PORT_P1_DIR_P5) | (((val) & 0x1) << 5))
/* Port 1 Pin #Direction Control (4) */
#define PORT_P1_DIR_P4   (0x1 << 4)
#define PORT_P1_DIR_P4_VAL(val)   (((val) & 0x1) << 4)
#define PORT_P1_DIR_P4_GET(val)   ((((val) & PORT_P1_DIR_P4) >> 4) & 0x1)
#define PORT_P1_DIR_P4_SET(reg,val) (reg) = ((reg & ~PORT_P1_DIR_P4) | (((val) & 0x1) << 4))
/* Port 1 Pin #Direction Control (3) */
#define PORT_P1_DIR_P3   (0x1 << 3)
#define PORT_P1_DIR_P3_VAL(val)   (((val) & 0x1) << 3)
#define PORT_P1_DIR_P3_GET(val)   ((((val) & PORT_P1_DIR_P3) >> 3) & 0x1)
#define PORT_P1_DIR_P3_SET(reg,val) (reg) = ((reg & ~PORT_P1_DIR_P3) | (((val) & 0x1) << 3))
/* Port 1 Pin #Direction Control (2) */
#define PORT_P1_DIR_P2   (0x1 << 2)
#define PORT_P1_DIR_P2_VAL(val)   (((val) & 0x1) << 2)
#define PORT_P1_DIR_P2_GET(val)   ((((val) & PORT_P1_DIR_P2) >> 2) & 0x1)
#define PORT_P1_DIR_P2_SET(reg,val) (reg) = ((reg & ~PORT_P1_DIR_P2) | (((val) & 0x1) << 2))
/* Port 1 Pin #Direction Control (1) */
#define PORT_P1_DIR_P1   (0x1 << 1)
#define PORT_P1_DIR_P1_VAL(val)   (((val) & 0x1) << 1)
#define PORT_P1_DIR_P1_GET(val)   ((((val) & PORT_P1_DIR_P1) >> 1) & 0x1)
#define PORT_P1_DIR_P1_SET(reg,val) (reg) = ((reg & ~PORT_P1_DIR_P1) | (((val) & 0x1) << 1))
/* Port 1 Pin #Direction Control (0) */
#define PORT_P1_DIR_P0   (0x1)
#define PORT_P1_DIR_P0_VAL(val)   (((val) & 0x1) << 0)
#define PORT_P1_DIR_P0_GET(val)   ((((val) & PORT_P1_DIR_P0) >> 0) & 0x1)
#define PORT_P1_DIR_P0_SET(reg,val) (reg) = ((reg & ~PORT_P1_DIR_P0) | (((val) & 0x1) << 0))

/*******************************************************************************
 * Port 1 Alternate Function Select Register 0
 ******************************************************************************/

/* Alternate Function at Port 1 Bit # (19) */
#define PORT_P1_ALTSEL0_P19   (0x1 << 19)
#define PORT_P1_ALTSEL0_P19_VAL(val)   (((val) & 0x1) << 19)
#define PORT_P1_ALTSEL0_P19_GET(val)   ((((val) & PORT_P1_ALTSEL0_P19) >> 19) & 0x1)
#define PORT_P1_ALTSEL0_P19_SET(reg,val) (reg) = ((reg & ~PORT_P1_ALTSEL0_P19) | (((val) & 0x1) << 19))
/* Alternate Function at Port 1 Bit # (18) */
#define PORT_P1_ALTSEL0_P18   (0x1 << 18)
#define PORT_P1_ALTSEL0_P18_VAL(val)   (((val) & 0x1) << 18)
#define PORT_P1_ALTSEL0_P18_GET(val)   ((((val) & PORT_P1_ALTSEL0_P18) >> 18) & 0x1)
#define PORT_P1_ALTSEL0_P18_SET(reg,val) (reg) = ((reg & ~PORT_P1_ALTSEL0_P18) | (((val) & 0x1) << 18))
/* Alternate Function at Port 1 Bit # (17) */
#define PORT_P1_ALTSEL0_P17   (0x1 << 17)
#define PORT_P1_ALTSEL0_P17_VAL(val)   (((val) & 0x1) << 17)
#define PORT_P1_ALTSEL0_P17_GET(val)   ((((val) & PORT_P1_ALTSEL0_P17) >> 17) & 0x1)
#define PORT_P1_ALTSEL0_P17_SET(reg,val) (reg) = ((reg & ~PORT_P1_ALTSEL0_P17) | (((val) & 0x1) << 17))
/* Alternate Function at Port 1 Bit # (16) */
#define PORT_P1_ALTSEL0_P16   (0x1 << 16)
#define PORT_P1_ALTSEL0_P16_VAL(val)   (((val) & 0x1) << 16)
#define PORT_P1_ALTSEL0_P16_GET(val)   ((((val) & PORT_P1_ALTSEL0_P16) >> 16) & 0x1)
#define PORT_P1_ALTSEL0_P16_SET(reg,val) (reg) = ((reg & ~PORT_P1_ALTSEL0_P16) | (((val) & 0x1) << 16))
/* Alternate Function at Port 1 Bit # (15) */
#define PORT_P1_ALTSEL0_P15   (0x1 << 15)
#define PORT_P1_ALTSEL0_P15_VAL(val)   (((val) & 0x1) << 15)
#define PORT_P1_ALTSEL0_P15_GET(val)   ((((val) & PORT_P1_ALTSEL0_P15) >> 15) & 0x1)
#define PORT_P1_ALTSEL0_P15_SET(reg,val) (reg) = ((reg & ~PORT_P1_ALTSEL0_P15) | (((val) & 0x1) << 15))
/* Alternate Function at Port 1 Bit # (14) */
#define PORT_P1_ALTSEL0_P14   (0x1 << 14)
#define PORT_P1_ALTSEL0_P14_VAL(val)   (((val) & 0x1) << 14)
#define PORT_P1_ALTSEL0_P14_GET(val)   ((((val) & PORT_P1_ALTSEL0_P14) >> 14) & 0x1)
#define PORT_P1_ALTSEL0_P14_SET(reg,val) (reg) = ((reg & ~PORT_P1_ALTSEL0_P14) | (((val) & 0x1) << 14))
/* Alternate Function at Port 1 Bit # (13) */
#define PORT_P1_ALTSEL0_P13   (0x1 << 13)
#define PORT_P1_ALTSEL0_P13_VAL(val)   (((val) & 0x1) << 13)
#define PORT_P1_ALTSEL0_P13_GET(val)   ((((val) & PORT_P1_ALTSEL0_P13) >> 13) & 0x1)
#define PORT_P1_ALTSEL0_P13_SET(reg,val) (reg) = ((reg & ~PORT_P1_ALTSEL0_P13) | (((val) & 0x1) << 13))
/* Alternate Function at Port 1 Bit # (12) */
#define PORT_P1_ALTSEL0_P12   (0x1 << 12)
#define PORT_P1_ALTSEL0_P12_VAL(val)   (((val) & 0x1) << 12)
#define PORT_P1_ALTSEL0_P12_GET(val)   ((((val) & PORT_P1_ALTSEL0_P12) >> 12) & 0x1)
#define PORT_P1_ALTSEL0_P12_SET(reg,val) (reg) = ((reg & ~PORT_P1_ALTSEL0_P12) | (((val) & 0x1) << 12))
/* Alternate Function at Port 1 Bit # (11) */
#define PORT_P1_ALTSEL0_P11   (0x1 << 11)
#define PORT_P1_ALTSEL0_P11_VAL(val)   (((val) & 0x1) << 11)
#define PORT_P1_ALTSEL0_P11_GET(val)   ((((val) & PORT_P1_ALTSEL0_P11) >> 11) & 0x1)
#define PORT_P1_ALTSEL0_P11_SET(reg,val) (reg) = ((reg & ~PORT_P1_ALTSEL0_P11) | (((val) & 0x1) << 11))
/* Alternate Function at Port 1 Bit # (10) */
#define PORT_P1_ALTSEL0_P10   (0x1 << 10)
#define PORT_P1_ALTSEL0_P10_VAL(val)   (((val) & 0x1) << 10)
#define PORT_P1_ALTSEL0_P10_GET(val)   ((((val) & PORT_P1_ALTSEL0_P10) >> 10) & 0x1)
#define PORT_P1_ALTSEL0_P10_SET(reg,val) (reg) = ((reg & ~PORT_P1_ALTSEL0_P10) | (((val) & 0x1) << 10))
/* Alternate Function at Port 1 Bit # (9) */
#define PORT_P1_ALTSEL0_P9   (0x1 << 9)
#define PORT_P1_ALTSEL0_P9_VAL(val)   (((val) & 0x1) << 9)
#define PORT_P1_ALTSEL0_P9_GET(val)   ((((val) & PORT_P1_ALTSEL0_P9) >> 9) & 0x1)
#define PORT_P1_ALTSEL0_P9_SET(reg,val) (reg) = ((reg & ~PORT_P1_ALTSEL0_P9) | (((val) & 0x1) << 9))
/* Alternate Function at Port 1 Bit # (8) */
#define PORT_P1_ALTSEL0_P8   (0x1 << 8)
#define PORT_P1_ALTSEL0_P8_VAL(val)   (((val) & 0x1) << 8)
#define PORT_P1_ALTSEL0_P8_GET(val)   ((((val) & PORT_P1_ALTSEL0_P8) >> 8) & 0x1)
#define PORT_P1_ALTSEL0_P8_SET(reg,val) (reg) = ((reg & ~PORT_P1_ALTSEL0_P8) | (((val) & 0x1) << 8))
/* Alternate Function at Port 1 Bit # (7) */
#define PORT_P1_ALTSEL0_P7   (0x1 << 7)
#define PORT_P1_ALTSEL0_P7_VAL(val)   (((val) & 0x1) << 7)
#define PORT_P1_ALTSEL0_P7_GET(val)   ((((val) & PORT_P1_ALTSEL0_P7) >> 7) & 0x1)
#define PORT_P1_ALTSEL0_P7_SET(reg,val) (reg) = ((reg & ~PORT_P1_ALTSEL0_P7) | (((val) & 0x1) << 7))
/* Alternate Function at Port 1 Bit # (6) */
#define PORT_P1_ALTSEL0_P6   (0x1 << 6)
#define PORT_P1_ALTSEL0_P6_VAL(val)   (((val) & 0x1) << 6)
#define PORT_P1_ALTSEL0_P6_GET(val)   ((((val) & PORT_P1_ALTSEL0_P6) >> 6) & 0x1)
#define PORT_P1_ALTSEL0_P6_SET(reg,val) (reg) = ((reg & ~PORT_P1_ALTSEL0_P6) | (((val) & 0x1) << 6))
/* Alternate Function at Port 1 Bit # (5) */
#define PORT_P1_ALTSEL0_P5   (0x1 << 5)
#define PORT_P1_ALTSEL0_P5_VAL(val)   (((val) & 0x1) << 5)
#define PORT_P1_ALTSEL0_P5_GET(val)   ((((val) & PORT_P1_ALTSEL0_P5) >> 5) & 0x1)
#define PORT_P1_ALTSEL0_P5_SET(reg,val) (reg) = ((reg & ~PORT_P1_ALTSEL0_P5) | (((val) & 0x1) << 5))
/* Alternate Function at Port 1 Bit # (4) */
#define PORT_P1_ALTSEL0_P4   (0x1 << 4)
#define PORT_P1_ALTSEL0_P4_VAL(val)   (((val) & 0x1) << 4)
#define PORT_P1_ALTSEL0_P4_GET(val)   ((((val) & PORT_P1_ALTSEL0_P4) >> 4) & 0x1)
#define PORT_P1_ALTSEL0_P4_SET(reg,val) (reg) = ((reg & ~PORT_P1_ALTSEL0_P4) | (((val) & 0x1) << 4))
/* Alternate Function at Port 1 Bit # (3) */
#define PORT_P1_ALTSEL0_P3   (0x1 << 3)
#define PORT_P1_ALTSEL0_P3_VAL(val)   (((val) & 0x1) << 3)
#define PORT_P1_ALTSEL0_P3_GET(val)   ((((val) & PORT_P1_ALTSEL0_P3) >> 3) & 0x1)
#define PORT_P1_ALTSEL0_P3_SET(reg,val) (reg) = ((reg & ~PORT_P1_ALTSEL0_P3) | (((val) & 0x1) << 3))
/* Alternate Function at Port 1 Bit # (2) */
#define PORT_P1_ALTSEL0_P2   (0x1 << 2)
#define PORT_P1_ALTSEL0_P2_VAL(val)   (((val) & 0x1) << 2)
#define PORT_P1_ALTSEL0_P2_GET(val)   ((((val) & PORT_P1_ALTSEL0_P2) >> 2) & 0x1)
#define PORT_P1_ALTSEL0_P2_SET(reg,val) (reg) = ((reg & ~PORT_P1_ALTSEL0_P2) | (((val) & 0x1) << 2))
/* Alternate Function at Port 1 Bit # (1) */
#define PORT_P1_ALTSEL0_P1   (0x1 << 1)
#define PORT_P1_ALTSEL0_P1_VAL(val)   (((val) & 0x1) << 1)
#define PORT_P1_ALTSEL0_P1_GET(val)   ((((val) & PORT_P1_ALTSEL0_P1) >> 1) & 0x1)
#define PORT_P1_ALTSEL0_P1_SET(reg,val) (reg) = ((reg & ~PORT_P1_ALTSEL0_P1) | (((val) & 0x1) << 1))
/* Alternate Function at Port 1 Bit # (0) */
#define PORT_P1_ALTSEL0_P0   (0x1)
#define PORT_P1_ALTSEL0_P0_VAL(val)   (((val) & 0x1) << 0)
#define PORT_P1_ALTSEL0_P0_GET(val)   ((((val) & PORT_P1_ALTSEL0_P0) >> 0) & 0x1)
#define PORT_P1_ALTSEL0_P0_SET(reg,val) (reg) = ((reg & ~PORT_P1_ALTSEL0_P0) | (((val) & 0x1) << 0))

/*******************************************************************************
 * Port 1 Pull Up Device Enable Register
 ******************************************************************************/

/* Pull Up Device Enable at Port 1 Bit # (19) */
#define PORT_P1_PUEN_P19   (0x1 << 19)
#define PORT_P1_PUEN_P19_VAL(val)   (((val) & 0x1) << 19)
#define PORT_P1_PUEN_P19_GET(val)   ((((val) & PORT_P1_PUEN_P19) >> 19) & 0x1)
#define PORT_P1_PUEN_P19_SET(reg,val) (reg) = ((reg & ~PORT_P1_PUEN_P19) | (((val) & 0x1) << 19))
/* Pull Up Device Enable at Port 1 Bit # (18) */
#define PORT_P1_PUEN_P18   (0x1 << 18)
#define PORT_P1_PUEN_P18_VAL(val)   (((val) & 0x1) << 18)
#define PORT_P1_PUEN_P18_GET(val)   ((((val) & PORT_P1_PUEN_P18) >> 18) & 0x1)
#define PORT_P1_PUEN_P18_SET(reg,val) (reg) = ((reg & ~PORT_P1_PUEN_P18) | (((val) & 0x1) << 18))
/* Pull Up Device Enable at Port 1 Bit # (17) */
#define PORT_P1_PUEN_P17   (0x1 << 17)
#define PORT_P1_PUEN_P17_VAL(val)   (((val) & 0x1) << 17)
#define PORT_P1_PUEN_P17_GET(val)   ((((val) & PORT_P1_PUEN_P17) >> 17) & 0x1)
#define PORT_P1_PUEN_P17_SET(reg,val) (reg) = ((reg & ~PORT_P1_PUEN_P17) | (((val) & 0x1) << 17))
/* Pull Up Device Enable at Port 1 Bit # (16) */
#define PORT_P1_PUEN_P16   (0x1 << 16)
#define PORT_P1_PUEN_P16_VAL(val)   (((val) & 0x1) << 16)
#define PORT_P1_PUEN_P16_GET(val)   ((((val) & PORT_P1_PUEN_P16) >> 16) & 0x1)
#define PORT_P1_PUEN_P16_SET(reg,val) (reg) = ((reg & ~PORT_P1_PUEN_P16) | (((val) & 0x1) << 16))
/* Pull Up Device Enable at Port 1 Bit # (15) */
#define PORT_P1_PUEN_P15   (0x1 << 15)
#define PORT_P1_PUEN_P15_VAL(val)   (((val) & 0x1) << 15)
#define PORT_P1_PUEN_P15_GET(val)   ((((val) & PORT_P1_PUEN_P15) >> 15) & 0x1)
#define PORT_P1_PUEN_P15_SET(reg,val) (reg) = ((reg & ~PORT_P1_PUEN_P15) | (((val) & 0x1) << 15))
/* Pull Up Device Enable at Port 1 Bit # (14) */
#define PORT_P1_PUEN_P14   (0x1 << 14)
#define PORT_P1_PUEN_P14_VAL(val)   (((val) & 0x1) << 14)
#define PORT_P1_PUEN_P14_GET(val)   ((((val) & PORT_P1_PUEN_P14) >> 14) & 0x1)
#define PORT_P1_PUEN_P14_SET(reg,val) (reg) = ((reg & ~PORT_P1_PUEN_P14) | (((val) & 0x1) << 14))
/* Pull Up Device Enable at Port 1 Bit # (13) */
#define PORT_P1_PUEN_P13   (0x1 << 13)
#define PORT_P1_PUEN_P13_VAL(val)   (((val) & 0x1) << 13)
#define PORT_P1_PUEN_P13_GET(val)   ((((val) & PORT_P1_PUEN_P13) >> 13) & 0x1)
#define PORT_P1_PUEN_P13_SET(reg,val) (reg) = ((reg & ~PORT_P1_PUEN_P13) | (((val) & 0x1) << 13))
/* Pull Up Device Enable at Port 1 Bit # (12) */
#define PORT_P1_PUEN_P12   (0x1 << 12)
#define PORT_P1_PUEN_P12_VAL(val)   (((val) & 0x1) << 12)
#define PORT_P1_PUEN_P12_GET(val)   ((((val) & PORT_P1_PUEN_P12) >> 12) & 0x1)
#define PORT_P1_PUEN_P12_SET(reg,val) (reg) = ((reg & ~PORT_P1_PUEN_P12) | (((val) & 0x1) << 12))
/* Pull Up Device Enable at Port 1 Bit # (11) */
#define PORT_P1_PUEN_P11   (0x1 << 11)
#define PORT_P1_PUEN_P11_VAL(val)   (((val) & 0x1) << 11)
#define PORT_P1_PUEN_P11_GET(val)   ((((val) & PORT_P1_PUEN_P11) >> 11) & 0x1)
#define PORT_P1_PUEN_P11_SET(reg,val) (reg) = ((reg & ~PORT_P1_PUEN_P11) | (((val) & 0x1) << 11))
/* Pull Up Device Enable at Port 1 Bit # (10) */
#define PORT_P1_PUEN_P10   (0x1 << 10)
#define PORT_P1_PUEN_P10_VAL(val)   (((val) & 0x1) << 10)
#define PORT_P1_PUEN_P10_GET(val)   ((((val) & PORT_P1_PUEN_P10) >> 10) & 0x1)
#define PORT_P1_PUEN_P10_SET(reg,val) (reg) = ((reg & ~PORT_P1_PUEN_P10) | (((val) & 0x1) << 10))
/* Pull Up Device Enable at Port 1 Bit # (9) */
#define PORT_P1_PUEN_P9   (0x1 << 9)
#define PORT_P1_PUEN_P9_VAL(val)   (((val) & 0x1) << 9)
#define PORT_P1_PUEN_P9_GET(val)   ((((val) & PORT_P1_PUEN_P9) >> 9) & 0x1)
#define PORT_P1_PUEN_P9_SET(reg,val) (reg) = ((reg & ~PORT_P1_PUEN_P9) | (((val) & 0x1) << 9))
/* Pull Up Device Enable at Port 1 Bit # (8) */
#define PORT_P1_PUEN_P8   (0x1 << 8)
#define PORT_P1_PUEN_P8_VAL(val)   (((val) & 0x1) << 8)
#define PORT_P1_PUEN_P8_GET(val)   ((((val) & PORT_P1_PUEN_P8) >> 8) & 0x1)
#define PORT_P1_PUEN_P8_SET(reg,val) (reg) = ((reg & ~PORT_P1_PUEN_P8) | (((val) & 0x1) << 8))
/* Pull Up Device Enable at Port 1 Bit # (7) */
#define PORT_P1_PUEN_P7   (0x1 << 7)
#define PORT_P1_PUEN_P7_VAL(val)   (((val) & 0x1) << 7)
#define PORT_P1_PUEN_P7_GET(val)   ((((val) & PORT_P1_PUEN_P7) >> 7) & 0x1)
#define PORT_P1_PUEN_P7_SET(reg,val) (reg) = ((reg & ~PORT_P1_PUEN_P7) | (((val) & 0x1) << 7))
/* Pull Up Device Enable at Port 1 Bit # (6) */
#define PORT_P1_PUEN_P6   (0x1 << 6)
#define PORT_P1_PUEN_P6_VAL(val)   (((val) & 0x1) << 6)
#define PORT_P1_PUEN_P6_GET(val)   ((((val) & PORT_P1_PUEN_P6) >> 6) & 0x1)
#define PORT_P1_PUEN_P6_SET(reg,val) (reg) = ((reg & ~PORT_P1_PUEN_P6) | (((val) & 0x1) << 6))
/* Pull Up Device Enable at Port 1 Bit # (5) */
#define PORT_P1_PUEN_P5   (0x1 << 5)
#define PORT_P1_PUEN_P5_VAL(val)   (((val) & 0x1) << 5)
#define PORT_P1_PUEN_P5_GET(val)   ((((val) & PORT_P1_PUEN_P5) >> 5) & 0x1)
#define PORT_P1_PUEN_P5_SET(reg,val) (reg) = ((reg & ~PORT_P1_PUEN_P5) | (((val) & 0x1) << 5))
/* Pull Up Device Enable at Port 1 Bit # (4) */
#define PORT_P1_PUEN_P4   (0x1 << 4)
#define PORT_P1_PUEN_P4_VAL(val)   (((val) & 0x1) << 4)
#define PORT_P1_PUEN_P4_GET(val)   ((((val) & PORT_P1_PUEN_P4) >> 4) & 0x1)
#define PORT_P1_PUEN_P4_SET(reg,val) (reg) = ((reg & ~PORT_P1_PUEN_P4) | (((val) & 0x1) << 4))
/* Pull Up Device Enable at Port 1 Bit # (3) */
#define PORT_P1_PUEN_P3   (0x1 << 3)
#define PORT_P1_PUEN_P3_VAL(val)   (((val) & 0x1) << 3)
#define PORT_P1_PUEN_P3_GET(val)   ((((val) & PORT_P1_PUEN_P3) >> 3) & 0x1)
#define PORT_P1_PUEN_P3_SET(reg,val) (reg) = ((reg & ~PORT_P1_PUEN_P3) | (((val) & 0x1) << 3))
/* Pull Up Device Enable at Port 1 Bit # (2) */
#define PORT_P1_PUEN_P2   (0x1 << 2)
#define PORT_P1_PUEN_P2_VAL(val)   (((val) & 0x1) << 2)
#define PORT_P1_PUEN_P2_GET(val)   ((((val) & PORT_P1_PUEN_P2) >> 2) & 0x1)
#define PORT_P1_PUEN_P2_SET(reg,val) (reg) = ((reg & ~PORT_P1_PUEN_P2) | (((val) & 0x1) << 2))
/* Pull Up Device Enable at Port 1 Bit # (1) */
#define PORT_P1_PUEN_P1   (0x1 << 1)
#define PORT_P1_PUEN_P1_VAL(val)   (((val) & 0x1) << 1)
#define PORT_P1_PUEN_P1_GET(val)   ((((val) & PORT_P1_PUEN_P1) >> 1) & 0x1)
#define PORT_P1_PUEN_P1_SET(reg,val) (reg) = ((reg & ~PORT_P1_PUEN_P1) | (((val) & 0x1) << 1))
/* Pull Up Device Enable at Port 1 Bit # (0) */
#define PORT_P1_PUEN_P0   (0x1)
#define PORT_P1_PUEN_P0_VAL(val)   (((val) & 0x1) << 0)
#define PORT_P1_PUEN_P0_GET(val)   ((((val) & PORT_P1_PUEN_P0) >> 0) & 0x1)
#define PORT_P1_PUEN_P0_SET(reg,val) (reg) = ((reg & ~PORT_P1_PUEN_P0) | (((val) & 0x1) << 0))

/*******************************************************************************
 * External Interrupt Control Register 0
 ******************************************************************************/

/* Type of Level or Edge Detection of EXINT15 (19) */
#define PORT_P1_EXINTCR0_EXINT15   (0x1 << 19)
#define PORT_P1_EXINTCR0_EXINT15_VAL(val)   (((val) & 0x1) << 19)
#define PORT_P1_EXINTCR0_EXINT15_GET(val)   ((((val) & PORT_P1_EXINTCR0_EXINT15) >> 19) & 0x1)
#define PORT_P1_EXINTCR0_EXINT15_SET(reg,val) (reg) = ((reg & ~PORT_P1_EXINTCR0_EXINT15) | (((val) & 0x1) << 19))
/* Type of Level or Edge Detection of EXINT11 (18) */
#define PORT_P1_EXINTCR0_EXINT11   (0x1 << 18)
#define PORT_P1_EXINTCR0_EXINT11_VAL(val)   (((val) & 0x1) << 18)
#define PORT_P1_EXINTCR0_EXINT11_GET(val)   ((((val) & PORT_P1_EXINTCR0_EXINT11) >> 18) & 0x1)
#define PORT_P1_EXINTCR0_EXINT11_SET(reg,val) (reg) = ((reg & ~PORT_P1_EXINTCR0_EXINT11) | (((val) & 0x1) << 18))
/* Type of Level or Edge Detection of EXINT12 (17) */
#define PORT_P1_EXINTCR0_EXINT12   (0x1 << 17)
#define PORT_P1_EXINTCR0_EXINT12_VAL(val)   (((val) & 0x1) << 17)
#define PORT_P1_EXINTCR0_EXINT12_GET(val)   ((((val) & PORT_P1_EXINTCR0_EXINT12) >> 17) & 0x1)
#define PORT_P1_EXINTCR0_EXINT12_SET(reg,val) (reg) = ((reg & ~PORT_P1_EXINTCR0_EXINT12) | (((val) & 0x1) << 17))
/* Type of Level or Edge Detection of EXINT13 (16) */
#define PORT_P1_EXINTCR0_EXINT13   (0x1 << 16)
#define PORT_P1_EXINTCR0_EXINT13_VAL(val)   (((val) & 0x1) << 16)
#define PORT_P1_EXINTCR0_EXINT13_GET(val)   ((((val) & PORT_P1_EXINTCR0_EXINT13) >> 16) & 0x1)
#define PORT_P1_EXINTCR0_EXINT13_SET(reg,val) (reg) = ((reg & ~PORT_P1_EXINTCR0_EXINT13) | (((val) & 0x1) << 16))
/* Type of Level or Edge Detection of EXINT14 (15) */
#define PORT_P1_EXINTCR0_EXINT14   (0x1 << 15)
#define PORT_P1_EXINTCR0_EXINT14_VAL(val)   (((val) & 0x1) << 15)
#define PORT_P1_EXINTCR0_EXINT14_GET(val)   ((((val) & PORT_P1_EXINTCR0_EXINT14) >> 15) & 0x1)
#define PORT_P1_EXINTCR0_EXINT14_SET(reg,val) (reg) = ((reg & ~PORT_P1_EXINTCR0_EXINT14) | (((val) & 0x1) << 15))

/*******************************************************************************
 * External Interrupt Control Register 1
 ******************************************************************************/

/* Type of Level or Edge Detection of EXINT15 (19) */
#define PORT_P1_EXINTCR1_EXINT15   (0x1 << 19)
#define PORT_P1_EXINTCR1_EXINT15_VAL(val)   (((val) & 0x1) << 19)
#define PORT_P1_EXINTCR1_EXINT15_GET(val)   ((((val) & PORT_P1_EXINTCR1_EXINT15) >> 19) & 0x1)
#define PORT_P1_EXINTCR1_EXINT15_SET(reg,val) (reg) = ((reg & ~PORT_P1_EXINTCR1_EXINT15) | (((val) & 0x1) << 19))
/* Type of Level or Edge Detection of EXINT11 (18) */
#define PORT_P1_EXINTCR1_EXINT11   (0x1 << 18)
#define PORT_P1_EXINTCR1_EXINT11_VAL(val)   (((val) & 0x1) << 18)
#define PORT_P1_EXINTCR1_EXINT11_GET(val)   ((((val) & PORT_P1_EXINTCR1_EXINT11) >> 18) & 0x1)
#define PORT_P1_EXINTCR1_EXINT11_SET(reg,val) (reg) = ((reg & ~PORT_P1_EXINTCR1_EXINT11) | (((val) & 0x1) << 18))
/* Type of Level or Edge Detection of EXINT12 (17) */
#define PORT_P1_EXINTCR1_EXINT12   (0x1 << 17)
#define PORT_P1_EXINTCR1_EXINT12_VAL(val)   (((val) & 0x1) << 17)
#define PORT_P1_EXINTCR1_EXINT12_GET(val)   ((((val) & PORT_P1_EXINTCR1_EXINT12) >> 17) & 0x1)
#define PORT_P1_EXINTCR1_EXINT12_SET(reg,val) (reg) = ((reg & ~PORT_P1_EXINTCR1_EXINT12) | (((val) & 0x1) << 17))
/* Type of Level or Edge Detection of EXINT13 (16) */
#define PORT_P1_EXINTCR1_EXINT13   (0x1 << 16)
#define PORT_P1_EXINTCR1_EXINT13_VAL(val)   (((val) & 0x1) << 16)
#define PORT_P1_EXINTCR1_EXINT13_GET(val)   ((((val) & PORT_P1_EXINTCR1_EXINT13) >> 16) & 0x1)
#define PORT_P1_EXINTCR1_EXINT13_SET(reg,val) (reg) = ((reg & ~PORT_P1_EXINTCR1_EXINT13) | (((val) & 0x1) << 16))
/* Type of Level or Edge Detection of EXINT14 (15) */
#define PORT_P1_EXINTCR1_EXINT14   (0x1 << 15)
#define PORT_P1_EXINTCR1_EXINT14_VAL(val)   (((val) & 0x1) << 15)
#define PORT_P1_EXINTCR1_EXINT14_GET(val)   ((((val) & PORT_P1_EXINTCR1_EXINT14) >> 15) & 0x1)
#define PORT_P1_EXINTCR1_EXINT14_SET(reg,val) (reg) = ((reg & ~PORT_P1_EXINTCR1_EXINT14) | (((val) & 0x1) << 15))

/*******************************************************************************
 * P1_IRNEN Register
 ******************************************************************************/

/* EXINT15 Interrupt Request Enable (19) */
#define PORT_P1_IRNEN_EXINT15   (0x1 << 19)
#define PORT_P1_IRNEN_EXINT15_VAL(val)   (((val) & 0x1) << 19)
#define PORT_P1_IRNEN_EXINT15_GET(val)   ((((val) & PORT_P1_IRNEN_EXINT15) >> 19) & 0x1)
#define PORT_P1_IRNEN_EXINT15_SET(reg,val) (reg) = ((reg & ~PORT_P1_IRNEN_EXINT15) | (((val) & 0x1) << 19))
/* EXINT11 Interrupt Request Enable (18) */
#define PORT_P1_IRNEN_EXINT11   (0x1 << 18)
#define PORT_P1_IRNEN_EXINT11_VAL(val)   (((val) & 0x1) << 18)
#define PORT_P1_IRNEN_EXINT11_GET(val)   ((((val) & PORT_P1_IRNEN_EXINT11) >> 18) & 0x1)
#define PORT_P1_IRNEN_EXINT11_SET(reg,val) (reg) = ((reg & ~PORT_P1_IRNEN_EXINT11) | (((val) & 0x1) << 18))
/* EXINT12 Interrupt Request Enable (17) */
#define PORT_P1_IRNEN_EXINT12   (0x1 << 17)
#define PORT_P1_IRNEN_EXINT12_VAL(val)   (((val) & 0x1) << 17)
#define PORT_P1_IRNEN_EXINT12_GET(val)   ((((val) & PORT_P1_IRNEN_EXINT12) >> 17) & 0x1)
#define PORT_P1_IRNEN_EXINT12_SET(reg,val) (reg) = ((reg & ~PORT_P1_IRNEN_EXINT12) | (((val) & 0x1) << 17))
/* EXINT13 Interrupt Request Enable (16) */
#define PORT_P1_IRNEN_EXINT13   (0x1 << 16)
#define PORT_P1_IRNEN_EXINT13_VAL(val)   (((val) & 0x1) << 16)
#define PORT_P1_IRNEN_EXINT13_GET(val)   ((((val) & PORT_P1_IRNEN_EXINT13) >> 16) & 0x1)
#define PORT_P1_IRNEN_EXINT13_SET(reg,val) (reg) = ((reg & ~PORT_P1_IRNEN_EXINT13) | (((val) & 0x1) << 16))
/* EXINT14 Interrupt Request Enable (15) */
#define PORT_P1_IRNEN_EXINT14   (0x1 << 15)
#define PORT_P1_IRNEN_EXINT14_VAL(val)   (((val) & 0x1) << 15)
#define PORT_P1_IRNEN_EXINT14_GET(val)   ((((val) & PORT_P1_IRNEN_EXINT14) >> 15) & 0x1)
#define PORT_P1_IRNEN_EXINT14_SET(reg,val) (reg) = ((reg & ~PORT_P1_IRNEN_EXINT14) | (((val) & 0x1) << 15))

/*******************************************************************************
 * P1_IRNICR Register
 ******************************************************************************/

/* EXINT15 Interrupt Request (19) */
#define PORT_P1_IRNICR_EXINT15   (0x1 << 19)
#define PORT_P1_IRNICR_EXINT15_GET(val)   ((((val) & PORT_P1_IRNICR_EXINT15) >> 19) & 0x1)
/* EXINT11 Interrupt Request (18) */
#define PORT_P1_IRNICR_EXINT11   (0x1 << 18)
#define PORT_P1_IRNICR_EXINT11_GET(val)   ((((val) & PORT_P1_IRNICR_EXINT11) >> 18) & 0x1)
/* EXINT12 Interrupt Request (17) */
#define PORT_P1_IRNICR_EXINT12   (0x1 << 17)
#define PORT_P1_IRNICR_EXINT12_GET(val)   ((((val) & PORT_P1_IRNICR_EXINT12) >> 17) & 0x1)
/* EXINT13 Interrupt Request (16) */
#define PORT_P1_IRNICR_EXINT13   (0x1 << 16)
#define PORT_P1_IRNICR_EXINT13_GET(val)   ((((val) & PORT_P1_IRNICR_EXINT13) >> 16) & 0x1)
/* EXINT14 Interrupt Request (15) */
#define PORT_P1_IRNICR_EXINT14   (0x1 << 15)
#define PORT_P1_IRNICR_EXINT14_GET(val)   ((((val) & PORT_P1_IRNICR_EXINT14) >> 15) & 0x1)

/*******************************************************************************
 * P1_IRNCR Register
 ******************************************************************************/

/* EXINT15 Interrupt Request (19) */
#define PORT_P1_IRNCR_EXINT15   (0x1 << 19)
#define PORT_P1_IRNCR_EXINT15_GET(val)   ((((val) & PORT_P1_IRNCR_EXINT15) >> 19) & 0x1)
/* EXINT11 Interrupt Request (18) */
#define PORT_P1_IRNCR_EXINT11   (0x1 << 18)
#define PORT_P1_IRNCR_EXINT11_GET(val)   ((((val) & PORT_P1_IRNCR_EXINT11) >> 18) & 0x1)
/* EXINT12 Interrupt Request (17) */
#define PORT_P1_IRNCR_EXINT12   (0x1 << 17)
#define PORT_P1_IRNCR_EXINT12_GET(val)   ((((val) & PORT_P1_IRNCR_EXINT12) >> 17) & 0x1)
/* EXINT13 Interrupt Request (16) */
#define PORT_P1_IRNCR_EXINT13   (0x1 << 16)
#define PORT_P1_IRNCR_EXINT13_GET(val)   ((((val) & PORT_P1_IRNCR_EXINT13) >> 16) & 0x1)
/* EXINT14 Interrupt Request (15) */
#define PORT_P1_IRNCR_EXINT14   (0x1 << 15)
#define PORT_P1_IRNCR_EXINT14_GET(val)   ((((val) & PORT_P1_IRNCR_EXINT14) >> 15) & 0x1)

/*******************************************************************************
 * P1 External Event Detection Configuration Register
 ******************************************************************************/

/* EXINT15 configured for Edge or Level Detection (19) */
#define PORT_P1_IRNCFG_EXINT15   (0x1 << 19)
#define PORT_P1_IRNCFG_EXINT15_VAL(val)   (((val) & 0x1) << 19)
#define PORT_P1_IRNCFG_EXINT15_GET(val)   ((((val) & PORT_P1_IRNCFG_EXINT15) >> 19) & 0x1)
#define PORT_P1_IRNCFG_EXINT15_SET(reg,val) (reg) = ((reg & ~PORT_P1_IRNCFG_EXINT15) | (((val) & 0x1) << 19))
/* EXINT11 configured for Edge or Level Detection (18) */
#define PORT_P1_IRNCFG_EXINT11   (0x1 << 18)
#define PORT_P1_IRNCFG_EXINT11_VAL(val)   (((val) & 0x1) << 18)
#define PORT_P1_IRNCFG_EXINT11_GET(val)   ((((val) & PORT_P1_IRNCFG_EXINT11) >> 18) & 0x1)
#define PORT_P1_IRNCFG_EXINT11_SET(reg,val) (reg) = ((reg & ~PORT_P1_IRNCFG_EXINT11) | (((val) & 0x1) << 18))
/* EXINT12 configured for Edge or Level Detection (17) */
#define PORT_P1_IRNCFG_EXINT12   (0x1 << 17)
#define PORT_P1_IRNCFG_EXINT12_VAL(val)   (((val) & 0x1) << 17)
#define PORT_P1_IRNCFG_EXINT12_GET(val)   ((((val) & PORT_P1_IRNCFG_EXINT12) >> 17) & 0x1)
#define PORT_P1_IRNCFG_EXINT12_SET(reg,val) (reg) = ((reg & ~PORT_P1_IRNCFG_EXINT12) | (((val) & 0x1) << 17))
/* EXINT13 configured for Edge or Level Detection (16) */
#define PORT_P1_IRNCFG_EXINT13   (0x1 << 16)
#define PORT_P1_IRNCFG_EXINT13_VAL(val)   (((val) & 0x1) << 16)
#define PORT_P1_IRNCFG_EXINT13_GET(val)   ((((val) & PORT_P1_IRNCFG_EXINT13) >> 16) & 0x1)
#define PORT_P1_IRNCFG_EXINT13_SET(reg,val) (reg) = ((reg & ~PORT_P1_IRNCFG_EXINT13) | (((val) & 0x1) << 16))
/* EXINT14 configured for Edge or Level Detection (15) */
#define PORT_P1_IRNCFG_EXINT14   (0x1 << 15)
#define PORT_P1_IRNCFG_EXINT14_VAL(val)   (((val) & 0x1) << 15)
#define PORT_P1_IRNCFG_EXINT14_GET(val)   ((((val) & PORT_P1_IRNCFG_EXINT14) >> 15) & 0x1)
#define PORT_P1_IRNCFG_EXINT14_SET(reg,val) (reg) = ((reg & ~PORT_P1_IRNCFG_EXINT14) | (((val) & 0x1) << 15))

/*******************************************************************************
 * P1_IRNENSET Register
 ******************************************************************************/

/* Set Interrupt Node Enable Flag EXINT15 (19) */
#define PORT_P1_IRNENSET_EXINT15   (0x1 << 19)
#define PORT_P1_IRNENSET_EXINT15_VAL(val)   (((val) & 0x1) << 19)
#define PORT_P1_IRNENSET_EXINT15_SET(reg,val) (reg) = (((reg & ~PORT_P1_IRNENSET_EXINT15) | (val) & 1) << 19)
/* Set Interrupt Node Enable Flag EXINT11 (18) */
#define PORT_P1_IRNENSET_EXINT11   (0x1 << 18)
#define PORT_P1_IRNENSET_EXINT11_VAL(val)   (((val) & 0x1) << 18)
#define PORT_P1_IRNENSET_EXINT11_SET(reg,val) (reg) = (((reg & ~PORT_P1_IRNENSET_EXINT11) | (val) & 1) << 18)
/* Set Interrupt Node Enable Flag EXINT12 (17) */
#define PORT_P1_IRNENSET_EXINT12   (0x1 << 17)
#define PORT_P1_IRNENSET_EXINT12_VAL(val)   (((val) & 0x1) << 17)
#define PORT_P1_IRNENSET_EXINT12_SET(reg,val) (reg) = (((reg & ~PORT_P1_IRNENSET_EXINT12) | (val) & 1) << 17)
/* Set Interrupt Node Enable Flag EXINT13 (16) */
#define PORT_P1_IRNENSET_EXINT13   (0x1 << 16)
#define PORT_P1_IRNENSET_EXINT13_VAL(val)   (((val) & 0x1) << 16)
#define PORT_P1_IRNENSET_EXINT13_SET(reg,val) (reg) = (((reg & ~PORT_P1_IRNENSET_EXINT13) | (val) & 1) << 16)
/* Set Interrupt Node Enable Flag EXINT14 (15) */
#define PORT_P1_IRNENSET_EXINT14   (0x1 << 15)
#define PORT_P1_IRNENSET_EXINT14_VAL(val)   (((val) & 0x1) << 15)
#define PORT_P1_IRNENSET_EXINT14_SET(reg,val) (reg) = (((reg & ~PORT_P1_IRNENSET_EXINT14) | (val) & 1) << 15)

/*******************************************************************************
 * P1_IRNENCLR Register
 ******************************************************************************/

/* Clear Interrupt Node Enable Flag EXINT15 (19) */
#define PORT_P1_IRNENCLR_EXINT15   (0x1 << 19)
#define PORT_P1_IRNENCLR_EXINT15_VAL(val)   (((val) & 0x1) << 19)
#define PORT_P1_IRNENCLR_EXINT15_SET(reg,val) (reg) = (((reg & ~PORT_P1_IRNENCLR_EXINT15) | (val) & 1) << 19)
/* Clear Interrupt Node Enable Flag EXINT11 (18) */
#define PORT_P1_IRNENCLR_EXINT11   (0x1 << 18)
#define PORT_P1_IRNENCLR_EXINT11_VAL(val)   (((val) & 0x1) << 18)
#define PORT_P1_IRNENCLR_EXINT11_SET(reg,val) (reg) = (((reg & ~PORT_P1_IRNENCLR_EXINT11) | (val) & 1) << 18)
/* Clear Interrupt Node Enable Flag EXINT12 (17) */
#define PORT_P1_IRNENCLR_EXINT12   (0x1 << 17)
#define PORT_P1_IRNENCLR_EXINT12_VAL(val)   (((val) & 0x1) << 17)
#define PORT_P1_IRNENCLR_EXINT12_SET(reg,val) (reg) = (((reg & ~PORT_P1_IRNENCLR_EXINT12) | (val) & 1) << 17)
/* Clear Interrupt Node Enable Flag EXINT13 (16) */
#define PORT_P1_IRNENCLR_EXINT13   (0x1 << 16)
#define PORT_P1_IRNENCLR_EXINT13_VAL(val)   (((val) & 0x1) << 16)
#define PORT_P1_IRNENCLR_EXINT13_SET(reg,val) (reg) = (((reg & ~PORT_P1_IRNENCLR_EXINT13) | (val) & 1) << 16)
/* Clear Interrupt Node Enable Flag EXINT14 (15) */
#define PORT_P1_IRNENCLR_EXINT14   (0x1 << 15)
#define PORT_P1_IRNENCLR_EXINT14_VAL(val)   (((val) & 0x1) << 15)
#define PORT_P1_IRNENCLR_EXINT14_SET(reg,val) (reg) = (((reg & ~PORT_P1_IRNENCLR_EXINT14) | (val) & 1) << 15)

/*******************************************************************************
 * Port 2 Data Output Register
 ******************************************************************************/

/* Port 2 Pin # Output Value (19) */
#define PORT_P2_OUT_P19   (0x1 << 19)
#define PORT_P2_OUT_P19_VAL(val)   (((val) & 0x1) << 19)
#define PORT_P2_OUT_P19_GET(val)   ((((val) & PORT_P2_OUT_P19) >> 19) & 0x1)
#define PORT_P2_OUT_P19_SET(reg,val) (reg) = ((reg & ~PORT_P2_OUT_P19) | (((val) & 0x1) << 19))
/* Port 2 Pin # Output Value (18) */
#define PORT_P2_OUT_P18   (0x1 << 18)
#define PORT_P2_OUT_P18_VAL(val)   (((val) & 0x1) << 18)
#define PORT_P2_OUT_P18_GET(val)   ((((val) & PORT_P2_OUT_P18) >> 18) & 0x1)
#define PORT_P2_OUT_P18_SET(reg,val) (reg) = ((reg & ~PORT_P2_OUT_P18) | (((val) & 0x1) << 18))
/* Port 2 Pin # Output Value (17) */
#define PORT_P2_OUT_P17   (0x1 << 17)
#define PORT_P2_OUT_P17_VAL(val)   (((val) & 0x1) << 17)
#define PORT_P2_OUT_P17_GET(val)   ((((val) & PORT_P2_OUT_P17) >> 17) & 0x1)
#define PORT_P2_OUT_P17_SET(reg,val) (reg) = ((reg & ~PORT_P2_OUT_P17) | (((val) & 0x1) << 17))
/* Port 2 Pin # Output Value (16) */
#define PORT_P2_OUT_P16   (0x1 << 16)
#define PORT_P2_OUT_P16_VAL(val)   (((val) & 0x1) << 16)
#define PORT_P2_OUT_P16_GET(val)   ((((val) & PORT_P2_OUT_P16) >> 16) & 0x1)
#define PORT_P2_OUT_P16_SET(reg,val) (reg) = ((reg & ~PORT_P2_OUT_P16) | (((val) & 0x1) << 16))
/* Port 2 Pin # Output Value (15) */
#define PORT_P2_OUT_P15   (0x1 << 15)
#define PORT_P2_OUT_P15_VAL(val)   (((val) & 0x1) << 15)
#define PORT_P2_OUT_P15_GET(val)   ((((val) & PORT_P2_OUT_P15) >> 15) & 0x1)
#define PORT_P2_OUT_P15_SET(reg,val) (reg) = ((reg & ~PORT_P2_OUT_P15) | (((val) & 0x1) << 15))
/* Port 2 Pin # Output Value (14) */
#define PORT_P2_OUT_P14   (0x1 << 14)
#define PORT_P2_OUT_P14_VAL(val)   (((val) & 0x1) << 14)
#define PORT_P2_OUT_P14_GET(val)   ((((val) & PORT_P2_OUT_P14) >> 14) & 0x1)
#define PORT_P2_OUT_P14_SET(reg,val) (reg) = ((reg & ~PORT_P2_OUT_P14) | (((val) & 0x1) << 14))
/* Port 2 Pin # Output Value (13) */
#define PORT_P2_OUT_P13   (0x1 << 13)
#define PORT_P2_OUT_P13_VAL(val)   (((val) & 0x1) << 13)
#define PORT_P2_OUT_P13_GET(val)   ((((val) & PORT_P2_OUT_P13) >> 13) & 0x1)
#define PORT_P2_OUT_P13_SET(reg,val) (reg) = ((reg & ~PORT_P2_OUT_P13) | (((val) & 0x1) << 13))
/* Port 2 Pin # Output Value (12) */
#define PORT_P2_OUT_P12   (0x1 << 12)
#define PORT_P2_OUT_P12_VAL(val)   (((val) & 0x1) << 12)
#define PORT_P2_OUT_P12_GET(val)   ((((val) & PORT_P2_OUT_P12) >> 12) & 0x1)
#define PORT_P2_OUT_P12_SET(reg,val) (reg) = ((reg & ~PORT_P2_OUT_P12) | (((val) & 0x1) << 12))
/* Port 2 Pin # Output Value (11) */
#define PORT_P2_OUT_P11   (0x1 << 11)
#define PORT_P2_OUT_P11_VAL(val)   (((val) & 0x1) << 11)
#define PORT_P2_OUT_P11_GET(val)   ((((val) & PORT_P2_OUT_P11) >> 11) & 0x1)
#define PORT_P2_OUT_P11_SET(reg,val) (reg) = ((reg & ~PORT_P2_OUT_P11) | (((val) & 0x1) << 11))
/* Port 2 Pin # Output Value (10) */
#define PORT_P2_OUT_P10   (0x1 << 10)
#define PORT_P2_OUT_P10_VAL(val)   (((val) & 0x1) << 10)
#define PORT_P2_OUT_P10_GET(val)   ((((val) & PORT_P2_OUT_P10) >> 10) & 0x1)
#define PORT_P2_OUT_P10_SET(reg,val) (reg) = ((reg & ~PORT_P2_OUT_P10) | (((val) & 0x1) << 10))
/* Port 2 Pin # Output Value (9) */
#define PORT_P2_OUT_P9   (0x1 << 9)
#define PORT_P2_OUT_P9_VAL(val)   (((val) & 0x1) << 9)
#define PORT_P2_OUT_P9_GET(val)   ((((val) & PORT_P2_OUT_P9) >> 9) & 0x1)
#define PORT_P2_OUT_P9_SET(reg,val) (reg) = ((reg & ~PORT_P2_OUT_P9) | (((val) & 0x1) << 9))
/* Port 2 Pin # Output Value (8) */
#define PORT_P2_OUT_P8   (0x1 << 8)
#define PORT_P2_OUT_P8_VAL(val)   (((val) & 0x1) << 8)
#define PORT_P2_OUT_P8_GET(val)   ((((val) & PORT_P2_OUT_P8) >> 8) & 0x1)
#define PORT_P2_OUT_P8_SET(reg,val) (reg) = ((reg & ~PORT_P2_OUT_P8) | (((val) & 0x1) << 8))
/* Port 2 Pin # Output Value (7) */
#define PORT_P2_OUT_P7   (0x1 << 7)
#define PORT_P2_OUT_P7_VAL(val)   (((val) & 0x1) << 7)
#define PORT_P2_OUT_P7_GET(val)   ((((val) & PORT_P2_OUT_P7) >> 7) & 0x1)
#define PORT_P2_OUT_P7_SET(reg,val) (reg) = ((reg & ~PORT_P2_OUT_P7) | (((val) & 0x1) << 7))
/* Port 2 Pin # Output Value (6) */
#define PORT_P2_OUT_P6   (0x1 << 6)
#define PORT_P2_OUT_P6_VAL(val)   (((val) & 0x1) << 6)
#define PORT_P2_OUT_P6_GET(val)   ((((val) & PORT_P2_OUT_P6) >> 6) & 0x1)
#define PORT_P2_OUT_P6_SET(reg,val) (reg) = ((reg & ~PORT_P2_OUT_P6) | (((val) & 0x1) << 6))
/* Port 2 Pin # Output Value (5) */
#define PORT_P2_OUT_P5   (0x1 << 5)
#define PORT_P2_OUT_P5_VAL(val)   (((val) & 0x1) << 5)
#define PORT_P2_OUT_P5_GET(val)   ((((val) & PORT_P2_OUT_P5) >> 5) & 0x1)
#define PORT_P2_OUT_P5_SET(reg,val) (reg) = ((reg & ~PORT_P2_OUT_P5) | (((val) & 0x1) << 5))
/* Port 2 Pin # Output Value (4) */
#define PORT_P2_OUT_P4   (0x1 << 4)
#define PORT_P2_OUT_P4_VAL(val)   (((val) & 0x1) << 4)
#define PORT_P2_OUT_P4_GET(val)   ((((val) & PORT_P2_OUT_P4) >> 4) & 0x1)
#define PORT_P2_OUT_P4_SET(reg,val) (reg) = ((reg & ~PORT_P2_OUT_P4) | (((val) & 0x1) << 4))
/* Port 2 Pin # Output Value (3) */
#define PORT_P2_OUT_P3   (0x1 << 3)
#define PORT_P2_OUT_P3_VAL(val)   (((val) & 0x1) << 3)
#define PORT_P2_OUT_P3_GET(val)   ((((val) & PORT_P2_OUT_P3) >> 3) & 0x1)
#define PORT_P2_OUT_P3_SET(reg,val) (reg) = ((reg & ~PORT_P2_OUT_P3) | (((val) & 0x1) << 3))
/* Port 2 Pin # Output Value (2) */
#define PORT_P2_OUT_P2   (0x1 << 2)
#define PORT_P2_OUT_P2_VAL(val)   (((val) & 0x1) << 2)
#define PORT_P2_OUT_P2_GET(val)   ((((val) & PORT_P2_OUT_P2) >> 2) & 0x1)
#define PORT_P2_OUT_P2_SET(reg,val) (reg) = ((reg & ~PORT_P2_OUT_P2) | (((val) & 0x1) << 2))
/* Port 2 Pin # Output Value (1) */
#define PORT_P2_OUT_P1   (0x1 << 1)
#define PORT_P2_OUT_P1_VAL(val)   (((val) & 0x1) << 1)
#define PORT_P2_OUT_P1_GET(val)   ((((val) & PORT_P2_OUT_P1) >> 1) & 0x1)
#define PORT_P2_OUT_P1_SET(reg,val) (reg) = ((reg & ~PORT_P2_OUT_P1) | (((val) & 0x1) << 1))
/* Port 2 Pin # Output Value (0) */
#define PORT_P2_OUT_P0   (0x1)
#define PORT_P2_OUT_P0_VAL(val)   (((val) & 0x1) << 0)
#define PORT_P2_OUT_P0_GET(val)   ((((val) & PORT_P2_OUT_P0) >> 0) & 0x1)
#define PORT_P2_OUT_P0_SET(reg,val) (reg) = ((reg & ~PORT_P2_OUT_P0) | (((val) & 0x1) << 0))

/*******************************************************************************
 * Port 2 Data Input Register
 ******************************************************************************/

/* Port 2 Pin # Latched Input Value (19) */
#define PORT_P2_IN_P19   (0x1 << 19)
#define PORT_P2_IN_P19_GET(val)   ((((val) & PORT_P2_IN_P19) >> 19) & 0x1)
/* Port 2 Pin # Latched Input Value (18) */
#define PORT_P2_IN_P18   (0x1 << 18)
#define PORT_P2_IN_P18_GET(val)   ((((val) & PORT_P2_IN_P18) >> 18) & 0x1)
/* Port 2 Pin # Latched Input Value (17) */
#define PORT_P2_IN_P17   (0x1 << 17)
#define PORT_P2_IN_P17_GET(val)   ((((val) & PORT_P2_IN_P17) >> 17) & 0x1)
/* Port 2 Pin # Latched Input Value (16) */
#define PORT_P2_IN_P16   (0x1 << 16)
#define PORT_P2_IN_P16_GET(val)   ((((val) & PORT_P2_IN_P16) >> 16) & 0x1)
/* Port 2 Pin # Latched Input Value (15) */
#define PORT_P2_IN_P15   (0x1 << 15)
#define PORT_P2_IN_P15_GET(val)   ((((val) & PORT_P2_IN_P15) >> 15) & 0x1)
/* Port 2 Pin # Latched Input Value (14) */
#define PORT_P2_IN_P14   (0x1 << 14)
#define PORT_P2_IN_P14_GET(val)   ((((val) & PORT_P2_IN_P14) >> 14) & 0x1)
/* Port 2 Pin # Latched Input Value (13) */
#define PORT_P2_IN_P13   (0x1 << 13)
#define PORT_P2_IN_P13_GET(val)   ((((val) & PORT_P2_IN_P13) >> 13) & 0x1)
/* Port 2 Pin # Latched Input Value (12) */
#define PORT_P2_IN_P12   (0x1 << 12)
#define PORT_P2_IN_P12_GET(val)   ((((val) & PORT_P2_IN_P12) >> 12) & 0x1)
/* Port 2 Pin # Latched Input Value (11) */
#define PORT_P2_IN_P11   (0x1 << 11)
#define PORT_P2_IN_P11_GET(val)   ((((val) & PORT_P2_IN_P11) >> 11) & 0x1)
/* Port 2 Pin # Latched Input Value (10) */
#define PORT_P2_IN_P10   (0x1 << 10)
#define PORT_P2_IN_P10_GET(val)   ((((val) & PORT_P2_IN_P10) >> 10) & 0x1)
/* Port 2 Pin # Latched Input Value (9) */
#define PORT_P2_IN_P9   (0x1 << 9)
#define PORT_P2_IN_P9_GET(val)   ((((val) & PORT_P2_IN_P9) >> 9) & 0x1)
/* Port 2 Pin # Latched Input Value (8) */
#define PORT_P2_IN_P8   (0x1 << 8)
#define PORT_P2_IN_P8_GET(val)   ((((val) & PORT_P2_IN_P8) >> 8) & 0x1)
/* Port 2 Pin # Latched Input Value (7) */
#define PORT_P2_IN_P7   (0x1 << 7)
#define PORT_P2_IN_P7_GET(val)   ((((val) & PORT_P2_IN_P7) >> 7) & 0x1)
/* Port 2 Pin # Latched Input Value (6) */
#define PORT_P2_IN_P6   (0x1 << 6)
#define PORT_P2_IN_P6_GET(val)   ((((val) & PORT_P2_IN_P6) >> 6) & 0x1)
/* Port 2 Pin # Latched Input Value (5) */
#define PORT_P2_IN_P5   (0x1 << 5)
#define PORT_P2_IN_P5_GET(val)   ((((val) & PORT_P2_IN_P5) >> 5) & 0x1)
/* Port 2 Pin # Latched Input Value (4) */
#define PORT_P2_IN_P4   (0x1 << 4)
#define PORT_P2_IN_P4_GET(val)   ((((val) & PORT_P2_IN_P4) >> 4) & 0x1)
/* Port 2 Pin # Latched Input Value (3) */
#define PORT_P2_IN_P3   (0x1 << 3)
#define PORT_P2_IN_P3_GET(val)   ((((val) & PORT_P2_IN_P3) >> 3) & 0x1)
/* Port 2 Pin # Latched Input Value (2) */
#define PORT_P2_IN_P2   (0x1 << 2)
#define PORT_P2_IN_P2_GET(val)   ((((val) & PORT_P2_IN_P2) >> 2) & 0x1)
/* Port 2 Pin # Latched Input Value (1) */
#define PORT_P2_IN_P1   (0x1 << 1)
#define PORT_P2_IN_P1_GET(val)   ((((val) & PORT_P2_IN_P1) >> 1) & 0x1)
/* Port 2 Pin # Latched Input Value (0) */
#define PORT_P2_IN_P0   (0x1)
#define PORT_P2_IN_P0_GET(val)   ((((val) & PORT_P2_IN_P0) >> 0) & 0x1)

/*******************************************************************************
 * Port 2 Direction Register
 ******************************************************************************/

/* Port 2 Pin #Direction Control (19) */
#define PORT_P2_DIR_P19   (0x1 << 19)
#define PORT_P2_DIR_P19_VAL(val)   (((val) & 0x1) << 19)
#define PORT_P2_DIR_P19_GET(val)   ((((val) & PORT_P2_DIR_P19) >> 19) & 0x1)
#define PORT_P2_DIR_P19_SET(reg,val) (reg) = ((reg & ~PORT_P2_DIR_P19) | (((val) & 0x1) << 19))
/* Port 2 Pin #Direction Control (18) */
#define PORT_P2_DIR_P18   (0x1 << 18)
#define PORT_P2_DIR_P18_VAL(val)   (((val) & 0x1) << 18)
#define PORT_P2_DIR_P18_GET(val)   ((((val) & PORT_P2_DIR_P18) >> 18) & 0x1)
#define PORT_P2_DIR_P18_SET(reg,val) (reg) = ((reg & ~PORT_P2_DIR_P18) | (((val) & 0x1) << 18))
/* Port 2 Pin #Direction Control (17) */
#define PORT_P2_DIR_P17   (0x1 << 17)
#define PORT_P2_DIR_P17_VAL(val)   (((val) & 0x1) << 17)
#define PORT_P2_DIR_P17_GET(val)   ((((val) & PORT_P2_DIR_P17) >> 17) & 0x1)
#define PORT_P2_DIR_P17_SET(reg,val) (reg) = ((reg & ~PORT_P2_DIR_P17) | (((val) & 0x1) << 17))
/* Port 2 Pin #Direction Control (16) */
#define PORT_P2_DIR_P16   (0x1 << 16)
#define PORT_P2_DIR_P16_VAL(val)   (((val) & 0x1) << 16)
#define PORT_P2_DIR_P16_GET(val)   ((((val) & PORT_P2_DIR_P16) >> 16) & 0x1)
#define PORT_P2_DIR_P16_SET(reg,val) (reg) = ((reg & ~PORT_P2_DIR_P16) | (((val) & 0x1) << 16))
/* Port 2 Pin #Direction Control (15) */
#define PORT_P2_DIR_P15   (0x1 << 15)
#define PORT_P2_DIR_P15_VAL(val)   (((val) & 0x1) << 15)
#define PORT_P2_DIR_P15_GET(val)   ((((val) & PORT_P2_DIR_P15) >> 15) & 0x1)
#define PORT_P2_DIR_P15_SET(reg,val) (reg) = ((reg & ~PORT_P2_DIR_P15) | (((val) & 0x1) << 15))
/* Port 2 Pin #Direction Control (14) */
#define PORT_P2_DIR_P14   (0x1 << 14)
#define PORT_P2_DIR_P14_VAL(val)   (((val) & 0x1) << 14)
#define PORT_P2_DIR_P14_GET(val)   ((((val) & PORT_P2_DIR_P14) >> 14) & 0x1)
#define PORT_P2_DIR_P14_SET(reg,val) (reg) = ((reg & ~PORT_P2_DIR_P14) | (((val) & 0x1) << 14))
/* Port 2 Pin #Direction Control (13) */
#define PORT_P2_DIR_P13   (0x1 << 13)
#define PORT_P2_DIR_P13_VAL(val)   (((val) & 0x1) << 13)
#define PORT_P2_DIR_P13_GET(val)   ((((val) & PORT_P2_DIR_P13) >> 13) & 0x1)
#define PORT_P2_DIR_P13_SET(reg,val) (reg) = ((reg & ~PORT_P2_DIR_P13) | (((val) & 0x1) << 13))
/* Port 2 Pin #Direction Control (12) */
#define PORT_P2_DIR_P12   (0x1 << 12)
#define PORT_P2_DIR_P12_VAL(val)   (((val) & 0x1) << 12)
#define PORT_P2_DIR_P12_GET(val)   ((((val) & PORT_P2_DIR_P12) >> 12) & 0x1)
#define PORT_P2_DIR_P12_SET(reg,val) (reg) = ((reg & ~PORT_P2_DIR_P12) | (((val) & 0x1) << 12))
/* Port 2 Pin #Direction Control (11) */
#define PORT_P2_DIR_P11   (0x1 << 11)
#define PORT_P2_DIR_P11_VAL(val)   (((val) & 0x1) << 11)
#define PORT_P2_DIR_P11_GET(val)   ((((val) & PORT_P2_DIR_P11) >> 11) & 0x1)
#define PORT_P2_DIR_P11_SET(reg,val) (reg) = ((reg & ~PORT_P2_DIR_P11) | (((val) & 0x1) << 11))
/* Port 2 Pin #Direction Control (10) */
#define PORT_P2_DIR_P10   (0x1 << 10)
#define PORT_P2_DIR_P10_VAL(val)   (((val) & 0x1) << 10)
#define PORT_P2_DIR_P10_GET(val)   ((((val) & PORT_P2_DIR_P10) >> 10) & 0x1)
#define PORT_P2_DIR_P10_SET(reg,val) (reg) = ((reg & ~PORT_P2_DIR_P10) | (((val) & 0x1) << 10))
/* Port 2 Pin #Direction Control (9) */
#define PORT_P2_DIR_P9   (0x1 << 9)
#define PORT_P2_DIR_P9_VAL(val)   (((val) & 0x1) << 9)
#define PORT_P2_DIR_P9_GET(val)   ((((val) & PORT_P2_DIR_P9) >> 9) & 0x1)
#define PORT_P2_DIR_P9_SET(reg,val) (reg) = ((reg & ~PORT_P2_DIR_P9) | (((val) & 0x1) << 9))
/* Port 2 Pin #Direction Control (8) */
#define PORT_P2_DIR_P8   (0x1 << 8)
#define PORT_P2_DIR_P8_VAL(val)   (((val) & 0x1) << 8)
#define PORT_P2_DIR_P8_GET(val)   ((((val) & PORT_P2_DIR_P8) >> 8) & 0x1)
#define PORT_P2_DIR_P8_SET(reg,val) (reg) = ((reg & ~PORT_P2_DIR_P8) | (((val) & 0x1) << 8))
/* Port 2 Pin #Direction Control (7) */
#define PORT_P2_DIR_P7   (0x1 << 7)
#define PORT_P2_DIR_P7_VAL(val)   (((val) & 0x1) << 7)
#define PORT_P2_DIR_P7_GET(val)   ((((val) & PORT_P2_DIR_P7) >> 7) & 0x1)
#define PORT_P2_DIR_P7_SET(reg,val) (reg) = ((reg & ~PORT_P2_DIR_P7) | (((val) & 0x1) << 7))
/* Port 2 Pin #Direction Control (6) */
#define PORT_P2_DIR_P6   (0x1 << 6)
#define PORT_P2_DIR_P6_VAL(val)   (((val) & 0x1) << 6)
#define PORT_P2_DIR_P6_GET(val)   ((((val) & PORT_P2_DIR_P6) >> 6) & 0x1)
#define PORT_P2_DIR_P6_SET(reg,val) (reg) = ((reg & ~PORT_P2_DIR_P6) | (((val) & 0x1) << 6))
/* Port 2 Pin #Direction Control (5) */
#define PORT_P2_DIR_P5   (0x1 << 5)
#define PORT_P2_DIR_P5_VAL(val)   (((val) & 0x1) << 5)
#define PORT_P2_DIR_P5_GET(val)   ((((val) & PORT_P2_DIR_P5) >> 5) & 0x1)
#define PORT_P2_DIR_P5_SET(reg,val) (reg) = ((reg & ~PORT_P2_DIR_P5) | (((val) & 0x1) << 5))
/* Port 2 Pin #Direction Control (4) */
#define PORT_P2_DIR_P4   (0x1 << 4)
#define PORT_P2_DIR_P4_VAL(val)   (((val) & 0x1) << 4)
#define PORT_P2_DIR_P4_GET(val)   ((((val) & PORT_P2_DIR_P4) >> 4) & 0x1)
#define PORT_P2_DIR_P4_SET(reg,val) (reg) = ((reg & ~PORT_P2_DIR_P4) | (((val) & 0x1) << 4))
/* Port 2 Pin #Direction Control (3) */
#define PORT_P2_DIR_P3   (0x1 << 3)
#define PORT_P2_DIR_P3_VAL(val)   (((val) & 0x1) << 3)
#define PORT_P2_DIR_P3_GET(val)   ((((val) & PORT_P2_DIR_P3) >> 3) & 0x1)
#define PORT_P2_DIR_P3_SET(reg,val) (reg) = ((reg & ~PORT_P2_DIR_P3) | (((val) & 0x1) << 3))
/* Port 2 Pin #Direction Control (2) */
#define PORT_P2_DIR_P2   (0x1 << 2)
#define PORT_P2_DIR_P2_VAL(val)   (((val) & 0x1) << 2)
#define PORT_P2_DIR_P2_GET(val)   ((((val) & PORT_P2_DIR_P2) >> 2) & 0x1)
#define PORT_P2_DIR_P2_SET(reg,val) (reg) = ((reg & ~PORT_P2_DIR_P2) | (((val) & 0x1) << 2))
/* Port 2 Pin #Direction Control (1) */
#define PORT_P2_DIR_P1   (0x1 << 1)
#define PORT_P2_DIR_P1_VAL(val)   (((val) & 0x1) << 1)
#define PORT_P2_DIR_P1_GET(val)   ((((val) & PORT_P2_DIR_P1) >> 1) & 0x1)
#define PORT_P2_DIR_P1_SET(reg,val) (reg) = ((reg & ~PORT_P2_DIR_P1) | (((val) & 0x1) << 1))
/* Port 2 Pin #Direction Control (0) */
#define PORT_P2_DIR_P0   (0x1)
#define PORT_P2_DIR_P0_VAL(val)   (((val) & 0x1) << 0)
#define PORT_P2_DIR_P0_GET(val)   ((((val) & PORT_P2_DIR_P0) >> 0) & 0x1)
#define PORT_P2_DIR_P0_SET(reg,val) (reg) = ((reg & ~PORT_P2_DIR_P0) | (((val) & 0x1) << 0))

/*******************************************************************************
 * Port 2 Alternate Function Select Register 0
 ******************************************************************************/

/* Alternate Function at Port 2 Bit # (19) */
#define PORT_P2_ALTSEL0_P19   (0x1 << 19)
#define PORT_P2_ALTSEL0_P19_VAL(val)   (((val) & 0x1) << 19)
#define PORT_P2_ALTSEL0_P19_GET(val)   ((((val) & PORT_P2_ALTSEL0_P19) >> 19) & 0x1)
#define PORT_P2_ALTSEL0_P19_SET(reg,val) (reg) = ((reg & ~PORT_P2_ALTSEL0_P19) | (((val) & 0x1) << 19))
/* Alternate Function at Port 2 Bit # (18) */
#define PORT_P2_ALTSEL0_P18   (0x1 << 18)
#define PORT_P2_ALTSEL0_P18_VAL(val)   (((val) & 0x1) << 18)
#define PORT_P2_ALTSEL0_P18_GET(val)   ((((val) & PORT_P2_ALTSEL0_P18) >> 18) & 0x1)
#define PORT_P2_ALTSEL0_P18_SET(reg,val) (reg) = ((reg & ~PORT_P2_ALTSEL0_P18) | (((val) & 0x1) << 18))
/* Alternate Function at Port 2 Bit # (17) */
#define PORT_P2_ALTSEL0_P17   (0x1 << 17)
#define PORT_P2_ALTSEL0_P17_VAL(val)   (((val) & 0x1) << 17)
#define PORT_P2_ALTSEL0_P17_GET(val)   ((((val) & PORT_P2_ALTSEL0_P17) >> 17) & 0x1)
#define PORT_P2_ALTSEL0_P17_SET(reg,val) (reg) = ((reg & ~PORT_P2_ALTSEL0_P17) | (((val) & 0x1) << 17))
/* Alternate Function at Port 2 Bit # (16) */
#define PORT_P2_ALTSEL0_P16   (0x1 << 16)
#define PORT_P2_ALTSEL0_P16_VAL(val)   (((val) & 0x1) << 16)
#define PORT_P2_ALTSEL0_P16_GET(val)   ((((val) & PORT_P2_ALTSEL0_P16) >> 16) & 0x1)
#define PORT_P2_ALTSEL0_P16_SET(reg,val) (reg) = ((reg & ~PORT_P2_ALTSEL0_P16) | (((val) & 0x1) << 16))
/* Alternate Function at Port 2 Bit # (15) */
#define PORT_P2_ALTSEL0_P15   (0x1 << 15)
#define PORT_P2_ALTSEL0_P15_VAL(val)   (((val) & 0x1) << 15)
#define PORT_P2_ALTSEL0_P15_GET(val)   ((((val) & PORT_P2_ALTSEL0_P15) >> 15) & 0x1)
#define PORT_P2_ALTSEL0_P15_SET(reg,val) (reg) = ((reg & ~PORT_P2_ALTSEL0_P15) | (((val) & 0x1) << 15))
/* Alternate Function at Port 2 Bit # (14) */
#define PORT_P2_ALTSEL0_P14   (0x1 << 14)
#define PORT_P2_ALTSEL0_P14_VAL(val)   (((val) & 0x1) << 14)
#define PORT_P2_ALTSEL0_P14_GET(val)   ((((val) & PORT_P2_ALTSEL0_P14) >> 14) & 0x1)
#define PORT_P2_ALTSEL0_P14_SET(reg,val) (reg) = ((reg & ~PORT_P2_ALTSEL0_P14) | (((val) & 0x1) << 14))
/* Alternate Function at Port 2 Bit # (13) */
#define PORT_P2_ALTSEL0_P13   (0x1 << 13)
#define PORT_P2_ALTSEL0_P13_VAL(val)   (((val) & 0x1) << 13)
#define PORT_P2_ALTSEL0_P13_GET(val)   ((((val) & PORT_P2_ALTSEL0_P13) >> 13) & 0x1)
#define PORT_P2_ALTSEL0_P13_SET(reg,val) (reg) = ((reg & ~PORT_P2_ALTSEL0_P13) | (((val) & 0x1) << 13))
/* Alternate Function at Port 2 Bit # (12) */
#define PORT_P2_ALTSEL0_P12   (0x1 << 12)
#define PORT_P2_ALTSEL0_P12_VAL(val)   (((val) & 0x1) << 12)
#define PORT_P2_ALTSEL0_P12_GET(val)   ((((val) & PORT_P2_ALTSEL0_P12) >> 12) & 0x1)
#define PORT_P2_ALTSEL0_P12_SET(reg,val) (reg) = ((reg & ~PORT_P2_ALTSEL0_P12) | (((val) & 0x1) << 12))
/* Alternate Function at Port 2 Bit # (11) */
#define PORT_P2_ALTSEL0_P11   (0x1 << 11)
#define PORT_P2_ALTSEL0_P11_VAL(val)   (((val) & 0x1) << 11)
#define PORT_P2_ALTSEL0_P11_GET(val)   ((((val) & PORT_P2_ALTSEL0_P11) >> 11) & 0x1)
#define PORT_P2_ALTSEL0_P11_SET(reg,val) (reg) = ((reg & ~PORT_P2_ALTSEL0_P11) | (((val) & 0x1) << 11))
/* Alternate Function at Port 2 Bit # (10) */
#define PORT_P2_ALTSEL0_P10   (0x1 << 10)
#define PORT_P2_ALTSEL0_P10_VAL(val)   (((val) & 0x1) << 10)
#define PORT_P2_ALTSEL0_P10_GET(val)   ((((val) & PORT_P2_ALTSEL0_P10) >> 10) & 0x1)
#define PORT_P2_ALTSEL0_P10_SET(reg,val) (reg) = ((reg & ~PORT_P2_ALTSEL0_P10) | (((val) & 0x1) << 10))
/* Alternate Function at Port 2 Bit # (9) */
#define PORT_P2_ALTSEL0_P9   (0x1 << 9)
#define PORT_P2_ALTSEL0_P9_VAL(val)   (((val) & 0x1) << 9)
#define PORT_P2_ALTSEL0_P9_GET(val)   ((((val) & PORT_P2_ALTSEL0_P9) >> 9) & 0x1)
#define PORT_P2_ALTSEL0_P9_SET(reg,val) (reg) = ((reg & ~PORT_P2_ALTSEL0_P9) | (((val) & 0x1) << 9))
/* Alternate Function at Port 2 Bit # (8) */
#define PORT_P2_ALTSEL0_P8   (0x1 << 8)
#define PORT_P2_ALTSEL0_P8_VAL(val)   (((val) & 0x1) << 8)
#define PORT_P2_ALTSEL0_P8_GET(val)   ((((val) & PORT_P2_ALTSEL0_P8) >> 8) & 0x1)
#define PORT_P2_ALTSEL0_P8_SET(reg,val) (reg) = ((reg & ~PORT_P2_ALTSEL0_P8) | (((val) & 0x1) << 8))
/* Alternate Function at Port 2 Bit # (7) */
#define PORT_P2_ALTSEL0_P7   (0x1 << 7)
#define PORT_P2_ALTSEL0_P7_VAL(val)   (((val) & 0x1) << 7)
#define PORT_P2_ALTSEL0_P7_GET(val)   ((((val) & PORT_P2_ALTSEL0_P7) >> 7) & 0x1)
#define PORT_P2_ALTSEL0_P7_SET(reg,val) (reg) = ((reg & ~PORT_P2_ALTSEL0_P7) | (((val) & 0x1) << 7))
/* Alternate Function at Port 2 Bit # (6) */
#define PORT_P2_ALTSEL0_P6   (0x1 << 6)
#define PORT_P2_ALTSEL0_P6_VAL(val)   (((val) & 0x1) << 6)
#define PORT_P2_ALTSEL0_P6_GET(val)   ((((val) & PORT_P2_ALTSEL0_P6) >> 6) & 0x1)
#define PORT_P2_ALTSEL0_P6_SET(reg,val) (reg) = ((reg & ~PORT_P2_ALTSEL0_P6) | (((val) & 0x1) << 6))
/* Alternate Function at Port 2 Bit # (5) */
#define PORT_P2_ALTSEL0_P5   (0x1 << 5)
#define PORT_P2_ALTSEL0_P5_VAL(val)   (((val) & 0x1) << 5)
#define PORT_P2_ALTSEL0_P5_GET(val)   ((((val) & PORT_P2_ALTSEL0_P5) >> 5) & 0x1)
#define PORT_P2_ALTSEL0_P5_SET(reg,val) (reg) = ((reg & ~PORT_P2_ALTSEL0_P5) | (((val) & 0x1) << 5))
/* Alternate Function at Port 2 Bit # (4) */
#define PORT_P2_ALTSEL0_P4   (0x1 << 4)
#define PORT_P2_ALTSEL0_P4_VAL(val)   (((val) & 0x1) << 4)
#define PORT_P2_ALTSEL0_P4_GET(val)   ((((val) & PORT_P2_ALTSEL0_P4) >> 4) & 0x1)
#define PORT_P2_ALTSEL0_P4_SET(reg,val) (reg) = ((reg & ~PORT_P2_ALTSEL0_P4) | (((val) & 0x1) << 4))
/* Alternate Function at Port 2 Bit # (3) */
#define PORT_P2_ALTSEL0_P3   (0x1 << 3)
#define PORT_P2_ALTSEL0_P3_VAL(val)   (((val) & 0x1) << 3)
#define PORT_P2_ALTSEL0_P3_GET(val)   ((((val) & PORT_P2_ALTSEL0_P3) >> 3) & 0x1)
#define PORT_P2_ALTSEL0_P3_SET(reg,val) (reg) = ((reg & ~PORT_P2_ALTSEL0_P3) | (((val) & 0x1) << 3))
/* Alternate Function at Port 2 Bit # (2) */
#define PORT_P2_ALTSEL0_P2   (0x1 << 2)
#define PORT_P2_ALTSEL0_P2_VAL(val)   (((val) & 0x1) << 2)
#define PORT_P2_ALTSEL0_P2_GET(val)   ((((val) & PORT_P2_ALTSEL0_P2) >> 2) & 0x1)
#define PORT_P2_ALTSEL0_P2_SET(reg,val) (reg) = ((reg & ~PORT_P2_ALTSEL0_P2) | (((val) & 0x1) << 2))
/* Alternate Function at Port 2 Bit # (1) */
#define PORT_P2_ALTSEL0_P1   (0x1 << 1)
#define PORT_P2_ALTSEL0_P1_VAL(val)   (((val) & 0x1) << 1)
#define PORT_P2_ALTSEL0_P1_GET(val)   ((((val) & PORT_P2_ALTSEL0_P1) >> 1) & 0x1)
#define PORT_P2_ALTSEL0_P1_SET(reg,val) (reg) = ((reg & ~PORT_P2_ALTSEL0_P1) | (((val) & 0x1) << 1))
/* Alternate Function at Port 2 Bit # (0) */
#define PORT_P2_ALTSEL0_P0   (0x1)
#define PORT_P2_ALTSEL0_P0_VAL(val)   (((val) & 0x1) << 0)
#define PORT_P2_ALTSEL0_P0_GET(val)   ((((val) & PORT_P2_ALTSEL0_P0) >> 0) & 0x1)
#define PORT_P2_ALTSEL0_P0_SET(reg,val) (reg) = ((reg & ~PORT_P2_ALTSEL0_P0) | (((val) & 0x1) << 0))

/*******************************************************************************
 * Port 2 Pull Up Device Enable Register
 ******************************************************************************/

/* Pull Up Device Enable at Port 2 Bit # (19) */
#define PORT_P2_PUEN_P19   (0x1 << 19)
#define PORT_P2_PUEN_P19_VAL(val)   (((val) & 0x1) << 19)
#define PORT_P2_PUEN_P19_GET(val)   ((((val) & PORT_P2_PUEN_P19) >> 19) & 0x1)
#define PORT_P2_PUEN_P19_SET(reg,val) (reg) = ((reg & ~PORT_P2_PUEN_P19) | (((val) & 0x1) << 19))
/* Pull Up Device Enable at Port 2 Bit # (18) */
#define PORT_P2_PUEN_P18   (0x1 << 18)
#define PORT_P2_PUEN_P18_VAL(val)   (((val) & 0x1) << 18)
#define PORT_P2_PUEN_P18_GET(val)   ((((val) & PORT_P2_PUEN_P18) >> 18) & 0x1)
#define PORT_P2_PUEN_P18_SET(reg,val) (reg) = ((reg & ~PORT_P2_PUEN_P18) | (((val) & 0x1) << 18))
/* Pull Up Device Enable at Port 2 Bit # (17) */
#define PORT_P2_PUEN_P17   (0x1 << 17)
#define PORT_P2_PUEN_P17_VAL(val)   (((val) & 0x1) << 17)
#define PORT_P2_PUEN_P17_GET(val)   ((((val) & PORT_P2_PUEN_P17) >> 17) & 0x1)
#define PORT_P2_PUEN_P17_SET(reg,val) (reg) = ((reg & ~PORT_P2_PUEN_P17) | (((val) & 0x1) << 17))
/* Pull Up Device Enable at Port 2 Bit # (16) */
#define PORT_P2_PUEN_P16   (0x1 << 16)
#define PORT_P2_PUEN_P16_VAL(val)   (((val) & 0x1) << 16)
#define PORT_P2_PUEN_P16_GET(val)   ((((val) & PORT_P2_PUEN_P16) >> 16) & 0x1)
#define PORT_P2_PUEN_P16_SET(reg,val) (reg) = ((reg & ~PORT_P2_PUEN_P16) | (((val) & 0x1) << 16))
/* Pull Up Device Enable at Port 2 Bit # (15) */
#define PORT_P2_PUEN_P15   (0x1 << 15)
#define PORT_P2_PUEN_P15_VAL(val)   (((val) & 0x1) << 15)
#define PORT_P2_PUEN_P15_GET(val)   ((((val) & PORT_P2_PUEN_P15) >> 15) & 0x1)
#define PORT_P2_PUEN_P15_SET(reg,val) (reg) = ((reg & ~PORT_P2_PUEN_P15) | (((val) & 0x1) << 15))
/* Pull Up Device Enable at Port 2 Bit # (14) */
#define PORT_P2_PUEN_P14   (0x1 << 14)
#define PORT_P2_PUEN_P14_VAL(val)   (((val) & 0x1) << 14)
#define PORT_P2_PUEN_P14_GET(val)   ((((val) & PORT_P2_PUEN_P14) >> 14) & 0x1)
#define PORT_P2_PUEN_P14_SET(reg,val) (reg) = ((reg & ~PORT_P2_PUEN_P14) | (((val) & 0x1) << 14))
/* Pull Up Device Enable at Port 2 Bit # (13) */
#define PORT_P2_PUEN_P13   (0x1 << 13)
#define PORT_P2_PUEN_P13_VAL(val)   (((val) & 0x1) << 13)
#define PORT_P2_PUEN_P13_GET(val)   ((((val) & PORT_P2_PUEN_P13) >> 13) & 0x1)
#define PORT_P2_PUEN_P13_SET(reg,val) (reg) = ((reg & ~PORT_P2_PUEN_P13) | (((val) & 0x1) << 13))
/* Pull Up Device Enable at Port 2 Bit # (12) */
#define PORT_P2_PUEN_P12   (0x1 << 12)
#define PORT_P2_PUEN_P12_VAL(val)   (((val) & 0x1) << 12)
#define PORT_P2_PUEN_P12_GET(val)   ((((val) & PORT_P2_PUEN_P12) >> 12) & 0x1)
#define PORT_P2_PUEN_P12_SET(reg,val) (reg) = ((reg & ~PORT_P2_PUEN_P12) | (((val) & 0x1) << 12))
/* Pull Up Device Enable at Port 2 Bit # (11) */
#define PORT_P2_PUEN_P11   (0x1 << 11)
#define PORT_P2_PUEN_P11_VAL(val)   (((val) & 0x1) << 11)
#define PORT_P2_PUEN_P11_GET(val)   ((((val) & PORT_P2_PUEN_P11) >> 11) & 0x1)
#define PORT_P2_PUEN_P11_SET(reg,val) (reg) = ((reg & ~PORT_P2_PUEN_P11) | (((val) & 0x1) << 11))
/* Pull Up Device Enable at Port 2 Bit # (10) */
#define PORT_P2_PUEN_P10   (0x1 << 10)
#define PORT_P2_PUEN_P10_VAL(val)   (((val) & 0x1) << 10)
#define PORT_P2_PUEN_P10_GET(val)   ((((val) & PORT_P2_PUEN_P10) >> 10) & 0x1)
#define PORT_P2_PUEN_P10_SET(reg,val) (reg) = ((reg & ~PORT_P2_PUEN_P10) | (((val) & 0x1) << 10))
/* Pull Up Device Enable at Port 2 Bit # (9) */
#define PORT_P2_PUEN_P9   (0x1 << 9)
#define PORT_P2_PUEN_P9_VAL(val)   (((val) & 0x1) << 9)
#define PORT_P2_PUEN_P9_GET(val)   ((((val) & PORT_P2_PUEN_P9) >> 9) & 0x1)
#define PORT_P2_PUEN_P9_SET(reg,val) (reg) = ((reg & ~PORT_P2_PUEN_P9) | (((val) & 0x1) << 9))
/* Pull Up Device Enable at Port 2 Bit # (8) */
#define PORT_P2_PUEN_P8   (0x1 << 8)
#define PORT_P2_PUEN_P8_VAL(val)   (((val) & 0x1) << 8)
#define PORT_P2_PUEN_P8_GET(val)   ((((val) & PORT_P2_PUEN_P8) >> 8) & 0x1)
#define PORT_P2_PUEN_P8_SET(reg,val) (reg) = ((reg & ~PORT_P2_PUEN_P8) | (((val) & 0x1) << 8))
/* Pull Up Device Enable at Port 2 Bit # (7) */
#define PORT_P2_PUEN_P7   (0x1 << 7)
#define PORT_P2_PUEN_P7_VAL(val)   (((val) & 0x1) << 7)
#define PORT_P2_PUEN_P7_GET(val)   ((((val) & PORT_P2_PUEN_P7) >> 7) & 0x1)
#define PORT_P2_PUEN_P7_SET(reg,val) (reg) = ((reg & ~PORT_P2_PUEN_P7) | (((val) & 0x1) << 7))
/* Pull Up Device Enable at Port 2 Bit # (6) */
#define PORT_P2_PUEN_P6   (0x1 << 6)
#define PORT_P2_PUEN_P6_VAL(val)   (((val) & 0x1) << 6)
#define PORT_P2_PUEN_P6_GET(val)   ((((val) & PORT_P2_PUEN_P6) >> 6) & 0x1)
#define PORT_P2_PUEN_P6_SET(reg,val) (reg) = ((reg & ~PORT_P2_PUEN_P6) | (((val) & 0x1) << 6))
/* Pull Up Device Enable at Port 2 Bit # (5) */
#define PORT_P2_PUEN_P5   (0x1 << 5)
#define PORT_P2_PUEN_P5_VAL(val)   (((val) & 0x1) << 5)
#define PORT_P2_PUEN_P5_GET(val)   ((((val) & PORT_P2_PUEN_P5) >> 5) & 0x1)
#define PORT_P2_PUEN_P5_SET(reg,val) (reg) = ((reg & ~PORT_P2_PUEN_P5) | (((val) & 0x1) << 5))
/* Pull Up Device Enable at Port 2 Bit # (4) */
#define PORT_P2_PUEN_P4   (0x1 << 4)
#define PORT_P2_PUEN_P4_VAL(val)   (((val) & 0x1) << 4)
#define PORT_P2_PUEN_P4_GET(val)   ((((val) & PORT_P2_PUEN_P4) >> 4) & 0x1)
#define PORT_P2_PUEN_P4_SET(reg,val) (reg) = ((reg & ~PORT_P2_PUEN_P4) | (((val) & 0x1) << 4))
/* Pull Up Device Enable at Port 2 Bit # (3) */
#define PORT_P2_PUEN_P3   (0x1 << 3)
#define PORT_P2_PUEN_P3_VAL(val)   (((val) & 0x1) << 3)
#define PORT_P2_PUEN_P3_GET(val)   ((((val) & PORT_P2_PUEN_P3) >> 3) & 0x1)
#define PORT_P2_PUEN_P3_SET(reg,val) (reg) = ((reg & ~PORT_P2_PUEN_P3) | (((val) & 0x1) << 3))
/* Pull Up Device Enable at Port 2 Bit # (2) */
#define PORT_P2_PUEN_P2   (0x1 << 2)
#define PORT_P2_PUEN_P2_VAL(val)   (((val) & 0x1) << 2)
#define PORT_P2_PUEN_P2_GET(val)   ((((val) & PORT_P2_PUEN_P2) >> 2) & 0x1)
#define PORT_P2_PUEN_P2_SET(reg,val) (reg) = ((reg & ~PORT_P2_PUEN_P2) | (((val) & 0x1) << 2))
/* Pull Up Device Enable at Port 2 Bit # (1) */
#define PORT_P2_PUEN_P1   (0x1 << 1)
#define PORT_P2_PUEN_P1_VAL(val)   (((val) & 0x1) << 1)
#define PORT_P2_PUEN_P1_GET(val)   ((((val) & PORT_P2_PUEN_P1) >> 1) & 0x1)
#define PORT_P2_PUEN_P1_SET(reg,val) (reg) = ((reg & ~PORT_P2_PUEN_P1) | (((val) & 0x1) << 1))
/* Pull Up Device Enable at Port 2 Bit # (0) */
#define PORT_P2_PUEN_P0   (0x1)
#define PORT_P2_PUEN_P0_VAL(val)   (((val) & 0x1) << 0)
#define PORT_P2_PUEN_P0_GET(val)   ((((val) & PORT_P2_PUEN_P0) >> 0) & 0x1)
#define PORT_P2_PUEN_P0_SET(reg,val) (reg) = ((reg & ~PORT_P2_PUEN_P0) | (((val) & 0x1) << 0))

/*******************************************************************************
 * Port 3 Data Output Register
 ******************************************************************************/

/* Port 3 Pin # Output Value (19) */
#define PORT_P3_OUT_P19   (0x1 << 19)
#define PORT_P3_OUT_P19_VAL(val)   (((val) & 0x1) << 19)
#define PORT_P3_OUT_P19_GET(val)   ((((val) & PORT_P3_OUT_P19) >> 19) & 0x1)
#define PORT_P3_OUT_P19_SET(reg,val) (reg) = ((reg & ~PORT_P3_OUT_P19) | (((val) & 0x1) << 19))
/* Port 3 Pin # Output Value (18) */
#define PORT_P3_OUT_P18   (0x1 << 18)
#define PORT_P3_OUT_P18_VAL(val)   (((val) & 0x1) << 18)
#define PORT_P3_OUT_P18_GET(val)   ((((val) & PORT_P3_OUT_P18) >> 18) & 0x1)
#define PORT_P3_OUT_P18_SET(reg,val) (reg) = ((reg & ~PORT_P3_OUT_P18) | (((val) & 0x1) << 18))
/* Port 3 Pin # Output Value (17) */
#define PORT_P3_OUT_P17   (0x1 << 17)
#define PORT_P3_OUT_P17_VAL(val)   (((val) & 0x1) << 17)
#define PORT_P3_OUT_P17_GET(val)   ((((val) & PORT_P3_OUT_P17) >> 17) & 0x1)
#define PORT_P3_OUT_P17_SET(reg,val) (reg) = ((reg & ~PORT_P3_OUT_P17) | (((val) & 0x1) << 17))
/* Port 3 Pin # Output Value (16) */
#define PORT_P3_OUT_P16   (0x1 << 16)
#define PORT_P3_OUT_P16_VAL(val)   (((val) & 0x1) << 16)
#define PORT_P3_OUT_P16_GET(val)   ((((val) & PORT_P3_OUT_P16) >> 16) & 0x1)
#define PORT_P3_OUT_P16_SET(reg,val) (reg) = ((reg & ~PORT_P3_OUT_P16) | (((val) & 0x1) << 16))
/* Port 3 Pin # Output Value (15) */
#define PORT_P3_OUT_P15   (0x1 << 15)
#define PORT_P3_OUT_P15_VAL(val)   (((val) & 0x1) << 15)
#define PORT_P3_OUT_P15_GET(val)   ((((val) & PORT_P3_OUT_P15) >> 15) & 0x1)
#define PORT_P3_OUT_P15_SET(reg,val) (reg) = ((reg & ~PORT_P3_OUT_P15) | (((val) & 0x1) << 15))
/* Port 3 Pin # Output Value (14) */
#define PORT_P3_OUT_P14   (0x1 << 14)
#define PORT_P3_OUT_P14_VAL(val)   (((val) & 0x1) << 14)
#define PORT_P3_OUT_P14_GET(val)   ((((val) & PORT_P3_OUT_P14) >> 14) & 0x1)
#define PORT_P3_OUT_P14_SET(reg,val) (reg) = ((reg & ~PORT_P3_OUT_P14) | (((val) & 0x1) << 14))
/* Port 3 Pin # Output Value (13) */
#define PORT_P3_OUT_P13   (0x1 << 13)
#define PORT_P3_OUT_P13_VAL(val)   (((val) & 0x1) << 13)
#define PORT_P3_OUT_P13_GET(val)   ((((val) & PORT_P3_OUT_P13) >> 13) & 0x1)
#define PORT_P3_OUT_P13_SET(reg,val) (reg) = ((reg & ~PORT_P3_OUT_P13) | (((val) & 0x1) << 13))
/* Port 3 Pin # Output Value (12) */
#define PORT_P3_OUT_P12   (0x1 << 12)
#define PORT_P3_OUT_P12_VAL(val)   (((val) & 0x1) << 12)
#define PORT_P3_OUT_P12_GET(val)   ((((val) & PORT_P3_OUT_P12) >> 12) & 0x1)
#define PORT_P3_OUT_P12_SET(reg,val) (reg) = ((reg & ~PORT_P3_OUT_P12) | (((val) & 0x1) << 12))
/* Port 3 Pin # Output Value (11) */
#define PORT_P3_OUT_P11   (0x1 << 11)
#define PORT_P3_OUT_P11_VAL(val)   (((val) & 0x1) << 11)
#define PORT_P3_OUT_P11_GET(val)   ((((val) & PORT_P3_OUT_P11) >> 11) & 0x1)
#define PORT_P3_OUT_P11_SET(reg,val) (reg) = ((reg & ~PORT_P3_OUT_P11) | (((val) & 0x1) << 11))
/* Port 3 Pin # Output Value (10) */
#define PORT_P3_OUT_P10   (0x1 << 10)
#define PORT_P3_OUT_P10_VAL(val)   (((val) & 0x1) << 10)
#define PORT_P3_OUT_P10_GET(val)   ((((val) & PORT_P3_OUT_P10) >> 10) & 0x1)
#define PORT_P3_OUT_P10_SET(reg,val) (reg) = ((reg & ~PORT_P3_OUT_P10) | (((val) & 0x1) << 10))
/* Port 3 Pin # Output Value (9) */
#define PORT_P3_OUT_P9   (0x1 << 9)
#define PORT_P3_OUT_P9_VAL(val)   (((val) & 0x1) << 9)
#define PORT_P3_OUT_P9_GET(val)   ((((val) & PORT_P3_OUT_P9) >> 9) & 0x1)
#define PORT_P3_OUT_P9_SET(reg,val) (reg) = ((reg & ~PORT_P3_OUT_P9) | (((val) & 0x1) << 9))
/* Port 3 Pin # Output Value (8) */
#define PORT_P3_OUT_P8   (0x1 << 8)
#define PORT_P3_OUT_P8_VAL(val)   (((val) & 0x1) << 8)
#define PORT_P3_OUT_P8_GET(val)   ((((val) & PORT_P3_OUT_P8) >> 8) & 0x1)
#define PORT_P3_OUT_P8_SET(reg,val) (reg) = ((reg & ~PORT_P3_OUT_P8) | (((val) & 0x1) << 8))
/* Port 3 Pin # Output Value (7) */
#define PORT_P3_OUT_P7   (0x1 << 7)
#define PORT_P3_OUT_P7_VAL(val)   (((val) & 0x1) << 7)
#define PORT_P3_OUT_P7_GET(val)   ((((val) & PORT_P3_OUT_P7) >> 7) & 0x1)
#define PORT_P3_OUT_P7_SET(reg,val) (reg) = ((reg & ~PORT_P3_OUT_P7) | (((val) & 0x1) << 7))
/* Port 3 Pin # Output Value (6) */
#define PORT_P3_OUT_P6   (0x1 << 6)
#define PORT_P3_OUT_P6_VAL(val)   (((val) & 0x1) << 6)
#define PORT_P3_OUT_P6_GET(val)   ((((val) & PORT_P3_OUT_P6) >> 6) & 0x1)
#define PORT_P3_OUT_P6_SET(reg,val) (reg) = ((reg & ~PORT_P3_OUT_P6) | (((val) & 0x1) << 6))
/* Port 3 Pin # Output Value (5) */
#define PORT_P3_OUT_P5   (0x1 << 5)
#define PORT_P3_OUT_P5_VAL(val)   (((val) & 0x1) << 5)
#define PORT_P3_OUT_P5_GET(val)   ((((val) & PORT_P3_OUT_P5) >> 5) & 0x1)
#define PORT_P3_OUT_P5_SET(reg,val) (reg) = ((reg & ~PORT_P3_OUT_P5) | (((val) & 0x1) << 5))
/* Port 3 Pin # Output Value (4) */
#define PORT_P3_OUT_P4   (0x1 << 4)
#define PORT_P3_OUT_P4_VAL(val)   (((val) & 0x1) << 4)
#define PORT_P3_OUT_P4_GET(val)   ((((val) & PORT_P3_OUT_P4) >> 4) & 0x1)
#define PORT_P3_OUT_P4_SET(reg,val) (reg) = ((reg & ~PORT_P3_OUT_P4) | (((val) & 0x1) << 4))
/* Port 3 Pin # Output Value (3) */
#define PORT_P3_OUT_P3   (0x1 << 3)
#define PORT_P3_OUT_P3_VAL(val)   (((val) & 0x1) << 3)
#define PORT_P3_OUT_P3_GET(val)   ((((val) & PORT_P3_OUT_P3) >> 3) & 0x1)
#define PORT_P3_OUT_P3_SET(reg,val) (reg) = ((reg & ~PORT_P3_OUT_P3) | (((val) & 0x1) << 3))
/* Port 3 Pin # Output Value (2) */
#define PORT_P3_OUT_P2   (0x1 << 2)
#define PORT_P3_OUT_P2_VAL(val)   (((val) & 0x1) << 2)
#define PORT_P3_OUT_P2_GET(val)   ((((val) & PORT_P3_OUT_P2) >> 2) & 0x1)
#define PORT_P3_OUT_P2_SET(reg,val) (reg) = ((reg & ~PORT_P3_OUT_P2) | (((val) & 0x1) << 2))
/* Port 3 Pin # Output Value (1) */
#define PORT_P3_OUT_P1   (0x1 << 1)
#define PORT_P3_OUT_P1_VAL(val)   (((val) & 0x1) << 1)
#define PORT_P3_OUT_P1_GET(val)   ((((val) & PORT_P3_OUT_P1) >> 1) & 0x1)
#define PORT_P3_OUT_P1_SET(reg,val) (reg) = ((reg & ~PORT_P3_OUT_P1) | (((val) & 0x1) << 1))
/* Port 3 Pin # Output Value (0) */
#define PORT_P3_OUT_P0   (0x1)
#define PORT_P3_OUT_P0_VAL(val)   (((val) & 0x1) << 0)
#define PORT_P3_OUT_P0_GET(val)   ((((val) & PORT_P3_OUT_P0) >> 0) & 0x1)
#define PORT_P3_OUT_P0_SET(reg,val) (reg) = ((reg & ~PORT_P3_OUT_P0) | (((val) & 0x1) << 0))

/*******************************************************************************
 * Port 3 Data Input Register
 ******************************************************************************/

/* Port 3 Pin # Latched Input Value (19) */
#define PORT_P3_IN_P19   (0x1 << 19)
#define PORT_P3_IN_P19_GET(val)   ((((val) & PORT_P3_IN_P19) >> 19) & 0x1)
/* Port 3 Pin # Latched Input Value (18) */
#define PORT_P3_IN_P18   (0x1 << 18)
#define PORT_P3_IN_P18_GET(val)   ((((val) & PORT_P3_IN_P18) >> 18) & 0x1)
/* Port 3 Pin # Latched Input Value (17) */
#define PORT_P3_IN_P17   (0x1 << 17)
#define PORT_P3_IN_P17_GET(val)   ((((val) & PORT_P3_IN_P17) >> 17) & 0x1)
/* Port 3 Pin # Latched Input Value (16) */
#define PORT_P3_IN_P16   (0x1 << 16)
#define PORT_P3_IN_P16_GET(val)   ((((val) & PORT_P3_IN_P16) >> 16) & 0x1)
/* Port 3 Pin # Latched Input Value (15) */
#define PORT_P3_IN_P15   (0x1 << 15)
#define PORT_P3_IN_P15_GET(val)   ((((val) & PORT_P3_IN_P15) >> 15) & 0x1)
/* Port 3 Pin # Latched Input Value (14) */
#define PORT_P3_IN_P14   (0x1 << 14)
#define PORT_P3_IN_P14_GET(val)   ((((val) & PORT_P3_IN_P14) >> 14) & 0x1)
/* Port 3 Pin # Latched Input Value (13) */
#define PORT_P3_IN_P13   (0x1 << 13)
#define PORT_P3_IN_P13_GET(val)   ((((val) & PORT_P3_IN_P13) >> 13) & 0x1)
/* Port 3 Pin # Latched Input Value (12) */
#define PORT_P3_IN_P12   (0x1 << 12)
#define PORT_P3_IN_P12_GET(val)   ((((val) & PORT_P3_IN_P12) >> 12) & 0x1)
/* Port 3 Pin # Latched Input Value (11) */
#define PORT_P3_IN_P11   (0x1 << 11)
#define PORT_P3_IN_P11_GET(val)   ((((val) & PORT_P3_IN_P11) >> 11) & 0x1)
/* Port 3 Pin # Latched Input Value (10) */
#define PORT_P3_IN_P10   (0x1 << 10)
#define PORT_P3_IN_P10_GET(val)   ((((val) & PORT_P3_IN_P10) >> 10) & 0x1)
/* Port 3 Pin # Latched Input Value (9) */
#define PORT_P3_IN_P9   (0x1 << 9)
#define PORT_P3_IN_P9_GET(val)   ((((val) & PORT_P3_IN_P9) >> 9) & 0x1)
/* Port 3 Pin # Latched Input Value (8) */
#define PORT_P3_IN_P8   (0x1 << 8)
#define PORT_P3_IN_P8_GET(val)   ((((val) & PORT_P3_IN_P8) >> 8) & 0x1)
/* Port 3 Pin # Latched Input Value (7) */
#define PORT_P3_IN_P7   (0x1 << 7)
#define PORT_P3_IN_P7_GET(val)   ((((val) & PORT_P3_IN_P7) >> 7) & 0x1)
/* Port 3 Pin # Latched Input Value (6) */
#define PORT_P3_IN_P6   (0x1 << 6)
#define PORT_P3_IN_P6_GET(val)   ((((val) & PORT_P3_IN_P6) >> 6) & 0x1)
/* Port 3 Pin # Latched Input Value (5) */
#define PORT_P3_IN_P5   (0x1 << 5)
#define PORT_P3_IN_P5_GET(val)   ((((val) & PORT_P3_IN_P5) >> 5) & 0x1)
/* Port 3 Pin # Latched Input Value (4) */
#define PORT_P3_IN_P4   (0x1 << 4)
#define PORT_P3_IN_P4_GET(val)   ((((val) & PORT_P3_IN_P4) >> 4) & 0x1)
/* Port 3 Pin # Latched Input Value (3) */
#define PORT_P3_IN_P3   (0x1 << 3)
#define PORT_P3_IN_P3_GET(val)   ((((val) & PORT_P3_IN_P3) >> 3) & 0x1)
/* Port 3 Pin # Latched Input Value (2) */
#define PORT_P3_IN_P2   (0x1 << 2)
#define PORT_P3_IN_P2_GET(val)   ((((val) & PORT_P3_IN_P2) >> 2) & 0x1)
/* Port 3 Pin # Latched Input Value (1) */
#define PORT_P3_IN_P1   (0x1 << 1)
#define PORT_P3_IN_P1_GET(val)   ((((val) & PORT_P3_IN_P1) >> 1) & 0x1)
/* Port 3 Pin # Latched Input Value (0) */
#define PORT_P3_IN_P0   (0x1)
#define PORT_P3_IN_P0_GET(val)   ((((val) & PORT_P3_IN_P0) >> 0) & 0x1)

/*******************************************************************************
 * Port 3 Direction Register
 ******************************************************************************/

/* Port 3 Pin #Direction Control (19) */
#define PORT_P3_DIR_P19   (0x1 << 19)
#define PORT_P3_DIR_P19_VAL(val)   (((val) & 0x1) << 19)
#define PORT_P3_DIR_P19_GET(val)   ((((val) & PORT_P3_DIR_P19) >> 19) & 0x1)
#define PORT_P3_DIR_P19_SET(reg,val) (reg) = ((reg & ~PORT_P3_DIR_P19) | (((val) & 0x1) << 19))
/* Port 3 Pin #Direction Control (18) */
#define PORT_P3_DIR_P18   (0x1 << 18)
#define PORT_P3_DIR_P18_VAL(val)   (((val) & 0x1) << 18)
#define PORT_P3_DIR_P18_GET(val)   ((((val) & PORT_P3_DIR_P18) >> 18) & 0x1)
#define PORT_P3_DIR_P18_SET(reg,val) (reg) = ((reg & ~PORT_P3_DIR_P18) | (((val) & 0x1) << 18))
/* Port 3 Pin #Direction Control (17) */
#define PORT_P3_DIR_P17   (0x1 << 17)
#define PORT_P3_DIR_P17_VAL(val)   (((val) & 0x1) << 17)
#define PORT_P3_DIR_P17_GET(val)   ((((val) & PORT_P3_DIR_P17) >> 17) & 0x1)
#define PORT_P3_DIR_P17_SET(reg,val) (reg) = ((reg & ~PORT_P3_DIR_P17) | (((val) & 0x1) << 17))
/* Port 3 Pin #Direction Control (16) */
#define PORT_P3_DIR_P16   (0x1 << 16)
#define PORT_P3_DIR_P16_VAL(val)   (((val) & 0x1) << 16)
#define PORT_P3_DIR_P16_GET(val)   ((((val) & PORT_P3_DIR_P16) >> 16) & 0x1)
#define PORT_P3_DIR_P16_SET(reg,val) (reg) = ((reg & ~PORT_P3_DIR_P16) | (((val) & 0x1) << 16))
/* Port 3 Pin #Direction Control (15) */
#define PORT_P3_DIR_P15   (0x1 << 15)
#define PORT_P3_DIR_P15_VAL(val)   (((val) & 0x1) << 15)
#define PORT_P3_DIR_P15_GET(val)   ((((val) & PORT_P3_DIR_P15) >> 15) & 0x1)
#define PORT_P3_DIR_P15_SET(reg,val) (reg) = ((reg & ~PORT_P3_DIR_P15) | (((val) & 0x1) << 15))
/* Port 3 Pin #Direction Control (14) */
#define PORT_P3_DIR_P14   (0x1 << 14)
#define PORT_P3_DIR_P14_VAL(val)   (((val) & 0x1) << 14)
#define PORT_P3_DIR_P14_GET(val)   ((((val) & PORT_P3_DIR_P14) >> 14) & 0x1)
#define PORT_P3_DIR_P14_SET(reg,val) (reg) = ((reg & ~PORT_P3_DIR_P14) | (((val) & 0x1) << 14))
/* Port 3 Pin #Direction Control (13) */
#define PORT_P3_DIR_P13   (0x1 << 13)
#define PORT_P3_DIR_P13_VAL(val)   (((val) & 0x1) << 13)
#define PORT_P3_DIR_P13_GET(val)   ((((val) & PORT_P3_DIR_P13) >> 13) & 0x1)
#define PORT_P3_DIR_P13_SET(reg,val) (reg) = ((reg & ~PORT_P3_DIR_P13) | (((val) & 0x1) << 13))
/* Port 3 Pin #Direction Control (12) */
#define PORT_P3_DIR_P12   (0x1 << 12)
#define PORT_P3_DIR_P12_VAL(val)   (((val) & 0x1) << 12)
#define PORT_P3_DIR_P12_GET(val)   ((((val) & PORT_P3_DIR_P12) >> 12) & 0x1)
#define PORT_P3_DIR_P12_SET(reg,val) (reg) = ((reg & ~PORT_P3_DIR_P12) | (((val) & 0x1) << 12))
/* Port 3 Pin #Direction Control (11) */
#define PORT_P3_DIR_P11   (0x1 << 11)
#define PORT_P3_DIR_P11_VAL(val)   (((val) & 0x1) << 11)
#define PORT_P3_DIR_P11_GET(val)   ((((val) & PORT_P3_DIR_P11) >> 11) & 0x1)
#define PORT_P3_DIR_P11_SET(reg,val) (reg) = ((reg & ~PORT_P3_DIR_P11) | (((val) & 0x1) << 11))
/* Port 3 Pin #Direction Control (10) */
#define PORT_P3_DIR_P10   (0x1 << 10)
#define PORT_P3_DIR_P10_VAL(val)   (((val) & 0x1) << 10)
#define PORT_P3_DIR_P10_GET(val)   ((((val) & PORT_P3_DIR_P10) >> 10) & 0x1)
#define PORT_P3_DIR_P10_SET(reg,val) (reg) = ((reg & ~PORT_P3_DIR_P10) | (((val) & 0x1) << 10))
/* Port 3 Pin #Direction Control (9) */
#define PORT_P3_DIR_P9   (0x1 << 9)
#define PORT_P3_DIR_P9_VAL(val)   (((val) & 0x1) << 9)
#define PORT_P3_DIR_P9_GET(val)   ((((val) & PORT_P3_DIR_P9) >> 9) & 0x1)
#define PORT_P3_DIR_P9_SET(reg,val) (reg) = ((reg & ~PORT_P3_DIR_P9) | (((val) & 0x1) << 9))
/* Port 3 Pin #Direction Control (8) */
#define PORT_P3_DIR_P8   (0x1 << 8)
#define PORT_P3_DIR_P8_VAL(val)   (((val) & 0x1) << 8)
#define PORT_P3_DIR_P8_GET(val)   ((((val) & PORT_P3_DIR_P8) >> 8) & 0x1)
#define PORT_P3_DIR_P8_SET(reg,val) (reg) = ((reg & ~PORT_P3_DIR_P8) | (((val) & 0x1) << 8))
/* Port 3 Pin #Direction Control (7) */
#define PORT_P3_DIR_P7   (0x1 << 7)
#define PORT_P3_DIR_P7_VAL(val)   (((val) & 0x1) << 7)
#define PORT_P3_DIR_P7_GET(val)   ((((val) & PORT_P3_DIR_P7) >> 7) & 0x1)
#define PORT_P3_DIR_P7_SET(reg,val) (reg) = ((reg & ~PORT_P3_DIR_P7) | (((val) & 0x1) << 7))
/* Port 3 Pin #Direction Control (6) */
#define PORT_P3_DIR_P6   (0x1 << 6)
#define PORT_P3_DIR_P6_VAL(val)   (((val) & 0x1) << 6)
#define PORT_P3_DIR_P6_GET(val)   ((((val) & PORT_P3_DIR_P6) >> 6) & 0x1)
#define PORT_P3_DIR_P6_SET(reg,val) (reg) = ((reg & ~PORT_P3_DIR_P6) | (((val) & 0x1) << 6))
/* Port 3 Pin #Direction Control (5) */
#define PORT_P3_DIR_P5   (0x1 << 5)
#define PORT_P3_DIR_P5_VAL(val)   (((val) & 0x1) << 5)
#define PORT_P3_DIR_P5_GET(val)   ((((val) & PORT_P3_DIR_P5) >> 5) & 0x1)
#define PORT_P3_DIR_P5_SET(reg,val) (reg) = ((reg & ~PORT_P3_DIR_P5) | (((val) & 0x1) << 5))
/* Port 3 Pin #Direction Control (4) */
#define PORT_P3_DIR_P4   (0x1 << 4)
#define PORT_P3_DIR_P4_VAL(val)   (((val) & 0x1) << 4)
#define PORT_P3_DIR_P4_GET(val)   ((((val) & PORT_P3_DIR_P4) >> 4) & 0x1)
#define PORT_P3_DIR_P4_SET(reg,val) (reg) = ((reg & ~PORT_P3_DIR_P4) | (((val) & 0x1) << 4))
/* Port 3 Pin #Direction Control (3) */
#define PORT_P3_DIR_P3   (0x1 << 3)
#define PORT_P3_DIR_P3_VAL(val)   (((val) & 0x1) << 3)
#define PORT_P3_DIR_P3_GET(val)   ((((val) & PORT_P3_DIR_P3) >> 3) & 0x1)
#define PORT_P3_DIR_P3_SET(reg,val) (reg) = ((reg & ~PORT_P3_DIR_P3) | (((val) & 0x1) << 3))
/* Port 3 Pin #Direction Control (2) */
#define PORT_P3_DIR_P2   (0x1 << 2)
#define PORT_P3_DIR_P2_VAL(val)   (((val) & 0x1) << 2)
#define PORT_P3_DIR_P2_GET(val)   ((((val) & PORT_P3_DIR_P2) >> 2) & 0x1)
#define PORT_P3_DIR_P2_SET(reg,val) (reg) = ((reg & ~PORT_P3_DIR_P2) | (((val) & 0x1) << 2))
/* Port 3 Pin #Direction Control (1) */
#define PORT_P3_DIR_P1   (0x1 << 1)
#define PORT_P3_DIR_P1_VAL(val)   (((val) & 0x1) << 1)
#define PORT_P3_DIR_P1_GET(val)   ((((val) & PORT_P3_DIR_P1) >> 1) & 0x1)
#define PORT_P3_DIR_P1_SET(reg,val) (reg) = ((reg & ~PORT_P3_DIR_P1) | (((val) & 0x1) << 1))
/* Port 3 Pin #Direction Control (0) */
#define PORT_P3_DIR_P0   (0x1)
#define PORT_P3_DIR_P0_VAL(val)   (((val) & 0x1) << 0)
#define PORT_P3_DIR_P0_GET(val)   ((((val) & PORT_P3_DIR_P0) >> 0) & 0x1)
#define PORT_P3_DIR_P0_SET(reg,val) (reg) = ((reg & ~PORT_P3_DIR_P0) | (((val) & 0x1) << 0))

/*******************************************************************************
 * Port 3 Alternate Function Select Register 0
 ******************************************************************************/

/* Alternate Function at Port 3 Bit # (19) */
#define PORT_P3_ALTSEL0_P19   (0x1 << 19)
#define PORT_P3_ALTSEL0_P19_VAL(val)   (((val) & 0x1) << 19)
#define PORT_P3_ALTSEL0_P19_GET(val)   ((((val) & PORT_P3_ALTSEL0_P19) >> 19) & 0x1)
#define PORT_P3_ALTSEL0_P19_SET(reg,val) (reg) = ((reg & ~PORT_P3_ALTSEL0_P19) | (((val) & 0x1) << 19))
/* Alternate Function at Port 3 Bit # (18) */
#define PORT_P3_ALTSEL0_P18   (0x1 << 18)
#define PORT_P3_ALTSEL0_P18_VAL(val)   (((val) & 0x1) << 18)
#define PORT_P3_ALTSEL0_P18_GET(val)   ((((val) & PORT_P3_ALTSEL0_P18) >> 18) & 0x1)
#define PORT_P3_ALTSEL0_P18_SET(reg,val) (reg) = ((reg & ~PORT_P3_ALTSEL0_P18) | (((val) & 0x1) << 18))
/* Alternate Function at Port 3 Bit # (17) */
#define PORT_P3_ALTSEL0_P17   (0x1 << 17)
#define PORT_P3_ALTSEL0_P17_VAL(val)   (((val) & 0x1) << 17)
#define PORT_P3_ALTSEL0_P17_GET(val)   ((((val) & PORT_P3_ALTSEL0_P17) >> 17) & 0x1)
#define PORT_P3_ALTSEL0_P17_SET(reg,val) (reg) = ((reg & ~PORT_P3_ALTSEL0_P17) | (((val) & 0x1) << 17))
/* Alternate Function at Port 3 Bit # (16) */
#define PORT_P3_ALTSEL0_P16   (0x1 << 16)
#define PORT_P3_ALTSEL0_P16_VAL(val)   (((val) & 0x1) << 16)
#define PORT_P3_ALTSEL0_P16_GET(val)   ((((val) & PORT_P3_ALTSEL0_P16) >> 16) & 0x1)
#define PORT_P3_ALTSEL0_P16_SET(reg,val) (reg) = ((reg & ~PORT_P3_ALTSEL0_P16) | (((val) & 0x1) << 16))
/* Alternate Function at Port 3 Bit # (15) */
#define PORT_P3_ALTSEL0_P15   (0x1 << 15)
#define PORT_P3_ALTSEL0_P15_VAL(val)   (((val) & 0x1) << 15)
#define PORT_P3_ALTSEL0_P15_GET(val)   ((((val) & PORT_P3_ALTSEL0_P15) >> 15) & 0x1)
#define PORT_P3_ALTSEL0_P15_SET(reg,val) (reg) = ((reg & ~PORT_P3_ALTSEL0_P15) | (((val) & 0x1) << 15))
/* Alternate Function at Port 3 Bit # (14) */
#define PORT_P3_ALTSEL0_P14   (0x1 << 14)
#define PORT_P3_ALTSEL0_P14_VAL(val)   (((val) & 0x1) << 14)
#define PORT_P3_ALTSEL0_P14_GET(val)   ((((val) & PORT_P3_ALTSEL0_P14) >> 14) & 0x1)
#define PORT_P3_ALTSEL0_P14_SET(reg,val) (reg) = ((reg & ~PORT_P3_ALTSEL0_P14) | (((val) & 0x1) << 14))
/* Alternate Function at Port 3 Bit # (13) */
#define PORT_P3_ALTSEL0_P13   (0x1 << 13)
#define PORT_P3_ALTSEL0_P13_VAL(val)   (((val) & 0x1) << 13)
#define PORT_P3_ALTSEL0_P13_GET(val)   ((((val) & PORT_P3_ALTSEL0_P13) >> 13) & 0x1)
#define PORT_P3_ALTSEL0_P13_SET(reg,val) (reg) = ((reg & ~PORT_P3_ALTSEL0_P13) | (((val) & 0x1) << 13))
/* Alternate Function at Port 3 Bit # (12) */
#define PORT_P3_ALTSEL0_P12   (0x1 << 12)
#define PORT_P3_ALTSEL0_P12_VAL(val)   (((val) & 0x1) << 12)
#define PORT_P3_ALTSEL0_P12_GET(val)   ((((val) & PORT_P3_ALTSEL0_P12) >> 12) & 0x1)
#define PORT_P3_ALTSEL0_P12_SET(reg,val) (reg) = ((reg & ~PORT_P3_ALTSEL0_P12) | (((val) & 0x1) << 12))
/* Alternate Function at Port 3 Bit # (11) */
#define PORT_P3_ALTSEL0_P11   (0x1 << 11)
#define PORT_P3_ALTSEL0_P11_VAL(val)   (((val) & 0x1) << 11)
#define PORT_P3_ALTSEL0_P11_GET(val)   ((((val) & PORT_P3_ALTSEL0_P11) >> 11) & 0x1)
#define PORT_P3_ALTSEL0_P11_SET(reg,val) (reg) = ((reg & ~PORT_P3_ALTSEL0_P11) | (((val) & 0x1) << 11))
/* Alternate Function at Port 3 Bit # (10) */
#define PORT_P3_ALTSEL0_P10   (0x1 << 10)
#define PORT_P3_ALTSEL0_P10_VAL(val)   (((val) & 0x1) << 10)
#define PORT_P3_ALTSEL0_P10_GET(val)   ((((val) & PORT_P3_ALTSEL0_P10) >> 10) & 0x1)
#define PORT_P3_ALTSEL0_P10_SET(reg,val) (reg) = ((reg & ~PORT_P3_ALTSEL0_P10) | (((val) & 0x1) << 10))
/* Alternate Function at Port 3 Bit # (9) */
#define PORT_P3_ALTSEL0_P9   (0x1 << 9)
#define PORT_P3_ALTSEL0_P9_VAL(val)   (((val) & 0x1) << 9)
#define PORT_P3_ALTSEL0_P9_GET(val)   ((((val) & PORT_P3_ALTSEL0_P9) >> 9) & 0x1)
#define PORT_P3_ALTSEL0_P9_SET(reg,val) (reg) = ((reg & ~PORT_P3_ALTSEL0_P9) | (((val) & 0x1) << 9))
/* Alternate Function at Port 3 Bit # (8) */
#define PORT_P3_ALTSEL0_P8   (0x1 << 8)
#define PORT_P3_ALTSEL0_P8_VAL(val)   (((val) & 0x1) << 8)
#define PORT_P3_ALTSEL0_P8_GET(val)   ((((val) & PORT_P3_ALTSEL0_P8) >> 8) & 0x1)
#define PORT_P3_ALTSEL0_P8_SET(reg,val) (reg) = ((reg & ~PORT_P3_ALTSEL0_P8) | (((val) & 0x1) << 8))
/* Alternate Function at Port 3 Bit # (7) */
#define PORT_P3_ALTSEL0_P7   (0x1 << 7)
#define PORT_P3_ALTSEL0_P7_VAL(val)   (((val) & 0x1) << 7)
#define PORT_P3_ALTSEL0_P7_GET(val)   ((((val) & PORT_P3_ALTSEL0_P7) >> 7) & 0x1)
#define PORT_P3_ALTSEL0_P7_SET(reg,val) (reg) = ((reg & ~PORT_P3_ALTSEL0_P7) | (((val) & 0x1) << 7))
/* Alternate Function at Port 3 Bit # (6) */
#define PORT_P3_ALTSEL0_P6   (0x1 << 6)
#define PORT_P3_ALTSEL0_P6_VAL(val)   (((val) & 0x1) << 6)
#define PORT_P3_ALTSEL0_P6_GET(val)   ((((val) & PORT_P3_ALTSEL0_P6) >> 6) & 0x1)
#define PORT_P3_ALTSEL0_P6_SET(reg,val) (reg) = ((reg & ~PORT_P3_ALTSEL0_P6) | (((val) & 0x1) << 6))
/* Alternate Function at Port 3 Bit # (5) */
#define PORT_P3_ALTSEL0_P5   (0x1 << 5)
#define PORT_P3_ALTSEL0_P5_VAL(val)   (((val) & 0x1) << 5)
#define PORT_P3_ALTSEL0_P5_GET(val)   ((((val) & PORT_P3_ALTSEL0_P5) >> 5) & 0x1)
#define PORT_P3_ALTSEL0_P5_SET(reg,val) (reg) = ((reg & ~PORT_P3_ALTSEL0_P5) | (((val) & 0x1) << 5))
/* Alternate Function at Port 3 Bit # (4) */
#define PORT_P3_ALTSEL0_P4   (0x1 << 4)
#define PORT_P3_ALTSEL0_P4_VAL(val)   (((val) & 0x1) << 4)
#define PORT_P3_ALTSEL0_P4_GET(val)   ((((val) & PORT_P3_ALTSEL0_P4) >> 4) & 0x1)
#define PORT_P3_ALTSEL0_P4_SET(reg,val) (reg) = ((reg & ~PORT_P3_ALTSEL0_P4) | (((val) & 0x1) << 4))
/* Alternate Function at Port 3 Bit # (3) */
#define PORT_P3_ALTSEL0_P3   (0x1 << 3)
#define PORT_P3_ALTSEL0_P3_VAL(val)   (((val) & 0x1) << 3)
#define PORT_P3_ALTSEL0_P3_GET(val)   ((((val) & PORT_P3_ALTSEL0_P3) >> 3) & 0x1)
#define PORT_P3_ALTSEL0_P3_SET(reg,val) (reg) = ((reg & ~PORT_P3_ALTSEL0_P3) | (((val) & 0x1) << 3))
/* Alternate Function at Port 3 Bit # (2) */
#define PORT_P3_ALTSEL0_P2   (0x1 << 2)
#define PORT_P3_ALTSEL0_P2_VAL(val)   (((val) & 0x1) << 2)
#define PORT_P3_ALTSEL0_P2_GET(val)   ((((val) & PORT_P3_ALTSEL0_P2) >> 2) & 0x1)
#define PORT_P3_ALTSEL0_P2_SET(reg,val) (reg) = ((reg & ~PORT_P3_ALTSEL0_P2) | (((val) & 0x1) << 2))
/* Alternate Function at Port 3 Bit # (1) */
#define PORT_P3_ALTSEL0_P1   (0x1 << 1)
#define PORT_P3_ALTSEL0_P1_VAL(val)   (((val) & 0x1) << 1)
#define PORT_P3_ALTSEL0_P1_GET(val)   ((((val) & PORT_P3_ALTSEL0_P1) >> 1) & 0x1)
#define PORT_P3_ALTSEL0_P1_SET(reg,val) (reg) = ((reg & ~PORT_P3_ALTSEL0_P1) | (((val) & 0x1) << 1))
/* Alternate Function at Port 3 Bit # (0) */
#define PORT_P3_ALTSEL0_P0   (0x1)
#define PORT_P3_ALTSEL0_P0_VAL(val)   (((val) & 0x1) << 0)
#define PORT_P3_ALTSEL0_P0_GET(val)   ((((val) & PORT_P3_ALTSEL0_P0) >> 0) & 0x1)
#define PORT_P3_ALTSEL0_P0_SET(reg,val) (reg) = ((reg & ~PORT_P3_ALTSEL0_P0) | (((val) & 0x1) << 0))

/*******************************************************************************
 * Port 3 Pull Up Device Enable Register
 ******************************************************************************/

/* Pull Up Device Enable at Port 3 Bit # (19) */
#define PORT_P3_PUEN_P19   (0x1 << 19)
#define PORT_P3_PUEN_P19_VAL(val)   (((val) & 0x1) << 19)
#define PORT_P3_PUEN_P19_GET(val)   ((((val) & PORT_P3_PUEN_P19) >> 19) & 0x1)
#define PORT_P3_PUEN_P19_SET(reg,val) (reg) = ((reg & ~PORT_P3_PUEN_P19) | (((val) & 0x1) << 19))
/* Pull Up Device Enable at Port 3 Bit # (18) */
#define PORT_P3_PUEN_P18   (0x1 << 18)
#define PORT_P3_PUEN_P18_VAL(val)   (((val) & 0x1) << 18)
#define PORT_P3_PUEN_P18_GET(val)   ((((val) & PORT_P3_PUEN_P18) >> 18) & 0x1)
#define PORT_P3_PUEN_P18_SET(reg,val) (reg) = ((reg & ~PORT_P3_PUEN_P18) | (((val) & 0x1) << 18))
/* Pull Up Device Enable at Port 3 Bit # (17) */
#define PORT_P3_PUEN_P17   (0x1 << 17)
#define PORT_P3_PUEN_P17_VAL(val)   (((val) & 0x1) << 17)
#define PORT_P3_PUEN_P17_GET(val)   ((((val) & PORT_P3_PUEN_P17) >> 17) & 0x1)
#define PORT_P3_PUEN_P17_SET(reg,val) (reg) = ((reg & ~PORT_P3_PUEN_P17) | (((val) & 0x1) << 17))
/* Pull Up Device Enable at Port 3 Bit # (16) */
#define PORT_P3_PUEN_P16   (0x1 << 16)
#define PORT_P3_PUEN_P16_VAL(val)   (((val) & 0x1) << 16)
#define PORT_P3_PUEN_P16_GET(val)   ((((val) & PORT_P3_PUEN_P16) >> 16) & 0x1)
#define PORT_P3_PUEN_P16_SET(reg,val) (reg) = ((reg & ~PORT_P3_PUEN_P16) | (((val) & 0x1) << 16))
/* Pull Up Device Enable at Port 3 Bit # (15) */
#define PORT_P3_PUEN_P15   (0x1 << 15)
#define PORT_P3_PUEN_P15_VAL(val)   (((val) & 0x1) << 15)
#define PORT_P3_PUEN_P15_GET(val)   ((((val) & PORT_P3_PUEN_P15) >> 15) & 0x1)
#define PORT_P3_PUEN_P15_SET(reg,val) (reg) = ((reg & ~PORT_P3_PUEN_P15) | (((val) & 0x1) << 15))
/* Pull Up Device Enable at Port 3 Bit # (14) */
#define PORT_P3_PUEN_P14   (0x1 << 14)
#define PORT_P3_PUEN_P14_VAL(val)   (((val) & 0x1) << 14)
#define PORT_P3_PUEN_P14_GET(val)   ((((val) & PORT_P3_PUEN_P14) >> 14) & 0x1)
#define PORT_P3_PUEN_P14_SET(reg,val) (reg) = ((reg & ~PORT_P3_PUEN_P14) | (((val) & 0x1) << 14))
/* Pull Up Device Enable at Port 3 Bit # (13) */
#define PORT_P3_PUEN_P13   (0x1 << 13)
#define PORT_P3_PUEN_P13_VAL(val)   (((val) & 0x1) << 13)
#define PORT_P3_PUEN_P13_GET(val)   ((((val) & PORT_P3_PUEN_P13) >> 13) & 0x1)
#define PORT_P3_PUEN_P13_SET(reg,val) (reg) = ((reg & ~PORT_P3_PUEN_P13) | (((val) & 0x1) << 13))
/* Pull Up Device Enable at Port 3 Bit # (12) */
#define PORT_P3_PUEN_P12   (0x1 << 12)
#define PORT_P3_PUEN_P12_VAL(val)   (((val) & 0x1) << 12)
#define PORT_P3_PUEN_P12_GET(val)   ((((val) & PORT_P3_PUEN_P12) >> 12) & 0x1)
#define PORT_P3_PUEN_P12_SET(reg,val) (reg) = ((reg & ~PORT_P3_PUEN_P12) | (((val) & 0x1) << 12))
/* Pull Up Device Enable at Port 3 Bit # (11) */
#define PORT_P3_PUEN_P11   (0x1 << 11)
#define PORT_P3_PUEN_P11_VAL(val)   (((val) & 0x1) << 11)
#define PORT_P3_PUEN_P11_GET(val)   ((((val) & PORT_P3_PUEN_P11) >> 11) & 0x1)
#define PORT_P3_PUEN_P11_SET(reg,val) (reg) = ((reg & ~PORT_P3_PUEN_P11) | (((val) & 0x1) << 11))
/* Pull Up Device Enable at Port 3 Bit # (10) */
#define PORT_P3_PUEN_P10   (0x1 << 10)
#define PORT_P3_PUEN_P10_VAL(val)   (((val) & 0x1) << 10)
#define PORT_P3_PUEN_P10_GET(val)   ((((val) & PORT_P3_PUEN_P10) >> 10) & 0x1)
#define PORT_P3_PUEN_P10_SET(reg,val) (reg) = ((reg & ~PORT_P3_PUEN_P10) | (((val) & 0x1) << 10))
/* Pull Up Device Enable at Port 3 Bit # (9) */
#define PORT_P3_PUEN_P9   (0x1 << 9)
#define PORT_P3_PUEN_P9_VAL(val)   (((val) & 0x1) << 9)
#define PORT_P3_PUEN_P9_GET(val)   ((((val) & PORT_P3_PUEN_P9) >> 9) & 0x1)
#define PORT_P3_PUEN_P9_SET(reg,val) (reg) = ((reg & ~PORT_P3_PUEN_P9) | (((val) & 0x1) << 9))
/* Pull Up Device Enable at Port 3 Bit # (8) */
#define PORT_P3_PUEN_P8   (0x1 << 8)
#define PORT_P3_PUEN_P8_VAL(val)   (((val) & 0x1) << 8)
#define PORT_P3_PUEN_P8_GET(val)   ((((val) & PORT_P3_PUEN_P8) >> 8) & 0x1)
#define PORT_P3_PUEN_P8_SET(reg,val) (reg) = ((reg & ~PORT_P3_PUEN_P8) | (((val) & 0x1) << 8))
/* Pull Up Device Enable at Port 3 Bit # (7) */
#define PORT_P3_PUEN_P7   (0x1 << 7)
#define PORT_P3_PUEN_P7_VAL(val)   (((val) & 0x1) << 7)
#define PORT_P3_PUEN_P7_GET(val)   ((((val) & PORT_P3_PUEN_P7) >> 7) & 0x1)
#define PORT_P3_PUEN_P7_SET(reg,val) (reg) = ((reg & ~PORT_P3_PUEN_P7) | (((val) & 0x1) << 7))
/* Pull Up Device Enable at Port 3 Bit # (6) */
#define PORT_P3_PUEN_P6   (0x1 << 6)
#define PORT_P3_PUEN_P6_VAL(val)   (((val) & 0x1) << 6)
#define PORT_P3_PUEN_P6_GET(val)   ((((val) & PORT_P3_PUEN_P6) >> 6) & 0x1)
#define PORT_P3_PUEN_P6_SET(reg,val) (reg) = ((reg & ~PORT_P3_PUEN_P6) | (((val) & 0x1) << 6))
/* Pull Up Device Enable at Port 3 Bit # (5) */
#define PORT_P3_PUEN_P5   (0x1 << 5)
#define PORT_P3_PUEN_P5_VAL(val)   (((val) & 0x1) << 5)
#define PORT_P3_PUEN_P5_GET(val)   ((((val) & PORT_P3_PUEN_P5) >> 5) & 0x1)
#define PORT_P3_PUEN_P5_SET(reg,val) (reg) = ((reg & ~PORT_P3_PUEN_P5) | (((val) & 0x1) << 5))
/* Pull Up Device Enable at Port 3 Bit # (4) */
#define PORT_P3_PUEN_P4   (0x1 << 4)
#define PORT_P3_PUEN_P4_VAL(val)   (((val) & 0x1) << 4)
#define PORT_P3_PUEN_P4_GET(val)   ((((val) & PORT_P3_PUEN_P4) >> 4) & 0x1)
#define PORT_P3_PUEN_P4_SET(reg,val) (reg) = ((reg & ~PORT_P3_PUEN_P4) | (((val) & 0x1) << 4))
/* Pull Up Device Enable at Port 3 Bit # (3) */
#define PORT_P3_PUEN_P3   (0x1 << 3)
#define PORT_P3_PUEN_P3_VAL(val)   (((val) & 0x1) << 3)
#define PORT_P3_PUEN_P3_GET(val)   ((((val) & PORT_P3_PUEN_P3) >> 3) & 0x1)
#define PORT_P3_PUEN_P3_SET(reg,val) (reg) = ((reg & ~PORT_P3_PUEN_P3) | (((val) & 0x1) << 3))
/* Pull Up Device Enable at Port 3 Bit # (2) */
#define PORT_P3_PUEN_P2   (0x1 << 2)
#define PORT_P3_PUEN_P2_VAL(val)   (((val) & 0x1) << 2)
#define PORT_P3_PUEN_P2_GET(val)   ((((val) & PORT_P3_PUEN_P2) >> 2) & 0x1)
#define PORT_P3_PUEN_P2_SET(reg,val) (reg) = ((reg & ~PORT_P3_PUEN_P2) | (((val) & 0x1) << 2))
/* Pull Up Device Enable at Port 3 Bit # (1) */
#define PORT_P3_PUEN_P1   (0x1 << 1)
#define PORT_P3_PUEN_P1_VAL(val)   (((val) & 0x1) << 1)
#define PORT_P3_PUEN_P1_GET(val)   ((((val) & PORT_P3_PUEN_P1) >> 1) & 0x1)
#define PORT_P3_PUEN_P1_SET(reg,val) (reg) = ((reg & ~PORT_P3_PUEN_P1) | (((val) & 0x1) << 1))
/* Pull Up Device Enable at Port 3 Bit # (0) */
#define PORT_P3_PUEN_P0   (0x1)
#define PORT_P3_PUEN_P0_VAL(val)   (((val) & 0x1) << 0)
#define PORT_P3_PUEN_P0_GET(val)   ((((val) & PORT_P3_PUEN_P0) >> 0) & 0x1)
#define PORT_P3_PUEN_P0_SET(reg,val) (reg) = ((reg & ~PORT_P3_PUEN_P0) | (((val) & 0x1) << 0))

/*******************************************************************************
 * Port 4 Data Output Register
 ******************************************************************************/

/* Port 4 Pin # Output Value (23) */
#define PORT_P4_OUT_P23   (0x1 << 23)
#define PORT_P4_OUT_P23_VAL(val)   (((val) & 0x1) << 23)
#define PORT_P4_OUT_P23_GET(val)   ((((val) & PORT_P4_OUT_P23) >> 23) & 0x1)
#define PORT_P4_OUT_P23_SET(reg,val) (reg) = ((reg & ~PORT_P4_OUT_P23) | (((val) & 0x1) << 23))
/* Port 4 Pin # Output Value (22) */
#define PORT_P4_OUT_P22   (0x1 << 22)
#define PORT_P4_OUT_P22_VAL(val)   (((val) & 0x1) << 22)
#define PORT_P4_OUT_P22_GET(val)   ((((val) & PORT_P4_OUT_P22) >> 22) & 0x1)
#define PORT_P4_OUT_P22_SET(reg,val) (reg) = ((reg & ~PORT_P4_OUT_P22) | (((val) & 0x1) << 22))
/* Port 4 Pin # Output Value (21) */
#define PORT_P4_OUT_P21   (0x1 << 21)
#define PORT_P4_OUT_P21_VAL(val)   (((val) & 0x1) << 21)
#define PORT_P4_OUT_P21_GET(val)   ((((val) & PORT_P4_OUT_P21) >> 21) & 0x1)
#define PORT_P4_OUT_P21_SET(reg,val) (reg) = ((reg & ~PORT_P4_OUT_P21) | (((val) & 0x1) << 21))
/* Port 4 Pin # Output Value (20) */
#define PORT_P4_OUT_P20   (0x1 << 20)
#define PORT_P4_OUT_P20_VAL(val)   (((val) & 0x1) << 20)
#define PORT_P4_OUT_P20_GET(val)   ((((val) & PORT_P4_OUT_P20) >> 20) & 0x1)
#define PORT_P4_OUT_P20_SET(reg,val) (reg) = ((reg & ~PORT_P4_OUT_P20) | (((val) & 0x1) << 20))
/* Port 4 Pin # Output Value (19) */
#define PORT_P4_OUT_P19   (0x1 << 19)
#define PORT_P4_OUT_P19_VAL(val)   (((val) & 0x1) << 19)
#define PORT_P4_OUT_P19_GET(val)   ((((val) & PORT_P4_OUT_P19) >> 19) & 0x1)
#define PORT_P4_OUT_P19_SET(reg,val) (reg) = ((reg & ~PORT_P4_OUT_P19) | (((val) & 0x1) << 19))
/* Port 4 Pin # Output Value (18) */
#define PORT_P4_OUT_P18   (0x1 << 18)
#define PORT_P4_OUT_P18_VAL(val)   (((val) & 0x1) << 18)
#define PORT_P4_OUT_P18_GET(val)   ((((val) & PORT_P4_OUT_P18) >> 18) & 0x1)
#define PORT_P4_OUT_P18_SET(reg,val) (reg) = ((reg & ~PORT_P4_OUT_P18) | (((val) & 0x1) << 18))
/* Port 4 Pin # Output Value (17) */
#define PORT_P4_OUT_P17   (0x1 << 17)
#define PORT_P4_OUT_P17_VAL(val)   (((val) & 0x1) << 17)
#define PORT_P4_OUT_P17_GET(val)   ((((val) & PORT_P4_OUT_P17) >> 17) & 0x1)
#define PORT_P4_OUT_P17_SET(reg,val) (reg) = ((reg & ~PORT_P4_OUT_P17) | (((val) & 0x1) << 17))
/* Port 4 Pin # Output Value (16) */
#define PORT_P4_OUT_P16   (0x1 << 16)
#define PORT_P4_OUT_P16_VAL(val)   (((val) & 0x1) << 16)
#define PORT_P4_OUT_P16_GET(val)   ((((val) & PORT_P4_OUT_P16) >> 16) & 0x1)
#define PORT_P4_OUT_P16_SET(reg,val) (reg) = ((reg & ~PORT_P4_OUT_P16) | (((val) & 0x1) << 16))
/* Port 4 Pin # Output Value (15) */
#define PORT_P4_OUT_P15   (0x1 << 15)
#define PORT_P4_OUT_P15_VAL(val)   (((val) & 0x1) << 15)
#define PORT_P4_OUT_P15_GET(val)   ((((val) & PORT_P4_OUT_P15) >> 15) & 0x1)
#define PORT_P4_OUT_P15_SET(reg,val) (reg) = ((reg & ~PORT_P4_OUT_P15) | (((val) & 0x1) << 15))
/* Port 4 Pin # Output Value (14) */
#define PORT_P4_OUT_P14   (0x1 << 14)
#define PORT_P4_OUT_P14_VAL(val)   (((val) & 0x1) << 14)
#define PORT_P4_OUT_P14_GET(val)   ((((val) & PORT_P4_OUT_P14) >> 14) & 0x1)
#define PORT_P4_OUT_P14_SET(reg,val) (reg) = ((reg & ~PORT_P4_OUT_P14) | (((val) & 0x1) << 14))
/* Port 4 Pin # Output Value (13) */
#define PORT_P4_OUT_P13   (0x1 << 13)
#define PORT_P4_OUT_P13_VAL(val)   (((val) & 0x1) << 13)
#define PORT_P4_OUT_P13_GET(val)   ((((val) & PORT_P4_OUT_P13) >> 13) & 0x1)
#define PORT_P4_OUT_P13_SET(reg,val) (reg) = ((reg & ~PORT_P4_OUT_P13) | (((val) & 0x1) << 13))
/* Port 4 Pin # Output Value (12) */
#define PORT_P4_OUT_P12   (0x1 << 12)
#define PORT_P4_OUT_P12_VAL(val)   (((val) & 0x1) << 12)
#define PORT_P4_OUT_P12_GET(val)   ((((val) & PORT_P4_OUT_P12) >> 12) & 0x1)
#define PORT_P4_OUT_P12_SET(reg,val) (reg) = ((reg & ~PORT_P4_OUT_P12) | (((val) & 0x1) << 12))
/* Port 4 Pin # Output Value (11) */
#define PORT_P4_OUT_P11   (0x1 << 11)
#define PORT_P4_OUT_P11_VAL(val)   (((val) & 0x1) << 11)
#define PORT_P4_OUT_P11_GET(val)   ((((val) & PORT_P4_OUT_P11) >> 11) & 0x1)
#define PORT_P4_OUT_P11_SET(reg,val) (reg) = ((reg & ~PORT_P4_OUT_P11) | (((val) & 0x1) << 11))
/* Port 4 Pin # Output Value (10) */
#define PORT_P4_OUT_P10   (0x1 << 10)
#define PORT_P4_OUT_P10_VAL(val)   (((val) & 0x1) << 10)
#define PORT_P4_OUT_P10_GET(val)   ((((val) & PORT_P4_OUT_P10) >> 10) & 0x1)
#define PORT_P4_OUT_P10_SET(reg,val) (reg) = ((reg & ~PORT_P4_OUT_P10) | (((val) & 0x1) << 10))
/* Port 4 Pin # Output Value (9) */
#define PORT_P4_OUT_P9   (0x1 << 9)
#define PORT_P4_OUT_P9_VAL(val)   (((val) & 0x1) << 9)
#define PORT_P4_OUT_P9_GET(val)   ((((val) & PORT_P4_OUT_P9) >> 9) & 0x1)
#define PORT_P4_OUT_P9_SET(reg,val) (reg) = ((reg & ~PORT_P4_OUT_P9) | (((val) & 0x1) << 9))
/* Port 4 Pin # Output Value (8) */
#define PORT_P4_OUT_P8   (0x1 << 8)
#define PORT_P4_OUT_P8_VAL(val)   (((val) & 0x1) << 8)
#define PORT_P4_OUT_P8_GET(val)   ((((val) & PORT_P4_OUT_P8) >> 8) & 0x1)
#define PORT_P4_OUT_P8_SET(reg,val) (reg) = ((reg & ~PORT_P4_OUT_P8) | (((val) & 0x1) << 8))
/* Port 4 Pin # Output Value (7) */
#define PORT_P4_OUT_P7   (0x1 << 7)
#define PORT_P4_OUT_P7_VAL(val)   (((val) & 0x1) << 7)
#define PORT_P4_OUT_P7_GET(val)   ((((val) & PORT_P4_OUT_P7) >> 7) & 0x1)
#define PORT_P4_OUT_P7_SET(reg,val) (reg) = ((reg & ~PORT_P4_OUT_P7) | (((val) & 0x1) << 7))
/* Port 4 Pin # Output Value (6) */
#define PORT_P4_OUT_P6   (0x1 << 6)
#define PORT_P4_OUT_P6_VAL(val)   (((val) & 0x1) << 6)
#define PORT_P4_OUT_P6_GET(val)   ((((val) & PORT_P4_OUT_P6) >> 6) & 0x1)
#define PORT_P4_OUT_P6_SET(reg,val) (reg) = ((reg & ~PORT_P4_OUT_P6) | (((val) & 0x1) << 6))
/* Port 4 Pin # Output Value (5) */
#define PORT_P4_OUT_P5   (0x1 << 5)
#define PORT_P4_OUT_P5_VAL(val)   (((val) & 0x1) << 5)
#define PORT_P4_OUT_P5_GET(val)   ((((val) & PORT_P4_OUT_P5) >> 5) & 0x1)
#define PORT_P4_OUT_P5_SET(reg,val) (reg) = ((reg & ~PORT_P4_OUT_P5) | (((val) & 0x1) << 5))
/* Port 4 Pin # Output Value (4) */
#define PORT_P4_OUT_P4   (0x1 << 4)
#define PORT_P4_OUT_P4_VAL(val)   (((val) & 0x1) << 4)
#define PORT_P4_OUT_P4_GET(val)   ((((val) & PORT_P4_OUT_P4) >> 4) & 0x1)
#define PORT_P4_OUT_P4_SET(reg,val) (reg) = ((reg & ~PORT_P4_OUT_P4) | (((val) & 0x1) << 4))
/* Port 4 Pin # Output Value (3) */
#define PORT_P4_OUT_P3   (0x1 << 3)
#define PORT_P4_OUT_P3_VAL(val)   (((val) & 0x1) << 3)
#define PORT_P4_OUT_P3_GET(val)   ((((val) & PORT_P4_OUT_P3) >> 3) & 0x1)
#define PORT_P4_OUT_P3_SET(reg,val) (reg) = ((reg & ~PORT_P4_OUT_P3) | (((val) & 0x1) << 3))
/* Port 4 Pin # Output Value (2) */
#define PORT_P4_OUT_P2   (0x1 << 2)
#define PORT_P4_OUT_P2_VAL(val)   (((val) & 0x1) << 2)
#define PORT_P4_OUT_P2_GET(val)   ((((val) & PORT_P4_OUT_P2) >> 2) & 0x1)
#define PORT_P4_OUT_P2_SET(reg,val) (reg) = ((reg & ~PORT_P4_OUT_P2) | (((val) & 0x1) << 2))
/* Port 4 Pin # Output Value (1) */
#define PORT_P4_OUT_P1   (0x1 << 1)
#define PORT_P4_OUT_P1_VAL(val)   (((val) & 0x1) << 1)
#define PORT_P4_OUT_P1_GET(val)   ((((val) & PORT_P4_OUT_P1) >> 1) & 0x1)
#define PORT_P4_OUT_P1_SET(reg,val) (reg) = ((reg & ~PORT_P4_OUT_P1) | (((val) & 0x1) << 1))
/* Port 4 Pin # Output Value (0) */
#define PORT_P4_OUT_P0   (0x1)
#define PORT_P4_OUT_P0_VAL(val)   (((val) & 0x1) << 0)
#define PORT_P4_OUT_P0_GET(val)   ((((val) & PORT_P4_OUT_P0) >> 0) & 0x1)
#define PORT_P4_OUT_P0_SET(reg,val) (reg) = ((reg & ~PORT_P4_OUT_P0) | (((val) & 0x1) << 0))

/*******************************************************************************
 * Port 4 Data Input Register
 ******************************************************************************/

/* Port 4 Pin # Latched Input Value (23) */
#define PORT_P4_IN_P23   (0x1 << 23)
#define PORT_P4_IN_P23_GET(val)   ((((val) & PORT_P4_IN_P23) >> 23) & 0x1)
/* Port 4 Pin # Latched Input Value (22) */
#define PORT_P4_IN_P22   (0x1 << 22)
#define PORT_P4_IN_P22_GET(val)   ((((val) & PORT_P4_IN_P22) >> 22) & 0x1)
/* Port 4 Pin # Latched Input Value (21) */
#define PORT_P4_IN_P21   (0x1 << 21)
#define PORT_P4_IN_P21_GET(val)   ((((val) & PORT_P4_IN_P21) >> 21) & 0x1)
/* Port 4 Pin # Latched Input Value (20) */
#define PORT_P4_IN_P20   (0x1 << 20)
#define PORT_P4_IN_P20_GET(val)   ((((val) & PORT_P4_IN_P20) >> 20) & 0x1)
/* Port 4 Pin # Latched Input Value (19) */
#define PORT_P4_IN_P19   (0x1 << 19)
#define PORT_P4_IN_P19_GET(val)   ((((val) & PORT_P4_IN_P19) >> 19) & 0x1)
/* Port 4 Pin # Latched Input Value (18) */
#define PORT_P4_IN_P18   (0x1 << 18)
#define PORT_P4_IN_P18_GET(val)   ((((val) & PORT_P4_IN_P18) >> 18) & 0x1)
/* Port 4 Pin # Latched Input Value (17) */
#define PORT_P4_IN_P17   (0x1 << 17)
#define PORT_P4_IN_P17_GET(val)   ((((val) & PORT_P4_IN_P17) >> 17) & 0x1)
/* Port 4 Pin # Latched Input Value (16) */
#define PORT_P4_IN_P16   (0x1 << 16)
#define PORT_P4_IN_P16_GET(val)   ((((val) & PORT_P4_IN_P16) >> 16) & 0x1)
/* Port 4 Pin # Latched Input Value (15) */
#define PORT_P4_IN_P15   (0x1 << 15)
#define PORT_P4_IN_P15_GET(val)   ((((val) & PORT_P4_IN_P15) >> 15) & 0x1)
/* Port 4 Pin # Latched Input Value (14) */
#define PORT_P4_IN_P14   (0x1 << 14)
#define PORT_P4_IN_P14_GET(val)   ((((val) & PORT_P4_IN_P14) >> 14) & 0x1)
/* Port 4 Pin # Latched Input Value (13) */
#define PORT_P4_IN_P13   (0x1 << 13)
#define PORT_P4_IN_P13_GET(val)   ((((val) & PORT_P4_IN_P13) >> 13) & 0x1)
/* Port 4 Pin # Latched Input Value (12) */
#define PORT_P4_IN_P12   (0x1 << 12)
#define PORT_P4_IN_P12_GET(val)   ((((val) & PORT_P4_IN_P12) >> 12) & 0x1)
/* Port 4 Pin # Latched Input Value (11) */
#define PORT_P4_IN_P11   (0x1 << 11)
#define PORT_P4_IN_P11_GET(val)   ((((val) & PORT_P4_IN_P11) >> 11) & 0x1)
/* Port 4 Pin # Latched Input Value (10) */
#define PORT_P4_IN_P10   (0x1 << 10)
#define PORT_P4_IN_P10_GET(val)   ((((val) & PORT_P4_IN_P10) >> 10) & 0x1)
/* Port 4 Pin # Latched Input Value (9) */
#define PORT_P4_IN_P9   (0x1 << 9)
#define PORT_P4_IN_P9_GET(val)   ((((val) & PORT_P4_IN_P9) >> 9) & 0x1)
/* Port 4 Pin # Latched Input Value (8) */
#define PORT_P4_IN_P8   (0x1 << 8)
#define PORT_P4_IN_P8_GET(val)   ((((val) & PORT_P4_IN_P8) >> 8) & 0x1)
/* Port 4 Pin # Latched Input Value (7) */
#define PORT_P4_IN_P7   (0x1 << 7)
#define PORT_P4_IN_P7_GET(val)   ((((val) & PORT_P4_IN_P7) >> 7) & 0x1)
/* Port 4 Pin # Latched Input Value (6) */
#define PORT_P4_IN_P6   (0x1 << 6)
#define PORT_P4_IN_P6_GET(val)   ((((val) & PORT_P4_IN_P6) >> 6) & 0x1)
/* Port 4 Pin # Latched Input Value (5) */
#define PORT_P4_IN_P5   (0x1 << 5)
#define PORT_P4_IN_P5_GET(val)   ((((val) & PORT_P4_IN_P5) >> 5) & 0x1)
/* Port 4 Pin # Latched Input Value (4) */
#define PORT_P4_IN_P4   (0x1 << 4)
#define PORT_P4_IN_P4_GET(val)   ((((val) & PORT_P4_IN_P4) >> 4) & 0x1)
/* Port 4 Pin # Latched Input Value (3) */
#define PORT_P4_IN_P3   (0x1 << 3)
#define PORT_P4_IN_P3_GET(val)   ((((val) & PORT_P4_IN_P3) >> 3) & 0x1)
/* Port 4 Pin # Latched Input Value (2) */
#define PORT_P4_IN_P2   (0x1 << 2)
#define PORT_P4_IN_P2_GET(val)   ((((val) & PORT_P4_IN_P2) >> 2) & 0x1)
/* Port 4 Pin # Latched Input Value (1) */
#define PORT_P4_IN_P1   (0x1 << 1)
#define PORT_P4_IN_P1_GET(val)   ((((val) & PORT_P4_IN_P1) >> 1) & 0x1)
/* Port 4 Pin # Latched Input Value (0) */
#define PORT_P4_IN_P0   (0x1)
#define PORT_P4_IN_P0_GET(val)   ((((val) & PORT_P4_IN_P0) >> 0) & 0x1)

/*******************************************************************************
 * Port 4 Direction Register
 ******************************************************************************/

/* Port 4 Pin #Direction Control (23) */
#define PORT_P4_DIR_P23   (0x1 << 23)
#define PORT_P4_DIR_P23_VAL(val)   (((val) & 0x1) << 23)
#define PORT_P4_DIR_P23_GET(val)   ((((val) & PORT_P4_DIR_P23) >> 23) & 0x1)
#define PORT_P4_DIR_P23_SET(reg,val) (reg) = ((reg & ~PORT_P4_DIR_P23) | (((val) & 0x1) << 23))
/* Port 4 Pin #Direction Control (22) */
#define PORT_P4_DIR_P22   (0x1 << 22)
#define PORT_P4_DIR_P22_VAL(val)   (((val) & 0x1) << 22)
#define PORT_P4_DIR_P22_GET(val)   ((((val) & PORT_P4_DIR_P22) >> 22) & 0x1)
#define PORT_P4_DIR_P22_SET(reg,val) (reg) = ((reg & ~PORT_P4_DIR_P22) | (((val) & 0x1) << 22))
/* Port 4 Pin #Direction Control (21) */
#define PORT_P4_DIR_P21   (0x1 << 21)
#define PORT_P4_DIR_P21_VAL(val)   (((val) & 0x1) << 21)
#define PORT_P4_DIR_P21_GET(val)   ((((val) & PORT_P4_DIR_P21) >> 21) & 0x1)
#define PORT_P4_DIR_P21_SET(reg,val) (reg) = ((reg & ~PORT_P4_DIR_P21) | (((val) & 0x1) << 21))
/* Port 4 Pin #Direction Control (20) */
#define PORT_P4_DIR_P20   (0x1 << 20)
#define PORT_P4_DIR_P20_VAL(val)   (((val) & 0x1) << 20)
#define PORT_P4_DIR_P20_GET(val)   ((((val) & PORT_P4_DIR_P20) >> 20) & 0x1)
#define PORT_P4_DIR_P20_SET(reg,val) (reg) = ((reg & ~PORT_P4_DIR_P20) | (((val) & 0x1) << 20))
/* Port 4 Pin #Direction Control (19) */
#define PORT_P4_DIR_P19   (0x1 << 19)
#define PORT_P4_DIR_P19_VAL(val)   (((val) & 0x1) << 19)
#define PORT_P4_DIR_P19_GET(val)   ((((val) & PORT_P4_DIR_P19) >> 19) & 0x1)
#define PORT_P4_DIR_P19_SET(reg,val) (reg) = ((reg & ~PORT_P4_DIR_P19) | (((val) & 0x1) << 19))
/* Port 4 Pin #Direction Control (18) */
#define PORT_P4_DIR_P18   (0x1 << 18)
#define PORT_P4_DIR_P18_VAL(val)   (((val) & 0x1) << 18)
#define PORT_P4_DIR_P18_GET(val)   ((((val) & PORT_P4_DIR_P18) >> 18) & 0x1)
#define PORT_P4_DIR_P18_SET(reg,val) (reg) = ((reg & ~PORT_P4_DIR_P18) | (((val) & 0x1) << 18))
/* Port 4 Pin #Direction Control (17) */
#define PORT_P4_DIR_P17   (0x1 << 17)
#define PORT_P4_DIR_P17_VAL(val)   (((val) & 0x1) << 17)
#define PORT_P4_DIR_P17_GET(val)   ((((val) & PORT_P4_DIR_P17) >> 17) & 0x1)
#define PORT_P4_DIR_P17_SET(reg,val) (reg) = ((reg & ~PORT_P4_DIR_P17) | (((val) & 0x1) << 17))
/* Port 4 Pin #Direction Control (16) */
#define PORT_P4_DIR_P16   (0x1 << 16)
#define PORT_P4_DIR_P16_VAL(val)   (((val) & 0x1) << 16)
#define PORT_P4_DIR_P16_GET(val)   ((((val) & PORT_P4_DIR_P16) >> 16) & 0x1)
#define PORT_P4_DIR_P16_SET(reg,val) (reg) = ((reg & ~PORT_P4_DIR_P16) | (((val) & 0x1) << 16))
/* Port 4 Pin #Direction Control (15) */
#define PORT_P4_DIR_P15   (0x1 << 15)
#define PORT_P4_DIR_P15_VAL(val)   (((val) & 0x1) << 15)
#define PORT_P4_DIR_P15_GET(val)   ((((val) & PORT_P4_DIR_P15) >> 15) & 0x1)
#define PORT_P4_DIR_P15_SET(reg,val) (reg) = ((reg & ~PORT_P4_DIR_P15) | (((val) & 0x1) << 15))
/* Port 4 Pin #Direction Control (14) */
#define PORT_P4_DIR_P14   (0x1 << 14)
#define PORT_P4_DIR_P14_VAL(val)   (((val) & 0x1) << 14)
#define PORT_P4_DIR_P14_GET(val)   ((((val) & PORT_P4_DIR_P14) >> 14) & 0x1)
#define PORT_P4_DIR_P14_SET(reg,val) (reg) = ((reg & ~PORT_P4_DIR_P14) | (((val) & 0x1) << 14))
/* Port 4 Pin #Direction Control (13) */
#define PORT_P4_DIR_P13   (0x1 << 13)
#define PORT_P4_DIR_P13_VAL(val)   (((val) & 0x1) << 13)
#define PORT_P4_DIR_P13_GET(val)   ((((val) & PORT_P4_DIR_P13) >> 13) & 0x1)
#define PORT_P4_DIR_P13_SET(reg,val) (reg) = ((reg & ~PORT_P4_DIR_P13) | (((val) & 0x1) << 13))
/* Port 4 Pin #Direction Control (12) */
#define PORT_P4_DIR_P12   (0x1 << 12)
#define PORT_P4_DIR_P12_VAL(val)   (((val) & 0x1) << 12)
#define PORT_P4_DIR_P12_GET(val)   ((((val) & PORT_P4_DIR_P12) >> 12) & 0x1)
#define PORT_P4_DIR_P12_SET(reg,val) (reg) = ((reg & ~PORT_P4_DIR_P12) | (((val) & 0x1) << 12))
/* Port 4 Pin #Direction Control (11) */
#define PORT_P4_DIR_P11   (0x1 << 11)
#define PORT_P4_DIR_P11_VAL(val)   (((val) & 0x1) << 11)
#define PORT_P4_DIR_P11_GET(val)   ((((val) & PORT_P4_DIR_P11) >> 11) & 0x1)
#define PORT_P4_DIR_P11_SET(reg,val) (reg) = ((reg & ~PORT_P4_DIR_P11) | (((val) & 0x1) << 11))
/* Port 4 Pin #Direction Control (10) */
#define PORT_P4_DIR_P10   (0x1 << 10)
#define PORT_P4_DIR_P10_VAL(val)   (((val) & 0x1) << 10)
#define PORT_P4_DIR_P10_GET(val)   ((((val) & PORT_P4_DIR_P10) >> 10) & 0x1)
#define PORT_P4_DIR_P10_SET(reg,val) (reg) = ((reg & ~PORT_P4_DIR_P10) | (((val) & 0x1) << 10))
/* Port 4 Pin #Direction Control (9) */
#define PORT_P4_DIR_P9   (0x1 << 9)
#define PORT_P4_DIR_P9_VAL(val)   (((val) & 0x1) << 9)
#define PORT_P4_DIR_P9_GET(val)   ((((val) & PORT_P4_DIR_P9) >> 9) & 0x1)
#define PORT_P4_DIR_P9_SET(reg,val) (reg) = ((reg & ~PORT_P4_DIR_P9) | (((val) & 0x1) << 9))
/* Port 4 Pin #Direction Control (8) */
#define PORT_P4_DIR_P8   (0x1 << 8)
#define PORT_P4_DIR_P8_VAL(val)   (((val) & 0x1) << 8)
#define PORT_P4_DIR_P8_GET(val)   ((((val) & PORT_P4_DIR_P8) >> 8) & 0x1)
#define PORT_P4_DIR_P8_SET(reg,val) (reg) = ((reg & ~PORT_P4_DIR_P8) | (((val) & 0x1) << 8))
/* Port 4 Pin #Direction Control (7) */
#define PORT_P4_DIR_P7   (0x1 << 7)
#define PORT_P4_DIR_P7_VAL(val)   (((val) & 0x1) << 7)
#define PORT_P4_DIR_P7_GET(val)   ((((val) & PORT_P4_DIR_P7) >> 7) & 0x1)
#define PORT_P4_DIR_P7_SET(reg,val) (reg) = ((reg & ~PORT_P4_DIR_P7) | (((val) & 0x1) << 7))
/* Port 4 Pin #Direction Control (6) */
#define PORT_P4_DIR_P6   (0x1 << 6)
#define PORT_P4_DIR_P6_VAL(val)   (((val) & 0x1) << 6)
#define PORT_P4_DIR_P6_GET(val)   ((((val) & PORT_P4_DIR_P6) >> 6) & 0x1)
#define PORT_P4_DIR_P6_SET(reg,val) (reg) = ((reg & ~PORT_P4_DIR_P6) | (((val) & 0x1) << 6))
/* Port 4 Pin #Direction Control (5) */
#define PORT_P4_DIR_P5   (0x1 << 5)
#define PORT_P4_DIR_P5_VAL(val)   (((val) & 0x1) << 5)
#define PORT_P4_DIR_P5_GET(val)   ((((val) & PORT_P4_DIR_P5) >> 5) & 0x1)
#define PORT_P4_DIR_P5_SET(reg,val) (reg) = ((reg & ~PORT_P4_DIR_P5) | (((val) & 0x1) << 5))
/* Port 4 Pin #Direction Control (4) */
#define PORT_P4_DIR_P4   (0x1 << 4)
#define PORT_P4_DIR_P4_VAL(val)   (((val) & 0x1) << 4)
#define PORT_P4_DIR_P4_GET(val)   ((((val) & PORT_P4_DIR_P4) >> 4) & 0x1)
#define PORT_P4_DIR_P4_SET(reg,val) (reg) = ((reg & ~PORT_P4_DIR_P4) | (((val) & 0x1) << 4))
/* Port 4 Pin #Direction Control (3) */
#define PORT_P4_DIR_P3   (0x1 << 3)
#define PORT_P4_DIR_P3_VAL(val)   (((val) & 0x1) << 3)
#define PORT_P4_DIR_P3_GET(val)   ((((val) & PORT_P4_DIR_P3) >> 3) & 0x1)
#define PORT_P4_DIR_P3_SET(reg,val) (reg) = ((reg & ~PORT_P4_DIR_P3) | (((val) & 0x1) << 3))
/* Port 4 Pin #Direction Control (2) */
#define PORT_P4_DIR_P2   (0x1 << 2)
#define PORT_P4_DIR_P2_VAL(val)   (((val) & 0x1) << 2)
#define PORT_P4_DIR_P2_GET(val)   ((((val) & PORT_P4_DIR_P2) >> 2) & 0x1)
#define PORT_P4_DIR_P2_SET(reg,val) (reg) = ((reg & ~PORT_P4_DIR_P2) | (((val) & 0x1) << 2))
/* Port 4 Pin #Direction Control (1) */
#define PORT_P4_DIR_P1   (0x1 << 1)
#define PORT_P4_DIR_P1_VAL(val)   (((val) & 0x1) << 1)
#define PORT_P4_DIR_P1_GET(val)   ((((val) & PORT_P4_DIR_P1) >> 1) & 0x1)
#define PORT_P4_DIR_P1_SET(reg,val) (reg) = ((reg & ~PORT_P4_DIR_P1) | (((val) & 0x1) << 1))
/* Port 4 Pin #Direction Control (0) */
#define PORT_P4_DIR_P0   (0x1)
#define PORT_P4_DIR_P0_VAL(val)   (((val) & 0x1) << 0)
#define PORT_P4_DIR_P0_GET(val)   ((((val) & PORT_P4_DIR_P0) >> 0) & 0x1)
#define PORT_P4_DIR_P0_SET(reg,val) (reg) = ((reg & ~PORT_P4_DIR_P0) | (((val) & 0x1) << 0))

/*******************************************************************************
 * Port 4 Alternate Function Select Register 0
 ******************************************************************************/

/* Alternate Function at Port 4 Bit # (23) */
#define PORT_P4_ALTSEL0_P23   (0x1 << 23)
#define PORT_P4_ALTSEL0_P23_VAL(val)   (((val) & 0x1) << 23)
#define PORT_P4_ALTSEL0_P23_GET(val)   ((((val) & PORT_P4_ALTSEL0_P23) >> 23) & 0x1)
#define PORT_P4_ALTSEL0_P23_SET(reg,val) (reg) = ((reg & ~PORT_P4_ALTSEL0_P23) | (((val) & 0x1) << 23))
/* Alternate Function at Port 4 Bit # (22) */
#define PORT_P4_ALTSEL0_P22   (0x1 << 22)
#define PORT_P4_ALTSEL0_P22_VAL(val)   (((val) & 0x1) << 22)
#define PORT_P4_ALTSEL0_P22_GET(val)   ((((val) & PORT_P4_ALTSEL0_P22) >> 22) & 0x1)
#define PORT_P4_ALTSEL0_P22_SET(reg,val) (reg) = ((reg & ~PORT_P4_ALTSEL0_P22) | (((val) & 0x1) << 22))
/* Alternate Function at Port 4 Bit # (21) */
#define PORT_P4_ALTSEL0_P21   (0x1 << 21)
#define PORT_P4_ALTSEL0_P21_VAL(val)   (((val) & 0x1) << 21)
#define PORT_P4_ALTSEL0_P21_GET(val)   ((((val) & PORT_P4_ALTSEL0_P21) >> 21) & 0x1)
#define PORT_P4_ALTSEL0_P21_SET(reg,val) (reg) = ((reg & ~PORT_P4_ALTSEL0_P21) | (((val) & 0x1) << 21))
/* Alternate Function at Port 4 Bit # (20) */
#define PORT_P4_ALTSEL0_P20   (0x1 << 20)
#define PORT_P4_ALTSEL0_P20_VAL(val)   (((val) & 0x1) << 20)
#define PORT_P4_ALTSEL0_P20_GET(val)   ((((val) & PORT_P4_ALTSEL0_P20) >> 20) & 0x1)
#define PORT_P4_ALTSEL0_P20_SET(reg,val) (reg) = ((reg & ~PORT_P4_ALTSEL0_P20) | (((val) & 0x1) << 20))
/* Alternate Function at Port 4 Bit # (19) */
#define PORT_P4_ALTSEL0_P19   (0x1 << 19)
#define PORT_P4_ALTSEL0_P19_VAL(val)   (((val) & 0x1) << 19)
#define PORT_P4_ALTSEL0_P19_GET(val)   ((((val) & PORT_P4_ALTSEL0_P19) >> 19) & 0x1)
#define PORT_P4_ALTSEL0_P19_SET(reg,val) (reg) = ((reg & ~PORT_P4_ALTSEL0_P19) | (((val) & 0x1) << 19))
/* Alternate Function at Port 4 Bit # (18) */
#define PORT_P4_ALTSEL0_P18   (0x1 << 18)
#define PORT_P4_ALTSEL0_P18_VAL(val)   (((val) & 0x1) << 18)
#define PORT_P4_ALTSEL0_P18_GET(val)   ((((val) & PORT_P4_ALTSEL0_P18) >> 18) & 0x1)
#define PORT_P4_ALTSEL0_P18_SET(reg,val) (reg) = ((reg & ~PORT_P4_ALTSEL0_P18) | (((val) & 0x1) << 18))
/* Alternate Function at Port 4 Bit # (17) */
#define PORT_P4_ALTSEL0_P17   (0x1 << 17)
#define PORT_P4_ALTSEL0_P17_VAL(val)   (((val) & 0x1) << 17)
#define PORT_P4_ALTSEL0_P17_GET(val)   ((((val) & PORT_P4_ALTSEL0_P17) >> 17) & 0x1)
#define PORT_P4_ALTSEL0_P17_SET(reg,val) (reg) = ((reg & ~PORT_P4_ALTSEL0_P17) | (((val) & 0x1) << 17))
/* Alternate Function at Port 4 Bit # (16) */
#define PORT_P4_ALTSEL0_P16   (0x1 << 16)
#define PORT_P4_ALTSEL0_P16_VAL(val)   (((val) & 0x1) << 16)
#define PORT_P4_ALTSEL0_P16_GET(val)   ((((val) & PORT_P4_ALTSEL0_P16) >> 16) & 0x1)
#define PORT_P4_ALTSEL0_P16_SET(reg,val) (reg) = ((reg & ~PORT_P4_ALTSEL0_P16) | (((val) & 0x1) << 16))
/* Alternate Function at Port 4 Bit # (15) */
#define PORT_P4_ALTSEL0_P15   (0x1 << 15)
#define PORT_P4_ALTSEL0_P15_VAL(val)   (((val) & 0x1) << 15)
#define PORT_P4_ALTSEL0_P15_GET(val)   ((((val) & PORT_P4_ALTSEL0_P15) >> 15) & 0x1)
#define PORT_P4_ALTSEL0_P15_SET(reg,val) (reg) = ((reg & ~PORT_P4_ALTSEL0_P15) | (((val) & 0x1) << 15))
/* Alternate Function at Port 4 Bit # (14) */
#define PORT_P4_ALTSEL0_P14   (0x1 << 14)
#define PORT_P4_ALTSEL0_P14_VAL(val)   (((val) & 0x1) << 14)
#define PORT_P4_ALTSEL0_P14_GET(val)   ((((val) & PORT_P4_ALTSEL0_P14) >> 14) & 0x1)
#define PORT_P4_ALTSEL0_P14_SET(reg,val) (reg) = ((reg & ~PORT_P4_ALTSEL0_P14) | (((val) & 0x1) << 14))
/* Alternate Function at Port 4 Bit # (13) */
#define PORT_P4_ALTSEL0_P13   (0x1 << 13)
#define PORT_P4_ALTSEL0_P13_VAL(val)   (((val) & 0x1) << 13)
#define PORT_P4_ALTSEL0_P13_GET(val)   ((((val) & PORT_P4_ALTSEL0_P13) >> 13) & 0x1)
#define PORT_P4_ALTSEL0_P13_SET(reg,val) (reg) = ((reg & ~PORT_P4_ALTSEL0_P13) | (((val) & 0x1) << 13))
/* Alternate Function at Port 4 Bit # (12) */
#define PORT_P4_ALTSEL0_P12   (0x1 << 12)
#define PORT_P4_ALTSEL0_P12_VAL(val)   (((val) & 0x1) << 12)
#define PORT_P4_ALTSEL0_P12_GET(val)   ((((val) & PORT_P4_ALTSEL0_P12) >> 12) & 0x1)
#define PORT_P4_ALTSEL0_P12_SET(reg,val) (reg) = ((reg & ~PORT_P4_ALTSEL0_P12) | (((val) & 0x1) << 12))
/* Alternate Function at Port 4 Bit # (11) */
#define PORT_P4_ALTSEL0_P11   (0x1 << 11)
#define PORT_P4_ALTSEL0_P11_VAL(val)   (((val) & 0x1) << 11)
#define PORT_P4_ALTSEL0_P11_GET(val)   ((((val) & PORT_P4_ALTSEL0_P11) >> 11) & 0x1)
#define PORT_P4_ALTSEL0_P11_SET(reg,val) (reg) = ((reg & ~PORT_P4_ALTSEL0_P11) | (((val) & 0x1) << 11))
/* Alternate Function at Port 4 Bit # (10) */
#define PORT_P4_ALTSEL0_P10   (0x1 << 10)
#define PORT_P4_ALTSEL0_P10_VAL(val)   (((val) & 0x1) << 10)
#define PORT_P4_ALTSEL0_P10_GET(val)   ((((val) & PORT_P4_ALTSEL0_P10) >> 10) & 0x1)
#define PORT_P4_ALTSEL0_P10_SET(reg,val) (reg) = ((reg & ~PORT_P4_ALTSEL0_P10) | (((val) & 0x1) << 10))
/* Alternate Function at Port 4 Bit # (9) */
#define PORT_P4_ALTSEL0_P9   (0x1 << 9)
#define PORT_P4_ALTSEL0_P9_VAL(val)   (((val) & 0x1) << 9)
#define PORT_P4_ALTSEL0_P9_GET(val)   ((((val) & PORT_P4_ALTSEL0_P9) >> 9) & 0x1)
#define PORT_P4_ALTSEL0_P9_SET(reg,val) (reg) = ((reg & ~PORT_P4_ALTSEL0_P9) | (((val) & 0x1) << 9))
/* Alternate Function at Port 4 Bit # (8) */
#define PORT_P4_ALTSEL0_P8   (0x1 << 8)
#define PORT_P4_ALTSEL0_P8_VAL(val)   (((val) & 0x1) << 8)
#define PORT_P4_ALTSEL0_P8_GET(val)   ((((val) & PORT_P4_ALTSEL0_P8) >> 8) & 0x1)
#define PORT_P4_ALTSEL0_P8_SET(reg,val) (reg) = ((reg & ~PORT_P4_ALTSEL0_P8) | (((val) & 0x1) << 8))
/* Alternate Function at Port 4 Bit # (7) */
#define PORT_P4_ALTSEL0_P7   (0x1 << 7)
#define PORT_P4_ALTSEL0_P7_VAL(val)   (((val) & 0x1) << 7)
#define PORT_P4_ALTSEL0_P7_GET(val)   ((((val) & PORT_P4_ALTSEL0_P7) >> 7) & 0x1)
#define PORT_P4_ALTSEL0_P7_SET(reg,val) (reg) = ((reg & ~PORT_P4_ALTSEL0_P7) | (((val) & 0x1) << 7))
/* Alternate Function at Port 4 Bit # (6) */
#define PORT_P4_ALTSEL0_P6   (0x1 << 6)
#define PORT_P4_ALTSEL0_P6_VAL(val)   (((val) & 0x1) << 6)
#define PORT_P4_ALTSEL0_P6_GET(val)   ((((val) & PORT_P4_ALTSEL0_P6) >> 6) & 0x1)
#define PORT_P4_ALTSEL0_P6_SET(reg,val) (reg) = ((reg & ~PORT_P4_ALTSEL0_P6) | (((val) & 0x1) << 6))
/* Alternate Function at Port 4 Bit # (5) */
#define PORT_P4_ALTSEL0_P5   (0x1 << 5)
#define PORT_P4_ALTSEL0_P5_VAL(val)   (((val) & 0x1) << 5)
#define PORT_P4_ALTSEL0_P5_GET(val)   ((((val) & PORT_P4_ALTSEL0_P5) >> 5) & 0x1)
#define PORT_P4_ALTSEL0_P5_SET(reg,val) (reg) = ((reg & ~PORT_P4_ALTSEL0_P5) | (((val) & 0x1) << 5))
/* Alternate Function at Port 4 Bit # (4) */
#define PORT_P4_ALTSEL0_P4   (0x1 << 4)
#define PORT_P4_ALTSEL0_P4_VAL(val)   (((val) & 0x1) << 4)
#define PORT_P4_ALTSEL0_P4_GET(val)   ((((val) & PORT_P4_ALTSEL0_P4) >> 4) & 0x1)
#define PORT_P4_ALTSEL0_P4_SET(reg,val) (reg) = ((reg & ~PORT_P4_ALTSEL0_P4) | (((val) & 0x1) << 4))
/* Alternate Function at Port 4 Bit # (3) */
#define PORT_P4_ALTSEL0_P3   (0x1 << 3)
#define PORT_P4_ALTSEL0_P3_VAL(val)   (((val) & 0x1) << 3)
#define PORT_P4_ALTSEL0_P3_GET(val)   ((((val) & PORT_P4_ALTSEL0_P3) >> 3) & 0x1)
#define PORT_P4_ALTSEL0_P3_SET(reg,val) (reg) = ((reg & ~PORT_P4_ALTSEL0_P3) | (((val) & 0x1) << 3))
/* Alternate Function at Port 4 Bit # (2) */
#define PORT_P4_ALTSEL0_P2   (0x1 << 2)
#define PORT_P4_ALTSEL0_P2_VAL(val)   (((val) & 0x1) << 2)
#define PORT_P4_ALTSEL0_P2_GET(val)   ((((val) & PORT_P4_ALTSEL0_P2) >> 2) & 0x1)
#define PORT_P4_ALTSEL0_P2_SET(reg,val) (reg) = ((reg & ~PORT_P4_ALTSEL0_P2) | (((val) & 0x1) << 2))
/* Alternate Function at Port 4 Bit # (1) */
#define PORT_P4_ALTSEL0_P1   (0x1 << 1)
#define PORT_P4_ALTSEL0_P1_VAL(val)   (((val) & 0x1) << 1)
#define PORT_P4_ALTSEL0_P1_GET(val)   ((((val) & PORT_P4_ALTSEL0_P1) >> 1) & 0x1)
#define PORT_P4_ALTSEL0_P1_SET(reg,val) (reg) = ((reg & ~PORT_P4_ALTSEL0_P1) | (((val) & 0x1) << 1))
/* Alternate Function at Port 4 Bit # (0) */
#define PORT_P4_ALTSEL0_P0   (0x1)
#define PORT_P4_ALTSEL0_P0_VAL(val)   (((val) & 0x1) << 0)
#define PORT_P4_ALTSEL0_P0_GET(val)   ((((val) & PORT_P4_ALTSEL0_P0) >> 0) & 0x1)
#define PORT_P4_ALTSEL0_P0_SET(reg,val) (reg) = ((reg & ~PORT_P4_ALTSEL0_P0) | (((val) & 0x1) << 0))

/*******************************************************************************
 * Port 4 Pull Up Device Enable Register
 ******************************************************************************/

/* Pull Up Device Enable at Port 4 Bit # (23) */
#define PORT_P4_PUEN_P23   (0x1 << 23)
#define PORT_P4_PUEN_P23_VAL(val)   (((val) & 0x1) << 23)
#define PORT_P4_PUEN_P23_GET(val)   ((((val) & PORT_P4_PUEN_P23) >> 23) & 0x1)
#define PORT_P4_PUEN_P23_SET(reg,val) (reg) = ((reg & ~PORT_P4_PUEN_P23) | (((val) & 0x1) << 23))
/* Pull Up Device Enable at Port 4 Bit # (22) */
#define PORT_P4_PUEN_P22   (0x1 << 22)
#define PORT_P4_PUEN_P22_VAL(val)   (((val) & 0x1) << 22)
#define PORT_P4_PUEN_P22_GET(val)   ((((val) & PORT_P4_PUEN_P22) >> 22) & 0x1)
#define PORT_P4_PUEN_P22_SET(reg,val) (reg) = ((reg & ~PORT_P4_PUEN_P22) | (((val) & 0x1) << 22))
/* Pull Up Device Enable at Port 4 Bit # (21) */
#define PORT_P4_PUEN_P21   (0x1 << 21)
#define PORT_P4_PUEN_P21_VAL(val)   (((val) & 0x1) << 21)
#define PORT_P4_PUEN_P21_GET(val)   ((((val) & PORT_P4_PUEN_P21) >> 21) & 0x1)
#define PORT_P4_PUEN_P21_SET(reg,val) (reg) = ((reg & ~PORT_P4_PUEN_P21) | (((val) & 0x1) << 21))
/* Pull Up Device Enable at Port 4 Bit # (20) */
#define PORT_P4_PUEN_P20   (0x1 << 20)
#define PORT_P4_PUEN_P20_VAL(val)   (((val) & 0x1) << 20)
#define PORT_P4_PUEN_P20_GET(val)   ((((val) & PORT_P4_PUEN_P20) >> 20) & 0x1)
#define PORT_P4_PUEN_P20_SET(reg,val) (reg) = ((reg & ~PORT_P4_PUEN_P20) | (((val) & 0x1) << 20))
/* Pull Up Device Enable at Port 4 Bit # (19) */
#define PORT_P4_PUEN_P19   (0x1 << 19)
#define PORT_P4_PUEN_P19_VAL(val)   (((val) & 0x1) << 19)
#define PORT_P4_PUEN_P19_GET(val)   ((((val) & PORT_P4_PUEN_P19) >> 19) & 0x1)
#define PORT_P4_PUEN_P19_SET(reg,val) (reg) = ((reg & ~PORT_P4_PUEN_P19) | (((val) & 0x1) << 19))
/* Pull Up Device Enable at Port 4 Bit # (18) */
#define PORT_P4_PUEN_P18   (0x1 << 18)
#define PORT_P4_PUEN_P18_VAL(val)   (((val) & 0x1) << 18)
#define PORT_P4_PUEN_P18_GET(val)   ((((val) & PORT_P4_PUEN_P18) >> 18) & 0x1)
#define PORT_P4_PUEN_P18_SET(reg,val) (reg) = ((reg & ~PORT_P4_PUEN_P18) | (((val) & 0x1) << 18))
/* Pull Up Device Enable at Port 4 Bit # (17) */
#define PORT_P4_PUEN_P17   (0x1 << 17)
#define PORT_P4_PUEN_P17_VAL(val)   (((val) & 0x1) << 17)
#define PORT_P4_PUEN_P17_GET(val)   ((((val) & PORT_P4_PUEN_P17) >> 17) & 0x1)
#define PORT_P4_PUEN_P17_SET(reg,val) (reg) = ((reg & ~PORT_P4_PUEN_P17) | (((val) & 0x1) << 17))
/* Pull Up Device Enable at Port 4 Bit # (16) */
#define PORT_P4_PUEN_P16   (0x1 << 16)
#define PORT_P4_PUEN_P16_VAL(val)   (((val) & 0x1) << 16)
#define PORT_P4_PUEN_P16_GET(val)   ((((val) & PORT_P4_PUEN_P16) >> 16) & 0x1)
#define PORT_P4_PUEN_P16_SET(reg,val) (reg) = ((reg & ~PORT_P4_PUEN_P16) | (((val) & 0x1) << 16))
/* Pull Up Device Enable at Port 4 Bit # (15) */
#define PORT_P4_PUEN_P15   (0x1 << 15)
#define PORT_P4_PUEN_P15_VAL(val)   (((val) & 0x1) << 15)
#define PORT_P4_PUEN_P15_GET(val)   ((((val) & PORT_P4_PUEN_P15) >> 15) & 0x1)
#define PORT_P4_PUEN_P15_SET(reg,val) (reg) = ((reg & ~PORT_P4_PUEN_P15) | (((val) & 0x1) << 15))
/* Pull Up Device Enable at Port 4 Bit # (14) */
#define PORT_P4_PUEN_P14   (0x1 << 14)
#define PORT_P4_PUEN_P14_VAL(val)   (((val) & 0x1) << 14)
#define PORT_P4_PUEN_P14_GET(val)   ((((val) & PORT_P4_PUEN_P14) >> 14) & 0x1)
#define PORT_P4_PUEN_P14_SET(reg,val) (reg) = ((reg & ~PORT_P4_PUEN_P14) | (((val) & 0x1) << 14))
/* Pull Up Device Enable at Port 4 Bit # (13) */
#define PORT_P4_PUEN_P13   (0x1 << 13)
#define PORT_P4_PUEN_P13_VAL(val)   (((val) & 0x1) << 13)
#define PORT_P4_PUEN_P13_GET(val)   ((((val) & PORT_P4_PUEN_P13) >> 13) & 0x1)
#define PORT_P4_PUEN_P13_SET(reg,val) (reg) = ((reg & ~PORT_P4_PUEN_P13) | (((val) & 0x1) << 13))
/* Pull Up Device Enable at Port 4 Bit # (12) */
#define PORT_P4_PUEN_P12   (0x1 << 12)
#define PORT_P4_PUEN_P12_VAL(val)   (((val) & 0x1) << 12)
#define PORT_P4_PUEN_P12_GET(val)   ((((val) & PORT_P4_PUEN_P12) >> 12) & 0x1)
#define PORT_P4_PUEN_P12_SET(reg,val) (reg) = ((reg & ~PORT_P4_PUEN_P12) | (((val) & 0x1) << 12))
/* Pull Up Device Enable at Port 4 Bit # (11) */
#define PORT_P4_PUEN_P11   (0x1 << 11)
#define PORT_P4_PUEN_P11_VAL(val)   (((val) & 0x1) << 11)
#define PORT_P4_PUEN_P11_GET(val)   ((((val) & PORT_P4_PUEN_P11) >> 11) & 0x1)
#define PORT_P4_PUEN_P11_SET(reg,val) (reg) = ((reg & ~PORT_P4_PUEN_P11) | (((val) & 0x1) << 11))
/* Pull Up Device Enable at Port 4 Bit # (10) */
#define PORT_P4_PUEN_P10   (0x1 << 10)
#define PORT_P4_PUEN_P10_VAL(val)   (((val) & 0x1) << 10)
#define PORT_P4_PUEN_P10_GET(val)   ((((val) & PORT_P4_PUEN_P10) >> 10) & 0x1)
#define PORT_P4_PUEN_P10_SET(reg,val) (reg) = ((reg & ~PORT_P4_PUEN_P10) | (((val) & 0x1) << 10))
/* Pull Up Device Enable at Port 4 Bit # (9) */
#define PORT_P4_PUEN_P9   (0x1 << 9)
#define PORT_P4_PUEN_P9_VAL(val)   (((val) & 0x1) << 9)
#define PORT_P4_PUEN_P9_GET(val)   ((((val) & PORT_P4_PUEN_P9) >> 9) & 0x1)
#define PORT_P4_PUEN_P9_SET(reg,val) (reg) = ((reg & ~PORT_P4_PUEN_P9) | (((val) & 0x1) << 9))
/* Pull Up Device Enable at Port 4 Bit # (8) */
#define PORT_P4_PUEN_P8   (0x1 << 8)
#define PORT_P4_PUEN_P8_VAL(val)   (((val) & 0x1) << 8)
#define PORT_P4_PUEN_P8_GET(val)   ((((val) & PORT_P4_PUEN_P8) >> 8) & 0x1)
#define PORT_P4_PUEN_P8_SET(reg,val) (reg) = ((reg & ~PORT_P4_PUEN_P8) | (((val) & 0x1) << 8))
/* Pull Up Device Enable at Port 4 Bit # (7) */
#define PORT_P4_PUEN_P7   (0x1 << 7)
#define PORT_P4_PUEN_P7_VAL(val)   (((val) & 0x1) << 7)
#define PORT_P4_PUEN_P7_GET(val)   ((((val) & PORT_P4_PUEN_P7) >> 7) & 0x1)
#define PORT_P4_PUEN_P7_SET(reg,val) (reg) = ((reg & ~PORT_P4_PUEN_P7) | (((val) & 0x1) << 7))
/* Pull Up Device Enable at Port 4 Bit # (6) */
#define PORT_P4_PUEN_P6   (0x1 << 6)
#define PORT_P4_PUEN_P6_VAL(val)   (((val) & 0x1) << 6)
#define PORT_P4_PUEN_P6_GET(val)   ((((val) & PORT_P4_PUEN_P6) >> 6) & 0x1)
#define PORT_P4_PUEN_P6_SET(reg,val) (reg) = ((reg & ~PORT_P4_PUEN_P6) | (((val) & 0x1) << 6))
/* Pull Up Device Enable at Port 4 Bit # (5) */
#define PORT_P4_PUEN_P5   (0x1 << 5)
#define PORT_P4_PUEN_P5_VAL(val)   (((val) & 0x1) << 5)
#define PORT_P4_PUEN_P5_GET(val)   ((((val) & PORT_P4_PUEN_P5) >> 5) & 0x1)
#define PORT_P4_PUEN_P5_SET(reg,val) (reg) = ((reg & ~PORT_P4_PUEN_P5) | (((val) & 0x1) << 5))
/* Pull Up Device Enable at Port 4 Bit # (4) */
#define PORT_P4_PUEN_P4   (0x1 << 4)
#define PORT_P4_PUEN_P4_VAL(val)   (((val) & 0x1) << 4)
#define PORT_P4_PUEN_P4_GET(val)   ((((val) & PORT_P4_PUEN_P4) >> 4) & 0x1)
#define PORT_P4_PUEN_P4_SET(reg,val) (reg) = ((reg & ~PORT_P4_PUEN_P4) | (((val) & 0x1) << 4))
/* Pull Up Device Enable at Port 4 Bit # (3) */
#define PORT_P4_PUEN_P3   (0x1 << 3)
#define PORT_P4_PUEN_P3_VAL(val)   (((val) & 0x1) << 3)
#define PORT_P4_PUEN_P3_GET(val)   ((((val) & PORT_P4_PUEN_P3) >> 3) & 0x1)
#define PORT_P4_PUEN_P3_SET(reg,val) (reg) = ((reg & ~PORT_P4_PUEN_P3) | (((val) & 0x1) << 3))
/* Pull Up Device Enable at Port 4 Bit # (2) */
#define PORT_P4_PUEN_P2   (0x1 << 2)
#define PORT_P4_PUEN_P2_VAL(val)   (((val) & 0x1) << 2)
#define PORT_P4_PUEN_P2_GET(val)   ((((val) & PORT_P4_PUEN_P2) >> 2) & 0x1)
#define PORT_P4_PUEN_P2_SET(reg,val) (reg) = ((reg & ~PORT_P4_PUEN_P2) | (((val) & 0x1) << 2))
/* Pull Up Device Enable at Port 4 Bit # (1) */
#define PORT_P4_PUEN_P1   (0x1 << 1)
#define PORT_P4_PUEN_P1_VAL(val)   (((val) & 0x1) << 1)
#define PORT_P4_PUEN_P1_GET(val)   ((((val) & PORT_P4_PUEN_P1) >> 1) & 0x1)
#define PORT_P4_PUEN_P1_SET(reg,val) (reg) = ((reg & ~PORT_P4_PUEN_P1) | (((val) & 0x1) << 1))
/* Pull Up Device Enable at Port 4 Bit # (0) */
#define PORT_P4_PUEN_P0   (0x1)
#define PORT_P4_PUEN_P0_VAL(val)   (((val) & 0x1) << 0)
#define PORT_P4_PUEN_P0_GET(val)   ((((val) & PORT_P4_PUEN_P0) >> 0) & 0x1)
#define PORT_P4_PUEN_P0_SET(reg,val) (reg) = ((reg & ~PORT_P4_PUEN_P0) | (((val) & 0x1) << 0))

#endif /* __PORT_H */
