/*
 * Copyright (c) 2016-2017, The Linux Foundation. All rights reserved.
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all copies.
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */


/**
 * @defgroup
 * @{
 */
#ifndef _HPPE_MIB_H_
#define _HPPE_MIB_H_

#define MAC_MIB_CTRL_MAX_ENTRY	6
#define RXBROAD_MAX_ENTRY	6
#define RXPAUSE_MAX_ENTRY	6
#define RXMULTI_MAX_ENTRY	6
#define RXFCSERR_MAX_ENTRY	6
#define RXALIGNERR_MAX_ENTRY	6
#define RXRUNT_MAX_ENTRY	6
#define RXFRAG_MAX_ENTRY	6
#define RXJUMBOFCSERR_MAX_ENTRY	6
#define RXJUMBOALIGNERR_MAX_ENTRY	6
#define RXPKT64_MAX_ENTRY	6
#define RXPKT65TO127_MAX_ENTRY	6
#define RXPKT128TO255_MAX_ENTRY	6
#define RXPKT256TO511_MAX_ENTRY	6
#define RXPKT512TO1023_MAX_ENTRY	6
#define RXPKT1024TO1518_MAX_ENTRY	6
#define RXPKT1519TOX_MAX_ENTRY	6
#define RXTOOLONG_MAX_ENTRY	6
#define RXGOODBYTE_L_MAX_ENTRY	6
#define RXGOODBYTE_H_MAX_ENTRY	6
#define RXBADBYTE_L_MAX_ENTRY	6
#define RXBADBYTE_H_MAX_ENTRY	6
#define RXUNI_MAX_ENTRY	6
#define TXBROAD_MAX_ENTRY	6
#define TXPAUSE_MAX_ENTRY	6
#define TXMULTI_MAX_ENTRY	6
#define TXUNDERRUN_MAX_ENTRY	6
#define TXPKT64_MAX_ENTRY	6
#define TXPKT65TO127_MAX_ENTRY	6
#define TXPKT128TO255_MAX_ENTRY	6
#define TXPKT256TO511_MAX_ENTRY	6
#define TXPKT512TO1023_MAX_ENTRY	6
#define TXPKT1024TO1518_MAX_ENTRY	6
#define TXPKT1519TOX_MAX_ENTRY	6
#define TXBYTE_L_MAX_ENTRY	6
#define TXBYTE_H_MAX_ENTRY	6
#define TXCOLLISIONS_MAX_ENTRY	6
#define TXABORTCOL_MAX_ENTRY	6
#define TXMULTICOL_MAX_ENTRY	6
#define TXSINGLECOL_MAX_ENTRY	6
#define TXEXCESSIVEDEFER_MAX_ENTRY	6
#define TXDEFER_MAX_ENTRY	6
#define TXLATECOL_MAX_ENTRY	6
#define TXUNI_MAX_ENTRY	6


sw_error_t
hppe_mac_mib_ctrl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_mib_ctrl_u *value);

sw_error_t
hppe_mac_mib_ctrl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_mib_ctrl_u *value);

sw_error_t
hppe_rxbroad_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxbroad_u *value);

sw_error_t
hppe_rxbroad_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxbroad_u *value);

sw_error_t
hppe_rxpause_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxpause_u *value);

sw_error_t
hppe_rxpause_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxpause_u *value);

sw_error_t
hppe_rxmulti_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxmulti_u *value);

sw_error_t
hppe_rxmulti_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxmulti_u *value);

sw_error_t
hppe_rxfcserr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxfcserr_u *value);

sw_error_t
hppe_rxfcserr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxfcserr_u *value);

sw_error_t
hppe_rxalignerr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxalignerr_u *value);

sw_error_t
hppe_rxalignerr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxalignerr_u *value);

sw_error_t
hppe_rxrunt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxrunt_u *value);

sw_error_t
hppe_rxrunt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxrunt_u *value);

sw_error_t
hppe_rxfrag_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxfrag_u *value);

sw_error_t
hppe_rxfrag_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxfrag_u *value);

sw_error_t
hppe_rxjumbofcserr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxjumbofcserr_u *value);

sw_error_t
hppe_rxjumbofcserr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxjumbofcserr_u *value);

sw_error_t
hppe_rxjumboalignerr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxjumboalignerr_u *value);

sw_error_t
hppe_rxjumboalignerr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxjumboalignerr_u *value);

sw_error_t
hppe_rxpkt64_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxpkt64_u *value);

sw_error_t
hppe_rxpkt64_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxpkt64_u *value);

sw_error_t
hppe_rxpkt65to127_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxpkt65to127_u *value);

sw_error_t
hppe_rxpkt65to127_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxpkt65to127_u *value);

sw_error_t
hppe_rxpkt128to255_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxpkt128to255_u *value);

sw_error_t
hppe_rxpkt128to255_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxpkt128to255_u *value);

sw_error_t
hppe_rxpkt256to511_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxpkt256to511_u *value);

sw_error_t
hppe_rxpkt256to511_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxpkt256to511_u *value);

sw_error_t
hppe_rxpkt512to1023_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxpkt512to1023_u *value);

sw_error_t
hppe_rxpkt512to1023_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxpkt512to1023_u *value);

sw_error_t
hppe_rxpkt1024to1518_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxpkt1024to1518_u *value);

sw_error_t
hppe_rxpkt1024to1518_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxpkt1024to1518_u *value);

sw_error_t
hppe_rxpkt1519tox_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxpkt1519tox_u *value);

sw_error_t
hppe_rxpkt1519tox_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxpkt1519tox_u *value);

sw_error_t
hppe_rxtoolong_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxtoolong_u *value);

sw_error_t
hppe_rxtoolong_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxtoolong_u *value);

sw_error_t
hppe_rxgoodbyte_l_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxgoodbyte_l_u *value);

sw_error_t
hppe_rxgoodbyte_l_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxgoodbyte_l_u *value);

sw_error_t
hppe_rxgoodbyte_h_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxgoodbyte_h_u *value);

sw_error_t
hppe_rxgoodbyte_h_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxgoodbyte_h_u *value);

sw_error_t
hppe_rxbadbyte_l_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxbadbyte_l_u *value);

sw_error_t
hppe_rxbadbyte_l_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxbadbyte_l_u *value);

sw_error_t
hppe_rxbadbyte_h_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxbadbyte_h_u *value);

sw_error_t
hppe_rxbadbyte_h_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxbadbyte_h_u *value);

sw_error_t
hppe_rxuni_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxuni_u *value);

sw_error_t
hppe_rxuni_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxuni_u *value);

sw_error_t
hppe_txbroad_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txbroad_u *value);

sw_error_t
hppe_txbroad_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txbroad_u *value);

sw_error_t
hppe_txpause_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txpause_u *value);

sw_error_t
hppe_txpause_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txpause_u *value);

sw_error_t
hppe_txmulti_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txmulti_u *value);

sw_error_t
hppe_txmulti_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txmulti_u *value);

sw_error_t
hppe_txunderrun_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txunderrun_u *value);

sw_error_t
hppe_txunderrun_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txunderrun_u *value);

sw_error_t
hppe_txpkt64_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txpkt64_u *value);

sw_error_t
hppe_txpkt64_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txpkt64_u *value);

sw_error_t
hppe_txpkt65to127_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txpkt65to127_u *value);

sw_error_t
hppe_txpkt65to127_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txpkt65to127_u *value);

sw_error_t
hppe_txpkt128to255_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txpkt128to255_u *value);

sw_error_t
hppe_txpkt128to255_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txpkt128to255_u *value);

sw_error_t
hppe_txpkt256to511_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txpkt256to511_u *value);

sw_error_t
hppe_txpkt256to511_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txpkt256to511_u *value);

sw_error_t
hppe_txpkt512to1023_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txpkt512to1023_u *value);

sw_error_t
hppe_txpkt512to1023_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txpkt512to1023_u *value);

sw_error_t
hppe_txpkt1024to1518_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txpkt1024to1518_u *value);

sw_error_t
hppe_txpkt1024to1518_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txpkt1024to1518_u *value);

sw_error_t
hppe_txpkt1519tox_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txpkt1519tox_u *value);

sw_error_t
hppe_txpkt1519tox_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txpkt1519tox_u *value);

sw_error_t
hppe_txbyte_l_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txbyte_l_u *value);

sw_error_t
hppe_txbyte_l_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txbyte_l_u *value);

sw_error_t
hppe_txbyte_h_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txbyte_h_u *value);

sw_error_t
hppe_txbyte_h_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txbyte_h_u *value);

sw_error_t
hppe_txcollisions_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txcollisions_u *value);

sw_error_t
hppe_txcollisions_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txcollisions_u *value);

sw_error_t
hppe_txabortcol_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txabortcol_u *value);

sw_error_t
hppe_txabortcol_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txabortcol_u *value);

sw_error_t
hppe_txmulticol_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txmulticol_u *value);

sw_error_t
hppe_txmulticol_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txmulticol_u *value);

sw_error_t
hppe_txsinglecol_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txsinglecol_u *value);

sw_error_t
hppe_txsinglecol_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txsinglecol_u *value);

sw_error_t
hppe_txexcessivedefer_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txexcessivedefer_u *value);

sw_error_t
hppe_txexcessivedefer_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txexcessivedefer_u *value);

sw_error_t
hppe_txdefer_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txdefer_u *value);

sw_error_t
hppe_txdefer_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txdefer_u *value);

sw_error_t
hppe_txlatecol_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txlatecol_u *value);

sw_error_t
hppe_txlatecol_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txlatecol_u *value);

sw_error_t
hppe_txuni_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txuni_u *value);

sw_error_t
hppe_txuni_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txuni_u *value);

sw_error_t
hppe_mac_mib_ctrl_mib_reset_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_mib_ctrl_mib_reset_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_mib_ctrl_mib_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_mib_ctrl_mib_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_mib_ctrl_mib_rd_clr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_mib_ctrl_mib_rd_clr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rxbroad_rxbroad_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rxbroad_rxbroad_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rxpause_rxpause_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rxpause_rxpause_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rxmulti_rxmulti_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rxmulti_rxmulti_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rxfcserr_rxfcserr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rxfcserr_rxfcserr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rxalignerr_rxalignerr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rxalignerr_rxalignerr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rxrunt_rxrunt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rxrunt_rxrunt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rxfrag_rxfrag_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rxfrag_rxfrag_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rxjumbofcserr_rxjumbofcserr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rxjumbofcserr_rxjumbofcserr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rxjumboalignerr_rxjumboalignerr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rxjumboalignerr_rxjumboalignerr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rxpkt64_rxpkt64_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rxpkt64_rxpkt64_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rxpkt65to127_rxpkt65to127_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rxpkt65to127_rxpkt65to127_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rxpkt128to255_rxpkt128to255_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rxpkt128to255_rxpkt128to255_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rxpkt256to511_rxpkt256to511_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rxpkt256to511_rxpkt256to511_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rxpkt512to1023_rxpkt512to1023_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rxpkt512to1023_rxpkt512to1023_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rxpkt1024to1518_rxpkt1024to1518_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rxpkt1024to1518_rxpkt1024to1518_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rxpkt1519tox_rxpkt1519tox_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rxpkt1519tox_rxpkt1519tox_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rxtoolong_rxtoolong_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rxtoolong_rxtoolong_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rxgoodbyte_l_rxgoodbyte_l_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rxgoodbyte_l_rxgoodbyte_l_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rxgoodbyte_h_rxgoodbyte_h_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rxgoodbyte_h_rxgoodbyte_h_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rxbadbyte_l_rxbadbyte_l_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rxbadbyte_l_rxbadbyte_l_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rxbadbyte_h_rxbadbyte_h_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rxbadbyte_h_rxbadbyte_h_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rxuni_rxuni_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rxuni_rxuni_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_txbroad_txbroad_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_txbroad_txbroad_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_txpause_txpause_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_txpause_txpause_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_txmulti_txmulti_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_txmulti_txmulti_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_txunderrun_txunderrun_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_txunderrun_txunderrun_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_txpkt64_txpkt64_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_txpkt64_txpkt64_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_txpkt65to127_txpkt65to127_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_txpkt65to127_txpkt65to127_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_txpkt128to255_txpkt128to255_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_txpkt128to255_txpkt128to255_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_txpkt256to511_txpkt256to511_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_txpkt256to511_txpkt256to511_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_txpkt512to1023_txpkt512to1023_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_txpkt512to1023_txpkt512to1023_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_txpkt1024to1518_txpkt1024to1518_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_txpkt1024to1518_txpkt1024to1518_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_txpkt1519tox_txpkt1519tox_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_txpkt1519tox_txpkt1519tox_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_txbyte_l_txbyte_l_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_txbyte_l_txbyte_l_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_txbyte_h_txbyte_h_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_txbyte_h_txbyte_h_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_txcollisions_txcollisions_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_txcollisions_txcollisions_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_txabortcol_txabortcol_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_txabortcol_txabortcol_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_txmulticol_txmulticol_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_txmulticol_txmulticol_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_txsinglecol_txsinglecol_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_txsinglecol_txsinglecol_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_txexcessivedefer_txexcessivedefer_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_txexcessivedefer_txexcessivedefer_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_txdefer_txdefer_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_txdefer_txdefer_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_txlatecol_txlatecol_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_txlatecol_txlatecol_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_txuni_txuni_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_txuni_txuni_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

#endif

