/*
 *  Misc useful routines to access NIC SROM
 *
 * Copyright 2004, Broadcom Corporation      
 * All Rights Reserved.      
 *       
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY      
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM      
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS      
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.      
 * $Id$
 */

#include <typedefs.h>
#include <osl.h>
#include <bcmutils.h>
#include <bcmsrom.h>
#include <bcmdevs.h>
#include <bcmendian.h>
#include <sbpcmcia.h>
#include <pcicfg.h>
#include <sbutils.h>

#include <proto/ethernet.h>	/* for sprom content groking */

#define	VARS_MAX	4096	/* should be reduced */

static int initvars_srom_pci(void *curmap, char **vars, int *count);
static int initvars_cis_pcmcia(void *sbh, void *curmap, void *osh, char **vars, int *count);
static int sprom_cmd_pcmcia(void *osh, uint8 cmd);
static int sprom_read_pcmcia(void *osh, uint16 addr, uint16 *data);
static int sprom_write_pcmcia(void *osh, uint16 addr, uint16 data);
static int sprom_read_pci(uint16 *sprom, uint byteoff, uint16 *buf, uint nbytes, bool check_crc);

/*
 * Initialize the vars from the right source for this platform.
 * Return 0 on success, nonzero on error.
 */
int
srom_var_init(void *sbh, uint bus, void *curmap, void *osh, char **vars, int *count)
{
	if (vars == NULL)
		return (0);

	switch (bus) {
	case SB_BUS:
		/* These two could be asserts ... */
		*vars = NULL;
		*count = 0;
		return(0);

	case PCI_BUS:
		ASSERT(curmap);	/* can not be NULL */
		return(initvars_srom_pci(curmap, vars, count));

	case PCMCIA_BUS:
		return(initvars_cis_pcmcia(sbh, curmap, osh, vars, count));


	default:
		ASSERT(0);
	}
	return (-1);
}


/* support only 16-bit word read from srom */
int
srom_read(uint bus, void *curmap, void *osh, uint byteoff, uint nbytes, uint16 *buf)
{
	void *srom;
	uint i, off, nw;

	/* check input - 16-bit access only */
	if (byteoff & 1 || nbytes & 1 || (byteoff + nbytes) > (SPROM_SIZE * 2))
		return 1;

	if (bus == PCI_BUS) {
		if (!curmap)
			return 1;
		srom = (void *)((uint)curmap + PCI_BAR0_SPROM_OFFSET);
		if (sprom_read_pci(srom, byteoff, buf, nbytes, FALSE))
			return 1;
	} else if (bus == PCMCIA_BUS) {
		off = byteoff / 2;
		nw = nbytes / 2;
		for (i = 0; i < nw; i++) {
			if (sprom_read_pcmcia(osh, (uint16)(off + i), (uint16*)(buf + i)))
				return 1;
		}
	} else {
		return 1;
	}

	return 0;
}

/* support only 16-bit word write into srom */
int
srom_write(uint bus, void *curmap, void *osh, uint byteoff, uint nbytes, uint16 *buf)
{
	uint16 *srom;
	uint i, off, nw, crc_range;
	uint16 image[SPROM_SIZE], *p;
	uint8 crc;
	volatile uint32 val32;

	/* check input - 16-bit access only */
	if (byteoff & 1 || nbytes & 1 || (byteoff + nbytes) > (SPROM_SIZE * 2))
		return 1;

	crc_range = ((bus == PCMCIA_BUS) ? SPROM_SIZE : SPROM_CRC_RANGE) * 2;

	/* if changes made inside crc cover range */
	if (byteoff < crc_range) {
		nw = (((byteoff + nbytes) > crc_range) ? byteoff + nbytes : crc_range) / 2;
		/* read data including entire first 64 words from srom */
		if (srom_read(bus, curmap, osh, 0, nw * 2, image))
			return 1;
		/* make changes */
		bcopy((void*)buf, (void*)&image[byteoff / 2], nbytes);
		/* calculate crc */
		htol16_buf(image, crc_range);
		crc = ~crc8((uint8 *)image, crc_range - 1, CRC8_INIT_VALUE);
		ltoh16_buf(image, crc_range);
		image[(crc_range / 2) - 1] = (crc << 8) | (image[(crc_range / 2) - 1] & 0xff);
		p = image;
		off = 0;
	} else {
		p = buf;
		off = byteoff / 2;
		nw = nbytes / 2;
	}

	if (bus == PCI_BUS) {
		srom = (uint16*)((uint)curmap + PCI_BAR0_SPROM_OFFSET);
		/* enable writes to the SPROM */
		val32 = OSL_PCI_READ_CONFIG(osh, PCI_SPROM_CONTROL, sizeof(uint32));
		val32 |= SPROM_WRITEEN;
		OSL_PCI_WRITE_CONFIG(osh, PCI_SPROM_CONTROL, sizeof(uint32), val32);
		bcm_mdelay(500);
		/* write srom */
		for (i = 0; i < nw; i++) {
			W_REG(&srom[off + i], p[i]);
			bcm_mdelay(20);
		}
		/* disable writes to the SPROM */
		OSL_PCI_WRITE_CONFIG(osh, PCI_SPROM_CONTROL, sizeof(uint32), val32 & ~SPROM_WRITEEN);
	} else if (bus == PCMCIA_BUS) {
		/* enable writes to the SPROM */
		if (sprom_cmd_pcmcia(osh, SROM_WEN))
			return 1;
		bcm_mdelay(500);
		/* write srom */
		for (i = 0; i < nw; i++) {
			sprom_write_pcmcia(osh, (uint16)(off + i), p[i]);
			bcm_mdelay(20);
		}
		/* disable writes to the SPROM */
		if (sprom_cmd_pcmcia(osh, SROM_WDS))
			return 1;
	} else {
		return 1;
	}

	bcm_mdelay(500);
	return 0;
}


int
srom_parsecis(uint8 *cis, char **vars, int *count)
{
	char eabuf[32];
	char *vp, *base;
	uint8 tup, tlen, sromrev = 1;
	int i, j;
	uint varsize;
	bool ag_init = FALSE;
	uint16 w;

	ASSERT(vars);
	ASSERT(count);

	base = vp = MALLOC(VARS_MAX);
	ASSERT(vp);

	i = 0;
	do {
		tup = cis[i++];
		tlen = cis[i++];
		if ((i + tlen) >= CIS_SIZE)
			break;

		switch (tup) {
		case CISTPL_MANFID:
			vp += sprintf(vp, "manfid=%d", (cis[i + 1] << 8) + cis[i]);
			vp++;
			vp += sprintf(vp, "prodid=%d", (cis[i + 3] << 8) + cis[i + 2]);
			vp++;
			break;

		case CISTPL_FUNCE:
			if (cis[i] == LAN_NID) {
				ASSERT(cis[i + 1] == ETHER_ADDR_LEN);
				bcm_ether_ntoa((uchar*)&cis[i + 2], eabuf);
				vp += sprintf(vp, "il0macaddr=%s", eabuf);
				vp++;
			}
			break;

		case CISTPL_CFTABLE:
			vp += sprintf(vp, "regwindowsz=%d", (cis[i + 7] << 8) | cis[i + 6]);
			vp++;
			break;

		case CISTPL_BRCM_HNBU:
			switch (cis[i]) {
			case HNBU_CHIPID:
				vp += sprintf(vp, "vendid=%d", (cis[i + 2] << 8) + cis[i + 1]);
				vp++;
				vp += sprintf(vp, "devid=%d", (cis[i + 4] << 8) + cis[i + 3]);
				vp++;
				if (tlen == 7) {
					vp += sprintf(vp, "chiprev=%d", (cis[i + 6] << 8) + cis[i + 5]);
					vp++;
				}
				break;

			case HNBU_BOARDREV:
				vp += sprintf(vp, "boardrev=%d", cis[i + 1]);
				vp++;
				break;

			case HNBU_AA:
				vp += sprintf(vp, "aa0=%d", cis[i + 1]);
				vp++;
				break;

			case HNBU_AG:
				vp += sprintf(vp, "ag0=%d", cis[i + 1]);
				vp++;
				ag_init = TRUE;
				break;

			case HNBU_CC:
				vp += sprintf(vp, "cc=%d", cis[i + 1]);
				vp++;
				break;

			case HNBU_PAPARMS:
				vp += sprintf(vp, "pa0maxpwr=%d", cis[i + tlen - 1]);
				vp++;
				if (tlen == 9) {
					/* New version */
					for (j = 0; j < 3; j++) {
						vp += sprintf(vp, "pa0b%d=%d", j,
							      (cis[i + (j * 2) + 2] << 8) + cis[i + (j * 2) + 1]);
						vp++;
					}
					vp += sprintf(vp, "pa0itssit=%d", cis[i + 7]);
					vp++;
				}
				break;

			case HNBU_OEM:
				vp += sprintf(vp, "oem=%02x%02x%02x%02x%02x%02x%02x%02x",
					cis[i + 1], cis[i + 2], cis[i + 3], cis[i + 4],
					cis[i + 5], cis[i + 6], cis[i + 7], cis[i + 8]);
				vp++;
				break;
			case HNBU_BOARDFLAGS:
				w = (cis[i + 2] << 8) + cis[i + 1];
				if (w == 0xffff) w = 0;
				vp += sprintf(vp, "boardflags=%d", w);
				vp++;
				break;
			case HNBU_LED:
				if (cis[i + 1] != 0xff) {
					vp += sprintf(vp, "wl0gpio0=%d", cis[i + 1]);
					vp++;
				}
				if (cis[i + 2] != 0xff) {
					vp += sprintf(vp, "wl0gpio1=%d", cis[i + 2]);
					vp++;
				}
				if (cis[i + 3] != 0xff) {
					vp += sprintf(vp, "wl0gpio2=%d", cis[i + 3]);
					vp++;
				}
				if (cis[i + 4] != 0xff) {
					vp += sprintf(vp, "wl0gpio3=%d", cis[i + 4]);
					vp++;
				}
				break;
			}
			break;

		}
		i += tlen;
	} while (tup != 0xff);

	/* Set the srom version */
	vp += sprintf(vp, "sromrev=%d", sromrev);
	vp++;

	/* For now just set boardflags2 to zero */
	vp += sprintf(vp, "boardflags2=0");
	vp++;

	/* if there is no antenna gain field, set default */
	if (ag_init == FALSE) {
		vp += sprintf(vp, "ag0=%d", 0xff);
		vp++;
	}

	/* final nullbyte terminator */
	*vp++ = '\0';
	varsize = (uint)vp - (uint)base;

	ASSERT(varsize < VARS_MAX);

	if (varsize == VARS_MAX) {
		*vars = base;
	} else {
		vp = MALLOC(varsize);
		ASSERT(vp);
		bcopy(base, vp, varsize);
		MFREE(base, VARS_MAX);
		*vars = vp;
	}
	*count = varsize;

	return (0);
}


/* set PCMCIA sprom command register */
static int
sprom_cmd_pcmcia(void *osh, uint8 cmd)
{
	uint8 status;
	uint wait_cnt = 1000;

	/* write sprom command register */
	OSL_PCMCIA_WRITE_ATTR(osh, SROM_CS, &cmd, 1);

	/* wait status */
	while (wait_cnt--) {
		OSL_PCMCIA_READ_ATTR(osh, SROM_CS, &status, 1);
		if (status & SROM_DONE)
			return 0;
	}

	return 1;
}

/* read a word from the PCMCIA srom */
static int
sprom_read_pcmcia(void *osh, uint16 addr, uint16 *data)
{
	uint8 addr_l, addr_h, data_l, data_h;

	addr_l = (uint8)((addr * 2) & 0xff);
	addr_h = (uint8)(((addr * 2) >> 8) & 0xff);

	/* set address */
	OSL_PCMCIA_WRITE_ATTR(osh, SROM_ADDRH, &addr_h, 1);
	OSL_PCMCIA_WRITE_ATTR(osh, SROM_ADDRL, &addr_l, 1);

	/* do read */
	if (sprom_cmd_pcmcia(osh, SROM_READ))
		return 1;

	/* read data */
	OSL_PCMCIA_READ_ATTR(osh, SROM_DATAH, &data_h, 1);
	OSL_PCMCIA_READ_ATTR(osh, SROM_DATAL, &data_l, 1);

	*data = (data_h << 8) | data_l;
	return 0;
}

/* write a word to the PCMCIA srom */
static int
sprom_write_pcmcia(void *osh, uint16 addr, uint16 data)
{
	uint8 addr_l, addr_h, data_l, data_h;

	addr_l = (uint8)((addr * 2) & 0xff);
	addr_h = (uint8)(((addr * 2) >> 8) & 0xff);
	data_l = (uint8)(data & 0xff);
	data_h = (uint8)((data >> 8) & 0xff);

	/* set address */
	OSL_PCMCIA_WRITE_ATTR(osh, SROM_ADDRH, &addr_h, 1);
	OSL_PCMCIA_WRITE_ATTR(osh, SROM_ADDRL, &addr_l, 1);

	/* write data */
	OSL_PCMCIA_WRITE_ATTR(osh, SROM_DATAH, &data_h, 1);
	OSL_PCMCIA_WRITE_ATTR(osh, SROM_DATAL, &data_l, 1);

	/* do write */
	return sprom_cmd_pcmcia(osh, SROM_WRITE);
}

/*
 * Read in and validate sprom.
 * Return 0 on success, nonzero on error.
 */
static int
sprom_read_pci(uint16 *sprom, uint byteoff, uint16 *buf, uint nbytes, bool check_crc)
{
	int off, nw;
	uint8 chk8;
	int i;

	off = byteoff / 2;
	nw = ROUNDUP(nbytes, 2) / 2;

	/* read the sprom */
	for (i = 0; i < nw; i++)
		buf[i] = R_REG(&sprom[off + i]);

	if (check_crc) {
		/* fixup the endianness so crc8 will pass */
		htol16_buf(buf, nw * 2);
		if ((chk8 = crc8((uchar*)buf, nbytes, CRC8_INIT_VALUE)) != CRC8_GOOD_VALUE)
			return (1);
		/* now correct the endianness of the byte array */
		ltoh16_buf(buf, nw * 2);
	}

	return (0);
}

/*
 * Initialize nonvolatile variable table from sprom.
 * Return 0 on success, nonzero on error.
 */

static int
initvars_srom_pci(void *curmap, char **vars, int *count)
{
	uint16 w, b[64];
	uint8 sromrev;
	struct ether_addr ea;
	char eabuf[32];		     
	uint32 bfl;
	int c, woff, i;
	char *vp, *base;

	if (sprom_read_pci((void *)((uint)curmap + PCI_BAR0_SPROM_OFFSET), 0, b, sizeof (b), TRUE))
		return (-1);

	/* top word of sprom contains version and crc8 */
	sromrev = b[63] & 0xff;
	if ((sromrev != 1) && (sromrev != 2)) {
		return (-2);
	}

	ASSERT(vars);
	ASSERT(count);

	base = vp = MALLOC(VARS_MAX);
	ASSERT(vp);

	vp += sprintf(vp, "sromrev=%d", sromrev);
	vp++;

	if (sromrev >= 2) {
		/* New section takes over the 4th hardware function space */

		/* Word 29 is max power 11a high/low */
		w = b[29];
		vp += sprintf(vp, "pa1himaxpwr=%d", w & 0xff);
		vp++;
		vp += sprintf(vp, "pa1lomaxpwr=%d", (w >> 8) & 0xff);
		vp++;

		/* Words 30-32 set the 11alow pa settings,
		 * 33-35 are the 11ahigh ones.
		 */
		for (i = 0; i < 3; i++) {
			vp += sprintf(vp, "pa1lob%d=%d", i, b[30 + i]);
			vp++;
			vp += sprintf(vp, "pa1hib%d=%d", i, b[33 + i]);
			vp++;
		}
		w = b[59];
		if (w == 0)
			vp += sprintf(vp, "ccode=");
		else
			vp += sprintf(vp, "ccode=%c%c", (w >> 8), (w & 0xff));
		vp++;

	}

	/* parameter section of sprom starts at byte offset 72 */
	woff = 72/2;

	/* first 6 bytes are il0macaddr */
	ea.octet[0] = (b[woff] >> 8) & 0xff;
	ea.octet[1] = b[woff] & 0xff;
	ea.octet[2] = (b[woff+1] >> 8) & 0xff;
	ea.octet[3] = b[woff+1] & 0xff;
	ea.octet[4] = (b[woff+2] >> 8) & 0xff;
	ea.octet[5] = b[woff+2] & 0xff;
	woff += ETHER_ADDR_LEN/2 ;
	bcm_ether_ntoa((uchar*)&ea, eabuf);
	vp += sprintf(vp, "il0macaddr=%s", eabuf);
	vp++;

	/* next 6 bytes are et0macaddr */
	ea.octet[0] = (b[woff] >> 8) & 0xff;
	ea.octet[1] = b[woff] & 0xff;
	ea.octet[2] = (b[woff+1] >> 8) & 0xff;
	ea.octet[3] = b[woff+1] & 0xff;
	ea.octet[4] = (b[woff+2] >> 8) & 0xff;
	ea.octet[5] = b[woff+2] & 0xff;
	woff += ETHER_ADDR_LEN/2 ;
	bcm_ether_ntoa((uchar*)&ea, eabuf);
	vp += sprintf(vp, "et0macaddr=%s", eabuf);
	vp++;

	/* next 6 bytes are et1macaddr */
	ea.octet[0] = (b[woff] >> 8) & 0xff;
	ea.octet[1] = b[woff] & 0xff;
	ea.octet[2] = (b[woff+1] >> 8) & 0xff;
	ea.octet[3] = b[woff+1] & 0xff;
	ea.octet[4] = (b[woff+2] >> 8) & 0xff;
	ea.octet[5] = b[woff+2] & 0xff;
	woff += ETHER_ADDR_LEN/2 ;
	bcm_ether_ntoa((uchar*)&ea, eabuf);
	vp += sprintf(vp, "et1macaddr=%s", eabuf);
	vp++;

	/*
	 * Enet phy settings one or two singles or a dual
	 * Bits 4-0 : MII address for enet0 (0x1f for not there)
	 * Bits 9-5 : MII address for enet1 (0x1f for not there)
	 * Bit 14   : Mdio for enet0
	 * Bit 15   : Mdio for enet1
	 */
	w = b[woff];
	vp += sprintf(vp, "et0phyaddr=%d", (w & 0x1f));
	vp++;
	vp += sprintf(vp, "et1phyaddr=%d", ((w >> 5) & 0x1f));
	vp++;
	vp += sprintf(vp, "et0mdcport=%d", ((w >> 14) & 0x1));
	vp++;
	vp += sprintf(vp, "et1mdcport=%d", ((w >> 15) & 0x1));
	vp++;

	/* Word 46 has board rev, antennas 0/1 & Country code/control */
	w = b[46];
	vp += sprintf(vp, "boardrev=%d", w & 0xff);
	vp++;

	if (sromrev > 1)
		vp += sprintf(vp, "cctl=%d", (w >> 8) & 0xf);
	else
		vp += sprintf(vp, "cc=%d", (w >> 8) & 0xf);
	vp++;

	vp += sprintf(vp, "aa0=%d", (w >> 12) & 0x3);
	vp++;

	vp += sprintf(vp, "aa1=%d", (w >> 14) & 0x3);
	vp++;

	/* Words 47-49 set the (wl) pa settings */
	woff = 47;

	for (i = 0; i < 3; i++) {
		vp += sprintf(vp, "pa0b%d=%d", i, b[woff+i]);
		vp++;
		vp += sprintf(vp, "pa1b%d=%d", i, b[woff+i+6]);
		vp++;
	}

	/*
	 * Words 50-51 set the customer-configured wl led behavior.
	 * 8 bits/gpio pin.  High bit:  activehi=0, activelo=1;
	 * LED behavior values defined in wlioctl.h .
	 */
	w = b[50];
	if ((w != 0) && (w != 0xffff)) {
		/* gpio0 */
		vp += sprintf(vp, "wl0gpio0=%d", (w & 0xff));
		vp++;

		/* gpio1 */
		vp += sprintf(vp, "wl0gpio1=%d", (w >> 8) & 0xff);
		vp++;
	}
	w = b[51];
	if ((w != 0) && (w != 0xffff)) {
		/* gpio2 */
		vp += sprintf(vp, "wl0gpio2=%d", w & 0xff);
		vp++;

		/* gpio3 */
		vp += sprintf(vp, "wl0gpio3=%d", (w >> 8) & 0xff);
		vp++;
	}
	
	/* Word 52 is max power 0/1 */
	w = b[52];
	vp += sprintf(vp, "pa0maxpwr=%d", w & 0xff);
	vp++;
	vp += sprintf(vp, "pa1maxpwr=%d", (w >> 8) & 0xff);
	vp++;

	/* Word 56 is idle tssi target 0/1 */
	w = b[56];
	vp += sprintf(vp, "pa0itssit=%d", w & 0xff);
	vp++;
	vp += sprintf(vp, "pa1itssit=%d", (w >> 8) & 0xff);
	vp++;

	/* Word 57 is boardflags, if not programmed make it zero */
	bfl = (uint32)b[57];
	if (bfl == 0xffff) bfl = 0;
	if (sromrev > 1) {
		/* Word 28 is boardflags2 */
		bfl |= (uint32)b[28] << 16;
	}
	vp += sprintf(vp, "boardflags=%d", bfl);
	vp++;

	/* Word 58 is antenna gain 0/1 */
	w = b[58];
	vp += sprintf(vp, "ag0=%d", w & 0xff);
	vp++;

	vp += sprintf(vp, "ag1=%d", (w >> 8) & 0xff);
	vp++;

	if (sromrev == 1) {
		/* set the oem string */
		vp += sprintf(vp, "oem=%02x%02x%02x%02x%02x%02x%02x%02x",
			      ((b[59] >> 8) & 0xff), (b[59] & 0xff),
			      ((b[60] >> 8) & 0xff), (b[60] & 0xff),
			      ((b[61] >> 8) & 0xff), (b[61] & 0xff),
			      ((b[62] >> 8) & 0xff), (b[62] & 0xff));
		vp++;
	} else {
		if (sromrev >= 1){
			/* Word 60 OFDM tx power offset from CCK level */
			/* OFDM Power Offset - opo */
			w = b[60] & 0xff;
			if (w == 0xff)
				w = 16;
			vp += sprintf(vp, "opo=%d", w);
			vp++;
		}
	}

	/* final nullbyte terminator */
	*vp++ = '\0';

	c = vp - base;
	ASSERT(c <= VARS_MAX);

	if (c == VARS_MAX) {
		*vars = base;
	} else {
		vp = MALLOC(c);
		ASSERT(vp);
		bcopy(base, vp, c);
		MFREE(base, VARS_MAX);
		*vars = vp;
	}
	*count = c;

	return (0);
}

/*
 * Read the cis and call parsecis to initialize the vars.
 * Return 0 on success, nonzero on error.
 */
static int
initvars_cis_pcmcia(void *sbh, void *curmap, void *osh, char **vars, int *count)
{
	uint8 *cis = NULL;
	int rc;
	uint data_sz;

	data_sz = (sb_pcmciarev(sbh) == 1) ? (SPROM_SIZE * 2) : CIS_SIZE;

	if ((cis = MALLOC(data_sz)) == NULL)
		return (-1);

	if (sb_pcmciarev(sbh) == 1) {
		if (srom_read(PCMCIA_BUS, (void *)NULL, osh, 0, data_sz, (uint16 *)cis)) {
			MFREE(cis, data_sz);
			return (-1);
		}
		/* fix up endianess for 16-bit data vs 8-bit parsing */
		ltoh16_buf((uint16 *)cis, data_sz);
	} else
		OSL_PCMCIA_READ_ATTR(osh, 0, cis, data_sz);

	rc = srom_parsecis(cis, vars, count);

	MFREE(cis, data_sz);

	return (rc);
}

