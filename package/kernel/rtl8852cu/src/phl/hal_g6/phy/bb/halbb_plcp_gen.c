/******************************************************************************
 *
 * Copyright(c) 2007 - 2020  Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * The full GNU General Public License is included in this distribution in the
 * file called LICENSE.
 *
 * Contact Information:
 * wlanfae <wlanfae@realtek.com>
 * Realtek Corporation, No. 2, Innovation Road II, Hsinchu Science Park,
 * Hsinchu 300, Taiwan.
 *
 * Larry Finger <Larry.Finger@lwfinger.net>
 *
 *****************************************************************************/
#include "halbb_precomp.h"
#include "halbb_plcp_gen.h"
//search tb_ru_tot_sts_max

void halbb_set_bit(u8 strt, u8 len, u32 in, u32 *out)
{
	u32 bit_mask = 0;
	u8 i = 0;

	for (i = 0; i < len; i++)
		bit_mask += 1 << i;
	in &= bit_mask;
	*out |= (in << strt);
}

u32 halbb_ceil(u32 numerator, u32 denominator)
{
	u32 out = 0;
	/*========[ return BSOD ]========*/
//	if (denominator == 0)
//		return 0;
	out = (numerator / denominator) + (numerator > (numerator / denominator)*denominator);
	return out;
}

u32 halbb_mod(u32 numerator, u32 denominator)
{
	u32 out;
	/*========[ return BSOD ]========*/
//	if (denominator == 0)
//		return 0;
	out = numerator - (numerator / denominator) * denominator;
	return out;
}

u32 halbb_min(u32 val_1, u32 val_2)
{
	u32 out = val_1 > val_2 ? val_2 : val_1;
	return out;
}

u32 halbb_max(u32 val_1, u32 val_2)
{
	u32 out = val_1 < val_2 ? val_2 : val_1;
	return out;
}
enum spec_list halbb_format2spec(enum packet_format_t in, bool *valid)
{
	enum spec_list spec = SPEC_B_MODE;

	*valid = true;

	switch (in){
	case B_MODE_FMT:
		spec = SPEC_B_MODE;
		break;
	case LEGACY_FMT:
		spec = SPEC_LEGACY;
		break;
	case HT_MF_FMT:
	case HT_GF_FMT:
		spec = SPEC_HT;
		break;
	case VHT_FMT:
		spec = SPEC_VHT;
		break;
	case HE_SU_FMT:
	case HE_ER_SU_FMT:
	case HE_MU_FMT:
	case HE_TB_FMT:
		spec = SPEC_HE;
		break;
	case EHT_MU_SU_FMT:
	case EHT_MU_MU_FMT:
	case EHT_MU_RU_FMT:
	case EHT_MU_ERSU_FMT:
	case EHT_TB_FMT:
		spec = SPEC_EHT;
		break;
	default:
		(*valid) = false;
		break;
	}

	return spec;
}

void halbb_find_apep(u32 *apep, bool *can_find, u32 *n_mpdu, u32 *mpdu_length, u8 spec_idx)
{
	u32 apep_tmp;
	bool is_match;
	
	if (spec_idx == SPEC_HT)
		*n_mpdu = halbb_max(halbb_ceil(*apep, (1 << 12) - 1 + 4), 2);
	else
		*n_mpdu = halbb_ceil(*apep, (1 << 14) - 1 + 4);

	/*========[ return BSOD ]========*/
//	if (*n_mpdu == 0)
//		return;


	*mpdu_length = *apep / *n_mpdu - 4; //?????????????
	*mpdu_length = *apep - (4 + 4 * halbb_ceil(*mpdu_length, 4)) * (*n_mpdu - 1) - 4;
	apep_tmp = (*n_mpdu - 1)*(4 + 4 * halbb_ceil(*mpdu_length, 4)) + (4 + *mpdu_length);
	is_match = (apep_tmp == *apep);
	*can_find = true;
	while (!is_match)
	{
		*n_mpdu = *n_mpdu + 1;

		/*========[ return BSOD ]========*/
//		if (*n_mpdu == 0)
//			return;

		*mpdu_length = *apep / *n_mpdu - 4;
		*mpdu_length = *apep - (4 + 4 * halbb_ceil(*mpdu_length , 4)) * (*n_mpdu - 1) - 4;
		apep_tmp = (*n_mpdu - 1)*(4 + 4 * halbb_ceil(*mpdu_length , 4)) + (4 + *mpdu_length);
		is_match = apep_tmp == *apep;
		if (*n_mpdu > (1 << 8)){
			*can_find = false;
			break;
		}
	}
}


void halbb_com_par_cal(struct bb_info *bb, u16 n_sd, enum coding_rate_t code_rate, u8 n_bpscs, u8 nss, bool dcm, struct plcp_mcs_table_out_t *out)
{
	if (dcm)
		out->n_cbps = (n_sd * nss * n_bpscs) >> 1;
	else
		out->n_cbps = n_sd * nss * n_bpscs;
	
	switch (code_rate) {
		case R12:
			out->n_dbps = out->n_cbps >> 1;
			break;
		case R23:
			out->n_dbps = (out->n_cbps * 2) / 3;
			break;
		case R34:
			out->n_dbps = (out->n_cbps * 3) / 4;
			break;
		case R56:
			out->n_dbps = (out->n_cbps * 5) / 6;
			break;
		default:
			break;
	}
}

bool ldpc_extra_check(u32 n_avbits, u32 n_pld, u8 code_rate)
{
	bool cnd0, cnd1, cnd2;
	u32 n_cw = 0, l_ldpc = 0, l_ldpc_idx = 0;
	u32 n_shrt = 0, n_punc = 0;
	u32 table0[4][4] = {{456, 304, 228, 152}, {732, 488, 366, 244},
			    {1458, 972, 729, 486}, {972, 1296, 1458, 1620}};//912 * (1-R), 1464 * (1-R), 2916 * (1-R), 1944 * R
	u32 table1[3][4] = {{324, 432, 486, 540}, {648, 864, 972, 1080},
			    {972, 1296, 1458, 1620}};//648 * R, 1296 * R, 1944 * R
	u32 table2[4]  = {1, 2, 3, 5} ;

	if (n_avbits <= 648) {
		n_cw = 1;
		if (n_avbits >= n_pld + table0[0][code_rate])
			l_ldpc = 1296;
		else
			l_ldpc = 648;
	} else if (n_avbits <= 1296) {
		n_cw = 1;
		if (n_avbits >= n_pld + table0[1][code_rate])
			l_ldpc = 1944;
		else
			l_ldpc = 1296;
	} else if (n_avbits <= 1944) {
		n_cw = 1;
		l_ldpc = 1944;
	} else if (n_avbits <= 2592) {
		n_cw = 2;
		if (n_avbits >= n_pld + table0[2][code_rate])
			l_ldpc = 1944;
		else
			l_ldpc = 1296;
	} else {
		l_ldpc = 1944;
		n_cw = halbb_ceil(n_pld, table0[3][code_rate]);
	}

	if (l_ldpc == 648)
		l_ldpc_idx = 0;
	else if (l_ldpc == 1296)
		l_ldpc_idx = 1;
	else if (l_ldpc == 1944)
		l_ldpc_idx = 2;

	n_shrt = n_cw * table1[l_ldpc_idx][code_rate] > n_pld ? n_cw * table1[l_ldpc_idx][code_rate] - n_pld : 0;
	n_punc = n_cw * l_ldpc > n_avbits + n_shrt ? n_cw * l_ldpc - n_avbits - n_shrt : 0;
	
	cnd0 = 10 * n_punc > n_cw * (l_ldpc - table1[l_ldpc_idx][code_rate]);

	cnd1 = 10 * n_shrt < 12 * n_punc * table2[code_rate];
	cnd2 = n_punc * 10 > 3 * n_cw * (l_ldpc - table1[l_ldpc_idx][code_rate]); //n_punc * 10 > 3 * n_cw * (l_ldpc - table1[l_ldpc_idx][code_rate]); ?

	return (cnd0 && cnd1) || cnd2;
}

bool halbb_legacy_mcs_table(struct bb_info *bb, const struct plcp_mcs_table_in_t *in, struct plcp_mcs_table_out_t *out)
{
	u16 n_dbps_table[8] = {24, 36, 48, 72, 96, 144, 192, 216}; // int size
	u16 n_cbps_table[8] = {48, 48, 96, 96, 192, 192, 288, 288};
	enum coding_rate_t code_rate_table[8] = {R12, R34, R12, R34, R12, R34,
						 R23,R34};
	/*if (in->mcs < 8)
		out->valid = true;
	else
		out->valid = false;
	*/
	if (in->mcs >= 8) {
		//rtw_error("invalid mcs input");
		out->valid = false;
		return false;
	} else {
		out->valid = true;
	}

	out->code_rate = code_rate_table[in->mcs];
	out->n_cbps = n_cbps_table[in->mcs];
	out->n_dbps = n_dbps_table[in->mcs];
	out->n_es = 1;
	out->fec = 0;
	out->dcm = 0;
	out->nss = 1;

	return true;
}

bool halbb_ht_mcs_table(struct bb_info *bb, const struct plcp_mcs_table_in_t *in, struct plcp_mcs_table_out_t *out)
{

	u8 nss, mcs;
	u16 n_sd_table[2] = {52, 108};
	u8 n_bpscs_table[8] = {1, 2, 2, 4, 4, 6, 6, 6};
	enum coding_rate_t code_rate_table[8] = {R12, R12, R34, R12, R34, R23,
						 R34, R56};

	out->valid = ((in->bw < 2) && (in->mcs <= 32));
	if ((in->mcs == 32) && (in->bw == 1)) {
		out->code_rate = R12;
		out->n_cbps = 48;
		out->n_dbps = 24;
		out->n_es = 1;
		out->valid = in->fec == BCC;
		out->fec = 0;
		out->nss = 1;
		out->dcm = 0;
	} else if (out->valid) {
		nss = (in->mcs >> 3) + 1;
		mcs = in->mcs - ((nss - 1) << 3);
		out->code_rate = code_rate_table[mcs];
		halbb_com_par_cal(bb, n_sd_table[in->bw], code_rate_table[mcs],
				  n_bpscs_table[mcs], nss, false, out);
		out->n_es = in->fec == BCC ? ((in->mcs == 21 || in->mcs == 22 ||
					      in->mcs == 23 || in->mcs == 28 ||
					      in->mcs == 29 || in->mcs == 30 ||
					      in->mcs == 31) && in->bw == 1) + 1
					      : 0;
		out->fec = in->fec;
		out->nss = nss;
		out->dcm = 0;
	} else {
		return false;
		//rtw_error("invalid mcs input");
	}
	return true;
}

bool halbb_vht_mcs_table(struct bb_info *bb, const struct plcp_mcs_table_in_t *in, struct plcp_mcs_table_out_t *out){
	u16 n_sd_table[4] = {52, 108, 234, 468};
	enum coding_rate_t code_rate_table[12] = {R12, R12, R34, R12, R34, R23,
						  R34, R56, R34, R56, R34, R56};
	u8 n_bpscs_table[12] = { 1, 2, 2, 4, 4, 6, 6, 6, 8, 8, 10, 10};
	s8 n_es_table[4][8][12] = {
		{
			{ 1, 1, 1, 1, 1, 1, 1, 1, 1, -1, 0, 0 },
			{ 1, 1, 1, 1, 1, 1, 1, 1, 1, -1, 0, 0 },
			{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 },
			{ 1, 1, 1, 1, 1, 1, 1, 1, 1, -1, 0, 0 },
			{ 1, 1, 1, 1, 1, 1, 1, 1, 1, -1, 0, 0 },
			{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 },
			{ 1, 1, 1, 1, 1, 1, 1, 1, 2, -1, 0, 0 },
			{ 1, 1, 1, 1, 1, 1, 1, 1, 2, -1, 0, 0 }
		},
		{
			{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 },
			{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 },
			{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 },
			{ 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 0, 0 },
			{ 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 0, 0 },
			{ 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 0, 0 },
			{ 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 0, 0 },
			{ 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 0, 0}
		},
		{
			{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 },
			{ 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 0, 0 },
			{ 1, 1, 1, 1, 1, 2, -1, 2, 2, 3, 0, 0 },
			{ 1, 1, 1, 1, 2, 2, 2, 3, 3, 3, 0, 0 },
			{ 1, 1, 1, 2, 2, 3, 3, 3, 4, 4, 0, 0 },
			{ 1, 1, 1, 2, 2, 3, 3, 4, 4, -1, 0, 0 },
			{ 1, 1, 3, 2, 3, 4, -1, 6, 6, 6, 0, 0 },
			{ 1, 1, 2, 2, 3, 4, 4, 6, 6, 6, 0, 0 }
		},
		{
			{ 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 0, 0 },
			{ 1, 1, 1, 1, 2, 2, 2, 3, 3, 3, 0, 0 },
			{ 1, 1, 1, 2, 2, 3, 3, 4, 4, -1, 0, 0 },
			{ 1, 1, 2, 2, 3, 4, 4, 6, 6, 6, 0, 0 },
			{ 1, 2, 2, 3, 4, 5, 5, 6, 8, 8, 0, 0 },
			{ 1, 2, 2, 3, 4, 6, 6, 8, 8, 9, 0, 0 },
			{ 1, 2, 3, 4, 6, 7, 7, 9, 12, 12, 0, 0 },
			{ 1, 2, 3, 4, 6, 8, 8, 9, 12, 12, 0, 0 }
		}
	};
	u16 n_sd = n_sd_table[in->bw];
	u8 n_bpscs = n_bpscs_table[in->mcs];
	//enum coding_rate_t code_rate = code_rate_table[in->mcs]; 
	u8 nss = in->nss;

	out->valid = ((((in->fec == LDPC) && (in->mcs <= 11)) ||
		      ((in->fec == BCC) && (in->mcs <= 9))) &&
		      ((in->nss > 0) && (in->nss <= 8)));
	out->code_rate = *(code_rate_table + in->mcs);
	halbb_com_par_cal(bb, n_sd, *(code_rate_table + in->mcs), n_bpscs, nss, false, out);
	if ((in->fec == BCC) && (out->valid)) {
		s8 n_es = *(*(*(n_es_table + in->bw) + in->nss - 1) + in->mcs);
		out->n_es = n_es;
		out->valid = (n_es != -1);
		out->fec = in->fec;
		out->dcm = 0;
		out->nss = in->nss;
	} else if (out->valid) {
		out->n_es = 0;
		out->fec = in->fec;
		out->dcm = 0;
		out->nss = in->nss;
	} else {
		//rtw_error("invalid mcs input");
		return false;
	}
	return true;
}

bool halbb_he_mcs_table(struct bb_info *bb, const struct plcp_mcs_table_in_t *in, struct plcp_mcs_table_out_t *out)
{
	struct plcp_mcs_table_out_t out_temp;
	u16 n_sd_table[8] = {24, 48, 102, 234, 468, 980, 1960, 52};
	enum coding_rate_t code_rate_table[12] = {R12, R12, R34, R12, R34, R23,
						  R34, R56, R34, R56, R34, R56};
	u8 n_bpscs_table[12] = {1, 2, 2, 4, 4, 6, 6, 6, 8, 8, 10, 10};
	u16 n_sd_short_table[2][8] = {{6,12,24,60,120,240,492,0},
				      {2,6,12,30,60,120,246,0}	};
	out->valid = ((in->nss > 0 && in->nss <= 8) && (!in->dcm || in->mcs == 0 || in->mcs == 1 || in->mcs == 3 || in->mcs == 4) && (!in->dcm || in->nss == 1 || in->nss == 2) && (in->fec == LDPC || ( in->mcs <= 9 && in->ru_size <= RU242)));
	if (!out->valid) {
		//rtw_error("invalid mcs input");
		return false;
	}
	out->code_rate = code_rate_table[in->mcs];
	halbb_com_par_cal(bb, n_sd_table[in->ru_size], code_rate_table[in->mcs],
			  n_bpscs_table[in->mcs], in->ru_size==HESIGB ? 1 : in->nss,
			  in->dcm, out);
	out->n_es = in->fec == BCC || in->ru_size == HESIGB;
	halbb_com_par_cal(bb, n_sd_short_table[in->dcm][in->ru_size], code_rate_table[in->mcs], n_bpscs_table[in->mcs],in->nss,false,&out_temp);
	out->he_n_cbps_short = out_temp.n_cbps;
	out->he_n_dbps_short = out_temp.n_dbps;
	out->fec = in->fec || in->ru_size == HESIGB;
	out->dcm = in->dcm;
	out->nss = in->ru_size == HESIGB ? 1 : in->nss;

	return true;
}

bool halbb_eht_mcs_table(struct bb_info* bb, const struct plcp_mcs_table_in_t* in, struct plcp_mcs_table_out_t* out)
{
	struct plcp_mcs_table_out_t out_temp;
	u16 n_sd_table[17] = { 24, 48, 102, 234, 468, 980, 1960, 52 ,3920, 72, 126, 702, 1448, 1682, 2482, 2940, 3408 };
	u16 n_sd_dup_table[3] = { 234, 490, 980 }; //mcs14 only be used in DUP mode when bw80/160/320
	enum coding_rate_t code_rate_table[16] = { R12, R12, R34, R12, R34, R23, R34, R56, R34, R56, R34, R56, R34, R56, R12, R12 };
	u8 n_bpscs_table[16] = { 1, 2, 2, 4, 4, 6, 6, 6, 8, 8, 10, 10, 12, 12, 1, 1 };
	u16 n_sd_short_table[2][17] = { {6,12,24,60,120,240,492,0,984,18,30,180,360,420,612,732,852},
					  {2,6,12,30,60,120,246,0,492,8,14,90,180,210,0,366,0} };
	u16 n_sd_dup_short_table[3] = { 60,120,246 }; //mcs14 only be used in DUP mode when bw80/160/320
	u16 n_sd = 0, n_sd_short = 0;
	bool is_dcm_valid = in->dcm ? (in->mcs == 14 || in->mcs == 15) : (in->mcs != 14 && in->mcs != 15);		//dcm is only used in MCS14/15
	bool is_mcs14_valid = in->mcs == 14 ? (in->ru_size == 5 || in->ru_size == 6 || in->ru_size == 8) : true;//mcs14 is only used in DUP mode when bw80/160/320  
	bool is_mcs15_valid = in->mcs == 15 ? (in->ru_size != 14 && in->ru_size != 16) : true;					//mcs15 doesn't support RU996X2_484 & RU996X3_484
	out->valid = ((in->nss > 0 && in->nss <= 8) && is_dcm_valid && (!in->dcm || in->nss == 1) && (in->fec == LDPC || ((in->mcs <= 9 || in->mcs <= 15) && in->ru_size <= RU242)) && (in->fec == LDPC || in->nss <= 4) && is_mcs14_valid && is_mcs15_valid);
	if (!out->valid) {
		//rtw_error("invalid mcs input");
		return false;
	}
	out->code_rate = code_rate_table[in->mcs];
	n_sd = in->mcs == 14 ? n_sd_dup_table[in->ru_size==5 ? 0 :(in->ru_size==6 ? 1:2)] : n_sd_table[in->ru_size];
	n_sd_short = in->mcs == 14 ? n_sd_dup_short_table[in->ru_size == 5 ? 0 : (in->ru_size == 6 ? 1 : 2)] : n_sd_short_table[in->dcm][in->ru_size];
	halbb_com_par_cal(bb, n_sd, code_rate_table[in->mcs],
		n_bpscs_table[in->mcs], in->ru_size == HESIGB ? 1 : in->nss,
		in->dcm, out);
	out->n_es = in->fec == BCC || in->ru_size == HESIGB;
	halbb_com_par_cal(bb, n_sd_short, code_rate_table[in->mcs], n_bpscs_table[in->mcs], in->nss, false, &out_temp);
	out->he_n_cbps_short = out_temp.n_cbps;
	out->he_n_dbps_short = out_temp.n_dbps;
	out->fec = in->fec || in->ru_size == HESIGB;
	out->dcm = in->dcm;
	out->nss = in->ru_size == HESIGB ? 1 : in->nss;

	return true;
}

enum plcp_sts halbb_mcs_table(struct bb_info *bb, const struct plcp_mcs_table_in_t *in, struct plcp_mcs_table_out_t *out)
{
	switch ((enum spec_list)(in->spec_idx)) {
		case SPEC_LEGACY:
			if (!halbb_legacy_mcs_table(bb, in, out))
				return OFDM_INVALID;
			break;
		case SPEC_HT:
			if (!halbb_ht_mcs_table(bb, in, out))
				return HT_INVALID;
			break;
		case SPEC_VHT:
			if (!halbb_vht_mcs_table(bb, in, out))
				return VHT_INVALID;
			break;
		case SPEC_HE:
			if (!halbb_he_mcs_table(bb, in, out))
				return HE_INVALID;
			break;
		case SPEC_EHT:
			if (!halbb_eht_mcs_table(bb, in, out))
				return HE_INVALID;
			break;
		default:
			out->valid = false;
			return SPEC_INVALID;
			//break;
	}
	//rtw_assert(out->valid, "invalid spec");		
	return PLCP_SUCCESS;
}


void halbb_get_mcs_out(struct bb_info *bb, const struct plcp_tx_pre_fec_padding_setting_in_t *in, struct plcp_tx_pre_fec_padding_setting_par_t *par, struct plcp_tx_pre_fec_padding_setting_out_t *out, bool *valid){
	
	bool mu_usr_en[14] = { false,false,false,false,false,false,false,true,true,false,true,true,false,true };
	struct plcp_mcs_table_in_t mcs_in;
	enum spec_list spec;
	u32 u = 0;

	if (mu_usr_en[in->format_idx])
		par->com.n_usr_refine = in->n_user;
	else
		par->com.n_usr_refine = 1;

	spec = halbb_format2spec((enum packet_format_t)(in->format_idx), valid);
	if (!(*valid)){
		//rtw_error("invalid spec");
		return;
	}

	par->com.spec_idx = spec;
	mcs_in.spec_idx = (u8)spec;
	for (u = 0; u < par->com.n_usr_refine; u++){
		mcs_in.bw = in->dbw; 
		mcs_in.dcm = in->usr[u].dcm;
		mcs_in.fec = in->usr[u].fec;
		mcs_in.mcs = in->usr[u].mcs;
		mcs_in.nss = in->usr[u].nss;

		if (bb->bb_80211spec == BB_AX_IC) { // AX series
			if (par->com.spec_idx == SPEC_HE) {
				if (in->format_idx == HE_TB_FMT || in->format_idx == HE_MU_FMT)
					mcs_in.ru_size = in->usr[u].ru_size_idx;
				else if (in->format_idx == HE_ER_SU_FMT)
					mcs_in.ru_size = (u8)(3 - in->he_er_u106ru_en);
				else
					mcs_in.ru_size = in->dbw + 3;
			} else {
				mcs_in.ru_size = 0;
			}
		} else { // BE series
			if ((par->com.spec_idx == SPEC_EHT) || (par->com.spec_idx == SPEC_HE)) {
				if ((in->format_idx == HE_ER_SU_FMT) && (par->com.spec_idx == SPEC_HE))
					mcs_in.ru_size = (u8)(3 - in->he_er_u106ru_en);
				else
					mcs_in.ru_size = (u8)out->usr[u].ru_size;
			} else {
				mcs_in.ru_size = 0;
			}
		}

		par->usr[u].ru_size_refine = mcs_in.ru_size;
		out->plcp_valid = halbb_mcs_table(bb, &mcs_in, &par->usr_mcs_out[u]);
		out->usr[u].mcs_valid = par->usr_mcs_out[u].valid;
		if (!par->usr_mcs_out[u].valid){
			*valid = false;
			return;
		}
		out->usr[u].ru_size = mcs_in.ru_size;
		out->usr[u].dcm = par->usr_mcs_out[u].dcm;
		out->usr[u].fec = par->usr_mcs_out[u].fec;
		out->usr[u].nss = par->usr_mcs_out[u].nss;
	}
}

void halbb_get_nsym_init(struct bb_info *bb, const struct plcp_tx_pre_fec_padding_setting_in_t *in, struct plcp_tx_pre_fec_padding_setting_par_t *par)
{
	u8 n_tail = 6;
	u8 n_service = 16;
	u32 u_max = 0;
	u32 val_max = 0;
	u32 comp_val;
	u32 u = 0;

	for (u = 0; u < par->com.n_usr_refine; u++){
		par->usr[u].n_excess = halbb_mod((par->usr[u].apep_refine << 3) + par->usr_mcs_out[u].n_es * n_tail + n_service, par->usr_mcs_out[u].n_dbps << par->com.stbc);
		if (par->usr[u].n_excess == 0 || (par->com.spec_idx != SPEC_HE && par->com.spec_idx != SPEC_EHT))
			par->usr[u].pre_fec_padding_factor_init = 4;
		else
			par->usr[u].pre_fec_padding_factor_init = halbb_min(halbb_ceil(par->usr[u].n_excess, par->usr_mcs_out[u].he_n_dbps_short << par->com.stbc), 4);

		par->usr[u].n_sym_init = halbb_ceil((par->usr[u].apep_refine << 3) + n_service + n_tail * par->usr_mcs_out[u].n_es, par->usr_mcs_out[u].n_dbps << par->com.stbc) << par->com.stbc;
	
		comp_val = ((par->usr[u].n_sym_init - par->com.m_stbc) << 2)  + (par->usr[u].pre_fec_padding_factor_init << par->com.stbc);
		if (comp_val >= val_max) {
			val_max = comp_val;
			u_max = u;
		}
	}

	par->com.n_sym_init = (u16)par->usr[u_max].n_sym_init;
	par->com.pre_fec_padding_factor_init = (u16)par->usr[u_max].pre_fec_padding_factor_init;

	if (par->com.pre_fec_padding_factor_init < 4) {
		for (u = 0; u < par->com.n_usr_refine; u++){
			par->usr[u].n_dbps_last_init = par->com.pre_fec_padding_factor_init * par->usr_mcs_out[u].he_n_dbps_short;
			par->usr[u].n_cbps_last_init = par->com.pre_fec_padding_factor_init * par->usr_mcs_out[u].he_n_cbps_short;
		}
	} else {
		for (u = 0; u < in->n_user; u++) {
			par->usr[u].n_dbps_last_init = par->usr_mcs_out[u].n_dbps;
			par->usr[u].n_cbps_last_init = par->usr_mcs_out[u].n_cbps;
		}
	}

}

void halbb_get_nsym(struct bb_info *bb, const struct plcp_tx_pre_fec_padding_setting_in_t *in, const struct plcp_tx_pre_fec_padding_setting_par_t *par, struct plcp_tx_pre_fec_padding_setting_out_t *out)
{
	u8 n_service, n_tail;
	u8 t_pe_table[4][4] = {{0, 2, 4, 5}, {0, 0, 1, 2}, {0, 0, 2, 3}, {0, 1, 3, 4}};
	u32 n_pld, n_avbits, n_dpbs_last;
	u32 psdu = 0;
	u32 u = 0;

	if (!par->com.ndp_en) {
		n_service = 16;
		n_tail = 6;
		out->ldpc_extra = false;

		out->stbc = par->com.stbc;
		for (u = 0; u < par->com.n_usr_refine; u++) {
			out->usr[u].nsts = out->usr[u].nss << par->com.stbc;
			if (par->usr_mcs_out[u].fec == LDPC) {	
				if (par->com.spec_idx == SPEC_HT)
					n_pld = (par->usr[u].apep_refine << 3) + n_service;
				else
					n_pld = (par->com.n_sym_init - par->com.m_stbc) * par->usr_mcs_out[u].n_dbps + (par->usr[u].n_dbps_last_init << par->com.stbc);

				n_avbits = (par->com.n_sym_init - par->com.m_stbc) * par->usr_mcs_out[u].n_cbps + (par->usr[u].n_cbps_last_init << par->com.stbc);
				out->ldpc_extra = ldpc_extra_check(n_avbits, n_pld, (u8)par->usr_mcs_out[u].code_rate);
				if (out->ldpc_extra > 0)
					break;
			}
		}

		out->doppler_en = par->com.doppler_mode > 0;
		out->midamble = out->doppler_en ? par->com.doppler_mode - 1 : 0;

		if (out->ldpc_extra > 0 || (par->com.tb_trig && par->com.tb_ldpc_extra)) {
			if (par->com.pre_fec_padding_factor_init == 4) {
				out->n_sym = par->com.n_sym_init + par->com.m_stbc;
				out->pre_fec_padding_factor = (par->com.spec_idx == SPEC_HE || par->com.spec_idx == SPEC_EHT) ? 1 : 0;
			} else {
				out->n_sym = par->com.n_sym_init;
				out->pre_fec_padding_factor = (par->com.spec_idx == SPEC_HE || par->com.spec_idx == SPEC_EHT) ? halbb_mod(par->com.pre_fec_padding_factor_init + 1, 4) : 0;
			}
		} else {
			out->n_sym = par->com.n_sym_init;
			out->pre_fec_padding_factor = (par->com.spec_idx == SPEC_HE || par->com.spec_idx == SPEC_EHT) ? halbb_mod(par->com.pre_fec_padding_factor_init, 4) : 0;
		}

		for (u = 0; u < par->com.n_usr_refine; u++) {
			//u32 n_pld(0), psdu(0);
			if (par->com.spec_idx == SPEC_HT && par->usr_mcs_out[u].fec == LDPC) {
				n_pld = (in->usr[u].apep << 3) + n_service;
			} else if (par->usr_mcs_out[u].fec == LDPC) {
				n_pld = (par->com.n_sym_init - par->com.m_stbc) * par->usr_mcs_out[u].n_dbps + (par->usr[u].n_dbps_last_init << par->com.stbc);
			} else {
				n_dpbs_last = out->pre_fec_padding_factor == 0 ? par->usr_mcs_out[u].n_dbps : par->usr_mcs_out[u].he_n_dbps_short * out->pre_fec_padding_factor;
				n_pld = (out->n_sym - par->com.m_stbc) * par->usr_mcs_out[u].n_dbps + (n_dpbs_last << par->com.stbc);
			}

			psdu = (n_pld - n_service - n_tail * par->usr_mcs_out[u].n_es) >> 3;
			//out->usr[u]->eof_padding_length = max(0,psdu - ceil(par->usr[u]->apep_refine, 4) << 2);
			if (par->com.spec_idx == SPEC_LEGACY || (par->com.spec_idx == SPEC_HT && par->usr[u].n_mpdu_refine == 1)) {
				out->usr[u].eof_padding_length = 0;
			} else {
				out->usr[u].eof_padding_length = (psdu > (halbb_ceil(par->usr[u].apep_refine, 4) << 2)) ? (psdu - (halbb_ceil(par->usr[u].apep_refine, 4) << 2)) : 0;
			}
			out->usr[u].nsts = par->usr_mcs_out[u].nss << par->com.stbc;
			out->usr[u].apep_len = par->usr[u].apep_refine;
			out->usr[u].mpdu_length_byte = par->usr[u].mpdu_length_byte_refine;
			out->usr[u].n_mpdu = par->usr[u].n_mpdu_refine;
		}

		if (par->com.tb_trig)
			out->t_pe = par->com.tb_trig_t_pe;
		else
			out->t_pe = (par->com.spec_idx == SPEC_HE || par->com.spec_idx == SPEC_EHT) ? t_pe_table[out->pre_fec_padding_factor][in->nominal_t_pe] : 0;
		
	} else {
		out->ldpc_extra = false;
		out->t_pe = par->com.spec_idx == SPEC_HE ? 1 : (par->com.spec_idx == SPEC_EHT ? (in->dbw <= 3 && in->usr[0].nss <= 8 ? 1 : 2) : 0);
		out->stbc = par->com.stbc;
		for (u = 0; u < par->com.n_usr_refine; u++) {			
			out->usr[u].eof_padding_length = 0;	
			out->usr[u].nsts = par->usr_mcs_out[u].nss << par->com.stbc;
			out->usr[u].apep_len = par->usr[u].apep_refine;
			out->usr[u].mpdu_length_byte = par->usr[u].mpdu_length_byte_refine;
			out->usr[u].n_mpdu = par->usr[u].n_mpdu_refine;
		}
		out->n_sym = 0;
		out->pre_fec_padding_factor = 0;
	}
	if (par->com.spec_idx == SPEC_HE)
		out->n_sym_hesigb = par->com.n_hesigb_sym;
	else if (par->com.spec_idx == SPEC_EHT)
		out->n_sym_ehtsig = par->com.n_hesigb_sym;
}

void halbb_get_txtime(struct bb_info *bb, const struct plcp_tx_pre_fec_padding_setting_in_t *in, struct plcp_tx_pre_fec_padding_setting_par_t *par, struct plcp_tx_pre_fec_padding_setting_out_t *out)
{
	struct bb_h2c_fw_tx_setting *fw_tx_i = &bb->bb_fwtx_h2c_i;
	//n_ma, m_ma
	u8 m_table[14] = {0,0,0,0,0,2,1,1,2,0,0,0,0,2};
	u32 l_len_temp;
	
	if (par->com.doppler_mode > 0)
		par->com.n_ma =  halbb_ceil(out->n_sym - 1, par->com.m_ma) > 1 ? halbb_ceil(out->n_sym - 1, par->com.m_ma) - 1 : 0;
	else
		par->com.n_ma = 0;
	
	out->tx_time_0p4us = par->com.preamble_0p4us + par->com.n_ma * par->com.n_ltf_sym * par->com.t_ltf_sym_0p4us + out->n_sym * par->com.t_sym_0p4us + out->t_pe * 10;

	if (par->com.spec_idx == SPEC_LEGACY) {
		out->l_len = (u16)par->usr[0].apep_refine;
	} else {
		l_len_temp = halbb_ceil(out->tx_time_0p4us - 50, 10) * 3 - 3 - m_table[in->format_idx];
		if (par->com.spec_idx == SPEC_HT)
			out->l_len = (u16)halbb_max(l_len_temp,in->ht_l_len);
		else
			out->l_len = (u16)l_len_temp;
	}

	if (par->com.spec_idx == SPEC_VHT)
		out->disamb = par->com.gi == 0 ? halbb_mod(out->n_sym, 10) == 9 : 0;
	else if (par->com.spec_idx == SPEC_HE || par->com.spec_idx == SPEC_EHT)
		out->disamb = (out->t_pe * 10 + halbb_mod(10 - halbb_mod(out->tx_time_0p4us, 10), 10)) >= par->com.t_sym_0p4us;
	else
		out->disamb = 0;
	out->ndp = par->com.ndp_en;
	out->n_ltf = par->com.n_ltf_sym > 0 ? (u16)(par->com.n_ltf_sym - 1) : 0;
	out->valid = out->tx_time_0p4us <= 13710 || in->format_idx == HT_GF_FMT;
	fw_tx_i->tx_time[0]= (u8) (out->tx_time_0p4us & 0x000000ff); // for FW trigger specific duty cycle used
	fw_tx_i->tx_time[1]= (u8) ((out->tx_time_0p4us & 0x0000ff00) >> 8);
	fw_tx_i->tx_time[2]= (u8) ((out->tx_time_0p4us & 0x00ff0000) >> 16);
	fw_tx_i->tx_time[3]= (u8) ((out->tx_time_0p4us & 0xff000000) >> 24);
	//rtw_assert(out->valid,"txtime exceeds");
	if (!out->valid)
		out->plcp_valid = LENGTH_EXCEED;
	out->n_usr = par->com.n_usr_refine;
	out->gi = par->com.gi;
}

void halbb_refine_input(struct bb_info *bb, const struct plcp_tx_pre_fec_padding_setting_in_t *in, struct plcp_tx_pre_fec_padding_setting_par_t *par)
{
	bool can_find = false , disam;
	u8 n_tail = 6;
	u8 n_service = 24;
	u8 remain_time_0p4us;
	u32 n_mpdu = 0, mpdu_length = 0, apep, tmp, max_tx_time_0p4us, n_dbps_last, n_pld, n_psdu;
	bool ndp_en_table[14] = {false,false,true,true,true,true,false,false,false,true,false,false,false,false };
	u8 n_ltf_table[9] = { 0, 1, 2, 4, 4, 6, 6, 8, 8 };
	u8 t_fft_0p4us[2] = { 8, 32 };
	u8 must_preamble_0p4us_table[14] = { 0, 50, 80, 50, 90, 90, 110, 90, 100, 90, 90, 90, 110 ,100};
	u8 m_ma_table[3] = { 0, 10, 20 };
	u32 u = 0;
	u8 spec_idx = 0;
	
	spec_idx = (u8)par->com.spec_idx;

	if (par->com.spec_idx > SPEC_LEGACY && par->com.spec_idx != SPEC_EHT)
		par->com.stbc = in->stbc;
	else
		par->com.stbc = false;

	par->com.m_stbc = par->com.stbc + 1;
	if (par->com.spec_idx == SPEC_HE)
		par->com.doppler_mode = in->doppler_mode;
	else
		par->com.doppler_mode = 0;

	par->com.n_sts_max = 0;
	for (u = 0; u < par->com.n_usr_refine; u++)
		par->com.n_sts_max = halbb_max(par->com.n_sts_max, par->usr_mcs_out[u].nss << par->com.stbc);

	if ((in->format_idx == HE_TB_FMT || in->format_idx == EHT_TB_FMT) && in->mode_idx == 3)
		par->com.n_ltf_sym = n_ltf_table[in->tb_ru_tot_sts_max+1];
	else
		par->com.n_ltf_sym = (par->com.spec_idx == SPEC_LEGACY ? 0 : n_ltf_table[par->com.n_sts_max]) + n_ltf_table[par->com.spec_idx == SPEC_HT ? in->ness : 0];

	if (in->format_idx == HE_MU_FMT || in->format_idx == EHT_MU_SU_FMT || in->format_idx == EHT_MU_MU_FMT || in->format_idx == EHT_MU_RU_FMT || in->format_idx == EHT_MU_ERSU_FMT)
		par->com.n_hesigb_sym = in->n_hesigb_sym; 
	else
		par->com.n_hesigb_sym = 0;
	//t_sym
	if (par->com.spec_idx == SPEC_LEGACY)
		par->com.gi = 1;
	else if (par->com.spec_idx == SPEC_HT || par->com.spec_idx == SPEC_VHT)
		par->com.gi = in->gi > 0;
	else
		par->com.gi = (u8)halbb_max(1,in->gi);

	
	par->com.t_sym_0p4us = t_fft_0p4us[par->com.spec_idx >= SPEC_HE] + (1 << par->com.gi);
	if (par->com.spec_idx == SPEC_HE || par->com.spec_idx == SPEC_EHT)
		par->com.t_ltf_sym_0p4us = (8 << halbb_min(in->ltf_type, 2)) + (1 << par->com.gi);
	else
		par->com.t_ltf_sym_0p4us = 10;

	
	par->com.preamble_0p4us = must_preamble_0p4us_table[in->format_idx] + (u16)par->com.n_ltf_sym * (u16)par->com.t_ltf_sym_0p4us + (u16)par->com.n_hesigb_sym * 10;//?????????????????????????

	
	par->com.m_ma = m_ma_table[par->com.doppler_mode];

	
	par->com.ndp_en = ndp_en_table[in->format_idx] && in->ndp;
	par->com.tb_trig = (in->mode_idx == 3 && (in->format_idx == HE_TB_FMT || in->format_idx == EHT_TB_FMT));
	if (par->com.ndp_en) {
		apep = 0;
		for (u = 0; u < par->com.n_usr_refine; u++) {
			par->usr[u].n_mpdu_refine = 1;
			par->usr[u].mpdu_length_byte_refine = apep;
			par->usr[u].apep_refine = apep;
		}
	} else if (in->mode_idx == 0){//apep
		for (u = 0; u < par->com.n_usr_refine; u++) {
			apep = in->usr[u].apep;
			if (par->com.spec_idx == SPEC_LEGACY || (par->com.spec_idx == SPEC_HT && apep < (1 << 14))) {
				par->usr[u].n_mpdu_refine = 1;
				par->usr[u].mpdu_length_byte_refine = apep;
				par->usr[u].apep_refine = apep;
			} else{
				halbb_find_apep(&apep,&can_find,&n_mpdu,&mpdu_length,spec_idx);
				while (!can_find){
					apep = apep - 1;
					halbb_find_apep(&apep, &can_find, &n_mpdu, &mpdu_length,spec_idx);
				}
				par->usr[u].n_mpdu_refine = n_mpdu;
				par->usr[u].mpdu_length_byte_refine = mpdu_length;
				par->usr[u].apep_refine = apep;
			}
		}
	} else if(in->mode_idx == 1 || par->com.tb_trig) { // max_tx_time
		disam = (par->com.spec_idx == SPEC_HE || par->com.spec_idx == SPEC_EHT) ? in->nominal_t_pe == 2 : false;
		disam = par->com.tb_trig ? (bool)in->tb_disam : disam;

		/*========[ return BSOD ]========*/
//		if ((par->com.n_ltf_sym * par->com.t_ltf_sym_0p4us + par->com.m_ma * par->com.t_sym_0p4us) == 0)
//			return;


		max_tx_time_0p4us = par->com.tb_trig ? halbb_ceil(in->tb_l_len + 3 + 2,3)*10 : in->max_tx_time_0p4us;
		tmp = (max_tx_time_0p4us > par->com.preamble_0p4us + (disam + 2) * par->com.t_sym_0p4us ) && par->com.doppler_mode > 0  ? (in->max_tx_time_0p4us - par->com.preamble_0p4us - (disam + 2) * par->com.t_sym_0p4us) / (par->com.n_ltf_sym * par->com.t_ltf_sym_0p4us + par->com.m_ma * par->com.t_sym_0p4us) : 0;
		par->com.n_ma = par->com.doppler_mode > 0 ? tmp : 0;
		par->com.n_sym_init = ((u16)max_tx_time_0p4us - par->com.preamble_0p4us - ((u16)par->com.n_ma * (u16)par->com.n_ltf_sym * (u16)par->com.t_ltf_sym_0p4us)) / (u16)par->com.t_sym_0p4us - disam; // Look

		if (par->com.stbc)
			par->com.n_sym_init = (u16)halbb_ceil(par->com.n_sym_init, 2) << 1;

		remain_time_0p4us = (u8)(max_tx_time_0p4us - par->com.preamble_0p4us - par->com.n_sym_init * par->com.t_sym_0p4us - par->com.n_ma * par->com.n_ltf_sym * par->com.t_ltf_sym_0p4us);//????????????????????????????????? false?

		if ((par->com.spec_idx == SPEC_EHT) && !par->com.tb_trig) {
			if (remain_time_0p4us >= 50) {
				par->com.pre_fec_padding_factor_init = 4;
			}
			else if (remain_time_0p4us >= 40) {
				if (in->nominal_t_pe == 3)
					par->com.pre_fec_padding_factor_init = 3;
				else
					par->com.pre_fec_padding_factor_init = 4;
			}
			else if (remain_time_0p4us >= 30) {
				if (in->nominal_t_pe == 3)
					par->com.pre_fec_padding_factor_init = 2;
				else if (in->nominal_t_pe == 2)
					par->com.pre_fec_padding_factor_init = 3;
				else
					par->com.pre_fec_padding_factor_init = 4;
			}
			else if (remain_time_0p4us >= 20) {
				if (in->nominal_t_pe == 3)
					par->com.pre_fec_padding_factor_init = 1;
				else if (in->nominal_t_pe == 2)
					par->com.pre_fec_padding_factor_init = 2;
				else
					par->com.pre_fec_padding_factor_init = 4;
			}
			else if (remain_time_0p4us >= 10) {
				if (in->nominal_t_pe == 3)
					par->com.pre_fec_padding_factor_init = 1;
				else if (in->nominal_t_pe == 2)
					par->com.pre_fec_padding_factor_init = 1;
				else if (in->nominal_t_pe == 1)
					par->com.pre_fec_padding_factor_init = 3;
				else
					par->com.pre_fec_padding_factor_init = 4;
			}
			else if (remain_time_0p4us >= 0) {
				par->com.pre_fec_padding_factor_init = 1;
			}
		}
		else if ((par->com.spec_idx == SPEC_HE) && !par->com.tb_trig) {
			if (remain_time_0p4us >= 40) {
				par->com.pre_fec_padding_factor_init = 4;
			} else if (remain_time_0p4us >= 30) {
				if (in->nominal_t_pe == 2)
					par->com.pre_fec_padding_factor_init = 3;
				else
					par->com.pre_fec_padding_factor_init = 4;
			} else if (remain_time_0p4us >= 20) {
				if (in->nominal_t_pe == 2)
					par->com.pre_fec_padding_factor_init = 2;
				else
					par->com.pre_fec_padding_factor_init = 4;
			} else if (remain_time_0p4us >= 10) {
				if (in->nominal_t_pe == 2)
					par->com.pre_fec_padding_factor_init = 1;
				else if (in->nominal_t_pe == 1)
					par->com.pre_fec_padding_factor_init = 3;
				else
					par->com.pre_fec_padding_factor_init = 4;
			} else if (remain_time_0p4us >= 0) {
				if (in->nominal_t_pe == 2)
					par->com.pre_fec_padding_factor_init = 1;
				else if (in->nominal_t_pe == 1)
					par->com.pre_fec_padding_factor_init = 1;
				else
					par->com.pre_fec_padding_factor_init = 1;
			}
		} else if(!par->com.tb_trig) {
			par->com.pre_fec_padding_factor_init = 4;
		} else {
			//tb_tri_en
			par->com.tb_trig_t_pe = (max_tx_time_0p4us - par->com.preamble_0p4us - par->com.n_sym_init * par->com.t_sym_0p4us - par->com.n_ma * par->com.n_ltf_sym * par->com.t_ltf_sym_0p4us)/10;
			par->com.tb_ldpc_extra = in->tb_ldpc_extra;
			if (par->com.tb_ldpc_extra) {
				if (in->tb_pre_fec_padding_factor == 1) {
					par->com.n_sym_init = par->com.n_sym_init - 1;
					par->com.pre_fec_padding_factor_init = 4;
				} else {
					par->com.n_sym_init = par->com.n_sym_init;
					par->com.pre_fec_padding_factor_init = (u16)in->tb_pre_fec_padding_factor - 1;
				}
			} else {
				par->com.n_sym_init = par->com.n_sym_init;
				par->com.pre_fec_padding_factor_init = in->tb_pre_fec_padding_factor == 0 ? 4 : (u16)in->tb_pre_fec_padding_factor;
			}
		}
		
		for (u = 0; u < par->com.n_usr_refine; u++) {
			n_dbps_last = par->com.pre_fec_padding_factor_init == 4 ? par->usr_mcs_out[u].n_dbps : par->usr_mcs_out[u].he_n_dbps_short * par->com.pre_fec_padding_factor_init;
			n_pld = (par->com.n_sym_init - par->com.m_stbc) * par->usr_mcs_out[u].n_dbps + (n_dbps_last << par->com.stbc);
			n_psdu = (n_pld - n_tail * par->usr_mcs_out[u].n_es - n_service) >> 3;
			apep = n_psdu;
			if (par->com.spec_idx == SPEC_LEGACY || (par->com.spec_idx == SPEC_HT && apep < (1 << 14))) {
				par->usr[u].n_mpdu_refine = 1;
				par->usr[u].mpdu_length_byte_refine = apep;
				par->usr[u].apep_refine = apep;
			} else {
				//can_find;
				//u32 n_mpdu, mpdu_length;
				halbb_find_apep(&apep, &can_find, &n_mpdu, &mpdu_length,spec_idx);
				while (!can_find) {
					apep = apep - 1;
					halbb_find_apep(&apep, &can_find, &n_mpdu, &mpdu_length,spec_idx);
				}
				par->usr[u].n_mpdu_refine = n_mpdu;
				par->usr[u].mpdu_length_byte_refine = mpdu_length;
				par->usr[u].apep_refine = apep;
			}
		}			
	} else { //n_mpdu,mpdu_len
		for (u = 0; u < par->com.n_usr_refine; u++) {
			par->usr[u].n_mpdu_refine = par->com.spec_idx > SPEC_LEGACY ? in->usr[u].n_mpdu : 1;
			par->usr[u].mpdu_length_byte_refine = in->usr[u].mpdu_length_byte;

			if (par->com.spec_idx == SPEC_LEGACY || (par->com.spec_idx == SPEC_HT && par->usr[u].n_mpdu_refine == 1))
				par->usr[u].apep_refine = par->usr[u].mpdu_length_byte_refine;
			else
				par->usr[u].apep_refine = (par->usr[u].n_mpdu_refine - 1)*(4 + 4 * halbb_ceil(par->usr[u].mpdu_length_byte_refine, 4)) + (4 + par->usr[u].mpdu_length_byte_refine);
		}			
	}
}

enum plcp_sts halbb_tx_plcp_cal(struct bb_info *bb, const struct plcp_tx_pre_fec_padding_setting_in_t *in, struct plcp_tx_pre_fec_padding_setting_out_t *out)
{
	bool mcs_out_valid = false;
	struct plcp_tx_pre_fec_padding_setting_par_t par;

	BB_DBG(bb, DBG_PHY_CONFIG, "<====== %s ======>\n", __func__);

	halbb_get_mcs_out(bb, in, &par, out, &mcs_out_valid);
	if (!mcs_out_valid) {
		out->valid = false;
		return out->plcp_valid;
	}
	halbb_refine_input(bb, in, &par);
	halbb_get_nsym_init(bb, in, &par);
	halbb_get_nsym(bb, in, &par, out);
	halbb_get_txtime(bb, in, &par, out);
	return out->plcp_valid;
}