/*
 * (C) Copyright 2003
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * (C) Copyright 2010
 * Thomas Langer, Ralph Hempel
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
#include <netdev.h>
#include <miiphy.h>
#include <asm/addrspace.h>
#include <asm/danube.h>
#include <asm/reboot.h>
#include <asm/io.h>
#if defined(CONFIG_CMD_HTTPD)
#include <httpd.h>
#endif

extern ulong ifx_get_ddr_hz(void);
extern ulong ifx_get_cpuclk(void);

/* definitions for external PHYs / Switches */
/* Split values into phy address and register address */
#define PHYADDR(_reg)	((_reg >> 5) & 0xff), (_reg & 0x1f)

/* IDs and registers of known external switches */
#define ID_SAMURAI_0	0x1020
#define ID_SAMURAI_1	0x0007
#define SAMURAI_ID_REG0	0xA0
#define SAMURAI_ID_REG1	0xA1

#define ID_TANTOS	0x2599

void _machine_restart(void)
{
	*DANUBE_RCU_RST_REQ |=1<<30;
}

#ifdef CONFIG_SYS_RAMBOOT
phys_size_t initdram(int board_type)
{
	return get_ram_size((long *)CONFIG_SYS_SDRAM_BASE, CONFIG_SYS_MAX_RAM);
}
#elif defined(CONFIG_USE_DDR_RAM)
phys_size_t initdram(int board_type)
{
	return (CONFIG_SYS_MAX_RAM);
}
#else

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

phys_size_t initdram(int board_type)
{
	int   rows, cols, best_val = *DANUBE_SDRAM_MC_CFGPB0;
	ulong size, max_size       = 0;
	ulong our_address;

	/* load t9 into our_address */
	asm volatile ("move %0, $25" : "=r" (our_address) :);

	/* Can't probe for RAM size unless we are running from Flash.
	 * find out whether running from DRAM or Flash.
	 */
	if (CPHYSADDR(our_address) < CPHYSADDR(PHYS_FLASH_1))
	{
		return max_sdram_size();
	}

	for (cols = 0x8; cols <= 0xC; cols++)
	{
		for (rows = 0xB; rows <= 0xD; rows++)
		{
			*DANUBE_SDRAM_MC_CFGPB0 = (0x14 << 8) |
			                          (rows << 4) | cols;
			size = get_ram_size((long *)CONFIG_SYS_SDRAM_BASE,
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
	unsigned long chipid = *DANUBE_MPS_CHIPID;
	int part_num;

	puts ("Board: ");

	part_num = DANUBE_MPS_CHIPID_PARTNUM_GET(chipid);
	switch (part_num)
	{
	case 0x129:
	case 0x12D:
		puts("Danube/Twinpass/Vinax-VE ");
		break;
	default:
		printf ("unknown, chip part number 0x%03X ", part_num);
		break;
	}
	printf ("V1.%ld, ", DANUBE_MPS_CHIPID_VERSION_GET(chipid));

	printf("DDR Speed %ld MHz, ", ifx_get_ddr_hz()/1000000);
	printf("CPU Speed %ld MHz\n", ifx_get_cpuclk()/1000000);

	return 0;
}

#ifdef CONFIG_SKIP_LOWLEVEL_INIT
int board_early_init_f(void)
{
#ifdef CONFIG_EBU_ADDSEL0
	(*DANUBE_EBU_ADDSEL0) = CONFIG_EBU_ADDSEL0;
#endif
#ifdef CONFIG_EBU_ADDSEL1
	(*DANUBE_EBU_ADDSEL1) = CONFIG_EBU_ADDSEL1;
#endif
#ifdef CONFIG_EBU_ADDSEL2
	(*DANUBE_EBU_ADDSEL2) = CONFIG_EBU_ADDSEL2;
#endif
#ifdef CONFIG_EBU_ADDSEL3
	(*DANUBE_EBU_ADDSEL3) = CONFIG_EBU_ADDSEL3;
#endif
#ifdef CONFIG_EBU_BUSCON0
	(*DANUBE_EBU_BUSCON0) = CONFIG_EBU_BUSCON0;
#endif
#ifdef CONFIG_EBU_BUSCON1
	(*DANUBE_EBU_BUSCON1) = CONFIG_EBU_BUSCON1;
#endif
#ifdef CONFIG_EBU_BUSCON2
	(*DANUBE_EBU_BUSCON2) = CONFIG_EBU_BUSCON2;
#endif
#ifdef CONFIG_EBU_BUSCON3
	(*DANUBE_EBU_BUSCON3) = CONFIG_EBU_BUSCON3;
#endif

	return 0;
}
#endif /* CONFIG_SKIP_LOWLEVEL_INIT */

#ifdef CONFIG_EXTRA_SWITCH
static int external_switch_init(void)
{
	unsigned short chipid0=0xdead, chipid1=0xbeef;
	static char * const name = "lq_cpe_eth";

#ifdef CLK_OUT2_25MHZ
	*DANUBE_GPIO_P0_DIR=0x0000ae78;
	*DANUBE_GPIO_P0_ALTSEL0=0x00008078;
	//joelin for Mii-1       *DANUBE_GPIO_P0_ALTSEL1=0x80000080;
	*DANUBE_GPIO_P0_ALTSEL1=0x80000000; //joelin for Mii-1
	*DANUBE_CGU_IFCCR=0x00400010;
	*DANUBE_GPIO_P0_OD=0x0000ae78;
#endif

	/* earlier no valid response is available, at least on Twinpass & Tantos @ 111MHz, M4530 platform */
	udelay(100000);

	debug("\nsearching for Samurai switch ... ");
	if ( (miiphy_read(name, PHYADDR(SAMURAI_ID_REG0), &chipid0)==0) &&
	     (miiphy_read(name, PHYADDR(SAMURAI_ID_REG1), &chipid1)==0) ) {
		if (((chipid0 & 0xFFF0) == ID_SAMURAI_0) &&
		    ((chipid1 & 0x000F) == ID_SAMURAI_1)) {
			debug("found");

			/* enable "Crossover Auto Detect" + defaults */
			/* P0 */
			miiphy_write(name, PHYADDR(0x01), 0x840F);
			/* P1 */
			miiphy_write(name, PHYADDR(0x03), 0x840F);
			/* P2 */
			miiphy_write(name, PHYADDR(0x05), 0x840F);
			/* P3 */
			miiphy_write(name, PHYADDR(0x07), 0x840F);
			/* P4 */
			miiphy_write(name, PHYADDR(0x08), 0x840F);
			/* P5 */
			miiphy_write(name, PHYADDR(0x09), 0x840F);
			/* System Control 4: CPU on port 1 and other */
			miiphy_write(name, PHYADDR(0x12), 0x3602);
			#ifdef CLK_OUT2_25MHZ
			/* Bandwidth Control Enable Register: enable */
			miiphy_write(name, PHYADDR(0x33), 0x4000);
			#endif
		}
	}

	debug("\nsearching for TANTOS switch ... ");
	if (miiphy_read(name, PHYADDR(0x101), &chipid0) == 0) {
		if (chipid0 == ID_TANTOS) {
			debug("found");

			/* P5 Basic Control: Force Link Up */
			miiphy_write(name, PHYADDR(0xA1), 0x0004);
			/* P6 Basic Control: Force Link Up */
			miiphy_write(name, PHYADDR(0xC1), 0x0004);
			/* RGMII/MII Port Control (P4/5/6) */
			miiphy_write(name, PHYADDR(0xF5), 0x0773);

			/* Software workaround. */
			/* PHY reset from P0 to P4. */

			/* set data for indirect write */
			miiphy_write(name, PHYADDR(0x121), 0x8000);

			/* P0 */
			miiphy_write(name, PHYADDR(0x120), 0x0400);
			udelay(1000);
			/* P1 */
			miiphy_write(name, PHYADDR(0x120), 0x0420);
			udelay(1000);
			/* P2 */
			miiphy_write(name, PHYADDR(0x120), 0x0440);
			udelay(1000);
			/* P3 */
			miiphy_write(name, PHYADDR(0x120), 0x0460);
			udelay(1000);
			/* P4 */
			miiphy_write(name, PHYADDR(0x120), 0x0480);
			udelay(1000);
		}
	}
	debug("\n");

	return 0;
}
#endif /* CONFIG_EXTRA_SWITCH */

int board_eth_init(bd_t *bis)
{
#if defined(CONFIG_IFX_ETOP)

	*DANUBE_PMU_PWDCR &= 0xFFFFEFDF;
	*DANUBE_PMU_PWDCR &=~(1<<DANUBE_PMU_DMA_SHIFT);/*enable DMA from PMU*/

	if (lq_eth_initialize(bis)<0)
		return -1;

	*DANUBE_RCU_RST_REQ |=1;
	udelay(200000);
	*DANUBE_RCU_RST_REQ &=(unsigned long)~1;
	udelay(1000);

#ifdef CONFIG_EXTRA_SWITCH
	if (external_switch_init()<0)
		return -1;
#endif /* CONFIG_EXTRA_SWITCH */
#endif /* CONFIG_IFX_ETOP */

	return 0;
}

#if defined(CONFIG_CMD_HTTPD)
int do_http_upgrade(const unsigned char *data, const ulong size)
{
	char buf[128];

	if(getenv ("ram_addr") == NULL)
		return -1;
	if(getenv ("kernel_addr") == NULL)
		return -1;
	/* check the image */
	if(run_command("imi ${ram_addr}", 0) < 0) {
		return -1;
	}
	/* write the image to the flash */
	puts("http ugrade ...\n");
	sprintf(buf, "era ${kernel_addr} +0x%x; cp.b ${ram_addr} ${kernel_addr} 0x%x", size, size);
	return run_command(buf, 0);
}

int do_http_progress(const int state)
{
	/* toggle LED's here */
	switch(state) {
		case HTTP_PROGRESS_START:
		puts("http start\n");
		break;
		case HTTP_PROGRESS_TIMEOUT:
		puts(".");
		break;
		case HTTP_PROGRESS_UPLOAD_READY:
		puts("http upload ready\n");
		break;
		case HTTP_PROGRESS_UGRADE_READY:
		puts("http ugrade ready\n");
		break;
		case HTTP_PROGRESS_UGRADE_FAILED:
		puts("http ugrade failed\n");
		break;
	}
	return 0;
}

unsigned long do_http_tmp_address(void)
{
	char *s = getenv ("ram_addr");
	if (s) {
		ulong tmp = simple_strtoul (s, NULL, 16);
		return tmp;
	}
	return 0 /*0x80a00000*/;
}

#endif
