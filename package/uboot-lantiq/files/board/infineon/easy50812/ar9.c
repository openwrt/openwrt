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
#include <asm/ar9.h>
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
#define ID_SAMURAI_0			0x1020
#define ID_SAMURAI_1			0x0007
#define SAMURAI_ID_REG0			0xA0
#define SAMURAI_ID_REG1			0xA1
#define ID_TANTOS			0x2599

#define RGMII_MODE			0
#define MII_MODE			1
#define REV_MII_MODE			2
#define RED_MII_MODE_IC			3		/*Input clock */
#define RGMII_MODE_100MB		4
#define TURBO_REV_MII_MODE		6		/*Turbo Rev Mii mode */
#define RED_MII_MODE_OC			7		/*Output clock */
#define RGMII_MODE_10MB			8

#define mdelay(n)   udelay((n)*1000)

static void ar9_sw_chip_init(u8 port, u8 mode);
static void ar9_enable_sw_port(u8 port, u8 state);
static void ar9_configure_sw_port(u8 port, u8 mode);
static u16 ar9_smi_reg_read(u16 reg);
static u16 ar9_smi_reg_write(u16 reg, u16 data);
static char * const name = "lq_cpe_eth";
static int external_switch_init(void);

void _machine_restart(void)
{
	*AR9_RCU_RST_REQ |= AR9_RST_ALL;
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

	ulong cfgpb0 = *AR9_SDRAM_MC_CFGPB0;
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
	int   rows, cols, best_val = *AR9_SDRAM_MC_CFGPB0;
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
			*AR9_SDRAM_MC_CFGPB0 = (0x14 << 8) |
											(rows << 4) | cols;
			size = get_ram_size((long *)CONFIG_SYS_SDRAM_BASE,
											max_sdram_size());

			if (size > max_size)
			{
				best_val = *AR9_SDRAM_MC_CFGPB0;
				max_size = size;
			}
		}
	}

	*AR9_SDRAM_MC_CFGPB0 = best_val;
	return max_size;
}
#endif

int checkboard (void)
{
	unsigned long chipid = *AR9_MPS_CHIPID;
	int part_num;

	puts ("Board: ");

	part_num = AR9_MPS_CHIPID_PARTNUM_GET(chipid);
	switch (part_num)
	{
	case 0x16C:
		puts("ARX188 ");
		break;
	case 0x16D:
		puts("ARX168 ");
		break;
	case 0x16F:
		puts("ARX182 ");
		break;
	case 0x170:
		puts("GRX188 ");
		break;
	case 0x171:
		puts("GRX168 ");
		break;
	default:
		printf ("unknown, chip part number 0x%03X ", part_num);
		break;
	}
	printf ("V1.%ld, ", AR9_MPS_CHIPID_VERSION_GET(chipid));

	printf("DDR Speed %ld MHz, ", ifx_get_ddr_hz()/1000000);
	printf("CPU Speed %ld MHz\n", ifx_get_cpuclk()/1000000);

	return 0;
}

#ifdef CONFIG_SKIP_LOWLEVEL_INIT
int board_early_init_f(void)
{
#ifdef CONFIG_EBU_ADDSEL0
	(*AR9_EBU_ADDSEL0) = CONFIG_EBU_ADDSEL0;
#endif
#ifdef CONFIG_EBU_ADDSEL1
	(*AR9_EBU_ADDSEL1) = CONFIG_EBU_ADDSEL1;
#endif
#ifdef CONFIG_EBU_ADDSEL2
	(*AR9_EBU_ADDSEL2) = CONFIG_EBU_ADDSEL2;
#endif
#ifdef CONFIG_EBU_ADDSEL3
	(*AR9_EBU_ADDSEL3) = CONFIG_EBU_ADDSEL3;
#endif
#ifdef CONFIG_EBU_BUSCON0
	(*AR9_EBU_BUSCON0) = CONFIG_EBU_BUSCON0;
#endif
#ifdef CONFIG_EBU_BUSCON1
	(*AR9_EBU_BUSCON1) = CONFIG_EBU_BUSCON1;
#endif
#ifdef CONFIG_EBU_BUSCON2
	(*AR9_EBU_BUSCON2) = CONFIG_EBU_BUSCON2;
#endif
#ifdef CONFIG_EBU_BUSCON3
	(*AR9_EBU_BUSCON3) = CONFIG_EBU_BUSCON3;
#endif

	return 0;
}
#endif /* CONFIG_SKIP_LOWLEVEL_INIT */

int board_eth_init(bd_t *bis)
{
#if defined(CONFIG_IFX_ETOP)

	*AR9_PMU_PWDCR &= 0xFFFFEFDF;
	*AR9_PMU_PWDCR &= ~AR9_PMU_DMA; /* enable DMA from PMU */

	if (lq_eth_initialize(bis) < 0)
		return -1;

	*AR9_RCU_RST_REQ |= 1;
	udelay(200000);
	*AR9_RCU_RST_REQ &= (unsigned long)~1;
	udelay(1000);

#ifdef CONFIG_EXTRA_SWITCH
	if (external_switch_init()<0)
		return -1;
#endif /* CONFIG_EXTRA_SWITCH */
#endif /* CONFIG_IFX_ETOP */

	return 0;
}

static void ar9_configure_sw_port(u8 port, u8 mode)
{
	if(port)
	{
		if (mode  == 1) //MII mode
		{
			*AR9_GPIO_P2_ALTSEL0 = *AR9_GPIO_P2_ALTSEL0 | (0xf000);
			*AR9_GPIO_P2_ALTSEL1 = *AR9_GPIO_P2_ALTSEL1 & ~(0xf000);
			*AR9_GPIO_P2_DIR = (*AR9_GPIO_P2_DIR & ~(0xf000)) | 0x2000;
			*AR9_GPIO_P2_OD = *AR9_GPIO_P2_OD | 0x2000;
		}
		else if(mode == 2 || mode == 6) //Rev Mii mode
		{
			*AR9_GPIO_P2_ALTSEL0 = *AR9_GPIO_P2_ALTSEL0 | (0xf000);
			*AR9_GPIO_P2_ALTSEL1 = *AR9_GPIO_P2_ALTSEL1 & ~(0xf000);
			*AR9_GPIO_P2_DIR = (*AR9_GPIO_P2_DIR | (0xf000)) & ~0x2000;
			*AR9_GPIO_P2_OD = *AR9_GPIO_P2_OD | 0xd000;
		}
	}
	else //Port 0
	{
		if (mode  == 1) //MII mode
		{
			*AR9_GPIO_P2_ALTSEL0 = *AR9_GPIO_P2_ALTSEL0 | (0x0303);
			*AR9_GPIO_P2_ALTSEL1 = *AR9_GPIO_P2_ALTSEL1 & ~(0x0303);
			*AR9_GPIO_P2_DIR = (*AR9_GPIO_P2_DIR & ~(0x0303)) | 0x0100;
			*AR9_GPIO_P2_OD = *AR9_GPIO_P2_OD | 0x0100;
		}
		else if(mode ==2 || mode ==6) //Rev Mii mode
		{
			*AR9_GPIO_P2_ALTSEL0 = *AR9_GPIO_P2_ALTSEL0 | (0x0303);
			*AR9_GPIO_P2_ALTSEL1 = *AR9_GPIO_P2_ALTSEL1 & ~(0x0303);
			*AR9_GPIO_P2_DIR = (*AR9_GPIO_P2_DIR | (0x0303)) & ~0x0100;
			*AR9_GPIO_P2_OD = *AR9_GPIO_P2_OD | 0x0203;
		}
	}
}

/*
Call this function to place either MAC port 0 or 1 into working mode.
Parameters:
port - select ports 0 or 1.
state of interface : state
0: RGMII
1: MII
2: Rev MII
3: Reduce MII (input clock)
4: RGMII 100mb
5: Reserve
6: Turbo Rev MII
7: Reduce MII (output clock)
*/
void ar9_enable_sw_port(u8 port, u8 state)
{
	REG32(AR9_SW_GCTL0) |= 0x80000000;
	if (port == 0)
	{
		REG32(AR9_SW_RGMII_CTL) &= 0xffcffc0e ;
	//#if AR9_REFBOARD_TANTOS
		REG32(0xbf20302c) &= 0xffff81ff;
		REG32(0xbf20302c) |= 4<<9 ;
	//#endif
		REG32(AR9_SW_RGMII_CTL) |= ((u32)(state &0x3))<<8;
		if((state &0x3) == 0)
		{
			REG32(AR9_SW_RGMII_CTL) &= 0xfffffff3;
			if(state == 4)
				REG32(AR9_SW_RGMII_CTL) |= 0x4;
			else
				REG32(AR9_SW_RGMII_CTL) |= 0x8;
		}
		if(state == 6)
			REG32(AR9_SW_RGMII_CTL) |= ((u32) (1<<20));
		if(state == 7)
			REG32(AR9_SW_RGMII_CTL) |= ((u32) (1<<21));
	}
//  *AR9_PPE32_ETOP_CFG = *AR9_PPE32_ETOP_CFG & 0xfffffffe;
	else
	{
		REG32(AR9_SW_RGMII_CTL) &= 0xff303fff ;
		REG32(AR9_SW_RGMII_CTL) |= ((u32)(state &0x3))<<18;
		if((state &0x3) == 0)
		{
			REG32(AR9_SW_RGMII_CTL) &= 0xffffcfff;
			if(state == 4)
				REG32(AR9_SW_RGMII_CTL) |= 0x1000;
			else
				REG32(AR9_SW_RGMII_CTL) |= 0x2000;
		}
		if(state == 6)
			REG32(AR9_SW_RGMII_CTL) |= ((u32) (1<<22));
		if(state == 7)
			REG32(AR9_SW_RGMII_CTL) |= ((u32) (1<<23));
	}
}

void pci_reset(void)
{
	int i,j;
#define AR9_V1_PCI_RST_FIX 1
#if AR9_V1_PCI_RST_FIX // 5th June 2008 Add GPIO19 to control EJTAG_TRST
	*AR9_GPIO_P1_ALTSEL0 = *AR9_GPIO_P1_ALTSEL0 & ~0x8;
	*AR9_GPIO_P1_ALTSEL1 = *AR9_GPIO_P1_ALTSEL1 & ~0x8;
	*AR9_GPIO_P1_DIR = *AR9_GPIO_P1_DIR | 0x8;
	*AR9_GPIO_P1_OD = *AR9_GPIO_P1_OD | 0x8;
	*AR9_GPIO_P1_OUT = *AR9_GPIO_P1_OUT | 0x8;
	*AR9_GPIO_P0_ALTSEL0 = *AR9_GPIO_P0_ALTSEL0 & ~0x4000;
	*AR9_GPIO_P0_ALTSEL1 = *AR9_GPIO_P0_ALTSEL1 & ~0x4000;
	*AR9_GPIO_P0_DIR = *AR9_GPIO_P0_DIR | 0x4000;
	*AR9_GPIO_P0_OD = *AR9_GPIO_P0_OD | 0x4000;
	for(j=0;j<5;j++) {
		*AR9_GPIO_P0_OUT = *AR9_GPIO_P0_OUT & ~0x4000;
		for(i=0;i<0x10000;i++);
		*AR9_GPIO_P0_OUT = *AR9_GPIO_P0_OUT | 0x4000;
		for(i=0;i<0x10000;i++);
	}
	*AR9_GPIO_P0_DIR = *AR9_GPIO_P0_DIR &  ~0x4000;
	*AR9_GPIO_P1_DIR = *AR9_GPIO_P1_DIR &  ~0x8;
#endif
}

static u16 ar9_smi_reg_read(u16 reg)
{
	int i;
	while(REG32(AR9_SW_MDIO_CTL) & 0x8000);
	REG32(AR9_SW_MDIO_CTL) = 0x8000| 0x2<<10 | ((u32) (reg&0x3ff)) ; /*0x10=MDIO_OP_READ*/
	for(i=0;i<0x3fff;i++);
	udelay(50);
        while(REG32(AR9_SW_MDIO_CTL) & 0x8000);
	return((u16) (REG32(AR9_SW_MDIO_DATA)));
}

static u16 ar9_smi_reg_write(u16 reg, u16 data)
{
	int i;
	while(REG32(AR9_SW_MDIO_CTL) & 0x8000);
	REG32(AR9_SW_MDIO_CTL) = 0x8000| (((u32) data)<<16) | 0x01<<10 | ((u32) (reg&0x3ff)) ; /*0x01=MDIO_OP_WRITE*/
	for(i=0;i<0x3fff;i++);
		udelay(50);
	return 0;
}

static void ar9_sw_chip_init(u8 port, u8 mode)
{
	int i;
	u16 chipid;

	debug("\nsearching for switches ... ");

	asm("sync");
	pci_reset();

	/* 25mhz clock out */
	*AR9_CGU_IFCCR &= ~(3<<10);
	*AR9_GPIO_P0_ALTSEL0 = *AR9_GPIO_P0_ALTSEL0 | (1<<3);
	*AR9_GPIO_P0_ALTSEL1 = *AR9_GPIO_P0_ALTSEL1 & ~(1<<3);
	*AR9_GPIO_P0_DIR = *AR9_GPIO_P0_DIR  | (1<<3);
	*AR9_GPIO_P0_OD = *AR9_GPIO_P0_OD | (1<<3);
	*AR9_GPIO_P2_ALTSEL0 = *AR9_GPIO_P2_ALTSEL0 & ~(1<<0);
	*AR9_GPIO_P2_ALTSEL1 = *AR9_GPIO_P2_ALTSEL1 & ~(1<<0);
	*AR9_GPIO_P2_DIR = *AR9_GPIO_P2_DIR  | (1<<0);
	*AR9_GPIO_P2_OD = *AR9_GPIO_P2_OD | (1<<0);

	*AR9_PMU_PWDCR = (*AR9_PMU_PWDCR & 0xFFFBDFDF) ; 
	*AR9_PMU_PWDCR = (*AR9_PMU_PWDCR & ~(AR9_PMU_DMA | AR9_PMU_SWITCH));
	*AR9_PMU_PWDCR = (*AR9_PMU_PWDCR | AR9_PMU_USB0 | AR9_PMU_USB0_P);

	*AR9_GPIO_P2_OUT &= ~(1<<0);
	asm("sync");

	ar9_configure_sw_port(port, mode);
	ar9_enable_sw_port(port, mode);
	REG32(AR9_SW_P0_CTL) |= 0x400000; /* disable mdio polling for tantos */
	asm("sync");

	/*GPIO 55(P3.7) used as output, set high*/
	*AR9_GPIO_P3_OD |=(1<<7);
	*AR9_GPIO_P3_DIR |= (1<<7);
	*AR9_GPIO_P3_ALTSEL0 &=~(1<<7);
	*AR9_GPIO_P3_ALTSEL1 &=~(1<<7);
	asm("sync");
	udelay(10);

	*AR9_GPIO_P3_OUT &= ~(1<<7);
	for(i=0;i<1000;i++)
		udelay(110);
	*AR9_GPIO_P3_OUT |=(1<<7);
	udelay(100);

	if(port==0)
		REG32(AR9_SW_P0_CTL) |= 0x40001;
	else
		REG32(AR9_SW_P1_CTL) |= 0x40001;

	REG32(AR9_SW_P2_CTL) |= 0x40001;
	REG32(AR9_SW_PMAC_HD_CTL) |= 0x40000; /* enable CRC */

	*AR9_GPIO_P2_ALTSEL0 = *AR9_GPIO_P2_ALTSEL0 | (0xc00);
	*AR9_GPIO_P2_ALTSEL1 = *AR9_GPIO_P2_ALTSEL1 & ~(0xc00);
	*AR9_GPIO_P2_DIR = *AR9_GPIO_P2_DIR  | 0xc00;
	*AR9_GPIO_P2_OD = *AR9_GPIO_P2_OD | 0xc00;

	asm("sync");
	chipid = (unsigned short)(ar9_smi_reg_read(0x101));
	printf("\nswitch chip id=%08x\n",chipid);
	if (chipid != ID_TANTOS) {
		debug("whatever detected\n");
		ar9_smi_reg_write(0x1,0x840f);
		ar9_smi_reg_write(0x3,0x840f);
		ar9_smi_reg_write(0x5,0x840f);
		ar9_smi_reg_write(0x7,0x840f);
		ar9_smi_reg_write(0x8,0x840f);
		ar9_smi_reg_write(0x12,0x3602);
#ifdef CLK_OUT2_25MHZ
		ar9_smi_reg_write(0x33,0x4000);
#endif
	} else { // Tantos switch ship
		debug("Tantos switch detected\n");
		ar9_smi_reg_write(0xa1,0x0004); /*port 5 force link up*/
		ar9_smi_reg_write(0xc1,0x0004); /*port 6 force link up*/
		ar9_smi_reg_write(0xf5,0x0BBB); /*port 4 duplex mode, flow control enable,1000Mbit/s*/
										/*port 5 duplex mode, flow control enable, 1000Mbit/s*/
										/*port 6 duplex mode, flow control enable, 1000Mbit/s*/
	}
	asm("sync");

	/*reset GPHY*/
	mdelay(200);
	*AR9_RCU_RST_REQ |= (AR9_RCU_RST_REQ_DMA | AR9_RCU_RST_REQ_PPE) ;
	udelay(50);
	*AR9_GPIO_P2_OUT |= (1<<0);
}

static void ar9_dma_init(void)
{
	/* select port */
	*AR9_DMA_PS = 0;

	/* 
	TXWGT 14:12 rw Port Weight for Transmit Direction (the default value “001”)

	TXENDI 11:10 rw Endianness for Transmit Direction
	Determine a byte swap between memory interface (left hand side) and
	peripheral interface (right hand side).
	00B B0_B1_B2_B3 No byte switching
	01B B1_B0_B3_B2 B0B1B2B3 => B1B0B3B2
	10B B2_B3_B0_B1 B0B1B2B3 => B2B3B0B1
	
	RXENDI 9:8 rw Endianness for Receive Direction
	Determine a byte swap between peripheral (left hand side) and memory
	interface (right hand side).
	00B B0_B1_B2_B3 No byte switching
	01B B1_B0_B3_B2 B0B1B2B3 => B1B0B3B2
	10B B2_B3_B0_B1 B0B1B2B3 => B2B3B0B1
	11B B3_B2_B1_B0 B0B1B2B3 => B3B2B1B0 

	TXBL 5:4 rw Burst Length for Transmit Direction
	Selects burst length for TX direction.
	Others are reserved and will result in 2_WORDS burst length.
	01B 2_WORDS 2 words
	10B 4_WORDS 4 words
	11B 8_WORDS 8 words

	RXBL 3:2 rw Burst Length for Receive Direction
	Selects burst length for RX direction.
	Others are reserved and will result in 2_WORDS burst length.
	01B 2_WORDS 2 words
	10B 4_WORDS 4 words
	11B 8_WORDS 8 words	
	*/
	*AR9_DMA_PCTRL = 0x1f28;
}

#ifdef CONFIG_EXTRA_SWITCH
static int external_switch_init(void)
{
	ar9_sw_chip_init(0, RGMII_MODE);

	ar9_dma_init();

	return 0;
}
#endif /* CONFIG_EXTRA_SWITCH */

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
