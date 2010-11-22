/*
 * m5485pci.h -- ColdFire 547x/548x PCI controller support.
 * Copyright 2007-2009 Freescale Semiconductor, Inc. All Rights Reserved.
 */
#ifndef __MCF548X_PCI_H__
#define __MCF548X_PCI_H__


/* PCI Type 0 Configuration Registers */
#define MCF_PCIIDR        MCF_REG32(0x000B00)
/* PCI Device ID/Vendor ID      */
#define MCF_PCISCR        MCF_REG32(0x000B04)
/* PCI Status/Command           */
#define MCF_PCICCRIR      MCF_REG32(0x000B08)
/* PCI Class Code / Revision ID */
#define MCF_PCICR1        MCF_REG32(0x000B0C)
/* PCI Configuration 1 Register */
#define MCF_PCIBAR0       MCF_REG32(0x000B10)
/* PCI Base Address Register 0  */
#define MCF_PCIBAR1       MCF_REG32(0x000B14)
/* PCI Base Address Register 1  */
#define MCF_PCICCPR       MCF_REG32(0x000B28)
/* PCI Cardbus CIS Pointer      */
#define MCF_PCISID        MCF_REG32(0x000B2C)
/* Subsystem ID/Subsystem Vendor ID*/
#define MCF_PCIERBAR      MCF_REG32(0x000B30)
/* PCI Expansion ROM            */
#define MCF_PCICPR        MCF_REG32(0x000B30)
/* PCI Capabilities Pointer     */
#define MCF_PCICR2        MCF_REG32(0x000B3C)
/* PCI Configuration Register 2 */

/* General Control/Status Registers */
#define MCF_PCIGSCR       MCF_REG32(0x000B60)
/* Global Status/Control Register   */
#define MCF_PCITBATR0     MCF_REG32(0x000B64)
/* Target Base Address Translation 0*/
#define MCF_PCITBATR1     MCF_REG32(0x000B68)
/* Target Base Address Translation 1*/
#define MCF_PCITCR        MCF_REG32(0x000B6C)
/* Target Control Register          */
#define MCF_PCIIW0BTAR    MCF_REG32(0x000B70)
/* Initiator Window 0 Base Address  */
#define MCF_PCIIW1BTAR    MCF_REG32(0x000B74)
/* Initiator Window 1 Base Address  */
#define MCF_PCIIW2BTAR    MCF_REG32(0x000B78)
/* Initiator Window 2 Base  Address */
#define MCF_PCIIWCR       MCF_REG32(0x000B80)
/* Initiator Window Configuration   */
#define MCF_PCIICR        MCF_REG32(0x000B84)
/* Initiator Control Register       */
#define MCF_PCIISR        MCF_REG32(0x000B88)
/* Initiator Status Register        */
#define MCF_PCICAR        MCF_REG32(0x000BF8)
/* Configuration Address Register   */

/* CommBus FIFO Transmit Interface Registers */
#define MCF_PCITPSR       MCF_REG32(0x008400)
/* Tx Packet Size Register      	*/
#define MCF_PCITSAR       MCF_REG32(0x008404)
/* Tx Start Address Register        */
#define MCF_PCITTCR       MCF_REG32(0x008408)
/* Tx Transaction Control Register  */
#define MCF_PCITER        MCF_REG32(0x00840C)
/* Tx Enables Register          	*/
#define MCF_PCITNAR       MCF_REG32(0x008410)
/* Tx Next Address Register         */
#define MCF_PCITLWR       MCF_REG32(0x008414)
/* Tx Last Word Register        	*/
#define MCF_PCITDCR       MCF_REG32(0x008418)
/* Tx Done Counts Register          */
#define MCF_PCITSR        MCF_REG32(0x00841C)
/* Tx Status Register           	*/
#define MCF_PCITFDR       MCF_REG32(0x008440)
/* Tx FIFO Data Register        	*/
#define MCF_PCITFSR       MCF_REG32(0x008444)
/* Tx FIFO Status Register          */
#define MCF_PCITFCR       MCF_REG32(0x008448)
/* Tx FIFO Control Register         */
#define MCF_PCITFAR       MCF_REG32(0x00844C)
/* Tx FIFO Alarm Register       	*/
#define MCF_PCITFRPR      MCF_REG32(0x008450)
/* Tx FIFO Read Pointer Register    */
#define MCF_PCITFWPR      MCF_REG32(0x008454)
/* Tx FIFO Write Pointer Register   */

/* CommBus FIFO Receive Interface Registers */
#define MCF_PCIRPSR       MCF_REG32(0x008480)
/* Tx Packet Size Register      	*/
#define MCF_PCIRSAR       MCF_REG32(0x008484)
/* Tx Start Address Register        */
#define MCF_PCIRTCR       MCF_REG32(0x008488)
/* Tx Transaction Control Register  */
#define MCF_PCIRER        MCF_REG32(0x00848C)
/* Tx Enables Register          	*/
#define MCF_PCIRNAR       MCF_REG32(0x008490)
/* Tx Next Address Register         */
#define MCF_PCIRDCR       MCF_REG32(0x008498)
/* Tx Done Counts Register          */
#define MCF_PCIRSR        MCF_REG32(0x00849C)
/* Tx Status Register           	*/
#define MCF_PCIRFDR       MCF_REG32(0x0084C0)
/* Tx FIFO Data Register        	*/
#define MCF_PCIRFSR       MCF_REG32(0x0084C4)
/* Tx FIFO Status Register          */
#define MCF_PCIRFCR       MCF_REG32(0x0084C8)
/* Tx FIFO Control Register         */
#define MCF_PCIRFAR       MCF_REG32(0x0084CC)
/* Tx FIFO Alarm Register       	*/
#define MCF_PCIRFRPR      MCF_REG32(0x0084D0)
/* Tx FIFO Read Pointer Register    */
#define MCF_PCIRFWPR      MCF_REG32(0x0084D4)
/* Tx FIFO Write Pointer Register   */

/* PCI Arbiter Registers */
#define MCF_PCIARB_PACR   MCF_REG32(0x000C00)
#define MCF_PCIARB_PASR   MCF_REG32(0x000C04)


/* Bit definitions and macros for MCF_PCIIDR */
#define MCF_PCIIDR_VENDORID(x)            (((x)&0x0000FFFF)<<0)
#define MCF_PCIIDR_DEVICEID(x)            (((x)&0x0000FFFF)<<16)

/* Bit definitions and macros for MCF_PCISCR */
#define MCF_PCISCR_M      (0x00000002)
#define MCF_PCISCR_B      (0x00000004)
#define MCF_PCISCR_SP     (0x00000008)
#define MCF_PCISCR_MW     (0x00000010)
#define MCF_PCISCR_PER    (0x00000040)
#define MCF_PCISCR_S      (0x00000100)
#define MCF_PCISCR_F      (0x00000200)
#define MCF_PCISCR_C      (0x00100000)
#define MCF_PCISCR_66M    (0x00200000)
#define MCF_PCISCR_R      (0x00400000)
#define MCF_PCISCR_FC     (0x00800000)
#define MCF_PCISCR_DP     (0x01000000)
#define MCF_PCISCR_DT(x)  (((x)&0x00000003)<<25)
#define MCF_PCISCR_TS     (0x08000000)
#define MCF_PCISCR_TR     (0x10000000)
#define MCF_PCISCR_MA     (0x20000000)
#define MCF_PCISCR_SE     (0x40000000)
#define MCF_PCISCR_PE     (0x80000000)

/* Bit definitions and macros for MCF_PCICCRIR */
#define MCF_PCICCRIR_REVID(x)             (((x)&0x000000FF)<<0)
#define MCF_PCICCRIR_CLASSCODE(x)         (((x)&0x00FFFFFF)<<8)

/* Bit definitions and macros for MCF_PCICR1 */
#define MCF_PCICR1_CACHELINESIZE(x)       (((x)&0x0000000F)<<0)
#define MCF_PCICR1_LATTIMER(x)            (((x)&0x000000FF)<<8)
#define MCF_PCICR1_HEADERTYPE(x)          (((x)&0x000000FF)<<16)
#define MCF_PCICR1_BIST(x)                (((x)&0x000000FF)<<24)

/* Bit definitions and macros for MCF_PCIBAR# */
#define MCF_PCIBAR0_ADDR(x)               (((x)&0x00003FFF)<<18)
#define MCF_PCIBAR1_ADDR(x)               (((x)&0x00000003)<<30)

/* Bit definitions and macros for MCF_PCICR2 */
#define MCF_PCICR2_INTLINE(x)             (((x)&0x000000FF)<<0)
#define MCF_PCICR2_INTPIN(x)              (((x)&0x000000FF)<<8)
#define MCF_PCICR2_MINGNT(x)              (((x)&0x000000FF)<<16)
#define MCF_PCICR2_MAXLAT(x)              (((x)&0x000000FF)<<24)

/* Bit definitions and macros for MCF_PCIGSCR */
#define MCF_PCIGSCR_PR                    (0x00000001)
#define MCF_PCIGSCR_SEE                   (0x00001000)
#define MCF_PCIGSCR_PEE                   (0x00002000)
#define MCF_PCIGSCR_SE                    (0x10000000)
#define MCF_PCIGSCR_PE                    (0x20000000)

/* Bit definitions and macros for MCF_PCITBATR0 */
#define MCF_PCITBATR0_EN                  (0x00000001)
#define MCF_PCITBATR0_BAT0(x)             (((x)&0x00003FFF)<<18)

/* Bit definitions and macros for MCF_PCITBATR1 */
#define MCF_PCITBATR1_EN                  (0x00000001)
#define MCF_PCITBATR1_BAT1(x)             (((x)&0x00000003)<<30)

/* Bit definitions and macros for MCF_PCITCR */
#define MCF_PCITCR_P                      (0x00010000)
#define MCF_PCITCR_LD                     (0x01000000)

/* Bit definitions and macros for MCF_PCIIW0BTAR */
#define MCF_PCIIW0BTAR_WTA0(x)            (((x)&0x000000FF)<<8)
#define MCF_PCIIW0BTAR_WAM0(x)            (((x)&0x000000FF)<<16)
#define MCF_PCIIW0BTAR_WBA0(x)            (((x)&0x000000FF)<<24)

/* Bit definitions and macros for MCF_PCIIW1BTAR */
#define MCF_PCIIW1BTAR_WTA1(x)            (((x)&0x000000FF)<<8)
#define MCF_PCIIW1BTAR_WAM1(x)            (((x)&0x000000FF)<<16)
#define MCF_PCIIW1BTAR_WBA1(x)            (((x)&0x000000FF)<<24)

/* Bit definitions and macros for MCF_PCIIW2BTAR */
#define MCF_PCIIW2BTAR_WTA2(x)            (((x)&0x000000FF)<<8)
#define MCF_PCIIW2BTAR_WAM2(x)            (((x)&0x000000FF)<<16)
#define MCF_PCIIW2BTAR_WBA2(x)            (((x)&0x000000FF)<<24)

/* Bit definitions and macros for MCF_PCIIWCR */
#define MCF_PCIIWCR_WINCTRL2(x)           (((x)&0x0000000F)<<8)
#define MCF_PCIIWCR_WINCTRL1(x)           (((x)&0x0000000F)<<16)
#define MCF_PCIIWCR_WINCTRL0(x)           (((x)&0x0000000F)<<24)
#define MCF_PCIIWCR_WINCTRL0_MEMREAD      (0x01000000)
#define MCF_PCIIWCR_WINCTRL0_MEMRDLINE    (0x03000000)
#define MCF_PCIIWCR_WINCTRL0_MEMRDMUL     (0x05000000)
#define MCF_PCIIWCR_WINCTRL0_IO           (0x09000000)
#define MCF_PCIIWCR_WINCTRL0_E            (0x01000000)
#define MCF_PCIIWCR_WINCTRL1_MEMREAD      (0x00010000)
#define MCF_PCIIWCR_WINCTRL1_MEMRDLINE    (0x00030000)
#define MCF_PCIIWCR_WINCTRL1_MEMRDMUL     (0x00050000)
#define MCF_PCIIWCR_WINCTRL1_IO           (0x00090000)
#define MCF_PCIIWCR_WINCTRL1_E            (0x00010000)
#define MCF_PCIIWCR_WINCTRL2_MEMREAD      (0x00000100)
#define MCF_PCIIWCR_WINCTRL2_MEMRDLINE    (0x00000300)
#define MCF_PCIIWCR_WINCTRL2_MEMRDMUL     (0x00000500)
#define MCF_PCIIWCR_WINCTRL2_IO           (0x00000900)
#define MCF_PCIIWCR_WINCTRL2_E            (0x00000100)


/* Bit definitions and macros for MCF_PCIICR */
#define MCF_PCIICR_MAXRETRY(x)            (((x)&0x000000FF)<<0)
#define MCF_PCIICR_TAE                    (0x01000000)
#define MCF_PCIICR_IAE                    (0x02000000)
#define MCF_PCIICR_REE                    (0x04000000)

/* Bit definitions and macros for MCF_PCIISR */
#define MCF_PCIISR_TA                     (0x01000000)
#define MCF_PCIISR_IA                     (0x02000000)
#define MCF_PCIISR_RE                     (0x04000000)

/* Bit definitions and macros for MCF_PCICAR */
#define MCF_PCICAR_DWORD(x)               (((x)&0x0000003F)<<2)
#define MCF_PCICAR_FUNCNUM(x)             (((x)&0x00000007)<<8)
#define MCF_PCICAR_DEVNUM(x)              (((x)&0x0000001F)<<11)
#define MCF_PCICAR_BUSNUM(x)              (((x)&0x000000FF)<<16)
#define MCF_PCICAR_E                      (0x80000000)

/* Bit definitions and macros for MCF_PCITPSR */
#define MCF_PCITPSR_PKTSIZE(x)            (((x)&0x0000FFFF)<<16)

/* Bit definitions and macros for MCF_PCITTCR */
#define MCF_PCITTCR_DI                    (0x00000001)
#define MCF_PCITTCR_W                     (0x00000010)
#define MCF_PCITTCR_MAXBEATS(x)           (((x)&0x00000007)<<8)
#define MCF_PCITTCR_MAXRETRY(x)           (((x)&0x000000FF)<<16)
#define MCF_PCITTCR_PCICMD(x)             (((x)&0x0000000F)<<24)

/* Bit definitions and macros for MCF_PCITER */
#define MCF_PCITER_NE                     (0x00010000)
#define MCF_PCITER_IAE                    (0x00020000)
#define MCF_PCITER_TAE                    (0x00040000)
#define MCF_PCITER_RE                     (0x00080000)
#define MCF_PCITER_SE                     (0x00100000)
#define MCF_PCITER_FEE                    (0x00200000)
#define MCF_PCITER_ME                     (0x01000000)
#define MCF_PCITER_BE                     (0x08000000)
#define MCF_PCITER_CM                     (0x10000000)
#define MCF_PCITER_RF                     (0x40000000)
#define MCF_PCITER_RC                     (0x80000000)

/* Bit definitions and macros for MCF_PCITDCR */
#define MCF_PCITDCR_PKTSDONE(x)           (((x)&0x0000FFFF)<<0)
#define MCF_PCITDCR_BYTESDONE(x)          (((x)&0x0000FFFF)<<16)

/* Bit definitions and macros for MCF_PCITSR */
#define MCF_PCITSR_IA                     (0x00010000)
#define MCF_PCITSR_TA                     (0x00020000)
#define MCF_PCITSR_RE                     (0x00040000)
#define MCF_PCITSR_SE                     (0x00080000)
#define MCF_PCITSR_FE                     (0x00100000)
#define MCF_PCITSR_BE1                    (0x00200000)
#define MCF_PCITSR_BE2                    (0x00400000)
#define MCF_PCITSR_BE3                    (0x00800000)
#define MCF_PCITSR_NT                     (0x01000000)

/* Bit definitions and macros for MCF_PCITFSR */
#define MCF_PCITFSR_EMT                   (0x00010000)
#define MCF_PCITFSR_ALARM                 (0x00020000)
#define MCF_PCITFSR_FU                    (0x00040000)
#define MCF_PCITFSR_FR                    (0x00080000)
#define MCF_PCITFSR_OF                    (0x00100000)
#define MCF_PCITFSR_UF                    (0x00200000)
#define MCF_PCITFSR_RXW                   (0x00400000)

/* Bit definitions and macros for MCF_PCITFCR */
#define MCF_PCITFCR_OF_MSK                (0x00080000)
#define MCF_PCITFCR_UF_MSK                (0x00100000)
#define MCF_PCITFCR_RXW_MSK               (0x00200000)
#define MCF_PCITFCR_FAE_MSK               (0x00400000)
#define MCF_PCITFCR_IP_MSK                (0x00800000)
#define MCF_PCITFCR_GR(x)                 (((x)&0x00000007)<<24)

/* Bit definitions and macros for MCF_PCITFAR */
#define MCF_PCITFAR_ALARM(x)              (((x)&0x0000007F)<<0)

/* Bit definitions and macros for MCF_PCITFRPR */
#define MCF_PCITFRPR_READ(x)              (((x)&0x00000FFF)<<0)

/* Bit definitions and macros for MCF_PCITFWPR */
#define MCF_PCITFWPR_WRITE(x)             (((x)&0x00000FFF)<<0)

/* Bit definitions and macros for MCF_PCIRPSR */
#define MCF_PCIRPSR_PKTSIZE(x)            (((x)&0x0000FFFF)<<16)

/* Bit definitions and macros for MCF_PCIRTCR */
#define MCF_PCIRTCR_DI                    (0x00000001)
#define MCF_PCIRTCR_W                     (0x00000010)
#define MCF_PCIRTCR_MAXBEATS(x)           (((x)&0x00000007)<<8)
#define MCF_PCIRTCR_FB                    (0x00001000)
#define MCF_PCIRTCR_MAXRETRY(x)           (((x)&0x000000FF)<<16)
#define MCF_PCIRTCR_PCICMD(x)             (((x)&0x0000000F)<<24)

/* Bit definitions and macros for MCF_PCIRER */
#define MCF_PCIRER_NE                     (0x00010000)
#define MCF_PCIRER_IAE                    (0x00020000)
#define MCF_PCIRER_TAE                    (0x00040000)
#define MCF_PCIRER_RE                     (0x00080000)
#define MCF_PCIRER_SE                     (0x00100000)
#define MCF_PCIRER_FEE                    (0x00200000)
#define MCF_PCIRER_ME                     (0x01000000)
#define MCF_PCIRER_BE                     (0x08000000)
#define MCF_PCIRER_CM                     (0x10000000)
#define MCF_PCIRER_FE                     (0x20000000)
#define MCF_PCIRER_RF                     (0x40000000)
#define MCF_PCIRER_RC                     (0x80000000)

/* Bit definitions and macros for MCF_PCIRDCR */
#define MCF_PCIRDCR_PKTSDONE(x)           (((x)&0x0000FFFF)<<0)
#define MCF_PCIRDCR_BYTESDONE(x)          (((x)&0x0000FFFF)<<16)

/* Bit definitions and macros for MCF_PCIRSR */
#define MCF_PCIRSR_IA                     (0x00010000)
#define MCF_PCIRSR_TA                     (0x00020000)
#define MCF_PCIRSR_RE                     (0x00040000)
#define MCF_PCIRSR_SE                     (0x00080000)
#define MCF_PCIRSR_FE                     (0x00100000)
#define MCF_PCIRSR_BE1                    (0x00200000)
#define MCF_PCIRSR_BE2                    (0x00400000)
#define MCF_PCIRSR_BE3                    (0x00800000)
#define MCF_PCIRSR_NT                     (0x01000000)

/* Bit definitions and macros for MCF_PCIRFSR */
#define MCF_PCIRFSR_EMT                   (0x00010000)
#define MCF_PCIRFSR_ALARM                 (0x00020000)
#define MCF_PCIRFSR_FU                    (0x00040000)
#define MCF_PCIRFSR_FR                    (0x00080000)
#define MCF_PCIRFSR_OF                    (0x00100000)
#define MCF_PCIRFSR_UF                    (0x00200000)
#define MCF_PCIRFSR_RXW                   (0x00400000)

/* Bit definitions and macros for MCF_PCIRFCR */
#define MCF_PCIRFCR_OF_MSK                (0x00080000)
#define MCF_PCIRFCR_UF_MSK                (0x00100000)
#define MCF_PCIRFCR_RXW_MSK               (0x00200000)
#define MCF_PCIRFCR_FAE_MSK               (0x00400000)
#define MCF_PCIRFCR_IP_MSK                (0x00800000)
#define MCF_PCIRFCR_GR(x)                 (((x)&0x00000007)<<24)

/* Bit definitions and macros for MCF_PCIRFAR */
#define MCF_PCIRFAR_ALARM(x)              (((x)&0x0000007F)<<0)

/* Bit definitions and macros for MCF_PCIRFRPR */
#define MCF_PCIRFRPR_READ(x)              (((x)&0x00000FFF)<<0)

/* Bit definitions and macros for MCF_PCIRFWPR */
#define MCF_PCIRFWPR_WRITE(x)             (((x)&0x00000FFF)<<0)


/* Bit definitions and macros for MCF_PCIARB_PACR */
#define MCF_PCIARB_PACR_INTMPRI         (0x00000001)
#define MCF_PCIARB_PACR_EXTMPRI(x)      (((x)&0x0000001F)<<1)
#define MCF_PCIARB_PACR_INTMINTEN       (0x00010000)
#define MCF_PCIARB_PACR_EXTMINTEN(x)    (((x)&0x0000001F)<<17)
#define MCF_PCIARB_PACR_PKMD            (0x40000000)
#define MCF_PCIARB_PACR_DS              (0x80000000)

/* Bit definitions and macros for MCF_PCIARB_PASR */
#define MCF_PCIARB_PASR_ITLMBK          (0x00010000)
#define MCF_PCIARB_PASR_EXTMBK(x)       (((x)&0x0000001F)<<17)

#endif /* __MCF548X_PCI_H__ */
