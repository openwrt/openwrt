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
#ifndef _HALBB_DBG_CMD_H_
#define _HALBB_DBG_CMD_H_
/*@--------------------------[Define] ---------------------------------------*/
#define	MAX_ARGC		20
#define	MAX_ARGV		16

#if 1
#define HALBB_SCAN _os_sscanf
#define DCMD_DECIMAL	"%d"
#define DCMD_HEX	"%x"
#define DCMD_HEX64	"%llx"
#define DCMD_CHAR	"%s"
#else
#define HALBB_SCAN halbb_scanf
enum bb_scanf_type
{
	DCMD_DECIMAL	= 1,
	DCMD_HEX	= 2,
	DCMD_CHAR	= 3,
};
#endif

#define DCMD_SCAN_LIMIT 10

/*@--------------------------[Enum]------------------------------------------*/
 
enum bb_scanf_type
{
	DCMD2_DECIMAL	= 1,
	DCMD2_HEX	= 2,
	DCMD2_CHAR	= 3,
};

 enum FWBB_DBG_COMP_SET {
	FWBBDBG_H2C = BIT0,
	FWBBDBG_RA = BIT8,
	FWBBDBG_RUA = BIT16,
    FWBBDBG_ULPWR_CTRL = BIT24,
    FWBBDBG_ULRA = BIT25,
};

/*@--------------------------[Structure]-------------------------------------*/

struct bb_echo_cmd_info {
	u8 cmd_size;
	enum phl_phy_idx echo_phy_idx;
};

struct bb_fw_dbg_cmn_info {
	u8 fw_dbg_comp[4];
	u8 fw_dbg_trace;
	u8 fw_cmn_info;
	u8 fw_rty_rpt_ctrl;
	u8 rsvd0;
};

/*@--------------------------[Prptotype]-------------------------------------*/
struct bb_info;
void halbb_dbgport_dump_all(struct bb_info *bb, u32 *_used, char *output,
			    u32 *_out_len);

void halbb_dbgport_dbg(struct bb_info *bb, char input[][16], u32 *_used,
			 char *output, u32 *_out_len);
void halbb_scanf(char *in, enum bb_scanf_type type, u32 *out);
void halbb_cmd_parser_init(struct bb_info *bb);
void halbb_fw_dbg(struct bb_info *bb, char input[][16], u32 *_used,
			 char *output, u32 *_out_len);
#endif
