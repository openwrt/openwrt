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
#ifndef _PHL_SER_DBG_CMD_H_
#define _PHL_SER_DBG_CMD_H_

void phl_ser_cmd_parser(struct phl_info_t *phl_info, char input[][MAX_ARGV],
			u32 input_num, char *output, u32 out_len);

#endif
