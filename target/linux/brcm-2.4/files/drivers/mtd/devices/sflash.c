/*
 * Broadcom SiliconBackplane chipcommon serial flash interface
 *
 * Copyright 2007, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * $Id$
 */

#include <typedefs.h>
#include <osl.h>
#include <sbutils.h>
#include <sbconfig.h>
#include <sbchipc.h>
#include <bcmdevs.h>
#include <sflash.h>

/* Private global state */
static struct sflash sflash;

/* Issue a serial flash command */
static INLINE void
sflash_cmd (osl_t * osh, chipcregs_t * cc, uint opcode)
{
  W_REG (osh, &cc->flashcontrol, SFLASH_START | opcode);
  while (R_REG (osh, &cc->flashcontrol) & SFLASH_BUSY);
}

/* Initialize serial flash access */
struct sflash *
sflash_init (sb_t * sbh, chipcregs_t * cc)
{
  uint32 id, id2;
  osl_t *osh;

  ASSERT (sbh);

  osh = sb_osh (sbh);

  bzero (&sflash, sizeof (sflash));

  sflash.type = sbh->cccaps & CC_CAP_FLASH_MASK;

  switch (sflash.type)
    {
    case SFLASH_ST:
      /* Probe for ST chips */
      sflash_cmd (osh, cc, SFLASH_ST_DP);
      sflash_cmd (osh, cc, SFLASH_ST_RES);
      id = R_REG (osh, &cc->flashdata);
      switch (id)
	{
	case 0x11:
	  /* ST M25P20 2 Mbit Serial Flash */
	  sflash.blocksize = 64 * 1024;
	  sflash.numblocks = 4;
	  break;
	case 0x12:
	  /* ST M25P40 4 Mbit Serial Flash */
	  sflash.blocksize = 64 * 1024;
	  sflash.numblocks = 8;
	  break;
	case 0x13:
	  /* ST M25P80 8 Mbit Serial Flash */
	  sflash.blocksize = 64 * 1024;
	  sflash.numblocks = 16;
	  break;
	case 0x14:
	  /* ST M25P16 16 Mbit Serial Flash */
	  sflash.blocksize = 64 * 1024;
	  sflash.numblocks = 32;
	  break;
	case 0x15:
	  /* ST M25P32 32 Mbit Serial Flash */
	  sflash.blocksize = 64 * 1024;
	  sflash.numblocks = 64;
	  break;
	case 0x16:
	  /* ST M25P64 64 Mbit Serial Flash */
	  sflash.blocksize = 64 * 1024;
	  sflash.numblocks = 128;
	  break;
	case 0xbf:
	  W_REG (osh, &cc->flashaddress, 1);
	  sflash_cmd (osh, cc, SFLASH_ST_RES);
	  id2 = R_REG (osh, &cc->flashdata);
	  if (id2 == 0x44)
	    {
	      /* SST M25VF80 4 Mbit Serial Flash */
	      sflash.blocksize = 64 * 1024;
	      sflash.numblocks = 8;
	    }
	  break;
	}
      break;

    case SFLASH_AT:
      /* Probe for Atmel chips */
      sflash_cmd (osh, cc, SFLASH_AT_STATUS);
      id = R_REG (osh, &cc->flashdata) & 0x3c;
      switch (id)
	{
	case 0xc:
	  /* Atmel AT45DB011 1Mbit Serial Flash */
	  sflash.blocksize = 256;
	  sflash.numblocks = 512;
	  break;
	case 0x14:
	  /* Atmel AT45DB021 2Mbit Serial Flash */
	  sflash.blocksize = 256;
	  sflash.numblocks = 1024;
	  break;
	case 0x1c:
	  /* Atmel AT45DB041 4Mbit Serial Flash */
	  sflash.blocksize = 256;
	  sflash.numblocks = 2048;
	  break;
	case 0x24:
	  /* Atmel AT45DB081 8Mbit Serial Flash */
	  sflash.blocksize = 256;
	  sflash.numblocks = 4096;
	  break;
	case 0x2c:
	  /* Atmel AT45DB161 16Mbit Serial Flash */
	  sflash.blocksize = 512;
	  sflash.numblocks = 4096;
	  break;
	case 0x34:
	  /* Atmel AT45DB321 32Mbit Serial Flash */
	  sflash.blocksize = 512;
	  sflash.numblocks = 8192;
	  break;
	case 0x3c:
	  /* Atmel AT45DB642 64Mbit Serial Flash */
	  sflash.blocksize = 1024;
	  sflash.numblocks = 8192;
	  break;
	}
      break;
    }

  sflash.size = sflash.blocksize * sflash.numblocks;
  return sflash.size ? &sflash : NULL;
}

/* Read len bytes starting at offset into buf. Returns number of bytes read. */
int
sflash_read (sb_t * sbh, chipcregs_t * cc, uint offset, uint len, uchar * buf)
{
  uint8 *from, *to;
  int cnt, i;
  osl_t *osh;

  ASSERT (sbh);

  if (!len)
    return 0;

  if ((offset + len) > sflash.size)
    return -22;

  if ((len >= 4) && (offset & 3))
    cnt = 4 - (offset & 3);
  else if ((len >= 4) && ((uintptr) buf & 3))
    cnt = 4 - ((uintptr) buf & 3);
  else
    cnt = len;

  osh = sb_osh (sbh);

  from = (uint8 *) (uintptr) OSL_UNCACHED (SB_FLASH2 + offset);
  to = (uint8 *) buf;

  if (cnt < 4)
    {
      for (i = 0; i < cnt; i++)
	{
	  *to = R_REG (osh, from);
	  from++;
	  to++;
	}
      return cnt;
    }

  while (cnt >= 4)
    {
      *(uint32 *) to = R_REG (osh, (uint32 *) from);
      from += 4;
      to += 4;
      cnt -= 4;
    }

  return (len - cnt);
}

/* Poll for command completion. Returns zero when complete. */
int
sflash_poll (sb_t * sbh, chipcregs_t * cc, uint offset)
{
  osl_t *osh;

  ASSERT (sbh);

  osh = sb_osh (sbh);

  if (offset >= sflash.size)
    return -22;

  switch (sflash.type)
    {
    case SFLASH_ST:
      /* Check for ST Write In Progress bit */
      sflash_cmd (osh, cc, SFLASH_ST_RDSR);
      return R_REG (osh, &cc->flashdata) & SFLASH_ST_WIP;
    case SFLASH_AT:
      /* Check for Atmel Ready bit */
      sflash_cmd (osh, cc, SFLASH_AT_STATUS);
      return !(R_REG (osh, &cc->flashdata) & SFLASH_AT_READY);
    }

  return 0;
}

/* Write len bytes starting at offset into buf. Returns number of bytes
 * written. Caller should poll for completion.
 */
int
sflash_write (sb_t * sbh, chipcregs_t * cc, uint offset, uint len,
	      const uchar * buf)
{
  struct sflash *sfl;
  int ret = 0;
  bool is4712b0;
  uint32 page, byte, mask;
  osl_t *osh;

  ASSERT (sbh);

  osh = sb_osh (sbh);

  if (!len)
    return 0;

  if ((offset + len) > sflash.size)
    return -22;

  sfl = &sflash;
  switch (sfl->type)
    {
    case SFLASH_ST:
      is4712b0 = (sbh->chip == BCM4712_CHIP_ID) && (sbh->chiprev == 3);
      /* Enable writes */
      sflash_cmd (osh, cc, SFLASH_ST_WREN);
      if (is4712b0)
	{
	  mask = 1 << 14;
	  W_REG (osh, &cc->flashaddress, offset);
	  W_REG (osh, &cc->flashdata, *buf++);
	  /* Set chip select */
	  OR_REG (osh, &cc->gpioout, mask);
	  /* Issue a page program with the first byte */
	  sflash_cmd (osh, cc, SFLASH_ST_PP);
	  ret = 1;
	  offset++;
	  len--;
	  while (len > 0)
	    {
	      if ((offset & 255) == 0)
		{
		  /* Page boundary, drop cs and return */
		  AND_REG (osh, &cc->gpioout, ~mask);
		  if (!sflash_poll (sbh, cc, offset))
		    {
		      /* Flash rejected command */
		      return -11;
		    }
		  return ret;
		}
	      else
		{
		  /* Write single byte */
		  sflash_cmd (osh, cc, *buf++);
		}
	      ret++;
	      offset++;
	      len--;
	    }
	  /* All done, drop cs if needed */
	  if ((offset & 255) != 1)
	    {
	      /* Drop cs */
	      AND_REG (osh, &cc->gpioout, ~mask);
	      if (!sflash_poll (sbh, cc, offset))
		{
		  /* Flash rejected command */
		  return -12;
		}
	    }
	}
      else if (sbh->ccrev >= 20)
	{
	  W_REG (NULL, &cc->flashaddress, offset);
	  W_REG (NULL, &cc->flashdata, *buf++);
	  /* Issue a page program with CSA bit set */
	  sflash_cmd (osh, cc, SFLASH_ST_CSA | SFLASH_ST_PP);
	  ret = 1;
	  offset++;
	  len--;
	  while (len > 0)
	    {
	      if ((offset & 255) == 0)
		{
		  /* Page boundary, poll droping cs and return */
		  W_REG (NULL, &cc->flashcontrol, 0);
		  if (!sflash_poll (sbh, cc, offset))
		    {
		      /* Flash rejected command */
		      return -11;
		    }
		  return ret;
		}
	      else
		{
		  /* Write single byte */
		  sflash_cmd (osh, cc, SFLASH_ST_CSA | *buf++);
		}
	      ret++;
	      offset++;
	      len--;
	    }
	  /* All done, drop cs if needed */
	  if ((offset & 255) != 1)
	    {
	      /* Drop cs, poll */
	      W_REG (NULL, &cc->flashcontrol, 0);
	      if (!sflash_poll (sbh, cc, offset))
		{
		  /* Flash rejected command */
		  return -12;
		}
	    }
	}
      else
	{
	  ret = 1;
	  W_REG (osh, &cc->flashaddress, offset);
	  W_REG (osh, &cc->flashdata, *buf);
	  /* Page program */
	  sflash_cmd (osh, cc, SFLASH_ST_PP);
	}
      break;
    case SFLASH_AT:
      mask = sfl->blocksize - 1;
      page = (offset & ~mask) << 1;
      byte = offset & mask;
      /* Read main memory page into buffer 1 */
      if (byte || (len < sfl->blocksize))
	{
	  W_REG (osh, &cc->flashaddress, page);
	  sflash_cmd (osh, cc, SFLASH_AT_BUF1_LOAD);
	  /* 250 us for AT45DB321B */
	  SPINWAIT (sflash_poll (sbh, cc, offset), 1000);
	  ASSERT (!sflash_poll (sbh, cc, offset));
	}
      /* Write into buffer 1 */
      for (ret = 0; (ret < (int) len) && (byte < sfl->blocksize); ret++)
	{
	  W_REG (osh, &cc->flashaddress, byte++);
	  W_REG (osh, &cc->flashdata, *buf++);
	  sflash_cmd (osh, cc, SFLASH_AT_BUF1_WRITE);
	}
      /* Write buffer 1 into main memory page */
      W_REG (osh, &cc->flashaddress, page);
      sflash_cmd (osh, cc, SFLASH_AT_BUF1_PROGRAM);
      break;
    }

  return ret;
}

/* Erase a region. Returns number of bytes scheduled for erasure.
 * Caller should poll for completion.
 */
int
sflash_erase (sb_t * sbh, chipcregs_t * cc, uint offset)
{
  struct sflash *sfl;
  osl_t *osh;

  ASSERT (sbh);

  osh = sb_osh (sbh);

  if (offset >= sflash.size)
    return -22;

  sfl = &sflash;
  switch (sfl->type)
    {
    case SFLASH_ST:
      sflash_cmd (osh, cc, SFLASH_ST_WREN);
      W_REG (osh, &cc->flashaddress, offset);
      sflash_cmd (osh, cc, SFLASH_ST_SE);
      return sfl->blocksize;
    case SFLASH_AT:
      W_REG (osh, &cc->flashaddress, offset << 1);
      sflash_cmd (osh, cc, SFLASH_AT_PAGE_ERASE);
      return sfl->blocksize;
    }

  return 0;
}

/*
 * writes the appropriate range of flash, a NULL buf simply erases
 * the region of flash
 */
int
sflash_commit (sb_t * sbh, chipcregs_t * cc, uint offset, uint len,
	       const uchar * buf)
{
  struct sflash *sfl;
  uchar *block = NULL, *cur_ptr, *blk_ptr;
  uint blocksize = 0, mask, cur_offset, cur_length, cur_retlen, remainder;
  uint blk_offset, blk_len, copied;
  int bytes, ret = 0;
  osl_t *osh;

  ASSERT (sbh);

  osh = sb_osh (sbh);

  /* Check address range */
  if (len <= 0)
    return 0;

  sfl = &sflash;
  if ((offset + len) > sfl->size)
    return -1;

  blocksize = sfl->blocksize;
  mask = blocksize - 1;

  /* Allocate a block of mem */
  if (!(block = MALLOC (osh, blocksize)))
    return -1;

  while (len)
    {
      /* Align offset */
      cur_offset = offset & ~mask;
      cur_length = blocksize;
      cur_ptr = block;

      remainder = blocksize - (offset & mask);
      if (len < remainder)
	cur_retlen = len;
      else
	cur_retlen = remainder;

      /* buf == NULL means erase only */
      if (buf)
	{
	  /* Copy existing data into holding block if necessary */
	  if ((offset & mask) || (len < blocksize))
	    {
	      blk_offset = cur_offset;
	      blk_len = cur_length;
	      blk_ptr = cur_ptr;

	      /* Copy entire block */
	      while (blk_len)
		{
		  copied =
		    sflash_read (sbh, cc, blk_offset, blk_len, blk_ptr);
		  blk_offset += copied;
		  blk_len -= copied;
		  blk_ptr += copied;
		}
	    }

	  /* Copy input data into holding block */
	  memcpy (cur_ptr + (offset & mask), buf, cur_retlen);
	}

      /* Erase block */
      if ((ret = sflash_erase (sbh, cc, (uint) cur_offset)) < 0)
	goto done;
      while (sflash_poll (sbh, cc, (uint) cur_offset));

      /* buf == NULL means erase only */
      if (!buf)
	{
	  offset += cur_retlen;
	  len -= cur_retlen;
	  continue;
	}

      /* Write holding block */
      while (cur_length > 0)
	{
	  if ((bytes = sflash_write (sbh, cc,
				     (uint) cur_offset,
				     (uint) cur_length,
				     (uchar *) cur_ptr)) < 0)
	    {
	      ret = bytes;
	      goto done;
	    }
	  while (sflash_poll (sbh, cc, (uint) cur_offset));
	  cur_offset += bytes;
	  cur_length -= bytes;
	  cur_ptr += bytes;
	}

      offset += cur_retlen;
      len -= cur_retlen;
      buf += cur_retlen;
    }

  ret = len;
done:
  if (block)
    MFREE (osh, block, blocksize);
  return ret;
}
