/*
 * (C) Copyright 2000
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

/* #define DEBUG */

#include <common.h>
#include <flash.h>

#if !defined(CFG_NO_FLASH)

extern flash_info_t  flash_info[]; /* info for FLASH chips */

/*-----------------------------------------------------------------------
 * Functions
 */

/*-----------------------------------------------------------------------
 * Set protection status for monitor sectors
 *
 * The monitor is always located in the _first_ Flash bank.
 * If necessary you have to map the second bank at lower addresses.
 */
void
flash_protect (int flag, ulong from, ulong to, flash_info_t *info)
{
	ulong b_end = info->start[0] + info->size - 1;	/* bank end address */
	short s_end = info->sector_count - 1;	/* index of last sector */
	int i;

	debug ("flash_protect %s: from 0x%08lX to 0x%08lX\n",
		(flag & FLAG_PROTECT_SET) ? "ON" :
			(flag & FLAG_PROTECT_CLEAR) ? "OFF" : "???",
		from, to);

	/* Do nothing if input data is bad. */
	if (info->sector_count == 0 || info->size == 0 || to < from) {
		return;
	}

	/* There is nothing to do if we have no data about the flash
	 * or the protect range and flash range don't overlap.
	 */
	if (info->flash_id == FLASH_UNKNOWN ||
	    to < info->start[0] || from > b_end) {
		return;
	}

	for (i=0; i<info->sector_count; ++i) {
		ulong end;		/* last address in current sect	*/

		end = (i == s_end) ? b_end : info->start[i + 1] - 1;

		/* Update protection if any part of the sector
		 * is in the specified range.
		 */
		if (from <= end && to >= info->start[i]) {
			if (flag & FLAG_PROTECT_CLEAR) {
#if defined(CFG_FLASH_PROTECTION)
				flash_real_protect(info, i, 0);
#else
				info->protect[i] = 0;
#endif	/* CFG_FLASH_PROTECTION */
				debug ("protect off %d\n", i);
			}
			else if (flag & FLAG_PROTECT_SET) {
#if defined(CFG_FLASH_PROTECTION)
				flash_real_protect(info, i, 1);
#else
				info->protect[i] = 1;
#endif	/* CFG_FLASH_PROTECTION */
				debug ("protect on %d\n", i);
			}
		}
	}
}

/*-----------------------------------------------------------------------
 */

flash_info_t *
addr2info (ulong addr)
{
#ifndef CONFIG_SPD823TS
	flash_info_t *info;
	int i;

	for (i=0, info=&flash_info[0]; i<CFG_MAX_FLASH_BANKS; ++i, ++info) {
		if (info->flash_id != FLASH_UNKNOWN &&
		    addr >= info->start[0] &&
		    /* WARNING - The '- 1' is needed if the flash
		     * is at the end of the address space, since
		     * info->start[0] + info->size wraps back to 0.
		     * Please don't change this unless you understand this.
		     */
		    addr <= info->start[0] + info->size - 1) {
			return (info);
		}
	}
#endif /* CONFIG_SPD823TS */

	return (NULL);
}

/*-----------------------------------------------------------------------
 * Copy memory to flash.
 * Make sure all target addresses are within Flash bounds,
 * and no protected sectors are hit.
 * Returns:
 * ERR_OK          0 - OK
 * ERR_TIMOUT      1 - write timeout
 * ERR_NOT_ERASED  2 - Flash not erased
 * ERR_PROTECTED   4 - target range includes protected sectors
 * ERR_INVAL       8 - target address not in Flash memory
 * ERR_ALIGN       16 - target address not aligned on boundary
 *			(only some targets require alignment)
 */
int
flash_write (char *src, ulong addr, ulong cnt)
{
#ifdef CONFIG_SPD823TS
	return (ERR_TIMOUT);	/* any other error codes are possible as well */
#else
	int i;
	ulong         end        = addr + cnt - 1;
	flash_info_t *info_first = addr2info (addr);
	flash_info_t *info_last  = addr2info (end );
	flash_info_t *info;

	if (cnt == 0) {
		return (ERR_OK);
	}

	if (!info_first || !info_last) {
		return (ERR_INVAL);
	}

	for (info = info_first; info <= info_last; ++info) {
		ulong b_end = info->start[0] + info->size;	/* bank end addr */
		short s_end = info->sector_count - 1;
		for (i=0; i<info->sector_count; ++i) {
			ulong e_addr = (i == s_end) ? b_end : info->start[i + 1];

			if ((end >= info->start[i]) && (addr < e_addr) &&
			    (info->protect[i] != 0) ) {
				return (ERR_PROTECTED);
			}
		}
	}

	/* finally write data to flash */
	for (info = info_first; info <= info_last && cnt>0; ++info) {
		ulong len;

		len = info->start[0] + info->size - addr;
		if (len > cnt)
			len = cnt;
		if ((i = write_buff(info, (uchar *)src, addr, len)) != 0) {
			return (i);
		}
		cnt  -= len;
		addr += len;
		src  += len;
	}
	return (ERR_OK);
#endif /* CONFIG_SPD823TS */
}

/*-----------------------------------------------------------------------
 */

void flash_perror (int err)
{
	switch (err) {
	case ERR_OK:
		break;
	case ERR_TIMOUT:
		puts ("Timeout writing to Flash\n");
		break;
	case ERR_NOT_ERASED:
		puts ("Flash not Erased\n");
		break;
	case ERR_PROTECTED:
		puts ("Can't write to protected Flash sectors\n");
		break;
	case ERR_INVAL:
		puts ("Outside available Flash\n");
		break;
	case ERR_ALIGN:
		puts ("Start and/or end address not on sector boundary\n");
		break;
	case ERR_UNKNOWN_FLASH_VENDOR:
		puts ("Unknown Vendor of Flash\n");
		break;
	case ERR_UNKNOWN_FLASH_TYPE:
		puts ("Unknown Type of Flash\n");
		break;
	case ERR_PROG_ERROR:
		puts ("General Flash Programming Error\n");
		break;
	default:
		printf ("%s[%d] FIXME: rc=%d\n", __FILE__, __LINE__, err);
		break;
	}
}

/*-----------------------------------------------------------------------
 */
#endif /* !CFG_NO_FLASH */
