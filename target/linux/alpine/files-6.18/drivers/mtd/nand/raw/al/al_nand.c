/*
 * Annapurna Labs Nand driver.
 *
 * Copyright (C) 2013 Annapurna Labs Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * TODO:
 * - add sysfs statistics
 * - use dma for reading writing
 * - get config parameters from device tree instead of config registers
 * - use correct ECC size and not entire OOB
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/platform_device.h>
#include <linux/slab.h>

#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/mtd/rawnand.h>

#include "al_hal_nand.h"
#include "al_hal_nand_regs.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Annapurna Labs");

#define WAIT_EMPTY_CMD_FIFO_TIME_OUT 1000000
#define AL_NAND_NAME "al-nand"
#define AL_NAND_MAX_ONFI_TIMING_MODE 1

#define AL_NAND_MAX_BIT_FLIPS 4
#define AL_NAND_MAX_OOB_SIZE SZ_1K
#define AL_NAND_STATUS_TIMEOUT_US 200000

#define NAND_SET_FEATURES_ADDR 0xfa

#define ONFI_COL_ADDR_CYCLE_MASK 0xf0
#define ONFI_COL_ADDR_CYCLE_POS 4
#define ONFI_ROW_ADDR_CYCLE_MASK 0x0f
#define ONFI_ROW_ADDR_CYCLE_POS 0

#define AL_NAND_MAX_CHIPS 4
#define AL_NAND_DEFAULT_CHIPS AL_NAND_MAX_CHIPS
#define AL_NAND_ECC_SUPPORT

static const char *probes[] = {"cmdlinepart", "ofpart", NULL};

struct nand_data {
  struct nand_chip chip;
  struct nand_controller controller;
  struct mtd_info mtd;
  struct platform_device *pdev;

  struct al_nand_ctrl_obj nand_obj;
  uint8_t word_cache[4];
  int cache_pos;
  uint32_t cw_size;
  struct al_nand_dev_properties device_properties;
  struct al_nand_extra_dev_properties dev_ext_props;
  struct al_nand_ecc_config ecc_config;

  /*** interrupts ***/
  struct completion complete;
  spinlock_t irq_lock;
  uint32_t irq_status;
  int irq;

  uint8_t oob[AL_NAND_MAX_OOB_SIZE];

  uint32_t s_ecc_loc;
  uint32_t s_oob_size;
  uint32_t s_ecc_oob_bytes;
  uint32_t max_chips;
  bool compact_ecc_oob;
};

static void nand_dump_ctrl_regs(struct nand_data *nand, const char *reason) {
  struct al_nand_regs *regs = nand->nand_obj.regs_base;
  int ready = -1;

  if (nand->nand_obj.current_dev_index >= 0)
    ready = al_nand_dev_is_ready(&nand->nand_obj);

  dev_err(&nand->pdev->dev,
          "%s: %s: dev=%d ready=%d irq=%d irq_status=0x%08x int_en=0x%08x int_stat=0x%08x\n",
          __func__, reason, nand->nand_obj.current_dev_index, ready, nand->irq,
          nand->irq_status, readl(&regs->nfc_int_en),
          readl(&regs->nfc_int_stat));
  dev_err(&nand->pdev->dev,
          "%s: %s: flash_ctl_3=0x%08x mode=0x%08x ctl0=0x%08x cmd=0x%08x row=0x%08x col=0x%08x\n",
          __func__, reason, readl(&regs->flash_ctl_3),
          readl(&regs->mode_select_reg), readl(&regs->ctl_reg0),
          readl(&regs->command_reg), readl(&regs->nflash_row_addr),
          readl(&regs->nflash_col_addr));
  dev_err(&nand->pdev->dev,
          "%s: %s: cw=0x%08x spare=0x%08x rdy_busy_wait=0x%08x rdy_busy=0x%08x flash_status=0x%08x reset_status=0x%08x\n",
          __func__, reason, readl(&regs->codeword_size_cnt_reg),
          readl(&regs->nflash_spare_offset),
          readl(&regs->rdy_busy_wait_cnt_reg),
          readl(&regs->rdy_busy_status_reg), readl(&regs->flash_status_reg),
          readl(&regs->reset_status_reg));
}

static int nand_wait_for_status(struct nand_data *nand, uint32_t mask,
                                const char *name) {
  uint32_t status = 0;
  int i;

  al_nand_int_enable(&nand->nand_obj, mask);
  for (i = 0; i < AL_NAND_STATUS_TIMEOUT_US; i++) {
    status = al_nand_int_status_get(&nand->nand_obj);
    if (status & mask) {
      al_nand_int_clear(&nand->nand_obj, mask);
      al_nand_int_disable(&nand->nand_obj, mask);
      return 0;
    }
    udelay(1);
  }

  al_nand_int_disable(&nand->nand_obj, mask);
  dev_err(&nand->pdev->dev,
          "%s: timeout waiting for %s (mask=0x%08x status=0x%08x)\n",
          __func__, name, mask, status);
  nand_dump_ctrl_regs(nand, name);
  return -ETIMEDOUT;
}

static int nand_wait_for_read_ready(struct nand_data *nand) {
  return nand_wait_for_status(nand, AL_NAND_INTR_STATUS_BUF_RDRDY,
                              "BUF_RDRDY");
}

static int nand_wait_for_write_ready(struct nand_data *nand) {
  return nand_wait_for_status(nand, AL_NAND_INTR_STATUS_BUF_WRRDY,
                              "BUF_WRRDY");
}

/*
 * Addressing RMN: 2903
 *
 * RMN description:
 * NAND timing parameters that are used in the non-manual mode are wrong and
 * reduce performance.
 * Replacing with the manual parameters to increase speed
 */
#define AL_NAND_NSEC_PER_CLK_CYCLES 2.666
#define NAND_CLK_CYCLES(nsec) ((nsec) / (AL_NAND_NSEC_PER_CLK_CYCLES))

const struct al_nand_device_timing al_nand_manual_timing[] = {
    {.tSETUP = NAND_CLK_CYCLES(14),
     .tHOLD = NAND_CLK_CYCLES(22),
     .tWRP = NAND_CLK_CYCLES(54),
     .tRR = NAND_CLK_CYCLES(43),
     .tWB = NAND_CLK_CYCLES(206),
     .tWH = NAND_CLK_CYCLES(32),
     .tINTCMD = NAND_CLK_CYCLES(86),
     .readDelay = NAND_CLK_CYCLES(3)},
    {.tSETUP = NAND_CLK_CYCLES(14),
     .tHOLD = NAND_CLK_CYCLES(14),
     .tWRP = NAND_CLK_CYCLES(27),
     .tRR = NAND_CLK_CYCLES(22),
     .tWB = NAND_CLK_CYCLES(104),
     .tWH = NAND_CLK_CYCLES(19),
     .tINTCMD = NAND_CLK_CYCLES(43),
     .readDelay = NAND_CLK_CYCLES(3)},
    {.tSETUP = NAND_CLK_CYCLES(14),
     .tHOLD = NAND_CLK_CYCLES(14),
     .tWRP = NAND_CLK_CYCLES(19),
     .tRR = NAND_CLK_CYCLES(22),
     .tWB = NAND_CLK_CYCLES(104),
     .tWH = NAND_CLK_CYCLES(16),
     .tINTCMD = NAND_CLK_CYCLES(43),
     .readDelay = NAND_CLK_CYCLES(3)},
    {.tSETUP = NAND_CLK_CYCLES(14),
     .tHOLD = NAND_CLK_CYCLES(14),
     .tWRP = NAND_CLK_CYCLES(16),
     .tRR = NAND_CLK_CYCLES(22),
     .tWB = NAND_CLK_CYCLES(104),
     .tWH = NAND_CLK_CYCLES(11),
     .tINTCMD = NAND_CLK_CYCLES(27),
     .readDelay = NAND_CLK_CYCLES(3)},
    {.tSETUP = NAND_CLK_CYCLES(14),
     .tHOLD = NAND_CLK_CYCLES(14),
     .tWRP = NAND_CLK_CYCLES(14),
     .tRR = NAND_CLK_CYCLES(22),
     .tWB = NAND_CLK_CYCLES(104),
     .tWH = NAND_CLK_CYCLES(11),
     .tINTCMD = NAND_CLK_CYCLES(27),
     .readDelay = NAND_CLK_CYCLES(3)},
    {.tSETUP = NAND_CLK_CYCLES(14),
     .tHOLD = NAND_CLK_CYCLES(14),
     .tWRP = NAND_CLK_CYCLES(14),
     .tRR = NAND_CLK_CYCLES(22),
     .tWB = NAND_CLK_CYCLES(104),
     .tWH = NAND_CLK_CYCLES(11),
     .tINTCMD = NAND_CLK_CYCLES(27),
     .readDelay = NAND_CLK_CYCLES(3)}};

static inline struct nand_data *nand_data_get(struct mtd_info *mtd) {
  struct nand_chip *nand_chip = mtd_to_nand(mtd);

  return nand_get_controller_data(nand_chip);
}

static uint32_t al_nand_max_chips_get(struct platform_device *pdev) {
  struct device_node *np = pdev->dev.of_node;
  uint32_t max_chips = AL_NAND_DEFAULT_CHIPS;
  const char *source = "driver-default";
  int ret_al = -EINVAL;
  int ret_num = -EINVAL;
  int ret_nand = -EINVAL;

  if (np) {
    ret_al = of_property_read_u32(np, "al,num-cs", &max_chips);
    if (!ret_al) {
      source = "al,num-cs";
    } else {
      ret_num = of_property_read_u32(np, "num-cs", &max_chips);
      if (!ret_num) {
        source = "num-cs";
      } else {
        ret_nand = of_property_read_u32(np, "nand-num-cs", &max_chips);
        if (!ret_nand)
          source = "nand-num-cs";
      }
    }
  }

  if (!max_chips || max_chips > AL_NAND_MAX_CHIPS) {
    dev_warn(&pdev->dev, "invalid max chip-selects %u, using %u\n",
             max_chips, AL_NAND_DEFAULT_CHIPS);
    max_chips = AL_NAND_DEFAULT_CHIPS;
  }

  dev_dbg(&pdev->dev,
          "debug: max_chips=%u source=%s ret_al=%d ret_num=%d ret_nand=%d of_node=%d\n",
          max_chips, source, ret_al, ret_num, ret_nand, np ? 1 : 0);
  return max_chips;
}

static void nand_cw_size_get(int num_bytes, uint32_t *cw_size,
                             uint32_t *cw_count) {
  num_bytes = AL_ALIGN_UP(num_bytes, 4);

  if (num_bytes < *cw_size)
    *cw_size = num_bytes;

  if (0 != (num_bytes % *cw_size))
    *cw_size = num_bytes / 4;

  BUG_ON(num_bytes % *cw_size);

  *cw_count = num_bytes / *cw_size;
}

static void nand_send_byte_count_command(struct al_nand_ctrl_obj *nand_obj,
                                         enum al_nand_command_type cmd_id,
                                         uint16_t len) {
  uint32_t cmd;

  cmd = AL_NAND_CMD_SEQ_ENTRY(cmd_id, (len & 0xff));

  al_nand_cmd_single_execute(nand_obj, cmd);

  cmd = AL_NAND_CMD_SEQ_ENTRY(cmd_id, ((len & 0xff00) >> 8));

  al_nand_cmd_single_execute(nand_obj, cmd);
}

static void nand_wait_cmd_fifo_empty(struct nand_data *nand) {
  uint32_t i = WAIT_EMPTY_CMD_FIFO_TIME_OUT;
  int cmd_buff_empty;

  while (i > 0) {
    cmd_buff_empty = al_nand_cmd_buff_is_empty(&nand->nand_obj);
    if (cmd_buff_empty)
      break;

    udelay(1);
    i--;
  }

  if (i == 0)
    dev_err(&nand->pdev->dev,
            "%s: waited for empty cmd fifo for more than a sec!\n", __func__);
}

static void nand_cmd_ctrl(struct nand_chip *chip, int dat, unsigned int ctrl) {
  struct mtd_info *mtd;
  struct nand_data *nand;
  enum al_nand_command_type type;
  uint32_t cmd;

  mtd = nand_to_mtd(chip);
  nand = nand_data_get(mtd);

  dev_dbg(&nand->pdev->dev, "%s: dat 0x%x ctrl 0x%x\n", __func__, dat, ctrl);

  if ((ctrl & (NAND_CLE | NAND_ALE)) == 0) {
    dev_dbg(&nand->pdev->dev, "%s: drop cmd ctrl\n", __func__);
    return;
  }

  nand->cache_pos = -1;

  type = ((ctrl & NAND_CTRL_CLE) == NAND_CTRL_CLE)
             ? AL_NAND_COMMAND_TYPE_CMD
             : AL_NAND_COMMAND_TYPE_ADDRESS;
  cmd = AL_NAND_CMD_SEQ_ENTRY(type, (dat & 0xff));

  dev_dbg(&nand->pdev->dev, "%s: type 0x%x cmd 0x%x\n", __func__, type, cmd);

  al_nand_cmd_single_execute(&nand->nand_obj, cmd);

  nand_wait_cmd_fifo_empty(nand);

  if ((dat == NAND_CMD_PAGEPROG) && (ctrl & NAND_CLE)) {
    cmd = AL_NAND_CMD_SEQ_ENTRY(AL_NAND_COMMAND_TYPE_WAIT_FOR_READY, 0);

    dev_dbg(&nand->pdev->dev, "%s: pageprog cmd = 0x%x\n", __func__, cmd);

    al_nand_cmd_single_execute(&nand->nand_obj, cmd);

    nand_wait_cmd_fifo_empty(nand);

    al_nand_wp_set_enable(&nand->nand_obj, 1);
    al_nand_tx_set_enable(&nand->nand_obj, 0);
  }
}

static void nand_dev_select(struct nand_chip *chip, int chipnr) {
  struct mtd_info *mtd;
  struct nand_data *nand;

  mtd = nand_to_mtd(chip);
  nand = nand_data_get(mtd);

  dev_dbg(&nand->pdev->dev, "%s: chipnr %d\n", __func__, chipnr);

  if (chipnr < 0)
    return;

  al_nand_dev_select(&nand->nand_obj, chipnr);
}

static int nand_dev_ready(struct nand_chip *chip) {
  struct mtd_info *mtd;
  struct nand_data *nand;
  int is_ready = 0;

  mtd = nand_to_mtd(chip);
  nand = nand_data_get(mtd);

  is_ready = al_nand_dev_is_ready(&nand->nand_obj);

  dev_dbg(&nand->pdev->dev, "%s: is_ready %d\n", __func__, is_ready);

  return is_ready;
}

/*
 * read len bytes from the nand device.
 */
static void nand_read_buff(struct nand_chip *chip, uint8_t *buf, int len) {
  uint32_t cw_size;
  uint32_t cw_count;
  struct mtd_info *mtd;
  struct nand_data *nand;
  int ret;

  mtd = nand_to_mtd(chip);
  nand = nand_data_get(mtd);

  dev_dbg(&nand->pdev->dev, "%s: len %d\n", __func__, len);

  cw_size = nand->cw_size;

  BUG_ON(len & 3);
  BUG_ON(nand->cache_pos != -1);

  nand_cw_size_get(len, &cw_size, &cw_count);

  dev_dbg(&nand->pdev->dev, "%s: cw_size %d cw_count %d\n", __func__, cw_size,
          cw_count);

  al_nand_cw_config(&nand->nand_obj, cw_size, cw_count);

  while (cw_count--)
    nand_send_byte_count_command(&nand->nand_obj,
                                 AL_NAND_COMMAND_TYPE_DATA_READ_COUNT, cw_size);

  while (len > 0) {
    dev_dbg(&nand->pdev->dev, "%s: waiting for read to become ready, len %d\n",
            __func__, len);

    ret = nand_wait_for_read_ready(nand);
    if (ret) {
      dev_err(&nand->pdev->dev, "%s: timeout occurred, len %d\n", __func__,
              len);
      break;
    }

    dev_dbg(&nand->pdev->dev, "%s: read ready\n", __func__);

    al_nand_data_buff_read(&nand->nand_obj, cw_size, 0, 0, buf);

    buf += cw_size;
    len -= cw_size;
  }
}

/*
 * read byte from the device.
 * read byte is not supported by the controller so this function reads
 * 4 bytes as a cache and use it in the next calls.
 */
static uint8_t nand_read_byte_from_fifo(struct nand_chip *chip) {
  struct mtd_info *mtd;
  struct nand_data *nand;
  uint8_t ret_val;

  mtd = nand_to_mtd(chip);
  nand = nand_data_get(mtd);

  dev_dbg(&nand->pdev->dev, "%s: cache_pos %d", __func__, nand->cache_pos);

  if (nand->cache_pos == -1) {
    nand_read_buff(chip, nand->word_cache, 4);
    nand->cache_pos = 0;
  }

  ret_val = nand->word_cache[nand->cache_pos];
  nand->cache_pos++;
  if (nand->cache_pos == 4)
    nand->cache_pos = -1;

  dev_dbg(&nand->pdev->dev, "%s: ret_val = 0x%x\n", __func__, ret_val);

  return ret_val;
}

/*
 * writing buffer to the nand device.
 * this func will wait for the write to be complete
 */
static void nand_write_buff(struct nand_chip *chip, const uint8_t *buf,
                            int len) {
  struct mtd_info *mtd = nand_to_mtd(chip);
  uint32_t cw_size = mtd_to_nand(mtd)->ecc.size;
  uint32_t cw_count;
  struct nand_data *nand;
  void __iomem *data_buff;
  int ret;

  nand = nand_data_get(mtd);

  dev_dbg(&nand->pdev->dev, "%s: cw_size %d len %d start: 0x%x%x%x\n", __func__,
          cw_size, len, buf[0], buf[1], buf[2]);

  al_nand_tx_set_enable(&nand->nand_obj, 1);
  al_nand_wp_set_enable(&nand->nand_obj, 0);

  nand_cw_size_get(len, &cw_size, &cw_count);

  dev_dbg(&nand->pdev->dev, "%s: cw_size %d cw_count %d\n", __func__, cw_size,
          cw_count);

  al_nand_cw_config(&nand->nand_obj, cw_size, cw_count);

  while (cw_count--)
    nand_send_byte_count_command(
        &nand->nand_obj, AL_NAND_COMMAND_TYPE_DATA_WRITE_COUNT, cw_size);

  while (len > 0) {
    dev_dbg(&nand->pdev->dev, "%s: waiting for write to become ready, len %d\n",
            __func__, len);

    ret = nand_wait_for_write_ready(nand);
    if (ret) {
      dev_err(&nand->pdev->dev, "%s: timeout occurred, len %d\n", __func__,
              len);
      break;
    }

    dev_dbg(&nand->pdev->dev, "%s: write ready\n", __func__);

    data_buff = al_nand_data_buff_base_get(&nand->nand_obj);
    memcpy(data_buff, buf, cw_size);

    buf += cw_size;
    len -= cw_size;
  }

  /* enable wp and disable tx will be executed after commands
   * NAND_CMD_PAGEPROG and AL_NAND_COMMAND_TYPE_WAIT_FOR_READY will be
   * sent to make sure all data were written.
   */
}

/******************************************************************************/
/**************************** ecc functions ***********************************/
/******************************************************************************/
#ifdef AL_NAND_ECC_SUPPORT

static inline int is_empty_oob(uint8_t *oob, int len) {
  int flips = 0;
  int i;
  int j;

  for (i = 0; i < len; i++) {
    if (oob[i] == 0xff)
      continue;

    for (j = 0; j < 8; j++) {
      if ((oob[i] & BIT(j)) == 0) {
        flips++;
        if (flips >= AL_NAND_MAX_BIT_FLIPS)
          break;
      }
    }
  }

  if (flips < AL_NAND_MAX_BIT_FLIPS)
    return 1;

  return 0;
}
/*
 * read page with HW ecc support (corrected and uncorrected stat will be
 * updated).
 */
static int ecc_read_page(struct nand_chip *chip, uint8_t *buf, int oob_required,
                         int page) {
  struct mtd_info *mtd;
  struct nand_data *nand;
  int uncorr_err_count = 0;
  int corr_err_count = 0;
  int ret;

  BUG_ON(oob_required);

  mtd = nand_to_mtd(chip);
  nand = nand_data_get(mtd);

  dev_dbg(&nand->pdev->dev, "%s: oob_required %d page %d\n", __func__,
          oob_required, page);

  /* Clear TX/RX ECC state machine */
  al_nand_tx_set_enable(&nand->nand_obj, 1);
  al_nand_tx_set_enable(&nand->nand_obj, 0);

  al_nand_uncorr_err_clear(&nand->nand_obj);
  al_nand_corr_err_clear(&nand->nand_obj);

  al_nand_ecc_set_enabled(&nand->nand_obj, 1);

  /* First need to read the OOB to the controller to calc the ecc */
  chip->legacy.cmdfunc(chip, NAND_CMD_READOOB, nand->s_ecc_loc, page);

  nand_send_byte_count_command(&nand->nand_obj,
                               AL_NAND_COMMAND_TYPE_SPARE_READ_COUNT,
                               nand->s_ecc_oob_bytes);

  /* move to the start of the page to read the data */
  chip->legacy.cmdfunc(chip, NAND_CMD_RNDOUT, 0x00, -1);

  /* read the buffer (after ecc correction) */
  chip->legacy.read_buf(chip, buf, mtd->writesize);
  ret = 0;

  uncorr_err_count = al_nand_uncorr_err_get(&nand->nand_obj);
  corr_err_count = al_nand_corr_err_get(&nand->nand_obj);

  al_nand_ecc_set_enabled(&nand->nand_obj, 0);

  /* update statistics*/
  if (uncorr_err_count != 0) {
    bool uncorr_err = true;
    /*
     * Hardware ECC can report an uncorrectable error on erased pages.
     * Suppress that case after checking the actual OOB bytes.
     */
    chip->legacy.read_buf(chip, nand->oob, mtd->oobsize);

    if (is_empty_oob(nand->oob, mtd->oobsize))
      uncorr_err = false;

    if (uncorr_err) {
      mtd->ecc_stats.failed++;
      dev_err(&nand->pdev->dev,
              "%s uncorrected errors found in page %d (increased to %d, alg=%d corr_bits=%d msg=%d ecc_loc=%u spare_read_bytes=%u)\n",
              __func__, page, mtd->ecc_stats.failed,
              nand->ecc_config.algorithm, nand->ecc_config.num_corr_bits,
              nand->ecc_config.messageSize, nand->s_ecc_loc,
              nand->s_ecc_oob_bytes);
      if (mtd->ecc_stats.failed <= 8)
        dev_dbg(&nand->pdev->dev, "%s debug: page %d oob[0..31]=%*phN\n",
                __func__, page, min_t(int, 32, mtd->oobsize), nand->oob);
    }
  }

  if (corr_err_count != 0) {
    mtd->ecc_stats.corrected++;
    dev_dbg(&nand->pdev->dev, "%s: corrected increased\n", __func__);
  }

  dev_dbg(&nand->pdev->dev, "%s: total corrected %d\n", __func__,
          mtd->ecc_stats.corrected);

  return ret;
}

static int ecc_read_subpage(struct nand_chip *chip, uint32_t offs, uint32_t len,
                            uint8_t *buf, int page) {
  struct mtd_info *mtd;
  struct nand_data *nand;

  mtd = nand_to_mtd(chip);
  nand = nand_data_get(mtd);

  dev_dbg(&nand->pdev->dev, "read subpage not supported!\n");
  return -1;
}
/*
 * program page with HW ecc support.
 * this function is called after the commands and adderess for this page sent.
 */
static int ecc_write_page(struct nand_chip *chip, const uint8_t *buf,
                          int oob_required, int page) {
  struct mtd_info *mtd;
  struct nand_data *nand;
  uint32_t cmd;

  BUG_ON(oob_required);

  mtd = nand_to_mtd(chip);
  nand = nand_data_get(mtd);

  dev_dbg(&nand->pdev->dev,
          "%s: page %d writesize %d ecc_loc %d ecc_oob_bytes %d\n", __func__,
          page, mtd->writesize, nand->s_ecc_loc, nand->s_ecc_oob_bytes);

  nand_prog_page_begin_op(chip, page, 0, NULL, 0);

  al_nand_ecc_set_enabled(&nand->nand_obj, 1);

  nand_write_buff(chip, buf, mtd->writesize);

  /* First need to read the OOB to the controller to calc the ecc */
  chip->legacy.cmdfunc(chip, NAND_CMD_RNDIN, mtd->writesize + nand->s_ecc_loc,
                       -1);

  cmd = AL_NAND_CMD_SEQ_ENTRY(AL_NAND_COMMAND_TYPE_WAIT_CYCLE_COUNT, 0);

  al_nand_tx_set_enable(&nand->nand_obj, 1);
  al_nand_wp_set_enable(&nand->nand_obj, 0);

  al_nand_cmd_single_execute(&nand->nand_obj, cmd);

  nand_send_byte_count_command(&nand->nand_obj,
                               AL_NAND_COMMAND_TYPE_SPARE_WRITE_COUNT,
                               nand->s_ecc_oob_bytes);

  nand_wait_cmd_fifo_empty(nand);

  al_nand_wp_set_enable(&nand->nand_obj, 1);
  al_nand_tx_set_enable(&nand->nand_obj, 0);

  al_nand_ecc_set_enabled(&nand->nand_obj, 0);

  return nand_prog_page_end_op(chip);
}

#endif /* #ifdef AL_NAND_ECC_SUPPORT */

/******************************************************************************/
/****************************** interrupts ************************************/
/******************************************************************************/
static irqreturn_t al_nand_isr(int irq, void *dev_id);

static void nand_interrupt_init(struct nand_data *nand) {
  int ret;

  init_completion(&nand->complete);
  spin_lock_init(&nand->irq_lock);
  nand->irq_status = 0;
  al_nand_int_disable(&nand->nand_obj, 0xffff);
  al_nand_int_clear(&nand->nand_obj, 0xffff);

  ret = request_irq(nand->irq, al_nand_isr, IRQF_SHARED, AL_NAND_NAME, nand);
  if (ret)
    dev_err(&nand->pdev->dev, "%s: failed to request irq %d (%d)\n", __func__,
            nand->irq, ret);
}
/*
 * ISR for nand interrupts - save the interrupt status, disable this interrupts
 * and notify the waiting proccess.
 */
static irqreturn_t al_nand_isr(int irq, void *dev_id) {
  struct nand_data *nand = dev_id;

  nand->irq_status = al_nand_int_status_get(&nand->nand_obj);
  al_nand_int_disable(&nand->nand_obj, nand->irq_status);

  dev_dbg(&nand->pdev->dev, "%s: irq_status 0x%x\n", __func__,
          nand->irq_status);

  complete(&nand->complete);

  return IRQ_HANDLED;
}

/******************************************************************************/
/**************************** configuration ***********************************/
/******************************************************************************/
static void nand_set_timing_mode(struct nand_data *nand,
                                 enum al_nand_device_timing_mode timing) {
  uint32_t cmds[] = {
      AL_NAND_CMD_SEQ_ENTRY(AL_NAND_COMMAND_TYPE_CMD, NAND_CMD_SET_FEATURES),
      AL_NAND_CMD_SEQ_ENTRY(AL_NAND_COMMAND_TYPE_ADDRESS,
                            NAND_SET_FEATURES_ADDR),
      AL_NAND_CMD_SEQ_ENTRY(AL_NAND_COMMAND_TYPE_STATUS_WRITE, timing),
      AL_NAND_CMD_SEQ_ENTRY(AL_NAND_COMMAND_TYPE_STATUS_WRITE, 0x00),
      AL_NAND_CMD_SEQ_ENTRY(AL_NAND_COMMAND_TYPE_STATUS_WRITE, 0x00),
      AL_NAND_CMD_SEQ_ENTRY(AL_NAND_COMMAND_TYPE_STATUS_WRITE, 0x00)};

  al_nand_cmd_seq_execute(&nand->nand_obj, cmds, ARRAY_SIZE(cmds));

  nand_wait_cmd_fifo_empty(nand);
}

static int nand_resources_get_and_map(struct platform_device *pdev,
                                      void __iomem **nand_base,
                                      void __iomem **pbs_base) {
  struct device_node *np;

  np = of_find_compatible_node(NULL, NULL, "annapurna-labs,al-nand");

  *nand_base = of_iomap(np, 0);
  if (!(*nand_base)) {
    pr_err("%s: failed to map nand memory\n", __func__);
    return -ENOMEM;
  }

  np = of_find_compatible_node(NULL, NULL, "annapurna-labs,al-pbs");

  *pbs_base = of_iomap(np, 0);
  if (!(*pbs_base)) {
    pr_err("%s: pbs_base map failed\n", __func__);
    return -ENOMEM;
  }

  return 0;
}

static int nand_sync_detected_geometry(struct nand_chip *nand) {
  struct mtd_info *mtd = nand_to_mtd(nand);
  struct nand_data *nand_dat = nand_data_get(mtd);
  struct al_nand_dev_properties *dev_props = &nand_dat->device_properties;
  struct al_nand_extra_dev_properties *ext_props = &nand_dat->dev_ext_props;
  struct al_nand_ecc_config *ecc_config = &nand_dat->ecc_config;
  unsigned int pbs_page_size = ext_props->pageSize;
  unsigned int pbs_block_size = ext_props->blockSize;
  int pbs_spare_offset = ecc_config->spareAreaOffset;
  int ecc_loc = ecc_config->spareAreaOffset - (int)ext_props->pageSize;
  bool geometry_mismatch;

  switch (mtd->writesize) {
  case 512:
    dev_props->pageSize = AL_NAND_DEVICE_PAGE_SIZE_512;
    dev_props->num_col_cyc = 1;
    break;
  case 2048:
    dev_props->pageSize = AL_NAND_DEVICE_PAGE_SIZE_2K;
    dev_props->num_col_cyc = 2;
    break;
  case 4096:
    dev_props->pageSize = AL_NAND_DEVICE_PAGE_SIZE_4K;
    dev_props->num_col_cyc = 2;
    break;
  case 8192:
    dev_props->pageSize = AL_NAND_DEVICE_PAGE_SIZE_8K;
    dev_props->num_col_cyc = 2;
    break;
  case 16384:
    dev_props->pageSize = AL_NAND_DEVICE_PAGE_SIZE_16K;
    dev_props->num_col_cyc = 2;
    break;
  default:
    dev_err(&nand_dat->pdev->dev, "%s: unsupported detected page size %u\n",
            __func__, mtd->writesize);
    return -EINVAL;
  }

  if (mtd->size > (u64)mtd->writesize * 0x10000ULL)
    dev_props->num_row_cyc = 3;
  else
    dev_props->num_row_cyc = 2;

  if (ecc_loc < 0 || ecc_loc > mtd->oobsize) {
    dev_warn(&nand_dat->pdev->dev,
             "%s: invalid PBS ECC OOB offset %d for detected OOB %u, using 4\n",
             __func__, ecc_loc, mtd->oobsize);
    ecc_loc = min_t(int, 4, mtd->oobsize);
  }

  geometry_mismatch = pbs_page_size != mtd->writesize ||
                      pbs_block_size != mtd->erasesize ||
                      pbs_spare_offset != (int)mtd->writesize + ecc_loc;

  if (geometry_mismatch && !nand->parameters.onfi &&
      mtd->writesize == 2048 && mtd->oobsize == 64 &&
      ext_props->eccIsEnabled) {
    dev_warn(&nand_dat->pdev->dev,
             "%s: non-ONFI 2K/64 NAND has mismatching PBS geometry, using legacy Hamming ECC instead of PBS ECC alg=%d corr=%d msg=%d\n",
             __func__, ecc_config->algorithm, ecc_config->num_corr_bits,
             ecc_config->messageSize);
    ecc_config->algorithm = AL_NAND_ECC_ALGORITHM_HAMMING;
    ecc_config->num_corr_bits = 0;
    ecc_config->messageSize = AL_NAND_ECC_BCH_MESSAGE_SIZE_512;
    ecc_loc = min_t(int, 4, mtd->oobsize);
    nand_dat->compact_ecc_oob = false;
  }

  ext_props->pageSize = mtd->writesize;
  ext_props->blockSize = mtd->erasesize;
  ecc_config->spareAreaOffset = ext_props->pageSize + ecc_loc;
  nand_dat->cw_size = 512 << ecc_config->messageSize;

  if (geometry_mismatch) {
    dev_warn(&nand_dat->pdev->dev,
             "%s: PBS geometry differs from detected NAND, using detected geometry: pbs page=%u block=%u spare=%d, detected page=%u block=%u oob=%u spare=%d row=%d col=%d ecc_alg=%d ecc_corr=%d ecc_msg=%d\n",
             __func__, pbs_page_size, pbs_block_size, pbs_spare_offset,
             mtd->writesize, mtd->erasesize, mtd->oobsize,
             ecc_config->spareAreaOffset, dev_props->num_row_cyc,
             dev_props->num_col_cyc, ecc_config->algorithm,
             ecc_config->num_corr_bits, ecc_config->messageSize);
  } else {
    dev_info(&nand_dat->pdev->dev,
             "%s: detected geometry matches PBS: page=%u block=%u oob=%u spare=%d row=%d col=%d ecc_alg=%d ecc_corr=%d ecc_msg=%d\n",
             __func__, mtd->writesize, mtd->erasesize, mtd->oobsize,
             ecc_config->spareAreaOffset, dev_props->num_row_cyc,
             dev_props->num_col_cyc, ecc_config->algorithm,
             ecc_config->num_corr_bits, ecc_config->messageSize);
  }

  return 0;
}

static void
nand_onfi_config_set(struct nand_chip *nand,
                     struct al_nand_dev_properties *device_properties,
                     struct al_nand_ecc_config *ecc_config) {
  struct mtd_info *mtd = nand_to_mtd(nand);
  struct nand_data *nand_dat = nand_data_get(mtd);
  int i;

  dev_dbg(&nand_dat->pdev->dev,
          "%s: debug: onfi=%d writesize=%u oobsize=%u erasesize=%u pbs_timing=%d pbs_page=%d pbs_block=%u ecc_alg=%d ecc_corr=%d ecc_msg=%d ecc_spare_off=%d\n",
          __func__, nand->parameters.onfi ? 1 : 0, mtd->writesize,
          mtd->oobsize, mtd->erasesize, device_properties->timingMode,
          device_properties->pageSize, nand_dat->dev_ext_props.blockSize,
          ecc_config->algorithm, ecc_config->num_corr_bits,
          ecc_config->messageSize, ecc_config->spareAreaOffset);

  if (!nand->parameters.onfi) {
    dev_info(&nand_dat->pdev->dev,
             "%s: non-ONFI NAND, keeping PBS timing mode %d\n", __func__,
             device_properties->timingMode);
    return;
  }

  dev_info(&nand_dat->pdev->dev, "%s: ONFI sdr_timing_modes 0x%x\n", __func__,
           le16_to_cpu(nand->parameters.onfi->sdr_timing_modes));

  /* find the max timing mode supported by the device and below
   * AL_NAND_MAX_ONFI_TIMING_MODE */
  for (i = AL_NAND_MAX_ONFI_TIMING_MODE; i >= 0; i--) {
    if (!(BIT(i) & le16_to_cpu(nand->parameters.onfi->sdr_timing_modes)))
      continue;

    dev_info(&nand_dat->pdev->dev, "%s: chosen manual timing mode index %d\n",
             __func__, i);
    /*
     * Addressing RMN: 2903
     */
    device_properties->timingMode = AL_NAND_DEVICE_TIMING_MODE_MANUAL;

    memcpy(&device_properties->timing, &al_nand_manual_timing[i],
           sizeof(struct al_nand_device_timing));

    break;
  }

  if (i < 0) {
    dev_warn(&nand_dat->pdev->dev,
             "%s: no supported ONFI SDR timing mode <= %d, keeping PBS timing mode %d\n",
             __func__, AL_NAND_MAX_ONFI_TIMING_MODE,
             device_properties->timingMode);
    return;
  }

  dev_info(&nand_dat->pdev->dev, "%s: timing mode %d\n", __func__,
           device_properties->timingMode);
  dev_dbg(&nand_dat->pdev->dev,
          "%s: debug: selected_timing_index=%d setup=%u hold=%u wrp=%u rr=%u wb=%u wh=%u intcmd=%u readDelay=%u\n",
          __func__, i, device_properties->timing.tSETUP,
          device_properties->timing.tHOLD, device_properties->timing.tWRP,
          device_properties->timing.tRR, device_properties->timing.tWB,
          device_properties->timing.tWH, device_properties->timing.tINTCMD,
          device_properties->timing.readDelay);

  nand_set_timing_mode(nand_get_controller_data(nand), i);
}

static void nand_ecc_config(struct nand_chip *nand, uint32_t oob_size,
                            uint32_t hw_ecc_enabled, uint32_t ecc_loc) {
  struct mtd_info *mtd = nand_to_mtd(nand);
  struct nand_data *nand_dat = nand_data_get(mtd);
  struct nand_ecc_ctrl *ecc = &nand->ecc;
  int ecc_steps;
  int ecc_bytes;
  int ecc_strength;

  dev_info(&nand_dat->pdev->dev,
           "%s: hw_ecc_enabled %d oob_size %d ecc_loc %d\n", __func__,
           hw_ecc_enabled, oob_size, ecc_loc);

#ifdef AL_NAND_ECC_SUPPORT
  if (hw_ecc_enabled != 0) {
    nand_dat->s_ecc_loc = ecc_loc;
    nand_dat->s_oob_size = oob_size;
    nand_dat->s_ecc_oob_bytes = oob_size - ecc_loc;

    ecc_steps = nand_dat->dev_ext_props.pageSize / nand_dat->cw_size;
    if (nand_dat->ecc_config.algorithm == AL_NAND_ECC_ALGORITHM_HAMMING) {
      ecc_bytes = 4;
      ecc_strength = 1;
      nand_dat->s_ecc_oob_bytes = ecc_steps * ecc_bytes;
    } else if (nand_dat->compact_ecc_oob) {
      ecc_strength = 4 * (1 + nand_dat->ecc_config.num_corr_bits);
      ecc_bytes = AL_ALIGN_UP(DIV_ROUND_UP(15 * ecc_strength, 8), 4);
      nand_dat->s_ecc_oob_bytes = ecc_steps * ecc_bytes;
    } else {
      ecc_bytes = nand_dat->s_ecc_oob_bytes / ecc_steps;
      ecc_strength = 4 * (1 + nand_dat->ecc_config.num_corr_bits);
    }

    ecc->engine_type = NAND_ECC_ENGINE_TYPE_ON_HOST;
    ecc->size = nand_dat->cw_size; // message size
    ecc->bytes = ecc_bytes;
    ecc->strength = ecc_strength;

    ecc->read_page = ecc_read_page;
    ecc->read_subpage = ecc_read_subpage;
    ecc->write_page = ecc_write_page;

    dev_dbg(&nand_dat->pdev->dev,
            "%s: debug: alg=%d corr_bits=%d msg=%d page=%u cw_size=%u steps=%d ecc_bytes=%d strength=%d ecc_loc=%u spare_read_bytes=%u\n",
            __func__, nand_dat->ecc_config.algorithm,
            nand_dat->ecc_config.num_corr_bits,
            nand_dat->ecc_config.messageSize, nand_dat->dev_ext_props.pageSize,
            nand_dat->cw_size, ecc_steps, ecc->bytes, ecc->strength,
            nand_dat->s_ecc_loc, nand_dat->s_ecc_oob_bytes);

    mtd_set_ooblayout(mtd, nand_get_large_page_ooblayout());
  } else {
    ecc->engine_type = NAND_ECC_ENGINE_TYPE_NONE;
  }
#else
  memset(layout, 0, sizeof(struct nand_ecclayout));
  layout->eccbytes = oob_size - ecc_loc;
  layout->oobfree[0].offset = 2;
  layout->oobfree[0].length = ecc_loc - 2;
  layout->eccpos[0] = ecc_loc;

  ecc->layout = layout;

  ecc->mode = NAND_ECC_NONE;
#endif
}

static int al_nand_attach_chip(struct nand_chip *nand) {
  struct mtd_info *mtd = nand_to_mtd(nand);
  struct nand_data *nand_dat = nand_data_get(mtd);
  uint32_t ecc_loc;
  int ret;

  BUG_ON(mtd->oobsize > AL_NAND_MAX_OOB_SIZE);

  ret = nand_sync_detected_geometry(nand);
  if (ret)
    return ret;

  nand_onfi_config_set(nand, &nand_dat->device_properties,
                       &nand_dat->ecc_config);

  /* Offset in OOB where ECC starts */
  ecc_loc =
      nand_dat->ecc_config.spareAreaOffset - nand_dat->dev_ext_props.pageSize;
  nand_ecc_config(nand, mtd->oobsize, nand_dat->dev_ext_props.eccIsEnabled,
                  ecc_loc);

  ret = al_nand_dev_config(&nand_dat->nand_obj, &nand_dat->device_properties,
                           &nand_dat->ecc_config);
  if (ret) {
    dev_err(&nand_dat->pdev->dev, "al_nand_dev_config failed with %d\n", ret);
    return -EIO;
  }

  return 0;
}

static const struct nand_controller_ops al_nand_controller_ops = {
    .attach_chip = al_nand_attach_chip,
};

static int al_nand_probe(struct platform_device *pdev) {
  struct mtd_info *mtd;
  struct nand_chip *nand;
  struct nand_data *nand_dat;
  int ret = 0;
  void __iomem *nand_base;
  void __iomem *pbs_base;

  nand_dat = kzalloc(sizeof(struct nand_data), GFP_KERNEL);
  if (nand_dat == NULL) {
    pr_err("Failed to allocate nand_data!\n");
    return -1;
  }

  pr_info("%s: AnnapurnaLabs nand driver\n", __func__);

  nand = &nand_dat->chip;
  mtd = nand_to_mtd(nand);
  //	mtd->priv = nand;

  nand_dat->cache_pos = -1;
  nand_controller_init(&nand_dat->controller);
  nand->controller = &nand_dat->controller;
  nand->controller->ops = &al_nand_controller_ops;
  nand_set_controller_data(nand, nand_dat);
  nand_dat->pdev = pdev;
  nand_dat->max_chips = al_nand_max_chips_get(pdev);
  mtd->dev.parent = &pdev->dev;
  nand_set_flash_node(nand, pdev->dev.of_node);

  dev_set_drvdata(&pdev->dev, nand_dat);

  mtd->name = kasprintf(GFP_KERNEL, AL_NAND_NAME);
  if (!mtd->name) {
    pr_err("%s: error allocating name\n", __func__);
    kfree(nand_dat);
    return -ENOMEM;
  }

  ret = nand_resources_get_and_map(pdev, &nand_base, &pbs_base);
  if (ret != 0) {
    pr_err("%s: nand_resources_get_and_map failed\n", __func__);
    goto err;
  }

  ret = al_nand_init(&nand_dat->nand_obj, nand_base, NULL, 0);
  if (ret != 0) {
    pr_err("nand init failed\n");
    goto err;
  }

  if (0 != al_nand_dev_config_basic(&nand_dat->nand_obj)) {
    pr_err("dev_config_basic failed\n");
    ret = -EIO;
    goto err;
  }

  nand_dat->irq = platform_get_irq(pdev, 0);
  if (nand_dat->irq < 0) {
    pr_err("%s: no irq defined\n", __func__);
    return -ENXIO;
  }
  nand_interrupt_init(nand_dat);

  nand->options = NAND_NO_SUBPAGE_WRITE;

  nand->legacy.cmd_ctrl = nand_cmd_ctrl;
  nand->legacy.read_byte = nand_read_byte_from_fifo;
  nand->legacy.read_buf = nand_read_buff;
  nand->legacy.dev_ready = nand_dev_ready;
  nand->legacy.write_buf = nand_write_buff;
  nand->legacy.select_chip = nand_dev_select;

  ret = al_nand_properties_decode(pbs_base, &nand_dat->device_properties,
                                  &nand_dat->ecc_config,
                                  &nand_dat->dev_ext_props);
  if (ret) {
    pr_err("%s: nand_properties_decode failed with %d\n", __func__, ret);
    ret = -EIO;
    goto err;
  }

  dev_info(&nand_dat->pdev->dev,
           "device_properties: num_col_cyc %d num_row_cyc %d pageSize %d\n",
           nand_dat->device_properties.num_col_cyc,
           nand_dat->device_properties.num_row_cyc,
           nand_dat->device_properties.pageSize);
  dev_info(
      &nand_dat->pdev->dev,
      "dev_ext_props: pageSize %d blockSize %d wordSize %d eccIsEnabled %d\n",
      nand_dat->dev_ext_props.pageSize, nand_dat->dev_ext_props.blockSize,
      nand_dat->dev_ext_props.wordSize, nand_dat->dev_ext_props.eccIsEnabled);
  dev_info(&nand_dat->pdev->dev,
           "ecc_config: algorithm %d num_corr_bits %d messageSize %d "
           "spareAreaOffset %d\n",
           nand_dat->ecc_config.algorithm, nand_dat->ecc_config.num_corr_bits,
           nand_dat->ecc_config.messageSize,
           nand_dat->ecc_config.spareAreaOffset);

  ret = al_nand_dev_config(&nand_dat->nand_obj, &nand_dat->device_properties,
                           &nand_dat->ecc_config);
  if (ret) {
    dev_err(&nand_dat->pdev->dev,
            "%s: al_nand_dev_config from PBS failed with %d\n", __func__, ret);
    ret = -EIO;
    goto err;
  }

  /* must be set before scan_ident cause it uses read_buff */
  nand_dat->cw_size = 512 << nand_dat->ecc_config.messageSize;

  ret = nand_scan(nand, nand_dat->max_chips);
  if (ret) {
    pr_err("%s: nand_scan failed\n", __func__);
    goto err;
  }

  dev_info(&nand_dat->pdev->dev, "oobavail %d\n", mtd->oobavail);

  mtd_device_parse_register(mtd, probes, NULL, NULL, 0);

  return 0;

err:
  kfree(nand_dat->mtd.name);
  kfree(nand_dat);
  return ret;
}

static void al_nand_remove(struct platform_device *pdev) {
  struct nand_data *nand_dat = dev_get_drvdata(&pdev->dev);
  int ret;

  dev_dbg(&nand_dat->pdev->dev, "%s: nand driver removed\n", __func__);

  ret = mtd_device_unregister(&nand_dat->mtd);
  WARN_ON(ret);
  nand_cleanup(&nand_dat->chip);

  kfree(nand_dat->mtd.name);
  kfree(nand_dat);
}

static const struct of_device_id al_nand_match[] = {
    {
        .compatible = "annapurna-labs,al-nand",
    },
    {}};

static struct platform_driver al_nand_driver = {
    .driver =
        {
            .name = "annapurna-labs,al-nand",
            .owner = THIS_MODULE,
            .of_match_table = al_nand_match,
        },
    .probe = al_nand_probe,
    .remove = al_nand_remove,
};

static int __init nand_init(void) {
  return platform_driver_register(&al_nand_driver);
}

static void __exit nand_exit(void) {
  platform_driver_unregister(&al_nand_driver);
}

module_init(nand_init);
module_exit(nand_exit);
