/*
 * SPI port driver for ETRAX FS et al. using a synchronous serial
 * port, but simplified by using the spi_bitbang framework.
 *
 * Copyright (c) 2007 Axis Communications AB
 *
 * Author: Hans-Peter Nilsson, though copying parts of
 * spi_s3c24xx_gpio.c, hence also:
 * Copyright (c) 2006 Ben Dooks
 * Copyright (c) 2006 Simtec Electronics
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This driver restricts frequency, polarity, "word" length and endian
 * much more than the hardware does.  I'm happy to unrestrict it, but
 * only with what I can test myself (at time of writing, just SD/MMC
 * SPI) and what people actually test and report.
 */

#include <linux/types.h>
#include <linux/device.h>
#include <linux/spi/spi.h>
#include <linux/spi/spi_bitbang.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <asm/io.h>
#include <asm/arch/board.h>
#include <asm/arch/hwregs/reg_map.h>
#include <asm/arch/hwregs/reg_rdwr.h>
#include <asm/arch/hwregs/sser_defs.h>
#include <asm/arch/dma.h>
#include <asm/arch/hwregs/dma.h>

/* A size "not much larger" than the max typical transfer size.  */
#define DMA_CHUNKSIZ 512

/*
 * For a transfer expected to take this long, we busy-wait instead of enabling
 * interrupts.
 */
#define IRQ_USAGE_THRESHOLD_NS 14000

/* A few register access macros to avoid verbiage and reduce typos.  */
#define REG_RD_DI(reg) REG_RD(dma, regi_dmain, reg)
#define REG_RD_DO(reg) REG_RD(dma, regi_dmaout, reg)
#define REG_RD_SSER(reg) REG_RD(sser, regi_sser, reg)
#define REG_WR_DI(reg, val) REG_WR(dma, regi_dmain, reg, val)
#define REG_WR_DO(reg, val) REG_WR(dma, regi_dmaout, reg, val)
#define REG_WR_SSER(reg, val) REG_WR(sser, regi_sser, reg, val)
#define REG_WRINT_DI(reg, val) REG_WR_INT(dma, regi_dmain, reg, val)
#define REG_WRINT_DO(reg, val) REG_WR_INT(dma, regi_dmaout, reg, val)
#define REG_WRINT_SSER(reg, val) REG_WR_INT(sser, regi_sser, reg, val)
#define REG_RDINT_DI(reg) REG_RD_INT(dma, regi_dmain, reg)
#define REG_RDINT_DO(reg) REG_RD_INT(dma, regi_dmaout, reg)
#define REG_RDINT_SSER(reg) REG_RD_INT(sser, regi_sser, reg)

#define DMA_WAIT_UNTIL_RESET(inst)			\
  do {							\
	reg_dma_rw_stat r;				\
	do {						\
	  	r = REG_RD(dma, (inst), rw_stat);	\
	} while (r.mode != regk_dma_rst);		\
  } while (0)

#define DMA_BUSY(inst) (REG_RD(dma, inst, rw_stream_cmd)).busy

/* Our main driver state.  */
struct crisv32_spi_hw_info {
	struct crisv32_regi_n_int sser;
	struct crisv32_regi_n_int dmain;
	struct crisv32_regi_n_int dmaout;

	reg_sser_rw_cfg cfg;
	reg_sser_rw_frm_cfg frm_cfg;
	reg_sser_rw_tr_cfg tr_cfg;
	reg_sser_rw_rec_cfg rec_cfg;
	reg_sser_rw_extra extra;

	/* We store the speed in kHz, so we can have expressions
	 * multiplying 100MHz by * 4 before dividing by it, and still
	 * keep it in an u32. */
	u32 effective_speed_kHz;

	/*
	 * The time in 10s of nanoseconds for half a cycles.
	 * For convenience and performance; derived from the above.
	 */
	u32 half_cycle_delay_ns;

	/* This should be overridable by a module parameter.  */
	u32 max_speed_Hz;

	/* Pre-computed timout for the max transfer chunk-size.  */
	u32 dma_timeout;

	struct completion dma_done;

	/*
	 * If we get a timeout from wait_for_completion_timeout on the
	 * above, first look at this before panicking.
	 */
	u32 dma_actually_done;

	/*
	 * Resources don't seem available at the remove call, so we
	 * have to save information we get through them.
	 */
	struct crisv32_spi_sser_controller_data *gc;
};

/*
 * The driver state hides behind the spi_bitbang state; we're
 * responsible for allocating that, so we can get a little something
 * for ourselves.
 */
struct crisv32_spi_sser_devdata {
	struct spi_bitbang bitbang;
	struct crisv32_spi_hw_info hw;
};

/* Our DMA descriptors that need alignment.  */
struct crisv32_spi_dma_descrs {
	dma_descr_context in_ctxt __attribute__ ((__aligned__(32)));
	dma_descr_context out_ctxt __attribute__ ((__aligned__(32)));

	/*
	 * The code takes advantage of the fact that in_descr and
	 * out_descr are on the same cache-line when working around
	 * the cache-bug in TR 106.
	 */
	dma_descr_data in_descr __attribute__ ((__aligned__(16)));
	dma_descr_data out_descr __attribute__ ((__aligned__(16)));
};

/*
 * Whatever needs DMA access is here, besides whatever DMA-able memory
 * comes in transfers.
 */
struct crisv32_spi_dma_cs {
	struct crisv32_spi_dma_descrs *descrp;

	/* Scratch-buffers when the original was non-DMA.  */
	u8 rx_buf[DMA_CHUNKSIZ];
	u8 tx_buf[DMA_CHUNKSIZ];
};

/*
 * Max speed.  If set, we won't go faster, promise.  May be useful
 * when dealing with weak hardware; misrouted signal paths or various
 * debug-situations.
 */
static ulong crisv32_spi_speed_limit_Hz = 0;

/* Helper function getting the driver state from a spi_device.  */

static inline struct crisv32_spi_hw_info *spidev_to_hw(struct spi_device *spi)
{
	struct crisv32_spi_sser_devdata *dd = spi_master_get_devdata(spi->master);
	return &dd->hw;
}

/* SPI-bitbang word transmit-function for non-DMA.  */

static u32 crisv32_spi_sser_txrx_mode3(struct spi_device *spi,
				       unsigned nsecs, u32 word, u8 bits)
{
	struct crisv32_spi_hw_info *hw = spidev_to_hw(spi);
	u32 regi_sser = hw->sser.regi;
	reg_sser_rw_ack_intr ack_intr = { .trdy = 1, .rdav = 1 };
	reg_sser_r_intr intr = {0};
	reg_sser_rw_tr_data w_data = { .data = (u8) word };
	reg_sser_r_rec_data r_data;
	u32 i;

	/*
	 * The timeout reflects one iteration per 10ns (impossible at
	 * 200MHz clock even without the ndelay) and a wait for a full
	 * byte.
	 */
	u32 timeout = 1000000/10*8/hw->effective_speed_kHz;

	BUG_ON(bits != 8);

	intr = REG_RD_SSER(r_intr);

	/*
	 * We should never get xruns when we control the transmitter
	 * and receiver in register mode.  And if we don't have
	 * transmitter-ready and data-ready on entry, something's
	 * seriously fishy.
	 */
	if (!intr.trdy || !intr.rdav || intr.orun || intr.urun)
		panic("sser hardware or SPI driver broken (1) 0x%x\n",
		      REG_TYPE_CONV(u32, reg_sser_r_intr, intr));

	REG_WR_SSER(rw_ack_intr, ack_intr);
	REG_WR_SSER(rw_tr_data, w_data);

	for (i = 0; i < timeout; i++) {
		intr = REG_RD_SSER(r_intr);
		/* Wait for received data.  */
		if (intr.rdav)
			break;
		ndelay(10);
	}

	if (!(intr.trdy && intr.rdav) || intr.orun || intr.urun)
		panic("sser hardware or SPI driver broken (2) 0x%x\n",
		      REG_TYPE_CONV(u32, reg_sser_r_intr, intr));

	r_data = REG_RD_SSER(r_rec_data);
	return r_data.data & 0xff;
}

/*
 * Wait for 1/2 bit-time if the transmitter or receiver is enabled.
 * We need to do this as the data-available indications may arrive
 * right at the edge, with half the last cycle remaining.
 */
static void inline crisv32_spi_sser_wait_halfabit(struct crisv32_spi_hw_info
						  *hw)
{
	if (hw->cfg.en)
		ndelay(hw->half_cycle_delay_ns);
}

/*
 * Assert or de-assert chip-select.
 * We have two functions, with the active one assigned to the bitbang
 * slot at setup, to avoid a performance penalty (1% on reads).
 */
static void crisv32_spi_sser_chip_select_active_high(struct spi_device *spi,
						     int value)
{
	struct crisv32_spi_hw_info *hw = spidev_to_hw(spi);
	u32 regi_sser = hw->sser.regi;

	/*
	 * We may have received data at the "last producing clock
	 * edge".  Thus we delay for another half a clock cycle.
	 */
	crisv32_spi_sser_wait_halfabit(hw);

	hw->frm_cfg.frame_pin_use
		= value == BITBANG_CS_ACTIVE ? regk_sser_gio1 : regk_sser_gio0;
	REG_WR_SSER(rw_frm_cfg, hw->frm_cfg);
}

static void crisv32_spi_sser_chip_select_active_low(struct spi_device *spi,
						    int value)
{
	struct crisv32_spi_hw_info *hw = spidev_to_hw(spi);
	u32 regi_sser = hw->sser.regi;

	crisv32_spi_sser_wait_halfabit(hw);
	hw->frm_cfg.frame_pin_use
		= value == BITBANG_CS_ACTIVE ? regk_sser_gio0 : regk_sser_gio1;
	REG_WR_SSER(rw_frm_cfg, hw->frm_cfg);
}

/* Set the transmission speed in Hz.  */

static int crisv32_spi_sser_set_speed_Hz(struct crisv32_spi_hw_info *hw,
					 u32 Hz)
{
	u32 kHz;
	u32 ns_delay;
	u32 regi_sser = hw->sser.regi;

	if (Hz > hw->max_speed_Hz)
		/*
		 * Should we complain?  Return error?  Current caller
		 * sequences want just the max speed.
		 */
		Hz = hw->max_speed_Hz;

	kHz = Hz/1000;

	/*
	 * If absolutely needed, we *could* change the base frequency
	 * and go lower.  Usually, a frequency set higher than wanted
	 * is a problem but lower isn't.
	 */
	if (Hz < 100000000 / 65536 + 1) {
		printk(KERN_ERR "attempt to set invalid sser speed: %u Hz\n",
		       Hz);
		Hz = 100000000 / 65536 + 1;
	}

	pr_debug("setting sser speed to %u Hz\n", Hz);

	/*
	 * Avoid going above the requested speed if there's a
	 * remainder for the 100 MHz clock-divider calculation, but
	 * don't unnecessarily go below if it's even.
	 */
	hw->cfg.clk_div = 100000000/Hz - ((100000000 % Hz) == 0);

	/* Make sure there's no ongoing transmission. */
	crisv32_spi_sser_wait_halfabit(hw);

	/*
	 * Wait for 3 times max of the old and the new clock before and after
	 * changing the frequency.  Not because of documentation or empirical
	 * need, but because it seems sane to do so.  The three-bit-times
	 * value is because that's the documented time it takes for a reset to
	 * take effect.
	 */
	ns_delay = 1000000*3/(kHz > hw->effective_speed_kHz
			      ? kHz : hw->effective_speed_kHz);
	ndelay(ns_delay);
	REG_WR_SSER(rw_cfg, hw->cfg);
	ndelay(ns_delay);

	hw->effective_speed_kHz = kHz;

	/*
	 * A timeout of twice the time for the largest chunk (not
	 * counting DMA overhead) plus one jiffy, should be more than
	 * enough for the transmission.
	 */
	hw->dma_timeout = 1 + usecs_to_jiffies(1000*2*DMA_CHUNKSIZ*8/kHz);

	hw->half_cycle_delay_ns
		= 1000000/2/hw->effective_speed_kHz;

	pr_debug(".clk_div %d, half %d, eff %d\n",
		 hw->cfg.clk_div, hw->half_cycle_delay_ns,
		 hw->effective_speed_kHz);
	return 0;
}

/*
 * Set up transmitter and receiver for non-DMA access.
 * Unfortunately, it doesn't seem like hispeed works for this mode
 * (mea culpa), so we're stuck with lospeed-mode.  A little slower,
 * but that's what you get for not allocating DMA.
 */
static int crisv32_setup_spi_sser_for_reg_access(struct crisv32_spi_hw_info *hw)
{
	u32 regi_sser = hw->sser.regi;

	reg_sser_rw_cfg cfg = {0};
	reg_sser_rw_frm_cfg frm_cfg = {0};
	reg_sser_rw_tr_cfg tr_cfg = {0};
	reg_sser_rw_rec_cfg rec_cfg = {0};
	reg_sser_rw_intr_mask mask = {0};
	reg_sser_rw_extra extra = {0};
	reg_sser_rw_tr_data tr_data = {0};
	reg_sser_r_intr intr;

	cfg.en = 0;
	tr_cfg.tr_en = 1;
	rec_cfg.rec_en = 1;
	REG_WR_SSER(rw_cfg, cfg);
	REG_WR_SSER(rw_tr_cfg, tr_cfg);
	REG_WR_SSER(rw_rec_cfg, rec_cfg);
	REG_WR_SSER(rw_intr_mask, mask);

	/*
	 * See 23.7.2 SPI in the hardware documentation.
	 * Except our configuration uses bulk mode; MMC/SD-SPI
	 * isn't isochronous in nature.
	 * Step 1.
	 */
	cfg.gate_clk = regk_sser_yes;
	cfg.clkgate_in = regk_sser_no;
	cfg.clkgate_ctrl = regk_sser_tr;

	/* Step 2.  */
	cfg.out_clk_pol = regk_sser_pos;
	cfg.out_clk_src = regk_sser_intern_clk;

	/* Step 3.  */
	tr_cfg.clk_src = regk_sser_intern;
	rec_cfg.clk_src = regk_sser_intern;
	frm_cfg.clk_src = regk_sser_intern;

	/* Step 4.  */
	tr_cfg.clk_pol = regk_sser_neg;
	rec_cfg.clk_pol = regk_sser_pos;
	frm_cfg.clk_pol = regk_sser_neg;

	/*
	 * Step 5: frame pin (PC03 or PD03) is frame; the status pin
	 * (PC02, PD02) is configured as input.
	 */
	frm_cfg.frame_pin_dir = regk_sser_out;

	/*
	 * Contrary to the doc example, we don't generate the frame
	 * signal "automatically".  This setting of the frame pin as
	 * constant 1, reflects an inactive /CS setting, for just idle
	 * clocking.  When we need to transmit or receive data, we
	 * change it.
	 */
	frm_cfg.frame_pin_use = regk_sser_gio1;
	frm_cfg.status_pin_dir = regk_sser_in;

	/*
	 * Step 6.  This is probably not necessary, as we don't
	 * generate the frame signal automatically.  Nevertheless,
	 * modified for bulk transmission.
	 */
	frm_cfg.out_on = regk_sser_tr;
	frm_cfg.out_off = regk_sser_tr;

	/* Step 7.  Similarly, maybe not necessary.  */
	frm_cfg.type = regk_sser_level;
	frm_cfg.level = regk_sser_neg_lo;

	/* Step 8.  These we have to set according to the bulk mode,
	 * which for tr_delay is the same as for iso; a value of 1
	 * means in sync with the frame signal.  For rec_delay, we
	 * start it at the same time as the transmitter.  See figure
	 * 23.7 in the hw documentation.  */
	frm_cfg.tr_delay = 1;
	frm_cfg.rec_delay = 0;

	/* Step 9.  */
	tr_cfg.sample_size = 7;
	rec_cfg.sample_size = 7;

	/* Step 10.  */
	frm_cfg.wordrate = 7;

	/* Step 11 (but for bulk).  */
	tr_cfg.rate_ctrl = regk_sser_bulk;

	/*
	 * Step 12.  Similarly, maybe not necessary; still, modified
	 * for bulk.
	 */
	tr_cfg.frm_src = regk_sser_intern;
	rec_cfg.frm_src = regk_sser_tx_bulk;

	/* Step 13.  */
	tr_cfg.mode = regk_sser_lospeed;
	rec_cfg.mode = regk_sser_lospeed;

	/* Step 14.  */
	tr_cfg.sh_dir = regk_sser_msbfirst;
	rec_cfg.sh_dir = regk_sser_msbfirst;

	/*
	 * Extra step for bulk-specific settings and other general
	 * settings not specified in the SPI config example.
	 * It's uncertain whether all of these are needed.
	 */
	tr_cfg.bulk_wspace = 1;
	tr_cfg.use_dma = 0;

	tr_cfg.urun_stop = 1;
	rec_cfg.orun_stop = 1;
	rec_cfg.use_dma = 0;

	rec_cfg.fifo_thr = regk_sser_inf;
	frm_cfg.early_wend = regk_sser_yes;

	cfg.clk_dir = regk_sser_out;
	tr_cfg.data_pin_use = regk_sser_dout;
	cfg.base_freq = regk_sser_f100;

	/* Setup for the initial frequency given to us.  */
	hw->cfg = cfg;
	crisv32_spi_sser_set_speed_Hz(hw, hw->max_speed_Hz);
	cfg = hw->cfg;

	/*
	 * Write it all, except cfg which is already written by
	 * crisv32_spi_sser_set_speed_Hz.
	 */
	REG_WR_SSER(rw_frm_cfg, frm_cfg);
	REG_WR_SSER(rw_tr_cfg, tr_cfg);
	REG_WR_SSER(rw_rec_cfg, rec_cfg);
	REG_WR_SSER(rw_extra, extra);

	/*
	 * The transmit-register needs to be written before the
	 * transmitter is enabled, and to get a valid trdy signal
	 * waiting for us when we want to transmit a byte.  Because
	 * the "frame event" is that the transmitter is written, this
	 * will cause a dummy 0xff-byte to be transmitted, but that's
	 * ok, because /CS is inactive.
	 */
	tr_data.data = 0xffff;
	REG_WR_SSER(rw_tr_data, tr_data);

	/*
	 * We ack everything interrupt-wise; left-over indicators don't have
	 * to come from *this* code.
	 */
	REG_WRINT_SSER(rw_ack_intr, -1);

	/*
	 * Wait 3 cycles before enabling, after the transmit register
	 * has been written.  (This'll be just a few microseconds for
	 * e.g. 400 KHz.)
	 */
	ndelay(3 * 2 * hw->half_cycle_delay_ns);
	cfg.en = 1;

	REG_WR_SSER(rw_cfg, cfg);

	/*
	 * Now wait for 8 + 3 cycles.  The 0xff byte should now have
	 * been transmitted and dummy data received.
	 */
	ndelay((8 + 3) * 2 * hw->half_cycle_delay_ns);

	/*
	 * Sanity-check that we have data-available and the
	 * transmitter is ready to send new data.
	 */
	intr = REG_RD_SSER(r_intr);
	if (!intr.rdav || !intr.trdy)
		panic("sser hw or SPI driver broken (3) 0x%x",
		      REG_TYPE_CONV(u32, reg_sser_r_intr, intr));

	hw->frm_cfg = frm_cfg;
	hw->tr_cfg = tr_cfg;
	hw->rec_cfg = rec_cfg;
	hw->extra = extra;
	hw->cfg = cfg;
	return 0;
}

/* Initialization, maybe fault recovery.  */

static void crisv32_reset_dma_hw(u32 regi)
{
	REG_WR_INT(dma, regi, rw_intr_mask, 0);

	DMA_RESET(regi);
	DMA_WAIT_UNTIL_RESET(regi);
	DMA_ENABLE(regi);
	REG_WR_INT(dma, regi, rw_ack_intr, -1);

	DMA_WR_CMD(regi, regk_dma_set_w_size1);
}

/* Interrupt from SSER, for use with DMA when only the transmitter is used.  */

static irqreturn_t sser_interrupt(int irqno, void *arg)
{
	struct crisv32_spi_hw_info *hw = arg;
	u32 regi_sser = hw->sser.regi;
	reg_sser_r_intr intr = REG_RD_SSER(r_intr);

	if (intr.tidle == 0 && intr.urun == 0) {
		printk(KERN_ERR
		       "sser @0x%x: spurious sser intr, flags: 0x%x\n",
		       regi_sser, REG_TYPE_CONV(u32, reg_sser_r_intr, intr));
	} else if (intr.urun == 0) {
		hw->dma_actually_done = 1;
		complete(&hw->dma_done);
	} else {
		/*
		 * Make any reception time out and notice the error,
		 * which it might not otherwise do data was *received*
		 * successfully.
		 */
		u32 regi_dmain = hw->dmain.regi;

		/*
		 * Recommended practice before acking urun is to turn
		 * off sser.  That might not be enough to stop DMA-in
		 * from signalling success if the underrun was late in
		 * the transmission, so we disable the DMA-in
		 * interrupts too.
		 */
		REG_WRINT_SSER(rw_cfg, 0);
		REG_WRINT_DI(rw_intr_mask, 0);
		REG_WRINT_DI(rw_ack_intr, -1);
	}

	REG_WRINT_SSER(rw_intr_mask, 0);

	/*
	 * We must at least ack urun together with tidle, but keep it
	 * simple and ack them all.
	 */
	REG_WRINT_SSER(rw_ack_intr, -1);

	return IRQ_HANDLED;
}

/*
 * Interrupt from receiver DMA connected to SSER, for use when the
 * receiver is used, with or without the transmitter.
 */
static irqreturn_t rec_dma_interrupt(int irqno, void *arg)
{
	struct crisv32_spi_hw_info *hw = arg;
	u32 regi_dmain = hw->dmain.regi;
	u32 regi_sser = hw->sser.regi;
	reg_dma_r_intr intr = REG_RD_DI(r_intr);

	if (intr.data == 0) {
		printk(KERN_ERR
		       "sser @0x%x: spurious rec dma intr, flags: 0x%x\n",
		       regi_dmain, REG_TYPE_CONV(u32, reg_dma_r_intr, intr));
	} else {
		hw->dma_actually_done = 1;
		complete(&hw->dma_done);
	}

	REG_WRINT_DI(rw_intr_mask, 0);

	/* Avoid false underrun indications; stop all sser interrupts.   */
	REG_WRINT_SSER(rw_intr_mask, 0);
	REG_WRINT_SSER(rw_ack_intr, -1);

	REG_WRINT_DI(rw_ack_intr, -1);
	return IRQ_HANDLED;
}

/*
 * Set up transmitter and receiver for DMA access.  We use settings
 * from the "Atmel fast flash" example.
 */
static int crisv32_setup_spi_sser_for_dma_access(struct crisv32_spi_hw_info
						 *hw)
{
	int ret;
	u32 regi_sser = hw->sser.regi;

	reg_sser_rw_cfg cfg = {0};
	reg_sser_rw_frm_cfg frm_cfg = {0};
	reg_sser_rw_tr_cfg tr_cfg = {0};
	reg_sser_rw_rec_cfg rec_cfg = {0};
	reg_sser_rw_intr_mask mask = {0};
	reg_sser_rw_extra extra = {0};

	cfg.en = 0;
	tr_cfg.tr_en = 1;
	rec_cfg.rec_en = 1;
	REG_WR_SSER(rw_cfg, cfg);
	REG_WR_SSER(rw_tr_cfg, tr_cfg);
	REG_WR_SSER(rw_rec_cfg, rec_cfg);
	REG_WR_SSER(rw_intr_mask, mask);

	/*
	 * See 23.7.5.2 (Atmel fast flash) in the hardware documentation.
	 * Step 1.
	 */
	cfg.gate_clk = regk_sser_no;

	/* Step 2.  */
	cfg.out_clk_pol = regk_sser_pos;

	/* Step 3.  */
	cfg.out_clk_src = regk_sser_intern_clk;

	/* Step 4.  */
	tr_cfg.sample_size = 1;
	rec_cfg.sample_size = 1;

	/* Step 5.  */
	frm_cfg.wordrate = 7;

	/* Step 6.  */
	tr_cfg.clk_src = regk_sser_intern;
	rec_cfg.clk_src = regk_sser_intern;
	frm_cfg.clk_src = regk_sser_intern;
	tr_cfg.clk_pol = regk_sser_neg;
	frm_cfg.clk_pol = regk_sser_neg;

	/* Step 7.  */
	rec_cfg.clk_pol = regk_sser_pos;

	/* Step 8.  */
	frm_cfg.tr_delay = 1;

	/* Step 9.  */
	frm_cfg.rec_delay = 1;

	/* Step 10.  */
	tr_cfg.sh_dir = regk_sser_msbfirst;
	rec_cfg.sh_dir = regk_sser_msbfirst;

	/* Step 11.  */
	tr_cfg.frm_src = regk_sser_intern;
	rec_cfg.frm_src = regk_sser_intern;

	/* Step 12.  */
	tr_cfg.rate_ctrl = regk_sser_iso;

	/*
	 * Step 13.  Note that 0 != tx_null, so we're good regarding
	 * the descriptor .md field.
	 */
	tr_cfg.eop_stop = 1;

	/* Step 14.  */
	frm_cfg.frame_pin_use = regk_sser_gio1;
	frm_cfg.frame_pin_dir = regk_sser_out;

	/* Step 15.  */
	extra.clkon_en = 1;
	extra.clkoff_en = 1;

	/* Step 16.  We'll modify this value for each "burst".  */
	extra.clkoff_cycles = 7;

	/* Step 17.  */
	cfg.prepare = 1;

	/*
	 * Things left out from the documented startup procedure.
	 * It's uncertain whether all of these are needed.
	 */
	frm_cfg.status_pin_dir = regk_sser_in;
	tr_cfg.mode = regk_sser_hispeed;
	rec_cfg.mode = regk_sser_hispeed;
	frm_cfg.out_on = regk_sser_intern_tb;
	frm_cfg.out_off = regk_sser_rec;
	frm_cfg.type = regk_sser_level;
	tr_cfg.use_dma = 1;
	tr_cfg.urun_stop = 1;
	rec_cfg.orun_stop = 1;
	rec_cfg.use_dma = 1;
	rec_cfg.fifo_thr = regk_sser_inf;
	frm_cfg.early_wend = regk_sser_yes;
	cfg.clk_dir = regk_sser_out;

	tr_cfg.data_pin_use = regk_sser_dout;
	cfg.base_freq = regk_sser_f100;

	REG_WR_SSER(rw_frm_cfg, frm_cfg);
	REG_WR_SSER(rw_tr_cfg, tr_cfg);
	REG_WR_SSER(rw_rec_cfg, rec_cfg);
	REG_WR_SSER(rw_extra, extra);
	REG_WR_SSER(rw_cfg, cfg);
	hw->frm_cfg = frm_cfg;
	hw->tr_cfg = tr_cfg;
	hw->rec_cfg = rec_cfg;
	hw->extra = extra;
	hw->cfg = cfg;

	crisv32_spi_sser_set_speed_Hz(hw, hw->max_speed_Hz);

	ret = request_irq(hw->sser.irq, sser_interrupt, 0, "sser", hw);
	if (ret != 0)
		goto noirq;

	ret = request_irq(hw->dmain.irq, rec_dma_interrupt, 0, "sser rec", hw);
	if (ret != 0)
		goto free_outirq;

	crisv32_reset_dma_hw(hw->dmain.regi);
	crisv32_reset_dma_hw(hw->dmaout.regi);
	return 0;

  free_outirq:
	free_irq(hw->sser.irq, hw);
  noirq:
	return ret;
}

/* SPI-master setup function for non-DMA.  */

static int crisv32_spi_sser_regs_master_setup(struct spi_device *spi)
{
	struct crisv32_spi_hw_info *hw = spidev_to_hw(spi);
	struct spi_bitbang *bitbang = spi_master_get_devdata(spi->master);
	int ret = 0;

	/* Just do a little initial constraining checks.  */
	if (spi->bits_per_word == 0)
		spi->bits_per_word = 8;

	if (spi->bits_per_word != 8)
		return -EINVAL;

	bitbang->chipselect = (spi->mode & SPI_CS_HIGH) != 0
		? crisv32_spi_sser_chip_select_active_high
		: crisv32_spi_sser_chip_select_active_low;

	if (hw->max_speed_Hz == 0) {
		u32 max_speed_Hz;

		/*
		 * At this time; at the first call to the SPI master
		 * setup function, spi->max_speed_hz reflects the
		 * board-init value.  It will be changed later on by
		 * the protocol master, but at the master setup call
		 * is the only time we actually get to see the hw max
		 * and thus a reasonable time to init the hw field.
		 */

		/* The module parameter overrides everything.  */
		if (crisv32_spi_speed_limit_Hz != 0)
			max_speed_Hz = crisv32_spi_speed_limit_Hz;
		/*
		 * I never could get hispeed mode to work for non-DMA.
		 * We adjust the max speed here (where we could
		 * presumably fix it), not in the board info file.
		 */
		else if (spi->max_speed_hz > 16667000)
			max_speed_Hz = 16667000;
		else
			max_speed_Hz = spi->max_speed_hz;

		hw->max_speed_Hz = max_speed_Hz;
		spi->max_speed_hz = max_speed_Hz;

		/*
		 * We also do one-time initialization of the hardware at this
		 * point.  We could defer to the return to the probe-function
		 * from spi_bitbang_start, but other hardware setup (like
		 * subsequent calls to this function before that) would have
		 * to be deferred until then too.
		 */
		ret = crisv32_setup_spi_sser_for_reg_access(hw);
		if (ret != 0)
			return ret;

		ret = spi_bitbang_setup(spi);
		if (ret != 0)
			return ret;

		dev_info(&spi->dev,
			 "CRIS v32 SPI driver for sser%d\n",
			 spi->master->bus_num);
	}

	return 0;
}

/*
 * SPI-master setup_transfer-function used for both DMA and non-DMA
 * (single function for DMA, together with spi_bitbang_setup_transfer
 * for non-DMA).
 */

static int crisv32_spi_sser_common_setup_transfer(struct spi_device *spi,
						  struct spi_transfer *t)
{
	struct crisv32_spi_hw_info *hw = spidev_to_hw(spi);
	u8 bits_per_word;
	u32 hz;
	int ret = 0;

	if (t) {
		bits_per_word = t->bits_per_word;
		hz = t->speed_hz;
	} else {
		bits_per_word = 0;
		hz = 0;
	}

	if (bits_per_word == 0)
		bits_per_word = spi->bits_per_word;

	if (bits_per_word != 8)
		return -EINVAL;

	if (hz == 0)
		hz = spi->max_speed_hz;

	if (hz != hw->effective_speed_kHz*1000 && hz != 0)
		ret = crisv32_spi_sser_set_speed_Hz(hw, hz);

	return ret;
}

/* Helper for a SPI-master setup_transfer function for non-DMA.  */

static int crisv32_spi_sser_regs_setup_transfer(struct spi_device *spi,
						struct spi_transfer *t)
{
	int ret = crisv32_spi_sser_common_setup_transfer(spi, t);

	if (ret != 0)
		return ret;

	/* Set up the loop-over-buffer parts.  */
	return spi_bitbang_setup_transfer (spi, t);
}

/* SPI-master setup function for DMA.  */

static int crisv32_spi_sser_dma_master_setup(struct spi_device *spi)
{
	/*
	 * As we don't dispatch to the spi_bitbang default function,
	 * we need to do whatever tests it does; keep it in sync.  On
	 * the bright side, we can use the spi->controller_state slot;
	 * we use it for DMA:able memory for the descriptors and
	 * temporary buffers to copy non-DMA:able transfers.
	 */
	struct crisv32_spi_hw_info *hw = spidev_to_hw(spi);
	struct spi_bitbang *bitbang = spi_master_get_devdata(spi->master);
	struct crisv32_spi_dma_cs *cs;
	u32 dmasize;
	int ret = 0;

	if (hw->max_speed_Hz == 0) {
		struct crisv32_spi_dma_descrs *descrp;
		u32 descrp_dma;
		u32 max_speed_Hz;

		/* The module parameter overrides everything.  */
		if (crisv32_spi_speed_limit_Hz != 0)
			max_speed_Hz = crisv32_spi_speed_limit_Hz;
		/*
		 * See comment at corresponding statement in
		 * crisv32_spi_sser_regs_master_setup.
		 */
		else
			max_speed_Hz = spi->max_speed_hz;

		hw->max_speed_Hz = max_speed_Hz;
		spi->max_speed_hz = max_speed_Hz;

		ret = crisv32_setup_spi_sser_for_dma_access(hw);
		if (ret != 0)
			return ret;

		/* Allocate some extra for necessary alignment.  */
		dmasize = sizeof *cs + 31
			+ sizeof(struct crisv32_spi_dma_descrs);

		cs = kzalloc(dmasize, GFP_KERNEL | GFP_DMA);
		if (cs == NULL)
			return -ENOMEM;

		/*
		 * Make descriptors aligned within the allocated area,
		 * some-place after cs.
		 */
		descrp = (struct crisv32_spi_dma_descrs *)
			(((u32) (cs + 1) + 31) & ~31);
		descrp_dma = virt_to_phys(descrp);

		/* Set up the "constant" parts of the descriptors.  */
		descrp->out_descr.eol = 1;
		descrp->out_descr.intr = 1;
		descrp->out_descr.out_eop = 1;
		descrp->out_ctxt.saved_data = (dma_descr_data *)
		  (descrp_dma
		   + offsetof(struct crisv32_spi_dma_descrs, out_descr));
		descrp->out_ctxt.next = 0;

		descrp->in_descr.eol = 1;
		descrp->in_descr.intr = 1;
		descrp->in_ctxt.saved_data = (dma_descr_data *)
			(descrp_dma
			 + offsetof(struct crisv32_spi_dma_descrs, in_descr));
		descrp->in_ctxt.next = 0;

		cs->descrp = descrp;
		spi->controller_state = cs;

		init_completion(&hw->dma_done);

		dev_info(&spi->dev,
			 "CRIS v32 SPI driver for sser%d/DMA\n",
			 spi->master->bus_num);
	}

	/* Do our extra constraining checks.  */
	if (spi->bits_per_word == 0)
		spi->bits_per_word = 8;

	if (spi->bits_per_word != 8)
		return -EINVAL;

	/* SPI_LSB_FIRST deliberately left out, and we only support mode 3.  */
	if ((spi->mode & ~(SPI_TX_1|SPI_CS_HIGH)) != SPI_MODE_3)
		return -EINVAL;

	bitbang->chipselect = (spi->mode & SPI_CS_HIGH) != 0
		? crisv32_spi_sser_chip_select_active_high
		: crisv32_spi_sser_chip_select_active_low;

	ret = bitbang->setup_transfer(spi, NULL);
	if (ret != 0)
		return ret;

	/* Remember to de-assert chip-select before the first transfer.  */
	spin_lock(&bitbang->lock);
	if (!bitbang->busy) {
		bitbang->chipselect(spi, BITBANG_CS_INACTIVE);
		ndelay(hw->half_cycle_delay_ns);
	}
	spin_unlock(&bitbang->lock);

	return 0;
}

/* SPI-master cleanup function for DMA.  */

static void crisv32_spi_sser_dma_cleanup(struct spi_device *spi)
{
	kfree(spi->controller_state);
	spi->controller_state = NULL;
}

/*
 * Set up DMA transmitter descriptors for a chunk of data.
 * The caller is responsible for working around TR 106.
 */
static void crisv32_spi_sser_setup_dma_descr_out(u32 regi,
						 struct crisv32_spi_dma_cs *cs,
						 u32 out_phys, u32 chunk_len)
{
	BUG_ON(chunk_len > DMA_CHUNKSIZ);
	struct crisv32_spi_dma_descrs *descrp = cs->descrp;
	u32 descrp_dma = virt_to_phys(descrp);

	descrp->out_descr.buf = (u8 *) out_phys;
	descrp->out_descr.after = (u8 *) out_phys + chunk_len;
	descrp->out_ctxt.saved_data_buf = (u8 *) out_phys;

	DMA_START_CONTEXT(regi,
			  descrp_dma
			  + offsetof(struct crisv32_spi_dma_descrs, out_ctxt));
}

/*
 * Set up DMA receiver descriptors for a chunk of data.
 * Also, work around TR 106.
 */
static void crisv32_spi_sser_setup_dma_descr_in(u32 regi_dmain,
						struct crisv32_spi_dma_cs *cs,
						u32 in_phys, u32 chunk_len)
{
	BUG_ON(chunk_len > DMA_CHUNKSIZ);
	struct crisv32_spi_dma_descrs *descrp = cs->descrp;
	u32 descrp_dma = virt_to_phys(descrp);

	descrp->in_descr.buf = (u8 *) in_phys;
	descrp->in_descr.after = (u8 *) in_phys + chunk_len;
	descrp->in_ctxt.saved_data_buf = (u8 *) in_phys;

	flush_dma_descr(&descrp->in_descr, 1);

	DMA_START_CONTEXT(regi_dmain,
			  descrp_dma
			  + offsetof(struct crisv32_spi_dma_descrs, in_ctxt));
}

/*
 * SPI-bitbang txrx_bufs function for DMA.
 * FIXME: We have SG DMA descriptors; use them.
 * (Requires abandoning the spi_bitbang framework if done reasonably.)
 */
static int crisv32_spi_sser_dma_txrx_bufs(struct spi_device *spi,
					  struct spi_transfer *t)
{
	struct crisv32_spi_dma_cs *cs = spi->controller_state;
	struct crisv32_spi_hw_info *hw = spidev_to_hw(spi);
	u32 len = t->len;
	reg_sser_rw_cfg cfg = hw->cfg;
	reg_sser_rw_tr_cfg tr_cfg = hw->tr_cfg;
	reg_sser_rw_rec_cfg rec_cfg = hw->rec_cfg;
	reg_sser_rw_extra extra = hw->extra;
	u32 regi_sser = hw->sser.regi;
	u32 dmain = 0;
	u32 dmaout = 0;
	u32 regi_dmain = hw->dmain.regi;
	u8 *rx_buf = t->rx_buf;

	/*
	 * Using IRQ+completion is measured to give an overhead of 14
	 * us, so let's instead busy-wait for the time that would be
	 * wasted anyway, and get back sooner.  We're not counting in
	 * other overhead such as the DMA descriptor in the
	 * time-expression, which causes us to use busy-wait for
	 * data-lengths that actually take a bit longer than
	 * IRQ_USAGE_THRESHOLD_NS.  Still, with IRQ_USAGE_THRESHOLD_NS
	 * = 14000, the threshold is for 20 MHz => 35 bytes, 25 => 44
	 * and 50 => 88 and the typical SPI transfer lengths for
	 * SDcard are { 1, 2, 7, 512 } bytes so a more complicated
	 * would likely give nothing but worse performance due to
	 * complexity.
	 */
	int use_irq = len * hw->half_cycle_delay_ns
		> IRQ_USAGE_THRESHOLD_NS / 8 / 2;

	if (len > DMA_CHUNKSIZ) {
		/*
		 * It should be quite easy to adjust the code if the need
		 * arises for something much larger than the preallocated
		 * buffers (which could themselves easily just be increased)
		 * but still what fits in extra.clkoff_cycles: kmalloc a
		 * temporary dmaable buffer in this function and free it at
		 * the end.  No need to optimize rare requests.  Until then,
		 * we'll keep the code as simple as performance allows.
		 * Alternatively or if we need to send even larger data,
		 * consider calling self with the required number of "faked"
		 * shorter transfers here.
		 */
		dev_err(&spi->dev,
			"Trying to transfer %d > max %d bytes:"
			" need to adjust the SPI driver\n",
			len, DMA_CHUNKSIZ);
		return -EMSGSIZE;
	}

	/*
	 * Need to separately tell the hispeed machinery the number of
	 * bits in this transmission.
	 */
	extra.clkoff_cycles = len * 8 - 1;

	if (t->tx_buf != NULL) {
		if (t->tx_dma == 0) {
			memcpy(cs->tx_buf, t->tx_buf, len);
			dmaout = virt_to_phys(cs->tx_buf);
		} else
			dmaout = t->tx_dma;

		crisv32_spi_sser_setup_dma_descr_out(hw->dmaout.regi,
						     cs, dmaout,
						     len);

		/* No need to do anything for TR 106; this DMA only reads.  */
		tr_cfg.tr_en = 1;
		tr_cfg.data_pin_use = regk_sser_dout;
	} else {
		tr_cfg.data_pin_use = (spi->mode & SPI_TX_1)
			? regk_sser_gio1 : regk_sser_gio0;
		tr_cfg.tr_en = 0;
	}

	if (rx_buf != 0) {
		if (t->rx_dma == 0)
			dmain = virt_to_phys(cs->rx_buf);
		else
			dmain = t->rx_dma;

		crisv32_spi_sser_setup_dma_descr_in(regi_dmain, cs,
						    dmain, len);
		rec_cfg.rec_en = 1;

		REG_WRINT_SSER(rw_ack_intr, -1);
		REG_WRINT_DI(rw_ack_intr, -1);

		/*
		 * If we're receiving, use the rec data interrupt from DMA as
		 * a signal that the HW is done.
		 */
		if (use_irq) {
			reg_sser_rw_intr_mask mask = { .urun = 1 };
			reg_dma_rw_intr_mask dmask = { .data = 1 };

			REG_WR_DI(rw_intr_mask, dmask);

			/*
			 * Catch transmitter underruns too.  We don't
			 * have to conditionalize that on the
			 * transmitter being enabled; it's off when
			 * the transmitter is off.  Any overruns will
			 * be indicated by a timeout, so we don't have
			 * to check for that specifically.
			 */
			REG_WR_SSER(rw_intr_mask, mask);
		}
	} else {
		rec_cfg.rec_en = 0;

		/*
		 * Ack previous overrun, underrun and tidle interrupts.  Or
		 * why not all.  We'll get orun and urun "normally" due to the
		 * way hispeed is (documented to) work and need to clear them,
		 * and we'll have a tidle from a previous transmit if we used
		 * to both receive and transmit, but now only transmit.
		 */
		REG_WRINT_SSER(rw_ack_intr, -1);

		if (use_irq) {
			reg_sser_rw_intr_mask mask = { .urun = 1, .tidle = 1 };
			REG_WR_SSER(rw_intr_mask, mask);
		}
	}

	REG_WR_SSER(rw_rec_cfg, rec_cfg);
	REG_WR_SSER(rw_tr_cfg, tr_cfg);
	REG_WR_SSER(rw_extra, extra);

	/*
	 * Barriers are needed to make sure that the completion inits don't
	 * migrate past the register writes due to gcc scheduling.
	 */
	mb();
	hw->dma_actually_done = 0;
	INIT_COMPLETION(hw->dma_done);
	mb();

	/*
	 * Wait until DMA tx FIFO has more than one byte (it reads one
	 * directly then one "very quickly") before starting sser tx.
	 */
	if (tr_cfg.tr_en) {
		u32 regi_dmaout = hw->dmaout.regi;
		u32 minlen = len > 2 ? 2 : len;
		while ((REG_RD_DO(rw_stat)).buf < minlen)
			;
	}

	/* Wait until DMA-in is finished reading the descriptors.  */
	if (rec_cfg.rec_en)
		while (DMA_BUSY(regi_dmain))
			;
	/*
	 * Wait 3 cycles before enabling (with .prepare = 1).
	 * FIXME: Can we cut this by some time already passed?
	 */
	ndelay(3 * 2 * hw->half_cycle_delay_ns);
	cfg.en = 1;
	REG_WR_SSER(rw_cfg, cfg);

	/*
	 * Wait 3 more cycles plus 30 ns before letting go.
	 * FIXME: Can we do something else before but after the
	 * previous cfg write and cut this by the time already passed?
	 */
	cfg.prepare = 0;
	hw->cfg = cfg;
	ndelay(3 * 2 * hw->half_cycle_delay_ns + 30);

	REG_WR_SSER(rw_cfg, cfg);

	/*, We'll disable sser next the time we change the configuration.  */
	cfg.en = 0;
	cfg.prepare = 1;
	hw->cfg = cfg;

	if (!use_irq) {
		/*
		 * We use a timeout corresponding to one iteration per ns,
		 * which of course is at least five * insns / loop times as
		 * much as reality, but we'll avoid a need for reading hw
		 * timers directly.
		 */
		u32 countdown = IRQ_USAGE_THRESHOLD_NS;

		do
			if (rec_cfg.rec_en == 0) {
				/* Using the transmitter only.  */
				reg_sser_r_intr intr = REG_RD_SSER(r_intr);

				if (intr.tidle != 0) {
					/*
					 * Almost done...  Just check if we
					 * had a transmitter underrun too.
					 */
					if (!intr.urun)
						goto transmission_done;

					/*
					 * Fall over to the "time is up" case;
					 * no need to provide a special path
					 * for the error case.
					 */
					countdown = 1;
				}
			} else {
				/* Using at least the receiver.  */
				if ((REG_RD_DI(r_intr)).data != 0) {
					if ((REG_RD_SSER(r_intr)).urun == 0)
						goto transmission_done;
					countdown = 1;
				}
			}
		while (--countdown != 0);

		/*
		 * The time is up.  Something might be wrong, or perhaps we've
		 * started using data lengths where the threshold was about a
		 * magnitude wrong.  Fall over to IRQ.  Remember not to ack
		 * interrupts here (but always above, before starting), else
		 * we'll have a race condition with the interrupt.
		 */
		if (!rec_cfg.rec_en) {
			reg_sser_rw_intr_mask mask = { .urun = 1, .tidle = 1 };
			REG_WR_SSER(rw_intr_mask, mask);
		} else {
			reg_dma_rw_intr_mask dmask = { .data = 1 };
			reg_sser_rw_intr_mask mask = { .urun = 1 };

			/*
			 * Never mind checking for tr being disabled; urun
			 * won't happen then.
			 */
			REG_WR_SSER(rw_intr_mask, mask);
			REG_WR_DI(rw_intr_mask, dmask);
		}
	}

	if (!wait_for_completion_timeout(&hw->dma_done, hw->dma_timeout)
	    /*
	     * Have to keep track manually too, else we'll get a timeout
	     * indication for being scheduled out too long, while the
	     * completion will still have trigged.
	     */
	    && !hw->dma_actually_done) {
		u32 regi_dmaout = hw->dmaout.regi;

		/*
		 * Transfer timed out.  Should not happen for a
		 * working controller, except perhaps if the system is
		 * badly conditioned, causing DMA memory bandwidth
		 * starvation.  Not much to do afterwards, but perhaps
		 * reset DMA and sser and hope it works the next time.
		 */
		REG_WRINT_SSER(rw_cfg, 0);
		REG_WR_SSER(rw_cfg, cfg);
		REG_WRINT_SSER(rw_intr_mask, 0);
		REG_WRINT_DI(rw_intr_mask, 0);
		REG_WRINT_SSER(rw_ack_intr, -1);
		crisv32_reset_dma_hw(hw->dmain.regi);
		crisv32_reset_dma_hw(hw->dmaout.regi);

		dev_err(&spi->dev, "timeout %u bytes %u kHz\n",
			len, hw->effective_speed_kHz);
		dev_err(&spi->dev, "sser=(%x,%x,%x,%x,%x)\n",
			REG_RDINT_SSER(rw_cfg), REG_RDINT_SSER(rw_tr_cfg),
			REG_RDINT_SSER(rw_rec_cfg), REG_RDINT_SSER(rw_extra),
			REG_RDINT_SSER(r_intr));
		dev_err(&spi->dev, "tx=(%x,%x,%x,%x)\n",
			dmaout, REG_RDINT_DO(rw_stat), REG_RDINT_DO(rw_data),
			REG_RDINT_DO(r_intr));
		dev_err(&spi->dev, "rx=(%x,%x,%x,%x)\n",
			dmain, REG_RDINT_DI(rw_stat), REG_RDINT_DI(rw_data),
			REG_RDINT_DI(r_intr));
		return -EIO;
	}

 transmission_done:
	/* Wait for the last half-cycle of the last cycle.  */
	crisv32_spi_sser_wait_halfabit(hw);

	/* Reset for another call.  */
	REG_WR_SSER(rw_cfg, cfg);

	/*
	 * If we had to use the temp DMAable rec buffer, copy it to the right
	 * position.
	 */
	if (t->rx_buf != 0 && t->rx_dma == 0)
		memcpy (t->rx_buf, cs->rx_buf, len);

	/*
	 * All clear.  The interrupt function disabled the interrupt, we don't
	 * have to do more.
	 */
	return len;
}

/* Platform-device probe function.  */

static int __devinit crisv32_spi_sser_probe(struct platform_device *dev)
{
	struct spi_master *master;
	struct crisv32_spi_sser_devdata *dd;
	struct crisv32_spi_hw_info *hw;
	struct resource *res;
	struct crisv32_spi_sser_controller_data *gc;
	int ret;

	/*
	 * We need to get the controller data as a hardware resource,
	 * or else it wouldn't be available until *after* the
	 * spi_bitbang_start call!
	 */
	res = platform_get_resource_byname(dev, 0, "controller_data_ptr");
	if (res == NULL) {
		dev_err(&dev->dev,
			"can't get controller_data resource at probe\n");
		return -EIO;
	}

	gc = (struct crisv32_spi_sser_controller_data *) res->start;

	master = spi_alloc_master(&dev->dev, sizeof *dd);
	if (master == NULL) {
		dev_err(&dev->dev, "failed to allocate spi master\n");
		ret = -ENOMEM;
		goto err;
	}

	dd = spi_master_get_devdata(master);
	platform_set_drvdata(dev, dd);

	/*
	 * The device data asks for this driver, and holds the id
	 * number, which must be unique among the same-type devices.
	 * We use this as the number of this SPI bus.
	 */
	master->bus_num = dev->id;

	/* Setup SPI bitbang adapter hooks.  */
	dd->bitbang.master = spi_master_get(master);
	dd->bitbang.chipselect = crisv32_spi_sser_chip_select_active_low;

	hw = &dd->hw;
	hw->gc = gc;

	/* Pre-spi_bitbang_start setup. */
	if (gc->using_dma) {
		/* Setup DMA and interrupts.  */
		ret = gc->iface_allocate(&hw->sser, &hw->dmain, &hw->dmaout);
		if (ret != 0)
			goto err_no_regs;

		dd->bitbang.master->setup = crisv32_spi_sser_dma_master_setup;
		dd->bitbang.setup_transfer
			= crisv32_spi_sser_common_setup_transfer;
		dd->bitbang.txrx_bufs = crisv32_spi_sser_dma_txrx_bufs;
		dd->bitbang.master->cleanup = crisv32_spi_sser_dma_cleanup;
	} else {
		/* Just registers, then.  */
		ret = gc->iface_allocate(&hw->sser, NULL, NULL);
		if (ret != 0)
			goto err_no_regs;

		dd->bitbang.master->setup
			= crisv32_spi_sser_regs_master_setup;
		dd->bitbang.setup_transfer
			= crisv32_spi_sser_regs_setup_transfer;
		dd->bitbang.master->cleanup = spi_bitbang_cleanup;

		/*
		 * We can do all modes pretty simply, but I have no
		 * simple enough way to test them, so I won't.
		 */
		dd->bitbang.txrx_word[SPI_MODE_3]
			= crisv32_spi_sser_txrx_mode3;
	}

	ret = spi_bitbang_start(&dd->bitbang);
	if (ret)
		goto err_no_bitbang;

	/*
	 * We don't have a dev_info here, as initialization that may fail is
	 * postponed to the first master->setup call.  It's called from
	 * spi_bitbang_start (above), where the call-chain doesn't look too
	 * close at error return values; we'll get here successfully anyway,
	 * so emitting a separate message here is at most confusing.
	 */
	dev_dbg(&dev->dev,
		"CRIS v32 SPI driver for sser%d%s present\n",
		master->bus_num,
		gc->using_dma ? "/DMA" : "");

	return 0;

 err_no_bitbang:
	gc->iface_free();

 err_no_regs:
	platform_set_drvdata(dev, NULL);
	spi_master_put(dd->bitbang.master);

 err:
	return ret;
}

/* Platform-device remove-function.  */

static int __devexit crisv32_spi_sser_remove(struct platform_device *dev)
{
	struct crisv32_spi_sser_devdata *dd = platform_get_drvdata(dev);
	struct crisv32_spi_hw_info *hw = &dd->hw;
	struct crisv32_spi_sser_controller_data *gc = hw->gc;
	int ret;

	/* We need to stop all bitbanging activity separately.  */
	ret = spi_bitbang_stop(&dd->bitbang);
	if (ret != 0)
		return ret;

	spi_master_put(dd->bitbang.master);

	/*
	 * If we get here, the queue is empty and there's no activity;
	 * it's safe to flip the switch on the interfaces.
	 */
	if (gc->using_dma) {
		u32 regi_dmain = hw->dmain.regi;
		u32 regi_dmaout = hw->dmaout.regi;
		u32 regi_sser = hw->sser.regi;

		REG_WRINT_SSER(rw_intr_mask, 0);
		REG_WRINT_DI(rw_intr_mask, 0);
		REG_WRINT_DO(rw_intr_mask, 0);
		hw->cfg.en = 0;
		REG_WR_SSER(rw_cfg, hw->cfg);
		DMA_RESET(regi_dmain);
		DMA_RESET(regi_dmaout);
		free_irq(hw->sser.irq, hw);
		free_irq(hw->dmain.irq, hw);
	}

	gc->iface_free();

	platform_set_drvdata(dev, NULL);
	return 0;
}

/*
 * For the time being, there's no suspend/resume support to care
 * about, so those handlers default to NULL.
 */
static struct platform_driver crisv32_spi_sser_drv = {
	.probe		= crisv32_spi_sser_probe,
	.remove		= __devexit_p(crisv32_spi_sser_remove),
	.driver		= {
		.name	= "spi_crisv32_sser",
		.owner	= THIS_MODULE,
	},
};

/* Module init function.  */

static int __devinit crisv32_spi_sser_init(void)
{
	return platform_driver_register(&crisv32_spi_sser_drv);
}

/* Module exit function.  */

static void __devexit crisv32_spi_sser_exit(void)
{
	platform_driver_unregister(&crisv32_spi_sser_drv);
}

/* Setter function for speed limit.  */

static int crisv32_spi_speed_limit_Hz_setter(const char *val,
					     struct kernel_param *kp)
{
	char *endp;
	ulong num = simple_strtoul(val, &endp, 0);
	if (endp == val
	    || *endp != 0
	    || num <= 0
	    /*
	     * We can't go above 100 MHz speed.  Actually we can't go
	     * above 50 MHz using the sser support but it might make
	     * sense trying.
	     */
	    || num > 100000000)
		return -EINVAL;
	*(ulong *) kp->arg = num;
	return 0;
}

module_param_call(crisv32_spi_max_speed_hz,
		  crisv32_spi_speed_limit_Hz_setter, param_get_ulong,
		  &crisv32_spi_speed_limit_Hz, 0644);

module_init(crisv32_spi_sser_init);
module_exit(crisv32_spi_sser_exit);

MODULE_DESCRIPTION("CRIS v32 SPI-SSER Driver");
MODULE_AUTHOR("Hans-Peter Nilsson, <hp@axis.com>");
MODULE_LICENSE("GPL");
