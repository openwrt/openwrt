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
//joelin 10/07/2004 for MXIC MX29LV320ABTC-90
#include <common.h>
#include <asm/danube.h>

/*
#ifdef CONFIG_AMAZON
	#define FLASH_DELAY 	{int i; \
				for(i=0;i<800;i++) \
					*((volatile u32 *)CFG_SDRAM_BASE_UNCACHE); \
				}
#else
	#define FLASH_DELAY
#endif
*/	

flash_info_t	flash_info[CFG_MAX_FLASH_BANKS]; /* info for FLASH chips	*/

/* NOTE - CONFIG_FLASH_16BIT means the CPU interface is 16-bit, it
 *        has nothing to do with the flash chip being 8-bit or 16-bit.
 */
#ifdef CONFIG_FLASH_16BIT
typedef unsigned short FLASH_PORT_WIDTH;
typedef volatile unsigned short FLASH_PORT_WIDTHV;
#define	FLASH_ID_MASK	0xFFFF
#else
typedef unsigned long FLASH_PORT_WIDTH;
typedef volatile unsigned long FLASH_PORT_WIDTHV;
#define	FLASH_ID_MASK	0xFFFFFFFF
#endif

#define FPW	FLASH_PORT_WIDTH
#define FPWV	FLASH_PORT_WIDTHV

#define ORMASK(size) ((-size) & OR_AM_MSK)	// 0xffff8000

#if 0
#define FLASH_CYCLE1	0x0555
#define FLASH_CYCLE2	0x02aa
#else
#define FLASH_CYCLE1	0x0554			//joelin for MX29LV320AT/B  0x0555 
#define FLASH_CYCLE2	0x02ab			//joelin for MX29LV320AT/B  0x02aa 
#endif

/*-----------------------------------------------------------------------
 * Functions
 */
static ulong flash_get_size(FPWV *addr, flash_info_t *info);
static void flash_reset(flash_info_t *info);
static int write_word_intel(flash_info_t *info, FPWV *dest, FPW data);
static int write_word_amd(flash_info_t *info, FPWV *dest, FPW data);
static void flash_get_offsets(ulong base, flash_info_t *info);
static flash_info_t *flash_get_info(ulong base);

/*-----------------------------------------------------------------------
 * flash_init()
 *
 * sets up flash_info and returns size of FLASH (bytes)
 */
unsigned long flash_init (void)
{
	unsigned long size = 0;
	int i;

	/* Init: no FLASHes known */
	for (i=0; i < CFG_MAX_FLASH_BANKS; ++i) {         // 1 bank 
		ulong flashbase = (i == 0) ? PHYS_FLASH_1 : PHYS_FLASH_2;      // 0xb0000000,  0xb4000000

       volatile ulong * buscon = (ulong *)
			((i == 0) ? DANUBE_EBU_BUSCON0 : DANUBE_EBU_BUSCON1);

		/* Disable write protection */
//		*buscon &= ~AMAZON_EBU_BUSCON0_WRDIS;
		/* Enable write protection */
		*buscon |= DANUBE_EBU_BUSCON0_WRDIS;

#if 1
		memset(&flash_info[i], 0, sizeof(flash_info_t));
#endif

		flash_info[i].size = 
			flash_get_size((FPW *)flashbase, &flash_info[i]);

		if (flash_info[i].flash_id == FLASH_UNKNOWN) {
			printf ("## Unknown FLASH on Bank %d - Size = 0x%08lx\n",
			i, flash_info[i].size);
		}
		
		size += flash_info[i].size;
	}

#if CFG_MONITOR_BASE >= CFG_FLASH_BASE    // TEXT_BASE >= 0xB3000000
	/* monitor protection ON by default */  /* only use software protection, info->protect[i]=0/1 */
/*	flash_protect(FLAG_PROTECT_SET,
		      CFG_MONITOR_BASE,
		      CFG_MONITOR_BASE+CFG_MONITOR_LEN-1,
		      flash_get_info(CFG_MONITOR_BASE));
*/
	flash_protect(FLAG_PROTECT_CLEAR,    // clear protect
		      CFG_MONITOR_BASE,
		      CFG_MONITOR_BASE+CFG_MONITOR_LEN-1,
		      flash_get_info(CFG_MONITOR_BASE));

#endif

#ifdef	CFG_ENV_IS_IN_FLASH     /* 1 */
	/* ENV protection ON by default */
/*	flash_protect(FLAG_PROTECT_SET,
		      CFG_ENV_ADDR,
		      CFG_ENV_ADDR+CFG_ENV_SIZE-1,
		      flash_get_info(CFG_ENV_ADDR));
*/
	flash_protect(FLAG_PROTECT_CLEAR,
		      CFG_ENV_ADDR,
		      CFG_ENV_ADDR+CFG_ENV_SIZE-1,
		      flash_get_info(CFG_ENV_ADDR));

#endif


	return size;
}

/*-----------------------------------------------------------------------
 */
static void flash_reset(flash_info_t *info)
{
	FPWV *base = (FPWV *)(info->start[0]);

	(*DANUBE_EBU_BUSCON0)&=(~0x80000000);	// enable writing
	(*DANUBE_EBU_BUSCON1)&=(~0x80000000);	// enable writing
	(*EBU_NAND_CON)=0;	
	/* Put FLASH back in read mode */
	if ((info->flash_id & FLASH_VENDMASK) == FLASH_MAN_INTEL){
		*base = (FPW)0x00FF00FF;	/* Intel Read Mode */
		asm("SYNC");
	}
	else if ((info->flash_id & FLASH_VENDMASK) == FLASH_MAN_AMD){
		*base = (FPW)0x00F000F0;	/* AMD Read Mode */
		asm("SYNC");			//joelin
	}
	else if ((info->flash_id & FLASH_VENDMASK) == FLASH_MAN_MX){
		*base = (FPW)0x00F000F0;	/* MXIC Read Mode */
		asm("SYNC");			//joelin
	}		

	(*DANUBE_EBU_BUSCON0)|=0x80000000;	// disable writing
	(*DANUBE_EBU_BUSCON1)|=0x80000000;	// disable writing

}

/*-----------------------------------------------------------------------
 */
static void flash_get_offsets (ulong base, flash_info_t *info)
{
	int i;

	/* set up sector start address table */
	if ((info->flash_id & FLASH_VENDMASK) == FLASH_MAN_INTEL
	    && (info->flash_id & FLASH_BTYPE)) {
		int bootsect_size;	/* number of bytes/boot sector	*/
		int sect_size;		/* number of bytes/regular sector */

		bootsect_size = 0x00002000 * (sizeof(FPW)/2);
		sect_size =     0x00010000 * (sizeof(FPW)/2);

		/* set sector offsets for bottom boot block type	*/
		for (i = 0; i < 8; ++i) {
			info->start[i] = base + (i * bootsect_size);
		}
		for (i = 8; i < info->sector_count; i++) {
			info->start[i] = base + ((i - 7) * sect_size);
		}
	}
	else if ((info->flash_id & FLASH_VENDMASK) == FLASH_MAN_AMD
		 && (info->flash_id & FLASH_TYPEMASK) == FLASH_AM640U) {

		int sect_size;		/* number of bytes/sector */

		sect_size = 0x00010000 * (sizeof(FPW)/2);

		/* set up sector start address table (uniform sector type) */
		for( i = 0; i < info->sector_count; i++ )
			info->start[i] = base + (i * sect_size);
	}
	else if(((info->flash_id & FLASH_VENDMASK) == FLASH_MAN_INTEL)
		&& ((info->flash_id & FLASH_TYPEMASK)==FLASH_28F128J3A)){
		int sect_size;
		sect_size = 0x20000;
		for(i=0;i < info->sector_count; i++)
			info->start[i]= base + (i*sect_size);
	}
	else if(((info->flash_id & FLASH_VENDMASK) == FLASH_MAN_INTEL)
		&& ((info->flash_id & FLASH_TYPEMASK)==FLASH_28F320J3A)){
		int sect_size;
		sect_size = 0x20000;
		for(i=0;i < info->sector_count; i++)
			info->start[i]= base + (i*sect_size);
	}
//joelin add for MX29LV320AB-- SA0~SA7:sector size=8K bytes ,SA9~SA70 :sector size=64k bytes	
	else if(((info->flash_id & FLASH_VENDMASK) == FLASH_MAN_MX)
		&& ((info->flash_id & FLASH_TYPEMASK)==FLASH_29LV320AB)){
		int bootsect_size;	/* number of bytes/boot sector	*/
		int sect_size;		/* number of bytes/regular sector */

		bootsect_size = 0x00002000 * (sizeof(FPW)/2);
		sect_size =     0x00010000 * (sizeof(FPW)/2);

		/* set sector offsets for bottom boot block type	*/
		for (i = 0; i < 8; ++i) {
			info->start[i] = base + (i * bootsect_size);
		}
		for (i = 8; i < info->sector_count; i++) {
			info->start[i] = base + ((i - 7) * sect_size);
		}
	}	
//joelin add for MX29LV160BB-- SA0=16K,SA1,SA2=8K,SA3=32K bytes ,SA4~SA34 :sector size=64k bytes			
	else if(((info->flash_id & FLASH_VENDMASK) == FLASH_MAN_MX)
		&& ((info->flash_id & FLASH_TYPEMASK)==FLASH_29LV160BB)){
		int bootsect_size;	/* number of bytes/boot sector	*/
		int sect_size;		/* number of bytes/regular sector */

		bootsect_size = 0x00002000 * (sizeof(FPW)/2);
		sect_size =     0x00010000 * (sizeof(FPW)/2);
/* set sector offsets for bottom boot block type	*/		
//MX29LV160BB
		info->start[0] = base ;				//SA0=16K bytes
		info->start[1] = info->start[0]  + (1 * 0x00004000 * (sizeof(FPW)/2)); //SA1=8K bytes
		info->start[2] = info->start[1]  + (1 * 0x00002000 * (sizeof(FPW)/2)); //SA2=8K bytes
		info->start[3] = info->start[2]  + (1 * 0x00002000 * (sizeof(FPW)/2)); //SA3=32K bytes

		for (i = 4; i < info->sector_count; i++) {
			info->start[i] = base + ((i - 3) * sect_size);
		}		
	}	
//liupeng add for MX29LV640BB-- SA0~SA7:sector size=8k bytes ,SA8~SA134 :sector size=64k bytes	
	else if(((info->flash_id & FLASH_VENDMASK) == FLASH_MAN_MX)
		&& ((info->flash_id & FLASH_TYPEMASK)==FLASH_29LV640BB)){
		int bootsect_size;	/* number of bytes/boot sector	*/
		int sect_size;		/* number of bytes/regular sector */

		bootsect_size = 0x00002000 * (sizeof(FPW)/2);
		sect_size =     0x00010000 * (sizeof(FPW)/2);

		/* set sector offsets for bottom boot block type	*/
		for (i = 0; i < 8; ++i) {
			info->start[i] = base + (i * bootsect_size);
		}
		for (i = 8; i < info->sector_count; i++) {
			info->start[i] = base + ((i - 7) * sect_size);
		}
	}	
	else{
		printf("flash get offsets fail\n");
	}
}

/*-----------------------------------------------------------------------
 */

static flash_info_t *flash_get_info(ulong base)
{
	int i;
	flash_info_t * info;
	
	for (i = 0; i < CFG_MAX_FLASH_BANKS; i ++) {
		info = & flash_info[i];
		if (info->start[0] <= base && base < info->start[0] + info->size)
			break;
	}
	
	return i == CFG_MAX_FLASH_BANKS ? 0 : info;
}

/*-----------------------------------------------------------------------
 */

void flash_print_info (flash_info_t *info)
{
	int i;
	uchar *boottype;
	uchar *bootletter;
	uchar *fmt;
	uchar botbootletter[] = "B";
	uchar topbootletter[] = "T";
	uchar botboottype[] = "bottom boot sector";
	uchar topboottype[] = "top boot sector";

	if (info->flash_id == FLASH_UNKNOWN) {
		printf ("missing or unknown FLASH type\n");
		return;
	}

	switch (info->flash_id & FLASH_VENDMASK) {
	case FLASH_MAN_AMD:	printf ("AMD ");		break;
	case FLASH_MAN_BM:	printf ("BRIGHT MICRO ");	break;
	case FLASH_MAN_FUJ:	printf ("FUJITSU ");		break;
	case FLASH_MAN_SST:	printf ("SST ");		break;
	case FLASH_MAN_STM:	printf ("STM ");		break;
	case FLASH_MAN_INTEL:	printf ("INTEL ");		break;
	case FLASH_MAN_MX:	printf ("MXIC  ");		break;	
	default:		printf ("Unknown Vendor ");	break;
	}

	/* check for top or bottom boot, if it applies */
	if (info->flash_id & FLASH_BTYPE) {
		boottype = botboottype;
		bootletter = botbootletter;
	}
	else {
		boottype = topboottype;
		bootletter = topbootletter;
	}

	switch (info->flash_id & FLASH_TYPEMASK) {
	case FLASH_AM640U:
		fmt = "29LV641D (64 Mbit, uniform sectors)\n";
		break;
        case FLASH_28F800C3B:
        case FLASH_28F800C3T:
		fmt = "28F800C3%s (8 Mbit, %s)\n";
		break;
	case FLASH_INTEL800B:
	case FLASH_INTEL800T:
		fmt = "28F800B3%s (8 Mbit, %s)\n";
		break;
        case FLASH_28F160C3B:
        case FLASH_28F160C3T:
		fmt = "28F160C3%s (16 Mbit, %s)\n";
		break;
	case FLASH_INTEL160B:
	case FLASH_INTEL160T:
		fmt = "28F160B3%s (16 Mbit, %s)\n";
		break;
        case FLASH_28F320C3B:
        case FLASH_28F320C3T:
		fmt = "28F320C3%s (32 Mbit, %s)\n";
		break;
	case FLASH_INTEL320B:
	case FLASH_INTEL320T:
		fmt = "28F320B3%s (32 Mbit, %s)\n";
		break;
        case FLASH_28F640C3B:
        case FLASH_28F640C3T:
		fmt = "28F640C3%s (64 Mbit, %s)\n";
		break;
	case FLASH_INTEL640B:
	case FLASH_INTEL640T:
		fmt = "28F640B3%s (64 Mbit, %s)\n";
		break;
	case FLASH_28F128J3A:
		fmt = "28F128J3A (128 Mbit, 128 uniform sectors)\n";
		break;
	case FLASH_28F320J3A:
		fmt = "28F320J3A (32 Mbit, 32 uniform sectors)\n";
		break;
	case FLASH_29LV640BB:		//liupeng for MXIC FLASH_29LV640BB
		fmt = "29LV640BB (64 Mbit, boot sector SA0~SA126 size 64k bytes,other sectors SA127~SA135 size 8k bytes)\n";
		break;	
	case FLASH_29LV320AB:		//joelin for MXIC FLASH_29LV320AB
		fmt = "29LV320AB (32 Mbit, boot sector SA0~SA7 size 8K bytes,other sectors SA8~SA70 size 64K bytes)\n";
		break;	
	case FLASH_29LV160BB:		//joelin for MXIC FLASH_29LV160BB
		fmt = "29LV160BB (16 Mbit, boot sector SA0 size 16K bytes,SA1,SA2 size 8K bytes,SA3 size 32k bytes,other sectors SA4~SA34 size 64K bytes)\n";
		break;					
	default:
		fmt = "Unknown Chip Type\n";
		break;
	}

	printf (fmt, bootletter, boottype);

	printf ("  Size: %ld MB in %d Sectors\n",
		info->size >> 20,
		info->sector_count);

	printf ("  Sector Start Addresses:");

	for (i=0; i<info->sector_count; ++i) {
		if ((i % 5) == 0) {
			printf ("\n   ");
		}

		printf (" %08lX%s", info->start[i],
			info->protect[i] ? " (RO)" : "     ");
	}

	printf ("\n");
}

/*-----------------------------------------------------------------------
 */

/*
 * The following code cannot be run from FLASH!
 */

ulong flash_get_size (FPWV *addr, flash_info_t *info)
{
        (*DANUBE_EBU_BUSCON0)=0x1d7ff;  //value from Aikann, should be used on the real chip
	(*EBU_ADDR_SEL_0) = 0x10000031; //starting address from 0xb0000000
	(*EBU_NAND_CON)=0;
	(*DANUBE_EBU_BUSCON0)&=(~0x80000000);	// enable writing
	(*DANUBE_EBU_BUSCON1)&=(~0x80000000);	// enable writing
	/* Write auto select command: read Manufacturer ID */

	/* Write auto select command sequence and test FLASH answer */
  	addr[FLASH_CYCLE1] = (FPW)0x00AA00AA;	/* for AMD, Intel ignores this */
  	asm("SYNC");	
  	addr[FLASH_CYCLE2] = (FPW)0x00550055;	/* for AMD, Intel ignores this */
  	asm("SYNC");
  	addr[FLASH_CYCLE1] = (FPW)0x00900090;	/* selects Intel or AMD */
  	asm("SYNC");
	
	/* The manufacturer codes are only 1 byte, so just use 1 byte.
	 * This works for any bus width and any FLASH device width.
	 */
 
//	printf("\n type is %08lx", addr[1] & 0xff); 	//joelin 10/06/2004 flash type
//	printf("\n type is %08lx", addr[0] & 0xff); 	//joelin 10/06/2004 flash type
//		asm("SYNC");	 
	switch (addr[1] & 0xff) {
	case (uchar)AMD_MANUFACT:
		info->flash_id = FLASH_MAN_AMD;
		break;

	case (uchar)INTEL_MANUFACT:			// 0x0089
		info->flash_id = FLASH_MAN_INTEL; //0x00300000
		break;
		
//joelin for MXIC		
	case (uchar)MX_MANUFACT:		// 0x00c2
		info->flash_id = FLASH_MAN_MX ;//0x00030000
		break;
		
	default:
		info->flash_id = FLASH_UNKNOWN;
		info->sector_count = 0;
		info->size = 0;
		break;
/*	default:
		info->flash_id = FLASH_MAN_INTEL; //0x00300000
		break;*/
	}

	/* Check 16 bits or 32 bits of ID so work on 32 or 16 bit bus. */
	if (info->flash_id != FLASH_UNKNOWN) switch (addr[0]) {
	case (FPW)AMD_ID_LV640U:	/* 29LV640 and 29LV641 have same ID */
		info->flash_id += FLASH_AM640U;
		info->sector_count = 128;
		info->size = 0x00800000 * (sizeof(FPW)/2);
		break;				/* => 8 or 16 MB	*/

	case (FPW)INTEL_ID_28F800C3B:
		info->flash_id += FLASH_28F800C3B;
		info->sector_count = 23;
		info->size = 0x00100000 * (sizeof(FPW)/2);
		break;				/* => 1 or 2 MB		*/

	case (FPW)INTEL_ID_28F800B3B:
		info->flash_id += FLASH_INTEL800B;
		info->sector_count = 23;
		info->size = 0x00100000 * (sizeof(FPW)/2);
		break;				/* => 1 or 2 MB		*/

	case (FPW)INTEL_ID_28F160C3B:
		info->flash_id += FLASH_28F160C3B;
		info->sector_count = 39;
		info->size = 0x00200000 * (sizeof(FPW)/2);
		break;				/* => 2 or 4 MB		*/

	case (FPW)INTEL_ID_28F160B3B:
		info->flash_id += FLASH_INTEL160B;
		info->sector_count = 39;
		info->size = 0x00200000 * (sizeof(FPW)/2);
		break;				/* => 2 or 4 MB		*/

	case (FPW)INTEL_ID_28F320C3B:
		info->flash_id += FLASH_28F320C3B;
		info->sector_count = 71;
		info->size = 0x00400000 * (sizeof(FPW)/2);
		break;				/* => 4 or 8 MB		*/

	case (FPW)INTEL_ID_28F320B3B:
		info->flash_id += FLASH_INTEL320B;
		info->sector_count = 71;
		info->size = 0x00400000 * (sizeof(FPW)/2);
		break;				/* => 4 or 8 MB		*/

	case (FPW)INTEL_ID_28F640C3B:
		info->flash_id += FLASH_28F640C3B;
		info->sector_count = 135;
		info->size = 0x00800000 * (sizeof(FPW)/2);
		break;				/* => 8 or 16 MB	*/

	case (FPW)INTEL_ID_28F640B3B:
		info->flash_id += FLASH_INTEL640B;
		info->sector_count = 135;
		info->size = 0x00800000 * (sizeof(FPW)/2);
		break;				/* => 8 or 16 MB	*/
	
	case (FPW)INTEL_ID_28F128J3A:
		info->flash_id +=FLASH_28F128J3A;
		info->sector_count = 128;
		info->size = 0x01000000 * (sizeof(FPW)/2);
		break;				/* => 16 MB */
	case (FPW)INTEL_ID_28F320J3A:
		info->flash_id += FLASH_28F320J3A;
		info->sector_count = 32;
		info->size = 0x00400000 * (sizeof(FPW)/2);
		break;	
//joelin for MXIC
	case (FPW)MX_ID_29LV320AB:
		info->flash_id += FLASH_29LV320AB;
		info->sector_count = 71;
		info->size = 0x00400000 * (sizeof(FPW)/2);
		break;				/* => 4 MB		*/		
					/* => 4 MB */
//joelin for MXIC
	case (FPW)MX_ID_29LV160BB:
		info->flash_id += FLASH_29LV160BB;
		info->sector_count = 35;
		info->size = 0x00200000 * (sizeof(FPW)/2);
		break;				/* => 2 MB		*/		
					/* => 2 MB */					
	/* liupeng*/
	case (FPW)MX_ID_29LV640BB:
		info->flash_id += FLASH_29LV640BB;
		info->sector_count = 135;
		info->size = 0x00800000 * (sizeof(FPW)/2);
		break;				/* => 2 MB		*/		
	default:
		info->flash_id = FLASH_UNKNOWN;
		info->sector_count = 0;
		info->size = 0;
		return (0);			/* => no or unknown flash */
/*	default:
		info->flash_id += FLASH_28F320J3A;
		info->sector_count = 32;
		info->size = 0x00400000 * (sizeof(FPW)/2);
		break;*/
	}


	(*DANUBE_EBU_BUSCON0)|=0x80000000;	// disable writing
	(*DANUBE_EBU_BUSCON1)|=0x80000000;	// disable writing
	
	flash_get_offsets((ulong)addr, info);

	/* Put FLASH back in read mode */
	flash_reset(info);
	
	return (info->size);
}

/*-----------------------------------------------------------------------
 */

int	flash_erase (flash_info_t *info, int s_first, int s_last)
{
	FPWV *addr;
	int flag, prot, sect;
	int intel = (info->flash_id & FLASH_VENDMASK) == FLASH_MAN_INTEL;
	ulong start, now, last;
	int rcode = 0;
	if ((s_first < 0) || (s_first > s_last)) {
		if (info->flash_id == FLASH_UNKNOWN) {
			printf ("- missing\n");
		} else {
			printf ("- no sectors to erase\n");
		}
		return 1;
	}

	switch (info->flash_id & FLASH_TYPEMASK) {
	case FLASH_INTEL800B:
	case FLASH_INTEL160B:
	case FLASH_INTEL320B:
	case FLASH_INTEL640B:
	case FLASH_28F800C3B:
	case FLASH_28F160C3B:
	case FLASH_28F320C3B:
	case FLASH_28F640C3B:
	case FLASH_28F128J3A:
	case FLASH_28F320J3A:
	case FLASH_AM640U:
	case FLASH_29LV640BB:	//liupeng for MXIC MX29LV640BB
	case FLASH_29LV320AB:	//joelin for MXIC MX29LV320AB
	case FLASH_29LV160BB:	//joelin for MXIC MX29LV160BB
		break;
	case FLASH_UNKNOWN:
	default:
		printf ("Can't erase unknown flash type %08lx - aborted\n",
			info->flash_id);
		return 1;
	}

	prot = 0;
	for (sect=s_first; sect<=s_last; ++sect) {
		if (info->protect[sect]) {
			prot++;
		}
	}

	if (prot) {
		printf ("- Warning: %d protected sectors will not be erased!\n",
			prot);
	} else {
		printf ("\n");
	}

	last  = get_timer(0);

	/* Start erase on unprotected sectors */
	for (sect = s_first; sect<=s_last && rcode == 0; sect++) {

		if (info->protect[sect] != 0)	/* protected, skip it */
			continue;

		/* Disable interrupts which might cause a timeout here */
		flag = disable_interrupts();
		
		(*DANUBE_EBU_BUSCON0)&=(~0x80000000);	// enable writing
		(*DANUBE_EBU_BUSCON1)&=(~0x80000000);	// enable writing
		(*EBU_NAND_CON)=0;
		addr = (FPWV *)(info->start[sect]);
		if (intel) {
			*addr = (FPW)0x00500050; /* clear status register */
			*addr = (FPW)0x00200020; /* erase setup */
			*addr = (FPW)0x00D000D0; /* erase confirm */
			asm("SYNC");
		}
		else {
			/* must be AMD style if not Intel */
			FPWV *base;		/* first address in bank */

			base = (FPWV *)(info->start[0]);
			base[FLASH_CYCLE1] = (FPW)0x00AA00AA;	/* unlock */
			base[FLASH_CYCLE2] = (FPW)0x00550055;	/* unlock */
			base[FLASH_CYCLE1] = (FPW)0x00800080;	/* erase mode */
			base[FLASH_CYCLE1] = (FPW)0x00AA00AA;	/* unlock */
			base[FLASH_CYCLE2] = (FPW)0x00550055;	/* unlock */
			*addr = (FPW)0x00300030;	/* erase sector */
		}

		/* re-enable interrupts if necessary */
		if (flag)
			enable_interrupts();

		start = get_timer(0);

		/* wait at least 50us for AMD, 80us for Intel.
		 * Let's wait 1 ms.
		 */
		udelay (1000);

		while ((*addr & (FPW)0x00800080) != (FPW)0x00800080) {
			if ((now = get_timer(start)) > CFG_FLASH_ERASE_TOUT) {
				printf ("Erase Timeout\n");

				if (intel) {
					/* suspend erase	*/
					*addr = (FPW)0x00B000B0;
				}

				flash_reset(info);	/* reset to read mode */
				rcode = 1;		/* failed */
				break;
			}

			/* show that we're waiting */
			if ((get_timer(last)) > CFG_HZ) {/* every second */
				putc ('.');
				last = get_timer(0);
			}
		}
		
			
//joelin for MXIC 
	switch (info->flash_id & FLASH_VENDMASK) {
 	case FLASH_MAN_MX:		//joelin for MXIC	
 		break;
 	default:
		if((*addr & (FPW)0x00200020) != (FPW)0x0)
			printf("Erase Error\n");
		break;
	}			
			
			

		/* show that we're waiting */
		if ((get_timer(last)) > CFG_HZ) {	/* every second */
			putc ('.');
			last = get_timer(0);
		}

		//flash_reset(info);	/* reset to read mode	*/
	}

	(*DANUBE_EBU_BUSCON0)|=0x80000000;	// disable writing
	(*DANUBE_EBU_BUSCON1)|=0x80000000;	// disable writing

	flash_reset(info);	/* Homebox Black with JS28F128J3D75 had trouble reading after erase */

	printf (" done\n");
	return rcode;
}

/*-----------------------------------------------------------------------
 * Copy memory to flash, returns:
 * 0 - OK
 * 1 - write timeout
 * 2 - Flash not erased
 */
int write_buff (flash_info_t *info, uchar *src, ulong addr, ulong cnt)
{
    FPW data = 0; /* 16 or 32 bit word, matches flash bus width on MPC8XX */
    int bytes;	  /* number of bytes to program in current word		*/
    int left;	  /* number of bytes left to program			*/
    int i, res;

    for (left = cnt, res = 0;
	 left > 0 && res == 0;
	 addr += sizeof(data), left -= sizeof(data) - bytes) {

        bytes = addr & (sizeof(data) - 1);
        addr &= ~(sizeof(data) - 1);

	/* combine source and destination data so can program
	 * an entire word of 16 or 32 bits
	 */
        for (i = 0; i < sizeof(data); i++) {
            data <<= 8;
            if (i < bytes || i - bytes >= left )
		data += *((uchar *)addr + i);
	    else
		data += *src++;
	}

	/* write one word to the flash */
	switch (info->flash_id & FLASH_VENDMASK) {
	case FLASH_MAN_AMD:
	case FLASH_MAN_MX:		//joelin for MXIC	
		res = write_word_amd(info, (FPWV *)addr, data);
		break;
	case FLASH_MAN_INTEL:
		res = write_word_intel(info, (FPWV *)addr, data);
		break;
	default:
		/* unknown flash type, error! */
		printf ("missing or unknown FLASH type\n");
		res = 1;	/* not really a timeout, but gives error */
		break;
	}
    }

    return (res);
}

/*-----------------------------------------------------------------------
 * Write a word to Flash for AMD FLASH
 * A word is 16 or 32 bits, whichever the bus width of the flash bank
 * (not an individual chip) is.
 *
 * returns:
 * 0 - OK
 * 1 - write timeout
 * 2 - Flash not erased
 */
static int write_word_amd (flash_info_t *info, FPWV *dest, FPW data)
{
    ulong start;
    int flag;
    int res = 0;	/* result, assume success	*/
    FPWV *base;		/* first address in flash bank	*/

    /* Check if Flash is (sufficiently) erased */
    if ((*dest & data) != data) {
	return (2);
    }

    base = (FPWV *)(info->start[0]);

    /* Disable interrupts which might cause a timeout here */
    flag = disable_interrupts();
  
    (*DANUBE_EBU_BUSCON0)&=(~0x80000000);	// enable writing
    (*DANUBE_EBU_BUSCON1)&=(~0x80000000);	// enable writing
    (*EBU_NAND_CON)=0;
	
    base[FLASH_CYCLE1] = (FPW)0x00AA00AA;	/* unlock */
    base[FLASH_CYCLE2] = (FPW)0x00550055;	/* unlock */
    base[FLASH_CYCLE1] = (FPW)0x00A000A0;	/* selects program mode */

    *dest = data;		/* start programming the data	*/

    /* re-enable interrupts if necessary */
    if (flag)
	enable_interrupts();

    start = get_timer (0);

    /* data polling for D7 */
    while (res == 0 && (*dest & (FPW)0x00800080) != (data & (FPW)0x00800080)) {
	if (get_timer(start) > CFG_FLASH_WRITE_TOUT) {
	    *dest = (FPW)0x00F000F0;	/* reset bank */
	    res = 1;
	}
    }
 
	(*DANUBE_EBU_BUSCON0)|=0x80000000;	// disable writing
	(*DANUBE_EBU_BUSCON1)|=0x80000000;	// disable writing
 
        return (res);
}

/*-----------------------------------------------------------------------
 * Write a word to Flash for Intel FLASH
 * A word is 16 or 32 bits, whichever the bus width of the flash bank
 * (not an individual chip) is.
 *
 * returns:
 * 0 - OK
 * 1 - write timeout
 * 2 - Flash not erased
 */
static int write_word_intel (flash_info_t *info, FPWV *dest, FPW data)
{
    ulong start;
    int flag;
    int res = 0;	/* result, assume success	*/
	
    /* Check if Flash is (sufficiently) erased */
    if ((*dest & data) != data) {
	return (2);
    }

    /* Disable interrupts which might cause a timeout here */
    flag = disable_interrupts();

    (*DANUBE_EBU_BUSCON0)&=(~0x80000000);	// enable writing
    (*DANUBE_EBU_BUSCON1)&=(~0x80000000);	// enable writing
    (*EBU_NAND_CON)=0;
    *dest = (FPW)0x00500050;	/* clear status register	*/
    *dest = (FPW)0x00FF00FF;	/* make sure in read mode	*/
    *dest = (FPW)0x00400040;	/* program setup		*/
    *dest = data;		/* start programming the data	*/
    asm("SYNC");
    
    /* re-enable interrupts if necessary */
    if (flag)
	enable_interrupts();

    start = get_timer (0);

    while (res == 0 && (*dest & (FPW)0x00800080) != (FPW)0x00800080) {
	if (get_timer(start) > CFG_FLASH_WRITE_TOUT) {
	    *dest = (FPW)0x00B000B0;	/* Suspend program	*/
	    res = 1;
	}
    }

    if (res == 0 && (*dest & (FPW)0x00100010))
	res = 1;	/* write failed, time out error is close enough	*/

    *dest = (FPW)0x00500050;	/* clear status register	*/
    flash_reset(info);

    (*DANUBE_EBU_BUSCON0)|=0x80000000;	// disable writing
    (*DANUBE_EBU_BUSCON1)|=0x80000000;	// disable writing
 
        return (res);
}
