// SPDX-License-Identifier: GPL-2.0
/*
 * AN7552 xPON - PHY read-only debugfs status
 */

#include <linux/bitfield.h>
#include <linux/debugfs.h>
#include <linux/seq_file.h>
#include <linux/string.h>

#include "an7552_phy.h"

/* one value of the status: a seq_file line, or one kernel log line */
static __printf(3, 4) void an7552_out(struct an7552_phy *priv,
				      struct seq_file *m, const char *fmt, ...)
{
	char buf[96];
	va_list ap;
	char *e;

	va_start(ap, fmt);
	if (m) {
		seq_vprintf(m, fmt, ap);
	} else {
		vsnprintf(buf, sizeof(buf), fmt, ap);
		e = strchr(buf, '\n');
		if (e)
			*e = 0;
		dev_info(priv->dev, "dump: %s\n", buf);
	}
	va_end(ap);
}

void an7552_phy_dump(struct an7552_phy *priv, struct seq_file *m)
{
	u32 sta = csr_read(priv, PHY_CSR_PHYSTA1);
	u32 rtd = 0;
	bool ready, have_rtd;

	/*
	 * Only the plain status registers here: an7552_phy_get_stats() latches
	 * and reads to clear the BIP counter, and a debugfs file that ate BIP
	 * errors would blind the BER interval that turns them into SD, SF and
	 * REI.  (The genl CMD_GET_COUNTERS path compensates for it in
	 * fill_global_counters().)
	 *
	 * The RTD reading settles with the link up and waits up to 10 ms for
	 * it, so it is attempted on a ready PHY only: this dump also runs from
	 * the PHY event work (debug bit 1), where a dark link would block the
	 * events for no value.
	 */
	ready = an7552_phy_is_ready(priv);
	have_rtd = ready && !an7552_phy_rtd(priv, &rtd);

#define OUT(...)	an7552_out(priv, m, __VA_ARGS__)
	OUT("mode:\t\t%d\n", priv->mode);
	OUT("configured:\t%d\n", priv->configured);
	OUT("started:\t%d\n", priv->started);
	OUT("physta1:\t%#010x\tstate %lu%s\n", sta,
	    FIELD_GET(PHYSTA1_PHYRDY, sta),
	    FIELD_GET(PHYSTA1_PHYRDY, sta) == PHYRDY_STATUS ? " (ready)" : "");
	OUT("physet2:\t%#010x\n", csr_read(priv, PHY_CSR_PHYSET2));
	OUT("physet3:\t%#010x\n", csr_read(priv, PHY_CSR_PHYSET3));
	OUT("physet10:\t%#010x\n", csr_read(priv, PHY_CSR_PHYSET10));
	OUT("xpon_setting:\t%#010x\n", csr_read(priv, PHY_CSR_XPON_SETTING));
	OUT("xpon_sta:\t%#010x\n", csr_read(priv, PHY_CSR_XPON_STA));
	OUT("int_en:\t\t%#010x\n", csr_read(priv, PHY_CSR_XPON_INT_EN));
	OUT("int_sta:\t%#010x\n", csr_read(priv, PHY_CSR_XPON_INT_STA));
	/* calibration results as phy_calibration_test prints them */
	OUT("anacal1:\t%#010x\n", csr_read(priv, PHY_CSR_ANACAL1));
	OUT("anacal2:\t%#010x\n", csr_read(priv, PHY_CSR_ANACAL2));
	OUT("physap:\t\t%#010x\n", csr_read(priv, PHY_CSR_PHYSAP));
	OUT("tdcsta1:\t%#010x\n", csr_read(priv, PHY_CSR_TDCSTA1));
	OUT("pma_int_sta:\t%#010x\n", pma_read(priv, PMA_PON_INTERRUPT_STS_0));
	OUT("ncpo:\t\t%#010x\n", pma_read(priv, PMA_SS_LCPLL_TDC_RO_4));
	/* FEC: is the OLT using it, and is the decoder on (phy_rx_fec_*()) */
	OUT("fec:\t\tdec %d rx %d tx %d\n",
	    !!(csr_read(priv, PHY_CSR_FECDEC_CTL) & BIT(0)),
	    !!(csr_read(priv, PHY_CSR_RX_STATUS) & PHY_RX_FEC),
	    !!(csr_read(priv, PHY_CSR_TX_STATUS) & PHY_TX_FEC));
	if (have_rtd)
		OUT("rtd:\t\t0x%04x\n", rtd);
	else
		OUT("rtd:\t\t%s\n", ready ? "unsettled" : "no link");
#undef OUT
}

static int an7552_status_show(struct seq_file *m, void *unused)
{
	an7552_phy_dump(m->private, m);

	return 0;
}
DEFINE_SHOW_ATTRIBUTE(an7552_status);

/*
 * One device, one debugfs directory: the MAC half owns it
 * (an7552_gpon_debugfs.c) and calls an7552_phy_debugfs_create() for the
 * PHY file, so the two halves cannot remove each other's directory.
 */
void an7552_phy_debugfs_create(struct dentry *dir, struct an7552_phy *priv)
{
	priv->debugfs = dir;
	debugfs_create_file("phy_status", 0444, dir, priv,
			    &an7552_status_fops);
}
