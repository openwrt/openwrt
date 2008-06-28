/*
 * (C) Copyright 2003
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <command.h>
#include <asm/addrspace.h>
#include <asm/danube.h>

#ifdef DANUBE_USE_DDR_RAM
long int initdram(int board_type)
{
	return (1024*1024*DANUBE_DDR_RAM_SIZE);
}
#else
extern uint danube_get_cpuclk(void);

static ulong max_sdram_size(void)     /* per Chip Select */
{
	/* The only supported SDRAM data width is 16bit.
	 */
#define CFG_DW	4

	/* The only supported number of SDRAM banks is 4.
	 */
#define CFG_NB	4

	ulong cfgpb0 = *DANUBE_SDRAM_MC_CFGPB0;
	int   cols   = cfgpb0 & 0xF;
	int   rows   = (cfgpb0 & 0xF0) >> 4;
	ulong size   = (1 << (rows + cols)) * CFG_DW * CFG_NB;

	return size;
}

/*
 * Check memory range for valid RAM. A simple memory test determines
 * the actually available RAM size between addresses `base' and
 * `base + maxsize'. 
 */

static long int dram_size(long int *base, long int maxsize)
{
	volatile long int *addr;
	ulong cnt, val;
	ulong save[32];			/* to make test non-destructive */
	unsigned char i = 0;

	for (cnt = (maxsize / sizeof (long)) >> 1; cnt > 0; cnt >>= 1) {
		addr = base + cnt;		/* pointer arith! */

		save[i++] = *addr;
		*addr = ~cnt;
	}

	/* write 0 to base address */
	addr = base;
	save[i] = *addr;
	*addr = 0;

	/* check at base address */
	if ((val = *addr) != 0) {
		*addr = save[i];
		return (0);
	}

	for (cnt = 1; cnt < maxsize / sizeof (long); cnt <<= 1) {
		addr = base + cnt;		/* pointer arith! */

		val = *addr;
		*addr = save[--i];

		if (val != (~cnt)) {
			return (cnt * sizeof (long));
		}
	}
	return (maxsize);
}

long int initdram(int board_type)
{
	int   rows, cols, best_val = *DANUBE_SDRAM_MC_CFGPB0;
	ulong size, max_size       = 0;
	ulong our_address;

	/* load t9 into our_address */	
	asm volatile ("move %0, $25" : "=r" (our_address) :);

	/* Can't probe for RAM size unless we are running from Flash.
	 * find out whether running from DRAM or Flash.
	 */
	if (PHYSADDR(our_address) < PHYSADDR(PHYS_FLASH_1))
	{
		return max_sdram_size();
	}

	for (cols = 0x8; cols <= 0xC; cols++)
	{
		for (rows = 0xB; rows <= 0xD; rows++)
		{
			*DANUBE_SDRAM_MC_CFGPB0 = (0x14 << 8) |
			                           (rows << 4) | cols;
			size = dram_size((ulong *)CFG_SDRAM_BASE,
			                                     max_sdram_size());

			if (size > max_size)
			{
				best_val = *DANUBE_SDRAM_MC_CFGPB0;
				max_size = size;
			}
		}
	}

	*DANUBE_SDRAM_MC_CFGPB0 = best_val;
	return max_size;
}
#endif

int checkboard (void)
{
	/*    No such register in Amazon */
#if 0
	unsigned long chipid = *AMAZON_MCD_CHIPID;
	int part_num;

	puts ("Board: AMAZON ");
	part_num = AMAZON_MCD_CHIPID_PART_NUMBER_GET(chipid);
	switch (part_num) {
	case AMAZON_CHIPID_STANDARD:
		printf ("Standard Version, ");
		break;
	case AMAZON_CHIPID_YANGTSE:
		printf ("Yangtse Version, ");
		break;
	default:
		printf ("Unknown Part Number 0x%x ", part_num);
		break;
	}

	printf ("Chip V1.%ld, ", AMAZON_MCD_CHIPID_VERSION_GET(chipid));
     

	printf("CPU Speed %d MHz\n", danube_get_cpuclk()/1000000);
	
#endif
	return 0;
}


/*
 * Disk On Chip (NAND) Millenium initialization.
 * The NAND lives in the CS2* space
 */
#if (CONFIG_COMMANDS & CFG_CMD_NAND)
extern void
nand_probe(ulong physadr);

#define AT91_SMARTMEDIA_BASE 0x40000000  /* physical address to access memory on NCS3 */
void
nand_init(void)
{
       	int devtype;
	/* Configure EBU */
//TODO: should we keep this?
        //Set GPIO23 to be Flash CS1;
	*DANUBE_GPIO_P1_ALTSEL0 = *DANUBE_GPIO_P1_ALTSEL0 | (1<<7);
	*DANUBE_GPIO_P1_ALTSEL1 = *DANUBE_GPIO_P1_ALTSEL1 & ~(1<<7);
	*DANUBE_GPIO_P1_DIR = *DANUBE_GPIO_P1_DIR | (1<<7) ;
	*DANUBE_GPIO_P1_OD = *DANUBE_GPIO_P1_OD | (1<<7) ;
	
	*EBU_ADDR_SEL_1 = (NAND_BASE_ADDRESS&0x1fffff00)|0x31;
	/* byte swap;minimum delay*/
	*EBU_CON_1      = 0x40C155;
	*EBU_NAND_CON   = 0x000005F3;

	/* Set bus signals to inactive */
	 NAND_READY_CLEAR;

	 NAND_CE_CLEAR;
         nand_probe(NAND_BASE_ADDRESS);



	//nand_probe(AT91_SMARTMEDIA_BASE);
}
#endif



