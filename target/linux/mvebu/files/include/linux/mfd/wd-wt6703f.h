/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _MFD_WD_WT6703F_H_
#define _MFD_WD_WT6703F_H_

#include <linux/types.h>

struct wd_wt6703f;

int wd_wt6703f_exec(struct wd_wt6703f *wt6703f, const u8 *cmd, size_t cmd_len,
			u8 *reply, size_t *reply_len);

#endif /* _MFD_WD_WT6703F_H_ */
