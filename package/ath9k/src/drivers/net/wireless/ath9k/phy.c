/*
 * Copyright (c) 2008 Atheros Communications Inc.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "ath9k.h"
#include "hw.h"
#include "reg.h"
#include "phy.h"

void
ath9k_hw_write_regs(struct ath_hal *ah, u_int modesIndex, u_int freqIndex,
		    int regWrites)
{
	struct ath_hal_5416 *ahp = AH5416(ah);

	REG_WRITE_ARRAY(&ahp->ah_iniBB_RfGain, freqIndex, regWrites);
}

enum hal_bool
ath9k_hw_set_channel(struct ath_hal *ah, struct hal_channel_internal *chan)
{
	u_int32_t channelSel = 0;
	u_int32_t bModeSynth = 0;
	u_int32_t aModeRefSel = 0;
	u_int32_t reg32 = 0;
	u_int16_t freq;
	struct chan_centers centers;

	ath9k_hw_get_channel_centers(ah, chan, &centers);
	freq = centers.synth_center;

	if (freq < 4800) {
		u_int32_t txctl;

		if (((freq - 2192) % 5) == 0) {
			channelSel = ((freq - 672) * 2 - 3040) / 10;
			bModeSynth = 0;
		} else if (((freq - 2224) % 5) == 0) {
			channelSel = ((freq - 704) * 2 - 3040) / 10;
			bModeSynth = 1;
		} else {
			HDPRINTF(ah, HAL_DBG_CHANNEL,
				 "%s: invalid channel %u MHz\n", __func__,
				 freq);
			return AH_FALSE;
		}

		channelSel = (channelSel << 2) & 0xff;
		channelSel = ath9k_hw_reverse_bits(channelSel, 8);

		txctl = REG_READ(ah, AR_PHY_CCK_TX_CTRL);
		if (freq == 2484) {

			REG_WRITE(ah, AR_PHY_CCK_TX_CTRL,
				  txctl | AR_PHY_CCK_TX_CTRL_JAPAN);
		} else {
			REG_WRITE(ah, AR_PHY_CCK_TX_CTRL,
				  txctl & ~AR_PHY_CCK_TX_CTRL_JAPAN);
		}

	} else if ((freq % 20) == 0 && freq >= 5120) {
		channelSel =
		    ath9k_hw_reverse_bits(((freq - 4800) / 20 << 2), 8);
		aModeRefSel = ath9k_hw_reverse_bits(1, 2);
	} else if ((freq % 10) == 0) {
		channelSel =
		    ath9k_hw_reverse_bits(((freq - 4800) / 10 << 1), 8);
		if (AR_SREV_9100(ah) || AR_SREV_9160_10_OR_LATER(ah))
			aModeRefSel = ath9k_hw_reverse_bits(2, 2);
		else
			aModeRefSel = ath9k_hw_reverse_bits(1, 2);
	} else if ((freq % 5) == 0) {
		channelSel = ath9k_hw_reverse_bits((freq - 4800) / 5, 8);
		aModeRefSel = ath9k_hw_reverse_bits(1, 2);
	} else {
		HDPRINTF(ah, HAL_DBG_CHANNEL,
			 "%s: invalid channel %u MHz\n", __func__, freq);
		return AH_FALSE;
	}

	reg32 =
	    (channelSel << 8) | (aModeRefSel << 2) | (bModeSynth << 1) |
	    (1 << 5) | 0x1;

	REG_WRITE(ah, AR_PHY(0x37), reg32);

	ah->ah_curchan = chan;

	AH5416(ah)->ah_curchanRadIndex = -1;

	return AH_TRUE;
}

enum hal_bool
ath9k_hw_ar9280_set_channel(struct ath_hal *ah,
			    struct hal_channel_internal *chan)
{
	u_int16_t bMode, fracMode, aModeRefSel = 0;
	u_int32_t freq, ndiv, channelSel = 0, channelFrac = 0, reg32 = 0;
	struct chan_centers centers;
	u_int32_t refDivA = 24;

	ath9k_hw_get_channel_centers(ah, chan, &centers);
	freq = centers.synth_center;

	reg32 = REG_READ(ah, AR_PHY_SYNTH_CONTROL);
	reg32 &= 0xc0000000;

	if (freq < 4800) {
		u_int32_t txctl;

		bMode = 1;
		fracMode = 1;
		aModeRefSel = 0;
		channelSel = (freq * 0x10000) / 15;

		txctl = REG_READ(ah, AR_PHY_CCK_TX_CTRL);
		if (freq == 2484) {

			REG_WRITE(ah, AR_PHY_CCK_TX_CTRL,
				  txctl | AR_PHY_CCK_TX_CTRL_JAPAN);
		} else {
			REG_WRITE(ah, AR_PHY_CCK_TX_CTRL,
				  txctl & ~AR_PHY_CCK_TX_CTRL_JAPAN);
		}
	} else {
		bMode = 0;
		fracMode = 0;

		if ((freq % 20) == 0) {
			aModeRefSel = 3;
		} else if ((freq % 10) == 0) {
			aModeRefSel = 2;
		} else {
			aModeRefSel = 0;

			fracMode = 1;
			refDivA = 1;
			channelSel = (freq * 0x8000) / 15;

			OS_REG_RMW_FIELD(ah, AR_AN_SYNTH9,
					 AR_AN_SYNTH9_REFDIVA, refDivA);
		}
		if (!fracMode) {
			ndiv = (freq * (refDivA >> aModeRefSel)) / 60;
			channelSel = ndiv & 0x1ff;
			channelFrac = (ndiv & 0xfffffe00) * 2;
			channelSel = (channelSel << 17) | channelFrac;
		}
	}

	reg32 = reg32 |
	    (bMode << 29) |
	    (fracMode << 28) | (aModeRefSel << 26) | (channelSel);

	REG_WRITE(ah, AR_PHY_SYNTH_CONTROL, reg32);

	ah->ah_curchan = chan;

	AH5416(ah)->ah_curchanRadIndex = -1;

	return AH_TRUE;
}

static void
ath9k_phy_modify_rx_buffer(u_int32_t *rfBuf, u_int32_t reg32,
			   u_int32_t numBits, u_int32_t firstBit,
			   u_int32_t column)
{
	u_int32_t tmp32, mask, arrayEntry, lastBit;
	int32_t bitPosition, bitsLeft;

	tmp32 = ath9k_hw_reverse_bits(reg32, numBits);
	arrayEntry = (firstBit - 1) / 8;
	bitPosition = (firstBit - 1) % 8;
	bitsLeft = numBits;
	while (bitsLeft > 0) {
		lastBit = (bitPosition + bitsLeft > 8) ?
		    8 : bitPosition + bitsLeft;
		mask = (((1 << lastBit) - 1) ^ ((1 << bitPosition) - 1)) <<
		    (column * 8);
		rfBuf[arrayEntry] &= ~mask;
		rfBuf[arrayEntry] |= ((tmp32 << bitPosition) <<
				      (column * 8)) & mask;
		bitsLeft -= 8 - bitPosition;
		tmp32 = tmp32 >> (8 - bitPosition);
		bitPosition = 0;
		arrayEntry++;
	}
}

enum hal_bool
ath9k_hw_set_rf_regs(struct ath_hal *ah, struct hal_channel_internal *chan,
		     u_int16_t modesIndex)
{
	struct ath_hal_5416 *ahp = AH5416(ah);

	u_int32_t eepMinorRev;
	u_int32_t ob5GHz = 0, db5GHz = 0;
	u_int32_t ob2GHz = 0, db2GHz = 0;
	int regWrites = 0;

	if (AR_SREV_9280_10_OR_LATER(ah))
		return AH_TRUE;

	eepMinorRev = ath9k_hw_get_eeprom(ahp, EEP_MINOR_REV);

	RF_BANK_SETUP(ahp->ah_analogBank0Data, &ahp->ah_iniBank0, 1);

	RF_BANK_SETUP(ahp->ah_analogBank1Data, &ahp->ah_iniBank1, 1);

	RF_BANK_SETUP(ahp->ah_analogBank2Data, &ahp->ah_iniBank2, 1);

	RF_BANK_SETUP(ahp->ah_analogBank3Data, &ahp->ah_iniBank3,
		      modesIndex);
	{
		int i;
		for (i = 0; i < ahp->ah_iniBank6TPC.ia_rows; i++) {
			ahp->ah_analogBank6Data[i] =
			    INI_RA(&ahp->ah_iniBank6TPC, i, modesIndex);
		}
	}

	if (eepMinorRev >= 2) {
		if (IS_CHAN_2GHZ(chan)) {
			ob2GHz = ath9k_hw_get_eeprom(ahp, EEP_OB_2);
			db2GHz = ath9k_hw_get_eeprom(ahp, EEP_DB_2);
			ath9k_phy_modify_rx_buffer(ahp->ah_analogBank6Data,
						   ob2GHz, 3, 197, 0);
			ath9k_phy_modify_rx_buffer(ahp->ah_analogBank6Data,
						   db2GHz, 3, 194, 0);
		} else {
			ob5GHz = ath9k_hw_get_eeprom(ahp, EEP_OB_5);
			db5GHz = ath9k_hw_get_eeprom(ahp, EEP_DB_5);
			ath9k_phy_modify_rx_buffer(ahp->ah_analogBank6Data,
						   ob5GHz, 3, 203, 0);
			ath9k_phy_modify_rx_buffer(ahp->ah_analogBank6Data,
						   db5GHz, 3, 200, 0);
		}
	}

	RF_BANK_SETUP(ahp->ah_analogBank7Data, &ahp->ah_iniBank7, 1);

	REG_WRITE_RF_ARRAY(&ahp->ah_iniBank0, ahp->ah_analogBank0Data,
			   regWrites);
	REG_WRITE_RF_ARRAY(&ahp->ah_iniBank1, ahp->ah_analogBank1Data,
			   regWrites);
	REG_WRITE_RF_ARRAY(&ahp->ah_iniBank2, ahp->ah_analogBank2Data,
			   regWrites);
	REG_WRITE_RF_ARRAY(&ahp->ah_iniBank3, ahp->ah_analogBank3Data,
			   regWrites);
	REG_WRITE_RF_ARRAY(&ahp->ah_iniBank6TPC, ahp->ah_analogBank6Data,
			   regWrites);
	REG_WRITE_RF_ARRAY(&ahp->ah_iniBank7, ahp->ah_analogBank7Data,
			   regWrites);

	return AH_TRUE;
}

void
ath9k_hw_rfdetach(struct ath_hal *ah)
{
	struct ath_hal_5416 *ahp = AH5416(ah);

	if (ahp->ah_analogBank0Data != NULL) {
		kfree(ahp->ah_analogBank0Data);
		ahp->ah_analogBank0Data = NULL;
	}
	if (ahp->ah_analogBank1Data != NULL) {
		kfree(ahp->ah_analogBank1Data);
		ahp->ah_analogBank1Data = NULL;
	}
	if (ahp->ah_analogBank2Data != NULL) {
		kfree(ahp->ah_analogBank2Data);
		ahp->ah_analogBank2Data = NULL;
	}
	if (ahp->ah_analogBank3Data != NULL) {
		kfree(ahp->ah_analogBank3Data);
		ahp->ah_analogBank3Data = NULL;
	}
	if (ahp->ah_analogBank6Data != NULL) {
		kfree(ahp->ah_analogBank6Data);
		ahp->ah_analogBank6Data = NULL;
	}
	if (ahp->ah_analogBank6TPCData != NULL) {
		kfree(ahp->ah_analogBank6TPCData);
		ahp->ah_analogBank6TPCData = NULL;
	}
	if (ahp->ah_analogBank7Data != NULL) {
		kfree(ahp->ah_analogBank7Data);
		ahp->ah_analogBank7Data = NULL;
	}
	if (ahp->ah_addac5416_21 != NULL) {
		kfree(ahp->ah_addac5416_21);
		ahp->ah_addac5416_21 = NULL;
	}
	if (ahp->ah_bank6Temp != NULL) {
		kfree(ahp->ah_bank6Temp);
		ahp->ah_bank6Temp = NULL;
	}
}

enum hal_bool
ath9k_hw_get_chip_power_limits(struct ath_hal *ah,
			       struct hal_channel *chans, u_int32_t nchans)
{
	enum hal_bool retVal = AH_TRUE;
	int i;

	for (i = 0; i < nchans; i++) {
		chans[i].maxTxPower = AR5416_MAX_RATE_POWER;
		chans[i].minTxPower = AR5416_MAX_RATE_POWER;
	}
	return retVal;
}


enum hal_bool ath9k_hw_init_rf(struct ath_hal *ah, enum hal_status *status)
{
	struct ath_hal_5416 *ahp = AH5416(ah);

	if (!AR_SREV_9280_10_OR_LATER(ah)) {

		ahp->ah_analogBank0Data =
		    kzalloc((sizeof(u_int32_t) *
			     ahp->ah_iniBank0.ia_rows), GFP_KERNEL);
		ahp->ah_analogBank1Data =
		    kzalloc((sizeof(u_int32_t) *
			     ahp->ah_iniBank1.ia_rows), GFP_KERNEL);
		ahp->ah_analogBank2Data =
		    kzalloc((sizeof(u_int32_t) *
			     ahp->ah_iniBank2.ia_rows), GFP_KERNEL);
		ahp->ah_analogBank3Data =
		    kzalloc((sizeof(u_int32_t) *
			     ahp->ah_iniBank3.ia_rows), GFP_KERNEL);
		ahp->ah_analogBank6Data =
		    kzalloc((sizeof(u_int32_t) *
			     ahp->ah_iniBank6.ia_rows), GFP_KERNEL);
		ahp->ah_analogBank6TPCData =
		    kzalloc((sizeof(u_int32_t) *
			     ahp->ah_iniBank6TPC.ia_rows), GFP_KERNEL);
		ahp->ah_analogBank7Data =
		    kzalloc((sizeof(u_int32_t) *
			     ahp->ah_iniBank7.ia_rows), GFP_KERNEL);

		if (ahp->ah_analogBank0Data == NULL
		    || ahp->ah_analogBank1Data == NULL
		    || ahp->ah_analogBank2Data == NULL
		    || ahp->ah_analogBank3Data == NULL
		    || ahp->ah_analogBank6Data == NULL
		    || ahp->ah_analogBank6TPCData == NULL
		    || ahp->ah_analogBank7Data == NULL) {
			HDPRINTF(ah, HAL_DBG_MALLOC,
				 "%s: cannot allocate RF banks\n",
				 __func__);
			*status = HAL_ENOMEM;
			return AH_FALSE;
		}

		ahp->ah_addac5416_21 =
		    kzalloc((sizeof(u_int32_t) *
			     ahp->ah_iniAddac.ia_rows *
			     ahp->ah_iniAddac.ia_columns), GFP_KERNEL);
		if (ahp->ah_addac5416_21 == NULL) {
			HDPRINTF(ah, HAL_DBG_MALLOC,
				 "%s: cannot allocate ah_addac5416_21\n",
				 __func__);
			*status = HAL_ENOMEM;
			return AH_FALSE;
		}

		ahp->ah_bank6Temp =
		    kzalloc((sizeof(u_int32_t) *
			     ahp->ah_iniBank6.ia_rows), GFP_KERNEL);
		if (ahp->ah_bank6Temp == NULL) {
			HDPRINTF(ah, HAL_DBG_MALLOC,
				 "%s: cannot allocate ah_bank6Temp\n",
				 __func__);
			*status = HAL_ENOMEM;
			return AH_FALSE;
		}
	}

	return AH_TRUE;
}

void
ath9k_hw_decrease_chain_power(struct ath_hal *ah, struct hal_channel *chan)
{
	int i, regWrites = 0;
	struct ath_hal_5416 *ahp = AH5416(ah);
	u_int32_t bank6SelMask;
	u_int32_t *bank6Temp = ahp->ah_bank6Temp;

	switch (ahp->ah_diversityControl) {
	case HAL_ANT_FIXED_A:
		bank6SelMask =
		    (ahp->
		     ah_antennaSwitchSwap & ANTSWAP_AB) ? REDUCE_CHAIN_0 :
		    REDUCE_CHAIN_1;
		break;
	case HAL_ANT_FIXED_B:
		bank6SelMask =
		    (ahp->
		     ah_antennaSwitchSwap & ANTSWAP_AB) ? REDUCE_CHAIN_1 :
		    REDUCE_CHAIN_0;
		break;
	case HAL_ANT_VARIABLE:
		return;
		break;
	default:
		return;
		break;
	}

	for (i = 0; i < ahp->ah_iniBank6.ia_rows; i++)
		bank6Temp[i] = ahp->ah_analogBank6Data[i];

	REG_WRITE(ah, AR_PHY_BASE + 0xD8, bank6SelMask);

	ath9k_phy_modify_rx_buffer(bank6Temp, 1, 1, 189, 0);
	ath9k_phy_modify_rx_buffer(bank6Temp, 1, 1, 190, 0);
	ath9k_phy_modify_rx_buffer(bank6Temp, 1, 1, 191, 0);
	ath9k_phy_modify_rx_buffer(bank6Temp, 1, 1, 192, 0);
	ath9k_phy_modify_rx_buffer(bank6Temp, 1, 1, 193, 0);
	ath9k_phy_modify_rx_buffer(bank6Temp, 1, 1, 222, 0);
	ath9k_phy_modify_rx_buffer(bank6Temp, 1, 1, 245, 0);
	ath9k_phy_modify_rx_buffer(bank6Temp, 1, 1, 246, 0);
	ath9k_phy_modify_rx_buffer(bank6Temp, 1, 1, 247, 0);

	REG_WRITE_RF_ARRAY(&ahp->ah_iniBank6, bank6Temp, regWrites);

	REG_WRITE(ah, AR_PHY_BASE + 0xD8, 0x00000053);
#ifdef ALTER_SWITCH
	REG_WRITE(ah, PHY_SWITCH_CHAIN_0,
		  (REG_READ(ah, PHY_SWITCH_CHAIN_0) & ~0x38)
		  | ((REG_READ(ah, PHY_SWITCH_CHAIN_0) >> 3) & 0x38));
#endif
}
