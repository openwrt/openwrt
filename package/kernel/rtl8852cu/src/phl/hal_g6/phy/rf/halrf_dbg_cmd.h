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
#ifndef _HALRF_DBG_CMD_H_
#define _HALRF_DBG_CMD_H_
/*@--------------------------[Define] ---------------------------------------*/
#define	RF_MAX_ARGC		20
#define	RF_MAX_ARGV		16

#define HALRF_SCAN halrf_scanf
#define HALRF_DCMD_SCAN_LIMIT 10
/*@--------------------------[Enum]------------------------------------------*/
 enum rf_scanf_type
{
	DCMD_DECIMAL	= 1,
	DCMD_HEX	= 2,
	DCMD_CHAR	= 3,
};
/*@--------------------------[Structure]-------------------------------------*/

struct rf_dbg_cmd_info {
	u8 cmd_size;
};

/*@--------------------------[Prptotype]-------------------------------------*/
void halrf_cmd_parser_init(struct rf_info *rf);

#endif
