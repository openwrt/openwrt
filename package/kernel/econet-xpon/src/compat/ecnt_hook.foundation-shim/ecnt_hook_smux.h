/*
 * compat shim for <ecnt_hook/ecnt_hook_smux.h>
 * SMUX (source-mac demux) get-dev-by-mac API payload.
 */
#ifndef _COMPAT_ECNT_HOOK_SMUX_H_
#define _COMPAT_ECNT_HOOK_SMUX_H_
#include "ecnt_hook/ecnt_common.h"

#define SMUX_ECNT_HOOK_PROCESS_SUCESS	(0)

enum { SMUX_API_TYPE_GET = 0, SMUX_API_TYPE_SET };
enum { SMUX_GET_DEV_BY_SRC_MAC = 0 };

typedef struct {
	struct sk_buff *skb;
	struct net_device **dev;
} SMUX_Dev_By_Mac;

struct smux_api_data_s {
	int api_type;
	int cmd_id;
	SMUX_Dev_By_Mac *smux_dev_by_mac;
	int ret;
};
#endif
