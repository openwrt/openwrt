/*
 * Copyright (c) 2016-2019, The Linux Foundation. All rights reserved.
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
#include "sw.h"
#include "hppe_mib_reg.h"
#include "hppe_mib.h"
#include "adpt.h"
#include "hppe_xgmacmib_reg.h"
#include "hppe_xgmacmib.h"

#include "hppe_init.h"
#include "adpt_hppe.h"
#ifdef CPPE
#include "adpt_cppe_mib.h"
#endif

sw_error_t
adpt_hppe_mib_cpukeep_get(a_uint32_t dev_id, a_bool_t *enable)
{
	sw_error_t rv = SW_OK;
	a_uint32_t port_id = 0, status = 0;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(enable);

	rv = hppe_mac_mib_ctrl_mib_rd_clr_get(dev_id, port_id, &status);

	if (status == A_TRUE)
	{
		*enable = A_FALSE;
	}
	else
	{
		*enable = A_TRUE;
	}

	if( rv != SW_OK )
	{
		return rv;
	}

	return SW_OK;
}

sw_error_t
adpt_ppe_mib_cpukeep_set(a_uint32_t dev_id, a_bool_t enable)
{
	a_uint32_t port_id = 0, g_port_id = 0;
	sw_error_t rv = SW_OK;

	ADPT_DEV_ID_CHECK(dev_id);

	for (port_id = SSDK_PHYSICAL_PORT1; port_id <= SSDK_PHYSICAL_PORT6; port_id++)
	{
#ifdef CPPE
		if (adpt_hppe_chip_revision_get(dev_id) == CPPE_REVISION &&
			port_id == SSDK_PHYSICAL_PORT6)
		{
			rv = adpt_cppe_lpbk_mib_cpukeep_set(dev_id, port_id, enable);
			SW_RTN_ON_ERROR(rv);
			continue;
		}
#endif
		g_port_id = HPPE_TO_GMAC_PORT_ID(port_id);
		hppe_mac_mib_ctrl_mib_rd_clr_set(dev_id, g_port_id, (a_uint32_t)(!enable));
	}

	return rv;
}

static sw_error_t
adpt_hppe_get_mib_info(a_uint32_t dev_id, fal_port_t port_id,
                     fal_mib_info_t * mib_info )
{
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(mib_info);
	memset(mib_info, 0, sizeof(fal_mib_info_t));

	port_id = HPPE_TO_GMAC_PORT_ID(port_id);
	hppe_rxbroad_get(dev_id, (a_uint32_t)port_id, (union rxbroad_u *)&mib_info->RxBroad);
	hppe_rxpause_get(dev_id, (a_uint32_t)port_id, (union rxpause_u *)&mib_info->RxPause);
	hppe_rxmulti_get(dev_id, (a_uint32_t)port_id, (union rxmulti_u *)&mib_info->RxMulti);
	hppe_rxfcserr_get(dev_id, (a_uint32_t)port_id, (union rxfcserr_u *)&mib_info->RxFcsErr);
	hppe_rxalignerr_get(dev_id, (a_uint32_t)port_id,
		(union rxalignerr_u *)&mib_info->RxAllignErr);
	hppe_rxrunt_get(dev_id, (a_uint32_t)port_id, (union rxrunt_u *)&mib_info->RxRunt);
	hppe_rxfrag_get(dev_id, (a_uint32_t)port_id, (union rxfrag_u *)&mib_info->RxFragment);
	hppe_rxjumbofcserr_get(dev_id, (a_uint32_t)port_id,
		(union rxjumbofcserr_u *)&mib_info->RxJumboFcsErr);
	hppe_rxjumboalignerr_get(dev_id, (a_uint32_t)port_id,
		(union rxjumboalignerr_u *)&mib_info->RxJumboAligenErr);
	hppe_rxpkt64_get(dev_id, (a_uint32_t)port_id, (union rxpkt64_u *)&mib_info->Rx64Byte);
	hppe_rxpkt65to127_get(dev_id, (a_uint32_t)port_id,
		(union rxpkt65to127_u *)&mib_info->Rx128Byte);
	hppe_rxpkt128to255_get(dev_id, (a_uint32_t)port_id,
		(union rxpkt128to255_u *)&mib_info->Rx256Byte);
	hppe_rxpkt256to511_get(dev_id, (a_uint32_t)port_id,
		(union rxpkt256to511_u *)&mib_info->Rx512Byte);
	hppe_rxpkt512to1023_get(dev_id, (a_uint32_t)port_id,
		(union rxpkt512to1023_u *)&mib_info->Rx1024Byte);
	hppe_rxpkt1024to1518_get(dev_id, (a_uint32_t)port_id,
		(union rxpkt1024to1518_u *)&mib_info->Rx1518Byte);
	hppe_rxpkt1519tox_get(dev_id, (a_uint32_t)port_id,
		(union rxpkt1519tox_u *)&mib_info->RxMaxByte);
	hppe_rxtoolong_get(dev_id, (a_uint32_t)port_id,
		(union rxtoolong_u *)&mib_info->RxTooLong);
	hppe_rxgoodbyte_l_get(dev_id, (a_uint32_t)port_id,
		(union rxgoodbyte_l_u *)&mib_info->RxGoodByte_lo);
	hppe_rxgoodbyte_h_get(dev_id, (a_uint32_t)port_id,
		(union rxgoodbyte_h_u *)&mib_info->RxGoodByte_hi);
	hppe_rxbadbyte_l_get(dev_id, (a_uint32_t)port_id,
		(union rxbadbyte_l_u *)&mib_info->RxBadByte_lo);
	hppe_rxbadbyte_h_get(dev_id, (a_uint32_t)port_id,
		(union rxbadbyte_h_u *)&mib_info->RxBadByte_hi);
	hppe_rxuni_get(dev_id, (a_uint32_t)port_id, (union rxuni_u *)&mib_info->RxUniCast);
	hppe_txbroad_get(dev_id, (a_uint32_t)port_id, (union txbroad_u *)&mib_info->TxBroad);
	hppe_txpause_get(dev_id, (a_uint32_t)port_id, (union txpause_u *)&mib_info->TxPause);
	hppe_txmulti_get(dev_id, (a_uint32_t)port_id, (union txmulti_u *)&mib_info->TxMulti);
	hppe_txunderrun_get(dev_id, (a_uint32_t)port_id,
		(union txunderrun_u *)&mib_info->TxUnderRun);
	hppe_txpkt64_get(dev_id, (a_uint32_t)port_id, (union txpkt64_u *)&mib_info->Tx64Byte);
	hppe_txpkt65to127_get(dev_id, (a_uint32_t)port_id,
		(union txpkt65to127_u *)&mib_info->Tx128Byte);
	hppe_txpkt128to255_get(dev_id, (a_uint32_t)port_id,
		(union txpkt128to255_u *)&mib_info->Tx256Byte);
	hppe_txpkt256to511_get(dev_id, (a_uint32_t)port_id,
		(union txpkt256to511_u *)&mib_info->Tx512Byte);
	hppe_txpkt512to1023_get(dev_id, (a_uint32_t)port_id,
		(union txpkt512to1023_u *)&mib_info->Tx1024Byte);
	hppe_txpkt1024to1518_get(dev_id, (a_uint32_t)port_id,
		(union txpkt1024to1518_u *)&mib_info->Tx1518Byte);
	hppe_txpkt1519tox_get(dev_id, (a_uint32_t)port_id,
		(union txpkt1519tox_u *)&mib_info->TxMaxByte);
	hppe_txbyte_l_get(dev_id, (a_uint32_t)port_id,
		(union txbyte_l_u *)&mib_info->TxByte_lo);
	hppe_txbyte_h_get(dev_id, (a_uint32_t)port_id,
		(union txbyte_h_u *)&mib_info->TxByte_hi);
	hppe_txcollisions_get(dev_id, (a_uint32_t)port_id,
		(union txcollisions_u *)&mib_info->TxCollision);
	hppe_txabortcol_get(dev_id, (a_uint32_t)port_id,
		(union txabortcol_u *)&mib_info->TxAbortCol);
	hppe_txmulticol_get(dev_id, (a_uint32_t)port_id,
		(union txmulticol_u *)&mib_info->TxMultiCol);
	hppe_txsinglecol_get(dev_id, (a_uint32_t)port_id,
		(union txsinglecol_u *)&mib_info->TxSingalCol);
	hppe_txexcessivedefer_get(dev_id, (a_uint32_t)port_id,
		(union txexcessivedefer_u *)&mib_info->TxExcDefer);
	hppe_txdefer_get(dev_id, (a_uint32_t)port_id, (union txdefer_u *)&mib_info->TxDefer);
	hppe_txlatecol_get(dev_id, (a_uint32_t)port_id, (union txlatecol_u *)&mib_info->TxLateCol);
	hppe_txuni_get(dev_id, (a_uint32_t)port_id, (union txuni_u *)&mib_info->TxUniCast);

	return SW_OK;
}

sw_error_t
adpt_ppe_get_mib_info(a_uint32_t dev_id, fal_port_t port_id,
                     fal_mib_info_t * mib_info)
{
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(mib_info);
#ifdef CPPE
	if (adpt_hppe_chip_revision_get(dev_id) == CPPE_REVISION &&
		port_id == SSDK_PHYSICAL_PORT6)
	{
		return adpt_cppe_lpbk_get_mib_info(dev_id, port_id, mib_info);
	}
#endif
	return adpt_hppe_get_mib_info(dev_id, port_id, mib_info);
}

sw_error_t
adpt_hppe_get_tx_mib_info(a_uint32_t dev_id, fal_port_t port_id,
                     fal_mib_info_t * mib_info )
{
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(mib_info);
	memset(mib_info, 0, sizeof(fal_mib_info_t));

	port_id = HPPE_TO_GMAC_PORT_ID(port_id);
	hppe_txbroad_get(dev_id, (a_uint32_t)port_id, (union txbroad_u *)&mib_info->TxBroad);
	hppe_txpause_get(dev_id, (a_uint32_t)port_id, (union txpause_u *)&mib_info->TxPause);
	hppe_txmulti_get(dev_id, (a_uint32_t)port_id, (union txmulti_u *)&mib_info->TxMulti);
	hppe_txunderrun_get(dev_id, (a_uint32_t)port_id,
		(union txunderrun_u *)&mib_info->TxUnderRun);
	hppe_txpkt64_get(dev_id, (a_uint32_t)port_id, (union txpkt64_u *)&mib_info->Tx64Byte);
	hppe_txpkt65to127_get(dev_id, (a_uint32_t)port_id,
		(union txpkt65to127_u *)&mib_info->Tx128Byte);
	hppe_txpkt128to255_get(dev_id, (a_uint32_t)port_id,
		(union txpkt128to255_u *)&mib_info->Tx256Byte);
	hppe_txpkt256to511_get(dev_id, (a_uint32_t)port_id,
		(union txpkt256to511_u *)&mib_info->Tx512Byte);
	hppe_txpkt512to1023_get(dev_id, (a_uint32_t)port_id,
		(union txpkt512to1023_u *)&mib_info->Tx1024Byte);
	hppe_txpkt1024to1518_get(dev_id, (a_uint32_t)port_id,
		(union txpkt1024to1518_u *)&mib_info->Tx1518Byte);
	hppe_txpkt1519tox_get(dev_id, (a_uint32_t)port_id,
		(union txpkt1519tox_u *)&mib_info->TxMaxByte);
	hppe_txbyte_l_get(dev_id, (a_uint32_t)port_id, (union txbyte_l_u *)&mib_info->TxByte_lo);
	hppe_txbyte_h_get(dev_id, (a_uint32_t)port_id, (union txbyte_h_u *)&mib_info->TxByte_hi);
	hppe_txcollisions_get(dev_id, (a_uint32_t)port_id,
		(union txcollisions_u *)&mib_info->TxCollision);
	hppe_txabortcol_get(dev_id, (a_uint32_t)port_id,
		(union txabortcol_u *)&mib_info->TxAbortCol);
	hppe_txmulticol_get(dev_id, (a_uint32_t)port_id,
		(union txmulticol_u *)&mib_info->TxMultiCol);
	hppe_txsinglecol_get(dev_id, (a_uint32_t)port_id,
		(union txsinglecol_u *)&mib_info->TxSingalCol);
	hppe_txexcessivedefer_get(dev_id, (a_uint32_t)port_id,
		(union txexcessivedefer_u *)&mib_info->TxExcDefer);
	hppe_txdefer_get(dev_id, (a_uint32_t)port_id, (union txdefer_u *)&mib_info->TxDefer);
	hppe_txlatecol_get(dev_id, (a_uint32_t)port_id,
		(union txlatecol_u *)&mib_info->TxLateCol);
	hppe_txuni_get(dev_id, (a_uint32_t)port_id, (union txuni_u *)&mib_info->TxUniCast);

	return SW_OK;
}

sw_error_t
adpt_ppe_mib_status_set(a_uint32_t dev_id, a_bool_t enable)
{
	a_uint32_t port_id = 0, xg_port_id = 0, g_port_id = 0;
	a_uint32_t port_num = SSDK_PHYSICAL_PORT6;
	union mmc_control_u mmc_control;
	sw_error_t rv = SW_OK;

	memset(&mmc_control, 0, sizeof(mmc_control));
	ADPT_DEV_ID_CHECK(dev_id);
#ifdef CPPE
	if (adpt_hppe_chip_revision_get(dev_id) == CPPE_REVISION)
	{
		port_num = SSDK_PHYSICAL_PORT5;
		rv = adpt_cppe_lpbk_mib_status_set(dev_id, SSDK_PHYSICAL_PORT6, enable);
		SW_RTN_ON_ERROR(rv);
	}
#endif
	for (port_id = SSDK_PHYSICAL_PORT1; port_id <= port_num; port_id++) {
		g_port_id = HPPE_TO_GMAC_PORT_ID(port_id);
		hppe_mac_mib_ctrl_mib_en_set(dev_id, g_port_id, (a_uint32_t)enable);
	}

	for (port_id = SSDK_PHYSICAL_PORT5; port_id <= port_num; port_id++) {
		xg_port_id = HPPE_TO_XGMAC_PORT_ID(port_id);
		hppe_mmc_control_get(dev_id, xg_port_id, &mmc_control);

		if(A_TRUE == enable)
			mmc_control.bf.mcf = 0;
		else
			mmc_control.bf.mcf = 1;

		hppe_mmc_control_set(dev_id, xg_port_id, &mmc_control);
	}

	return rv;
}

sw_error_t
adpt_hppe_mib_port_flush_counters(a_uint32_t dev_id, fal_port_t port_id)
{
	union mmc_control_u mmc_control;

	memset(&mmc_control, 0, sizeof(mmc_control));
	ADPT_DEV_ID_CHECK(dev_id);

	if(port_id < SSDK_PHYSICAL_PORT1 || port_id > SSDK_PHYSICAL_PORT6)
		return SW_BAD_PARAM;
	/*GMAC*/
	if(!hppe_xgmac_port_check(port_id))
	{
		port_id = HPPE_TO_GMAC_PORT_ID(port_id);
		hppe_mac_mib_ctrl_mib_reset_set(dev_id, port_id, A_TRUE);
		hppe_mac_mib_ctrl_mib_reset_set(dev_id, port_id, A_FALSE);
	}
	/*XGMAC*/
	else
	{
		port_id = HPPE_TO_XGMAC_PORT_ID(port_id);
		hppe_mmc_control_get(dev_id, port_id, &mmc_control);
		mmc_control.bf.cntrst = 1;
		hppe_mmc_control_set(dev_id, port_id, &mmc_control);
	}

	return SW_OK;
}

sw_error_t
adpt_ppe_mib_port_flush_counters(a_uint32_t dev_id, fal_port_t port_id)
{
#ifdef CPPE
	if (adpt_hppe_chip_revision_get(dev_id) == CPPE_REVISION &&
		port_id == SSDK_PHYSICAL_PORT6)
	{
		return adpt_cppe_lpbk_mib_flush_counters(dev_id, port_id);
	}
#endif
	return adpt_hppe_mib_port_flush_counters(dev_id, port_id);
}

sw_error_t
adpt_hppe_mib_status_get(a_uint32_t dev_id, a_bool_t * enable)
{
	sw_error_t rv = SW_OK;
	a_uint32_t port_id = 0, status = 0;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(enable);

	rv = hppe_mac_mib_ctrl_mib_en_get(dev_id, port_id, &status);
	*enable = status;

	if( rv != SW_OK )
		return rv;

	return SW_OK;
}

sw_error_t
adpt_hppe_get_rx_mib_info(a_uint32_t dev_id, fal_port_t port_id,
                     fal_mib_info_t * mib_info )
{
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(mib_info);
	memset(mib_info, 0, sizeof(fal_mib_info_t));

	port_id = HPPE_TO_GMAC_PORT_ID(port_id);
	hppe_rxbroad_get(dev_id, (a_uint32_t)port_id, (union rxbroad_u *)&mib_info->RxBroad);
	hppe_rxpause_get(dev_id, (a_uint32_t)port_id, (union rxpause_u *)&mib_info->RxPause);
	hppe_rxmulti_get(dev_id, (a_uint32_t)port_id, (union rxmulti_u *)&mib_info->RxMulti);
	hppe_rxfcserr_get(dev_id, (a_uint32_t)port_id, (union rxfcserr_u *)&mib_info->RxFcsErr);
	hppe_rxalignerr_get(dev_id, (a_uint32_t)port_id,
		(union rxalignerr_u *)&mib_info->RxAllignErr);
	hppe_rxrunt_get(dev_id, (a_uint32_t)port_id, (union rxrunt_u *)&mib_info->RxRunt);
	hppe_rxfrag_get(dev_id, (a_uint32_t)port_id, (union rxfrag_u *)&mib_info->RxFragment);
	hppe_rxjumbofcserr_get(dev_id, (a_uint32_t)port_id,
		(union rxjumbofcserr_u *)&mib_info->RxJumboFcsErr);
	hppe_rxjumboalignerr_get(dev_id, (a_uint32_t)port_id,
		(union rxjumboalignerr_u *)&mib_info->RxJumboAligenErr);
	hppe_rxpkt64_get(dev_id, (a_uint32_t)port_id, (union rxpkt64_u *)&mib_info->Rx64Byte);
	hppe_rxpkt65to127_get(dev_id, (a_uint32_t)port_id,
		(union rxpkt65to127_u *)&mib_info->Rx128Byte);
	hppe_rxpkt128to255_get(dev_id, (a_uint32_t)port_id,
		(union rxpkt128to255_u *)&mib_info->Rx256Byte);
	hppe_rxpkt256to511_get(dev_id, (a_uint32_t)port_id,
		(union rxpkt256to511_u *)&mib_info->Rx512Byte);
	hppe_rxpkt512to1023_get(dev_id, (a_uint32_t)port_id,
		(union rxpkt512to1023_u *)&mib_info->Rx1024Byte);
	hppe_rxpkt1024to1518_get(dev_id, (a_uint32_t)port_id,
		(union rxpkt1024to1518_u *)&mib_info->Rx1518Byte);
	hppe_rxpkt1519tox_get(dev_id, (a_uint32_t)port_id,
		(union rxpkt1519tox_u *)&mib_info->RxMaxByte);
	hppe_rxtoolong_get(dev_id, (a_uint32_t)port_id,
		(union rxtoolong_u *)&mib_info->RxTooLong);
	hppe_rxgoodbyte_l_get(dev_id, (a_uint32_t)port_id,
		(union rxgoodbyte_l_u *)&mib_info->RxGoodByte_lo);
	hppe_rxgoodbyte_h_get(dev_id, (a_uint32_t)port_id,
		(union rxgoodbyte_h_u *)&mib_info->RxGoodByte_hi);
	hppe_rxbadbyte_l_get(dev_id, (a_uint32_t)port_id,
		(union rxbadbyte_l_u *)&mib_info->RxBadByte_lo);
	hppe_rxbadbyte_h_get(dev_id, (a_uint32_t)port_id,
		(union rxbadbyte_h_u *)&mib_info->RxBadByte_hi);
	hppe_rxuni_get(dev_id, (a_uint32_t)port_id, (union rxuni_u *)&mib_info->RxUniCast);

	return SW_OK;
}

sw_error_t
adpt_ppe_get_rx_mib_info(a_uint32_t dev_id, fal_port_t port_id,
                     fal_mib_info_t * mib_info)
{
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(mib_info);
#ifdef CPPE
	if (adpt_hppe_chip_revision_get(dev_id) == CPPE_REVISION &&
		port_id == SSDK_PHYSICAL_PORT6)
	{
		return adpt_cppe_lpbk_get_mib_info(dev_id, port_id, mib_info);
	}
#endif
	return adpt_hppe_get_rx_mib_info(dev_id, port_id, mib_info);
}

void adpt_hppe_mib_func_bitmap_init(a_uint32_t dev_id)
{
	adpt_api_t *p_adpt_api = NULL;

	p_adpt_api = adpt_api_ptr_get(dev_id);

	if(p_adpt_api == NULL)
		return;

	p_adpt_api->adpt_mib_func_bitmap = ((1<<FUNC_GET_MIB_INFO)|
						(1<<FUNC_GET_RX_MIB_INFO)|
						(1<<FUNC_GET_TX_MIB_INFO)|
						(1<<FUNC_GET_XGMIB_INFO)|
						(1<<FUNC_GET_TX_XGMIB_INFO)|
						(1<<FUNC_GET_RX_XGMIB_INFO)|
						(1<<FUNC_MIB_STATUS_SET)|
						(1<<FUNC_MIB_STATUS_GET)|
						(1<<FUNC_MIB_PORT_FLUSH_COUNTERS)|
						(1<<FUNC_MIB_CPUKEEP_SET)|
						(1<<FUNC_MIB_CPUKEEP_GET)
						);
	return;
}

static void adpt_hppe_mib_func_unregister(a_uint32_t dev_id, adpt_api_t *p_adpt_api)
{
	if(p_adpt_api == NULL)
		return;

	p_adpt_api->adpt_get_mib_info = NULL;
	p_adpt_api->adpt_get_rx_mib_info = NULL;
	p_adpt_api->adpt_get_tx_mib_info = NULL;
	p_adpt_api->adpt_mib_status_set = NULL;
	p_adpt_api->adpt_mib_status_get = NULL;
	p_adpt_api->adpt_mib_port_flush_counters = NULL;
	p_adpt_api->adpt_mib_cpukeep_set = NULL;
	p_adpt_api->adpt_mib_cpukeep_get = NULL;
	p_adpt_api->adpt_get_xgmib_info = NULL;
	p_adpt_api->adpt_get_tx_xgmib_info = NULL;
	p_adpt_api->adpt_get_rx_xgmib_info = NULL;

	return;
}

sw_error_t
adpt_hppe_get_xgmib_info(a_uint32_t dev_id, fal_port_t port_id,
                     fal_xgmib_info_t * mib_info )
{
	a_uint64_t data_low , data_high ;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(mib_info);
	memset(mib_info, 0, sizeof( * mib_info ));

	if(!(hppe_xgmac_port_check(port_id)))
	{
		printk("this port is not xg port!\n");
		return SW_FAIL;
	}
	port_id = HPPE_TO_XGMAC_PORT_ID(port_id);

	/*get tx xgmib information*/
	data_low = 0; data_high = 0;
	hppe_tx_octet_count_good_bad_low_get(dev_id, port_id, (union tx_octet_count_good_bad_low_u*)&data_low);
	hppe_tx_octet_count_good_bad_high_get(dev_id, port_id, (union tx_octet_count_good_bad_high_u *)&data_high);
	mib_info->TxByte = (data_high<<32) |data_low;

	data_low = 0; data_high = 0;
	hppe_tx_frame_count_good_bad_low_get(dev_id, port_id, (union tx_frame_count_good_bad_low_u *)&data_low);
	hppe_tx_frame_count_good_bad_high_get(dev_id, port_id, (union tx_frame_count_good_bad_high_u *)&data_high);
	mib_info->TxFrame =  (data_high<<32) |data_low;

	data_low = 0; data_high = 0;
	hppe_tx_broadcast_frames_good_low_get(dev_id, port_id, (union tx_broadcast_frames_good_low_u *)&data_low);
	hppe_tx_broadcast_frames_good_high_get(dev_id, port_id, (union tx_broadcast_frames_good_high_u*)&data_high);
	mib_info->TxBroadGood =  (data_high<<32) |data_low;

	data_low = 0; data_high = 0;
	hppe_tx_multicast_frames_good_low_get(dev_id, port_id, (union tx_multicast_frames_good_low_u*)&data_low);
	hppe_tx_multicast_frames_good_high_get(dev_id, port_id, (union tx_multicast_frames_good_high_u*)&data_high);
	mib_info->TxMultiGood =  (data_high<<32) |data_low;

	data_low = 0; data_high = 0;
	hppe_tx_64octets_frames_good_bad_low_get(dev_id, port_id, (union tx_64octets_frames_good_bad_low_u*)&data_low);
	hppe_tx_64octets_frames_good_bad_high_get(dev_id, port_id, (union tx_64octets_frames_good_bad_high_u*)&data_high);
	mib_info->Tx64Byte =  (data_high<<32) |data_low;

	data_low = 0; data_high = 0;
	hppe_tx_65to127octets_frames_good_bad_low_get(dev_id, port_id, (union tx_65to127octets_frames_good_bad_low_u *)&data_low);
	hppe_tx_65to127octets_frames_good_bad_high_get(dev_id, port_id, (union tx_65to127octets_frames_good_bad_high_u *)&data_high);
	mib_info->Tx128Byte =  (data_high<<32) |data_low;

	data_low = 0; data_high = 0;
	hppe_tx_128to255octets_frames_good_bad_low_get(dev_id, port_id, (union tx_128to255octets_frames_good_bad_low_u  *)&data_low);
	hppe_tx_128to255octets_frames_good_bad_high_get(dev_id, port_id, (union tx_128to255octets_frames_good_bad_high_u  *)&data_high);
	mib_info->Tx256Byte =  (data_high<<32) |data_low;

	data_low = 0; data_high = 0;
	hppe_tx_256to511octets_frames_good_bad_low_get(dev_id, port_id, (union tx_256to511octets_frames_good_bad_low_u  *)&data_low);
	hppe_tx_256to511octets_frames_good_bad_high_get(dev_id, port_id, (union tx_256to511octets_frames_good_bad_high_u  *)&data_high);
	mib_info->Tx512Byte =  (data_high<<32) |data_low;

	data_low = 0; data_high = 0;
	hppe_tx_512to1023octets_frames_good_bad_low_get(dev_id, port_id, (union tx_512to1023octets_frames_good_bad_low_u*)&data_low);
	hppe_tx_512to1023octets_frames_good_bad_high_get(dev_id, port_id, (union tx_512to1023octets_frames_good_bad_high_u*)&data_high);
	mib_info->Tx1024Byte =  (data_high<<32) |data_low;

	data_low = 0; data_high = 0;
	hppe_tx_1024tomaxoctets_frames_good_bad_low_get(dev_id, port_id, (union tx_1024tomaxoctets_frames_good_bad_low_u*)&data_low);
	hppe_tx_1024tomaxoctets_frames_good_bad_high_get(dev_id, port_id, (union tx_1024tomaxoctets_frames_good_bad_high_u*)&data_high);
	mib_info->TxMaxByte =  (data_high<<32) |data_low;

	data_low = 0; data_high = 0;
	hppe_tx_unicast_frames_good_bad_low_get(dev_id, port_id, (union tx_unicast_frames_good_bad_low_u*)&data_low);
	hppe_tx_unicast_frames_good_bad_high_get(dev_id, port_id, (union tx_unicast_frames_good_bad_high_u*)&data_high);
	mib_info->TxUnicast =  (data_high<<32) |data_low;

	data_low = 0; data_high = 0;
	hppe_tx_multicast_frames_good_bad_low_get(dev_id, port_id, (union tx_multicast_frames_good_bad_low_u *)&data_low);
	hppe_tx_multicast_frames_good_bad_high_get(dev_id, port_id, (union tx_multicast_frames_good_bad_high_u *)&data_high);
	mib_info->TxMulti =  (data_high<<32) |data_low;

	data_low = 0; data_high = 0;
	hppe_tx_broadcast_frames_good_bad_low_get(dev_id, port_id, (union tx_broadcast_frames_good_bad_low_u*)&data_low);
	hppe_tx_broadcast_frames_good_bad_high_get(dev_id, port_id, (union tx_broadcast_frames_good_bad_high_u*)&data_high);
	mib_info->TxBroad =  (data_high<<32) |data_low;

	data_low = 0; data_high = 0;
	hppe_tx_underflow_error_frames_low_get(dev_id, port_id, (union tx_underflow_error_frames_low_u*)&data_low);
	hppe_tx_underflow_error_frames_high_get(dev_id, port_id, (union tx_underflow_error_frames_high_u*)&data_high);
	mib_info->TxUnderFlowError =  (data_high<<32) |data_low;

	data_low = 0; data_high = 0;
	hppe_tx_octet_count_good_low_get(dev_id, port_id, (union tx_octet_count_good_low_u*)&data_low);
	hppe_tx_octet_count_good_high_get(dev_id, port_id, (union tx_octet_count_good_high_u*)&data_high);
	mib_info->TxByteGood =  (data_high<<32) |data_low;

	data_low = 0; data_high = 0;
	hppe_tx_frame_count_good_low_get(dev_id, port_id, (union tx_frame_count_good_low_u*)&data_low);
	hppe_tx_frame_count_good_high_get(dev_id, port_id, (union tx_frame_count_good_high_u*)&data_high);
	mib_info->TxFrameGood =  (data_high<<32) |data_low;

	data_low = 0; data_high = 0;
	hppe_tx_pause_frames_low_get(dev_id, port_id, (union tx_pause_frames_low_u *)&data_low);
	hppe_tx_pause_frames_high_get(dev_id, port_id, (union tx_pause_frames_high_u *)&data_high);
	mib_info->TxPause =  (data_high<<32) |data_low;

	data_low = 0; data_high = 0;
	hppe_tx_vlan_frames_good_low_get(dev_id, port_id, (union tx_vlan_frames_good_low_u *)&data_low);
	hppe_tx_vlan_frames_good_high_get(dev_id, port_id, (union tx_vlan_frames_good_high_u *)&data_high);
	mib_info->TxVLANFrameGood =  (data_high<<32) |data_low;

	data_low = 0;
	hppe_tx_lpi_usec_cntr_get(dev_id, port_id, (union tx_lpi_usec_cntr_u  *)&data_low);
	mib_info->TxLPIUsec =  data_low;

	data_low = 0;
	hppe_tx_lpi_tran_cntr_get(dev_id, port_id, (union tx_lpi_tran_cntr_u  *)&data_low);
	mib_info->TxLPITran =  data_low;

	/*get rx xgmib information*/
	data_low = 0; data_high = 0;
	hppe_rx_frame_count_good_bad_low_get(dev_id, port_id, (union rx_frame_count_good_bad_low_u *)&data_low);
	hppe_rx_frame_count_good_bad_high_get(dev_id, port_id, (union rx_frame_count_good_bad_high_u *)&data_high);
	mib_info->RxFrame =  (data_high<<32) |data_low;

	data_low = 0; data_high = 0;
	hppe_rx_octet_count_good_bad_low_get(dev_id, port_id, (union rx_octet_count_good_bad_low_u*)&data_low);
	hppe_rx_octet_count_good_bad_high_get(dev_id, port_id, (union rx_octet_count_good_bad_high_u*)&data_high);
	mib_info->RxByte =  (data_high<<32) |data_low;

	data_low = 0; data_high = 0;
	hppe_rx_octet_count_good_low_get(dev_id, port_id, (union rx_octet_count_good_low_u *)&data_low);
	hppe_rx_octet_count_good_high_get(dev_id, port_id, (union rx_octet_count_good_high_u *)&data_high);
	mib_info->RxByteGood =  (data_high<<32) |data_low;

	data_low = 0; data_high = 0;
	hppe_rx_broadcast_frames_good_low_get(dev_id, port_id, (union rx_broadcast_frames_good_low_u *)&data_low);
	hppe_rx_broadcast_frames_good_high_get(dev_id, port_id, (union rx_broadcast_frames_good_high_u*)&data_high);
	mib_info->RxBroadGood =  (data_high<<32) |data_low;

	data_low = 0; data_high = 0;
	hppe_rx_multicast_frames_good_low_get(dev_id, port_id, (union rx_multicast_frames_good_low_u*)&data_low);
	hppe_rx_multicast_frames_good_high_get(dev_id, port_id, (union rx_multicast_frames_good_high_u*)&data_high);
	mib_info->RxMultiGood =  (data_high<<32) |data_low;

	data_low = 0; data_high = 0;
	hppe_rx_crc_error_frames_low_get(dev_id, port_id, (union rx_crc_error_frames_low_u *)&data_low);
	hppe_rx_crc_error_frames_high_get(dev_id, port_id, (union rx_crc_error_frames_high_u *)&data_high);
	mib_info->RxFcsErr =  (data_high<<32) |data_low;

	data_low = 0;
	hppe_rx_runt_error_frames_get(dev_id, port_id, (union rx_runt_error_frames_u  *)&data_low);
	mib_info->RxRuntErr  =   data_low;

	data_low = 0;
	hppe_rx_jabber_error_frames_get(dev_id, port_id, (union rx_jabber_error_frames_u  *)&data_low);
	mib_info->RxJabberError = data_low;

	data_low = 0;
	hppe_rx_undersize_frames_good_get(dev_id, port_id, (union rx_undersize_frames_good_u  *)&data_low);
	mib_info->RxUndersizeGood =  data_low;

	data_low = 0;
	hppe_rx_oversize_frames_good_get(dev_id, port_id, (union rx_oversize_frames_good_u  *)&data_low);
	mib_info->RxOversizeGood = data_low;

	data_low = 0; data_high = 0;
	hppe_rx_64octets_frames_good_bad_low_get(dev_id, port_id, (union rx_64octets_frames_good_bad_low_u  *)&data_low);
	hppe_rx_64octets_frames_good_bad_high_get(dev_id, port_id, (union rx_64octets_frames_good_bad_high_u  *)&data_high);
	mib_info->Rx64Byte =  (data_high<<32) |data_low;

	data_low = 0; data_high = 0;
	hppe_rx_65to127octets_frames_good_bad_low_get(dev_id, port_id, (union rx_65to127octets_frames_good_bad_low_u  *)&data_low);
	hppe_rx_65to127octets_frames_good_bad_high_get(dev_id, port_id, (union rx_65to127octets_frames_good_bad_high_u  *)&data_high);
	mib_info->Rx128Byte =  (data_high<<32) |data_low;

	data_low = 0; data_high = 0;
	hppe_rx_128to255octets_frames_good_bad_low_get(dev_id, port_id, (union rx_128to255octets_frames_good_bad_low_u  *)&data_low);
	hppe_rx_128to255octets_frames_good_bad_high_get(dev_id, port_id, (union rx_128to255octets_frames_good_bad_high_u  *)&data_high);
	mib_info->Rx256Byte =  (data_high<<32) |data_low;

	data_low = 0; data_high = 0;
	hppe_rx_256to511octets_frames_good_bad_low_get	(dev_id, port_id, (union rx_256to511octets_frames_good_bad_low_u  *)&data_low);
	hppe_rx_256to511octets_frames_good_bad_high_get(dev_id, port_id, (union rx_256to511octets_frames_good_bad_high_u  *)&data_high);
	mib_info->Rx512Byte =  (data_high<<32) |data_low;

	data_low = 0; data_high = 0;
	hppe_rx_512to1023octets_frames_good_bad_low_get(dev_id, port_id, (union rx_512to1023octets_frames_good_bad_low_u *)&data_low);
	hppe_rx_512to1023octets_frames_good_bad_high_get(dev_id, port_id, (union rx_512to1023octets_frames_good_bad_high_u *)&data_high);
	mib_info->Rx1024Byte =  (data_high<<32) |data_low;

	data_low = 0; data_high = 0;
	hppe_rx_1024tomaxoctets_frames_good_bad_low_get(dev_id, port_id, (union rx_1024tomaxoctets_frames_good_bad_low_u *)&data_low);
	hppe_rx_1024tomaxoctets_frames_good_bad_high_get(dev_id, port_id, (union rx_1024tomaxoctets_frames_good_bad_high_u *)&data_high);
	mib_info->RxMaxByte =  (data_high<<32) |data_low;

	data_low = 0; data_high = 0;
	hppe_rx_unicast_frames_good_low_get(dev_id, port_id, (union rx_unicast_frames_good_low_u  *)&data_low);
	hppe_rx_unicast_frames_good_high_get(dev_id, port_id, (union rx_unicast_frames_good_high_u  *)&data_high);
	mib_info->RxUnicastGood =  (data_high<<32) |data_low;

	data_low = 0; data_high = 0;
	hppe_rx_length_error_frames_low_get(dev_id, port_id, (union rx_length_error_frames_low_u  *)&data_low);
	hppe_rx_length_error_frames_high_get(dev_id, port_id, (union rx_length_error_frames_high_u  *)&data_high);
	mib_info->RxLengthError =  (data_high<<32) |data_low;

	data_low = 0; data_high = 0;
	hppe_rx_outofrange_frames_low_get(dev_id, port_id, (union rx_outofrange_frames_low_u*)&data_low);
	hppe_rx_outofrange_frames_high_get(dev_id, port_id, (union rx_outofrange_frames_high_u*)&data_high);
	mib_info->RxOutOfRangeError =  (data_high<<32) |data_low;

	data_low = 0; data_high = 0;
	hppe_rx_pause_frames_low_get(dev_id, port_id, (union rx_pause_frames_low_u *)&data_low);
	hppe_rx_pause_frames_high_get(dev_id, port_id, (union rx_pause_frames_high_u *)&data_high);
	mib_info->RxPause =  (data_high<<32) |data_low;

	data_low = 0; data_high = 0;
	hppe_rx_fifooverflow_frames_low_get(dev_id, port_id, (union rx_fifooverflow_frames_low_u *)&data_low);
	hppe_rx_fifooverflow_frames_high_get(dev_id, port_id, (union rx_fifooverflow_frames_high_u *)&data_high);
	mib_info->RxOverFlow =  (data_high<<32) |data_low;

	data_low = 0; data_high = 0;
	hppe_rx_vlan_frames_good_bad_low_get(dev_id, port_id, (union rx_vlan_frames_good_bad_low_u *)&data_low);
	hppe_rx_vlan_frames_good_bad_high_get(dev_id, port_id, (union rx_vlan_frames_good_bad_high_u*)&data_high);
	mib_info->RxVLANFrameGoodBad =  (data_high<<32) |data_low;

	data_low = 0;
	hppe_rx_watchdog_error_frames_get(dev_id, port_id, (union rx_watchdog_error_frames_u*)&data_low);
	mib_info->RxWatchDogError = data_low;

	data_low = 0;
	hppe_rx_lpi_usec_cntr_get(dev_id, port_id, (union rx_lpi_usec_cntr_u *)&data_low);
	mib_info->RxLPIUsec = data_low;

	data_low = 0;
	hppe_rx_lpi_tran_cntr_get(dev_id, port_id, (union rx_lpi_tran_cntr_u *)&data_low);
	mib_info->RxLPITran = data_low;

	data_low = 0; data_high = 0;
	hppe_rx_discard_frame_count_good_bad_low_get(dev_id, port_id, (union rx_discard_frame_count_good_bad_low_u *)&data_low);
	hppe_rx_discard_frame_count_good_bad_high_get(dev_id, port_id, (union rx_discard_frame_count_good_bad_high_u *)&data_high);
	mib_info->RxDropFrameGoodBad =  (data_high<<32) |data_low;

	data_low = 0; data_high = 0;
	hppe_rx_discard_octet_count_good_bad_low_get(dev_id, port_id, (union rx_discard_octet_count_good_bad_low_u *)&data_low);
	hppe_rx_discard_octet_count_good_bad_high_get(dev_id, port_id, (union rx_discard_octet_count_good_bad_high_u *)&data_high);
	mib_info->RxDropByteGoodBad =  (data_high<<32) |data_low;

	return SW_OK;
}

sw_error_t
adpt_hppe_get_tx_xgmib_info(a_uint32_t dev_id, fal_port_t port_id,
                     fal_xgmib_info_t * mib_info )
{

	a_uint64_t data_low , data_high ;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(mib_info);
	memset(mib_info, 0, sizeof(* mib_info));

	if(!(hppe_xgmac_port_check(port_id)))
	{
		printk("this port is not xg port!\n");
		return SW_FAIL;
	}
	port_id = HPPE_TO_XGMAC_PORT_ID(port_id);

	/*get tx xgmib information*/
	data_low = 0; data_high = 0;
	hppe_tx_octet_count_good_bad_low_get(dev_id, port_id, (union tx_octet_count_good_bad_low_u*)&data_low);
	hppe_tx_octet_count_good_bad_high_get(dev_id, port_id, (union tx_octet_count_good_bad_high_u *)&data_high);
	mib_info->TxByte = (data_high<<32) |data_low;

	data_low = 0; data_high = 0;
	hppe_tx_frame_count_good_bad_low_get(dev_id, port_id, (union tx_frame_count_good_bad_low_u *)&data_low);
	hppe_tx_frame_count_good_bad_high_get(dev_id, port_id, (union tx_frame_count_good_bad_high_u *)&data_high);
	mib_info->TxFrame =  (data_high<<32) |data_low;

	data_low = 0; data_high = 0;
	hppe_tx_broadcast_frames_good_low_get(dev_id, port_id, (union tx_broadcast_frames_good_low_u *)&data_low);
	hppe_tx_broadcast_frames_good_high_get(dev_id, port_id, (union tx_broadcast_frames_good_high_u*)&data_high);
	mib_info->TxBroadGood =  (data_high<<32) |data_low;

	data_low = 0; data_high = 0;
	hppe_tx_multicast_frames_good_low_get(dev_id, port_id, (union tx_multicast_frames_good_low_u*)&data_low);
	hppe_tx_multicast_frames_good_high_get(dev_id, port_id, (union tx_multicast_frames_good_high_u*)&data_high);
	mib_info->TxMultiGood =  (data_high<<32) |data_low;

	data_low = 0; data_high = 0;
	hppe_tx_64octets_frames_good_bad_low_get(dev_id, port_id, (union tx_64octets_frames_good_bad_low_u*)&data_low);
	hppe_tx_64octets_frames_good_bad_high_get(dev_id, port_id, (union tx_64octets_frames_good_bad_high_u*)&data_high);
	mib_info->Tx64Byte =  (data_high<<32) |data_low;

	data_low = 0; data_high = 0;
	hppe_tx_65to127octets_frames_good_bad_low_get(dev_id, port_id, (union tx_65to127octets_frames_good_bad_low_u *)&data_low);
	hppe_tx_65to127octets_frames_good_bad_high_get(dev_id, port_id, (union tx_65to127octets_frames_good_bad_high_u *)&data_high);
	mib_info->Tx128Byte =  (data_high<<32) |data_low;

	data_low = 0; data_high = 0;
	hppe_tx_128to255octets_frames_good_bad_low_get(dev_id, port_id, (union tx_128to255octets_frames_good_bad_low_u  *)&data_low);
	hppe_tx_128to255octets_frames_good_bad_high_get(dev_id, port_id, (union tx_128to255octets_frames_good_bad_high_u  *)&data_high);
	mib_info->Tx256Byte =  (data_high<<32) |data_low;

	data_low = 0; data_high = 0;
	hppe_tx_256to511octets_frames_good_bad_low_get(dev_id, port_id, (union tx_256to511octets_frames_good_bad_low_u  *)&data_low);
	hppe_tx_256to511octets_frames_good_bad_high_get(dev_id, port_id, (union tx_256to511octets_frames_good_bad_high_u  *)&data_high);
	mib_info->Tx512Byte =  (data_high<<32) |data_low;

	data_low = 0; data_high = 0;
	hppe_tx_512to1023octets_frames_good_bad_low_get(dev_id, port_id, (union tx_512to1023octets_frames_good_bad_low_u*)&data_low);
	hppe_tx_512to1023octets_frames_good_bad_high_get(dev_id, port_id, (union tx_512to1023octets_frames_good_bad_high_u*)&data_high);
	mib_info->Tx1024Byte =  (data_high<<32) |data_low;

	data_low = 0; data_high = 0;
	hppe_tx_1024tomaxoctets_frames_good_bad_low_get(dev_id, port_id, (union tx_1024tomaxoctets_frames_good_bad_low_u*)&data_low);
	hppe_tx_1024tomaxoctets_frames_good_bad_high_get(dev_id, port_id, (union tx_1024tomaxoctets_frames_good_bad_high_u*)&data_high);
	mib_info->TxMaxByte =  (data_high<<32) |data_low;

	data_low = 0; data_high = 0;
	hppe_tx_unicast_frames_good_bad_low_get(dev_id, port_id, (union tx_unicast_frames_good_bad_low_u*)&data_low);
	hppe_tx_unicast_frames_good_bad_high_get(dev_id, port_id, (union tx_unicast_frames_good_bad_high_u*)&data_high);
	mib_info->TxUnicast =  (data_high<<32) |data_low;

	data_low = 0; data_high = 0;
	hppe_tx_multicast_frames_good_bad_low_get(dev_id, port_id, (union tx_multicast_frames_good_bad_low_u *)&data_low);
	hppe_tx_multicast_frames_good_bad_high_get(dev_id, port_id, (union tx_multicast_frames_good_bad_high_u *)&data_high);
	mib_info->TxMulti =  (data_high<<32) |data_low;

	data_low = 0; data_high = 0;
	hppe_tx_broadcast_frames_good_bad_low_get(dev_id, port_id, (union tx_broadcast_frames_good_bad_low_u*)&data_low);
	hppe_tx_broadcast_frames_good_bad_high_get(dev_id, port_id, (union tx_broadcast_frames_good_bad_high_u*)&data_high);
	mib_info->TxBroad =  (data_high<<32) |data_low;

	data_low = 0; data_high = 0;
	hppe_tx_underflow_error_frames_low_get(dev_id, port_id, (union tx_underflow_error_frames_low_u*)&data_low);
	hppe_tx_underflow_error_frames_high_get(dev_id, port_id, (union tx_underflow_error_frames_high_u*)&data_high);
	mib_info->TxUnderFlowError =  (data_high<<32) |data_low;

	data_low = 0; data_high = 0;
	hppe_tx_octet_count_good_low_get(dev_id, port_id, (union tx_octet_count_good_low_u*)&data_low);
	hppe_tx_octet_count_good_high_get(dev_id, port_id, (union tx_octet_count_good_high_u*)&data_high);
	mib_info->TxByteGood =  (data_high<<32) |data_low;

	data_low = 0; data_high = 0;
	hppe_tx_frame_count_good_low_get(dev_id, port_id, (union tx_frame_count_good_low_u*)&data_low);
	hppe_tx_frame_count_good_high_get(dev_id, port_id, (union tx_frame_count_good_high_u*)&data_high);
	mib_info->TxFrameGood =  (data_high<<32) |data_low;

	data_low = 0; data_high = 0;
	hppe_tx_pause_frames_low_get(dev_id, port_id, (union tx_pause_frames_low_u *)&data_low);
	hppe_tx_pause_frames_high_get(dev_id, port_id, (union tx_pause_frames_high_u *)&data_high);
	mib_info->TxPause =  (data_high<<32) |data_low;

	data_low = 0; data_high = 0;
	hppe_tx_vlan_frames_good_low_get(dev_id, port_id, (union tx_vlan_frames_good_low_u *)&data_low);
	hppe_tx_vlan_frames_good_high_get(dev_id, port_id, (union tx_vlan_frames_good_high_u *)&data_high);
	mib_info->TxVLANFrameGood =  (data_high<<32) |data_low;

	data_low = 0;
	hppe_tx_lpi_usec_cntr_get(dev_id, port_id, (union tx_lpi_usec_cntr_u  *)&data_low);
	mib_info->TxLPIUsec = data_low;

	data_low = 0;
	hppe_tx_lpi_tran_cntr_get(dev_id, port_id, (union tx_lpi_tran_cntr_u  *)&data_low);
	mib_info->TxLPITran = data_low;

	return SW_OK;
}

sw_error_t
adpt_hppe_get_rx_xgmib_info(a_uint32_t dev_id, fal_port_t port_id,
                     fal_xgmib_info_t * mib_info )
{
	a_uint64_t data_low , data_high ;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(mib_info);
	memset(mib_info, 0, sizeof(* mib_info));

	if(!(hppe_xgmac_port_check(port_id)))
	{
		printk("this port is not xg port!\n");
		return SW_FAIL;
	}
	port_id = HPPE_TO_XGMAC_PORT_ID(port_id);

	/*get tx xgmib information*/
	data_low = 0; data_high = 0;
	hppe_rx_frame_count_good_bad_low_get(dev_id, port_id, (union rx_frame_count_good_bad_low_u *)&data_low);
	hppe_rx_frame_count_good_bad_high_get(dev_id, port_id, (union rx_frame_count_good_bad_high_u *)&data_high);
	mib_info->RxFrame =  (data_high<<32) |data_low;

	data_low = 0; data_high = 0;
	hppe_rx_octet_count_good_bad_low_get(dev_id, port_id, (union rx_octet_count_good_bad_low_u*)&data_low);
	hppe_rx_octet_count_good_bad_high_get(dev_id, port_id, (union rx_octet_count_good_bad_high_u*)&data_high);
	mib_info->RxByte =  (data_high<<32) |data_low;

	data_low = 0; data_high = 0;
	hppe_rx_octet_count_good_low_get(dev_id, port_id, (union rx_octet_count_good_low_u *)&data_low);
	hppe_rx_octet_count_good_high_get(dev_id, port_id, (union rx_octet_count_good_high_u *)&data_high);
	mib_info->RxByteGood =  (data_high<<32) |data_low;

	data_low = 0; data_high = 0;
	hppe_rx_broadcast_frames_good_low_get(dev_id, port_id, (union rx_broadcast_frames_good_low_u *)&data_low);
	hppe_rx_broadcast_frames_good_high_get(dev_id, port_id, (union rx_broadcast_frames_good_high_u*)&data_high);
	mib_info->RxBroadGood =  (data_high<<32) |data_low;

	data_low = 0; data_high = 0;
	hppe_rx_multicast_frames_good_low_get(dev_id, port_id, (union rx_multicast_frames_good_low_u*)&data_low);
	hppe_rx_multicast_frames_good_high_get(dev_id, port_id, (union rx_multicast_frames_good_high_u*)&data_high);
	mib_info->RxMultiGood =  (data_high<<32) |data_low;

	data_low = 0; data_high = 0;
	hppe_rx_crc_error_frames_low_get(dev_id, port_id, (union rx_crc_error_frames_low_u *)&data_low);
	hppe_rx_crc_error_frames_high_get(dev_id, port_id, (union rx_crc_error_frames_high_u *)&data_high);
	mib_info->RxFcsErr =  (data_high<<32) |data_low;

	data_low = 0;
	hppe_rx_runt_error_frames_get(dev_id, port_id, (union rx_runt_error_frames_u  *)&data_low);
	mib_info->RxRuntErr = data_low;

	data_low = 0;
	hppe_rx_jabber_error_frames_get(dev_id, port_id, (union rx_jabber_error_frames_u  *)&data_low);
	mib_info->RxJabberError = data_low;

	data_low = 0;
	hppe_rx_undersize_frames_good_get(dev_id, port_id, (union rx_undersize_frames_good_u  *)&data_low);
	mib_info->RxUndersizeGood = data_low;

	data_low = 0;
	hppe_rx_oversize_frames_good_get(dev_id, port_id, (union rx_oversize_frames_good_u  *)&data_low);
	mib_info->RxOversizeGood = data_low;

	data_low = 0; data_high = 0;
	hppe_rx_64octets_frames_good_bad_low_get(dev_id, port_id, (union rx_64octets_frames_good_bad_low_u  *)&data_low);
	hppe_rx_64octets_frames_good_bad_high_get(dev_id, port_id, (union rx_64octets_frames_good_bad_high_u  *)&data_high);
	mib_info->Rx64Byte =  (data_high<<32) |data_low;

	data_low = 0; data_high = 0;
	hppe_rx_65to127octets_frames_good_bad_low_get(dev_id, port_id, (union rx_65to127octets_frames_good_bad_low_u  *)&data_low);
	hppe_rx_65to127octets_frames_good_bad_high_get(dev_id, port_id, (union rx_65to127octets_frames_good_bad_high_u  *)&data_high);
	mib_info->Rx128Byte =  (data_high<<32) |data_low;

	data_low = 0; data_high = 0;
	hppe_rx_128to255octets_frames_good_bad_low_get(dev_id, port_id, (union rx_128to255octets_frames_good_bad_low_u  *)&data_low);
	hppe_rx_128to255octets_frames_good_bad_high_get(dev_id, port_id, (union rx_128to255octets_frames_good_bad_high_u  *)&data_high);
	mib_info->Rx256Byte =  (data_high<<32) |data_low;

	data_low = 0; data_high = 0;
	hppe_rx_256to511octets_frames_good_bad_low_get	(dev_id, port_id, (union rx_256to511octets_frames_good_bad_low_u  *)&data_low);
	hppe_rx_256to511octets_frames_good_bad_high_get(dev_id, port_id, (union rx_256to511octets_frames_good_bad_high_u  *)&data_high);
	mib_info->Rx512Byte =  (data_high<<32) |data_low;

	data_low = 0; data_high = 0;
	hppe_rx_512to1023octets_frames_good_bad_low_get(dev_id, port_id, (union rx_512to1023octets_frames_good_bad_low_u *)&data_low);
	hppe_rx_512to1023octets_frames_good_bad_high_get(dev_id, port_id, (union rx_512to1023octets_frames_good_bad_high_u *)&data_high);
	mib_info->Rx1024Byte =  (data_high<<32) |data_low;

	data_low = 0; data_high = 0;
	hppe_rx_1024tomaxoctets_frames_good_bad_low_get(dev_id, port_id, (union rx_1024tomaxoctets_frames_good_bad_low_u *)&data_low);
	hppe_rx_1024tomaxoctets_frames_good_bad_high_get(dev_id, port_id, (union rx_1024tomaxoctets_frames_good_bad_high_u *)&data_high);
	mib_info->RxMaxByte =  (data_high<<32) |data_low;

	data_low = 0; data_high = 0;
	hppe_rx_unicast_frames_good_low_get(dev_id, port_id, (union rx_unicast_frames_good_low_u  *)&data_low);
	hppe_rx_unicast_frames_good_high_get(dev_id, port_id, (union rx_unicast_frames_good_high_u  *)&data_high);
	mib_info->RxUnicastGood =  (data_high<<32) |data_low;

	data_low = 0; data_high = 0;
	hppe_rx_length_error_frames_low_get(dev_id, port_id, (union rx_length_error_frames_low_u  *)&data_low);
	hppe_rx_length_error_frames_high_get(dev_id, port_id, (union rx_length_error_frames_high_u  *)&data_high);
	mib_info->RxLengthError =  (data_high<<32) |data_low;

	data_low = 0; data_high = 0;
	hppe_rx_outofrange_frames_low_get(dev_id, port_id, (union rx_outofrange_frames_low_u*)&data_low);
	hppe_rx_outofrange_frames_high_get(dev_id, port_id, (union rx_outofrange_frames_high_u*)&data_high);
	mib_info->RxOutOfRangeError =  (data_high<<32) |data_low;

	data_low = 0; data_high = 0;
	hppe_rx_pause_frames_low_get(dev_id, port_id, (union rx_pause_frames_low_u *)&data_low);
	hppe_rx_pause_frames_high_get(dev_id, port_id, (union rx_pause_frames_high_u *)&data_high);
	mib_info->RxPause =  (data_high<<32) |data_low;

	data_low = 0; data_high = 0;
	hppe_rx_fifooverflow_frames_low_get(dev_id, port_id, (union rx_fifooverflow_frames_low_u *)&data_low);
	hppe_rx_fifooverflow_frames_high_get(dev_id, port_id, (union rx_fifooverflow_frames_high_u *)&data_high);
	mib_info->RxOverFlow =  (data_high<<32) |data_low;

	data_low = 0; data_high = 0;
	hppe_rx_vlan_frames_good_bad_low_get(dev_id, port_id, (union rx_vlan_frames_good_bad_low_u *)&data_low);
	hppe_rx_vlan_frames_good_bad_high_get(dev_id, port_id, (union rx_vlan_frames_good_bad_high_u*)&data_high);
	mib_info->RxVLANFrameGoodBad =  (data_high<<32) |data_low;

	data_low = 0;
	hppe_rx_watchdog_error_frames_get(dev_id, port_id, (union rx_watchdog_error_frames_u*)&data_low);
	mib_info->RxWatchDogError = data_low;

	data_low = 0;
	hppe_rx_lpi_usec_cntr_get(dev_id, port_id, (union rx_lpi_usec_cntr_u *)&data_low);
	mib_info->RxLPIUsec = data_low;

	data_low = 0;
	hppe_rx_lpi_tran_cntr_get(dev_id, port_id, (union rx_lpi_tran_cntr_u *)&data_low);
	mib_info->RxLPITran =   (data_high<<32) & data_low;

	data_low = 0; data_high = 0;
	hppe_rx_discard_frame_count_good_bad_low_get(dev_id, port_id, (union rx_discard_frame_count_good_bad_low_u *)&data_low);
	hppe_rx_discard_frame_count_good_bad_high_get(dev_id, port_id, (union rx_discard_frame_count_good_bad_high_u *)&data_high);
	mib_info->RxDropFrameGoodBad =  (data_high<<32) |data_low;

	data_low = 0; data_high = 0;
	hppe_rx_discard_octet_count_good_bad_low_get(dev_id, port_id, (union rx_discard_octet_count_good_bad_low_u *)&data_low);
	hppe_rx_discard_octet_count_good_bad_high_get(dev_id, port_id, (union rx_discard_octet_count_good_bad_high_u *)&data_high);
	mib_info->RxDropByteGoodBad =  (data_high<<32) |data_low;

	return SW_OK;
}

sw_error_t adpt_hppe_mib_init(a_uint32_t dev_id)
{
	adpt_api_t *p_adpt_api = NULL;

	p_adpt_api = adpt_api_ptr_get(dev_id);

	if(p_adpt_api == NULL)
		return SW_FAIL;

	adpt_hppe_mib_func_unregister(dev_id, p_adpt_api);

	if(p_adpt_api->adpt_mib_func_bitmap & (1<<FUNC_GET_MIB_INFO))
	{
		p_adpt_api->adpt_get_mib_info = adpt_ppe_get_mib_info;
	}
	if(p_adpt_api->adpt_mib_func_bitmap & (1<<FUNC_GET_RX_MIB_INFO))
	{
		p_adpt_api->adpt_get_rx_mib_info = adpt_ppe_get_rx_mib_info;
	}
	if(p_adpt_api->adpt_mib_func_bitmap & (1<<FUNC_GET_TX_MIB_INFO))
	{
		p_adpt_api->adpt_get_tx_mib_info = adpt_hppe_get_tx_mib_info;
	}
	if(p_adpt_api->adpt_mib_func_bitmap & (1<<FUNC_MIB_STATUS_SET))
	{
		p_adpt_api->adpt_mib_status_set = adpt_ppe_mib_status_set;
	}
	if(p_adpt_api->adpt_mib_func_bitmap & (1<<FUNC_MIB_STATUS_GET))
	{
		p_adpt_api->adpt_mib_status_get = adpt_hppe_mib_status_get;
	}
	if(p_adpt_api->adpt_mib_func_bitmap & (1<<FUNC_MIB_PORT_FLUSH_COUNTERS))
	{
		p_adpt_api->adpt_mib_port_flush_counters = adpt_ppe_mib_port_flush_counters;
	}
	if(p_adpt_api->adpt_mib_func_bitmap & (1<<FUNC_MIB_CPUKEEP_SET))
	{
		p_adpt_api->adpt_mib_cpukeep_set = adpt_ppe_mib_cpukeep_set;
	}
	if(p_adpt_api->adpt_mib_func_bitmap & (1<<FUNC_MIB_CPUKEEP_GET))
	{
		p_adpt_api->adpt_mib_cpukeep_get = adpt_hppe_mib_cpukeep_get;
	}
	if(p_adpt_api->adpt_mib_func_bitmap & (1<<FUNC_GET_XGMIB_INFO))
	{
		p_adpt_api->adpt_get_xgmib_info= adpt_hppe_get_xgmib_info;
	}
	if(p_adpt_api->adpt_mib_func_bitmap & (1<<FUNC_GET_TX_XGMIB_INFO))
	{
		p_adpt_api->adpt_get_tx_xgmib_info = adpt_hppe_get_tx_xgmib_info;
	}
	if(p_adpt_api->adpt_mib_func_bitmap & (1<<FUNC_GET_RX_XGMIB_INFO))
	{
		p_adpt_api->adpt_get_rx_xgmib_info = adpt_hppe_get_rx_xgmib_info;
	}

	return SW_OK;
}

/**
 * @}
 */
