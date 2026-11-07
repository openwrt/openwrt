// SPDX-License-Identifier: GPL-2.0
/*
 * Kernel notifications: OMCI frames ("omci" group), state changes,
 * unhandled PLOAM and ToD updates ("event" group).
 */
#include <netlink/attr.h>

#include "gpond.h"

void event_handle(int cmd, struct nlattr **tb)
{
	switch (cmd) {
	case GPON_CMD_OMCI:
		if (tb[GPON_ATTR_OMCI])
			omci_rx(nla_data(tb[GPON_ATTR_OMCI]),
				nla_len(tb[GPON_ATTR_OMCI]));
		break;
	case GPON_CMD_GET_STATE:
		if (tb[GPON_ATTR_STATE]) {
			omci_state(nla_get_u8(tb[GPON_ATTR_STATE]),
				   tb[GPON_ATTR_ONU_ID] ?
				   nla_get_u8(tb[GPON_ATTR_ONU_ID]) : -1);
			/* after the state: reports need O5 */
			if (tb[GPON_ATTR_ALARMS])
				monitor_gpon_alarms(nla_get_u32(
					tb[GPON_ATTR_ALARMS]));
		}
		ubus_notify_state(tb);
		break;
	case GPON_CMD_PLOAM:
		if (tb[GPON_ATTR_PLOAM]) {
			const uint8_t *p = nla_data(tb[GPON_ATTR_PLOAM]);

			DBG("unhandled PLOAM id 0x%02x for ONU 0x%02x",
			    p[1], p[0]);
		}
		break;
	case GPON_CMD_TOD:
		/* ToD taken over at the first 1PPS after an update */
		if (tb[GPON_ATTR_TOD_SEC] && tb[GPON_ATTR_TOD_NSEC])
			ubus_notify_tod(nla_get_u32(tb[GPON_ATTR_TOD_SEC]),
					nla_get_u32(tb[GPON_ATTR_TOD_NSEC]));
		break;
	default:
		break;
	}
}
