/* $Id: flash.c,v 1.39 2004/04/20 07:57:57 jonashg Exp $
 *
 * Stolen from the eLinux kernel and stripped down.
 *
 * HISTORY:
 *
 * $Log: flash.c,v $
 * Revision 1.39  2004/04/20 07:57:57  jonashg
 * Clear flash_status fields to make it possible to flash several images
 * sequentially.
 *
 * Revision 1.38  2003/12/16 09:04:07  magnusmn
 * Removed FLASHFILL command
 *
 * Revision 1.37  2003/12/16 08:49:01  magnusmn
 * Merging change_branch--fast_flash
 *
 * Revision 1.36.2.6  2003/12/15 17:21:27  magnusmn
 * Reset counter when continuing with operations the next sector.
 *
 * Revision 1.36.2.5  2003/12/15 11:35:57  magnusmn
 * Bail out if we try to erase the same sector more that 10 times
 *
 * Revision 1.36.2.4  2003/12/12 12:07:10  magnusmn
 * FIX for ST M29W320DT
 * Some chip need a reset to bring them back to read mode again.
 *
 * Revision 1.36.2.3  2003/11/10 16:38:04  orjanf
 * Unified Erasing/Writing messages
 *
 * Revision 1.36.2.2  2003/11/10 15:52:34  magnusmn
 * More info on a sector basis
 *
 * Revision 1.36.2.1  2003/11/07 16:23:20  magnusmn
 * o Only erase a flash sector if we need to, that is if the source content isn't already is in place.
 * o Don't erase a flash sector that already contain ones.
 * o Don't write ones to a (d)word that already contain ones.
 * o If there are two flashes, switch flash after an erase operation is started on one of them.
 * o Flash fill doesn't work yet.
 * o No timeout implemented, we will continue to erase/program until we succeed.
 * o Interleave not tested.
 *
 * Revision 1.36  2003/10/16 17:08:51  jonashg
 * Bugfix: reversed CFI-tables wasn't handled correctly since regions support was
 * merged.
 *
 * Revision 1.35  2003/10/14 13:43:41  pkj
 * Fixed compiler warnings.
 *
 * Revision 1.34  2003/10/14 10:48:13  magnusmn
 * No need to write ones to a (d)word where there already are ones. This will save time during flash programming.
 *
 * Revision 1.33  2003/10/10 11:46:25  jonashg
 * Merged change_branch--regions_support.
 *
 * Revision 1.32.2.3  2003/10/10 09:38:13  jonashg
 * Corrected calculation of current region and sector before erase.
 *
 * Revision 1.32.2.2  2003/10/09 16:31:26  jonashg
 * Regions support in JEDEC probe.
 *
 * Revision 1.32.2.1  2003/09/19 15:28:22  jonashg
 * Support for unusual region layouts. It only works for CFI compliant chips (yet).
 *
 * Revision 1.32  2002/12/13 15:55:54  jonashg
 * Fix for ST M29W160ET. It seems to need a reset before erase (even though the
 * probe functions did reset it).
 *
 * Revision 1.31  2002/07/01 14:37:25  pkj
 * Merged with the ASIC version of e100boot. Main difference is that
 * information about the executed commands are sent back to e100boot
 * instead of being sent to the debug port. This means there is no
 * longer any need to use different boot loaders for different
 * debug ports.
 *
 * Revision 1.30  2002/06/26 13:28:29  pkj
 * flash_write() can now be used to erase an area (by specifying
 * source as NULL), and to fill an area with the first udword of
 * source by setting do_fill to TRUE).
 *
 * Revision 1.29  2002/06/26 13:19:37  pkj
 * * flash_write() now returns a status code.
 * * timeout is now decremented correctly in flash_write_part() to
 *   actually be able to trigger the timeout message.
 * * Fixed all compiler warnings.
 *
 * Revision 1.28  2002/06/20 12:58:18  pkj
 * Changed svinto_boot.h to e100boot.h
 *
 * Revision 1.27  2002/06/19 14:00:29  pkj
 * * Broke out the probing of the flash chips from  flash_write()
 *   into flash_probe_chips().
 * * flash_probe_chips() is not limited to two chips or that the
 *   first chip exists.
 *
 * Revision 1.26  2002/02/21 14:37:52  jonashg
 * Optimized away my sanity. It's back now I think.
 *
 * Revision 1.25  2002/02/21 14:28:24  jonashg
 * Added support for Atmel AT49?V16?T (had to optimize a bit to make room).
 *
 * Revision 1.24  2002/01/31 14:36:14  jonashg
 * * Added support for Atmel AT49[BL]V16[01] (the chip used in the ETRAX MCM).
 * * Replaced concurrent sector erase with sequential (we have found three
 *   different chips that cannot erase multiple sectors at the same time,
 *   one of the is the chip in the MCM). I haven't noticed any performance
 *   loss on chips (CFI and non-CFI) that can erase all sectors at the same
 *   time either (maybe they don't really erase them at the same time in
 *   hardware).
 * * Added check for manufacturer id as well as device id (should have been
 *   done a long time ago).
 *
 * Revision 1.23  2001/11/21 15:52:44  jonashg
 * Almost readable.
 *
 * Revision 1.22  2001/11/21 15:24:38  jonashg
 * Increased readability and decreased size some 40bytes.
 *
 * Revision 1.21  2001/11/20 13:40:12  starvik
 * Corrected handling for CFI capable bottom boot flashes
 * Shorted some strings to make more space available
 *
 * Revision 1.20  2001/08/08 17:51:28  pkj
 * Made it possible to flash at a start offset other than zero when
 * there are more than one physical flash chip available. Previously
 * it always started flashing from the start of the first flash if
 * there were more than one, even though the start offset was set to
 * something else...
 *
 * Revision 1.19  2001/06/19 14:51:17  jonashg
 * Added support for non-CFI flash Toshiba TC58FVT800.
 *
 * Revision 1.18  2001/04/05 06:32:39  starvik
 * Works with flashes with multiple banks
 *
 * Revision 1.17  2001/03/06 15:21:16  jonashg
 * More output to user.
 *
 * Revision 1.16  2001/03/06 14:11:16  jonashg
 * * Switch to second device correctly when flashing images that extend past the
 *   first device.
 * * Only enter autoselect mode once saves a few bytes (not needed before reading
 *   device id, since it was done before reading manufacturer id).
 * * A few unnecessary resets removed to save another few bytes.
 *
 * Revision 1.15  2001/02/28 14:52:43  jonashg
 * * Reverted to old sector erase sequence (that was correct).
 * * A bit of executable size optimization (a few hundred bytes).
 * * Cleanup.
 *
 * Revision 1.14  2001/02/27 14:18:59  jonashg
 * * Write full erase command sequence to all sectors that should be erased.
 * * Write 16bit erase command to non-interleaved chips.
 *
 * Revision 1.13  2001/02/23 11:03:41  jonashg
 * Added support for 2 x 16Mb flashes (32-bits buswidth).
 * The CFI probe does not detect two parallel flash devices, but the normal
 * probe does (it should be easy to add that in the CFI-probe, but I didn't
 * have any hardware to try it on and the size of the executable is getting
 * pretty close to the size of the ETRAX cache).
 *
 * Revision 1.12  2001/02/12 13:59:00  jonashg
 * Bugfix: pointer arithmetics made bootsector calculation go wrong.
 *
 * Revision 1.11  2000/11/10 08:02:23  starvik
 * Added CFI support
 *
 * Revision 1.10  2000/10/26 13:47:32  johana
 * Added support for Fujitsu flash 16MBit (2MByte) MBM29LV160BE and MBM29LV160TE.
 * NOT VERIFIED YET!
 *
 * Revision 1.9  2000/06/28 13:02:50  bjornw
 * * Added support for SST39LF800 and SST39LF160 flashes
 * * Fixed some indentation issues
 *
 * Revision 1.8  2000/06/13 11:51:11  starvik
 * Support for two flashes. Second flash is erased and programmed if program
 * is larger than first flash.
 *
 * Revision 1.7  2000/04/13 16:06:15  macce
 * See if flash is empty before erasing it. Might save some production time.
 *
 * Revision 1.6  2000/01/27 17:52:07  bjornw
 * * Added Toshiba flashes
 * * Added proper bootblock erase for the different flashes
 *   (this caused the verify errors when trying to do ./flashitall before)
 *
 * Revision 1.5  2000/01/20 11:41:28  finn
 * Improved the verify error printouts in flash_write.
 *
 * Revision 1.4  1999/12/21 19:32:53  bjornw
 * Dont choke on full chip erases even though we dont implement it efficiently.
 *
 * Revision 1.3  1999/11/12 01:30:04  bjornw
 * Added wait for busy to be ready. Removed some warnings.
 *
 * Revision 1.2  1999/10/27 07:42:42  johana
 * Added support for ST M29W800T flash used in 5600
 *
 * Revision 1.1  1999/10/27 01:37:12  bjornw
 * Wrote routines to erase and flash data into a flash ROM.
 *
 */

#include "e100boot.h"

//#define DEBUG

#ifdef DEBUG
#define FDEBUG(x) x
#else
#define FDEBUG(x)
#endif

/* Try turning of some of these if you run into space problems. */
#define CFI_PROBE
#define JEDEC_PROBE
#define INTERLEAVE

#define TYPE_X16	(16 / 8)

#define nop() __asm__("nop")

#define safe_printk send_string

static char *message_bottom_boot_8 = "8Mb BB";
static char *message_top_boot_8 = "8Mb TB";
static char *message_bottom_boot_16 = "16Mb BB";
static char *message_top_boot_16 = "16Mb TB";
static char *message_top_boot_32 = "32Mb TB";

enum {
	/* Addresses */
	ADDR_UNLOCK_1			= 0x0555,
	ADDR_UNLOCK_2			= 0x02AA,
	ADDR_MANUFACTURER		= 0x0000,
	ADDR_DEVICE_ID			= 0x0001,
	ADDR_CFI_QUERY			= 0x0055,

	/* Commands */
	CMD_UNLOCK_DATA_1		= 0x00AA,
	CMD_UNLOCK_DATA_2		= 0x0055,
	CMD_MANUFACTURER_UNLOCK_DATA	= 0x0090,
	CMD_PROGRAM_UNLOCK_DATA		= 0x00A0,
	CMD_RESET_DATA			= 0x00F0,
	CMD_SECTOR_ERASE_UNLOCK_DATA_1	= 0x0080,
	CMD_SECTOR_ERASE_UNLOCK_DATA_2	= 0x0030,
	CMD_CFI_QUERY_DATA		= 0x0098,

	/* Offsets */
	OFFSET_CFI_ID			= 0x10,
	OFFSET_CFI_SIZE			= 0x27,
	OFFSET_CFI_BLOCK_COUNT		= 0x2C,
	OFFSET_CFI_BLOCK		= 0x2D,

	/* Manufacturers */
	MANUFACTURER_AMD		= 0x01,
	MANUFACTURER_ATMEL		= 0x1F,
	MANUFACTURER_FUJITSU		= 0x04,
	MANUFACTURER_SST		= 0xBF,
	MANUFACTURER_ST			= 0x20,
	MANUFACTURER_TOSHIBA		= 0x98,


	/* To save precious space we store mfr and dev id together */

	/* AMD devices */
	AM29F800BB			= 0x00012258,
	AM29F800BT			= 0x000122D6,
	AM29LV800BB			= 0x0001225B,
	AM29LV800BT			= 0x000122DA,
	AM29LV160BT			= 0x000122C4,

	/* Atmel devices */
	AT49xV16x			= 0x001F00C0,
	AT49xV16xT			= 0x001F00C2,
	AT49BV32xAT			= 0x001F00C9,

	/* Fujitsu devices */
	MBM29LV160TE			= 0x000422C4,
	MBM29LV160BE			= 0x00042249,

	/* SST devices */
	SST39LF800			= 0x00BF2781,
	SST39LF160			= 0x00BF2782,

	/* ST devices */
	M29W800T			= 0x002000D7, /* Used in 5600, similar
						       * to AM29LV800, but no
						       * unlock bypass
						       */
	/* Toshiba devices */
	TC58FVT160			= 0x009800C2,
	TC58FVB160			= 0x00980043,
	TC58FVT800			= 0x0098004F,

	/* Toggle bit mask */
	D6_MASK				= 0x40
};

struct region {
	unsigned long offset;
	unsigned int sector_size;
	unsigned int numsectors;
};

#define MAXREGIONS 8

struct chip {
	volatile unsigned char *base;
#ifdef INTERLEAVE
	byte interleave;
	byte buswidth;
#endif
	unsigned int size;
	unsigned short numregions;
	struct region regions[MAXREGIONS];
};

/* Allocate flash structures and initialize base. */
static struct chip chips[2] = {
	{ (unsigned char *)0x80000000,
#ifdef INTERLEAVE
		0, 0,
#endif
		0, 0, { } },
	{ (unsigned char *)0x84000000,
#ifdef INTERLEAVE
		0, 0,
#endif
		0, 0, { } }
};



static unsigned int
wide_read(struct chip *flash, unsigned long offset)
{
#ifdef INTERLEAVE
	switch (flash->buswidth) {
	case 2:
#endif
		return *((uword *)(flash->base + offset));

#ifdef INTERLEAVE
	case 4:
		return *((udword *)(flash->base + offset));
	}

	return 0;
#endif
}

static int
wide_write_chunk(struct chip *flash, unsigned long offset, const void *chunk)
{
#ifdef INTERLEAVE
	switch (flash->buswidth) {
	case 2:
#endif
		*((uword *)(flash->base + offset)) = *((uword *)chunk);
		return 2;

#ifdef INTERLEAVE
	case 4:
		*((udword *)(flash->base + offset)) = *((udword *)chunk);
		return 4;
	}

	return 0;
#endif
}

static void
wide_cmd(struct chip *flash, udword cmd, unsigned long offset)
{
#ifdef INTERLEAVE
	if (flash->interleave == 1) {
#endif
		offset <<= 1;
#ifdef INTERLEAVE
	} else if (flash->interleave == 2) {
		cmd |= (cmd << 16);
		offset <<= 2;
	} else {
		safe_printk("Unsupported interleave!\n");
		return;
	}
#endif

	wide_write_chunk(flash, offset, &cmd);
}

static void
flash_unlock(struct chip *flash)
{
	wide_cmd(flash, CMD_UNLOCK_DATA_1, ADDR_UNLOCK_1);
	wide_cmd(flash, CMD_UNLOCK_DATA_2, ADDR_UNLOCK_2);
}

static int
flash_is_busy(struct chip *flash, unsigned long offset)
{
#ifdef INTERLEAVE
	if (flash->interleave == 2) {
		udword read1, read2;

		read1 = wide_read(flash, offset);
		read2 = wide_read(flash, offset);
		return (((read1 >> 16) & D6_MASK) !=
			((read2 >> 16) & D6_MASK)) ||
		       (((read1 & 0xffff) & D6_MASK) !=
			((read2 & 0xffff) & D6_MASK));
	}
#endif

	return ((wide_read(flash, offset) & D6_MASK) !=
		(wide_read(flash, offset) & D6_MASK));
}



#ifdef CFI_PROBE
static int
try_cfi(struct chip *flash)
{
	int offset_shift = 1;

#ifdef INTERLEAVE
	if (flash->interleave == 2) {
		offset_shift = 2;
	}
#endif

	/* Enter CFI mode */
	wide_cmd(flash, CMD_CFI_QUERY_DATA, ADDR_CFI_QUERY);

	/* Check if flash responds correctly */
	if ((byte)wide_read(flash, (OFFSET_CFI_ID+0) << offset_shift) == 'Q' &&
	    (byte)wide_read(flash, (OFFSET_CFI_ID+1) << offset_shift) == 'R' &&
	    (byte)wide_read(flash, (OFFSET_CFI_ID+2) << offset_shift) == 'Y') {
		int block;               /* Current block */
		int block_count;         /* Number of blocks */
		unsigned int offset = 0; /* Offset into flash */
		int reverse = 0;         /* Reverse block table */
		int primary;             /* Offset to vendor specific table */

		safe_printk("Found 1 x CFI at ");
		send_hex((udword)flash->base, NL);

		flash->size =
			1 << wide_read(flash, OFFSET_CFI_SIZE << offset_shift);

		/* CFI stores flash organization in blocks. Each block contains
		 * a number of sectors with the same size
		 */
		block_count = wide_read(flash, OFFSET_CFI_BLOCK_COUNT <<
					       offset_shift);

		/* Check if table is reversed */
		primary = wide_read(flash, (OFFSET_CFI_ID+5) << offset_shift);
		/* For CFI version 1.0 we don't know. Assume that id & 0x80 */
		/* indicates top boot */
		if ((byte)wide_read(flash, (primary+4) << offset_shift) == 0x30)
		{
			/* read device id */
			wide_cmd(flash, CMD_RESET_DATA, ADDR_UNLOCK_1);
			flash_unlock(flash);
			wide_cmd(flash, CMD_MANUFACTURER_UNLOCK_DATA,
				 ADDR_UNLOCK_1);
			reverse = wide_read(flash, ADDR_DEVICE_ID * TYPE_X16
#ifdef INTERLEAVE
					    * flash->interleave
#endif
					   ) & 0x80;
			wide_cmd(flash, CMD_CFI_QUERY_DATA, ADDR_CFI_QUERY);
		} else {
			reverse = ((byte)wide_read(flash,
					(primary+15) << offset_shift) == 3);
		}

		flash->numregions = block_count;
		if (block_count > MAXREGIONS) {
			safe_printk("Too many regions on chip!\n");
			return 0;
		}

		/* Blocks are stored backwards compared to flash organization */
		for (block = reverse ? block_count - 1 : 0;
		     reverse ? block >= 0 : block < block_count;
		     reverse ? block-- : block++) {
			int region;

			/* Size of each sector in block. Size is stored as
			 * sector_size / 256.
			 */
			int sector_size =
			    (wide_read(flash, (OFFSET_CFI_BLOCK+block * 4+2) <<
					      offset_shift)
				|
			    (wide_read(flash, (OFFSET_CFI_BLOCK+block * 4+3) <<
					      offset_shift) << 8)
			    ) << 8;

			/* Number of sectors */
			int sector_count =
			    (wide_read(flash, (OFFSET_CFI_BLOCK+block * 4+0) <<
					      offset_shift)
				|
			    (wide_read(flash, (OFFSET_CFI_BLOCK+block * 4+1) <<
					      offset_shift) << 8)
			    ) + 1;

			region = reverse? block_count - 1 - block : block;
			flash->regions[region].offset = offset;
			flash->regions[region].sector_size = sector_size;
			flash->regions[region].numsectors = sector_count;

			/* Can't use multiplication (we have no lib). */
			{
				int temp;
				for (temp = 0 ; temp < sector_count ; temp++) {
					offset += sector_size;
				}
			}

FDEBUG(
	if (reverse) {
		safe_printk("NOTE! reversed table:\n");
	}
	safe_printk("region: ");
	send_hex((udword)region, NL);
	safe_printk("   offset: ");
	send_hex((udword)flash->regions[region].offset, NL);
	safe_printk("   sector_size: ");
	send_hex((udword)flash->regions[region].sector_size, NL);
	safe_printk("   numsectors: ");
	send_hex((udword)flash->regions[region].numsectors, NL);
)

		/* Some flashes (SST) store information about alternate
			 * block sizes. Ignore those by breaking when the sum
			 * of the sector sizes == flash size.
			 */
			if (offset == flash->size) {
				break;
			}
		}

		/* reset */
		wide_cmd(flash, CMD_RESET_DATA, ADDR_UNLOCK_1);

		return 1;
	}

	/* reset */
	wide_cmd(flash, CMD_RESET_DATA, ADDR_UNLOCK_1);

	return 0;
}
#endif



static int
flash_probe(struct chip *flash)
{
	char *message;
	udword dev_id;
	udword mfr_id;
	udword id;

	if (flash->size
#ifdef CFI_PROBE
	    || try_cfi(flash)
#endif
	   ) {
		return 1;
	}

#ifdef JEDEC_PROBE
	/* Read manufacturer ID. */
	flash_unlock(flash);
	wide_cmd(flash, CMD_MANUFACTURER_UNLOCK_DATA, ADDR_UNLOCK_1);
	mfr_id = wide_read(flash, ADDR_MANUFACTURER * TYPE_X16
#ifdef INTERLEAVE
			   * flash->interleave
#endif
			  );
	/* Read device ID. */
	dev_id = wide_read(flash, ADDR_DEVICE_ID * TYPE_X16
#ifdef INTERLEAVE
			   * flash->interleave
#endif
			  );
FDEBUG(
	safe_printk("mfr_id: ");
	send_hex(mfr_id, NL);
	safe_printk("dev_id: ");
	send_hex(dev_id, NL);
)

#ifdef INTERLEAVE
	if ((flash->interleave == 2) &&
	    ((mfr_id >> 16) == (mfr_id & 0xffff)) &&
	    ((dev_id >> 16) == (dev_id & 0xffff))) {
		mfr_id &= 0xffff;
		dev_id &= 0xffff;
	}
#endif

	id = (mfr_id << 16) | dev_id;

	/* reset */
	wide_cmd(flash, CMD_RESET_DATA, ADDR_UNLOCK_1);

	/* Check device type and fill in correct sizes. */
	switch (id) {
		case AM29LV160BT:
		case TC58FVT160:
		// case MBM29LV160TE: /* This is same id as AM29LV160BT */
			message = message_top_boot_16;

			flash->size = 0x00200000;

			flash->regions[0].offset = 0x00000000;
			flash->regions[0].sector_size = 0x10000;
			flash->regions[0].numsectors = 31;

			flash->regions[1].offset = 0x001F0000;
			flash->regions[1].sector_size = 0x08000;
			flash->regions[1].numsectors = 1;

			flash->regions[2].offset = 0x001F8000;
			flash->regions[2].sector_size = 0x02000;
			flash->regions[2].numsectors = 2;

			flash->regions[3].offset = 0x001FC000;
			flash->regions[3].sector_size = 0x04000;
			flash->regions[3].numsectors = 1;
				break;

		// case AM29LV160BB:
		case TC58FVB160:
		case MBM29LV160BE:
			message = message_bottom_boot_16;

			flash->size = 0x00200000;

			flash->regions[0].offset = 0x00000000;
			flash->regions[0].sector_size = 0x04000;
			flash->regions[0].numsectors = 1;

			flash->regions[1].offset = 0x00004000;
			flash->regions[1].sector_size = 0x02000;
			flash->regions[1].numsectors = 2;

			flash->regions[2].offset = 0x00008000;
			flash->regions[2].sector_size = 0x08000;
			flash->regions[2].numsectors = 1;

			flash->regions[3].offset = 0x00010000;
			flash->regions[3].sector_size = 0x10000;
			flash->regions[3].numsectors = 31;
			break;

		case AM29LV800BB:
		case AM29F800BB:
			message = message_bottom_boot_8;

			flash->size = 0x00100000;

			flash->regions[0].offset = 0x00000000;
			flash->regions[0].sector_size = 0x04000;
			flash->regions[0].numsectors = 1;

			flash->regions[1].offset = 0x00004000;
			flash->regions[1].sector_size = 0x02000;
			flash->regions[1].numsectors = 2;

			flash->regions[2].offset = 0x00008000;
			flash->regions[2].sector_size = 0x08000;
			flash->regions[2].numsectors = 1;

			flash->regions[3].offset = 0x00010000;
			flash->regions[3].sector_size = 0x10000;
			flash->regions[3].numsectors = 15;
			break;

		case M29W800T:
		case AM29LV800BT:
		case AM29F800BT:
		case TC58FVT800:
			message = message_top_boot_8;

			flash->size = 0x00100000;

			flash->regions[0].offset = 0x00000000;
			flash->regions[0].sector_size = 0x10000;
			flash->regions[0].numsectors = 15;

			flash->regions[1].offset = 0x000F0000;
			flash->regions[1].sector_size = 0x08000;
			flash->regions[1].numsectors = 1;

			flash->regions[2].offset = 0x000F8000;
			flash->regions[2].sector_size = 0x02000;
			flash->regions[2].numsectors = 2;

			flash->regions[3].offset = 0x000FC000;
			flash->regions[3].sector_size = 0x04000;
			flash->regions[3].numsectors = 1;

			break;

		case AT49xV16x:
			message = message_bottom_boot_16;

			flash->size = 0x00200000;

			flash->regions[0].offset = 0x00000000;
			flash->regions[0].sector_size = 0x02000;
			flash->regions[0].numsectors = 8;

			flash->regions[1].offset = 0x00010000;
			flash->regions[1].sector_size = 0x10000;
			flash->regions[1].numsectors = 31;

			break;

		case AT49xV16xT:
			message = message_top_boot_16;

			flash->size = 0x00200000;

			flash->regions[0].offset = 0x00000000;
			flash->regions[0].sector_size = 0x10000;
			flash->regions[0].numsectors = 31;

			flash->regions[1].offset = 0x001F0000;
			flash->regions[1].sector_size = 0x02000;
			flash->regions[1].numsectors = 8;

			break;

		case AT49BV32xAT:
			message = message_top_boot_32;

			flash->size = 0x00400000;

			flash->regions[0].offset = 0x00000000;
			flash->regions[0].sector_size = 0x10000;
			flash->regions[0].numsectors = 63;

			flash->regions[1].offset = 0x001F0000;
			flash->regions[1].sector_size = 0x02000;
			flash->regions[1].numsectors = 8;

			break;

		default:
#endif
#ifdef INTERLEAVE
			if (flash->interleave == 1) {
#endif
				safe_printk("No single x16 at ");
#ifdef INTERLEAVE
			} else {
				safe_printk("No interleaved x16 at ");
			}
#endif
			send_hex((udword)flash->base, NL);

			return 0;
#ifdef JEDEC_PROBE
	}

	safe_printk("Found ");
#ifdef INTERLEAVE
	if (flash->interleave == 1) {
#endif
		safe_printk("1");
#ifdef INTERLEAVE
	}
	if (flash->interleave == 2) {
	int count = 0;

		flash->size <<= 1;
	while (count < MAXREGIONS) {
		flash->regions[count].offset <<= 1;
		flash->regions[count].sector_size <<= 1;
		count++;
	}
		safe_printk("2");
	}
#endif
	safe_printk(" x ");
	safe_printk(message);
	safe_printk(" at ");
	send_hex((udword)flash->base, NL);

	return 1;
#endif
}

/* Start erase of a sector but do no wait for completion */
static void
start_sector_erase(struct chip *flash, unsigned long offset)
{
	flash_unlock(flash);
	wide_cmd(flash, CMD_SECTOR_ERASE_UNLOCK_DATA_1, ADDR_UNLOCK_1);
	flash_unlock(flash);

#ifdef INTERLEAVE
	if (flash->interleave == 2) {
		*(udword *)(flash->base+offset) = (CMD_SECTOR_ERASE_UNLOCK_DATA_2 << 16) |
						   CMD_SECTOR_ERASE_UNLOCK_DATA_2;
	} else {
#endif
		*(uword *)(flash->base+offset) = CMD_SECTOR_ERASE_UNLOCK_DATA_2;
#ifdef INTERLEAVE
	}
#endif
}

/* Return the size of the sector at the given offset */
static int
find_sector_size(struct chip *flash, unsigned long offset)
{
  	unsigned int i, j;
	int region_size;
	/* Sanity check */
	if (offset >= flash->size)
		return 0;

	for(i=0; i < MAXREGIONS; i++) 
		if (offset >= flash->regions[i].offset) {
			region_size=0;
			for (j=0; j < flash->regions[i].numsectors; j++)
				region_size += flash->regions[i].sector_size;
			if (offset < flash->regions[i].offset + region_size)
				return flash->regions[i].sector_size;
		}

	/* Should not happen */
	return 0;
}

/* Check and see if we need to erase the sector  */
/* The return values mean */
/* 0: The source and destination are the same. */
/* 1: The source and destination are not the same, but flash sector already contains only ones. */
/* 2: The source and destination are not the same and the flash sector is tainted by some zeroes. */
static char
need_to_erase(struct chip *flash, unsigned long offset, const unsigned char *source, int size)
{
	int i;
	unsigned long j;
		
	for (i = 0; i < size; i+=2)
		if (*(uword*)(flash->base + i + offset) != *(uword*)(source + i)) {
			/* Check if the sector only contain zeroes */
			for (j = offset; j < (size + offset); j+=2) {
				if (*(uword*)(flash->base + j) != 0xffff)
					return 2;
			}
			return 1;
		}
			
	/* The source is equal to the destination */
	return 0;
}

static unsigned int
flash_probe_chips(void)
{
	unsigned int tot_size = 0;
	unsigned int i = 0;

	for (; i < sizeof chips/sizeof *chips; i++) {
#ifdef INTERLEAVE
		byte interleave;

		for (interleave = 1; interleave < 4; interleave *= 2) {
			chips[i].interleave = interleave;
			if (interleave == 1) {
				chips[i].buswidth = sizeof(uword);
			} else {
				chips[i].buswidth = sizeof(udword);
			}

			if (flash_probe(&chips[i])) {
				break;
			}
		}
#else
		flash_probe(&chips[i]);
#endif

		tot_size += chips[i].size;
	}

	return tot_size;
}

/* Program a sector (given by size) at the given offset. Do not write only ones. */
static void
program_sector(struct chip *flash, unsigned long offset, const unsigned char *source, int size)
{
	int chunk_size = 0;
	int bytes_written = 0;

	
	while (bytes_written < size) {
	 	if (
#ifdef INTERLEAVE
		    (flash->buswidth == 2) && 
#endif
		    *(uword*)(source + bytes_written) == 0xffff) {
			chunk_size=2;	
		}
#ifdef INTERLEAVE
		else if ((flash->buswidth == 4) && *(udword*)(source + bytes_written) == 0xffffffff) {
			chunk_size=4;	
		}
#endif
		else {
		  	flash_unlock(flash);
			wide_cmd(flash, CMD_PROGRAM_UNLOCK_DATA, ADDR_UNLOCK_1);
			chunk_size = wide_write_chunk(flash, offset + bytes_written, source + bytes_written);
			while(flash_is_busy(flash, offset + bytes_written))
			/* Nothing */  
			;
		}
		
		bytes_written += chunk_size;
	}
}

int
flash_write(const unsigned char *source, unsigned int offset, unsigned int size)
{
	struct flash_status {
		unsigned char busy;		/* Indicates if the flash is busy */
		const unsigned char *src;	/* From where to get the source info */
		unsigned long offset;		/* Start operations in flash at this offset */
		unsigned int size;		/* Size to erase/program (if needed) */
		unsigned int bytes_done;	/* Bytes written (if needed) */
		unsigned int erase_attempts;	/* Keep track how many times we try to erase the same sector */
	};
  	
	unsigned int tot_size = flash_probe_chips();
	unsigned int i, j;
	unsigned int current_sector_size;
	unsigned long current_offset;
	const unsigned char *current_src;
	char need_erase;
	struct flash_status *current_flash = NULL;
	
	static struct flash_status flash_status[2] = {
		{ 0, NULL, 0, 0, 0, 0 },
		{ 0, NULL, 0, 0, 0, 0 }
	};

	if (!tot_size) {
		/* No chips found, bail out. */
		return ERR_FLASH_NONE;
	}

	if (offset + size > tot_size) {
		safe_printk("Fatal: flash is too small.\n");
		return ERR_FLASH_TOO_SMALL;
	}

	/* Initiate the flash_status structs so that we can keep track of what needs to be done
	   on the different flash chips */
	
	/* Operations only on flash chip 1 */
	if (offset >= (&chips[0])->size) {
		flash_status[0].size = 0;
		flash_status[1].src = source;
		flash_status[1].offset = offset - (&chips[0])->size;
		flash_status[1].size = size;
	}
	/* Operations on both flash chips */
	else if ((offset < (&chips[0])->size) && ((offset+size) > (&chips[0])->size)) {
		flash_status[0].src = source;
		flash_status[0].offset = offset;
		flash_status[0].size = (&chips[0])->size - offset;
		flash_status[1].src = source + flash_status[0].size;
		flash_status[1].offset = 0;
		flash_status[1].size = size - flash_status[0].size;
	} 
	/* Operations only on flash chip 0 */
	else {
		flash_status[0].src = source;
		flash_status[0].offset = offset;
		flash_status[0].size = size;
		flash_status[1].size = 0;
	}
	flash_status[0].busy = 0;
	flash_status[0].bytes_done = 0;
	flash_status[0].erase_attempts = 0;
	flash_status[1].busy = 0;
	flash_status[1].bytes_done = 0;
	flash_status[1].erase_attempts = 0;
#if 0
        for (i = 0; i < 2; i++) { 
                safe_printk("\nFlash ");
                send_hex(i, NL);
                safe_printk("src:\t");
                send_hex((int)flash_status[i].src, NL);
                safe_printk("offset:\t");
                send_hex(flash_status[i].offset, NL);
                safe_printk("size:\t");
                send_hex(flash_status[i].size, NL);
                safe_printk("\n");
        }
#endif

	/* Erase and write */

	i = 0;	/* Start operations on flash 0 */	

#define CHANGE_FLASH

	while (((&flash_status[0])->bytes_done + (&flash_status[1])->bytes_done) < size) {
	
		struct flash_status *previous_flash = &flash_status[i ? 0 : 1];
		current_flash = &flash_status[i];

#ifdef CHANGE_FLASH
		/* Change flash only if:
		   - There is a flash to change to and operations should be made on that flash *AND*
		   - There is more to write to the previous flash *AND*
		   - Operations should be made on the current flash *OR*
		   - The current flash is busy *OR*
		   - All has been written to the current flash */
	
		if (previous_flash->size && (previous_flash->bytes_done < previous_flash->size) &&
		   	(!current_flash->size || current_flash->busy || 
			  current_flash->bytes_done == current_flash->size))	
				i = i ? 0 : 1;	/* Change flash chip */	
#else
		/* Finish one flash chip before continuing on the next one */
		
		if ((&flash_status[i])->bytes_done == (&flash_status[i])->size)
			i = i ? 0 : 1;	/* Change flash chip */	
#endif
		/* Bail out if we have tried to erase the same sector more that 10 times. */
		if(current_flash->erase_attempts > 10) {
			safe_printk("Sector erase error\n");
			return ERR_FLASH_ERASE;
		}

		/* Get the current status from the chip we are about to access */
		current_flash = &flash_status[i];
		current_offset = current_flash->offset + current_flash->bytes_done;
		current_src = current_flash->src + current_flash->bytes_done;
		current_sector_size = find_sector_size(&chips[i], current_offset);
	
		/* Make sure that the chip we are about to access has finished erasing */
		if (current_flash->busy) {
			while (flash_is_busy(&chips[i], current_offset))
				/* nothing */
				;
			current_flash->busy = 0;
		}
		
		/* Some flash chip need a reset to bring them back to read mode again. */
		wide_cmd(&chips[i], CMD_RESET_DATA, ADDR_UNLOCK_1);
	
		/* Find out if we need to erase the sector or not */
		need_erase = need_to_erase(&chips[i], current_offset, current_src, current_sector_size);
		
		if (need_erase == 0) {
			current_flash->bytes_done += current_sector_size;
			current_flash->erase_attempts = 0;
			send_hex((int)(&chips[i])->base + current_offset, 0);
			safe_printk(": No need to write\n");
			continue;
		} else if (need_erase == 1) {
			/* Erased, not worth printing. */
		}
		else if (need_erase == 2) {
			send_hex((int)(&chips[i])->base + current_offset, 0);
			safe_printk(": Erasing ");
			send_hex(current_sector_size, 0);
			safe_printk(" bytes\n");
			start_sector_erase(&chips[i], current_offset);		
			current_flash->busy=1;
			current_flash->erase_attempts++;
			continue;
		}
			
		/* The sector is ready to be programmed */	
		send_hex((int)(&chips[i])->base + current_offset, 0);
		safe_printk(": Writing ");
		send_hex(current_sector_size, 0);
		safe_printk(" bytes\n");
		program_sector(&chips[i], current_offset, current_src, current_sector_size);
		current_flash->bytes_done += current_sector_size;
		current_flash->erase_attempts = 0;
	}
	
	/* Verify that the flash chip(s) have the correct content */
	for (i = 0; i < 2; i++) {
	  	current_flash = &flash_status[i]; 
		if (!current_flash->size)
			continue;
		send_hex((int)(&chips[i])->base, 0);
		safe_printk(": Verifying...");
		for (j = 0; j < current_flash->size; j+=2) {
			if (*(uword*)(current_flash->offset + j + (&chips[i])->base) != 
			    *(uword*)(current_flash->src + j)) {
				safe_printk("Error at ");
				send_hex(j, NL);
				return ERR_FLASH_VERIFY;
			}
		}
		safe_printk("OK\n");
	}

	return ERR_FLASH_OK;
}
