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
#include <malloc.h>
#include <devices.h>
#include <version.h>
#include <net.h>
#include <environment.h>
#ifdef CONFIG_DANUBE
#include <asm-mips/danube.h>
#include <configs/danube.h>
#endif
#include "LzmaWrapper.h"

//#define DEBUG_ENABLE_BOOTSTRAP_PRINTF

DECLARE_GLOBAL_DATA_PTR;

#if ( ((CFG_ENV_ADDR+CFG_ENV_SIZE) < BOOTSTRAP_CFG_MONITOR_BASE) || \
      (CFG_ENV_ADDR >= (BOOTSTRAP_CFG_MONITOR_BASE + CFG_MONITOR_LEN)) ) || \
    defined(CFG_ENV_IS_IN_NVRAM)
#define	TOTAL_MALLOC_LEN	(CFG_MALLOC_LEN + CFG_ENV_SIZE)
#else
#define	TOTAL_MALLOC_LEN	CFG_MALLOC_LEN
#endif

#undef DEBUG

#if (CONFIG_COMMANDS & CFG_CMD_NAND)
extern unsigned long nand_init(void);
#endif

#ifdef CONFIG_SERIAL_FLASH
extern int serial_flash_init (void);
#endif

extern int timer_init(void);

extern int incaip_set_cpuclk(void);

extern ulong uboot_end_data_bootstrap;
extern ulong uboot_end_bootstrap;

ulong monitor_flash_len;

const char version_string[] =
	U_BOOT_VERSION" (" __DATE__ " - " __TIME__ ")";

static char *failed = "*** failed ***\n";

/*
 * Begin and End of memory area for malloc(), and current "brk"
 */
static ulong mem_malloc_start;
static ulong mem_malloc_end;
static ulong mem_malloc_brk;


/*
 * The Malloc area is immediately below the monitor copy in DRAM
 */
static void mem_malloc_init (ulong dest_addr)
{
//	ulong dest_addr = BOOTSTRAP_CFG_MONITOR_BASE + gd->reloc_off;

	mem_malloc_end = dest_addr;
	mem_malloc_start = dest_addr - TOTAL_MALLOC_LEN;
	mem_malloc_brk = mem_malloc_start;

	memset ((void *) mem_malloc_start,
		0,
		mem_malloc_end - mem_malloc_start);
}

void *malloc(unsigned int size)
{
	if(size < (mem_malloc_end - mem_malloc_start))
	{
		mem_malloc_start += size;
		return (void *)(mem_malloc_start - size);
	}
	return NULL;
}

void *realloc(void *src,unsigned int size)
{
	return NULL;
}

void free(void *src)
{
	return;
}


void *sbrk (ptrdiff_t increment)
{
	ulong old = mem_malloc_brk;
	ulong new = old + increment;

	if ((new < mem_malloc_start) || (new > mem_malloc_end)) {
		return (NULL);
	}
	mem_malloc_brk = new;
	return ((void *) old);
}


static int init_func_ram (void)
{
#ifdef	CONFIG_BOARD_TYPES
	int board_type = gd->board_type;
#else
	int board_type = 0;	/* use dummy arg */
#endif

#ifdef DEBUG_ENABLE_BOOTSTRAP_PRINTF
	puts ("DRAM:  ");
#endif

	if ((gd->ram_size = initdram (board_type)) > 0) {
#ifdef DEBUG_ENABLE_BOOTSTRAP_PRINTF
		print_size (gd->ram_size, "\n");
#endif
		return (0);
	}
#ifdef DEBUG_ENABLE_BOOTSTRAP_PRINTF
	puts (failed);
#endif
	return (1);
}

static int display_banner(void)
{
#ifdef DEBUG_ENABLE_BOOTSTRAP_PRINTF
	printf ("\n\n%s\n\n", version_string);
#endif
	return (0);
}

static int init_baudrate (void)
{
#if 0
	char tmp[64];	/* long enough for environment variables */
	int i = getenv_r ("baudrate", tmp, sizeof (tmp));

	gd->baudrate = (i > 0)
			? (int) simple_strtoul (tmp, NULL, 10)
			: CONFIG_BAUDRATE;
#endif

	gd->baudrate = CONFIG_BAUDRATE;

	return (0);
}
#ifdef CONFIG_DANUBE
static void init_led(void)
{

  *(unsigned long *)0xBE100B18 |=  0x70;
  *(unsigned long *)0xBE100B1C |=  0x70;
  *(unsigned long *)0xBE100B20 &= ~0x70;
  *(unsigned long *)0xBE100B24 |=  0x70;
#ifdef USE_REFERENCE_BOARD

  *DANUBE_LED_CON1 = 0x00000003;
  *DANUBE_LED_CPU0 = 0x0000010;
  *DANUBE_LED_CPU1 = 0x00000000;
  *DANUBE_LED_AR   = 0x00000000;
  *DANUBE_LED_CON0 = 0x84000000;

#else

  *DANUBE_LED_CON1 = 0x00000007;
  *DANUBE_LED_CPU0 = 0x00001000;
  *DANUBE_LED_CPU1 = 0x00000000;
  *DANUBE_LED_AR   = 0x00000000;
  *DANUBE_LED_CON0 = 0x84000000;

#endif

}
#endif
/*
 * Breath some life into the board...
 *
 * The first part of initialization is running from Flash memory;
 * its main purpose is to initialize the RAM so that we
 * can relocate the monitor code to RAM.
 */

/*
 * All attempts to come up with a "common" initialization sequence
 * that works for all boards and architectures failed: some of the
 * requirements are just _too_ different. To get rid of the resulting
 * mess of board dependend #ifdef'ed code we now make the whole
 * initialization sequence configurable to the user.
 *
 * The requirements for any new initalization function is simple: it
 * receives a pointer to the "global data" structure as it's only
 * argument, and returns an integer return code, where 0 means
 * "continue" and != 0 means "fatal error, hang the system".
 */
typedef int (init_fnc_t) (void);

init_fnc_t *init_sequence[] = {
	//timer_init,
	//env_init,		/* initialize environment */
#ifdef CONFIG_INCA_IP
	incaip_set_cpuclk,	/* set cpu clock according to environment variable */
#endif
#ifdef DEBUG_ENABLE_BOOTSTRAP_PRINTF
	init_baudrate,		/* initialze baudrate settings */
	serial_init,		/* serial communications setup */
	console_init_f,
	display_banner,		/* say that we are here */
#endif
	init_func_ram,
	//checkboard,
	NULL,
};


void bootstrap_board_init_f(ulong bootflag)
{
	gd_t gd_data, *id;
	bd_t *bd;
	init_fnc_t **init_fnc_ptr;
	ulong addr, addr_sp, len = (ulong)&uboot_end_bootstrap - BOOTSTRAP_CFG_MONITOR_BASE;
	ulong *s;
	ulong lzmaImageaddr  = 0;
#ifdef CONFIG_PURPLE
	void copy_code (ulong);
#endif

	/* Pointer is writable since we allocated a register for it.
	 */
	gd = &gd_data;
	/* compiler optimization barrier needed for GCC >= 3.4 */
	__asm__ __volatile__("": : :"memory");

	memset ((void *)gd, 0, sizeof (gd_t));

	for (init_fnc_ptr = init_sequence; *init_fnc_ptr; ++init_fnc_ptr) {
		if ((*init_fnc_ptr)() != 0) {
			hang ();
		}
	}

	/*
	 * Now that we have DRAM mapped and working, we can
	 * relocate the code and continue running from DRAM.
	 */
	addr = CFG_SDRAM_BASE + gd->ram_size;

	/* We can reserve some RAM "on top" here.
	 */

	/* round down to next 4 kB limit.
	 */
	addr &= ~(4096 - 1);
	debug ("Top of RAM usable for U-Boot at: %08lx\n", addr);

	/* Reserve memory for U-Boot code, data & bss
	 * round down to next 16 kB limit
	 */
	addr -= len;
	addr &= ~(16 * 1024 - 1);

	debug ("Reserving %ldk for U-Boot at: %08lx\n", len >> 10, addr);

	 /* Reserve memory for malloc() arena.
	 */
	addr_sp = addr - TOTAL_MALLOC_LEN;
	debug ("Reserving %dk for malloc() at: %08lx\n",
			TOTAL_MALLOC_LEN >> 10, addr_sp);

	/*
	 * (permanently) allocate a Board Info struct
	 * and a permanent copy of the "global" data
	 */
	addr_sp -= sizeof(bd_t);
	bd = (bd_t *)addr_sp;
	gd->bd = bd;
	debug ("Reserving %d Bytes for Board Info at: %08lx\n",
			sizeof(bd_t), addr_sp);

	addr_sp -= sizeof(gd_t);
	id = (gd_t *)addr_sp;
	debug ("Reserving %d Bytes for Global Data at: %08lx\n",
			sizeof (gd_t), addr_sp);

 	/* Reserve memory for boot params.
	 */
	addr_sp -= CFG_BOOTPARAMS_LEN;
	bd->bi_boot_params = addr_sp;
	debug ("Reserving %dk for boot params() at: %08lx\n",
			CFG_BOOTPARAMS_LEN >> 10, addr_sp);

	/*
	 * Finally, we set up a new (bigger) stack.
	 *
	 * Leave some safety gap for SP, force alignment on 16 byte boundary
	 * Clear initial stack frame
	 */
	addr_sp -= 16;
	addr_sp &= ~0xF;
	s = (ulong *)addr_sp;
	*s-- = 0;
	*s-- = 0;
	addr_sp = (ulong)s;
	debug ("Stack Pointer at: %08lx\n", addr_sp);

	/*
	 * Save local variables to board info struct
	 */
	bd->bi_memstart	= CFG_SDRAM_BASE;	/* start of  DRAM memory */
	bd->bi_memsize	= gd->ram_size;		/* size  of  DRAM memory in bytes */
	bd->bi_baudrate	= gd->baudrate;		/* Console Baudrate */

	memcpy (id, (void *)gd, sizeof (gd_t));

	/* On the purple board we copy the code in a special way
	 * in order to solve flash problems
	 */
#ifdef CONFIG_PURPLE
	copy_code(addr);
#endif

	lzmaImageaddr = (ulong)&uboot_end_data_bootstrap;

#ifdef DEBUG_ENABLE_BOOTSTRAP_PRINTF
	printf ("\n relocating to address %08x ", addr);
#endif

	bootstrap_relocate_code (addr_sp, id, addr);

	/* NOTREACHED - relocate_code() does not return */
}
/************************************************************************
 *
 * This is the next part if the initialization sequence: we are now
 * running from RAM and have a "normal" C environment, i. e. global
 * data can be written, BSS has been cleared, the stack size in not
 * that critical any more, etc.
 *
 ************************************************************************
 */
#define CONFIG_LZMA 

void bootstrap_board_init_r (gd_t *id, ulong dest_addr)
{
	int i;

	ulong	addr;
	ulong	data, len, checksum;
	ulong  *len_ptr;
	image_header_t header;
	image_header_t *hdr = &header;
	unsigned int destLen;
	int (*fn)();

#if 1
#endif



	/* initialize malloc() area */
	mem_malloc_init(dest_addr);

#ifdef DEBUG_ENABLE_BOOTSTRAP_PRINTF
	printf("\n Compressed Image at %08x \n ", (BOOTSTRAP_CFG_MONITOR_BASE + ((ulong)&uboot_end_data_bootstrap - dest_addr)));
#endif
	
	addr = (char *)(BOOTSTRAP_CFG_MONITOR_BASE + ((ulong)&uboot_end_data_bootstrap - dest_addr));
	memmove (&header, (char *)addr, sizeof(image_header_t));

	if (ntohl(hdr->ih_magic) != IH_MAGIC) {
#ifdef DEBUG_ENABLE_BOOTSTRAP_PRINTF
		printf ("Bad Magic Number at address 0x%08lx\n",addr);
#endif
		return;
	}

	data = (ulong)&header;
	len  = sizeof(image_header_t);

	checksum = ntohl(hdr->ih_hcrc);
	hdr->ih_hcrc = 0;
	if (crc32 (0, (char *)data, len) != checksum) {
#ifdef DEBUG_ENABLE_BOOTSTRAP_PRINTF
		printf ("Bad Header Checksum\n");
#endif
		return;
	}
	
	data = addr + sizeof(image_header_t);
	len  = ntohl(hdr->ih_size);

	len_ptr = (ulong *)data;

    

#ifdef DEBUG_ENABLE_BOOTSTRAP_PRINTF
	printf ("Disabling all the interrupts\n");
#endif
	disable_interrupts();
#ifdef DEBUG_ENABLE_BOOTSTRAP_PRINTF
	printf ("   Uncompressing UBoot Image ... \n" );
#endif

	/*
	 * If we've got less than 4 MB of malloc() space,
	 * use slower decompression algorithm which requires
	 * at most 2300 KB of memory.
	 */
	destLen = 0x0;

#ifdef CONFIG_BZIP2
	i = BZ2_bzBuffToBuffDecompress ((char*)ntohl(hdr->ih_load),
					0x400000, (char *)data, len,
					CFG_MALLOC_LEN < (4096 * 1024), 0);
	if (i != BZ_OK) {
#ifdef DEBUG_ENABLE_BOOTSTRAP_PRINTF
			printf ("BUNZIP2 ERROR %d - must RESET board to recover\n", i);
#endif
			return;
	}
#endif /* CONFIG_BZIP2 */
	
#ifdef CONFIG_MICROBZIP2 
	i = micro_bzBuffToBuffDecompress ((char*)ntohl(hdr->ih_load),
					&destLen, (char *)data, len,
					CFG_MALLOC_LEN < (4096 * 1024), 0);
	if (i != RETVAL_OK) {
#ifdef DEBUG_ENABLE_BOOTSTRAP_PRINTF
		printf ("MICRO_BUNZIP2 ERROR %d - must RESET board to recover\n", i);
#endif
		//do_reset (cmdtp, flag, argc, argv);
		return;
	}
#endif
	
#ifdef CONFIG_LZMA 
#if 0
	i = lzmaBuffToBuffDecompress ((char*)ntohl(hdr->ih_load),
					&destLen, (char *)data, len);
#endif

	i = lzma_inflate ((unsigned char *)data, len, (unsigned char*)ntohl(hdr->ih_load), &destLen);
	if (i != LZMA_RESULT_OK) {
#ifdef DEBUG_ENABLE_BOOTSTRAP_PRINTF
		printf ("LZMA ERROR %d - must RESET board to recover\n", i);
#endif
		//do_reset (cmdtp, flag, argc, argv);
		return;
	}
#endif

#ifdef DEBUG_ENABLE_BOOTSTRAP_PRINTF
	printf ("   Uncompression completed successfully with destLen %d\n ",destLen );
#endif

    fn = ntohl(hdr->ih_load);

	(*fn)();

	hang ();

}

void hang (void)
{

#ifdef DEBUG_ENABLE_BOOTSTRAP_PRINTF
	puts ("### ERROR ### Please RESET the board ###\n");
#endif
	for (;;);
}
