/*
 *  Routines to access SPROM and to parse SROM/CIS variables.
 *
 * Copyright 2007, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 * $Id$
 */

#include <typedefs.h>
#include <bcmdefs.h>
#include <osl.h>
#include <stdarg.h>
#include <sbchipc.h>
#include <bcmdevs.h>
#include <bcmendian.h>
#include <sbpcmcia.h>
#include <pcicfg.h>
#include <sbconfig.h>
#include <sbutils.h>
#include <bcmsrom.h>
#include <bcmnvram.h>
#include "utils.h"

/* debug/trace */
#if defined(WLTEST)
#define	BS_ERROR(args)	printf args
#else
#define	BS_ERROR(args)
#endif

#define WRITE_ENABLE_DELAY	500	/* 500 ms after write enable/disable toggle */
#define WRITE_WORD_DELAY	20	/* 20 ms between each word write */

typedef struct varbuf
{
  char *buf;			/* pointer to current position */
  unsigned int size;		/* current (residual) size in bytes */
} varbuf_t;

static int initvars_srom_sb (sb_t * sbh, osl_t * osh, void *curmap,
			     char **vars, uint * count);
static void _initvars_srom_pci (uint8 sromrev, uint16 * srom, uint off,
				varbuf_t * b);
static int initvars_srom_pci (sb_t * sbh, void *curmap, char **vars,
			      uint * count);
static int initvars_cis_pcmcia (sb_t * sbh, osl_t * osh, char **vars,
				uint * count);
#if !defined(BCMUSBDEV) && !defined(BCMSDIODEV)
static int initvars_flash_sb (sb_t * sbh, char **vars, uint * count);
#endif /* !BCMUSBDEV && !BCMSDIODEV */
static int sprom_cmd_pcmcia (osl_t * osh, uint8 cmd);
static int sprom_read_pcmcia (osl_t * osh, uint16 addr, uint16 * data);
static int sprom_write_pcmcia (osl_t * osh, uint16 addr, uint16 data);
static int sprom_read_pci (osl_t * osh, uint16 * sprom, uint wordoff,
			   uint16 * buf, uint nwords, bool check_crc);

static int initvars_table (osl_t * osh, char *start, char *end, char **vars,
			   uint * count);
static int initvars_flash (sb_t * sbh, osl_t * osh, char **vp, uint len);

#ifdef BCMUSBDEV
static int get_sb_pcmcia_srom (sb_t * sbh, osl_t * osh, uint8 * pcmregs,
			       uint boff, uint16 * srom, uint bsz);
static int set_sb_pcmcia_srom (sb_t * sbh, osl_t * osh, uint8 * pcmregs,
			       uint boff, uint16 * srom, uint bsz);
static uint srom_size (sb_t * sbh, osl_t * osh);
#endif /* def BCMUSBDEV */

/* Initialization of varbuf structure */
static void
varbuf_init (varbuf_t * b, char *buf, uint size)
{
  b->size = size;
  b->buf = buf;
}

/* append a null terminated var=value string */
static int
varbuf_append (varbuf_t * b, const char *fmt, ...)
{
  va_list ap;
  int r;

  if (b->size < 2)
    return 0;

  va_start (ap, fmt);
  r = vsnprintf (b->buf, b->size, fmt, ap);
  va_end (ap);

  /* C99 snprintf behavior returns r >= size on overflow,
   * others return -1 on overflow.
   * All return -1 on format error.
   * We need to leave room for 2 null terminations, one for the current var
   * string, and one for final null of the var table. So check that the
   * strlen written, r, leaves room for 2 chars.
   */
  if ((r == -1) || (r > (int) (b->size - 2)))
    {
      b->size = 0;
      return 0;
    }

  /* skip over this string's null termination */
  r++;
  b->size -= r;
  b->buf += r;

  return r;
}

/*
 * Initialize local vars from the right source for this platform.
 * Return 0 on success, nonzero on error.
 */
int
BCMINITFN (srom_var_init) (sb_t * sbh, uint bustype, void *curmap,
			   osl_t * osh, char **vars, uint * count)
{
  ASSERT (bustype == BUSTYPE (bustype));
  if (vars == NULL || count == NULL)
    return (0);

  *vars = NULL;
  *count = 0;

  switch (BUSTYPE (bustype))
    {
    case SB_BUS:
    case JTAG_BUS:
      return initvars_srom_sb (sbh, osh, curmap, vars, count);

    case PCI_BUS:
      ASSERT (curmap);		/* can not be NULL */
      return initvars_srom_pci (sbh, curmap, vars, count);

    case PCMCIA_BUS:
      return initvars_cis_pcmcia (sbh, osh, vars, count);


    default:
      ASSERT (0);
    }
  return (-1);
}

/* support only 16-bit word read from srom */
int
srom_read (sb_t * sbh, uint bustype, void *curmap, osl_t * osh,
	   uint byteoff, uint nbytes, uint16 * buf)
{
  void *srom;
  uint i, off, nw;

  ASSERT (bustype == BUSTYPE (bustype));

  /* check input - 16-bit access only */
  if (byteoff & 1 || nbytes & 1 || (byteoff + nbytes) > (SPROM_SIZE * 2))
    return 1;

  off = byteoff / 2;
  nw = nbytes / 2;

  if (BUSTYPE (bustype) == PCI_BUS)
    {
      if (!curmap)
	return 1;
      srom = (uchar *) curmap + PCI_BAR0_SPROM_OFFSET;
      if (sprom_read_pci (osh, srom, off, buf, nw, FALSE))
	return 1;
    }
  else if (BUSTYPE (bustype) == PCMCIA_BUS)
    {
      for (i = 0; i < nw; i++)
	{
	  if (sprom_read_pcmcia
	      (osh, (uint16) (off + i), (uint16 *) (buf + i)))
	    return 1;
	}
    }
  else if (BUSTYPE (bustype) == SB_BUS)
    {
#ifdef BCMUSBDEV
      if (SPROMBUS == PCMCIA_BUS)
	{
	  uint origidx;
	  void *regs;
	  int rc;
	  bool wasup;

	  origidx = sb_coreidx (sbh);
	  regs = sb_setcore (sbh, SB_PCMCIA, 0);
	  ASSERT (regs != NULL);

	  if (!(wasup = sb_iscoreup (sbh)))
	    sb_core_reset (sbh, 0, 0);

	  rc = get_sb_pcmcia_srom (sbh, osh, regs, byteoff, buf, nbytes);

	  if (!wasup)
	    sb_core_disable (sbh, 0);

	  sb_setcoreidx (sbh, origidx);
	  return rc;
	}
#endif /* def BCMUSBDEV */

      return 1;
    }
  else
    {
      return 1;
    }

  return 0;
}

/* support only 16-bit word write into srom */
int
srom_write (sb_t * sbh, uint bustype, void *curmap, osl_t * osh,
	    uint byteoff, uint nbytes, uint16 * buf)
{
  uint16 *srom;
  uint i, nw, crc_range;
  uint16 image[SPROM_SIZE];
  uint8 crc;
  volatile uint32 val32;

  ASSERT (bustype == BUSTYPE (bustype));

  /* check input - 16-bit access only */
  if ((byteoff & 1) || (nbytes & 1))
    return 1;

  if (byteoff == 0x55aa)
    {
      /* Erase request */
      crc_range = 0;
      memset ((void *) image, 0xff, nbytes);
      nw = nbytes / 2;
    }
  else if ((byteoff == 0) &&
	   ((nbytes == SPROM_SIZE * 2) ||
	    (nbytes == (SPROM_CRC_RANGE * 2)) ||
	    (nbytes == (SROM4_WORDS * 2))))
    {
      /* Are we writing the whole thing at once? */
      crc_range = nbytes;
      bcopy ((void *) buf, (void *) image, nbytes);
      nw = nbytes / 2;
    }
  else
    {
      if ((byteoff + nbytes) > (SPROM_SIZE * 2))
	return 1;

      if (BUSTYPE (bustype) == PCMCIA_BUS)
	{
	  crc_range = SPROM_SIZE * 2;
	}
      else
	{
	  crc_range = SPROM_CRC_RANGE * 2;	/* Tentative */
	}

      nw = crc_range / 2;
      /* read first 64 words from srom */
      if (srom_read (sbh, bustype, curmap, osh, 0, crc_range, image))
	return 1;
      if (image[SROM4_SIGN] == SROM4_SIGNATURE)
	{
	  nw = SROM4_WORDS;
	  crc_range = nw * 2;
	  if (srom_read (sbh, bustype, curmap, osh, 0, crc_range, image))
	    return 1;
	}
      /* make changes */
      bcopy ((void *) buf, (void *) &image[byteoff / 2], nbytes);
    }

  if (crc_range)
    {
      /* calculate crc */
      htol16_buf (image, crc_range);
      crc = ~hndcrc8 ((uint8 *) image, crc_range - 1, 0xff);
      ltoh16_buf (image, crc_range);
      image[nw - 1] = (crc << 8) | (image[nw - 1] & 0xff);
    }

  if (BUSTYPE (bustype) == PCI_BUS)
    {
      srom = (uint16 *) ((uchar *) curmap + PCI_BAR0_SPROM_OFFSET);
      /* enable writes to the SPROM */
      val32 = OSL_PCI_READ_CONFIG (osh, PCI_SPROM_CONTROL, sizeof (uint32));
      val32 |= SPROM_WRITEEN;
      OSL_PCI_WRITE_CONFIG (osh, PCI_SPROM_CONTROL, sizeof (uint32), val32);
      bcm_mdelay (WRITE_ENABLE_DELAY);
      /* write srom */
      for (i = 0; i < nw; i++)
	{
	  W_REG (osh, &srom[i], image[i]);
	  bcm_mdelay (WRITE_WORD_DELAY);
	}
      /* disable writes to the SPROM */
      OSL_PCI_WRITE_CONFIG (osh, PCI_SPROM_CONTROL, sizeof (uint32), val32 &
			    ~SPROM_WRITEEN);
    }
  else if (BUSTYPE (bustype) == PCMCIA_BUS)
    {
      /* enable writes to the SPROM */
      if (sprom_cmd_pcmcia (osh, SROM_WEN))
	return 1;
      bcm_mdelay (WRITE_ENABLE_DELAY);
      /* write srom */
      for (i = 0; i < nw; i++)
	{
	  sprom_write_pcmcia (osh, (uint16) (i), image[i]);
	  bcm_mdelay (WRITE_WORD_DELAY);
	}
      /* disable writes to the SPROM */
      if (sprom_cmd_pcmcia (osh, SROM_WDS))
	return 1;
    }
  else if (BUSTYPE (bustype) == SB_BUS)
    {
#ifdef BCMUSBDEV
      if (SPROMBUS == PCMCIA_BUS)
	{
	  uint origidx;
	  void *regs;
	  int rc;
	  bool wasup;

	  origidx = sb_coreidx (sbh);
	  regs = sb_setcore (sbh, SB_PCMCIA, 0);
	  ASSERT (regs != NULL);

	  if (!(wasup = sb_iscoreup (sbh)))
	    sb_core_reset (sbh, 0, 0);

	  rc = set_sb_pcmcia_srom (sbh, osh, regs, byteoff, buf, nbytes);

	  if (!wasup)
	    sb_core_disable (sbh, 0);

	  sb_setcoreidx (sbh, origidx);
	  return rc;
	}
#endif /* def BCMUSBDEV */
      return 1;
    }
  else
    {
      return 1;
    }

  bcm_mdelay (WRITE_ENABLE_DELAY);
  return 0;
}

#ifdef BCMUSBDEV
#define SB_PCMCIA_READ(osh, regs, fcr) \
		R_REG(osh, (volatile uint8 *)(regs) + 0x600 + (fcr) - 0x700 / 2)
#define SB_PCMCIA_WRITE(osh, regs, fcr, v) \
		W_REG(osh, (volatile uint8 *)(regs) + 0x600 + (fcr) - 0x700 / 2, v)

/* set PCMCIA srom command register */
static int
srom_cmd_sb_pcmcia (osl_t * osh, uint8 * pcmregs, uint8 cmd)
{
  uint8 status = 0;
  uint wait_cnt = 0;

  /* write srom command register */
  SB_PCMCIA_WRITE (osh, pcmregs, SROM_CS, cmd);

  /* wait status */
  while (++wait_cnt < 1000000)
    {
      status = SB_PCMCIA_READ (osh, pcmregs, SROM_CS);
      if (status & SROM_DONE)
	return 0;
      OSL_DELAY (1);
    }

  BS_ERROR (("sr_cmd: Give up after %d tries, stat = 0x%x\n", wait_cnt,
	     status));
  return 1;
}

/* read a word from the PCMCIA srom over SB */
static int
srom_read_sb_pcmcia (osl_t * osh, uint8 * pcmregs, uint16 addr, uint16 * data)
{
  uint8 addr_l, addr_h, data_l, data_h;

  addr_l = (uint8) ((addr * 2) & 0xff);
  addr_h = (uint8) (((addr * 2) >> 8) & 0xff);

  /* set address */
  SB_PCMCIA_WRITE (osh, pcmregs, SROM_ADDRH, addr_h);
  SB_PCMCIA_WRITE (osh, pcmregs, SROM_ADDRL, addr_l);

  /* do read */
  if (srom_cmd_sb_pcmcia (osh, pcmregs, SROM_READ))
    return 1;

  /* read data */
  data_h = SB_PCMCIA_READ (osh, pcmregs, SROM_DATAH);
  data_l = SB_PCMCIA_READ (osh, pcmregs, SROM_DATAL);
  *data = ((uint16) data_h << 8) | data_l;

  return 0;
}

/* write a word to the PCMCIA srom over SB */
static int
srom_write_sb_pcmcia (osl_t * osh, uint8 * pcmregs, uint16 addr, uint16 data)
{
  uint8 addr_l, addr_h, data_l, data_h;
  int rc;

  addr_l = (uint8) ((addr * 2) & 0xff);
  addr_h = (uint8) (((addr * 2) >> 8) & 0xff);

  /* set address */
  SB_PCMCIA_WRITE (osh, pcmregs, SROM_ADDRH, addr_h);
  SB_PCMCIA_WRITE (osh, pcmregs, SROM_ADDRL, addr_l);

  data_l = (uint8) (data & 0xff);
  data_h = (uint8) ((data >> 8) & 0xff);

  /* write data */
  SB_PCMCIA_WRITE (osh, pcmregs, SROM_DATAH, data_h);
  SB_PCMCIA_WRITE (osh, pcmregs, SROM_DATAL, data_l);

  /* do write */
  rc = srom_cmd_sb_pcmcia (osh, pcmregs, SROM_WRITE);
  OSL_DELAY (20000);
  return rc;
}

/*
 * Read the srom for the pcmcia-srom over sb case.
 * Return 0 on success, nonzero on error.
 */
static int
get_sb_pcmcia_srom (sb_t * sbh, osl_t * osh, uint8 * pcmregs,
		    uint boff, uint16 * srom, uint bsz)
{
  uint i, nw, woff, wsz;
  int err = 0;

  /* read must be at word boundary */
  ASSERT ((boff & 1) == 0 && (bsz & 1) == 0);

  /* read sprom size and validate the parms */
  if ((nw = srom_size (sbh, osh)) == 0)
    {
      BS_ERROR (("get_sb_pcmcia_srom: sprom size unknown\n"));
      err = -1;
      goto out;
    }
  if (boff + bsz > 2 * nw)
    {
      BS_ERROR (("get_sb_pcmcia_srom: sprom size exceeded\n"));
      err = -2;
      goto out;
    }

  /* read in sprom contents */
  for (woff = boff / 2, wsz = bsz / 2, i = 0;
       woff < nw && i < wsz; woff++, i++)
    {
      if (srom_read_sb_pcmcia (osh, pcmregs, (uint16) woff, &srom[i]))
	{
	  BS_ERROR (("get_sb_pcmcia_srom: sprom read failed\n"));
	  err = -3;
	  goto out;
	}
    }

out:
  return err;
}

/*
 * Write the srom for the pcmcia-srom over sb case.
 * Return 0 on success, nonzero on error.
 */
static int
set_sb_pcmcia_srom (sb_t * sbh, osl_t * osh, uint8 * pcmregs,
		    uint boff, uint16 * srom, uint bsz)
{
  uint i, nw, woff, wsz;
  uint16 word;
  uint8 crc;
  int err = 0;

  /* write must be at word boundary */
  ASSERT ((boff & 1) == 0 && (bsz & 1) == 0);

  /* read sprom size and validate the parms */
  if ((nw = srom_size (sbh, osh)) == 0)
    {
      BS_ERROR (("set_sb_pcmcia_srom: sprom size unknown\n"));
      err = -1;
      goto out;
    }
  if (boff + bsz > 2 * nw)
    {
      BS_ERROR (("set_sb_pcmcia_srom: sprom size exceeded\n"));
      err = -2;
      goto out;
    }

  /* enable write */
  if (srom_cmd_sb_pcmcia (osh, pcmregs, SROM_WEN))
    {
      BS_ERROR (("set_sb_pcmcia_srom: sprom wen failed\n"));
      err = -3;
      goto out;
    }

  /* write buffer to sprom */
  for (woff = boff / 2, wsz = bsz / 2, i = 0;
       woff < nw && i < wsz; woff++, i++)
    {
      if (srom_write_sb_pcmcia (osh, pcmregs, (uint16) woff, srom[i]))
	{
	  BS_ERROR (("set_sb_pcmcia_srom: sprom write failed\n"));
	  err = -4;
	  goto out;
	}
    }

  /* fix crc */
  crc = 0xff;
  for (woff = 0; woff < nw; woff++)
    {
      if (srom_read_sb_pcmcia (osh, pcmregs, (uint16) woff, &word))
	{
	  BS_ERROR (("set_sb_pcmcia_srom: sprom fix crc read failed\n"));
	  err = -5;
	  goto out;
	}
      word = htol16 (word);
      crc = hndcrc8 ((uint8 *) & word, woff != nw - 1 ? 2 : 1, crc);
    }
  word = (~crc << 8) + (ltoh16 (word) & 0xff);
  if (srom_write_sb_pcmcia (osh, pcmregs, (uint16) (woff - 1), word))
    {
      BS_ERROR (("set_sb_pcmcia_srom: sprom fix crc write failed\n"));
      err = -6;
      goto out;
    }

  /* disable write */
  if (srom_cmd_sb_pcmcia (osh, pcmregs, SROM_WDS))
    {
      BS_ERROR (("set_sb_pcmcia_srom: sprom wds failed\n"));
      err = -7;
      goto out;
    }

out:
  return err;
}
#endif /* def BCMUSBDEV */

int
srom_parsecis (osl_t * osh, uint8 * pcis[], uint ciscnt, char **vars,
	       uint * count)
{
  char eabuf[32];
  char *base;
  varbuf_t b;
  uint8 *cis, tup, tlen, sromrev = 1;
  int i, j;
  uint varsize;
  bool ag_init = FALSE;
  uint32 w32;
  uint funcid;
  uint cisnum;
  int32 boardnum = -1;

  ASSERT (vars);
  ASSERT (count);

  base = MALLOC (osh, MAXSZ_NVRAM_VARS);
  ASSERT (base);
  if (!base)
    return -2;

  varbuf_init (&b, base, MAXSZ_NVRAM_VARS);

  eabuf[0] = '\0';
  for (cisnum = 0; cisnum < ciscnt; cisnum++)
    {
      cis = *pcis++;
      i = 0;
      funcid = 0;
      do
	{
	  tup = cis[i++];
	  tlen = cis[i++];
	  if ((i + tlen) >= CIS_SIZE)
	    break;

	  switch (tup)
	    {
	    case CISTPL_VERS_1:
	      /* assume the strings are good if the version field checks out */
	      if (((cis[i + 1] << 8) + cis[i]) >= 0x0008)
		{
		  varbuf_append (&b, "manf=%s", &cis[i + 2]);
		  varbuf_append (&b, "productname=%s",
				 &cis[i + 3 + strlen ((char *) &cis[i + 2])]);
		  break;
		}

	    case CISTPL_MANFID:
	      varbuf_append (&b, "manfid=0x%x", (cis[i + 1] << 8) + cis[i]);
	      varbuf_append (&b, "prodid=0x%x",
			     (cis[i + 3] << 8) + cis[i + 2]);
	      break;

	    case CISTPL_FUNCID:
	      funcid = cis[i];
	      break;

	    case CISTPL_FUNCE:
	      switch (funcid)
		{
		default:
		  /* set macaddr if HNBU_MACADDR not seen yet */
		  if (eabuf[0] == '\0' && cis[i] == LAN_NID)
		    {
		      ASSERT (cis[i + 1] == ETHER_ADDR_LEN);
		      bcm_ether_ntoa ((struct ether_addr *) &cis[i + 2],
				      eabuf);
		    }
		  /* set boardnum if HNBU_BOARDNUM not seen yet */
		  if (boardnum == -1)
		    boardnum = (cis[i + 6] << 8) + cis[i + 7];
		  break;
		}
	      break;

	    case CISTPL_CFTABLE:
	      varbuf_append (&b, "regwindowsz=%d",
			     (cis[i + 7] << 8) | cis[i + 6]);
	      break;

	    case CISTPL_BRCM_HNBU:
	      switch (cis[i])
		{
		case HNBU_SROMREV:
		  sromrev = cis[i + 1];
		  varbuf_append (&b, "sromrev=%d", sromrev);
		  break;

		case HNBU_CHIPID:
		  varbuf_append (&b, "vendid=0x%x", (cis[i + 2] << 8) +
				 cis[i + 1]);
		  varbuf_append (&b, "devid=0x%x", (cis[i + 4] << 8) +
				 cis[i + 3]);
		  if (tlen >= 7)
		    {
		      varbuf_append (&b, "chiprev=%d",
				     (cis[i + 6] << 8) + cis[i + 5]);
		    }
		  if (tlen >= 9)
		    {
		      varbuf_append (&b, "subvendid=0x%x",
				     (cis[i + 8] << 8) + cis[i + 7]);
		    }
		  if (tlen >= 11)
		    {
		      varbuf_append (&b, "subdevid=0x%x",
				     (cis[i + 10] << 8) + cis[i + 9]);
		      /* subdevid doubles for boardtype */
		      varbuf_append (&b, "boardtype=0x%x",
				     (cis[i + 10] << 8) + cis[i + 9]);
		    }
		  break;

		case HNBU_BOARDREV:
		  varbuf_append (&b, "boardrev=0x%x", cis[i + 1]);
		  break;

		case HNBU_AA:
		  varbuf_append (&b, "aa2g=%d", cis[i + 1]);
		  break;

		case HNBU_AG:
		  varbuf_append (&b, "ag0=%d", cis[i + 1]);
		  ag_init = TRUE;
		  break;

		case HNBU_ANT5G:
		  varbuf_append (&b, "aa5g=%d", cis[i + 1]);
		  varbuf_append (&b, "ag1=%d", cis[i + 2]);
		  break;

		case HNBU_CC:
		  ASSERT (sromrev == 1);
		  varbuf_append (&b, "cc=%d", cis[i + 1]);
		  break;

		case HNBU_PAPARMS:
		  if (tlen == 2)
		    {
		      ASSERT (sromrev == 1);
		      varbuf_append (&b, "pa0maxpwr=%d", cis[i + 1]);
		    }
		  else if (tlen >= 9)
		    {
		      if (tlen == 10)
			{
			  ASSERT (sromrev >= 2);
			  varbuf_append (&b, "opo=%d", cis[i + 9]);
			}
		      else
			ASSERT (tlen == 9);

		      for (j = 0; j < 3; j++)
			{
			  varbuf_append (&b, "pa0b%d=%d", j,
					 (cis[i + (j * 2) + 2] << 8) +
					 cis[i + (j * 2) + 1]);
			}
		      varbuf_append (&b, "pa0itssit=%d", cis[i + 7]);
		      varbuf_append (&b, "pa0maxpwr=%d", cis[i + 8]);
		    }
		  else
		    ASSERT (tlen >= 9);
		  break;

		case HNBU_PAPARMS5G:
		  ASSERT ((sromrev == 2) || (sromrev == 3));
		  for (j = 0; j < 3; j++)
		    {
		      varbuf_append (&b, "pa1b%d=%d", j,
				     (cis[i + (j * 2) + 2] << 8) +
				     cis[i + (j * 2) + 1]);
		    }
		  for (j = 3; j < 6; j++)
		    {
		      varbuf_append (&b, "pa1lob%d=%d", j - 3,
				     (cis[i + (j * 2) + 2] << 8) +
				     cis[i + (j * 2) + 1]);
		    }
		  for (j = 6; j < 9; j++)
		    {
		      varbuf_append (&b, "pa1hib%d=%d", j - 6,
				     (cis[i + (j * 2) + 2] << 8) +
				     cis[i + (j * 2) + 1]);
		    }
		  varbuf_append (&b, "pa1itssit=%d", cis[i + 19]);
		  varbuf_append (&b, "pa1maxpwr=%d", cis[i + 20]);
		  varbuf_append (&b, "pa1lomaxpwr=%d", cis[i + 21]);
		  varbuf_append (&b, "pa1himaxpwr=%d", cis[i + 22]);
		  break;

		case HNBU_OEM:
		  ASSERT (sromrev == 1);
		  varbuf_append (&b, "oem=%02x%02x%02x%02x%02x%02x%02x%02x",
				 cis[i + 1], cis[i + 2],
				 cis[i + 3], cis[i + 4],
				 cis[i + 5], cis[i + 6],
				 cis[i + 7], cis[i + 8]);
		  break;

		case HNBU_BOARDFLAGS:
		  w32 = (cis[i + 2] << 8) + cis[i + 1];
		  if (tlen == 5)
		    w32 |= (cis[i + 4] << 24) + (cis[i + 3] << 16);
		  varbuf_append (&b, "boardflags=0x%x", w32);
		  break;

		case HNBU_LEDS:
		  if (cis[i + 1] != 0xff)
		    {
		      varbuf_append (&b, "ledbh0=%d", cis[i + 1]);
		    }
		  if (cis[i + 2] != 0xff)
		    {
		      varbuf_append (&b, "ledbh1=%d", cis[i + 2]);
		    }
		  if (cis[i + 3] != 0xff)
		    {
		      varbuf_append (&b, "ledbh2=%d", cis[i + 3]);
		    }
		  if (cis[i + 4] != 0xff)
		    {
		      varbuf_append (&b, "ledbh3=%d", cis[i + 4]);
		    }
		  break;

		case HNBU_CCODE:
		  ASSERT (sromrev > 1);
		  if ((cis[i + 1] == 0) || (cis[i + 2] == 0))
		    varbuf_append (&b, "ccode=");
		  else
		    varbuf_append (&b, "ccode=%c%c", cis[i + 1], cis[i + 2]);
		  varbuf_append (&b, "cctl=0x%x", cis[i + 3]);
		  break;

		case HNBU_CCKPO:
		  ASSERT (sromrev > 2);
		  varbuf_append (&b, "cckpo=0x%x",
				 (cis[i + 2] << 8) | cis[i + 1]);
		  break;

		case HNBU_OFDMPO:
		  ASSERT (sromrev > 2);
		  varbuf_append (&b, "ofdmpo=0x%x",
				 (cis[i + 4] << 24) |
				 (cis[i + 3] << 16) |
				 (cis[i + 2] << 8) | cis[i + 1]);
		  break;

		case HNBU_RDLID:
		  varbuf_append (&b, "rdlid=0x%x",
				 (cis[i + 2] << 8) | cis[i + 1]);
		  break;

		case HNBU_RDLRNDIS:
		  varbuf_append (&b, "rdlrndis=%d", cis[i + 1]);
		  break;

		case HNBU_RDLRWU:
		  varbuf_append (&b, "rdlrwu=%d", cis[i + 1]);
		  break;

		case HNBU_RDLSN:
		  varbuf_append (&b, "rdlsn=%d",
				 (cis[i + 2] << 8) | cis[i + 1]);
		  break;

		case HNBU_XTALFREQ:
		  varbuf_append (&b, "xtalfreq=%d",
				 (cis[i + 4] << 24) |
				 (cis[i + 3] << 16) |
				 (cis[i + 2] << 8) | cis[i + 1]);
		  break;

		case HNBU_RSSISMBXA2G:
		  ASSERT (sromrev == 3);
		  varbuf_append (&b, "rssismf2g=%d", cis[i + 1] & 0xf);
		  varbuf_append (&b, "rssismc2g=%d", (cis[i + 1] >> 4) & 0xf);
		  varbuf_append (&b, "rssisav2g=%d", cis[i + 2] & 0x7);
		  varbuf_append (&b, "bxa2g=%d", (cis[i + 2] >> 3) & 0x3);
		  break;

		case HNBU_RSSISMBXA5G:
		  ASSERT (sromrev == 3);
		  varbuf_append (&b, "rssismf5g=%d", cis[i + 1] & 0xf);
		  varbuf_append (&b, "rssismc5g=%d", (cis[i + 1] >> 4) & 0xf);
		  varbuf_append (&b, "rssisav5g=%d", cis[i + 2] & 0x7);
		  varbuf_append (&b, "bxa5g=%d", (cis[i + 2] >> 3) & 0x3);
		  break;

		case HNBU_TRI2G:
		  ASSERT (sromrev == 3);
		  varbuf_append (&b, "tri2g=%d", cis[i + 1]);
		  break;

		case HNBU_TRI5G:
		  ASSERT (sromrev == 3);
		  varbuf_append (&b, "tri5gl=%d", cis[i + 1]);
		  varbuf_append (&b, "tri5g=%d", cis[i + 2]);
		  varbuf_append (&b, "tri5gh=%d", cis[i + 3]);
		  break;

		case HNBU_RXPO2G:
		  ASSERT (sromrev == 3);
		  varbuf_append (&b, "rxpo2g=%d", cis[i + 1]);
		  break;

		case HNBU_RXPO5G:
		  ASSERT (sromrev == 3);
		  varbuf_append (&b, "rxpo5g=%d", cis[i + 1]);
		  break;

		case HNBU_BOARDNUM:
		  boardnum = (cis[i + 2] << 8) + cis[i + 1];
		  break;

		case HNBU_MACADDR:
		  bcm_ether_ntoa ((struct ether_addr *) &cis[i + 1], eabuf);
		  break;

		case HNBU_BOARDTYPE:
		  varbuf_append (&b, "boardtype=0x%x",
				 (cis[i + 2] << 8) + cis[i + 1]);
		  break;

#if defined(BCMCCISSR3)
		case HNBU_SROM3SWRGN:
		  {
		    uint16 srom[35];
		    uint8 srev = cis[i + 1 + 70];
		    ASSERT (srev == 3);
		    /* make tuple value 16-bit aligned and parse it */
		    bcopy (&cis[i + 1], srom, sizeof (srom));
		    _initvars_srom_pci (srev, srom, SROM3_SWRGN_OFF, &b);
		    /* create extra variables */
		    varbuf_append (&b, "vendid=0x%x",
				   (cis[i + 1 + 73] << 8) + cis[i + 1 + 72]);
		    varbuf_append (&b, "devid=0x%x",
				   (cis[i + 1 + 75] << 8) + cis[i + 1 + 74]);
		    varbuf_append (&b, "xtalfreq=%d",
				   (cis[i + 1 + 77] << 8) + cis[i + 1 + 76]);
		    /* 2.4G antenna gain is included in SROM */
		    ag_init = TRUE;
		    /* Ethernet MAC address is included in SROM */
		    eabuf[0] = 0;
		    boardnum = -1;
		    break;
		  }
#endif
		}
	      break;
	    }
	  i += tlen;
	}
      while (tup != CISTPL_END);
    }

  if (boardnum != -1)
    {
      varbuf_append (&b, "boardnum=%d", boardnum);
    }

  if (eabuf[0])
    {
      varbuf_append (&b, "macaddr=%s", eabuf);
    }

  /* if there is no antenna gain field, set default */
  if (ag_init == FALSE)
    {
      varbuf_append (&b, "ag0=%d", 0xff);
    }

  /* final nullbyte terminator */
  ASSERT (b.size >= 1);
  *b.buf++ = '\0';
  varsize = (uint) (b.buf - base);
  ASSERT (varsize < MAXSZ_NVRAM_VARS);
  if (varsize < MAXSZ_NVRAM_VARS)
    {
      char *new_buf;
      new_buf = (char *) MALLOC (osh, varsize);
      ASSERT (new_buf);
      if (new_buf)
	{
	  bcopy (base, new_buf, varsize);
	  MFREE (osh, base, MAXSZ_NVRAM_VARS);
	  base = new_buf;
	}
    }

  *vars = base;
  *count = varsize;

  return (0);
}


/* set PCMCIA sprom command register */
static int
sprom_cmd_pcmcia (osl_t * osh, uint8 cmd)
{
  uint8 status = 0;
  uint wait_cnt = 1000;

  /* write sprom command register */
  OSL_PCMCIA_WRITE_ATTR (osh, SROM_CS, &cmd, 1);

  /* wait status */
  while (wait_cnt--)
    {
      OSL_PCMCIA_READ_ATTR (osh, SROM_CS, &status, 1);
      if (status & SROM_DONE)
	return 0;
    }

  return 1;
}

/* read a word from the PCMCIA srom */
static int
sprom_read_pcmcia (osl_t * osh, uint16 addr, uint16 * data)
{
  uint8 addr_l, addr_h, data_l, data_h;

  addr_l = (uint8) ((addr * 2) & 0xff);
  addr_h = (uint8) (((addr * 2) >> 8) & 0xff);

  /* set address */
  OSL_PCMCIA_WRITE_ATTR (osh, SROM_ADDRH, &addr_h, 1);
  OSL_PCMCIA_WRITE_ATTR (osh, SROM_ADDRL, &addr_l, 1);

  /* do read */
  if (sprom_cmd_pcmcia (osh, SROM_READ))
    return 1;

  /* read data */
  data_h = data_l = 0;
  OSL_PCMCIA_READ_ATTR (osh, SROM_DATAH, &data_h, 1);
  OSL_PCMCIA_READ_ATTR (osh, SROM_DATAL, &data_l, 1);

  *data = (data_h << 8) | data_l;
  return 0;
}

/* write a word to the PCMCIA srom */
static int
sprom_write_pcmcia (osl_t * osh, uint16 addr, uint16 data)
{
  uint8 addr_l, addr_h, data_l, data_h;

  addr_l = (uint8) ((addr * 2) & 0xff);
  addr_h = (uint8) (((addr * 2) >> 8) & 0xff);
  data_l = (uint8) (data & 0xff);
  data_h = (uint8) ((data >> 8) & 0xff);

  /* set address */
  OSL_PCMCIA_WRITE_ATTR (osh, SROM_ADDRH, &addr_h, 1);
  OSL_PCMCIA_WRITE_ATTR (osh, SROM_ADDRL, &addr_l, 1);

  /* write data */
  OSL_PCMCIA_WRITE_ATTR (osh, SROM_DATAH, &data_h, 1);
  OSL_PCMCIA_WRITE_ATTR (osh, SROM_DATAL, &data_l, 1);

  /* do write */
  return sprom_cmd_pcmcia (osh, SROM_WRITE);
}

/*
 * Read in and validate sprom.
 * Return 0 on success, nonzero on error.
 */
static int
sprom_read_pci (osl_t * osh, uint16 * sprom, uint wordoff, uint16 * buf,
		uint nwords, bool check_crc)
{
  int err = 0;
  uint i;

  /* read the sprom */
  for (i = 0; i < nwords; i++)
    {
#ifdef BCMQT
      buf[i] = R_REG (osh, &sprom[wordoff + i]);
#endif
      buf[i] = R_REG (osh, &sprom[wordoff + i]);
    }

  if (check_crc)
    {
      if (buf[0] == 0xffff)
	{
	  /* The hardware thinks that an srom that starts with 0xffff
	   * is blank, regardless of the rest of the content, so declare
	   * it bad.
	   */
	  BS_ERROR (("%s: buf[0] = 0x%x, returning bad-crc\n", __FUNCTION__,
		     buf[0]));
	  return 1;
	}

      /* fixup the endianness so crc8 will pass */
      htol16_buf (buf, nwords * 2);
      if (hndcrc8 ((uint8 *) buf, nwords * 2, 0xff) != 0x9f)
	err = 1;
      /* now correct the endianness of the byte array */
      ltoh16_buf (buf, nwords * 2);
    }

  return err;
}

/*
* Create variable table from memory.
* Return 0 on success, nonzero on error.
*/
static int
BCMINITFN (initvars_table) (osl_t * osh, char *start, char *end, char **vars,
			    uint * count)
{
  int c = (int) (end - start);

  /* do it only when there is more than just the null string */
  if (c > 1)
    {
      char *vp = MALLOC (osh, c);
      ASSERT (vp);
      if (!vp)
	return BCME_NOMEM;
      bcopy (start, vp, c);
      *vars = vp;
      *count = c;
    }
  else
    {
      *vars = NULL;
      *count = 0;
    }

  return 0;
}

/*
 * Find variables with <devpath> from flash. 'base' points to the beginning
 * of the table upon enter and to the end of the table upon exit when success.
 * Return 0 on success, nonzero on error.
 */
static int
initvars_flash (sb_t * sbh, osl_t * osh, char **base, uint len)
{
  char *vp = *base;
  char *flash;
  int err;
  char *s;
  uint l, dl, copy_len;
  char devpath[SB_DEVPATH_BUFSZ];

  /* allocate memory and read in flash */
  if (!(flash = MALLOC (osh, NVRAM_SPACE)))
    return BCME_NOMEM;
  if ((err = nvram_getall (flash, NVRAM_SPACE)))
    goto exit;

  sb_devpath (sbh, devpath, sizeof (devpath));

  /* grab vars with the <devpath> prefix in name */
  dl = strlen (devpath);
  for (s = flash; s && *s; s += l + 1)
    {
      l = strlen (s);

      /* skip non-matching variable */
      if (strncmp (s, devpath, dl))
	continue;

      /* is there enough room to copy? */
      copy_len = l - dl + 1;
      if (len < copy_len)
	{
	  err = BCME_BUFTOOSHORT;
	  goto exit;
	}

      /* no prefix, just the name=value */
      strncpy (vp, &s[dl], copy_len);
      vp += copy_len;
      len -= copy_len;
    }

  /* add null string as terminator */
  if (len < 1)
    {
      err = BCME_BUFTOOSHORT;
      goto exit;
    }
  *vp++ = '\0';

  *base = vp;

exit:MFREE (osh, flash, NVRAM_SPACE);
  return err;
}

#if !defined(BCMUSBDEV) && !defined(BCMSDIODEV)
/*
 * Initialize nonvolatile variable table from flash.
 * Return 0 on success, nonzero on error.
 */
static int
initvars_flash_sb (sb_t * sbh, char **vars, uint * count)
{
  osl_t *osh = sb_osh (sbh);
  char *vp, *base;
  int err;

  ASSERT (vars);
  ASSERT (count);

  base = vp = MALLOC (osh, MAXSZ_NVRAM_VARS);
  ASSERT (vp);
  if (!vp)
    return BCME_NOMEM;

  if ((err = initvars_flash (sbh, osh, &vp, MAXSZ_NVRAM_VARS)) == 0)
    err = initvars_table (osh, base, vp, vars, count);

  MFREE (osh, base, MAXSZ_NVRAM_VARS);

  return err;
}
#endif /* !BCMUSBDEV && !BCMSDIODEV */

#ifdef WLTEST
char mfgsromvars[256];
char *defaultsromvars = "il0macaddr=00:11:22:33:44:51\0"
  "et0macaddr=00:11:22:33:44:52\0"
  "et1macaddr=00:11:22:33:44:53\0"
  "boardtype=0xffff\0"
  "boardrev=0x10\0" "boardflags=8\0" "sromrev=2\0" "aa2g=3\0" "\0";
#define	MFGSROM_DEFVARSLEN	149	/* default srom len */
#endif /* WL_TEST */

/*
 * Initialize nonvolatile variable table from sprom.
 * Return 0 on success, nonzero on error.
 */

typedef struct
{
  const char *name;
  uint32 revmask;
  uint32 flags;
  uint16 off;
  uint16 mask;
} sromvar_t;

#define SRFL_MORE	1	/* value continues as described by the next entry */
#define	SRFL_NOFFS	2	/* value bits can't be all one's */
#define	SRFL_PRHEX	4	/* value is in hexdecimal format */
#define	SRFL_PRSIGN	8	/* value is in signed decimal format */
#define	SRFL_CCODE	0x10	/* value is in country code format */
#define	SRFL_ETHADDR	0x20	/* value is an Ethernet address */
#define SRFL_LEDDC	0x40	/* value is an LED duty cycle */

/* Assumptions:
 * - Ethernet address spins across 3 consective words
 *
 * Table rules:
 * - Add multiple entries next to each other if a value spins across multiple words
 *   (even multiple fields in the same word) with each entry except the last having
 *   it's SRFL_MORE bit set.
 * - Ethernet address entry does not follow above rule and must not have SRFL_MORE
 *   bit set. Its SRFL_ETHADDR bit implies it takes multiple words.
 * - The last entry's name field must be NULL to indicate the end of the table. Other
 *   entries must have non-NULL name.
 */

static const sromvar_t pci_sromvars[] = {
  {"boardrev", 0x0000000e, SRFL_PRHEX, SROM_AABREV, SROM_BR_MASK},
  {"boardrev", 0x000000f0, SRFL_PRHEX, SROM4_BREV, 0xffff},
  {"boardrev", 0xffffff00, SRFL_PRHEX, SROM8_BREV, 0xffff},
  {"boardflags", 0x00000002, SRFL_PRHEX, SROM_BFL, 0xffff},
  {"boardflags", 0x00000004, SRFL_PRHEX | SRFL_MORE, SROM_BFL, 0xffff},
  {"", 0, 0, SROM_BFL2, 0xffff},
  {"boardflags", 0x00000008, SRFL_PRHEX | SRFL_MORE, SROM_BFL, 0xffff},
  {"", 0, 0, SROM3_BFL2, 0xffff},
  {"boardflags", 0x00000010, SRFL_PRHEX | SRFL_MORE, SROM4_BFL0, 0xffff},
  {"", 0, 0, SROM4_BFL1, 0xffff},
  {"boardflags", 0x000000e0, SRFL_PRHEX | SRFL_MORE, SROM5_BFL0, 0xffff},
  {"", 0, 0, SROM5_BFL1, 0xffff},
  {"boardflags", 0xffffff00, SRFL_PRHEX | SRFL_MORE, SROM8_BFL0, 0xffff},
  {"", 0, 0, SROM8_BFL1, 0xffff},
  {"boardflags2", 0x00000010, SRFL_PRHEX | SRFL_MORE, SROM4_BFL2, 0xffff},
  {"", 0, 0, SROM4_BFL3, 0xffff},
  {"boardflags2", 0x000000e0, SRFL_PRHEX | SRFL_MORE, SROM5_BFL2, 0xffff},
  {"", 0, 0, SROM5_BFL3, 0xffff},
  {"boardflags2", 0xffffff00, SRFL_PRHEX | SRFL_MORE, SROM8_BFL2, 0xffff},
  {"", 0, 0, SROM8_BFL3, 0xffff},
  {"boardtype", 0xfffffffc, SRFL_PRHEX, SROM_SSID, 0xffff},
  {"boardnum", 0x00000006, 0, SROM_MACLO_IL0, 0xffff},
  {"boardnum", 0x00000008, 0, SROM3_MACLO, 0xffff},
  {"boardnum", 0x00000010, 0, SROM4_MACLO, 0xffff},
  {"boardnum", 0x000000e0, 0, SROM5_MACLO, 0xffff},
  {"boardnum", 0xffffff00, 0, SROM8_MACLO, 0xffff},
  {"cc", 0x00000002, 0, SROM_AABREV, SROM_CC_MASK},
  {"regrev", 0x00000008, 0, SROM_OPO, 0xff00},
  {"regrev", 0x00000010, 0, SROM4_REGREV, 0xff},
  {"regrev", 0x000000e0, 0, SROM5_REGREV, 0xff},
  {"regrev", 0xffffff00, 0, SROM8_REGREV, 0xff},
  {"ledbh0", 0x0000000e, SRFL_NOFFS, SROM_LEDBH10, 0xff},
  {"ledbh1", 0x0000000e, SRFL_NOFFS, SROM_LEDBH10, 0xff00},
  {"ledbh2", 0x0000000e, SRFL_NOFFS, SROM_LEDBH32, 0xff},
  {"ledbh3", 0x0000000e, SRFL_NOFFS, SROM_LEDBH32, 0xff00},
  {"ledbh0", 0x00000010, SRFL_NOFFS, SROM4_LEDBH10, 0xff},
  {"ledbh1", 0x00000010, SRFL_NOFFS, SROM4_LEDBH10, 0xff00},
  {"ledbh2", 0x00000010, SRFL_NOFFS, SROM4_LEDBH32, 0xff},
  {"ledbh3", 0x00000010, SRFL_NOFFS, SROM4_LEDBH32, 0xff00},
  {"ledbh0", 0x000000e0, SRFL_NOFFS, SROM5_LEDBH10, 0xff},
  {"ledbh1", 0x000000e0, SRFL_NOFFS, SROM5_LEDBH10, 0xff00},
  {"ledbh2", 0x000000e0, SRFL_NOFFS, SROM5_LEDBH32, 0xff},
  {"ledbh3", 0x000000e0, SRFL_NOFFS, SROM5_LEDBH32, 0xff00},
  {"ledbh0", 0xffffff00, SRFL_NOFFS, SROM8_LEDBH10, 0xff},
  {"ledbh1", 0xffffff00, SRFL_NOFFS, SROM8_LEDBH10, 0xff00},
  {"ledbh2", 0xffffff00, SRFL_NOFFS, SROM8_LEDBH32, 0xff},
  {"ledbh3", 0xffffff00, SRFL_NOFFS, SROM8_LEDBH32, 0xff00},
  {"pa0b0", 0x0000000e, SRFL_PRHEX, SROM_WL0PAB0, 0xffff},
  {"pa0b1", 0x0000000e, SRFL_PRHEX, SROM_WL0PAB1, 0xffff},
  {"pa0b2", 0x0000000e, SRFL_PRHEX, SROM_WL0PAB2, 0xffff},
  {"pa0itssit", 0x0000000e, 0, SROM_ITT, 0xff},
  {"pa0maxpwr", 0x0000000e, 0, SROM_WL10MAXP, 0xff},
  {"pa0b0", 0xffffff00, SRFL_PRHEX, SROM8_W0_PAB0, 0xffff},
  {"pa0b1", 0xffffff00, SRFL_PRHEX, SROM8_W0_PAB1, 0xffff},
  {"pa0b2", 0xffffff00, SRFL_PRHEX, SROM8_W0_PAB2, 0xffff},
  {"pa0itssit", 0xffffff00, 0, SROM8_W0_ITTMAXP, 0xff00},
  {"pa0maxpwr", 0xffffff00, 0, SROM8_W0_ITTMAXP, 0xff},
  {"opo", 0x0000000c, 0, SROM_OPO, 0xff},
  {"opo", 0xffffff00, 0, SROM8_2G_OFDMPO, 0xff},
  {"aa2g", 0x0000000e, 0, SROM_AABREV, SROM_AA0_MASK},
  {"aa2g", 0x000000f0, 0, SROM4_AA, 0xff},
  {"aa2g", 0xffffff00, 0, SROM8_AA, 0xff},
  {"aa5g", 0x0000000e, 0, SROM_AABREV, SROM_AA1_MASK},
  {"aa5g", 0x000000f0, 0, SROM4_AA, 0xff00},
  {"aa5g", 0xffffff00, 0, SROM8_AA, 0xff00},
  {"ag0", 0x0000000e, 0, SROM_AG10, 0xff},
  {"ag1", 0x0000000e, 0, SROM_AG10, 0xff00},
  {"ag0", 0x000000f0, 0, SROM4_AG10, 0xff},
  {"ag1", 0x000000f0, 0, SROM4_AG10, 0xff00},
  {"ag2", 0x000000f0, 0, SROM4_AG32, 0xff},
  {"ag3", 0x000000f0, 0, SROM4_AG32, 0xff00},
  {"ag0", 0xffffff00, 0, SROM8_AG10, 0xff},
  {"ag1", 0xffffff00, 0, SROM8_AG10, 0xff00},
  {"ag2", 0xffffff00, 0, SROM8_AG32, 0xff},
  {"ag3", 0xffffff00, 0, SROM8_AG32, 0xff00},
  {"pa1b0", 0x0000000e, SRFL_PRHEX, SROM_WL1PAB0, 0xffff},
  {"pa1b1", 0x0000000e, SRFL_PRHEX, SROM_WL1PAB1, 0xffff},
  {"pa1b2", 0x0000000e, SRFL_PRHEX, SROM_WL1PAB2, 0xffff},
  {"pa1lob0", 0x0000000c, SRFL_PRHEX, SROM_WL1LPAB0, 0xffff},
  {"pa1lob1", 0x0000000c, SRFL_PRHEX, SROM_WL1LPAB1, 0xffff},
  {"pa1lob2", 0x0000000c, SRFL_PRHEX, SROM_WL1LPAB2, 0xffff},
  {"pa1hib0", 0x0000000c, SRFL_PRHEX, SROM_WL1HPAB0, 0xffff},
  {"pa1hib1", 0x0000000c, SRFL_PRHEX, SROM_WL1HPAB1, 0xffff},
  {"pa1hib2", 0x0000000c, SRFL_PRHEX, SROM_WL1HPAB2, 0xffff},
  {"pa1itssit", 0x0000000e, 0, SROM_ITT, 0xff00},
  {"pa1maxpwr", 0x0000000e, 0, SROM_WL10MAXP, 0xff00},
  {"pa1lomaxpwr", 0x0000000c, 0, SROM_WL1LHMAXP, 0xff00},
  {"pa1himaxpwr", 0x0000000c, 0, SROM_WL1LHMAXP, 0xff},
  {"pa1b0", 0xffffff00, SRFL_PRHEX, SROM8_W1_PAB0, 0xffff},
  {"pa1b1", 0xffffff00, SRFL_PRHEX, SROM8_W1_PAB1, 0xffff},
  {"pa1b2", 0xffffff00, SRFL_PRHEX, SROM8_W1_PAB2, 0xffff},
  {"pa1lob0", 0xffffff00, SRFL_PRHEX, SROM8_W1_PAB0_LC, 0xffff},
  {"pa1lob1", 0xffffff00, SRFL_PRHEX, SROM8_W1_PAB1_LC, 0xffff},
  {"pa1lob2", 0xffffff00, SRFL_PRHEX, SROM8_W1_PAB2_LC, 0xffff},
  {"pa1hib0", 0xffffff00, SRFL_PRHEX, SROM8_W1_PAB0_HC, 0xffff},
  {"pa1hib1", 0xffffff00, SRFL_PRHEX, SROM8_W1_PAB1_HC, 0xffff},
  {"pa1hib2", 0xffffff00, SRFL_PRHEX, SROM8_W1_PAB2_HC, 0xffff},
  {"pa1itssit", 0xffffff00, 0, SROM8_W1_ITTMAXP, 0xff00},
  {"pa1maxpwr", 0xffffff00, 0, SROM8_W1_ITTMAXP, 0xff},
  {"pa1lomaxpwr", 0xffffff00, 0, SROM8_W1_MAXP_LCHC, 0xff00},
  {"pa1himaxpwr", 0xffffff00, 0, SROM8_W1_MAXP_LCHC, 0xff},
  {"bxa2g", 0x00000008, 0, SROM_BXARSSI2G, 0x1800},
  {"rssisav2g", 0x00000008, 0, SROM_BXARSSI2G, 0x0700},
  {"rssismc2g", 0x00000008, 0, SROM_BXARSSI2G, 0x00f0},
  {"rssismf2g", 0x00000008, 0, SROM_BXARSSI2G, 0x000f},
  {"bxa2g", 0xffffff00, 0, SROM8_BXARSSI2G, 0x1800},
  {"rssisav2g", 0xffffff00, 0, SROM8_BXARSSI2G, 0x0700},
  {"rssismc2g", 0xffffff00, 0, SROM8_BXARSSI2G, 0x00f0},
  {"rssismf2g", 0xffffff00, 0, SROM8_BXARSSI2G, 0x000f},
  {"bxa5g", 0x00000008, 0, SROM_BXARSSI5G, 0x1800},
  {"rssisav5g", 0x00000008, 0, SROM_BXARSSI5G, 0x0700},
  {"rssismc5g", 0x00000008, 0, SROM_BXARSSI5G, 0x00f0},
  {"rssismf5g", 0x00000008, 0, SROM_BXARSSI5G, 0x000f},
  {"bxa5g", 0xffffff00, 0, SROM8_BXARSSI5G, 0x1800},
  {"rssisav5g", 0xffffff00, 0, SROM8_BXARSSI5G, 0x0700},
  {"rssismc5g", 0xffffff00, 0, SROM8_BXARSSI5G, 0x00f0},
  {"rssismf5g", 0xffffff00, 0, SROM8_BXARSSI5G, 0x000f},
  {"tri2g", 0x00000008, 0, SROM_TRI52G, 0xff},
  {"tri5g", 0x00000008, 0, SROM_TRI52G, 0xff00},
  {"tri5gl", 0x00000008, 0, SROM_TRI5GHL, 0xff},
  {"tri5gh", 0x00000008, 0, SROM_TRI5GHL, 0xff00},
  {"tri2g", 0xffffff00, 0, SROM8_TRI52G, 0xff},
  {"tri5g", 0xffffff00, 0, SROM8_TRI52G, 0xff00},
  {"tri5gl", 0xffffff00, 0, SROM8_TRI5GHL, 0xff},
  {"tri5gh", 0xffffff00, 0, SROM8_TRI5GHL, 0xff00},
  {"rxpo2g", 0x00000008, SRFL_PRSIGN, SROM_RXPO52G, 0xff},
  {"rxpo5g", 0x00000008, SRFL_PRSIGN, SROM_RXPO52G, 0xff00},
  {"rxpo2g", 0xffffff00, SRFL_PRSIGN, SROM8_RXPO52G, 0xff},
  {"rxpo5g", 0xffffff00, SRFL_PRSIGN, SROM8_RXPO52G, 0xff00},
  {"txchain", 0x000000f0, SRFL_NOFFS, SROM4_TXRXC, SROM4_TXCHAIN_MASK},
  {"rxchain", 0x000000f0, SRFL_NOFFS, SROM4_TXRXC, SROM4_RXCHAIN_MASK},
  {"antswitch", 0x000000f0, SRFL_NOFFS, SROM4_TXRXC, SROM4_SWITCH_MASK},
  {"txchain", 0xffffff00, SRFL_NOFFS, SROM8_TXRXC, SROM4_TXCHAIN_MASK},
  {"rxchain", 0xffffff00, SRFL_NOFFS, SROM8_TXRXC, SROM4_RXCHAIN_MASK},
  {"antswitch", 0xffffff00, SRFL_NOFFS, SROM8_TXRXC, SROM4_SWITCH_MASK},
  {"txpid2ga0", 0x000000f0, 0, SROM4_TXPID2G, 0xff},
  {"txpid2ga1", 0x000000f0, 0, SROM4_TXPID2G, 0xff00},
  {"txpid2ga2", 0x000000f0, 0, SROM4_TXPID2G + 1, 0xff},
  {"txpid2ga3", 0x000000f0, 0, SROM4_TXPID2G + 1, 0xff00},
  {"txpid5ga0", 0x000000f0, 0, SROM4_TXPID5G, 0xff},
  {"txpid5ga1", 0x000000f0, 0, SROM4_TXPID5G, 0xff00},
  {"txpid5ga2", 0x000000f0, 0, SROM4_TXPID5G + 1, 0xff},
  {"txpid5ga3", 0x000000f0, 0, SROM4_TXPID5G + 1, 0xff00},
  {"txpid5gla0", 0x000000f0, 0, SROM4_TXPID5GL, 0xff},
  {"txpid5gla1", 0x000000f0, 0, SROM4_TXPID5GL, 0xff00},
  {"txpid5gla2", 0x000000f0, 0, SROM4_TXPID5GL + 1, 0xff},
  {"txpid5gla3", 0x000000f0, 0, SROM4_TXPID5GL + 1, 0xff00},
  {"txpid5gha0", 0x000000f0, 0, SROM4_TXPID5GH, 0xff},
  {"txpid5gha1", 0x000000f0, 0, SROM4_TXPID5GH, 0xff00},
  {"txpid5gha2", 0x000000f0, 0, SROM4_TXPID5GH + 1, 0xff},
  {"txpid5gha3", 0x000000f0, 0, SROM4_TXPID5GH + 1, 0xff00},
  {"cck2gpo", 0x000000f0, 0, SROM4_2G_CCKPO, 0xffff},
  {"cck2gpo", 0xffffff00, 0, SROM8_2G_CCKPO, 0xffff},
  {"ofdm2gpo", 0x000000f0, SRFL_MORE, SROM4_2G_OFDMPO, 0xffff},
  {"", 0, 0, SROM4_2G_OFDMPO + 1, 0xffff},
  {"ofdm5gpo", 0x000000f0, SRFL_MORE, SROM4_5G_OFDMPO, 0xffff},
  {"", 0, 0, SROM4_5G_OFDMPO + 1, 0xffff},
  {"ofdm5glpo", 0x000000f0, SRFL_MORE, SROM4_5GL_OFDMPO, 0xffff},
  {"", 0, 0, SROM4_5GL_OFDMPO + 1, 0xffff},
  {"ofdm5ghpo", 0x000000f0, SRFL_MORE, SROM4_5GH_OFDMPO, 0xffff},
  {"", 0, 0, SROM4_5GH_OFDMPO + 1, 0xffff},
  {"ofdm2gpo", 0xffffff00, SRFL_MORE, SROM8_2G_OFDMPO, 0xffff},
  {"", 0, 0, SROM8_2G_OFDMPO + 1, 0xffff},
  {"ofdm5gpo", 0xffffff00, SRFL_MORE, SROM8_5G_OFDMPO, 0xffff},
  {"", 0, 0, SROM8_5G_OFDMPO + 1, 0xffff},
  {"ofdm5glpo", 0xffffff00, SRFL_MORE, SROM8_5GL_OFDMPO, 0xffff},
  {"", 0, 0, SROM8_5GL_OFDMPO + 1, 0xffff},
  {"ofdm5ghpo", 0xffffff00, SRFL_MORE, SROM8_5GH_OFDMPO, 0xffff},
  {"", 0, 0, SROM8_5GH_OFDMPO + 1, 0xffff},
  {"mcs2gpo0", 0x000000f0, 0, SROM4_2G_MCSPO, 0xffff},
  {"mcs2gpo1", 0x000000f0, 0, SROM4_2G_MCSPO + 1, 0xffff},
  {"mcs2gpo2", 0x000000f0, 0, SROM4_2G_MCSPO + 2, 0xffff},
  {"mcs2gpo3", 0x000000f0, 0, SROM4_2G_MCSPO + 3, 0xffff},
  {"mcs2gpo4", 0x000000f0, 0, SROM4_2G_MCSPO + 4, 0xffff},
  {"mcs2gpo5", 0x000000f0, 0, SROM4_2G_MCSPO + 5, 0xffff},
  {"mcs2gpo6", 0x000000f0, 0, SROM4_2G_MCSPO + 6, 0xffff},
  {"mcs2gpo7", 0x000000f0, 0, SROM4_2G_MCSPO + 7, 0xffff},
  {"mcs5gpo0", 0x000000f0, 0, SROM4_5G_MCSPO, 0xffff},
  {"mcs5gpo1", 0x000000f0, 0, SROM4_5G_MCSPO + 1, 0xffff},
  {"mcs5gpo2", 0x000000f0, 0, SROM4_5G_MCSPO + 2, 0xffff},
  {"mcs5gpo3", 0x000000f0, 0, SROM4_5G_MCSPO + 3, 0xffff},
  {"mcs5gpo4", 0x000000f0, 0, SROM4_5G_MCSPO + 4, 0xffff},
  {"mcs5gpo5", 0x000000f0, 0, SROM4_5G_MCSPO + 5, 0xffff},
  {"mcs5gpo6", 0x000000f0, 0, SROM4_5G_MCSPO + 6, 0xffff},
  {"mcs5gpo7", 0x000000f0, 0, SROM4_5G_MCSPO + 7, 0xffff},
  {"mcs5glpo0", 0x000000f0, 0, SROM4_5GL_MCSPO, 0xffff},
  {"mcs5glpo1", 0x000000f0, 0, SROM4_5GL_MCSPO + 1, 0xffff},
  {"mcs5glpo2", 0x000000f0, 0, SROM4_5GL_MCSPO + 2, 0xffff},
  {"mcs5glpo3", 0x000000f0, 0, SROM4_5GL_MCSPO + 3, 0xffff},
  {"mcs5glpo4", 0x000000f0, 0, SROM4_5GL_MCSPO + 4, 0xffff},
  {"mcs5glpo5", 0x000000f0, 0, SROM4_5GL_MCSPO + 5, 0xffff},
  {"mcs5glpo6", 0x000000f0, 0, SROM4_5GL_MCSPO + 6, 0xffff},
  {"mcs5glpo7", 0x000000f0, 0, SROM4_5GL_MCSPO + 7, 0xffff},
  {"mcs5ghpo0", 0x000000f0, 0, SROM4_5GH_MCSPO, 0xffff},
  {"mcs5ghpo1", 0x000000f0, 0, SROM4_5GH_MCSPO + 1, 0xffff},
  {"mcs5ghpo2", 0x000000f0, 0, SROM4_5GH_MCSPO + 2, 0xffff},
  {"mcs5ghpo3", 0x000000f0, 0, SROM4_5GH_MCSPO + 3, 0xffff},
  {"mcs5ghpo4", 0x000000f0, 0, SROM4_5GH_MCSPO + 4, 0xffff},
  {"mcs5ghpo5", 0x000000f0, 0, SROM4_5GH_MCSPO + 5, 0xffff},
  {"mcs5ghpo6", 0x000000f0, 0, SROM4_5GH_MCSPO + 6, 0xffff},
  {"mcs5ghpo7", 0x000000f0, 0, SROM4_5GH_MCSPO + 7, 0xffff},
  {"mcs2gpo0", 0xffffff00, 0, SROM8_2G_MCSPO, 0xffff},
  {"mcs2gpo1", 0xffffff00, 0, SROM8_2G_MCSPO + 1, 0xffff},
  {"mcs2gpo2", 0xffffff00, 0, SROM8_2G_MCSPO + 2, 0xffff},
  {"mcs2gpo3", 0xffffff00, 0, SROM8_2G_MCSPO + 3, 0xffff},
  {"mcs2gpo4", 0xffffff00, 0, SROM8_2G_MCSPO + 4, 0xffff},
  {"mcs2gpo5", 0xffffff00, 0, SROM8_2G_MCSPO + 5, 0xffff},
  {"mcs2gpo6", 0xffffff00, 0, SROM8_2G_MCSPO + 6, 0xffff},
  {"mcs2gpo7", 0xffffff00, 0, SROM8_2G_MCSPO + 7, 0xffff},
  {"mcs5gpo0", 0xffffff00, 0, SROM8_5G_MCSPO, 0xffff},
  {"mcs5gpo1", 0xffffff00, 0, SROM8_5G_MCSPO + 1, 0xffff},
  {"mcs5gpo2", 0xffffff00, 0, SROM8_5G_MCSPO + 2, 0xffff},
  {"mcs5gpo3", 0xffffff00, 0, SROM8_5G_MCSPO + 3, 0xffff},
  {"mcs5gpo4", 0xffffff00, 0, SROM8_5G_MCSPO + 4, 0xffff},
  {"mcs5gpo5", 0xffffff00, 0, SROM8_5G_MCSPO + 5, 0xffff},
  {"mcs5gpo6", 0xffffff00, 0, SROM8_5G_MCSPO + 6, 0xffff},
  {"mcs5gpo7", 0xffffff00, 0, SROM8_5G_MCSPO + 7, 0xffff},
  {"mcs5glpo0", 0xffffff00, 0, SROM8_5GL_MCSPO, 0xffff},
  {"mcs5glpo1", 0xffffff00, 0, SROM8_5GL_MCSPO + 1, 0xffff},
  {"mcs5glpo2", 0xffffff00, 0, SROM8_5GL_MCSPO + 2, 0xffff},
  {"mcs5glpo3", 0xffffff00, 0, SROM8_5GL_MCSPO + 3, 0xffff},
  {"mcs5glpo4", 0xffffff00, 0, SROM8_5GL_MCSPO + 4, 0xffff},
  {"mcs5glpo5", 0xffffff00, 0, SROM8_5GL_MCSPO + 5, 0xffff},
  {"mcs5glpo6", 0xffffff00, 0, SROM8_5GL_MCSPO + 6, 0xffff},
  {"mcs5glpo7", 0xffffff00, 0, SROM8_5GL_MCSPO + 7, 0xffff},
  {"mcs5ghpo0", 0xffffff00, 0, SROM8_5GH_MCSPO, 0xffff},
  {"mcs5ghpo1", 0xffffff00, 0, SROM8_5GH_MCSPO + 1, 0xffff},
  {"mcs5ghpo2", 0xffffff00, 0, SROM8_5GH_MCSPO + 2, 0xffff},
  {"mcs5ghpo3", 0xffffff00, 0, SROM8_5GH_MCSPO + 3, 0xffff},
  {"mcs5ghpo4", 0xffffff00, 0, SROM8_5GH_MCSPO + 4, 0xffff},
  {"mcs5ghpo5", 0xffffff00, 0, SROM8_5GH_MCSPO + 5, 0xffff},
  {"mcs5ghpo6", 0xffffff00, 0, SROM8_5GH_MCSPO + 6, 0xffff},
  {"mcs5ghpo7", 0xffffff00, 0, SROM8_5GH_MCSPO + 7, 0xffff},
  {"cddpo", 0x000000f0, 0, SROM4_CDDPO, 0xffff},
  {"stbcpo", 0x000000f0, 0, SROM4_STBCPO, 0xffff},
  {"bw40po", 0x000000f0, 0, SROM4_BW40PO, 0xffff},
  {"bwduppo", 0x000000f0, 0, SROM4_BWDUPPO, 0xffff},
  {"cddpo", 0xffffff00, 0, SROM8_CDDPO, 0xffff},
  {"stbcpo", 0xffffff00, 0, SROM8_STBCPO, 0xffff},
  {"bw40po", 0xffffff00, 0, SROM8_BW40PO, 0xffff},
  {"bwduppo", 0xffffff00, 0, SROM8_BWDUPPO, 0xffff},
  {"ccode", 0x0000000f, SRFL_CCODE, SROM_CCODE, 0xffff},
  {"ccode", 0x00000010, SRFL_CCODE, SROM4_CCODE, 0xffff},
  {"ccode", 0x000000e0, SRFL_CCODE, SROM5_CCODE, 0xffff},
  {"ccode", 0xffffff00, SRFL_CCODE, SROM8_CCODE, 0xffff},
  {"macaddr", 0xffffff00, SRFL_ETHADDR, SROM8_MACHI, 0xffff},
  {"macaddr", 0x000000e0, SRFL_ETHADDR, SROM5_MACHI, 0xffff},
  {"macaddr", 0x00000010, SRFL_ETHADDR, SROM4_MACHI, 0xffff},
  {"macaddr", 0x00000008, SRFL_ETHADDR, SROM3_MACHI, 0xffff},
  {"il0macaddr", 0x00000007, SRFL_ETHADDR, SROM_MACHI_IL0, 0xffff},
  {"et1macaddr", 0x00000007, SRFL_ETHADDR, SROM_MACHI_ET1, 0xffff},
  {"leddc", 0xffffff00, SRFL_NOFFS | SRFL_LEDDC, SROM8_LEDDC, 0xffff},
  {"leddc", 0x000000e0, SRFL_NOFFS | SRFL_LEDDC, SROM5_LEDDC, 0xffff},
  {"leddc", 0x00000010, SRFL_NOFFS | SRFL_LEDDC, SROM4_LEDDC, 0xffff},
  {"leddc", 0x00000008, SRFL_NOFFS | SRFL_LEDDC, SROM3_LEDDC, 0xffff},
  {NULL, 0, 0, 0, 0}
};

static const sromvar_t perpath_pci_sromvars[] = {
  {"maxp2ga", 0x000000f0, 0, SROM4_2G_ITT_MAXP, 0xff},
  {"itt2ga", 0x000000f0, 0, SROM4_2G_ITT_MAXP, 0xff00},
  {"itt5ga", 0x000000f0, 0, SROM4_5G_ITT_MAXP, 0xff00},
  {"pa2gw0a", 0x000000f0, SRFL_PRHEX, SROM4_2G_PA, 0xffff},
  {"pa2gw1a", 0x000000f0, SRFL_PRHEX, SROM4_2G_PA + 1, 0xffff},
  {"pa2gw2a", 0x000000f0, SRFL_PRHEX, SROM4_2G_PA + 2, 0xffff},
  {"pa2gw3a", 0x000000f0, SRFL_PRHEX, SROM4_2G_PA + 3, 0xffff},
  {"maxp5ga", 0x000000f0, 0, SROM4_5G_ITT_MAXP, 0xff},
  {"maxp5gha", 0x000000f0, 0, SROM4_5GLH_MAXP, 0xff},
  {"maxp5gla", 0x000000f0, 0, SROM4_5GLH_MAXP, 0xff00},
  {"pa5gw0a", 0x000000f0, SRFL_PRHEX, SROM4_5G_PA, 0xffff},
  {"pa5gw1a", 0x000000f0, SRFL_PRHEX, SROM4_5G_PA + 1, 0xffff},
  {"pa5gw2a", 0x000000f0, SRFL_PRHEX, SROM4_5G_PA + 2, 0xffff},
  {"pa5gw3a", 0x000000f0, SRFL_PRHEX, SROM4_5G_PA + 3, 0xffff},
  {"pa5glw0a", 0x000000f0, SRFL_PRHEX, SROM4_5GL_PA, 0xffff},
  {"pa5glw1a", 0x000000f0, SRFL_PRHEX, SROM4_5GL_PA + 1, 0xffff},
  {"pa5glw2a", 0x000000f0, SRFL_PRHEX, SROM4_5GL_PA + 2, 0xffff},
  {"pa5glw3a", 0x000000f0, SRFL_PRHEX, SROM4_5GL_PA + 3, 0xffff},
  {"pa5ghw0a", 0x000000f0, SRFL_PRHEX, SROM4_5GH_PA, 0xffff},
  {"pa5ghw1a", 0x000000f0, SRFL_PRHEX, SROM4_5GH_PA + 1, 0xffff},
  {"pa5ghw2a", 0x000000f0, SRFL_PRHEX, SROM4_5GH_PA + 2, 0xffff},
  {"pa5ghw3a", 0x000000f0, SRFL_PRHEX, SROM4_5GH_PA + 3, 0xffff},
  {"maxp2ga", 0xffffff00, 0, SROM8_2G_ITT_MAXP, 0xff},
  {"itt2ga", 0xffffff00, 0, SROM8_2G_ITT_MAXP, 0xff00},
  {"itt5ga", 0xffffff00, 0, SROM8_5G_ITT_MAXP, 0xff00},
  {"pa2gw0a", 0xffffff00, SRFL_PRHEX, SROM8_2G_PA, 0xffff},
  {"pa2gw1a", 0xffffff00, SRFL_PRHEX, SROM8_2G_PA + 1, 0xffff},
  {"pa2gw2a", 0xffffff00, SRFL_PRHEX, SROM8_2G_PA + 2, 0xffff},
  {"maxp5ga", 0xffffff00, 0, SROM8_5G_ITT_MAXP, 0xff},
  {"maxp5gha", 0xffffff00, 0, SROM8_5GLH_MAXP, 0xff},
  {"maxp5gla", 0xffffff00, 0, SROM8_5GLH_MAXP, 0xff00},
  {"pa5gw0a", 0xffffff00, SRFL_PRHEX, SROM8_5G_PA, 0xffff},
  {"pa5gw1a", 0xffffff00, SRFL_PRHEX, SROM8_5G_PA + 1, 0xffff},
  {"pa5gw2a", 0xffffff00, SRFL_PRHEX, SROM8_5G_PA + 2, 0xffff},
  {"pa5glw0a", 0xffffff00, SRFL_PRHEX, SROM8_5GL_PA, 0xffff},
  {"pa5glw1a", 0xffffff00, SRFL_PRHEX, SROM8_5GL_PA + 1, 0xffff},
  {"pa5glw2a", 0xffffff00, SRFL_PRHEX, SROM8_5GL_PA + 2, 0xffff},
  {"pa5ghw0a", 0xffffff00, SRFL_PRHEX, SROM8_5GH_PA, 0xffff},
  {"pa5ghw1a", 0xffffff00, SRFL_PRHEX, SROM8_5GH_PA + 1, 0xffff},
  {"pa5ghw2a", 0xffffff00, SRFL_PRHEX, SROM8_5GH_PA + 2, 0xffff},
  {NULL, 0, 0, 0, 0}
};

/* Parse SROM and create name=value pairs. 'srom' points to
 * the SROM word array. 'off' specifies the offset of the
 * first word 'srom' points to, which should be either 0 or
 * SROM3_SWRG_OFF (full SROM or software region).
 */

static uint
mask_shift (uint16 mask)
{
  uint i;
  for (i = 0; i < (sizeof (mask) << 3); i++)
    {
      if (mask & (1 << i))
	return i;
    }
  ASSERT (mask);
  return 0;
}

static uint
mask_width (uint16 mask)
{
  int i;
  for (i = (sizeof (mask) << 3) - 1; i >= 0; i--)
    {
      if (mask & (1 << i))
	return (uint) (i - mask_shift (mask) + 1);
    }
  ASSERT (mask);
  return 0;
}

#ifdef BCMDBG_ASSERT
static bool
mask_valid (uint16 mask)
{
  uint shift = mask_shift (mask);
  uint width = mask_width (mask);
  return mask == ((~0 << shift) & ~(~0 << (shift + width)));
}
#endif

static void
_initvars_srom_pci (uint8 sromrev, uint16 * srom, uint off, varbuf_t * b)
{
  uint16 w;
  uint32 val;
  const sromvar_t *srv;
  uint width;
  uint flags;
  uint32 sr = (1 << sromrev);

  varbuf_append (b, "sromrev=%d", sromrev);

  for (srv = pci_sromvars; srv->name != NULL; srv++)
    {
      const char *name;

      if ((srv->revmask & sr) == 0)
	continue;

      if (srv->off < off)
	continue;

      flags = srv->flags;
      name = srv->name;

      if (flags & SRFL_ETHADDR)
	{
	  char eabuf[ETHER_ADDR_STR_LEN];
	  struct ether_addr ea;

	  ea.octet[0] = (srom[srv->off - off] >> 8) & 0xff;
	  ea.octet[1] = srom[srv->off - off] & 0xff;
	  ea.octet[2] = (srom[srv->off + 1 - off] >> 8) & 0xff;
	  ea.octet[3] = srom[srv->off + 1 - off] & 0xff;
	  ea.octet[4] = (srom[srv->off + 2 - off] >> 8) & 0xff;
	  ea.octet[5] = srom[srv->off + 2 - off] & 0xff;
	  bcm_ether_ntoa (&ea, eabuf);

	  varbuf_append (b, "%s=%s", name, eabuf);
	}
      else
	{
	  ASSERT (mask_valid (srv->mask));
	  ASSERT (mask_width (srv->mask));

	  w = srom[srv->off - off];
	  val = (w & srv->mask) >> mask_shift (srv->mask);
	  width = mask_width (srv->mask);

	  while (srv->flags & SRFL_MORE)
	    {
	      srv++;
	      ASSERT (srv->name);

	      if (srv->off == 0 || srv->off < off)
		continue;

	      ASSERT (mask_valid (srv->mask));
	      ASSERT (mask_width (srv->mask));

	      w = srom[srv->off - off];
	      val += ((w & srv->mask) >> mask_shift (srv->mask)) << width;
	      width += mask_width (srv->mask);
	    }

	  if ((flags & SRFL_NOFFS) && ((int) val == (1 << width) - 1))
	    continue;

	  if (flags & SRFL_CCODE)
	    {
	      if (val == 0)
		varbuf_append (b, "ccode=");
	      else
		varbuf_append (b, "ccode=%c%c", (val >> 8), (val & 0xff));
	    }
	  /* LED Powersave duty cycle has to be scaled:
	   *(oncount >> 24) (offcount >> 8)
	   */
	  else if (flags & SRFL_LEDDC)
	    {
	      uint32 w32 = (((val >> 8) & 0xff) << 24) |	/* oncount */
		(((val & 0xff)) << 8);	/* offcount */
	      varbuf_append (b, "leddc=%d", w32);
	    }
	  else if (flags & SRFL_PRHEX)
	    varbuf_append (b, "%s=0x%x", name, val);
	  else if ((flags & SRFL_PRSIGN) && (val & (1 << (width - 1))))
	    varbuf_append (b, "%s=%d", name, (int) (val | (~0 << width)));
	  else
	    varbuf_append (b, "%s=%u", name, val);
	}
    }

  if (sromrev >= 4)
    {
      /* Do per-path variables */
      uint p, pb, psz;

      if (sromrev >= 8)
	{
	  pb = SROM8_PATH0;
	  psz = SROM8_PATH1 - SROM8_PATH0;
	}
      else
	{
	  pb = SROM4_PATH0;
	  psz = SROM4_PATH1 - SROM4_PATH0;
	}

      for (p = 0; p < MAX_PATH; p++)
	{
	  for (srv = perpath_pci_sromvars; srv->name != NULL; srv++)
	    {
	      if ((srv->revmask & sr) == 0)
		continue;

	      if (pb + srv->off < off)
		continue;

	      w = srom[pb + srv->off - off];
	      ASSERT (mask_valid (srv->mask));
	      val = (w & srv->mask) >> mask_shift (srv->mask);
	      width = mask_width (srv->mask);

	      /* Cheating: no per-path var is more than 1 word */

	      if ((srv->flags & SRFL_NOFFS)
		  && ((int) val == (1 << width) - 1))
		continue;

	      if (srv->flags & SRFL_PRHEX)
		varbuf_append (b, "%s%d=0x%x", srv->name, p, val);
	      else
		varbuf_append (b, "%s%d=%d", srv->name, p, val);
	    }
	  pb += psz;
	}
    }
}

static int
initvars_srom_pci (sb_t * sbh, void *curmap, char **vars, uint * count)
{
  uint16 *srom;
  uint8 sromrev = 0;
  uint32 sr;
  varbuf_t b;
  char *vp, *base = NULL;
  osl_t *osh = sb_osh (sbh);
  bool flash = FALSE;
  char *value;
  int err;

  /*
   * Apply CRC over SROM content regardless SROM is present or not,
   * and use variable <devpath>sromrev's existance in flash to decide
   * if we should return an error when CRC fails or read SROM variables
   * from flash.
   */
  srom = MALLOC (osh, SROM_MAX);
  ASSERT (srom);
  if (!srom)
    return -2;

  err =
    sprom_read_pci (osh, (void *) ((int8 *) curmap + PCI_BAR0_SPROM_OFFSET),
		    0, srom, SROM_WORDS, TRUE);

  if ((srom[SROM4_SIGN] == SROM4_SIGNATURE) ||
      ((sbh->buscoretype == SB_PCIE) && (sbh->buscorerev >= 6)))
    {
      /* sromrev >= 4, read more */
      err =
	sprom_read_pci (osh,
			(void *) ((int8 *) curmap + PCI_BAR0_SPROM_OFFSET), 0,
			srom, SROM4_WORDS, TRUE);
      sromrev = srom[SROM4_CRCREV] & 0xff;
    }
  else if (err == 0)
    {
      /* srom is good and is rev < 4 */
      /* top word of sprom contains version and crc8 */
      sromrev = srom[SROM_CRCREV] & 0xff;
      /* bcm4401 sroms misprogrammed */
      if (sromrev == 0x10)
	sromrev = 1;
    }

  if (err)
    {
#ifdef WLTEST
      uint32 val;

      BS_ERROR (("SROM Crc Error, so see if we could use a default\n"));
      val = OSL_PCI_READ_CONFIG (osh, PCI_SPROM_CONTROL, sizeof (uint32));
      if (val & SPROM_OTPIN_USE)
	{
	  BS_ERROR (("srom crc failed with OTP, use default vars....\n"));
	  vp = base = mfgsromvars;
	  if (sb_chip (sbh) == BCM4311_CHIP_ID)
	    {
	      const char *devid = "devid=0x4311";
	      const size_t devid_strlen = strlen (devid);
	      BS_ERROR (("setting the devid to be 4311\n"));
	      bcopy (devid, vp, devid_strlen + 1);
	      vp += devid_strlen + 1;
	    }
	  bcopy (defaultsromvars, vp, MFGSROM_DEFVARSLEN);
	  vp += MFGSROM_DEFVARSLEN;
	  goto varsdone;
	}
      else
	{
#endif /* WLTEST */
	  BS_ERROR (("srom crc failed with SPROM....\n"));
	  if (!(value = sb_getdevpathvar (sbh, "sromrev")))
	    {
	      err = -1;
	      goto errout;
	    }
	  sromrev = (uint8) simple_strtoul (value, NULL, 0);
	  flash = TRUE;
#ifdef WLTEST
	}
#endif /* WLTEST */
    }

  /* Bitmask for the sromrev */
  sr = 1 << sromrev;

  /* srom version check
   * Current valid versions: 1, 2, 3, 4, 5, 8
   */
  if ((sr & 0x13e) == 0)
    {
      err = -2;
      goto errout;
    }

  ASSERT (vars);
  ASSERT (count);

  base = vp = MALLOC (osh, MAXSZ_NVRAM_VARS);
  ASSERT (vp);
  if (!vp)
    {
      err = -2;
      goto errout;
    }

  /* read variables from flash */
  if (flash)
    {
      if ((err = initvars_flash (sbh, osh, &vp, MAXSZ_NVRAM_VARS)))
	goto errout;
      goto varsdone;
    }

  varbuf_init (&b, base, MAXSZ_NVRAM_VARS);

  /* parse SROM into name=value pairs. */
  _initvars_srom_pci (sromrev, srom, 0, &b);

  /* final nullbyte terminator */
  ASSERT (b.size >= 1);
  vp = b.buf;
  *vp++ = '\0';

  ASSERT ((vp - base) <= MAXSZ_NVRAM_VARS);

varsdone:
  err = initvars_table (osh, base, vp, vars, count);

errout:
#ifdef WLTEST
  if (base && (base != mfgsromvars))
#else
  if (base)
#endif
    MFREE (osh, base, MAXSZ_NVRAM_VARS);

  MFREE (osh, srom, SROM_MAX);
  return err;
}

/*
 * Read the cis and call parsecis to initialize the vars.
 * Return 0 on success, nonzero on error.
 */
static int
initvars_cis_pcmcia (sb_t * sbh, osl_t * osh, char **vars, uint * count)
{
  uint8 *cis = NULL;
  int rc;
  uint data_sz;

  data_sz = (sb_pcmciarev (sbh) == 1) ? (SPROM_SIZE * 2) : CIS_SIZE;

  if ((cis = MALLOC (osh, data_sz)) == NULL)
    return (-2);

  if (sb_pcmciarev (sbh) == 1)
    {
      if (srom_read
	  (sbh, PCMCIA_BUS, (void *) NULL, osh, 0, data_sz, (uint16 *) cis))
	{
	  MFREE (osh, cis, data_sz);
	  return (-1);
	}
      /* fix up endianess for 16-bit data vs 8-bit parsing */
      htol16_buf ((uint16 *) cis, data_sz);
    }
  else
    OSL_PCMCIA_READ_ATTR (osh, 0, cis, data_sz);

  rc = srom_parsecis (osh, &cis, 1, vars, count);

  MFREE (osh, cis, data_sz);

  return (rc);
}


static int
BCMINITFN (initvars_srom_sb) (sb_t * sbh, osl_t * osh, void *curmap,
			      char **vars, uint * varsz)
{
#if defined(BCMSDIODEV)
  /* CIS is read and supplied by the host */
  return BCME_OK;
#elif defined(BCMUSBDEV)
  static bool srvars = FALSE;	/* Use OTP/SPROM as global variables */

  int sel = 0;			/* where to read the srom. 0 - nowhere, 1 - otp, 2 - sprom */
  uint sz = 0;			/* srom size in bytes */
  void *oh = NULL;
  int rc = BCME_OK;

  /* Bail out if we've dealt with OTP/SPROM before! */
  if (srvars)
    return 0;

#if defined(BCM4328)
  if (sbh->chip == BCM4328_CHIP_ID)
    {
      /* Access the SPROM if it is present */
      if ((sz = srom_size (sbh, osh)) != 0)
	{
	  sz <<= 1;
	  sel = 2;
	}
    }
#endif
#if defined(BCM4325)
  if (sbh->chip == BCM4325_CHIP_ID)
    {
      uint32 cst = sbh->chipst & CST4325_SPROM_OTP_SEL_MASK;

      /* Access OTP if it is present, powered on, and programmed */
      if ((oh = otp_init (sbh)) != NULL && (otp_status (oh) & OTPS_GUP_SW))
	{
	  sz = otp_size (oh);
	  sel = 1;
	}
      /* Access the SPROM if it is present and allow to be accessed */
      else if ((cst == CST4325_OTP_PWRDN || cst == CST4325_SPROM_SEL) &&
	       (sz = srom_size (sbh, osh)) != 0)
	{
	  sz <<= 1;
	  sel = 2;
	}
    }
#endif /* BCM4325 */

  /* Read CIS in OTP/SPROM */
  if (sel != 0)
    {
      uint16 *srom;
      uint8 *body = NULL;

      ASSERT (sz);

      /* Allocate memory */
      if ((srom = (uint16 *) MALLOC (osh, sz)) == NULL)
	return BCME_NOMEM;

      /* Read CIS */
      switch (sel)
	{
	case 1:
	  rc = otp_read_region (oh, OTP_SW_RGN, srom, sz);
	  body = (uint8 *) srom;
	  break;
	case 2:
	  rc = srom_read (sbh, SB_BUS, curmap, osh, 0, sz, srom);
	  /* sprom has 8 byte h/w header */
	  body = (uint8 *) srom + SBSDIO_SPROM_CIS_OFFSET;
	  break;
	default:
	  /* impossible to come here */
	  ASSERT (0);
	  break;
	}

      /* Parse CIS */
      if (rc == BCME_OK)
	{
	  uint i, tpls = 0xffffffff;
	  /* # sdiod fns + common + extra */
	  uint8 *cis[SBSDIO_NUM_FUNCTION + 2];
	  uint ciss = 0;

	  /* each word is in host endian */
	  htol16_buf ((uint8 *) srom, sz);

	  ASSERT (body);

	  /* count cis tuple chains */
	  for (i = 0; i < sz && ciss < ARRAYSIZE (cis) && tpls != 0; i++)
	    {
	      cis[ciss++] = &body[i];
	      for (tpls = 0; i < sz - 1; tpls++)
		{
		  if (body[i++] == CISTPL_END)
		    break;
		  i += body[i] + 1;
		}
	    }

	  /* call parser routine only when there are tuple chains */
	  if (ciss > 1)
	    rc = srom_parsecis (osh, cis, ciss, vars, varsz);
	}

      /* Clean up */
      MFREE (osh, srom, sz);

      /* Make SROM variables global */
      if (rc == BCME_OK)
	{
	  rc = nvram_append ((void *) sbh, *vars, *varsz);
	  srvars = TRUE;

	  /* Tell the caller there is no individual SROM variables */
	  *vars = NULL;
	  *varsz = 0;
	}
    }

  return rc;
#else /* !BCMUSBDEV && !BCMSDIODEV */
  /* Search flash nvram section for srom variables */
  return initvars_flash_sb (sbh, vars, varsz);
#endif /* !BCMUSBDEV && !BCMSDIODEV */
}

#ifdef BCMUSBDEV
/* Return sprom size in 16-bit words */
static uint
srom_size (sb_t * sbh, osl_t * osh)
{
  uint size = 0;
  if (SPROMBUS == PCMCIA_BUS)
    {
      uint32 origidx;
      sdpcmd_regs_t *pcmregs;
      bool wasup;

      origidx = sb_coreidx (sbh);
      pcmregs = sb_setcore (sbh, SB_PCMCIA, 0);
      ASSERT (pcmregs);

      if (!(wasup = sb_iscoreup (sbh)))
	sb_core_reset (sbh, 0, 0);

      /* not worry about earlier core revs */
      if (sb_corerev (sbh) < 8)
	goto done;

      /* SPROM is accessible only in PCMCIA mode unless there is SDIO clock */
      if (!(R_REG (osh, &pcmregs->corestatus) & CS_PCMCIAMODE))
	goto done;

      switch (SB_PCMCIA_READ (osh, pcmregs, SROM_INFO) & SRI_SZ_MASK)
	{
	case 1:
	  size = 256;		/* SROM_INFO == 1 means 4kbit */
	  break;
	case 2:
	  size = 1024;		/* SROM_INFO == 2 means 16kbit */
	  break;
	default:
	  break;
	}

    done:
      if (!wasup)
	sb_core_disable (sbh, 0);

      sb_setcoreidx (sbh, origidx);
    }
  return size;
}
#endif /* def BCMUSBDEV */
