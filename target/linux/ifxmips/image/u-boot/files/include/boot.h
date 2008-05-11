#ifndef _BOOT_H
#define _BOOT_H

/* All this should be defined somewhere in danube.h later... */

#define MPS_SRAM_BASE_ADDRESS   0xBF200000
#define MPS_SRAM_BOOT_OFFSET    0x1C0

/* Offset for CPU1 (both CPUs have same register set) */
#define BOOT_BASE_ADDRESS   (MPS_SRAM_BASE_ADDRESS + MPS_SRAM_BOOT_OFFSET)
#define BOOT_CPU_OFFSET     0x20


#ifdef __ASSEMBLY__
#define BOOT_RVEC		      (BOOT_BASE_ADDRESS + 0x00)
#define BOOT_NVEC		      (BOOT_BASE_ADDRESS + 0x04)
#define BOOT_EVEC		      (BOOT_BASE_ADDRESS + 0x08)
#define BOOT_CP0_CAUSE     (BOOT_BASE_ADDRESS + 0x0C)
#define BOOT_CP0_EPC	      (BOOT_BASE_ADDRESS + 0x10)
#define BOOT_CP0_EEPC	   (BOOT_BASE_ADDRESS + 0x14)
#define BOOT_SIZE        (BOOT_BASE_ADDRESS + 0x18)   /* for CPU1 */
#define BOOT_RCU_SR        (BOOT_BASE_ADDRESS + 0x18) /* for CPU0 */
#define BOOT_CFG_STAT	   (BOOT_BASE_ADDRESS + 0x1C)
#else
#define BOOT_RVEC(cpu)		(volatile u32*)(BOOT_BASE_ADDRESS + (cpu * BOOT_CPU_OFFSET) + 0x00)
#define BOOT_NVEC(cpu)		(volatile u32*)(BOOT_BASE_ADDRESS + (cpu * BOOT_CPU_OFFSET) + 0x04)
#define BOOT_EVEC(cpu)		(volatile u32*)(BOOT_BASE_ADDRESS + (cpu * BOOT_CPU_OFFSET) + 0x08)
#define BOOT_CP0_STATUS(cpu)	(volatile u32*)(BOOT_BASE_ADDRESS + (cpu * BOOT_CPU_OFFSET) + 0x0C)
#define BOOT_CP0_EPC(cpu)	(volatile u32*)(BOOT_BASE_ADDRESS + (cpu * BOOT_CPU_OFFSET) + 0x10)
#define BOOT_CP0_EEPC(cpu)	(volatile u32*)(BOOT_BASE_ADDRESS + (cpu * BOOT_CPU_OFFSET) + 0x14)
#define BOOT_SIZE(cpu)       (volatile u32*)(BOOT_BASE_ADDRESS + (cpu * BOOT_CPU_OFFSET) + 0x18)    /* for CPU1 */
#define BOOT_RCU_SR(cpu)       (volatile u32*)(BOOT_BASE_ADDRESS + (cpu * BOOT_CPU_OFFSET) + 0x18)  /* for CPU0 */
#define BOOT_CFG_STAT(cpu)	(volatile u32*)(BOOT_BASE_ADDRESS + (cpu * BOOT_CPU_OFFSET) + 0x1C)
#endif

#define BOOT_CFG_NOR   		0x01
#define BOOT_CFG_MII   		0x02
#define BOOT_CFG_PCI  		0x03
#define BOOT_CFG_ASC   		0x04
#define BOOT_CFG_SFLASH		0x05
#define BOOT_CFG_NAND  		0x06
#define BOOT_CFG_RMII   	0x07
#define BOOT_CFG_TEST   	0x00

#define BOOT_NUM_RETRY  3

#define BOOT_STAT_MASK_ALL     0x0000FFFF
#define BOOT_STAT_MASK_STAT    0x0000F000
#define BOOT_STAT_MASK_BERR    0x00000F00
#define BOOT_STAT_MASK_BSTRAP  0x000000F0
#define BOOT_STAT_MASK_BMODULE 0x0000000F

#define BOOT_STAT_INIT         0x00000000
#define BOOT_STAT_BSTRAP       0x00001000
#define BOOT_STAT_RETRY        0x00002000
#define BOOT_STAT_START        0x00003000
#define BOOT_STAT_HALT         0x0000F000

#define BOOT_ERR_NO_RVEC       0x00000100
#define BOOT_ERR_NO_NVEC       0x00000200
#define BOOT_ERR_NO_EVEC       0x00000300
#define BOOT_ERR_BSTRAP        0x00000400
#define BOOT_ERR_EXC           0x00000800

#ifndef __ASSEMBLY__
void boot_set_status( u32 status, u32 mask);
void boot_set_config( u32 config);
void boot_set_rvec( u32 vector);
void boot_set_size( u32 size);
void boot_sdbg( u8* string, u32 value);
void boot_error( u32 berr);
int boot_from_ebu(void);
void _boot_rvec(void);
typedef struct
{
	u32   cpu;              /** CPU number */
	u32   config;           /** Boot configuration */
   u32   endian;           /** CPU endianess */
   u32   debug;            /** Debug mode */
	u32	(*exit)(void);	   /** application vector */
} boot_data;

extern boot_data bootrom;
#endif

#endif /* #ifdef _BOOT_H */
