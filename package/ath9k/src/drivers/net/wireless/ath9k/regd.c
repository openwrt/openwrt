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

#include <linux/kernel.h>
#include <linux/slab.h>
#include "ath9k.h"
#include "regd.h"
#include "regd_common.h"

static int ath9k_regd_chansort(const void *a, const void *b)
{
	const struct hal_channel_internal *ca = a;
	const struct hal_channel_internal *cb = b;

	return (ca->channel == cb->channel) ?
	    (ca->channelFlags & CHAN_FLAGS) -
	    (cb->channelFlags & CHAN_FLAGS) : ca->channel - cb->channel;
}

static void
ath9k_regd_sort(void *a, u_int32_t n, u_int32_t size, ath_hal_cmp_t *cmp)
{
	u_int8_t *aa = a;
	u_int8_t *ai, *t;

	for (ai = aa + size; --n >= 1; ai += size)
		for (t = ai; t > aa; t -= size) {
			u_int8_t *u = t - size;
			if (cmp(u, t) <= 0)
				break;
			swap(u, t, size);
		}
}

static u_int16_t ath9k_regd_get_eepromRD(struct ath_hal *ah)
{
	return ah->ah_currentRD & ~WORLDWIDE_ROAMING_FLAG;
}

static enum hal_bool ath9k_regd_is_chan_bm_zero(u_int64_t *bitmask)
{
	int i;

	for (i = 0; i < BMLEN; i++) {
		if (bitmask[i] != 0)
			return AH_FALSE;
	}
	return AH_TRUE;
}

static enum hal_bool ath9k_regd_is_eeprom_valid(struct ath_hal *ah)
{
	u_int16_t rd = ath9k_regd_get_eepromRD(ah);
	int i;

	if (rd & COUNTRY_ERD_FLAG) {
		u_int16_t cc = rd & ~COUNTRY_ERD_FLAG;
		for (i = 0; i < ARRAY_SIZE(allCountries); i++)
			if (allCountries[i].countryCode == cc)
				return AH_TRUE;
	} else {
		for (i = 0; i < ARRAY_SIZE(regDomainPairs); i++)
			if (regDomainPairs[i].regDmnEnum == rd)
				return AH_TRUE;
	}
	HDPRINTF(ah, HAL_DBG_REGULATORY,
		 "%s: invalid regulatory domain/country code 0x%x\n",
		 __func__, rd);
	return AH_FALSE;
}

static enum hal_bool ath9k_regd_is_fcc_midband_supported(struct ath_hal
							 *ah)
{
	u_int32_t regcap;

	regcap = ah->ah_caps.halRegCap;

	if (regcap & AR_EEPROM_EEREGCAP_EN_FCC_MIDBAND)
		return AH_TRUE;
	else
		return AH_FALSE;
}

static enum hal_bool ath9k_regd_is_ccode_valid(struct ath_hal *ah,
					       u_int16_t cc)
{
	u_int16_t rd;
	int i;

	if (cc == CTRY_DEFAULT)
		return AH_TRUE;
#ifdef AH_DEBUG_COUNTRY
	if (cc == CTRY_DEBUG)
		return AH_TRUE;
#endif
	rd = ath9k_regd_get_eepromRD(ah);
	HDPRINTF(ah, HAL_DBG_REGULATORY, "%s: EEPROM regdomain 0x%x\n",
		 __func__, rd);

	if (rd & COUNTRY_ERD_FLAG) {

		HDPRINTF(ah, HAL_DBG_REGULATORY,
			 "%s: EEPROM setting is country code %u\n",
			 __func__, rd & ~COUNTRY_ERD_FLAG);
		return cc == (rd & ~COUNTRY_ERD_FLAG);
	}

	for (i = 0; i < ARRAY_SIZE(allCountries); i++) {
		if (cc == allCountries[i].countryCode) {
#ifdef AH_SUPPORT_11D
			if ((rd & WORLD_SKU_MASK) == WORLD_SKU_PREFIX)
				return AH_TRUE;
#endif
			if (allCountries[i].regDmnEnum == rd ||
			    rd == DEBUG_REG_DMN || rd == NO_ENUMRD)
				return AH_TRUE;
		}
	}
	return AH_FALSE;
}

static u_int
ath9k_regd_get_wmodes_nreg(struct ath_hal *ah,
			   struct country_code_to_enum_rd *country,
			   struct regDomain *rd5GHz)
{
	u_int modesAvail;

	modesAvail = ah->ah_caps.halWirelessModes;

	if ((modesAvail & ATH9K_MODE_SEL_11G) && (!country->allow11g))
		modesAvail &= ~ATH9K_MODE_SEL_11G;
	if ((modesAvail & ATH9K_MODE_SEL_11A) &&
	    (ath9k_regd_is_chan_bm_zero(rd5GHz->chan11a)))
		modesAvail &= ~ATH9K_MODE_SEL_11A;

	if ((modesAvail & ATH9K_MODE_SEL_11NG_HT20)
	    && (!country->allow11ng20))
		modesAvail &= ~ATH9K_MODE_SEL_11NG_HT20;

	if ((modesAvail & ATH9K_MODE_SEL_11NA_HT20)
	    && (!country->allow11na20))
		modesAvail &= ~ATH9K_MODE_SEL_11NA_HT20;

	if ((modesAvail & ATH9K_MODE_SEL_11NG_HT40PLUS) &&
	    (!country->allow11ng40))
		modesAvail &= ~ATH9K_MODE_SEL_11NG_HT40PLUS;

	if ((modesAvail & ATH9K_MODE_SEL_11NG_HT40MINUS) &&
	    (!country->allow11ng40))
		modesAvail &= ~ATH9K_MODE_SEL_11NG_HT40MINUS;

	if ((modesAvail & ATH9K_MODE_SEL_11NA_HT40PLUS) &&
	    (!country->allow11na40))
		modesAvail &= ~ATH9K_MODE_SEL_11NA_HT40PLUS;

	if ((modesAvail & ATH9K_MODE_SEL_11NA_HT40MINUS) &&
	    (!country->allow11na40))
		modesAvail &= ~ATH9K_MODE_SEL_11NA_HT40MINUS;

	return modesAvail;
}

enum hal_bool ath9k_regd_is_public_safety_sku(struct ath_hal *ah)
{
	u_int16_t rd;

	rd = ath9k_regd_get_eepromRD(ah);

	switch (rd) {
	case FCC4_FCCA:
	case (CTRY_UNITED_STATES_FCC49 | COUNTRY_ERD_FLAG):
		return AH_TRUE;
	case DEBUG_REG_DMN:
	case NO_ENUMRD:
		if (ah->ah_countryCode == CTRY_UNITED_STATES_FCC49)
			return AH_TRUE;
		break;
	}
	return AH_FALSE;
}

static struct country_code_to_enum_rd *ath9k_regd_find_country(u_int16_t
							       countryCode)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(allCountries); i++) {
		if (allCountries[i].countryCode == countryCode)
			return &allCountries[i];
	}
	return NULL;
}

static u_int16_t ath9k_regd_get_default_country(struct ath_hal *ah)
{
	u_int16_t rd;
	int i;

	rd = ath9k_regd_get_eepromRD(ah);
	if (rd & COUNTRY_ERD_FLAG) {
		struct country_code_to_enum_rd *country = NULL;
		u_int16_t cc = rd & ~COUNTRY_ERD_FLAG;

		country = ath9k_regd_find_country(cc);
		if (country != NULL)
			return cc;
	}

	for (i = 0; i < ARRAY_SIZE(regDomainPairs); i++)
		if (regDomainPairs[i].regDmnEnum == rd) {
			if (regDomainPairs[i].singleCC != 0)
				return regDomainPairs[i].singleCC;
			else
				i = ARRAY_SIZE(regDomainPairs);
		}
	return CTRY_DEFAULT;
}

static enum hal_bool ath9k_regd_is_valid_reg_domain(int regDmn,
						    struct regDomain *rd)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(regDomains); i++) {
		if (regDomains[i].regDmnEnum == regDmn) {
			if (rd != NULL) {
				memcpy(rd, &regDomains[i],
				       sizeof(struct regDomain));
			}
			return AH_TRUE;
		}
	}
	return AH_FALSE;
}

static enum hal_bool ath9k_regd_is_valid_reg_domainPair(int regDmnPair)
{
	int i;

	if (regDmnPair == NO_ENUMRD)
		return AH_FALSE;
	for (i = 0; i < ARRAY_SIZE(regDomainPairs); i++) {
		if (regDomainPairs[i].regDmnEnum == regDmnPair)
			return AH_TRUE;
	}
	return AH_FALSE;
}

static enum hal_bool
ath9k_regd_get_wmode_regdomain(struct ath_hal *ah, int regDmn,
			       u_int16_t channelFlag, struct regDomain *rd)
{
	int i, found;
	u_int64_t flags = NO_REQ;
	struct reg_dmn_pair_mapping *regPair = NULL;
	int regOrg;

	regOrg = regDmn;
	if (regDmn == CTRY_DEFAULT) {
		u_int16_t rdnum;
		rdnum = ath9k_regd_get_eepromRD(ah);

		if (!(rdnum & COUNTRY_ERD_FLAG)) {
			if (ath9k_regd_is_valid_reg_domain(rdnum, NULL) ||
			    ath9k_regd_is_valid_reg_domainPair(rdnum)) {
				regDmn = rdnum;
			}
		}
	}

	if ((regDmn & MULTI_DOMAIN_MASK) == 0) {

		for (i = 0, found = 0;
		     (i < ARRAY_SIZE(regDomainPairs)) && (!found); i++) {
			if (regDomainPairs[i].regDmnEnum == regDmn) {
				regPair = &regDomainPairs[i];
				found = 1;
			}
		}
		if (!found) {
			HDPRINTF(ah, HAL_DBG_REGULATORY,
				 "%s: Failed to find reg domain pair %u\n",
				 __func__, regDmn);
			return AH_FALSE;
		}
		if (!(channelFlag & CHANNEL_2GHZ)) {
			regDmn = regPair->regDmn5GHz;
			flags = regPair->flags5GHz;
		}
		if (channelFlag & CHANNEL_2GHZ) {
			regDmn = regPair->regDmn2GHz;
			flags = regPair->flags2GHz;
		}
	}

	found = ath9k_regd_is_valid_reg_domain(regDmn, rd);
	if (!found) {
		HDPRINTF(ah, HAL_DBG_REGULATORY,
			 "%s: Failed to find unitary reg domain %u\n",
			 __func__, regDmn);
		return AH_FALSE;
	} else {
		rd->pscan &= regPair->pscanMask;
		if (((regOrg & MULTI_DOMAIN_MASK) == 0) &&
		    (flags != NO_REQ)) {
			rd->flags = flags;
		}

		rd->flags &= (channelFlag & CHANNEL_2GHZ) ?
		    REG_DOMAIN_2GHZ_MASK : REG_DOMAIN_5GHZ_MASK;
		return AH_TRUE;
	}
}

static enum hal_bool ath9k_regd_is_bit_set(int bit, u_int64_t *bitmask)
{
	int byteOffset, bitnum;
	u_int64_t val;

	byteOffset = bit / 64;
	bitnum = bit - byteOffset * 64;
	val = ((u_int64_t) 1) << bitnum;
	if (bitmask[byteOffset] & val)
		return AH_TRUE;
	else
		return AH_FALSE;
}

static void
ath9k_regd_add_reg_classid(u_int8_t *regclassids, u_int maxregids,
			   u_int *nregids, u_int8_t regclassid)
{
	int i;

	if (regclassid == 0)
		return;

	for (i = 0; i < maxregids; i++) {
		if (regclassids[i] == regclassid)
			return;
		if (regclassids[i] == 0)
			break;
	}

	if (i == maxregids)
		return;
	else {
		regclassids[i] = regclassid;
		*nregids += 1;
	}

	return;
}

static enum hal_bool
ath9k_regd_get_eeprom_reg_ext_bits(struct ath_hal *ah,
				   enum reg_ext_bitmap bit)
{
	return (ah->ah_currentRDExt & (1 << bit)) ? AH_TRUE : AH_FALSE;
}

#ifdef ATH_NF_PER_CHAN

static void ath9k_regd_init_rf_buffer(struct hal_channel_internal *ichans,
				      int nchans)
{
	int i, j, next;

	for (next = 0; next < nchans; next++) {
		for (i = 0; i < NUM_NF_READINGS; i++) {
			ichans[next].nfCalHist[i].currIndex = 0;
			ichans[next].nfCalHist[i].privNF =
			    AR_PHY_CCA_MAX_GOOD_VALUE;
			ichans[next].nfCalHist[i].invalidNFcount =
			    AR_PHY_CCA_FILTERWINDOW_LENGTH;
			for (j = 0; j < HAL_NF_CAL_HIST_MAX; j++) {
				ichans[next].nfCalHist[i].nfCalBuffer[j] =
				    AR_PHY_CCA_MAX_GOOD_VALUE;
			}
		}
	}
}
#endif

enum hal_bool
ath9k_regd_init_channels(struct ath_hal *ah,
			 struct hal_channel *chans, u_int maxchans,
			 u_int *nchans, u_int8_t *regclassids,
			 u_int maxregids, u_int *nregids, u_int16_t cc,
			 u_int32_t modeSelect, enum hal_bool enableOutdoor,
			 enum hal_bool enableExtendedChannels)
{
	u_int modesAvail;
	u_int16_t maxChan = 7000;
	struct country_code_to_enum_rd *country = NULL;
	struct regDomain rd5GHz, rd2GHz;
	const struct cmode *cm;
	struct hal_channel_internal *ichans = &ah->ah_channels[0];
	int next = 0, b;
	u_int8_t ctl;
	int is_quarterchan_cap, is_halfchan_cap;
	int regdmn;
	u_int16_t chanSep;

	HDPRINTF(ah, HAL_DBG_REGULATORY, "%s: cc %u mode 0x%x%s%s\n",
		 __func__, cc, modeSelect,
		 enableOutdoor ? " Enable outdoor" : " ",
		 enableExtendedChannels ? " Enable ecm" : "");

	if (!ath9k_regd_is_ccode_valid(ah, cc)) {

		HDPRINTF(ah, HAL_DBG_REGULATORY,
			 "%s: invalid country code %d\n", __func__, cc);
		return AH_FALSE;
	}

	if (!ath9k_regd_is_eeprom_valid(ah)) {

		HDPRINTF(ah, HAL_DBG_REGULATORY,
			 "%s: invalid EEPROM contents\n", __func__);
		return AH_FALSE;
	}

	ah->ah_countryCode = ath9k_regd_get_default_country(ah);

	if (ah->ah_countryCode == CTRY_DEFAULT) {

		ah->ah_countryCode = cc & COUNTRY_CODE_MASK;

		if ((ah->ah_countryCode == CTRY_DEFAULT) &&
		    (ath9k_regd_get_eepromRD(ah) == CTRY_DEFAULT)) {

			ah->ah_countryCode = CTRY_UNITED_STATES;
		}
	}
#ifdef AH_SUPPORT_11D
	if (ah->ah_countryCode == CTRY_DEFAULT) {
		regdmn = ath9k_regd_get_eepromRD(ah);
		country = NULL;
	} else {
#endif
		country = ath9k_regd_find_country(ah->ah_countryCode);

		if (country == NULL) {
			HDPRINTF(ah, HAL_DBG_REGULATORY,
				 "Country is NULL!!!!, cc= %d\n",
				 ah->ah_countryCode);
			return AH_FALSE;
		} else {
			regdmn = country->regDmnEnum;
#ifdef AH_SUPPORT_11D
			if (((ath9k_regd_get_eepromRD(ah) & WORLD_SKU_MASK)
			     == WORLD_SKU_PREFIX)
			    && (cc == CTRY_UNITED_STATES)) {
				if (!isWwrSKU_NoMidband(ah)
				    &&
				    ath9k_regd_is_fcc_midband_supported
				    (ah))
					regdmn = FCC3_FCCA;
				else
					regdmn = FCC1_FCCA;
			}
#endif
		}
#ifdef AH_SUPPORT_11D
	}
#endif

	if (!ath9k_regd_get_wmode_regdomain
	    (ah, regdmn, ~CHANNEL_2GHZ, &rd5GHz)) {
		HDPRINTF(ah, HAL_DBG_REGULATORY,
			"%s: couldn't find unitary "
			"5GHz reg domain for country %u\n",
			 __func__, ah->ah_countryCode);
		return AH_FALSE;
	}
	if (!ath9k_regd_get_wmode_regdomain
	    (ah, regdmn, CHANNEL_2GHZ, &rd2GHz)) {
		HDPRINTF(ah, HAL_DBG_REGULATORY,
			"%s: couldn't find unitary 2GHz "
			"reg domain for country %u\n",
			 __func__, ah->ah_countryCode);
		return AH_FALSE;
	}

	if (!isWwrSKU(ah)
	    && ((rd5GHz.regDmnEnum == FCC1)
		|| (rd5GHz.regDmnEnum == FCC2))) {
		if (ath9k_regd_is_fcc_midband_supported(ah)) {

			if (!ath9k_regd_get_wmode_regdomain
			    (ah, FCC3_FCCA, ~CHANNEL_2GHZ, &rd5GHz)) {
				HDPRINTF(ah, HAL_DBG_REGULATORY,
					"%s: couldn't find unitary 5GHz "
					"reg domain for country %u\n",
					 __func__, ah->ah_countryCode);
				return AH_FALSE;
			}
		}
	}

	if (country == NULL) {
		modesAvail = ah->ah_caps.halWirelessModes;
	} else {
		modesAvail =
		    ath9k_regd_get_wmodes_nreg(ah, country, &rd5GHz);

		if (!enableOutdoor)
			maxChan = country->outdoorChanStart;
	}

	next = 0;

	if (maxchans > ARRAY_SIZE(ah->ah_channels))
		maxchans = ARRAY_SIZE(ah->ah_channels);

	is_halfchan_cap = ah->ah_caps.halChanHalfRate;
	is_quarterchan_cap = ah->ah_caps.halChanQuarterRate;
	for (cm = modes; cm < &modes[ARRAY_SIZE(modes)]; cm++) {
		u_int16_t c, c_hi, c_lo;
		u_int64_t *channelBM = NULL;
		struct regDomain *rd = NULL;
		struct RegDmnFreqBand *fband = NULL, *freqs;
		int8_t low_adj = 0, hi_adj = 0;

		if ((cm->mode & modeSelect) == 0) {
			HDPRINTF(ah, HAL_DBG_REGULATORY,
				 "%s: skip mode 0x%x flags 0x%x\n",
				 __func__, cm->mode, cm->flags);
			continue;
		}
		if ((cm->mode & modesAvail) == 0) {
			HDPRINTF(ah, HAL_DBG_REGULATORY,
				 "%s: !avail mode 0x%x (0x%x) flags 0x%x\n",
				 __func__, modesAvail, cm->mode,
				 cm->flags);
			continue;
		}
		if (!ath9k_get_channel_edges(ah, cm->flags, &c_lo, &c_hi)) {

			HDPRINTF(ah, HAL_DBG_REGULATORY,
				"%s: channels 0x%x not supported "
				"by hardware\n",
				 __func__, cm->flags);
			continue;
		}
		switch (cm->mode) {
		case ATH9K_MODE_SEL_11A:
		case ATH9K_MODE_SEL_11NA_HT20:
		case ATH9K_MODE_SEL_11NA_HT40PLUS:
		case ATH9K_MODE_SEL_11NA_HT40MINUS:
			rd = &rd5GHz;
			channelBM = rd->chan11a;
			freqs = &regDmn5GhzFreq[0];
			ctl = rd->conformanceTestLimit;
			break;
		case ATH9K_MODE_SEL_11B:
			rd = &rd2GHz;
			channelBM = rd->chan11b;
			freqs = &regDmn2GhzFreq[0];
			ctl = rd->conformanceTestLimit | CTL_11B;
			break;
		case ATH9K_MODE_SEL_11G:
		case ATH9K_MODE_SEL_11NG_HT20:
		case ATH9K_MODE_SEL_11NG_HT40PLUS:
		case ATH9K_MODE_SEL_11NG_HT40MINUS:
			rd = &rd2GHz;
			channelBM = rd->chan11g;
			freqs = &regDmn2Ghz11gFreq[0];
			ctl = rd->conformanceTestLimit | CTL_11G;
			break;
		default:
			HDPRINTF(ah, HAL_DBG_REGULATORY,
				 "%s: Unkonwn HAL mode 0x%x\n", __func__,
				 cm->mode);
			continue;
		}
		if (ath9k_regd_is_chan_bm_zero(channelBM))
			continue;


		if ((cm->mode == ATH9K_MODE_SEL_11NA_HT40PLUS) ||
		    (cm->mode == ATH9K_MODE_SEL_11NG_HT40PLUS)) {
			hi_adj = -20;
		}

		if ((cm->mode == ATH9K_MODE_SEL_11NA_HT40MINUS) ||
		    (cm->mode == ATH9K_MODE_SEL_11NG_HT40MINUS)) {
			low_adj = 20;
		}

		/* XXX: Add a helper here instead */
		for (b = 0; b < 64 * BMLEN; b++) {
			if (ath9k_regd_is_bit_set(b, channelBM)) {
				fband = &freqs[b];


				if (rd5GHz.regDmnEnum == MKK1
				    || rd5GHz.regDmnEnum == MKK2) {
					int i, skipband = 0;
					u_int32_t regcap;

					for (i = 0;
					     i < ARRAY_SIZE(j_bandcheck);
					     i++) {
						if (j_bandcheck[i].
						    freqbandbit == b) {
							regcap =
							    ah->ah_caps.
							    halRegCap;
							if ((j_bandcheck
							     [i].
							     eepromflagtocheck
							     & regcap) ==
							    0) {
								skipband =
								    1;
							} else
							    if ((regcap &
								 AR_EEPROM_EEREGCAP_EN_KK_U2)
								|| (regcap
								    &
								    AR_EEPROM_EEREGCAP_EN_KK_MIDBAND)) {

								rd5GHz.
								    dfsMask
								    |=
								    DFS_MKK4;
								rd5GHz.
								    pscan
								    |=
								    PSCAN_MKK3;
							}
							break;
						}
					}
					if (skipband) {
						HDPRINTF(ah,
							 HAL_DBG_REGULATORY,
							 "%s: Skipping %d "
							"freq band.\n",
							 __func__,
							 j_bandcheck[i].
							 freqbandbit);
						continue;
					}
				}

				ath9k_regd_add_reg_classid(regclassids,
							   maxregids,
							   nregids,
							   fband->
							   regClassId);

				if (IS_HT40_MODE(cm->mode)
				    && (rd == &rd5GHz)) {

					chanSep = 40;


					if (fband->lowChannel == 5280)
						low_adj += 20;

					if (fband->lowChannel == 5170)
						continue;
				} else
					chanSep = fband->channelSep;

				for (c = fband->lowChannel + low_adj;
				     ((c <= (fband->highChannel + hi_adj))
				      && (c >=
					  (fband->lowChannel + low_adj)));
				     c += chanSep) {
					struct hal_channel_internal icv;

					if (!(c_lo <= c && c <= c_hi)) {
						HDPRINTF(ah,
							 HAL_DBG_REGULATORY,
							"%s: c %u out of "
							"range [%u..%u]\n",
							 __func__, c, c_lo,
							 c_hi);
						continue;
					}
					if ((fband->channelBW ==
					     CHANNEL_HALF_BW) &&
					    !is_halfchan_cap) {
						HDPRINTF(ah,
							 HAL_DBG_REGULATORY,
							 "%s: Skipping %u half "
							"rate channel\n",
							 __func__, c);
						continue;
					}

					if ((fband->channelBW ==
					     CHANNEL_QUARTER_BW) &&
					    !is_quarterchan_cap) {
						HDPRINTF(ah,
							 HAL_DBG_REGULATORY,
							"%s: Skipping %u "
							"quarter rate "
							"channel\n",
							 __func__, c);
						continue;
					}

					if (((c + fband->channelSep) / 2) >
					    (maxChan + HALF_MAXCHANBW)) {
						HDPRINTF(ah,
							HAL_DBG_REGULATORY,
							"%s: c %u > "
							"maxChan %u\n",
							__func__, c,
							 maxChan);
						continue;
					}
					if (next >= maxchans) {
						HDPRINTF(ah,
							 HAL_DBG_REGULATORY,
							"%s: too many "
							"channels for channel "
							"table\n",
							 __func__);
						goto done;
					}
					if ((fband->
					     usePassScan & IS_ECM_CHAN)
					    && !enableExtendedChannels) {
						HDPRINTF(ah,
							 HAL_DBG_REGULATORY,
							"Skipping ecm "
							"channel\n");
						continue;
					}
					if ((rd->flags & NO_HOSTAP) &&
					    (ah->ah_opmode ==
					     HAL_M_HOSTAP)) {
						HDPRINTF(ah,
							 HAL_DBG_REGULATORY,
							"Skipping HOSTAP "
							"channel\n");
						continue;
					}
					if (IS_HT40_MODE(cm->mode) &&
					    !
					    (ath9k_regd_get_eeprom_reg_ext_bits
					     (ah, REG_EXT_FCC_DFS_HT40))
					    && (fband->useDfs)
					    && (rd->conformanceTestLimit !=
						MKK)) {

						HDPRINTF(ah,
							 HAL_DBG_REGULATORY,
							"Skipping HT40 "
							"channel "
							"(en_fcc_dfs_ht40 = "
							"0)\n");
						continue;
					}
					if (IS_HT40_MODE(cm->mode) &&
					    !
					    (ath9k_regd_get_eeprom_reg_ext_bits
					     (ah,
					      REG_EXT_JAPAN_NONDFS_HT40))
					    && !(fband->useDfs)
					    && (rd->conformanceTestLimit ==
						MKK)) {
						HDPRINTF(ah,
							 HAL_DBG_REGULATORY,
							"Skipping HT40 "
							"channel (en_jap_ht40 "
							"= 0)\n");
						continue;
					}
					if (IS_HT40_MODE(cm->mode) &&
					    !
					    (ath9k_regd_get_eeprom_reg_ext_bits
					     (ah, REG_EXT_JAPAN_DFS_HT40))
					    && (fband->useDfs)
					    && (rd->conformanceTestLimit ==
						MKK)) {
						HDPRINTF(ah,
							 HAL_DBG_REGULATORY,
							"Skipping HT40 channel"
							" (en_jap_dfs_ht40 = "
							"0)\n");
						continue;
					}
					memset(&icv, 0, sizeof(icv));
					icv.channel = c;
					icv.channelFlags = cm->flags;

					switch (fband->channelBW) {
					case CHANNEL_HALF_BW:
						icv.channelFlags |=
						    CHANNEL_HALF;
						break;
					case CHANNEL_QUARTER_BW:
						icv.channelFlags |=
						    CHANNEL_QUARTER;
						break;
					}

					icv.maxRegTxPower =
					    fband->powerDfs;
					icv.antennaMax = fband->antennaMax;
					icv.regDmnFlags = rd->flags;
					icv.conformanceTestLimit = ctl;
					if (fband->usePassScan & rd->pscan)
						icv.channelFlags |=
						    CHANNEL_PASSIVE;
					else
						icv.channelFlags &=
						    ~CHANNEL_PASSIVE;
					if (fband->useDfs & rd->dfsMask)
						icv.privFlags =
						    CHANNEL_DFS;
					else
						icv.privFlags = 0;
					if (rd->flags & LIMIT_FRAME_4MS)
						icv.privFlags |=
						    CHANNEL_4MS_LIMIT;

					if (icv.privFlags & CHANNEL_DFS) {
						icv.privFlags |=
						    CHANNEL_DISALLOW_ADHOC;
					}
					if (icv.
					    regDmnFlags & ADHOC_PER_11D) {
						icv.privFlags |=
						    CHANNEL_PER_11D_ADHOC;
					}
					if (icv.
					    channelFlags & CHANNEL_PASSIVE) {

						if ((icv.channel < 2412)
						    || (icv.channel >
							2462)) {
							if (rd5GHz.
							    regDmnEnum ==
							    MKK1
							    || rd5GHz.
							    regDmnEnum ==
							    MKK2) {
								u_int32_t
								    regcap
								    =
								    ah->
								    ah_caps.
								    halRegCap;
								if (!
								    (regcap
								     &
								     (AR_EEPROM_EEREGCAP_EN_KK_U1_EVEN
								      |
								      AR_EEPROM_EEREGCAP_EN_KK_U2
								      |
								      AR_EEPROM_EEREGCAP_EN_KK_MIDBAND))
&& isUNII1OddChan(icv.channel)) {

									icv.channelFlags &= ~CHANNEL_PASSIVE;
								} else {
									icv.privFlags |= CHANNEL_DISALLOW_ADHOC;
								}
							} else {
								icv.privFlags |= CHANNEL_DISALLOW_ADHOC;
							}
						}
					}
					if (cm->
					    mode & (ATH9K_MODE_SEL_11A |
						    ATH9K_MODE_SEL_11NA_HT20
						    |
						    ATH9K_MODE_SEL_11NA_HT40PLUS
						    |
						    ATH9K_MODE_SEL_11NA_HT40MINUS)) {
						if (icv.
						    regDmnFlags &
						    (ADHOC_NO_11A |
						     DISALLOW_ADHOC_11A)) {
							icv.privFlags |=
							    CHANNEL_DISALLOW_ADHOC;
						}
					}

					memcpy(&ichans[next++], &icv,
					       sizeof(struct
						      hal_channel_internal));
				}

				if (IS_HT40_MODE(cm->mode)
				    && (fband->lowChannel == 5280)) {
					low_adj -= 20;
				}
			}
		}
	}
done:
      if (next != 0) {
		int i;


		if (next > ARRAY_SIZE(ah->ah_channels)) {
			HDPRINTF(ah, HAL_DBG_REGULATORY,
				 "%s: too many channels %u; truncating to %u\n",
				 __func__, next,
				 (int) ARRAY_SIZE(ah->ah_channels));
			next = ARRAY_SIZE(ah->ah_channels);
		}
#ifdef ATH_NF_PER_CHAN

		ath9k_regd_init_rf_buffer(ichans, next);
#endif

		ath9k_regd_sort(ichans, next,
				sizeof(struct hal_channel_internal),
				ath9k_regd_chansort);
		ah->ah_nchan = next;

		HDPRINTF(ah, HAL_DBG_REGULATORY, "Channel list:\n");
		for (i = 0; i < next; i++) {
			HDPRINTF(ah, HAL_DBG_REGULATORY,
				 "chan: %d flags: 0x%x\n",
				 ichans[i].channel,
				 ichans[i].channelFlags);
			chans[i].channel = ichans[i].channel;
			chans[i].channelFlags = ichans[i].channelFlags;
			chans[i].privFlags = ichans[i].privFlags;
			chans[i].maxRegTxPower = ichans[i].maxRegTxPower;
		}

		ath9k_hw_get_chip_power_limits(ah, chans, next);
		for (i = 0; i < next; i++) {
			ichans[i].maxTxPower = chans[i].maxTxPower;
			ichans[i].minTxPower = chans[i].minTxPower;
		}
	}
	*nchans = next;

	ah->ah_countryCode = ah->ah_countryCode;

	ah->ah_currentRDInUse = regdmn;
	ah->ah_currentRD5G = rd5GHz.regDmnEnum;
	ah->ah_currentRD2G = rd2GHz.regDmnEnum;
	if (country == NULL) {
		ah->ah_iso[0] = 0;
		ah->ah_iso[1] = 0;
	} else {
		ah->ah_iso[0] = country->isoName[0];
		ah->ah_iso[1] = country->isoName[1];
	}
	return next != 0;
}

struct hal_channel_internal *ath9k_regd_check_channel(struct ath_hal *ah,
	const struct hal_channel *c)
{
	struct hal_channel_internal *base, *cc;

	int flags = c->channelFlags & CHAN_FLAGS;
	int n, lim;

	HDPRINTF(ah, HAL_DBG_REGULATORY,
		 "%s: channel %u/0x%x (0x%x) requested\n", __func__,
		 c->channel, c->channelFlags, flags);

	cc = ah->ah_curchan;
	if (cc != NULL && cc->channel == c->channel &&
	    (cc->channelFlags & CHAN_FLAGS) == flags) {
		if ((cc->privFlags & CHANNEL_INTERFERENCE) &&
		    (cc->privFlags & CHANNEL_DFS))
			return NULL;
		else
			return cc;
	}

	base = ah->ah_channels;
	n = ah->ah_nchan;

	for (lim = n; lim != 0; lim >>= 1) {
		int d;
		cc = &base[lim >> 1];
		d = c->channel - cc->channel;
		if (d == 0) {
			if ((cc->channelFlags & CHAN_FLAGS) == flags) {
				if ((cc->privFlags & CHANNEL_INTERFERENCE)
				    && (cc->privFlags & CHANNEL_DFS))
					return NULL;
				else
					return cc;
			}
			d = flags - (cc->channelFlags & CHAN_FLAGS);
		}
		HDPRINTF(ah, HAL_DBG_REGULATORY,
			 "%s: channel %u/0x%x d %d\n", __func__,
			 cc->channel, cc->channelFlags, d);
		if (d > 0) {
			base = cc + 1;
			lim--;
		}
	}
	HDPRINTF(ah, HAL_DBG_REGULATORY, "%s: no match for %u/0x%x\n",
		 __func__, c->channel, c->channelFlags);
	return NULL;
}

u_int
ath9k_regd_get_antenna_allowed(struct ath_hal *ah,
			       struct hal_channel *chan)
{
	struct hal_channel_internal *ichan = NULL;

	ichan = ath9k_regd_check_channel(ah, chan);
	if (!ichan)
		return 0;

	return ichan->antennaMax;
}

u_int ath9k_regd_get_ctl(struct ath_hal *ah, struct hal_channel *chan)
{
	u_int ctl = NO_CTL;
	struct hal_channel_internal *ichan;

	if (ah->ah_countryCode == CTRY_DEFAULT && isWwrSKU(ah)) {
		if (IS_CHAN_B(chan))
			ctl = SD_NO_CTL | CTL_11B;
		else if (IS_CHAN_G(chan))
			ctl = SD_NO_CTL | CTL_11G;
		else
			ctl = SD_NO_CTL | CTL_11A;
	} else {
		ichan = ath9k_regd_check_channel(ah, chan);
		if (ichan != NULL) {
			ctl = ichan->conformanceTestLimit;

			if (IS_CHAN_PUREG(chan) && (ctl & 0xf) == CTL_11B)
				ctl = (ctl & ~0xf) | CTL_11G;
		}
	}
	return ctl;
}

void ath9k_regd_get_current_country(struct ath_hal *ah,
				    struct hal_country_entry *ctry)
{
	u_int16_t rd = ath9k_regd_get_eepromRD(ah);

	ctry->isMultidomain = AH_FALSE;
	if (rd == CTRY_DEFAULT)
		ctry->isMultidomain = AH_TRUE;
	else if (!(rd & COUNTRY_ERD_FLAG))
		ctry->isMultidomain = isWwrSKU(ah);

	ctry->countryCode = ah->ah_countryCode;
	ctry->regDmnEnum = ah->ah_currentRD;
	ctry->regDmn5G = ah->ah_currentRD5G;
	ctry->regDmn2G = ah->ah_currentRD2G;
	ctry->iso[0] = ah->ah_iso[0];
	ctry->iso[1] = ah->ah_iso[1];
	ctry->iso[2] = ah->ah_iso[2];
}
