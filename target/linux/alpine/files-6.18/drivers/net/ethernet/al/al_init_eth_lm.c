/*******************************************************************************
Copyright (C) 2014 Annapurna Labs Ltd.

This file may be licensed under the terms of the Annapurna Labs Commercial
License Agreement.

Alternatively, this file can be distributed under the terms of the GNU General
Public License V2 as published by the Free Software Foundation and can be
found at http://www.gnu.org/licenses/gpl-2.0.html

Alternatively, redistribution and use in source and binary forms, with or
without modification, are permitted provided that the following conditions are
met:

    *     Redistributions of source code must retain the above copyright notice,
          this list of conditions and the following disclaimer.

    *     Redistributions in binary form must reproduce the above copyright
          notice, this list of conditions and the following disclaimer in
          the documentation and/or other materials provided with the
          distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

/**
 *  @{
 * @file   al_init_eth_lm.c
 *
 * @brief ethernet link management common utilities
 *
 */

#include "al_init_eth_lm.h"
#include "al_hal_eth.h"
#include "al_init_eth_kr.h"
#include <mach/al_hal_serdes.h>
#include <mach/al_hal_serdes_internal_regs.h>

/* delay before checking link status with new serdes parameters (uSec) */
#define AL_ETH_LM_LINK_STATUS_DELAY 1000
/* delay before checking link status after reconfiguring the retimer (mSec) */
#define AL_ETH_LM_RETIMER_LINK_STATUS_DELAY 50

#define AL_ETH_LM_EQ_ITERATIONS 15
#define AL_ETH_LM_MAX_DCGAIN 8

/* num of link training failures till serdes reset */
#define AL_ETH_LT_FAILURES_TO_RESET (10)

#define SFP_I2C_HEADER_10G_IDX 3
#define SFP_I2C_HEADER_10G_DA_IDX 8
#define SFP_I2C_HEADER_10G_DA_LEN_IDX 18
#define SFP_I2C_HEADER_1G_IDX 6

#define RETIMER_I2C_BOOST_CH_A_ADDR 0xf
#define RETIMER_I2C_BOOST_CH_B_ADDR 0x16

#define SFP_10G_DA_ACTIVE 0x8
#define SFP_10G_DA_PASSIVE 0x4

#define lm_debug(...)                                                          \
  do {                                                                         \
    if (lm_context->debug)                                                     \
      al_warn(__VA_ARGS__);                                                    \
    else                                                                       \
      al_dbg(__VA_ARGS__);                                                     \
  } while (0)

static int al_eth_sfp_detect(struct al_eth_lm_context *lm_context,
                             enum al_eth_lm_link_mode *new_mode) {
  int rc = 0;
  uint8_t sfp_10g;
  uint8_t sfp_1g;
  uint8_t sfp_cable_tech;
  uint8_t sfp_da_len;

  do {
    rc = lm_context->i2c_read(lm_context->i2c_context, lm_context->sfp_bus_id,
                              lm_context->sfp_i2c_addr, SFP_I2C_HEADER_10G_IDX,
                              &sfp_10g);
    if (rc)
      break;

    rc = lm_context->i2c_read(lm_context->i2c_context, lm_context->sfp_bus_id,
                              lm_context->sfp_i2c_addr, SFP_I2C_HEADER_1G_IDX,
                              &sfp_1g);
    if (rc)
      break;

    rc = lm_context->i2c_read(lm_context->i2c_context, lm_context->sfp_bus_id,
                              lm_context->sfp_i2c_addr,
                              SFP_I2C_HEADER_10G_DA_IDX, &sfp_cable_tech);
    if (rc)
      break;

    rc = lm_context->i2c_read(lm_context->i2c_context, lm_context->sfp_bus_id,
                              lm_context->sfp_i2c_addr,
                              SFP_I2C_HEADER_10G_DA_LEN_IDX, &sfp_da_len);
  } while (0);

  if (rc) {
    if (rc == -ETIMEDOUT) {
      /* ETIMEDOUT is returned when no SFP is connected */
      lm_debug("%s: SFP Disconnected\n", __func__);
      *new_mode = AL_ETH_LM_MODE_DISCONNECTED;
    } else {
      return rc;
    }
  } else if (sfp_cable_tech & SFP_10G_DA_PASSIVE) {
    lm_debug("%s: 10G passive DAC (%d M) detected\n", __func__, sfp_da_len);
    *new_mode = AL_ETH_LM_MODE_10G_DA;
    lm_context->da_len = sfp_da_len;
  } else if (sfp_cable_tech & SFP_10G_DA_ACTIVE) {
    lm_debug("%s: 10G DAC active (%d M) detected\n", __func__, sfp_da_len);
    *new_mode = AL_ETH_LM_MODE_10G_DA;
    /* for active direct attached need to use len 0 in the retimer configuration
     */
    lm_context->da_len = 0;
  } else if (sfp_10g != 0) {
    lm_debug("%s: 10 SFP detected\n", __func__);
    *new_mode = AL_ETH_LM_MODE_10G_OPTIC;
  } else if (sfp_1g != 0) {
    lm_debug("%s: 1G SFP detected\n", __func__);
    *new_mode = AL_ETH_LM_MODE_1G;
  } else {
    al_warn("%s: unknown SFP inserted. eeprom content: 10G compliance 0x%x,"
            " 1G compliance 0x%x, sfp+cable 0x%x. default to %s\n",
            __func__, sfp_10g, sfp_1g, sfp_cable_tech,
            al_eth_lm_mode_convert_to_str(lm_context->default_mode));
    *new_mode = lm_context->default_mode;
    lm_context->da_len = lm_context->default_dac_len;
  }

  lm_context->mode = *new_mode;

  return 0;
}

static struct al_serdes_adv_tx_params da_tx_params = {
    .override = AL_TRUE,
    .amp = 0x1,
    .total_driver_units = 0x13,
    .c_plus_1 = 0x2,
    .c_plus_2 = 0,
    .c_minus_1 = 0x2,
    .slew_rate = 0,
};

static struct al_serdes_adv_rx_params da_rx_params = {
    .override = AL_TRUE,
    .dcgain = 0x4,
    .dfe_3db_freq = 0x4,
    .dfe_gain = 0x3,
    .dfe_first_tap_ctrl = 0x5,
    .dfe_secound_tap_ctrl = 0x1,
    .dfe_third_tap_ctrl = 0x8,
    .dfe_fourth_tap_ctrl = 0x1,
    .low_freq_agc_gain = 0x7,
    .precal_code_sel = 0,
    .high_freq_agc_boost = 0x1d,
};

static struct al_serdes_adv_tx_params optic_tx_params = {
    .override = AL_TRUE,
    .amp = 0x1,
    .total_driver_units = 0x13,
    .c_plus_1 = 0x2,
    .c_plus_2 = 0,
    .c_minus_1 = 0,
    .slew_rate = 0,
};

static struct al_serdes_adv_rx_params optic_rx_params = {
    .override = AL_TRUE,
    .dcgain = 0x0,
    .dfe_3db_freq = 0x7,
    .dfe_gain = 0x0,
    .dfe_first_tap_ctrl = 0x0,
    .dfe_secound_tap_ctrl = 0x8,
    .dfe_third_tap_ctrl = 0x0,
    .dfe_fourth_tap_ctrl = 0x8,
    .low_freq_agc_gain = 0x7,
    .precal_code_sel = 0,
    .high_freq_agc_boost = 0x4,
};

static void
al_eth_serdes_static_tx_params_set(struct al_eth_lm_context *lm_context) {
  if (lm_context->tx_param_dirty == 0)
    return;

  if (lm_context->serdes_tx_params_valid) {
    lm_context->tx_param_dirty = 0;

    lm_context->tx_params_override.override = AL_TRUE;

    al_serdes_tx_advanced_params_set(lm_context->serdes_obj, lm_context->grp,
                                     lm_context->lane,
                                     &lm_context->tx_params_override);

  } else if (lm_context->static_values) {
    lm_context->tx_param_dirty = 0;

    if ((!lm_context->retimer_exist) &&
        (lm_context->mode == AL_ETH_LM_MODE_10G_DA))
      al_serdes_tx_advanced_params_set(lm_context->serdes_obj, lm_context->grp,
                                       lm_context->lane, &da_tx_params);
    else
      al_serdes_tx_advanced_params_set(lm_context->serdes_obj, lm_context->grp,
                                       lm_context->lane, &optic_tx_params);
  }
}

static void
al_eth_serdes_static_rx_params_set(struct al_eth_lm_context *lm_context) {
  if (lm_context->rx_param_dirty == 0)
    return;

  if (lm_context->serdes_rx_params_valid) {
    lm_context->rx_param_dirty = 0;

    lm_context->rx_params_override.override = AL_TRUE;

    al_serdes_rx_advanced_params_set(lm_context->serdes_obj, lm_context->grp,
                                     lm_context->lane,
                                     &lm_context->rx_params_override);

  } else if (lm_context->static_values) {
    lm_context->rx_param_dirty = 0;

    if ((!lm_context->retimer_exist) &&
        (lm_context->mode == AL_ETH_LM_MODE_10G_DA))
      al_serdes_rx_advanced_params_set(lm_context->serdes_obj, lm_context->grp,
                                       lm_context->lane, &da_rx_params);
    else
      al_serdes_rx_advanced_params_set(lm_context->serdes_obj, lm_context->grp,
                                       lm_context->lane, &optic_rx_params);
  }
}

static int al_eth_rx_equal_run(struct al_eth_lm_context *lm_context) {
  struct al_serdes_adv_rx_params rx_params;
  int dcgain;
  int best_dcgain = -1;
  int i;
  uint8_t reg;
  int best_score = -1;
  int test_score = -1;

  rx_params.override = AL_FALSE;
  al_serdes_rx_advanced_params_set(lm_context->serdes_obj, lm_context->grp,
                                   lm_context->lane, &rx_params);

  lm_debug("score | dcgain | dfe3db | dfegain | tap1 | tap2 | tap3 | tap4 | "
           "low freq | high freq\n");

  for (dcgain = 0; dcgain < AL_ETH_LM_MAX_DCGAIN; dcgain++) {
    al_serdes_reg_read(lm_context->serdes_obj, lm_context->grp,
                       AL_SRDS_REG_PAGE_4_COMMON, AL_SRDS_REG_TYPE_PMA,
                       SERDES_IREG_FLD_RXEQ_DCGAIN_LUP0_REG_NUM, &reg);

    AL_REG_FIELD_SET(reg, SERDES_IREG_FLD_RXEQ_DCGAIN_LUP0_MASK,
                     SERDES_IREG_FLD_RXEQ_DCGAIN_LUP0_SHIFT, dcgain);

    al_serdes_reg_write(lm_context->serdes_obj, lm_context->grp,
                        AL_SRDS_REG_PAGE_4_COMMON, AL_SRDS_REG_TYPE_PMA,
                        SERDES_IREG_FLD_RXEQ_DCGAIN_LUP0_REG_NUM, reg);

    test_score = al_serdes_rx_equalization(lm_context->serdes_obj,
                                           lm_context->grp, lm_context->lane);

    if (test_score < 0) {
      al_warn("serdes rx equalization failed on error\n");
      return test_score;
    }

    if (test_score > best_score) {
      best_score = test_score;
      best_dcgain = dcgain;
    }

    al_serdes_rx_advanced_params_get(lm_context->serdes_obj, lm_context->grp,
                                     lm_context->lane, &rx_params);

    lm_debug("%6d|%8x|%8x|%9x|%6x|%6x|%6x|%6x|%10x|%10x|\n", test_score,
             rx_params.dcgain, rx_params.dfe_3db_freq, rx_params.dfe_gain,
             rx_params.dfe_first_tap_ctrl, rx_params.dfe_secound_tap_ctrl,
             rx_params.dfe_third_tap_ctrl, rx_params.dfe_fourth_tap_ctrl,
             rx_params.low_freq_agc_gain, rx_params.high_freq_agc_boost);
  }

  al_serdes_reg_read(lm_context->serdes_obj, lm_context->grp,
                     AL_SRDS_REG_PAGE_4_COMMON, AL_SRDS_REG_TYPE_PMA,
                     SERDES_IREG_FLD_RXEQ_DCGAIN_LUP0_REG_NUM, &reg);

  AL_REG_FIELD_SET(reg, SERDES_IREG_FLD_RXEQ_DCGAIN_LUP0_MASK,
                   SERDES_IREG_FLD_RXEQ_DCGAIN_LUP0_SHIFT, best_dcgain);

  al_serdes_reg_write(lm_context->serdes_obj, lm_context->grp,
                      AL_SRDS_REG_PAGE_4_COMMON, AL_SRDS_REG_TYPE_PMA,
                      SERDES_IREG_FLD_RXEQ_DCGAIN_LUP0_REG_NUM, reg);

  best_score = -1;
  for (i = 0; i < AL_ETH_LM_EQ_ITERATIONS; i++) {
    test_score = al_serdes_rx_equalization(lm_context->serdes_obj,
                                           lm_context->grp, lm_context->lane);

    if (test_score < 0) {
      al_warn("serdes rx equalization failed on error\n");
      return test_score;
    }

    if (test_score > best_score) {
      best_score = test_score;
      al_serdes_rx_advanced_params_get(lm_context->serdes_obj, lm_context->grp,
                                       lm_context->lane, &rx_params);
    }
  }

  rx_params.precal_code_sel = 0;
  rx_params.override = AL_TRUE;
  al_serdes_rx_advanced_params_set(lm_context->serdes_obj, lm_context->grp,
                                   lm_context->lane, &rx_params);

  lm_debug("-------------------- best dcgain %d "
           "------------------------------------\n",
           best_dcgain);
  lm_debug("%6d|%8x|%8x|%9x|%6x|%6x|%6x|%6x|%10x|%10x|\n", best_score,
           rx_params.dcgain, rx_params.dfe_3db_freq, rx_params.dfe_gain,
           rx_params.dfe_first_tap_ctrl, rx_params.dfe_secound_tap_ctrl,
           rx_params.dfe_third_tap_ctrl, rx_params.dfe_fourth_tap_ctrl,
           rx_params.low_freq_agc_gain, rx_params.high_freq_agc_boost);

  return 0;
}

static int al_eth_lm_retimer_config(struct al_eth_lm_context *lm_context) {
  int rc = 0;
  uint8_t boost = 0;

  if ((lm_context->mode != AL_ETH_LM_MODE_10G_DA) || (lm_context->da_len == 0))
    boost = 0; /* ~5dB Loss */
  else if (lm_context->da_len <= 1)
    boost = 0x1; /* ~5dB Loss */
  else if (lm_context->da_len <= 2)
    boost = 0x2; /* ~9dB Loss */
  else if (lm_context->da_len <= 3)
    boost = 0x3; /* ~12dB Loss */
  else if (lm_context->da_len <= 5)
    boost = 0x7; /* ~16dB Loss */
  else
    boost = 0xb; /* ~19dB Loss */

  lm_debug("config retimer boost in channel %d to 0x%x\n",
           lm_context->retimer_channel, boost);

  if (lm_context->retimer_channel == AL_ETH_RETIMER_CHANNEL_A)
    rc = lm_context->i2c_write(
        lm_context->i2c_context, lm_context->retimer_bus_id,
        lm_context->retimer_i2c_addr, RETIMER_I2C_BOOST_CH_A_ADDR, boost);
  else
    rc = lm_context->i2c_write(
        lm_context->i2c_context, lm_context->retimer_bus_id,
        lm_context->retimer_i2c_addr, RETIMER_I2C_BOOST_CH_B_ADDR, boost);

  if (rc) {
    al_err("%s: Error occurred (%d) while writing retimer configuration "
           "(bus-id %x i2c-addr %x)\n",
           __func__, rc, lm_context->retimer_bus_id,
           lm_context->retimer_i2c_addr);
    return rc;
  }

  return 0;
}

/*****************************************************************************/
/***************************** API functions *********************************/
/*****************************************************************************/
int al_eth_lm_init(struct al_eth_lm_context *lm_context,
                   struct al_eth_lm_init_params *params) {
  lm_context->adapter = params->adapter;
  lm_context->serdes_obj = params->serdes_obj;
  lm_context->grp = params->grp;
  lm_context->lane = params->lane;
  lm_context->sfp_detection = params->sfp_detection;
  lm_context->sfp_bus_id = params->sfp_bus_id;
  lm_context->sfp_i2c_addr = params->sfp_i2c_addr;

  lm_context->retimer_exist = params->retimer_exist;
  lm_context->retimer_bus_id = params->retimer_bus_id;
  lm_context->retimer_i2c_addr = params->retimer_i2c_addr;
  lm_context->retimer_channel = params->retimer_channel;

  lm_context->default_mode = params->default_mode;
  lm_context->default_dac_len = params->default_dac_len;
  lm_context->link_training = params->link_training;
  lm_context->rx_equal = params->rx_equal;
  lm_context->static_values = params->static_values;
  lm_context->i2c_read = params->i2c_read;
  lm_context->i2c_write = params->i2c_write;
  lm_context->i2c_context = params->i2c_context;
  lm_context->get_random_byte = params->get_random_byte;

  /* eeprom_read must be provided if sfp_detection is true */
  al_assert((lm_context->sfp_detection == AL_FALSE) ||
            (lm_context->i2c_read != NULL));

  al_assert((lm_context->retimer_exist == AL_FALSE) ||
            (lm_context->i2c_write != NULL));

  lm_context->local_adv.selector_field = 1;
  lm_context->local_adv.capability = 0;
  lm_context->local_adv.remote_fault = 0;
  lm_context->local_adv.acknowledge = 0;
  lm_context->local_adv.next_page = 0;
  lm_context->local_adv.technology = AL_ETH_AN_TECH_10GBASE_KR;
  lm_context->local_adv.fec_capability = params->kr_fec_enable;

  lm_context->mode = AL_ETH_LM_MODE_DISCONNECTED;
  lm_context->serdes_tx_params_valid = AL_FALSE;
  lm_context->serdes_rx_params_valid = AL_FALSE;

  lm_context->last_link_status.link_up = AL_FALSE;

  lm_context->rx_param_dirty = 1;
  lm_context->tx_param_dirty = 1;

  return 0;
}

int al_eth_lm_link_detection(struct al_eth_lm_context *lm_context,
                             al_bool *link_fault,
                             enum al_eth_lm_link_mode *old_mode,
                             enum al_eth_lm_link_mode *new_mode) {
  int err;
  struct al_eth_link_status status;

  al_assert(lm_context != NULL);
  al_assert(old_mode != NULL);
  al_assert(new_mode != NULL);

  al_eth_link_status_get(lm_context->adapter, &status);

  *old_mode = lm_context->mode;
  *new_mode = lm_context->mode;

  if (status.link_up == AL_FALSE)
    al_eth_led_set(lm_context->adapter, AL_FALSE);

  if (link_fault) {
    *link_fault = AL_FALSE;

    /* link status lost */
    if ((lm_context->last_link_status.link_up == AL_TRUE) &&
        (status.link_up == AL_TRUE))
      return 0;

    lm_context->last_link_status = status;
    *link_fault = AL_TRUE;
  }

  if (lm_context->sfp_detection) {
    err = al_eth_sfp_detect(lm_context, new_mode);
    if (err) {
      al_err("sfp_detection failed!\n");
      return err;
    }

    lm_context->mode = *new_mode;
  } else {
    lm_context->mode = lm_context->default_mode;
    *new_mode = lm_context->mode;
  }

  if (*old_mode != *new_mode) {
    al_info("%s: New SFP mode detected %s -> %s\n", __func__,
            al_eth_lm_mode_convert_to_str(*old_mode),
            al_eth_lm_mode_convert_to_str(*new_mode));

    lm_context->rx_param_dirty = 1;
    lm_context->tx_param_dirty = 1;
  }

  return 0;
}

int al_eth_lm_link_establish(struct al_eth_lm_context *lm_context,
                             al_bool *link_up) {
  al_bool signal_detected;
  int ret = 0;

  signal_detected = al_serdes_signal_is_detected(
      lm_context->serdes_obj, lm_context->grp, lm_context->lane);

  if (signal_detected == AL_FALSE) {
    /* if no signal detected there is nothing to do */
    *link_up = AL_FALSE;
    return 0;
  }

  if (lm_context->retimer_exist) {
    al_eth_serdes_static_rx_params_set(lm_context);
    al_eth_serdes_static_tx_params_set(lm_context);
    al_eth_lm_retimer_config(lm_context);
    al_msleep(AL_ETH_LM_RETIMER_LINK_STATUS_DELAY);

    al_eth_link_status_get(lm_context->adapter, &lm_context->last_link_status);

    if (lm_context->last_link_status.link_up == AL_TRUE) {
      al_info("%s: link up with retimer\n", __func__);
      goto link_is_up;
    }

    *link_up = AL_FALSE;
    return -1;
  }

  if ((lm_context->mode == AL_ETH_LM_MODE_10G_DA) &&
      (lm_context->link_training)) {
    lm_context->local_adv.transmitted_nonce = lm_context->get_random_byte();
    lm_context->local_adv.transmitted_nonce &= 0x1f;

    ret = al_eth_an_lt_execute(
        lm_context->adapter, lm_context->serdes_obj, lm_context->grp,
        lm_context->lane, &lm_context->local_adv, &lm_context->partner_adv);

    lm_context->rx_param_dirty = 1;
    lm_context->tx_param_dirty = 1;

    if (ret == 0) {
      al_info("%s: link training finished successfully\n", __func__);
      lm_context->last_link_status.link_up = AL_TRUE;
      lm_context->link_training_failures = 0;
      goto link_is_up;
    }

    lm_context->link_training_failures++;
    if (lm_context->link_training_failures > AL_ETH_LT_FAILURES_TO_RESET) {
      lm_debug("%s: failed to establish LT %d times. reset serdes\n", __func__,
               AL_ETH_LT_FAILURES_TO_RESET);

      al_serdes_pma_hard_reset_lane(lm_context->serdes_obj, lm_context->grp,
                                    lm_context->lane, AL_TRUE);
      al_serdes_pma_hard_reset_lane(lm_context->serdes_obj, lm_context->grp,
                                    lm_context->lane, AL_FALSE);
      lm_context->link_training_failures = 0;
    }
  }

  al_eth_serdes_static_tx_params_set(lm_context);

  if ((lm_context->mode == AL_ETH_LM_MODE_10G_DA) && (lm_context->rx_equal)) {
    ret = al_eth_rx_equal_run(lm_context);

    if (ret == 0) {
      al_udelay(AL_ETH_LM_LINK_STATUS_DELAY);
      al_eth_link_status_get(lm_context->adapter,
                             &lm_context->last_link_status);
      if (lm_context->last_link_status.link_up == AL_TRUE) {
        al_info("%s: rx_equal finished successfully\n", __func__);
        goto link_is_up;
      }
    }
  }

  al_eth_serdes_static_rx_params_set(lm_context);

  al_udelay(AL_ETH_LM_LINK_STATUS_DELAY);

  al_eth_link_status_get(lm_context->adapter, &lm_context->last_link_status);

  if (lm_context->last_link_status.link_up == AL_TRUE) {
    al_info("%s: link up with static parameters\n", __func__);
    goto link_is_up;
  }

  *link_up = AL_FALSE;
  return -1;

link_is_up:
  al_eth_led_set(lm_context->adapter, AL_TRUE);
  *link_up = AL_TRUE;
  return 0;
}

int al_eth_lm_static_parameters_override(
    struct al_eth_lm_context *lm_context,
    struct al_serdes_adv_tx_params *tx_params,
    struct al_serdes_adv_rx_params *rx_params) {
  if (tx_params) {
    lm_context->tx_params_override = *tx_params;
    lm_context->tx_param_dirty = 1;
    lm_context->serdes_tx_params_valid = AL_TRUE;
  }

  if (rx_params) {
    lm_context->rx_params_override = *rx_params;
    lm_context->rx_param_dirty = 1;
    lm_context->serdes_rx_params_valid = AL_TRUE;
  }

  return 0;
}

int al_eth_lm_static_parameters_override_disable(
    struct al_eth_lm_context *lm_context, al_bool tx_params,
    al_bool rx_params) {
  if (tx_params)
    lm_context->serdes_tx_params_valid = AL_FALSE;
  if (rx_params)
    lm_context->serdes_tx_params_valid = AL_FALSE;

  return 0;
}

int al_eth_lm_static_parameters_get(struct al_eth_lm_context *lm_context,
                                    struct al_serdes_adv_tx_params *tx_params,
                                    struct al_serdes_adv_rx_params *rx_params) {
  if (tx_params) {
    if (lm_context->serdes_tx_params_valid)
      *tx_params = lm_context->tx_params_override;
    else
      al_serdes_tx_advanced_params_get(lm_context->serdes_obj, lm_context->grp,
                                       lm_context->lane, tx_params);
  }

  if (rx_params) {
    if (lm_context->serdes_rx_params_valid)
      *rx_params = lm_context->rx_params_override;
    else
      al_serdes_rx_advanced_params_get(lm_context->serdes_obj, lm_context->grp,
                                       lm_context->lane, rx_params);
  }

  return 0;
}

const char *al_eth_lm_mode_convert_to_str(enum al_eth_lm_link_mode val) {
  switch (val) {
  case AL_ETH_LM_MODE_DISCONNECTED:
    return "AL_ETH_LM_MODE_DISCONNECTED";
  case AL_ETH_LM_MODE_10G_OPTIC:
    return "AL_ETH_LM_MODE_10G_OPTIC";
  case AL_ETH_LM_MODE_10G_DA:
    return "AL_ETH_LM_MODE_10G_DA";
  case AL_ETH_LM_MODE_1G:
    return "AL_ETH_LM_MODE_1G";
  }

  return "N/A";
}

void al_eth_lm_debug_mode_set(struct al_eth_lm_context *lm_context,
                              al_bool enable) {
  lm_context->debug = enable;
}
