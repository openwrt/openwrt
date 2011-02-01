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
#if defined(CONFIG_PCI)
#include <pci.h>
#endif
#if defined(CONFIG_AR8216_SWITCH)
#include "athrs26_phy.h"
#endif

extern ulong ifx_get_ddr_hz(void);
extern ulong ifx_get_cpuclk(void);

/* IDs and registers of known external switches */
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

static void gpio_default(void)
{
#ifdef CONFIG_SWITCH_PORT0
	*DANUBE_GPIO_P0_ALTSEL0 &= ~(1<<CONFIG_SWITCH_PIN);
	*DANUBE_GPIO_P0_ALTSEL1 &= ~(1<<CONFIG_SWITCH_PIN);
	*DANUBE_GPIO_P0_OD |= (1<<CONFIG_SWITCH_PIN);
	*DANUBE_GPIO_P0_DIR |= (1<<CONFIG_SWITCH_PIN);
	*DANUBE_GPIO_P0_OUT |= (1<<CONFIG_SWITCH_PIN);
#elif defined(CONFIG_SWITCH_PORT1)
	*DANUBE_GPIO_P1_ALTSEL0 &= ~(1<<CONFIG_SWITCH_PIN);
	*DANUBE_GPIO_P1_ALTSEL1 &= ~(1<<CONFIG_SWITCH_PIN);
	*DANUBE_GPIO_P1_OD |= (1<<CONFIG_SWITCH_PIN);
	*DANUBE_GPIO_P1_DIR |= (1<<CONFIG_SWITCH_PIN);
	*DANUBE_GPIO_P1_OUT |= (1<<CONFIG_SWITCH_PIN);
#endif
#ifdef CONFIG_EBU_GPIO
	{
		int i = 0;
		printf ("bring up ebu gpio\n");
		*DANUBE_EBU_BUSCON1 = 0x1e7ff;
		*DANUBE_EBU_ADDSEL1 = 0x14000001;

		*((volatile u16*)0xb4000000) = 0x0;
		for(i = 0; i < 1000; i++)
			udelay(1000);
		*((volatile u16*)0xb4000000) = CONFIG_EBU_GPIO;
		*DANUBE_EBU_BUSCON1 = 0x8001e7ff;
	}
#endif
#ifdef CONFIG_BUTTON_PORT0
	*DANUBE_GPIO_P0_ALTSEL0 &= ~(1<<CONFIG_BUTTON_PIN);
	*DANUBE_GPIO_P0_ALTSEL1 &= ~(1<<CONFIG_BUTTON_PIN);
	*DANUBE_GPIO_P0_DIR &= ~(1<<CONFIG_BUTTON_PIN);
	if(!!(*DANUBE_GPIO_P0_IN & (1<<CONFIG_BUTTON_PIN)) == CONFIG_BUTTON_LEVEL)
	{
		printf("button is pressed\n");
		setenv("bootdelay", "0");
		setenv("bootcmd", "httpd");
	}
#elif defined(CONFIG_BUTTON_PORT1)
	*DANUBE_GPIO_P1_ALTSEL0 &= ~(1<<CONFIG_BUTTON_PIN);
	*DANUBE_GPIO_P1_ALTSEL1 &= ~(1<<CONFIG_BUTTON_PIN);
	*DANUBE_GPIO_P1_DIR &= ~(1<<CONFIG_BUTTON_PIN);
	if(!!(*DANUBE_GPIO_P1_IN & (1<<CONFIG_BUTTON_PIN)) == CONFIG_BUTTON_LEVEL)
	{
		printf("button is pressed\n");
		setenv("bootdelay", "0");
		setenv("bootcmd", "httpd");
	}
#endif

}

int checkboard (void)
{
	unsigned long chipid = *DANUBE_MPS_CHIPID;
	int part_num;

	puts ("Board: "CONFIG_ARCADYAN"\n");
	puts ("SoC: ");

	part_num = DANUBE_MPS_CHIPID_PARTNUM_GET(chipid);
	switch (part_num)
	{
	case 0x129:
	case 0x12D:
	case 0x12b: 
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

#ifdef CONFIG_RTL8306_SWITCH
#define ID_RTL8306	0x5988
static int external_switch_rtl8306(void)
{
	unsigned short chipid;
	static char * const name = "lq_cpe_eth";

	udelay(100000);

	puts("\nsearching for rtl8306 switch ... ");
	if (miiphy_read(name, 4, 30, &chipid) == 0) {
		if (chipid == ID_RTL8306) {
			puts("found");
			/* set led mode */
			miiphy_write(name, 0, 19, 0xffff);
			/* magic */
			miiphy_write(name, 4, 22, 0x877f);
			puts("\n");
			return 0;
		}
		puts("failed\n");
	}
	puts("\nno known switch found ... \n");

	return 0;
}
#endif

#ifdef CONFIG_AR8216_SWITCH
static int external_switch_ar8216(void)
{
	puts("initializing ar8216 switch... ");
	if (athrs26_phy_setup(0)==0) {
	   printf("initialized\n");
	   return 0;
	}
	puts("failed ... \n");
	return 0;
}
#endif

int board_eth_init(bd_t *bis)
{
	gpio_default();

#if defined(CONFIG_IFX_ETOP)
	uchar enetaddr[6];
	if (!eth_getenv_enetaddr("ethaddr", enetaddr))
               eth_setenv_enetaddr("ethaddr", (uchar *)0xb03f0016);

	*DANUBE_PMU_PWDCR &= 0xFFFFEFDF;
	*DANUBE_PMU_PWDCR &=~(1<<DANUBE_PMU_DMA_SHIFT);/*enable DMA from PMU*/

	if (lq_eth_initialize(bis))
		return -1;

	*DANUBE_RCU_RST_REQ |=1;
	udelay(200000);
	*DANUBE_RCU_RST_REQ &=(unsigned long)~1;
	udelay(1000);

#ifdef CONFIG_RTL8306_SWITCH
	if (external_switch_rtl8306()<0)
		return -1;
#endif
#ifdef CONFIG_AR8216_SWITCH
	if (external_switch_ar8216()<0)
		return -1;
#endif
#endif
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
	sprintf(buf, "era ${kernel_addr} +0x%lx; cp.b ${ram_addr} ${kernel_addr} 0x%lx", size, size);
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
