/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (c) 2023 Airoha Inc.
 * Author: Min Yao <min.yao@airoha.com>
 */

#ifndef _AN8855_NL_H_
#define _AN8855_NL_H_

#define AN8855_DSA_GENL_NAME "an8855_dsa"
#define AN8855_GENL_VERSION		0x1

enum an8855_cmd {
	AN8855_CMD_UNSPEC = 0,
	AN8855_CMD_REQUEST,
	AN8855_CMD_REPLY,
	AN8855_CMD_READ,
	AN8855_CMD_WRITE,

	__AN8855_CMD_MAX,
};

enum an8855_attr {
	AN8855_ATTR_TYPE_UNSPEC = 0,
	AN8855_ATTR_TYPE_MESG,
	AN8855_ATTR_TYPE_PHY,
	AN8855_ATTR_TYPE_DEVAD,
	AN8855_ATTR_TYPE_REG,
	AN8855_ATTR_TYPE_VAL,
	AN8855_ATTR_TYPE_DEV_NAME,
	AN8855_ATTR_TYPE_DEV_ID,

	__AN8855_ATTR_TYPE_MAX,
};

#define AN8855_NR_ATTR_TYPE		(__AN8855_ATTR_TYPE_MAX - 1)

#ifdef __KERNEL__
int an8855_nl_init(struct an8855_priv **priv);
void an8855_nl_exit(void);
#endif /* __KERNEL__ */

#endif /* _AN8855_NL_H_ */
