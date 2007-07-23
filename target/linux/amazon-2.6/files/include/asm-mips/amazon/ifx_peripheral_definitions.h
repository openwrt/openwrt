//*************************************************************************
//* Summary of definitions which are used in each peripheral              *
//*************************************************************************

#ifndef peripheral_definitions_h
#define peripheral_definitions_h

typedef unsigned char		UINT8;
typedef signed char		INT8;
typedef unsigned short		UINT16;
typedef signed short		INT16;
typedef unsigned int		UINT32;
typedef signed int		INT32;
typedef unsigned long long	UINT64;
typedef signed long long	INT64;

#define REG8( addr )		  (*(volatile UINT8 *) (addr))
#define REG16( addr )		  (*(volatile UINT16 *)(addr))
#define REG32( addr )		  (*(volatile UINT32 *)(addr))
#define REG64( addr )		  (*(volatile UINT64 *)(addr))

/* define routine to set FPI access in Supervisor Mode */
#define IFX_SUPERVISOR_ON()                         REG32(FB0_CFG) = 0x01
/* Supervisor mode ends, following functions will be done in User mode */
#define IFX_SUPERVISOR_OFF()                        REG32(FB0_CFG) = 0x00
/* Supervisor mode ends, following functions will be done in User mode */
#define IFX_SUPERVISOR_MODE()                       REG32(FB0_CFG)
/* Supervisor mode ends, following functions will be done in User mode */
#define IFX_SUPERVISOR_SET(svm)                     REG32(FB0_CFG) = svm
/* enable all Interrupts in IIU */
//#define IFX_ENABLE_IRQ(irq_mask, im_base)           REG32(im_base | IIU_MASK) = irq_mask
///* get all high priority interrupt bits in IIU */
//#define IFX_GET_IRQ_MASKED(im_base)                 REG32(im_base | IIU_IRMASKED)
///* signal ends of interrupt to IIU */
//#define IFX_CLEAR_DIRECT_IRQ(irq_bit, im_base)      REG32(im_base | IIU_IR) = irq_bit
///* force IIU interrupt register */
//#define IFX_FORCE_IIU_REGISTER(data, im_base)       REG32(im_base | IIU_IRDEBUG) = data
///* get all bits of interrupt register */
//#define IFX_GET_IRQ_UNMASKED(im_base)               REG32(im_base | IIU_IR)
/* insert a NOP instruction */
#define NOP                                     _nop()
/* CPU goes to power down mode until interrupt occurs */
#define IFX_CPU_SLEEP                               _sleep()
/* enable all interrupts to CPU */
#define IFX_CPU_ENABLE_ALL_INTERRUPT                sys_enable_int()
/* get all low priority interrupt bits in peripheral */
#define IFX_GET_LOW_PRIO_IRQ(int_reg)               REG32(int_reg)
/* clear low priority interrupt bit in peripheral */
#define IFX_CLEAR_LOW_PRIO_IRQ(irq_bit, int_reg)    REG32(int_reg) = irq_bit
/* write FPI bus */
#define WRITE_FPI_BYTE(data, addr)              REG8(addr) = data
#define WRITE_FPI_16BIT(data, addr)             REG16(addr) = data
#define WRITE_FPI_32BIT(data, addr)             REG32(addr) = data
/* read FPI bus */
#define READ_FPI_BYTE(addr)                     REG8(addr)
#define READ_FPI_16BIT(addr)                    REG16(addr)
#define READ_FPI_32BIT(addr)                    REG32(addr)
/* write peripheral register */
#define WRITE_PERIPHERAL_REGISTER(data, addr)   REG32(addr) = data 

#ifdef	CONFIG_CPU_LITTLE_ENDIAN
#define WRITE_PERIPHERAL_REGISTER_16(data, addr) REG16(addr) = data
#define WRITE_PERIPHERAL_REGISTER_8(data, addr) REG8(addr) = data
#else //not CONFIG_CPU_LITTLE_ENDIAN
#define WRITE_PERIPHERAL_REGISTER_16(data, addr) REG16(addr+2) = data
#define WRITE_PERIPHERAL_REGISTER_8(data, addr) REG8(addr+3) = data
#endif //CONFIG_CPU_LITTLE_ENDIAN

/* read peripheral register */
#define READ_PERIPHERAL_REGISTER(addr)          REG32(addr)

/* read/modify(or)/write peripheral register */
#define RMW_OR_PERIPHERAL_REGISTER(data, addr)	REG32(addr) = REG32(addr) | data
/* read/modify(and)/write peripheral register */
#define RMW_AND_PERIPHERAL_REGISTER(data, addr)	REG32(addr) = REG32(addr) & (UINT32)data

/* CPU-independent mnemonic constants */
/* CLC register bits */
#define IFX_CLC_ENABLE                0x00000000
#define IFX_CLC_DISABLE               0x00000001
#define IFX_CLC_DISABLE_STATUS        0x00000002
#define IFX_CLC_SUSPEND_ENABLE        0x00000004
#define IFX_CLC_CLOCK_OFF_DISABLE     0x00000008
#define IFX_CLC_OVERWRITE_SPEN_FSOE   0x00000010
#define IFX_CLC_FAST_CLOCK_SWITCH_OFF 0x00000020
#define IFX_CLC_RUN_DIVIDER_MASK      0x0000FF00
#define IFX_CLC_RUN_DIVIDER_OFFSET    8
#define IFX_CLC_SLEEP_DIVIDER_MASK    0x00FF0000
#define IFX_CLC_SLEEP_DIVIDER_OFFSET  16
#define IFX_CLC_SPECIFIC_DIVIDER_MASK 0x00FF0000
#define IFX_CLC_SPECIFIC_DIVIDER_OFFSET 24

/* number of cycles to wait for interrupt service routine to be called */
#define WAIT_CYCLES   50

#endif /* PERIPHERAL_DEFINITIONS_H not yet defined */
