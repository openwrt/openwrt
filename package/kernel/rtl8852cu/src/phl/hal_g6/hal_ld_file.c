/******************************************************************************
 *
 * Copyright(c) 2019 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 *****************************************************************************/
#define _HAL_LD_FILE_C_
#include "hal_headers.h"
#include "hal_ld_file.h"

static int
_hal_parse_phyreg(void *drv_priv, void *para_info_t, u8 *psrc_buf, u32 buflen)
{
	struct rtw_para_info_t *para_info = (struct rtw_para_info_t *)para_info_t;
	u32 *pdest_buf = para_info->para_data;
	char	*sz_line, *ptmp;
	u32 dest_buf_idx = 0;
	u32	u4bregoffset, u4bregvalue, u4bmove;

	ptmp = (char *)psrc_buf;

	for (sz_line = hal_getLinefrombuffer(ptmp); sz_line != NULL; sz_line = hal_getLinefrombuffer(ptmp)) {
		if (!hal_is_comment_string(sz_line)) {
			/* Get 1st hex value as register offset. */
			if (hal_get_hexvalue_fromstring(sz_line, &u4bregoffset, &u4bmove)) {

				pdest_buf[dest_buf_idx] = u4bregoffset;
				dest_buf_idx++;

				if (u4bregoffset == 0xffff) {
					/* Ending. */
					pdest_buf[dest_buf_idx] = 0xffff;
					PHL_INFO(" dest_buf_len: %d \n", dest_buf_idx);
					break;
				}
				/* Get 2nd hex value as register value. */
				sz_line += u4bmove;
				if (hal_get_hexvalue_fromstring(sz_line, &u4bregvalue, &u4bmove)) {
					/* PHL_INFO("[BB-ADDR]%03lX=%08lX\n", u4bregoffset, u4bregvalue); */
					pdest_buf[dest_buf_idx] = u4bregvalue;
					dest_buf_idx++;
				}
			}
		}
	}
#if 0
	{ /*debug dump buf*/
		u32 i = 0;
		for (i = 0; i <= dest_buf_idx; i+=2)
			PHL_INFO("[BB-ADDR] 0x%x :[0x%x]\n", pdest_buf[i], pdest_buf[i+1]);
	}
#endif
	return dest_buf_idx;
}

static u32 phypg_combvalue_fromstring(		char *sz_line)
{
	u32	u4bmove;
	u32 combvalue = 0;
	u8	integer = 0, fraction = 0;
	u8 txgi_pdbm = 4;

	if (hal_get_fractionvalue_fromstring(sz_line, &integer, &fraction, &u4bmove))
		sz_line += u4bmove;
	else
		goto exit;

	combvalue |= ((integer * txgi_pdbm) + (fraction * txgi_pdbm / 100));

	if (hal_get_fractionvalue_fromstring(sz_line, &integer, &fraction, &u4bmove))
		sz_line += u4bmove;
	else
		goto exit;

	combvalue <<= 8;
	combvalue |= ((integer * txgi_pdbm) + (fraction * txgi_pdbm / 100));

	if (hal_get_fractionvalue_fromstring(sz_line, &integer, &fraction, &u4bmove))
		sz_line += u4bmove;
	else
		goto exit;

	combvalue <<= 8;
	combvalue |= ((integer * txgi_pdbm) + (fraction * txgi_pdbm / 100));

	if (hal_get_fractionvalue_fromstring(sz_line, &integer, &fraction, &u4bmove))
		sz_line += u4bmove;
	else
		goto exit;

	combvalue <<= 8;
	combvalue |= ((integer * txgi_pdbm) + (fraction * txgi_pdbm / 100));

	/*PHL_DBG("[combvalue 4] 0x%x , hex = %x\n", combvalue , integer);*/
exit:
	return combvalue;
}

static int
_hal_parse_txpwr_by_rate(void *drv_priv, void *para_info_t, u8 *psrc_buf, u32 buflen)
{
	struct rtw_para_info_t *para_info = (struct rtw_para_info_t *)para_info_t;
	u32 *pdest_buf = para_info->para_data;
	char	*sz_line, *ptmp;
	u8	band = 0;
	u8 phy_reg_pg_version = 0;
	u8 phy_reg_pg_value_type = 0;
	u32 pwrhexval = 0;
	u32 line_idx = 0;
	u32 buf_idx = 0;
	bool firstline = true;

	ptmp = (char*)psrc_buf;
	for (sz_line = hal_getLinefrombuffer(ptmp); sz_line != NULL; sz_line = hal_getLinefrombuffer(ptmp)) {
		if (hal_is_allspace_tab(sz_line, sizeof(*sz_line)))
			continue;

		if (!hal_is_comment_string(sz_line)) {
			if (firstline) {
				if (_os_strncmp(sz_line, "#[v2]", 5) == 0
					|| _os_strncmp(sz_line, "#[v3]", 5) == 0) {
					phy_reg_pg_version = sz_line[3] - '0'; /* store pg version*/
					PHL_INFO("phy_reg_pg_version %d\n", phy_reg_pg_version);
				} else {
					PHL_ERR("The format in PHY_REG_PG are invalid %s\n", sz_line);
					goto exit;
				}
				if (_os_strncmp(sz_line + 5, "[Exact]#", 8) == 0) {
					phy_reg_pg_value_type = 1; /* store PHY_REG_PG_EXACT_VALUE*/
					firstline = false;
					PHL_INFO("REG_PG_EXACT_VALUE\n");
					continue;
				} else {
					PHL_ERR("The values in PHY_REG_PG are invalid %s\n", sz_line);
					goto exit;
				}
			}
			if (_os_strncmp(sz_line , "#[2.4G]#", 8) == 0) {
					band = BAND_ON_24G;
					continue;
			}
			if (_os_strncmp(sz_line , "#[5G]#", 6) == 0) {
					band = BAND_ON_5G;
					continue;
			}
			if (_os_strncmp(sz_line , "#[6G]#", 6) == 0) {
					band = BAND_ON_6G;
					continue;
			}
			if ((_os_strncmp(sz_line , "#[START]#", 9) == 0) || (_os_strncmp(sz_line , "#[END]#", 9) == 0) ) {
					continue;
			}
			if (_os_strncmp(sz_line , "[Nss1]", 6) == 0) {
				line_idx = 0;
				pwrhexval = 0;

				pdest_buf[buf_idx++] = band;
				pdest_buf[buf_idx++] = PHYPG_RF1Tx;
				line_idx += 6;
				PHL_INFO("[Line]%s , line_idx %d \n", sz_line, line_idx);

				while (sz_line[line_idx] == ' ' || sz_line[line_idx] == '\0' || sz_line[line_idx] == '\t')
					++line_idx;

				sz_line += line_idx;
				if (_os_strncmp(sz_line , "11M_1M", 6) == 0) {
					pdest_buf[buf_idx++] = CCK_11M_1M;
					sz_line += 6;
					PHL_INFO("[Line]%s, line_idx %d \n", sz_line, line_idx);
					pwrhexval = phypg_combvalue_fromstring(sz_line);
					PHL_INFO("combine Value Get Pwr hex Val = %x\n", pwrhexval);
					pdest_buf[buf_idx++] = pwrhexval;
				} else if (_os_strncmp(sz_line , "18M_6M", 6) == 0) {
					pdest_buf[buf_idx++] = OFDM_18M_6M;
					sz_line += 6;
					PHL_INFO("[Line]%s, line_idx %d \n", sz_line, line_idx);
					pwrhexval = phypg_combvalue_fromstring(sz_line);
					PHL_INFO("combine Value Get Pwr hex Val = %x\n", pwrhexval);
					pdest_buf[buf_idx++] = pwrhexval;
				} else if (_os_strncmp(sz_line , "54M_24M", 7) == 0) {
					pdest_buf[buf_idx++] = OFDM_54M_24M;
					sz_line += 7;
					PHL_INFO("[Line]%s, line_idx %d \n", sz_line, line_idx);
					pwrhexval = phypg_combvalue_fromstring(sz_line);
					PHL_INFO("combine Value Get Pwr hex Val = %x\n", pwrhexval);
					pdest_buf[buf_idx++] = pwrhexval;
				} else if (_os_strncmp(sz_line , "MCS3_0", 6) == 0) {
					pdest_buf[buf_idx++] = HE1SS_MCS3_0;
					sz_line += 6;
					PHL_INFO("[Line]%s, line_idx %d \n", sz_line, line_idx);
					pwrhexval = phypg_combvalue_fromstring(sz_line);
					PHL_INFO("combine Value Get Pwr hex Val = %x\n", pwrhexval);
					pdest_buf[buf_idx++] = pwrhexval;
				} else if (_os_strncmp(sz_line , "MCS7_4", 6) == 0) {
					pdest_buf[buf_idx++] = HE1SS_MCS7_4;
					sz_line += 6;
					PHL_INFO("[Line]%s, line_idx %d \n", sz_line, line_idx);
					pwrhexval = phypg_combvalue_fromstring(sz_line);
					PHL_INFO("combine Value Get Pwr hex Val = %x\n", pwrhexval);
					pdest_buf[buf_idx++] = pwrhexval;
				} else if (_os_strncmp(sz_line , "MCS11_8", 6) == 0) {
					pdest_buf[buf_idx++] = HE1SS_MCS11_8;
					sz_line += 7;
					PHL_INFO("[Line]%s, line_idx %d \n", sz_line, line_idx);
					pwrhexval = phypg_combvalue_fromstring(sz_line);
					PHL_INFO("combine Value Get Pwr hex Val = %x\n", pwrhexval);
					pdest_buf[buf_idx++] = pwrhexval;
				} else if (_os_strncmp(sz_line , "DCM4_0", 6) == 0) {
					pdest_buf[buf_idx++] = HE1SS_DCM4_0;
					sz_line += 6;
					PHL_INFO("[Line]%s, line_idx %d \n", sz_line, line_idx);
					pwrhexval = phypg_combvalue_fromstring(sz_line);
					PHL_INFO("combine Value Get Pwr hex Val = %x\n", pwrhexval);
					pdest_buf[buf_idx++] = pwrhexval;
				}
			} else if (_os_strncmp(sz_line , "[Nss2]", 6) == 0) {
				line_idx = 0;
				pwrhexval = 0;

				pdest_buf[buf_idx++] = band;
				pdest_buf[buf_idx++] = PHYPG_RF2Tx;
				PHL_INFO("[Line]%s , line_idx %d \n", sz_line, line_idx);

				line_idx += 6;
				while (sz_line[line_idx] == ' ' || sz_line[line_idx] == '\0' || sz_line[line_idx] == '\t')
					++line_idx;

				sz_line += line_idx;
				if (_os_strncmp(sz_line , "MCS3_0", 6) == 0) {
					pdest_buf[buf_idx++] = HE2SS_MCS3_0;
					sz_line += 6;
					PHL_INFO("[Line]%s, line_idx %d \n", sz_line, line_idx);
					pwrhexval = phypg_combvalue_fromstring(sz_line);
					PHL_INFO("combine Value Get Pwr hex Val = %x\n", pwrhexval);
					pdest_buf[buf_idx++] = pwrhexval;
				} else if (_os_strncmp(sz_line , "MCS7_4", 6) == 0) {
					pdest_buf[buf_idx++] = HE2SS_MCS7_4;
					sz_line += 6;
					PHL_INFO("[Line]%s, line_idx %d \n", sz_line, line_idx);
					pwrhexval = phypg_combvalue_fromstring(sz_line);
					PHL_INFO("combine Value Get Pwr hex Val = %x\n", pwrhexval);
					pdest_buf[buf_idx++] = pwrhexval;
				} else if (_os_strncmp(sz_line , "MCS11_8", 6) == 0) {
					pdest_buf[buf_idx++] = HE2SS_MCS11_8;
					sz_line += 7;
					PHL_INFO("[Line]%s, line_idx %d \n", sz_line, line_idx);
					pwrhexval = phypg_combvalue_fromstring(sz_line);
					PHL_INFO("combine Value Get Pwr hex Val = %x\n", pwrhexval);
					pdest_buf[buf_idx++] = pwrhexval;
				} else if (_os_strncmp(sz_line , "DCM4_0", 6) == 0) {
					pdest_buf[buf_idx++] = HE2SS_DCM4_0;
					sz_line += 6;
					PHL_INFO("[Line]%s, line_idx %d \n", sz_line, line_idx);
					pwrhexval = phypg_combvalue_fromstring(sz_line);
					PHL_INFO("combine Value Get Pwr hex Val = %x\n", pwrhexval);
					pdest_buf[buf_idx++] = pwrhexval;
				}
			} else if (_os_strncmp(sz_line , "[Offset]", 8) == 0) {
				line_idx = 0;
				pwrhexval = 0;

				pdest_buf[buf_idx++] = band;
				pdest_buf[buf_idx++] = PHYPG_OFFSET;
				PHL_INFO("[Line]%s , line_idx %d \n", sz_line, line_idx);

				line_idx += 8;
				while (sz_line[line_idx] == ' ' || sz_line[line_idx] == '\t' || sz_line[line_idx] == '\0')
					++line_idx;

				sz_line += line_idx;
				if (band == BAND_ON_24G) {
					if (_os_strncmp(sz_line , "AllRate2_1", 10) == 0) {
						pdest_buf[buf_idx++] = Legacy_AllRate2_1;
						sz_line += 10;
						PHL_INFO("[Line]%s, line_idx %d \n", sz_line, line_idx);
						pwrhexval = phypg_combvalue_fromstring(sz_line);
						PHL_INFO("combine Value Get Pwr hex Val = %x\n", pwrhexval);
						pdest_buf[buf_idx++] = pwrhexval;
					}
					else if (_os_strncmp(sz_line , "AllRate2_2", 10) == 0) {
						pdest_buf[buf_idx++] = HE_AllRate2_2;
						sz_line += 10;
						PHL_INFO("[Line]%s, line_idx %d \n", sz_line, line_idx);
						pwrhexval = phypg_combvalue_fromstring(sz_line);
						PHL_INFO("combine Value Get Pwr hex Val = %x\n", pwrhexval);
						pdest_buf[buf_idx++] = pwrhexval;
					}
				}
				if (band == BAND_ON_5G) {
					if (_os_strncmp(sz_line , "AllRate5_1", 10) == 0) {
						pdest_buf[buf_idx++] = OFDM_AllRate5_1; /* 5G band all rate only OFDM upper */
						sz_line += 10;
						PHL_INFO("[Line]%s, line_idx %d \n", sz_line, line_idx);
						pwrhexval = phypg_combvalue_fromstring(sz_line);
						PHL_INFO("combine Value Get Pwr hex Val = %x\n", pwrhexval);
						pdest_buf[buf_idx++] = pwrhexval;
					}
				}
				if (band == BAND_ON_6G) {
					if (_os_strncmp(sz_line , "AllRate6_1", 10) == 0) {
						pdest_buf[buf_idx++] = OFDM_AllRate6_1; /* 6G band all rate only OFDM upper */
						sz_line += 10;
						PHL_INFO("[Line]%s, line_idx %d \n", sz_line, line_idx);
						pwrhexval = phypg_combvalue_fromstring(sz_line);
						PHL_INFO("combine Value Get Pwr hex Val = %x\n", pwrhexval);
						pdest_buf[buf_idx++] = pwrhexval;
					}
				}
			}
		}
	}
	return buf_idx;
exit:
#if 0 /*dbg dump*/
	{
		u32 i = 0 ;
		char *strbuf , *pextra;
		strbuf = _os_mem_alloc(drv_priv, buf_idx  * sizeof(u32));
		pextra = strbuf;

		sprintf(strbuf, "\n");
		pextra = strbuf + _os_strlen(strbuf);
		for (i = 0; i <= buf_idx; i ++) {
			pextra += sprintf(pextra, " %x \t", pdest_buf[i]);
			if ( ((i+1)%4 == 0) && (i != 0))
				pextra += sprintf(pextra, "\n");
		}
		printk( "%s \n" , strbuf);
		_os_mem_free(drv_priv, strbuf, buf_idx  * sizeof(u32));
	}
#endif
	return 0;
}

static int
_hal_parse_radio(void *drv_priv, void *para_info_t, u8 *psrc_buf, u32 buflen)
{
	struct rtw_para_info_t *para_info = (struct rtw_para_info_t *)para_info_t;
	u32 *pdest_buf = para_info->para_data;
	char	*sz_line, *ptmp;
	u32 dest_buf_idx = 0;
	u32 u4bregoffset, u4bregvalue, u4bmove;

	ptmp = (char *)psrc_buf;

	for (sz_line = hal_getLinefrombuffer(ptmp); sz_line != NULL; sz_line = hal_getLinefrombuffer(ptmp)) {
		if (!hal_is_comment_string(sz_line)) {
			/* Get 1st hex value as register offset. */
			if (hal_get_hexvalue_fromstring(sz_line, &u4bregoffset, &u4bmove)) {

				pdest_buf[dest_buf_idx] = u4bregoffset;
				dest_buf_idx++;

				if (u4bregoffset == 0xffff) {
					/* Ending. */
					pdest_buf[dest_buf_idx] = 0xffff;
					PHL_INFO(" dest_buf_len: %d \n", dest_buf_idx);
					break;
				}
				/* Get 2nd hex value as register value. */
				sz_line += u4bmove;
				if (hal_get_hexvalue_fromstring(sz_line, &u4bregvalue, &u4bmove)) {
					/*PHL_INFO("[RF-ADDR]%03lx=%08lx\n", u4bregoffset, u4bregvalue);*/
					pdest_buf[dest_buf_idx] = u4bregvalue;
					dest_buf_idx++;
				}
			}
		}
	}
#if 0
	{ /*debug dump buf*/
		u32 i = 0;
		for (i = 0; i <= dest_buf_idx; i+=2)
			PHL_INFO("[Radio-ADDR] 0x%x :[0x%x]\n", pdest_buf[i], pdest_buf[i+1]);
	}
#endif
	return dest_buf_idx;
}

static int
hal_phy_find_ext_regd_num(struct rtw_para_pwrlmt_info_t *para_info,
			const char *regd_name)
{
	int i = 0;

	for (i = 0; i <= para_info->ext_regd_arridx ; i++) {
		if (_os_strcmp(regd_name, para_info->ext_regd_name[i]) == 0) {
			return i;
		}
	}
	return -1;
}

static void
_hal_add_ext_reg_codemap(void *d,
			struct rtw_para_pwrlmt_info_t *para_info,
			const char *country,
			u16 		domain,
			const char *regd_name,
			u32 		nlen)
{
	struct _hal_file_regd_ext *pregd_codemap;
	struct _hal_file_regd_ext *ent;

	if (!regd_name || !nlen) {
		PHL_ERR("regd_name || nlen Null\n");
		goto exit;
	}

	pregd_codemap = (struct _hal_file_regd_ext *)para_info->ext_reg_codemap;
	ent = &pregd_codemap[para_info->ext_reg_map_num];

	if (regd_name && _os_strlen((u8*)regd_name) < 10) {

		if (hal_phy_find_ext_regd_num(para_info, regd_name) == -1) {
			u8 idx = (u8)(para_info->ext_regd_arridx + 1);

			if (idx < regd_name_max_size) {
				_os_strcpy(para_info->ext_regd_name[idx], regd_name);
				para_info->ext_regd_arridx++;
				PHL_INFO("extrea reg [%d] = [%s]\n",
					idx, para_info->ext_regd_name[idx]);
			} else {
				PHL_ERR("extrea reg [%d] over size\n", idx);
				goto exit;
			}
		}

		_os_mem_cpy(d, ent->reg_name, (void*)regd_name, nlen);
		PHL_INFO("store reg_name = [%s]\n", ent->reg_name);
	} else {
		PHL_ERR("reg_name = [%s]\n", ent->reg_name);
		goto exit;
	}

	if (domain != 0)
		ent->domain = domain;
	else
		ent->domain = 0xffff;
	PHL_INFO("Store DomainCode = [0x%x]\n", ent->domain);

	if (country) {
		_os_mem_cpy(d, ent->country, (void*)country, 2);
		PHL_INFO("Store country = [%c%c]\n", ent->country[0], ent->country[1]);
	} else
		_os_mem_set(d, ent->country, '\0', 2);

	para_info->ext_reg_map_num++;
exit:
	return;
}

/*
* @@Ver=2.0
* or
* @@DomainCode=0x28, Regulation=C6
* or
* @@CountryCode=GB, Regulation=C7
*/
static u8
parse_reg_exc_config(void *drv_priv,
		char *sz_line ,
		struct rtw_para_pwrlmt_info_t *para_info)
{
#define VER_PREFIX "Ver="
#define DOMAIN_PREFIX "DomainCode=0x"
#define COUNTRY_PREFIX "CountryCode="
#define REG_PREFIX "Regulation="

	const u8 ver_prefix_len = (u8)_os_strlen((u8*)VER_PREFIX);
	const u8 domain_prefix_len = (u8)_os_strlen((u8*)DOMAIN_PREFIX);
	const u8 country_prefix_len = (u8)_os_strlen((u8*)COUNTRY_PREFIX);
	const u8 reg_prefix_len = (u8)_os_strlen((u8*)REG_PREFIX);
	u32 i, i_val_s, i_val_e;
	u32 j;
	u16 domain = 0;
	char *country = NULL;
	u8 parse_reg = 0;

	if (sz_line[0] != '@' || sz_line[1] != '@')
		return false;

	i = 2;
	if (_os_strncmp(sz_line + i, VER_PREFIX, ver_prefix_len) == 0)
		; /* nothing to do */
	else if (_os_strncmp(sz_line + i, DOMAIN_PREFIX, domain_prefix_len) == 0) {
		/* get string after domain prefix to ',' */
		i += domain_prefix_len;
		i_val_s = i;
		while (sz_line[i] != ',') {
			if (sz_line[i] == '\0')
				return false;
			i++;
		}
		i_val_e = i;

		/* check if all hex */
		for (j = i_val_s; j < i_val_e; j++)
			if (hal_ishexdigit(sz_line[j]) == false)
				return false;

		/* get value from hex string */
		if (_os_sscanf(sz_line + i_val_s, "%hx", &domain) != 1)
			return false;

		parse_reg = 1;
	} else if (_os_strncmp(sz_line + i, COUNTRY_PREFIX, country_prefix_len) == 0) {
		/* get string after country prefix to ',' */
		i += country_prefix_len;
		i_val_s = i;
		while (sz_line[i] != ',') {
			if (sz_line[i] == '\0')
				return false;
			i++;
		}
		i_val_e = i;

		if (i_val_e - i_val_s != 2)
			return false;

		/* check if all alpha */
		for (j = i_val_s; j < i_val_e; j++)
			if (hal_is_alpha(sz_line[j]) == false)
				return false;

		country = sz_line + i_val_s;

		parse_reg = 1;

	} else
		return false;

	if (parse_reg) {
		/* move to 'R' */
		while (sz_line[i] != 'R') {
			if (sz_line[i] == '\0')
				return false;
			i++;
		}

		/* check if matching regulation prefix */
		if (_os_strncmp(sz_line + i, REG_PREFIX, reg_prefix_len) != 0)
			return false;

		/* get string after regulation prefix ending with space */
		i += reg_prefix_len;
		i_val_s = i;
		while (sz_line[i] != ' ' && sz_line[i] != '\t' && sz_line[i] != '\0')
			i++;

		if (i == i_val_s)
			return false;

		_hal_add_ext_reg_codemap(drv_priv, para_info,
					country, domain,
					sz_line + i_val_s, i - i_val_s);
	}

	return true;
}

static void hal_phy_store_tx_power_limit		(void *drv_priv,
		u8				*sregulation,
		u8				*sband,
		u8				*sbandwidth,
		u8				*sratesection,
		u8				*sbf,
		u8				*sntx,
		u8				*schannel,
		u8				*spwrlimit,
		u8				*spshape_idx,
		void *pstc_txpwr_lmt,
		struct rtw_para_pwrlmt_info_t *para_info)
{
	u8 band = 0, bandwidth = 0, ratesec = 0, channel = 0;
	u8 ntx_idx = 0 , bf = 0 , pshape_idx = 0;
	int regulation = -1;
	s8 powerlimit = 0;

	struct hal_txpwr_lmt_t *array_tc_8852a_txpwr_lmt = (struct hal_txpwr_lmt_t *)pstc_txpwr_lmt;

	if (hal_get_u1bint_fromstr_indec((char *)schannel, &channel) == false
		|| hal_get_s1bint_fromstr_indec((char *)spwrlimit, &powerlimit) == false) {
		PHL_INFO("Illegal index of power limit table [ch %s][val %s]\n",
			 (char *)schannel, (char *)spwrlimit);
		return;
	}

	if (spshape_idx != NULL) {
		if (hal_get_u1bint_fromstr_indec((char *)spshape_idx, &pshape_idx) == false) {
			PHL_INFO("Illegal index of pshape idx [val %s]\n", (char *)spshape_idx);
			return;
		}
	}

	if (_os_strncmp((char *)sratesection, (const char *)"CCK", 3) == 0)
		ratesec = _PW_LMT_RS_CCK;
	else if (_os_strncmp((char *)sratesection, (const char *)"OFDM", 4) == 0)
		ratesec = _PW_LMT_RS_OFDM;
	else if (_os_strncmp((char *)sratesection, (const char *)"HT", 2) == 0)
		ratesec = _PW_LMT_RS_HT;
	else if (_os_strncmp((char *)sratesection, (const char *)"VHT", 3) == 0)
		ratesec = _PW_LMT_RS_VHT;
	else if (_os_strncmp((char *)sratesection, (const char *)"HE", 2) == 0)
		ratesec = _PW_LMT_RS_HE;
	else {
		PHL_INFO("Wrong rate section:%s\n", (char *)sratesection);
		return;
	}

	if (_os_strncmp((char *)sntx, (const char *)"1T", 2) == 0)
		ntx_idx = _PW_LMT_PH_1T;
	else if (_os_strncmp((char *)sntx, (const char *)"2T", 2) == 0)
		ntx_idx = _PW_LMT_PH_2T;
	else if (_os_strncmp((char *)sntx, (const char *)"3T", 2) == 0)
		ntx_idx = _PW_LMT_PH_3T;
	else if (_os_strncmp((char *)sntx, (const char *)"4T", 2) == 0)
		ntx_idx = _PW_LMT_PH_4T;
	else {
		PHL_INFO("Wrong tx num:%s\n", (char *)sntx);
		return;
	}

	if (_os_strncmp((char *)sbandwidth, (const char *)"20M", 3) == 0)
		bandwidth = _PW_LMT_BW_20M;
	else if (_os_strncmp((char *)sbandwidth, (const char *)"40M", 3) == 0)
		bandwidth = _PW_LMT_BW_40M;
	else if (_os_strncmp((char *)sbandwidth, (const char *)"80M", 3) == 0)
		bandwidth = _PW_LMT_BW_80M;
	else if (_os_strncmp((char *)sbandwidth, (const char *)"160M", 4) == 0)
		bandwidth = _PW_LMT_BW_160M;
	else {
		PHL_INFO("unknown bandwidth: %s\n", (char *)sbandwidth);
		return;
	}

	if (_os_strncmp((char *)sband, (const char *)"2.4G", 4) == 0)
		band = _PW_LMT_BAND_2_4G;
	else if (_os_strncmp((char *)sband, (const char *)"5G", 2) == 0)
		band = _PW_LMT_BAND_5G;
	else if (_os_strncmp((char *)sband, (const char *)"6G", 2) == 0)
		band = _PW_LMT_BAND_6G;
	else {
		PHL_INFO("unknown band: %s\n", (char *)sband);
		return;
	}

	if (_os_strncmp((char *)sbf, (const char *)"Non-BF", 6) == 0 || _os_strncmp((char *)sbf, (const char *)"NA", 2) == 0)
		bf = _PW_LMT_NONBF;
	else if (_os_strncmp((char *)sbf, (const char *)"BF", 2) == 0)
		bf = _PW_LMT_BF;
	else {
		PHL_INFO("unknown BF: %s\n", (char *)sbf);
		return;
	}

	regulation = rtw_hal_rf_get_predefined_pw_lmt_regu_type_from_str((char *)sregulation);
	if (regulation == -1) {
		int regd_num = hal_phy_find_ext_regd_num(para_info, (char *)sregulation);

		if (regd_num != -1) {
			regulation = (u8)regd_num;
			PHL_INFO("new regulation num: %d\n", regulation);
		} else {
		PHL_INFO("unknown regulation: %s\n", (char *)sregulation);
		return;
	}
	}

	array_tc_8852a_txpwr_lmt->band = band;
	array_tc_8852a_txpwr_lmt->bw = bandwidth;
	array_tc_8852a_txpwr_lmt->ch = channel;
	array_tc_8852a_txpwr_lmt->rs = ratesec;
	array_tc_8852a_txpwr_lmt->ntx = ntx_idx;
	array_tc_8852a_txpwr_lmt->bf = bf;
	array_tc_8852a_txpwr_lmt->val = powerlimit;
	array_tc_8852a_txpwr_lmt->reg = (u8)regulation;
	array_tc_8852a_txpwr_lmt->tx_shap_idx = pshape_idx;

	PHL_INFO("Store of power limit table [regulation %d][band %d][bw %d]"\
		"[rate section %d][ntx %d][BF %d][chnl %d][val %d] [sphape_idx %d]\n",
		array_tc_8852a_txpwr_lmt->reg, array_tc_8852a_txpwr_lmt->band,
		array_tc_8852a_txpwr_lmt->bw , array_tc_8852a_txpwr_lmt->rs,
		array_tc_8852a_txpwr_lmt->ntx, array_tc_8852a_txpwr_lmt->bf,
		array_tc_8852a_txpwr_lmt->ch, array_tc_8852a_txpwr_lmt->val,
		array_tc_8852a_txpwr_lmt->tx_shap_idx);

}


static int
_hal_parse_txpwrlmt(void *drv_priv, void *para_info_t, u8 *psrc_buf, u32 buflen)
{
#define LD_STAGE_EXC_MAPPING	0
#define LD_STAGE_TAB_DEFINE		1
#define LD_STAGE_TAB_START		2
#define LD_STAGE_COLUMN_DEFINE	3
#define LD_STAGE_COLUMN_REG		4
#define LD_STAGE_CH_ROW			5
#define LD_STAGE_PSHAPE_NUM		6

	struct rtw_para_pwrlmt_info_t *para_info = (struct rtw_para_pwrlmt_info_t *)para_info_t;
	u32 *pdest_buf = para_info->para_data;
	char **regulation = NULL;
	char **pshape = NULL;
	char	*sz_line = NULL, *ptmp = NULL;
	char band[10], bandwidth[10], ratesection[10], ntx[10], colnumbuf[10], bf_type[10];
	u8	colnum = 0;
	u8	loadingstage = LD_STAGE_EXC_MAPPING;
	u32 struct_idx = 0;
	u32	i = 0, forcnt = 0;
	PHAL_TXPWR_LMT_T array_tc_8852a_txpwr_lmt;

	if (pdest_buf == NULL || psrc_buf == NULL) {
		PHL_INFO("%s, fail !!! NULL buf !!!\n", __func__);
		return 0;
	}

	array_tc_8852a_txpwr_lmt = (PHAL_TXPWR_LMT_T)pdest_buf;

	ptmp = (char *)psrc_buf;
	for (sz_line = hal_getLinefrombuffer(ptmp); sz_line != NULL; sz_line = hal_getLinefrombuffer(ptmp)) {
line_start:
		if (hal_is_allspace_tab(sz_line, sizeof(*sz_line)))
			continue;

		if (hal_is_comment_string(sz_line))
			continue;

		if (loadingstage == LD_STAGE_EXC_MAPPING) {
			if (sz_line[0] == '#' || sz_line[1] == '#') {
				loadingstage = LD_STAGE_TAB_DEFINE;
			} else {
				if (parse_reg_exc_config(drv_priv, sz_line, para_info) == false) {
					PHL_ERR("Fail to parse regulation exception ruls!\n");
					goto exit;
				}
			continue;
			}
		}

		if (loadingstage == LD_STAGE_TAB_DEFINE) {
			/* read "## 2.4G, 20M, 1T, CCK" */
			if (sz_line[0] != '#' || sz_line[1] != '#')
				continue;

			/* skip the space */
			i = 2;
			while (sz_line[i] == ' ' || sz_line[i] == '\t')
				++i;

			sz_line[--i] = ' '; /* return the space in front of the regulation info */

			/* Parse the label of the table */
			_os_mem_set(drv_priv, (void *) band, 0, 10);
			_os_mem_set(drv_priv, (void *) bandwidth, 0, 10);
			_os_mem_set(drv_priv, (void *) ntx, 0, 10);
			_os_mem_set(drv_priv, (void *) ratesection, 0, 10);
			_os_mem_set(drv_priv, (void *) bf_type, 0, 10);

			if (!hal_parse_fiedstring(sz_line, &i, band, ' ', ',')) {
				PHL_ERR("Fail to parse band!\n");
				struct_idx = 0;
				goto exit;
			}
			if (!hal_parse_fiedstring(sz_line, &i, bandwidth, ' ', ',')) {
				PHL_ERR("Fail to parse bandwidth!\n");
				struct_idx = 0;
				goto exit;
			}
			if (!hal_parse_fiedstring(sz_line, &i, ntx, ' ', ',')) {
				PHL_ERR("Fail to parse ntx!\n");
				struct_idx = 0;
				goto exit;
			}
			if (!hal_parse_fiedstring(sz_line, &i, ratesection, ' ', ',')) {
				PHL_ERR("Fail to parse rate!\n");
				struct_idx = 0;
				goto exit;
			}
			if (!hal_parse_fiedstring(sz_line, &i, bf_type, ' ', '/')) {
				PHL_ERR("Fail to parse BF!\n");
				struct_idx = 0;
				goto exit;
			}

			loadingstage = LD_STAGE_TAB_START;
		} else if (loadingstage == LD_STAGE_TAB_START) {
			/* read "## START" */
			if (sz_line[0] != '#' || sz_line[1] != '#')
				continue;

			/* skip the space */
			i = 2;
			while (sz_line[i] == ' ' || sz_line[i] == '\t')
				++i;

			if (_os_strncmp((char *)(sz_line + i), (const char *)"START", 5)) {
				PHL_ERR("Missing \"##	START\" label\n");
				struct_idx = 0;
				goto exit;
			}
			loadingstage = LD_STAGE_COLUMN_DEFINE;
		} else if (loadingstage == LD_STAGE_COLUMN_DEFINE) {
			/* read "## #5# " */
			if (sz_line[0] != '#' || sz_line[1] != '#')

			/* skip the space */
			i = 2;
			while (sz_line[i] == ' ' || sz_line[i] == '\t')
				++i;

			_os_mem_set(drv_priv, (void *) colnumbuf, 0, 10);
			if (!hal_parse_fiedstring(sz_line, &i, colnumbuf, '#', '#')) {
				PHL_ERR("Fail to parse column number!\n");
				struct_idx = 0;
				goto exit;
			}
			if (!hal_get_u1bint_fromstr_indec(colnumbuf, &colnum)) {
				PHL_ERR("Column number \"%s\" is not unsigned decimal\n", colnumbuf);
				struct_idx = 0;
				goto exit;
			}
			if (colnum == 0) {
				PHL_ERR("Column number is 0\n");
				struct_idx = 0;
				goto exit;
			}
			PHL_INFO("[%s][%s][%s][%s][%s] column num:%d\n",
				band, bandwidth, ratesection, ntx, bf_type, colnum);

			regulation = (char **)_os_mem_alloc(drv_priv, sizeof(char *) * colnum);
			if (!regulation) {
				PHL_ERR("Regulation alloc fail\n");
				struct_idx = 0;
				goto exit;
			}
			pshape = (char **)_os_mem_alloc(drv_priv, sizeof(char *) * colnum);
			if (!pshape) {
				PHL_ERR("Regulation alloc fail\n");
				struct_idx = 0;
				goto exit;
			}
			loadingstage = LD_STAGE_COLUMN_REG;
		} else if (loadingstage == LD_STAGE_COLUMN_REG) {
			/* read "##		FCC ETSI	MKK IC	KCC" */
			if (sz_line[0] != '#' || sz_line[1] != '#')
				continue;

			/* skip the space */
			i = 2;
			for (forcnt = 0; forcnt < colnum; ++forcnt) {
				u32 i_ns;

				/* skip the space */
				while (sz_line[i] == ' ' || sz_line[i] == '\t')
					i++;
				i_ns = i;

				while (sz_line[i] != ' ' && sz_line[i] != '\t' && sz_line[i] != '\0')
					i++;

				regulation[forcnt] = (char *)_os_mem_alloc(drv_priv, i - i_ns + 1);
				if (!regulation[forcnt]) {
					PHL_ERR("Regulation alloc fail\n");
					struct_idx = 0;
					goto exit;
				}

				_os_mem_cpy(drv_priv, regulation[forcnt], sz_line + i_ns, i - i_ns);
				regulation[forcnt][i - i_ns] = '\0';
			}

			if (1) {
				PHL_INFO("column name:");
				for (forcnt = 0; forcnt < colnum; ++forcnt)
					PHL_INFO(" %s", regulation[forcnt]);
				PHL_INFO("\n");
			}

			loadingstage = LD_STAGE_PSHAPE_NUM;
		}else if (loadingstage == LD_STAGE_PSHAPE_NUM) {

			if (sz_line[0] == '#' || sz_line[1] == '#')
				continue;

			if ((sz_line[0] != 'p' && sz_line[0] != 'P') ||
				(sz_line[1] != 's' && sz_line[1] != 'S')) {
				PHL_INFO("No PShape prefix: '%c','%c'(%d,%d), continue to CH ROW\n",
					sz_line[0], sz_line[1], sz_line[0], sz_line[1]);
				loadingstage = LD_STAGE_CH_ROW;
				goto line_start;
			}
			PHL_INFO("1.sz_line[0 1] %c %c", sz_line[0], sz_line[1]);
			i = 6;/* move to the  location behind 'e' */

			for (forcnt = 0; forcnt < colnum; ++forcnt) {
				u32	i_ns;

				/* skip the space */
				while (sz_line[i] == ' ' || sz_line[i] == '\t')
					i++;
				i_ns = i;
				PHL_INFO("1.sz_line[%d] %c\n", i ,sz_line[i]);
				while (sz_line[i] != ' ' && sz_line[i] != '\t' && sz_line[i] != '\0')
					i++;

				pshape[forcnt] = (char *)_os_mem_alloc(drv_priv, i - i_ns + 1);
				if (!pshape[forcnt]) {
					PHL_ERR("Regulation alloc fail, pshape [%d]\n", forcnt);
					goto exit;
				}
				PHL_INFO("2.pshape [%d] = %s\n", forcnt, pshape[forcnt]);

				_os_mem_cpy(drv_priv, pshape[forcnt], sz_line + i_ns, i - i_ns);
				pshape[forcnt][i - i_ns] = '\0';
				PHL_INFO(" forcnt %d shape idx: %s:", forcnt, pshape[forcnt]);
			}

			if (1) {
				PHL_INFO("pshape idx:");
				for (forcnt = 0; forcnt < colnum; ++forcnt)
					PHL_INFO(" %s", pshape[forcnt]);
				PHL_INFO("\n");
			}

			loadingstage = LD_STAGE_CH_ROW;
		} else if (loadingstage == LD_STAGE_CH_ROW) {
			char	channel[10] = {0}, powerlimit[10] = {0};
			u8	cnt = 0;

			/* the table ends */
			if (sz_line[0] == '#' && sz_line[1] == '#') {
				i = 2;
				while (sz_line[i] == ' ' || sz_line[i] == '\t')
					++i;

				if (_os_strncmp((char *)(sz_line + i), (const char *)"END", 3) == 0) {
					loadingstage = LD_STAGE_TAB_DEFINE;
					if (regulation) {
						for (forcnt = 0; forcnt < colnum; ++forcnt) {
							if (regulation[forcnt]) {
								_os_mem_free(drv_priv,
									(u8 *)regulation[forcnt],
									_os_strlen((u8 *)regulation[forcnt]) + 1);
								regulation[forcnt] = NULL;
							}
						}
						_os_mem_free(drv_priv, (u8 *)regulation, sizeof(char *) * colnum);
						regulation = NULL;
					}
					if (pshape) {
						for (forcnt = 0; forcnt < colnum; ++forcnt) {
							if (pshape[forcnt]) {
								_os_mem_free(drv_priv,
									(u8 *)pshape[forcnt],
									_os_strlen((u8 *)pshape[forcnt]) + 1);
								pshape[forcnt] = NULL;
							}
						}
						_os_mem_free(drv_priv, (u8 *)pshape, sizeof(char *) * colnum);
						pshape = NULL;
					}
					colnum = 0;
					continue;
				} else {
					PHL_ERR("Missing \"##	END\" label\n");
					struct_idx = 0;
					goto exit;
				}
			}

			if ((sz_line[0] != 'c' && sz_line[0] != 'C') ||
				(sz_line[1] != 'h' && sz_line[1] != 'H')) {
				PHL_ERR("Wrong channel prefix: '%c','%c'(%d,%d)\n",
					sz_line[0], sz_line[1], sz_line[0], sz_line[1]);
				continue;
			}
			i = 2;/* move to the  location behind 'h' */

			/* load the channel number */
			cnt = 0;
			while (sz_line[i] >= '0' && sz_line[i] <= '9') {
				channel[cnt] = sz_line[i];
				++cnt;
				++i;
			}
			PHL_INFO("chnl %s!\n", channel);

			for (forcnt = 0; forcnt < colnum; ++forcnt) {
				/* skip the space between channel number and the power limit value */
				while (sz_line[i] == ' ' || sz_line[i] == '\t')
					++i;

				/* load the power limit value */
				_os_mem_set(drv_priv, (void *) powerlimit, 0, 10);

				if (sz_line[i] == 'W' && sz_line[i + 1] == 'W') {
					/*
					* case "WW" assign special ww value
					* means to get minimal limit in other regulations at same channel
					*/
					s8 ww_value = -63;
					_os_snprintf(powerlimit, 10, "%d", ww_value);
					i += 2;

				} else if (sz_line[i] == 'N' && sz_line[i + 1] == 'A') {
					/*
					* case "NA" assign max txgi value
					* means no limitation
					*/
					_os_snprintf(powerlimit, 10, "%d", 63);
					i += 2;

				} else if ((sz_line[i] >= '0' && sz_line[i] <= '9') || sz_line[i] == '.'
					|| sz_line[i] == '+' || sz_line[i] == '-') {
					/* case of dBm value */
					u8 integer = 0, fraction = 0, negative = 0;
					u32 u4bmove;
					s8 lmt = 0;

					if (sz_line[i] == '+' || sz_line[i] == '-') {
						if (sz_line[i] == '-')
							negative = 1;
						i++;
					}

					if (hal_get_fractionvalue_fromstring(&sz_line[i], &integer, &fraction, &u4bmove))
						i += u4bmove;
					else {
						PHL_ERR("Limit \"%s\" is not valid decimal\n", &sz_line[i]);
						struct_idx = 0;
						goto exit;
					}

					/* transform to string of value in unit of txgi */
					lmt = (integer * 4) + ((u16)fraction * 4 / 100);
					if (negative)
						lmt = -lmt;
					_os_snprintf(powerlimit, 10, "%d", lmt);

					} else {
						PHL_ERR("Wrong limit expression \"%c%c\"(%d, %d)\n"
							, sz_line[i], sz_line[i + 1], sz_line[i], sz_line[i + 1]);
						struct_idx = 0;
						goto exit;
					}

					/* store the power limit value */
					hal_phy_store_tx_power_limit(drv_priv, (u8 *)regulation[forcnt],
						(u8 *)band, (u8 *)bandwidth, (u8 *)ratesection,
						(u8 *)bf_type, (u8 *)ntx, (u8 *)channel,
						(u8 *)powerlimit, (u8 *)pshape[forcnt],
						(void*)&array_tc_8852a_txpwr_lmt[struct_idx], para_info);

					struct_idx++;
					}
				}
	}
exit:
	if (regulation) {
		for (forcnt = 0; forcnt < colnum; ++forcnt) {
			if (regulation[forcnt]) {
				_os_mem_free(drv_priv,
					(u8 *)regulation[forcnt],
					_os_strlen((u8 *)regulation[forcnt]) + 1);
				regulation[forcnt] = NULL;
			}
		}
		_os_mem_free(drv_priv, (u8 *)regulation, sizeof(char *) * colnum);
		regulation = NULL;
	}
	if (pshape) {
		for (forcnt = 0; forcnt < colnum; ++forcnt) {
			if (pshape[forcnt]) {
				_os_mem_free(drv_priv,
					(u8 *)pshape[forcnt],
					_os_strlen((u8 *)pshape[forcnt]) + 1);
				pshape[forcnt] = NULL;
			}
		}
		_os_mem_free(drv_priv, (u8 *)pshape, sizeof(char *) * colnum);
		pshape = NULL;
	}

	return struct_idx;
}


static void hal_phy_store_tx_power_limit_ru		(void *drv_priv,
		u8				*sregulation,
		u8				*sband,
		u8				*sbandwidth,
		u8				*sratesection,
		u8				*sntx,
		u8				*schannel,
		u8				*spwrlimit,
		u8				*spshape_idx,
		void *pstc_txpwr_lmt_ru,
		struct rtw_para_pwrlmt_info_t *para_info)
{
	u8 band = 0, bandwidth = 0, ratesec = 0, channel = 0;
	u8 ntx_idx = 0 , pshape_idx = 0;
	int regulation = -1;
	s8 powerlimit = 0;

	struct hal_txpwr_lmt_ru_t *array_tc_8852a_txpwr_lmt_ru = (struct hal_txpwr_lmt_ru_t *)pstc_txpwr_lmt_ru;

	if (hal_get_u1bint_fromstr_indec((char *)schannel, &channel) == false
		|| hal_get_s1bint_fromstr_indec((char *)spwrlimit, &powerlimit) == false) {
		PHL_INFO("Illegal index of power limit table [ch %s][val %s]\n",
			 (char *)schannel, (char *)spwrlimit);
		return;
	}

	if (spshape_idx != NULL) {
		if (hal_get_u1bint_fromstr_indec((char *)spshape_idx, &pshape_idx) == false) {
			PHL_INFO("Illegal index of pshape idx [val %s]\n", (char *)spshape_idx);
			return;
		}
	}

	if (_os_strncmp((char *)sband, (const char *)"2.4G", 4) == 0)
		band = _PW_LMT_BAND_2_4G;
	else if (_os_strncmp((char *)sband, (const char *)"5G", 2) == 0)
		band = _PW_LMT_BAND_5G;
	else if (_os_strncmp((char *)sband, (const char *)"6G", 2) == 0)
		band = _PW_LMT_BAND_6G;
	else {
		PHL_INFO("unknown band: %s\n", (char *)sband);
		return;
	}

	if (_os_strncmp((char *)sbandwidth, (const char *)"RU26", 4) == 0)
		bandwidth = _PW_LMT_RU_BW_RU26;
	else if (_os_strncmp((char *)sbandwidth, (const char *)"RU52", 4) == 0)
		bandwidth = _PW_LMT_RU_BW_RU52;
	else if (_os_strncmp((char *)sbandwidth, (const char *)"RU106", 5) == 0)
		bandwidth = _PW_LMT_RU_BW_RU106;
	else {
		PHL_INFO("unknown RU bandwidth: %s\n", (char *)sbandwidth);
		return;
	}

	if (_os_strncmp((char *)sntx, (const char *)"1T", 2) == 0)
		ntx_idx = _PW_LMT_PH_1T;
	else if (_os_strncmp((char *)sntx, (const char *)"2T", 2) == 0)
		ntx_idx = _PW_LMT_PH_2T;
	else if (_os_strncmp((char *)sntx, (const char *)"3T", 2) == 0)
		ntx_idx = _PW_LMT_PH_3T;
	else if (_os_strncmp((char *)sntx, (const char *)"4T", 2) == 0)
		ntx_idx = _PW_LMT_PH_4T;
	else {
		PHL_INFO("Wrong tx num:%s\n", (char *)sntx);
		return;
	}

	if (_os_strncmp((char *)sratesection, (const char *)"HE", 2) == 0)
		ratesec = _PW_LMT_RS_HE;
	else {
		PHL_INFO("Wrong RU rate section:%s\n", (char *)sratesection);
		return;
	}

	regulation = rtw_hal_rf_get_predefined_pw_lmt_regu_type_from_str((char *)sregulation);
	if (regulation == -1) {
		int regd_num = hal_phy_find_ext_regd_num(para_info, (char *)sregulation);

		if (regd_num != -1) {
			regulation = (u8)regd_num;
			PHL_INFO("new regulation num: %d\n", regulation);
		} else {
		PHL_INFO("unknown regulation: %s\n", (char *)sregulation);
		return;
	}

	}

	array_tc_8852a_txpwr_lmt_ru->band = band;
	array_tc_8852a_txpwr_lmt_ru->rubw = bandwidth;
	array_tc_8852a_txpwr_lmt_ru->ntx = ntx_idx;
	array_tc_8852a_txpwr_lmt_ru->rs = ratesec;
	array_tc_8852a_txpwr_lmt_ru->reg = (u8)regulation;
	array_tc_8852a_txpwr_lmt_ru->ch = channel;
	array_tc_8852a_txpwr_lmt_ru->val = powerlimit;
	array_tc_8852a_txpwr_lmt_ru->tx_shap_idx = pshape_idx;

	PHL_INFO("Store of power limit RU table [band %d][bw %d][ntx %d]"\
			"[rate section %d][regulation %d][chnl %d][val %d]  [sphape_idx %d]\n",
		array_tc_8852a_txpwr_lmt_ru->band , array_tc_8852a_txpwr_lmt_ru->rubw,
		array_tc_8852a_txpwr_lmt_ru->ntx, array_tc_8852a_txpwr_lmt_ru->rs,
		array_tc_8852a_txpwr_lmt_ru->reg, array_tc_8852a_txpwr_lmt_ru->ch,
		array_tc_8852a_txpwr_lmt_ru->val, array_tc_8852a_txpwr_lmt_ru->tx_shap_idx);
}


static int
_hal_parse_txpwrlmt_ru(void *drv_priv, void *para_info_t, u8 *psrc_buf, u32 buflen)
{
#define LD_STAGE_EXC_MAPPING	0
#define LD_STAGE_TAB_DEFINE		1
#define LD_STAGE_TAB_START		2
#define LD_STAGE_COLUMN_DEFINE	3
#define LD_STAGE_COLUMN_REG		4
#define LD_STAGE_CH_ROW			5
#define LD_STAGE_PSHAPE_NUM		6

	struct rtw_para_pwrlmt_info_t *para_info = (struct rtw_para_pwrlmt_info_t *)para_info_t;
	u32 *pdest_buf = para_info->para_data;
	char **regulation = NULL;
	char **pshape = NULL;
	char	*sz_line = NULL, *ptmp = NULL;
	char band[10], bandwidth[10], ratesection[10], ntx[10], col_num_buf[10];
	u8	col_num = 0;
	u8	loadingstage = LD_STAGE_EXC_MAPPING;
	u32 struct_idx = 0;
	u32	i = 0, for_cnt = 0;
	pHal_Txpwr_lmt_Ru_t array_tc_8852a_txpwr_lmt_ru;

	if (pdest_buf == NULL || psrc_buf == NULL) {
		PHL_INFO("%s, fail !!! NULL buf !!!\n", __func__);
		return 0;
	}

	array_tc_8852a_txpwr_lmt_ru = (pHal_Txpwr_lmt_Ru_t)pdest_buf;

	ptmp = (char *)psrc_buf;
	for (sz_line = hal_getLinefrombuffer(ptmp); sz_line != NULL; sz_line = hal_getLinefrombuffer(ptmp)) {
line_start:
		if (hal_is_allspace_tab(sz_line, sizeof(*sz_line)))
			continue;

		if (hal_is_comment_string(sz_line))
			continue;

		if (loadingstage == LD_STAGE_EXC_MAPPING) {
				if (sz_line[0] == '#' || sz_line[1] == '#') {
					loadingstage = LD_STAGE_TAB_DEFINE;
			} else {
				if (parse_reg_exc_config(drv_priv, sz_line, para_info) == false) {
					PHL_ERR("Fail to parse regulation exception ruls!\n");
					goto exit;
				}
				continue;
			}
		}

		if (loadingstage == LD_STAGE_TAB_DEFINE) {
			/* read "## 2.4G, RU26, 1T, HE //" */
			if (sz_line[0] != '#' || sz_line[1] != '#')
				continue;

			/* skip the space */
			i = 2;
			while (sz_line[i] == ' ' || sz_line[i] == '\t')
				++i;

			sz_line[--i] = ' '; /* return the space in front of the regulation info */

			/* Parse the label of the table */
			_os_mem_set(drv_priv, (void *) band, 0, 10);
			_os_mem_set(drv_priv, (void *) bandwidth, 0, 10);
			_os_mem_set(drv_priv, (void *) ntx, 0, 10);
			_os_mem_set(drv_priv, (void *) ratesection, 0, 10);

			if (!hal_parse_fiedstring(sz_line, &i, band, ' ', ',')) {
				PHL_ERR("Fail to parse band!\n");
				struct_idx = 0;
				goto exit;
			}
			if (!hal_parse_fiedstring(sz_line, &i, bandwidth, ' ', ',')) {
				PHL_ERR("Fail to parse bandwidth!\n");
				struct_idx = 0;
				goto exit;
			}
			if (!hal_parse_fiedstring(sz_line, &i, ntx, ' ', ',')) {
				PHL_ERR("Fail to parse ntx!\n");
				struct_idx = 0;
				goto exit;
			}
			if (!hal_parse_fiedstring(sz_line, &i, ratesection, ' ', ' ')) {
				PHL_ERR("Fail to parse rate!\n");
				struct_idx = 0;
				goto exit;
			}

			loadingstage = LD_STAGE_TAB_START;
		} else if (loadingstage == LD_STAGE_TAB_START) {
			/* read "## START" */
			if (sz_line[0] != '#' || sz_line[1] != '#')
				continue;

			/* skip the space */
			i = 2;
			while (sz_line[i] == ' ' || sz_line[i] == '\t')
				++i;

			if (_os_strncmp((char *)(sz_line + i), (const char *)"START", 5)) {
				PHL_ERR("Missing \"##	START\" label\n");
				struct_idx = 0;
				goto exit;
			}
			loadingstage = LD_STAGE_COLUMN_DEFINE;
		} else if (loadingstage == LD_STAGE_COLUMN_DEFINE) {
			/* read "## #5# " */
			if (sz_line[0] != '#' || sz_line[1] != '#')

			/* skip the space */
			i = 2;
			while (sz_line[i] == ' ' || sz_line[i] == '\t')
				++i;

			_os_mem_set(drv_priv, (void *) col_num_buf, 0, 10);
			if (!hal_parse_fiedstring(sz_line, &i, col_num_buf, '#', '#')) {
				PHL_ERR("Fail to parse column number!\n");
				struct_idx = 0;
				goto exit;
			}
			if (!hal_get_u1bint_fromstr_indec(col_num_buf, &col_num)) {
				PHL_ERR("Column number \"%s\" is not unsigned decimal\n", col_num_buf);
				struct_idx = 0;
				goto exit;
			}
			if (col_num == 0) {
				PHL_ERR("Column number is 0\n");
				struct_idx = 0;
				goto exit;
			}
			PHL_INFO("[%s][%s][%s][%s] column num:%d\n",
					band, bandwidth, ratesection, ntx, col_num);

			regulation = (char **)_os_mem_alloc(drv_priv, sizeof(char *) * col_num);
			if (!regulation) {
				PHL_ERR("Regulation alloc fail\n");
				struct_idx = 0;
				goto exit;
			}
			pshape = (char **)_os_mem_alloc(drv_priv, sizeof(char *) * col_num);
			if (!pshape) {
				PHL_ERR("Regulation alloc fail\n");
				struct_idx = 0;
				goto exit;
			}
			loadingstage = LD_STAGE_COLUMN_REG;
		}else if (loadingstage == LD_STAGE_COLUMN_REG) {
			/* read "##		FCC ETSI	MKK IC	KCC" */
			if (sz_line[0] != '#' || sz_line[1] != '#')
				continue;

			/* skip the space */
			i = 2;
			for (for_cnt = 0; for_cnt < col_num; ++for_cnt) {
				u32 i_ns;

				/* skip the space */
				while (sz_line[i] == ' ' || sz_line[i] == '\t')
					i++;
				i_ns = i;

				while (sz_line[i] != ' ' && sz_line[i] != '\t' && sz_line[i] != '\0')
					i++;

				regulation[for_cnt] = (char *)_os_mem_alloc(drv_priv, i - i_ns + 1);
				if (!regulation[for_cnt]) {
					PHL_ERR("Regulation alloc fail\n");
					struct_idx = 0;
					goto exit;
				}

				_os_mem_cpy(drv_priv, regulation[for_cnt], sz_line + i_ns, i - i_ns);
				regulation[for_cnt][i - i_ns] = '\0';
			}

			if (1) {
				PHL_INFO("column name:");
				for (for_cnt = 0; for_cnt < col_num; ++for_cnt)
					PHL_INFO(" %s", regulation[for_cnt]);
				PHL_INFO("\n");
			}
				loadingstage = LD_STAGE_PSHAPE_NUM;
		}else if (loadingstage == LD_STAGE_PSHAPE_NUM) {

			if (sz_line[0] == '#' || sz_line[1] == '#')
				continue;

			if ((sz_line[0] != 'p' && sz_line[0] != 'P') ||
				(sz_line[1] != 's' && sz_line[1] != 'S')) {
				PHL_INFO("No PShape prefix: '%c','%c'(%d,%d), continue to CH ROW\n",
					sz_line[0], sz_line[1], sz_line[0], sz_line[1]);
				loadingstage = LD_STAGE_CH_ROW;
				goto line_start;
			}
			PHL_INFO("1.sz_line[0 1] %c %c", sz_line[0], sz_line[1]);
			i = 6;/* move to the  location behind 'e' */

			for (for_cnt = 0; for_cnt < col_num; ++for_cnt) {
				u32	i_ns;

				/* skip the space */
				while (sz_line[i] == ' ' || sz_line[i] == '\t')
					i++;
				i_ns = i;
				PHL_INFO("1.sz_line[%d] %c\n", i ,sz_line[i]);
				while (sz_line[i] != ' ' && sz_line[i] != '\t' && sz_line[i] != '\0')
					i++;

				pshape[for_cnt] = (char *)_os_mem_alloc(drv_priv, i - i_ns + 1);
				if (!pshape[for_cnt]) {
					PHL_ERR("Regulation alloc fail, pshape [%d]\n", for_cnt);
					goto exit;
				}
				PHL_INFO("2.pshape [%d] = %s\n", for_cnt, pshape[for_cnt]);

				_os_mem_cpy(drv_priv, pshape[for_cnt], sz_line + i_ns, i - i_ns);
				pshape[for_cnt][i - i_ns] = '\0';
				PHL_INFO(" forcnt %d shape idx: %s:", for_cnt, pshape[for_cnt]);
			}

			if (1) {
				PHL_INFO("pshape idx:");
				for (for_cnt = 0; for_cnt < col_num; ++for_cnt)
					PHL_INFO(" %s", pshape[for_cnt]);
				PHL_INFO("\n");
			}

			loadingstage = LD_STAGE_CH_ROW;
		} else if (loadingstage == LD_STAGE_CH_ROW) {
			char	channel[10] = {0}, powerlimit[10] = {0};
			u8	cnt = 0;

			/* the table ends */
			if (sz_line[0] == '#' && sz_line[1] == '#') {
				i = 2;
				while (sz_line[i] == ' ' || sz_line[i] == '\t')
					++i;

				if (_os_strncmp((char *)(sz_line + i), (const char *)"END", 3) == 0) {
					loadingstage = LD_STAGE_TAB_DEFINE;
					if (regulation) {
						for (for_cnt = 0; for_cnt < col_num; ++for_cnt) {
							if (regulation[for_cnt]) {
								_os_mem_free(drv_priv,
									(u8 *)regulation[for_cnt],
									_os_strlen((u8 *)regulation[for_cnt]) + 1);
								regulation[for_cnt] = NULL;
							}
						}
						_os_mem_free(drv_priv, (u8 *)regulation, sizeof(char *) * col_num);
						regulation = NULL;
					}
					if (pshape) {
						for (for_cnt = 0; for_cnt < col_num; ++for_cnt) {
							if (pshape[for_cnt]) {
								_os_mem_free(drv_priv,
									(u8 *)pshape[for_cnt],
									_os_strlen((u8 *)pshape[for_cnt]) + 1);
								pshape[for_cnt] = NULL;
							}
						}
						_os_mem_free(drv_priv, (u8 *)pshape, sizeof(char *) * col_num);
						pshape = NULL;
					}
					col_num = 0;
					continue;
				} else {
					PHL_ERR("Missing \"##	END\" label\n");
					struct_idx = 0;
					goto exit;
				}
			}

			if ((sz_line[0] != 'c' && sz_line[0] != 'C') ||
				(sz_line[1] != 'h' && sz_line[1] != 'H')) {
				PHL_ERR("Wrong channel prefix: '%c','%c'(%d,%d)\n",
					sz_line[0], sz_line[1], sz_line[0], sz_line[1]);
				continue;
			}
			i = 2;/* move to the  location behind 'h' */

			/* load the channel number */
			cnt = 0;
			while (sz_line[i] >= '0' && sz_line[i] <= '9') {
				channel[cnt] = sz_line[i];
				++cnt;
				++i;
			}
			PHL_INFO("chnl %s!\n", channel);

			for (for_cnt = 0; for_cnt < col_num; ++for_cnt) {
				/* skip the space between channel number and the power limit value */
				while (sz_line[i] == ' ' || sz_line[i] == '\t')
					++i;

				/* load the power limit value */
				_os_mem_set(drv_priv, (void *) powerlimit, 0, 10);

				if (sz_line[i] == 'W' && sz_line[i + 1] == 'W') {
					/*
					* case "WW" assign special ww value
					* means to get minimal limit in other regulations at same channel
					*/
					s8 ww_value = -63;//phy_txpwr_ww_lmt_value(Adapter);

					_os_snprintf(powerlimit, 10, "%d", ww_value);
					i += 2;

				} else if (sz_line[i] == 'N' && sz_line[i + 1] == 'A') {
					/*
					* case "NA" assign max txgi value
					* means no limitation
					*/
					_os_snprintf(powerlimit, 10, "%d", 127);
					i += 2;

				} else if ((sz_line[i] >= '0' && sz_line[i] <= '9') || sz_line[i] == '.'
					|| sz_line[i] == '+' || sz_line[i] == '-') {
					/* case of dBm value */
					u8 integer = 0, fraction = 0, negative = 0;
					u32 u4bmove;
					s8 lmt = 0;

					if (sz_line[i] == '+' || sz_line[i] == '-') {
						if (sz_line[i] == '-')
							negative = 1;
						i++;
					}

					if (hal_get_fractionvalue_fromstring(&sz_line[i], &integer, &fraction, &u4bmove))
						i += u4bmove;
					else {
						PHL_ERR("Limit \"%s\" is not valid decimal\n", &sz_line[i]);
						struct_idx = 0;
						goto exit;
					}
					/* transform to string of value in unit of txgi */
					lmt = (integer * 4) + ((u16)fraction * 4 / 100);
					if (negative)
						lmt = -lmt;
					_os_snprintf(powerlimit, 10, "%d", lmt);

					} else {
						PHL_ERR("Wrong limit expression \"%c%c\"(%d, %d)\n"
							, sz_line[i], sz_line[i + 1], sz_line[i], sz_line[i + 1]);
						struct_idx = 0;
						goto exit;
					}

					/* store the power limit value */
					hal_phy_store_tx_power_limit_ru(drv_priv,
									(u8 *)regulation[for_cnt], (u8 *)band,
									(u8 *)bandwidth, (u8 *)ratesection,
									(u8 *)ntx, (u8 *)channel, (u8 *)powerlimit,
									(u8 *)pshape[for_cnt],
									(void*)&array_tc_8852a_txpwr_lmt_ru[struct_idx],
									para_info);
					PHL_INFO("array_tc_8852a_txpwr_lmt_ru[%d] \n", struct_idx);
					struct_idx++;
					}
				}
	}
exit:
	if (regulation) {
		for (for_cnt = 0; for_cnt < col_num; ++for_cnt) {
			if (regulation[for_cnt]) {
				_os_mem_free(drv_priv,
					(u8 *)regulation[for_cnt],
					_os_strlen((u8 *)regulation[for_cnt]) + 1);
				regulation[for_cnt] = NULL;
			}
		}
		_os_mem_free(drv_priv, (u8 *)regulation, sizeof(char *) * col_num);
		regulation = NULL;
	}
	if (pshape) {
		for (for_cnt = 0; for_cnt < col_num; ++for_cnt) {
			if (pshape[for_cnt]) {
				_os_mem_free(drv_priv,
					(u8 *)pshape[for_cnt],
					_os_strlen((u8 *)pshape[for_cnt]) + 1);
				pshape[for_cnt] = NULL;
			}
		}
		_os_mem_free(drv_priv, (u8 *)pshape, sizeof(char *) * col_num);
		pshape = NULL;
	}

	return struct_idx;
}

static void hal_phy_store_tx_power_track(
		void *drv_priv,
		char *band,
		char *path,
		char *sign,
		char *channel,
		char *rate,
		char *data,
		void *txpwr_track_table)
{
#define STR_EQUAL_6G(_band, _path, _sign, _rate, _chnl) \
	((_os_strcmp(band, _band) == 0) && (_os_strcmp(path, _path) == 0) && (_os_strcmp(sign, _sign) == 0) &&\
	 (_os_strcmp(rate, _rate) == 0) && (_os_strcmp(channel, _chnl) == 0)\
	)
#define STR_EQUAL_5G(_band, _path, _sign, _rate, _chnl) \
	((_os_strcmp(band, _band) == 0) && (_os_strcmp(path, _path) == 0) && (_os_strcmp(sign, _sign) == 0) &&\
	 (_os_strcmp(rate, _rate) == 0) && (_os_strcmp(channel, _chnl) == 0)\
	)
#define STR_EQUAL_2G(_band, _path, _sign, _rate) \
	((_os_strcmp(band, _band) == 0) && (_os_strcmp(path, _path) == 0) && (_os_strcmp(sign, _sign) == 0) &&\
	 (_os_strcmp(rate, _rate) == 0)\
	)

#define STORE_SWING_TABLE(_array, _iteratedIdx) \
	do {	\
	for (token = (char *)_os_strsep(&data, delim); token != (char *)NULL; token = (char *)_os_strsep(&data, delim)) {\
		_os_sscanf(token, "%d", &idx);\
		_array[_iteratedIdx++] = (s8)idx;\
	} } while (0)\

	struct hal_txpwr_track_t *prfcalibrateInfo = (struct hal_txpwr_track_t *)txpwr_track_table;

	u32	j = 0;
	char	*token;
	char	delim[] = ",";
	u32	idx = 0;

	PHL_INFO("===>initDeltaSwingIndexTables():"\
		"Band: %s;\nPath: %s;\nSign: %s;\nChannel: %s;\nRate: %s;\n, Data: %s;\n",
	band, path, sign, channel, rate, data);

	if (STR_EQUAL_2G("2G", "A", "+", "CCK"))
		STORE_SWING_TABLE(prfcalibrateInfo->delta_swing_table_idx_2g_cck_a_p, j);
	else if (STR_EQUAL_2G("2G", "A", "-", "CCK"))
		STORE_SWING_TABLE(prfcalibrateInfo->delta_swing_table_idx_2g_cck_a_n, j);
	else if (STR_EQUAL_2G("2G", "B", "+", "CCK"))
		STORE_SWING_TABLE(prfcalibrateInfo->delta_swing_table_idx_2g_cck_b_p, j);
	else if (STR_EQUAL_2G("2G", "B", "-", "CCK"))
		STORE_SWING_TABLE(prfcalibrateInfo->delta_swing_table_idx_2g_cck_b_n, j);
	else if (STR_EQUAL_2G("2G", "A", "+", "ALL"))
		STORE_SWING_TABLE(prfcalibrateInfo->delta_swing_table_idx_2ga_p, j);
	else if (STR_EQUAL_2G("2G", "A", "-", "ALL"))
		STORE_SWING_TABLE(prfcalibrateInfo->delta_swing_table_idx_2ga_n, j);
	else if (STR_EQUAL_2G("2G", "B", "+", "ALL"))
		STORE_SWING_TABLE(prfcalibrateInfo->delta_swing_table_idx_2gb_p, j);
	else if (STR_EQUAL_2G("2G", "B", "-", "ALL"))
		STORE_SWING_TABLE(prfcalibrateInfo->delta_swing_table_idx_2gb_n, j);
	else if (STR_EQUAL_5G("5G", "A", "+", "ALL", "0"))
		STORE_SWING_TABLE(prfcalibrateInfo->delta_swing_table_idx_5ga_p[0], j);
	else if (STR_EQUAL_5G("5G", "A", "-", "ALL", "0"))
		STORE_SWING_TABLE(prfcalibrateInfo->delta_swing_table_idx_5ga_n[0], j);
	else if (STR_EQUAL_5G("5G", "B", "+", "ALL", "0"))
		STORE_SWING_TABLE(prfcalibrateInfo->delta_swing_table_idx_5gb_p[0], j);
	else if (STR_EQUAL_5G("5G", "B", "-", "ALL", "0"))
		STORE_SWING_TABLE(prfcalibrateInfo->delta_swing_table_idx_5gb_n[0], j);
#if (BAND_NUM > 1)
	else if (STR_EQUAL_5G("5G", "A", "+", "ALL", "1"))
		STORE_SWING_TABLE(prfcalibrateInfo->delta_swing_table_idx_5ga_p[1], j);
	else if (STR_EQUAL_5G("5G", "A", "-", "ALL", "1"))
		STORE_SWING_TABLE(prfcalibrateInfo->delta_swing_table_idx_5ga_n[1], j);
	else if (STR_EQUAL_5G("5G", "B", "+", "ALL", "1"))
		STORE_SWING_TABLE(prfcalibrateInfo->delta_swing_table_idx_5gb_p[1], j);
	else if (STR_EQUAL_5G("5G", "B", "-", "ALL", "1"))
		STORE_SWING_TABLE(prfcalibrateInfo->delta_swing_table_idx_5gb_n[1], j);
#endif
#if (BAND_NUM > 2)
	else if (STR_EQUAL_5G("5G", "A", "+", "ALL", "2"))
		STORE_SWING_TABLE(prfcalibrateInfo->delta_swing_table_idx_5ga_p[2], j);
	else if (STR_EQUAL_5G("5G", "A", "-", "ALL", "2"))
		STORE_SWING_TABLE(prfcalibrateInfo->delta_swing_table_idx_5ga_n[2], j);
	else if (STR_EQUAL_5G("5G", "B", "+", "ALL", "2"))
		STORE_SWING_TABLE(prfcalibrateInfo->delta_swing_table_idx_5gb_p[2], j);
	else if (STR_EQUAL_5G("5G", "B", "-", "ALL", "2"))
		STORE_SWING_TABLE(prfcalibrateInfo->delta_swing_table_idx_5gb_n[2], j);
#endif
#if (BAND_NUM > 3)
	else if (STR_EQUAL_5G("5G", "A", "+", "ALL", "3"))
		STORE_SWING_TABLE(prfcalibrateInfo->delta_swing_table_idx_5ga_p[3], j);
	else if (STR_EQUAL_5G("5G", "A", "-", "ALL", "3"))
		STORE_SWING_TABLE(prfcalibrateInfo->delta_swing_table_idx_5ga_n[3], j);
	else if (STR_EQUAL_5G("5G", "B", "+", "ALL", "3"))
		STORE_SWING_TABLE(prfcalibrateInfo->delta_swing_table_idx_5gb_p[3], j);
	else if (STR_EQUAL_5G("5G", "B", "-", "ALL", "3"))
		STORE_SWING_TABLE(prfcalibrateInfo->delta_swing_table_idx_5gb_n[3], j);
#endif
	else if (STR_EQUAL_6G("6G", "A", "+", "ALL", "0"))
		STORE_SWING_TABLE(prfcalibrateInfo->delta_swing_table_idx_6ga_p[0], j);
	else if (STR_EQUAL_6G("6G", "A", "-", "ALL", "0"))
		STORE_SWING_TABLE(prfcalibrateInfo->delta_swing_table_idx_6ga_n[0], j);
	else if (STR_EQUAL_6G("6G", "B", "+", "ALL", "0"))
		STORE_SWING_TABLE(prfcalibrateInfo->delta_swing_table_idx_6gb_p[0], j);
	else if (STR_EQUAL_6G("6G", "B", "-", "ALL", "0"))
		STORE_SWING_TABLE(prfcalibrateInfo->delta_swing_table_idx_6gb_n[0], j);
	else if (STR_EQUAL_6G("6G", "A", "+", "ALL", "1"))
		STORE_SWING_TABLE(prfcalibrateInfo->delta_swing_table_idx_6ga_p[1], j);
	else if (STR_EQUAL_6G("6G", "A", "-", "ALL", "1"))
		STORE_SWING_TABLE(prfcalibrateInfo->delta_swing_table_idx_6ga_n[1], j);
	else if (STR_EQUAL_6G("6G", "B", "+", "ALL", "1"))
		STORE_SWING_TABLE(prfcalibrateInfo->delta_swing_table_idx_6gb_p[1], j);
	else if (STR_EQUAL_6G("6G", "B", "-", "ALL", "1"))
		STORE_SWING_TABLE(prfcalibrateInfo->delta_swing_table_idx_6gb_n[1], j);
	else if (STR_EQUAL_6G("6G", "A", "+", "ALL", "2"))
		STORE_SWING_TABLE(prfcalibrateInfo->delta_swing_table_idx_6ga_p[2], j);
	else if (STR_EQUAL_6G("6G", "A", "-", "ALL", "2"))
		STORE_SWING_TABLE(prfcalibrateInfo->delta_swing_table_idx_6ga_n[2], j);
	else if (STR_EQUAL_6G("6G", "B", "+", "ALL", "2"))
		STORE_SWING_TABLE(prfcalibrateInfo->delta_swing_table_idx_6gb_p[2], j);
	else if (STR_EQUAL_6G("6G", "B", "-", "ALL", "2"))
		STORE_SWING_TABLE(prfcalibrateInfo->delta_swing_table_idx_6gb_n[2], j);
	else if (STR_EQUAL_6G("6G", "A", "+", "ALL", "3"))
		STORE_SWING_TABLE(prfcalibrateInfo->delta_swing_table_idx_6ga_p[3], j);
	else if (STR_EQUAL_6G("6G", "A", "-", "ALL", "3"))
		STORE_SWING_TABLE(prfcalibrateInfo->delta_swing_table_idx_6ga_n[3], j);
	else if (STR_EQUAL_6G("6G", "B", "+", "ALL", "3"))
		STORE_SWING_TABLE(prfcalibrateInfo->delta_swing_table_idx_6gb_p[3], j);
	else if (STR_EQUAL_6G("6G", "B", "-", "ALL", "3"))
		STORE_SWING_TABLE(prfcalibrateInfo->delta_swing_table_idx_6gb_n[3], j);
	else
		PHL_INFO("===>initDeltaSwingIndexTables(): The input is invalid!!\n");
}

static int
_hal_parse_txpwrtrack(void *drv_priv, void *para_info_t, u8 *psrc_buf, u32 buflen)
{
	struct rtw_para_info_t *para_info = (struct rtw_para_info_t *)para_info_t;
	u32 *pdest_buf = para_info->para_data;
	char *sz_line = NULL, *ptmp = NULL;
	u32	i = 0;
	struct hal_txpwr_track_t *txpwr_track;

	if (pdest_buf == NULL || psrc_buf == NULL) {
		PHL_INFO("%s, fail !!! NULL buf !!!\n", __func__);
		return 0;
	}
	/* Assing destination buffer to be Txpwr_track table format*/
	txpwr_track = (struct hal_txpwr_track_t *)pdest_buf;

	ptmp = (char *)psrc_buf;
	for (sz_line = hal_getLinefrombuffer(ptmp); sz_line != NULL;
								sz_line = hal_getLinefrombuffer(ptmp))
	{
		if (!hal_is_comment_string(sz_line)) {
			char band[5] = "", path[5] = "", sign[5]  = "";
			char chnl[5] = "", rate[10] = "";
			char data[300] = ""; /* 100 is too small */

			if (_os_strlen((u8 *)sz_line) < 10 || sz_line[0] != '[')
				continue;

			_os_strncpy(band, sz_line + 1, 2);
			_os_strncpy(path, sz_line + 5, 1);
			_os_strncpy(sign, sz_line + 8, 1);

			i = 10; /* sz_line+10 */
			if (!hal_parse_fiedstring(sz_line, &i, rate, '[', ']')) {
				PHL_ERR("Fail to parse rate!\n");
			}
			if (!hal_parse_fiedstring(sz_line, &i, chnl, '[', ']')) {
				if (!_os_strcmp("5G",band))
					PHL_ERR("Fail to parse channel group!\n");
			}
			while ('{' != sz_line[i] && i < _os_strlen((u8 *)sz_line))
				i++;
			if (!hal_parse_fiedstring(sz_line, &i, data, '{', '}')) {
				PHL_ERR("Fail to parse data!\n");
			}
			hal_phy_store_tx_power_track(drv_priv,
						band, path, sign,
						chnl, rate, data,
						(void*)txpwr_track);
		}
	}
	return 1;
}

void
_hal_dl_para_file(struct rtw_phl_com_t *phl_com,
	void *para_info_t, char *ic_name,
	int (*parser_fun)(void *drv_priv, void *para_info_t, u8 *psrc_buf, u32 buflen),
	const char *file_name)
{
#ifdef CONFIG_LOAD_PHY_PARA_FROM_FILE

	void *drv = phl_com->drv_priv;
	char hal_phy_folder[MAX_PATH_LEN] = {0};
	char para_file_name[MAX_PATH_LEN] = {0};
	char *sp, *ext = NULL;
	u8 i, dot_pos;
	u32 para_size = 0, postfix_size = 0;
	u8 *para_buf = NULL;
	struct rtw_para_info_t *para_info = (struct rtw_para_info_t *)para_info_t;

	if (para_info->para_src == RTW_PARA_SRC_INTNAL) {
		PHL_TRACE(COMP_PHL_DBG, _PHL_DEBUG_, "%s.parser_fun=NULL \n", file_name);
		return;
	}

	if (!parser_fun || (!para_info->para_data)) {
		PHL_TRACE(COMP_PHL_DBG, _PHL_DEBUG_, "%s.parser_fun=NULL \n", file_name);
		para_info->para_src = RTW_PARA_SRC_INTNAL;
		return;
	}

	if ((para_info->para_data_len != 0) && (para_info->para_src == RTW_PARA_SRC_EXTNAL)) {
		PHL_TRACE(COMP_PHL_DBG, _PHL_DEBUG_, "%s. para_data_len != 0 !!!\n", file_name);
		return;
	}

	para_buf = _os_mem_alloc(drv, MAX_HWCONFIG_FILE_CONTENT);
	if (!para_buf) {
		PHL_TRACE(COMP_PHL_DBG, _PHL_DEBUG_, "para_buf=NULL \n");
		para_info->para_src = RTW_PARA_SRC_INTNAL;
		return;
	}

	if (para_info->para_src == RTW_PARA_SRC_EXTNAL_BUF) {
		if (para_info->ext_para_file_buf != 0) {
			/* Parsing file content */
			para_info->para_data_len =
				parser_fun(drv, para_info,
					   para_info->ext_para_file_buf,
					   para_info->ext_para_file_buf_len);

			if (para_info->para_data_len) {
				PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_,
					"%s:: Download file ok.\n", __FUNCTION__);
			} else {
				PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_,
					"%s:: Failed to parser %s\n",
					__FUNCTION__, file_name);
				para_info->para_src = RTW_PARA_SRC_INTNAL;
			}
			PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "%s:: Download file ok.\n", file_name);
		} else {
			PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "%s:: Error reading file.\n", file_name);

			para_info->para_src = RTW_PARA_SRC_INTNAL;
			para_info->para_data_len = 0;
		}
	} else if (para_info->para_src == RTW_PARA_SRC_EXTNAL) {
		char para_path[MAX_PATH_LEN];
		/* Use path in para_info if it is not empty. */
		if (para_info->para_path[0] != 0) {
			_os_snprintf(para_path, MAX_PATH_LEN,
				     "%s%s", para_info->para_path,
				     file_name);
		} else {
			_os_snprintf(para_path, MAX_PATH_LEN, "%s%s%s%s",
				     hal_phy_folder, ic_name, _os_path_sep,
				     file_name);
		}

		/* Determine parameter folder path */
		if (para_info->hal_phy_folder != NULL) {
			_os_snprintf(hal_phy_folder, MAX_PATH_LEN, "%s",
						 para_info->hal_phy_folder);
		} else {
			_os_snprintf(hal_phy_folder, MAX_PATH_LEN, "%s%s%s",
					     HAL_FILE_CONFIG_PATH , ic_name, _os_path_sep);
		}

		/* Determine parameter file name */
		_os_strncpy(para_file_name, file_name, _os_strlen((u8 *)file_name)+1);

		/* Add postfix into original file name if it is specified by user */
		postfix_size = _os_strlen((u8 *)para_info->postfix);

		if (postfix_size != 0) {
			/* find the position of latest dot char in file name */
			sp = para_file_name;
			for (i = 0, dot_pos = 0; i < _os_strlen((u8 *)file_name); i++) {
				if (sp[i] == '.')
					dot_pos = i;
			}

			/* Get file extension name from original file name string */
			ext = (char *)file_name + dot_pos;

			/* Attach postfix, extension name and null terminator */
			_os_strncpy(sp + dot_pos, para_info->postfix, postfix_size);
			_os_strncpy(sp + dot_pos + postfix_size, ext, _os_strlen((u8 *)ext));
			*(sp + dot_pos + postfix_size + _os_strlen((u8 *)ext)) = '\0';
		}

		/* Generate final parameter file full path */
		_os_snprintf(para_info->para_path, MAX_PATH_LEN, "%s%s",
				 hal_phy_folder, para_file_name);

		PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "%s:: %s\n",__FUNCTION__,
			  para_path);

		para_size = _os_read_file(para_path, para_buf,
					  MAX_HWCONFIG_FILE_CONTENT);
	} else if (para_info->para_src == RTW_PARA_SRC_CUSTOM) {
		_os_mem_cpy(drv, para_buf, para_info->para_data, para_info->para_data_len);
		_os_mem_set(drv, para_info->para_data, 0, para_info->para_data_len);
		para_size = para_info->para_data_len;
		para_info->para_data_len = 0;
	}

	if (para_size != 0) {
		/* Parsing file content */
		para_info->para_data_len = parser_fun(drv, para_info, para_buf,
						      para_size);

		if (para_info->para_data_len) {
			PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_,
				  "%s:: Download file ok.\n", __FUNCTION__);
		} else {
			PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_,
				  "%s:: Failed to parser %s\n",
				  __FUNCTION__, file_name);
			para_info->para_src = RTW_PARA_SRC_INTNAL;
		}
		PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "%s:: Download file ok.\n",
			  file_name);
	} else {
		PHL_TRACE(COMP_PHL_DBG, _PHL_ERR_, "%s:: Error reading file.\n",
			  file_name);

		para_info->para_src = RTW_PARA_SRC_INTNAL;
		para_info->para_data_len = 0;
	}
	_os_mem_free(drv, para_buf, MAX_HWCONFIG_FILE_CONTENT);
	para_buf = NULL;
#endif
}

enum rtw_hal_status
_phl_pwrlmt_para_alloc(struct rtw_phl_com_t* phl_com,
				struct rtw_para_pwrlmt_info_t *para_info)
{
#ifdef CONFIG_LOAD_PHY_PARA_FROM_FILE
	if (para_info->para_src == RTW_PARA_SRC_EXTNAL) {
		u32 file_buf_sz = MAX_HWCONFIG_FILE_CONTENT;
		u32 buf_sz = MAX_LINES_HWCONFIG_TXT;
		void *drv = phl_com->drv_priv;
		u8 para_regd_str_arridx;
		const char * const *_para_regd_str = rtw_hal_rf_get_predefined_pw_lmt_regu_type_str_array(&para_regd_str_arridx);
		u8 i = 0;

		if(_para_regd_str == NULL)
			return RTW_HAL_STATUS_FAILURE;

		for (i = 0; i < para_regd_str_arridx ; i++) {
			if (_para_regd_str[i]) {
				_os_strncpy(para_info->ext_regd_name[i],
				            _para_regd_str[i],
				            sizeof(para_info->ext_regd_name[i]) - 1);
				para_info->ext_regd_name[i][sizeof(para_info->ext_regd_name[i])-1] = 0;
				PHL_INFO(" prepare ext_regd_name[%d] = %s\n",
					 i , para_info->ext_regd_name[i]);
			} else {
				PHL_ERR(" prepare ext_regd_name[%d] = NULL\n", i);
				para_info->ext_regd_name[i][0] = 0;
			}
		}
		para_info->ext_regd_arridx = para_regd_str_arridx;

		para_info->para_data = _os_mem_alloc(drv, file_buf_sz * sizeof(u32));
		if (!para_info->para_data) {
			PHL_TRACE(COMP_PHL_DBG, _PHL_ERR_,
				"%s::para_data allocmem fail\n",__FUNCTION__);
			return RTW_HAL_STATUS_FAILURE;
		}

		para_info->ext_reg_codemap = _os_mem_alloc(drv, buf_sz * sizeof(u8));
		if (!para_info->ext_reg_codemap) {
			if (para_info->para_data)
				_os_mem_free(drv, para_info->para_data, file_buf_sz * sizeof(u32));
			para_info->para_data = NULL;
			PHL_TRACE(COMP_PHL_DBG, _PHL_ERR_,
				"%s::ext_reg_codemap allocmem fail\n",__FUNCTION__);
			return RTW_HAL_STATUS_FAILURE;
		}
		return RTW_HAL_STATUS_SUCCESS;
	} else
		return RTW_HAL_STATUS_FAILURE;
#else
	return RTW_HAL_STATUS_FAILURE;
#endif
}

enum rtw_hal_status
phl_load_file_data_alloc(struct rtw_phl_com_t* phl_com, struct rtw_para_info_t *para_info)
{
#ifdef CONFIG_LOAD_PHY_PARA_FROM_FILE
	u32 buf_sz = MAX_HWCONFIG_FILE_CONTENT;
	void *drv = phl_com->drv_priv;

	if (para_info->para_src == RTW_PARA_SRC_EXTNAL) {
		if (para_info->para_data_len == 0) {
			para_info->para_data = _os_mem_alloc(drv, buf_sz * sizeof(u32));
	}
	if (!para_info->para_data) {
			PHL_TRACE(COMP_PHL_DBG, _PHL_ERR_,
				"%s:: allocmem fail\n",__FUNCTION__);
			return RTW_HAL_STATUS_FAILURE;
		}
		return RTW_HAL_STATUS_SUCCESS;
	} else
		return RTW_HAL_STATUS_FAILURE;
#else
	return RTW_HAL_STATUS_FAILURE;
#endif
}

int rtw_hal_find_ext_regd_num(struct rtw_para_pwrlmt_info_t *para_info, const char *regd_name)
{
	return hal_phy_find_ext_regd_num(para_info, regd_name);
}

void
rtw_hal_dl_all_para_file(struct rtw_phl_com_t *phl_com,
							char *ic_name, void *hal)
{
#ifdef CONFIG_LOAD_PHY_PARA_FROM_FILE
	struct phy_sw_cap_t *phy_sw_cap = NULL;
	u8 idx=0;
	u8 max_phy_num = 1;

#ifdef CONFIG_DBCC_SUPPORT
	if (phl_com->dev_cap.dbcc_sup == true)
		max_phy_num = MAX_BAND_NUM;
#endif

	for (idx = 0; idx < max_phy_num; idx++) {

		phy_sw_cap = &phl_com->phy_sw_cap[idx];
		if (phl_load_file_data_alloc(phl_com,
					&phy_sw_cap->bb_phy_reg_info) == RTW_HAL_STATUS_SUCCESS)
			_hal_dl_para_file(phl_com,
					&phy_sw_cap->bb_phy_reg_info,
					ic_name,
					_hal_parse_phyreg,
					"PHY_REG.txt");

		if (phl_load_file_data_alloc(phl_com,
					&phy_sw_cap->bb_phy_reg_gain_info) == RTW_HAL_STATUS_SUCCESS)
			_hal_dl_para_file(phl_com,
					&phy_sw_cap->bb_phy_reg_gain_info,
					ic_name,
					_hal_parse_phyreg,
					"PHY_REG_GAIN.txt");

		if (phl_load_file_data_alloc(phl_com,
					&phy_sw_cap->rf_radio_a_info) == RTW_HAL_STATUS_SUCCESS)
			_hal_dl_para_file(phl_com,
					&phy_sw_cap->rf_radio_a_info,
					ic_name,
					_hal_parse_radio,
					"RadioA.txt");

		if (phl_load_file_data_alloc(phl_com,
					&phy_sw_cap->rf_radio_b_info) == RTW_HAL_STATUS_SUCCESS)
			_hal_dl_para_file(phl_com,
					&phy_sw_cap->rf_radio_b_info,
					ic_name,
					_hal_parse_radio,
					"RadioB.txt");

		if (phl_load_file_data_alloc(phl_com,
				&phy_sw_cap->rf_txpwr_byrate_info) == RTW_HAL_STATUS_SUCCESS)
			_hal_dl_para_file(phl_com,
					&phy_sw_cap->rf_txpwr_byrate_info,
					ic_name,
					_hal_parse_txpwr_by_rate,
					"TXPWR_ByRate.txt");

		if (phl_load_file_data_alloc(phl_com,
				&phy_sw_cap->rf_txpwrtrack_info) == RTW_HAL_STATUS_SUCCESS)
			_hal_dl_para_file(phl_com,
					&phy_sw_cap->rf_txpwrtrack_info,
					ic_name,
					_hal_parse_txpwrtrack,
					"TXPWR_TrackTSSI.txt");

		if (_phl_pwrlmt_para_alloc(phl_com,
				&phy_sw_cap->rf_txpwrlmt_info) == RTW_HAL_STATUS_SUCCESS)
			_hal_dl_para_file(phl_com,
					&phy_sw_cap->rf_txpwrlmt_info,
					ic_name,
					_hal_parse_txpwrlmt,
					"TXPWR_LMT.txt");

		if (_phl_pwrlmt_para_alloc(phl_com,
				&phy_sw_cap->rf_txpwrlmt_ru_info) == RTW_HAL_STATUS_SUCCESS)
			_hal_dl_para_file(phl_com,
					&phy_sw_cap->rf_txpwrlmt_ru_info,
					ic_name,
					_hal_parse_txpwrlmt_ru,
					"TXPWR_LMT_RU.txt");

		if (_phl_pwrlmt_para_alloc(phl_com,
				&phy_sw_cap->rf_txpwrlmt_6g_info) == RTW_HAL_STATUS_SUCCESS)
			_hal_dl_para_file(phl_com,
					&phy_sw_cap->rf_txpwrlmt_6g_info,
					ic_name,
					_hal_parse_txpwrlmt,
					"TXPWR_LMT_6G.txt");

		if (_phl_pwrlmt_para_alloc(phl_com,
				&phy_sw_cap->rf_txpwrlmt_ru_6g_info) == RTW_HAL_STATUS_SUCCESS)
			_hal_dl_para_file(phl_com,
					&phy_sw_cap->rf_txpwrlmt_ru_6g_info,
					ic_name,
					_hal_parse_txpwrlmt_ru,
					"TXPWR_LMT_RU_6G.txt");

		phy_sw_cap->bfreed_para = false;
	}
#endif
}

u8 rtw_hal_ld_fw_symbol(struct rtw_phl_com_t *phl_com,
	struct rtw_hal_com_t *hal_com, const char *name, u8 **buf, u32 *buf_size)
{
	struct rtw_fw_info_t *fw_info = &phl_com->fw_info;
	char path[256] = {0};
	char *ic_name = NULL;
	char *intf = NULL;
	void *d = phlcom_to_drvpriv(phl_com);

	switch (phl_com->hci_type) {
	case RTW_HCI_PCIE:
		intf = "e";
		break;
	case RTW_HCI_USB:
		intf = "u";
		break;
	case RTW_HCI_SDIO:
		intf = "s";
		break;
	default:
		PHL_WARN("%s unknown hci type %u \n", __func__, phl_com->hci_type);
		return (u8)RTW_HAL_STATUS_FAILURE;
	}

	switch (hal_com->chip_id) {
	case CHIP_WIFI6_8852A:
		ic_name = "rtl8852a";
		break;
	case CHIP_WIFI6_8852B:
		ic_name = "rtl8852b";
		break;
	default:
		PHL_WARN("%s unknown chip id %u \n", __func__, hal_com->chip_id);
		return (u8)RTW_HAL_STATUS_FAILURE;
	}

	if (name == NULL || buf_size == NULL)
		return (u8)RTW_HAL_STATUS_FAILURE;

	if (!fw_info->sym_buf) {

		fw_info->sym_buf = _os_mem_alloc(d, RTW_MAX_FW_SIZE);
		if (!fw_info->sym_buf) {
			PHL_WARN("%s : buf for fw symbol allocate fail!!\n", __func__);
			return (u8)RTW_HAL_STATUS_FAILURE;
		}
	}
	_os_snprintf(path, MAX_PATH_LEN, "%s%s%s%s%s", FW_FILE_CONFIG_PATH,
		     ic_name, intf, _os_path_sep, name);

	PHL_INFO("%s : %s\n", __func__, path);

	fw_info->sym_buf_size = _os_read_file(path, fw_info->sym_buf,
		RTW_MAX_FW_SIZE);
	/* reading fw file failed */
	if (0 == fw_info->sym_buf_size)
		return (u8)RTW_HAL_STATUS_FAILURE;

	*buf = fw_info->sym_buf;
	*buf_size = fw_info->sym_buf_size;

	return (u8)RTW_HAL_STATUS_SUCCESS;
}

u8 rtw_hal_efuse_shadow_file_load(void *hal, char *ic_name, bool is_limit)
{
	u8 status = true;
#ifdef CONFIG_EFUSE_CONFIG_FILE
	struct rtw_hal_com_t *hal_com = (struct rtw_hal_com_t *)hal;
	struct hal_info_t *hal_info = (struct hal_info_t *)hal_com->hal_priv;
	char *hal_phy_folder = HAL_FILE_CONFIG_PATH;
	char file_path[256];

	_os_snprintf(file_path, MAX_PATH_LEN, "%s%s%s%s",
			hal_phy_folder , ic_name, _os_path_sep, "efuse.map");
	PHL_INFO("%s::open map path: %s\n", __FUNCTION__, file_path);

	status = rtw_hal_efuse_file_map_load(hal_info, file_path, is_limit);
	if (status == RTW_HAL_STATUS_FAILURE) {
		PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_,
				"%s:: %s FAIL\n", __FUNCTION__, file_path);
		status = false;
		goto exit;
	}

	_os_snprintf(file_path, MAX_PATH_LEN, "%s%s%s%s",
			hal_phy_folder , ic_name, _os_path_sep, "efuse.mask");
	PHL_INFO("%s::open mask path: %s\n", __FUNCTION__, file_path);

	status = rtw_hal_efuse_file_mask_load(hal_info, file_path, is_limit);
	if (status == RTW_HAL_STATUS_FAILURE) {
		PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_,
				"%s:: %s FAIL\n", __FUNCTION__, file_path);
		status = false;
		goto exit;
	}
exit:
#endif
	return status;
}

