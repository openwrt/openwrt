// SPDX-License-Identifier: GPL-2.0
/*
 * Push the persistent configuration into the kernel driver.
 *
 * Identity, thresholds and timers are taken by the MAC at the next
 * activation; the PON mode (GPON / symmetric GPON) can only change while
 * pon0 is down, so a mode change restarts the link through netifd (ubus).
 */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <netlink/attr.h>
#include <netlink/msg.h>

#include "gpond.h"

/*
 * pon0 is brought up and down by netifd only: "ubus call
 * network.interface.<iface> up|down" for the interface that uses it.
 */
int link_set(bool up)
{
	int ret;

	if (!cfg.iface[0]) {
		ERR("main.iface is not set, cannot restart %s", GPON_NETDEV);
		return -EINVAL;
	}

	ret = ubus_iface_call(cfg.iface, up ? "up" : "down");
	if (ret)
		ERR("network.interface.%s %s: %s", cfg.iface, up ? "up" : "down",
		    strerror(-ret));

	return ret;
}

static void get_mode_cb(struct nlattr **tb, void *arg)
{
	if (tb[GPON_ATTR_MODE])
		*(int *)arg = nla_get_u8(tb[GPON_ATTR_MODE]);
}

static int kernel_mode(void)
{
	int mode = -1;

	nl_request(nl_msg(GPON_CMD_GET_STATE, false), get_mode_cb,
		   &mode);

	return mode;
}

static void apply_gpon(void)
{
	struct nl_msg *m;

	/*
	 * Identity and activation go out as one message.  The kernel applies
	 * the attributes in order and stores an identity before it arms, so
	 * a single { SN, PASSWORD, ACTIVATE } is one atomic ask: by the time
	 * the gate opens, the numbers it is going to announce are already in
	 * place.  Sending them in separate messages left a window where the
	 * gate was open and the serial number still the old one.
	 *
	 * Storing an identity un-arms the gate, which is also why ACTIVATE has
	 * to be in the same message rather than a follow-up one.
	 *
	 * When the ONU is configured off the same message closes the gate:
	 * DEACTIVATE takes a running link back to O1, and a later start stays
	 * silent, so "disabled" survives an interface that comes up again.
	 */
	m = nl_msg(GPON_CMD_ONU_CTRL, false);
	if (cfg.sn_set)
		nl_put(m, GPON_ATTR_SN, cfg.sn, GPON_SN_LEN);
	if (cfg.passwd_set)
		nl_put(m, GPON_ATTR_PASSWORD, cfg.passwd,
		       GPON_PASSWD_LEN);
	if (cfg.enabled)
		nl_put_flag(m, GPON_ATTR_ACTIVATE);
	else
		nl_put_flag(m, GPON_ATTR_DEACTIVATE);
	if (cfg.sd)
		nl_put_u8(m, GPON_ATTR_SD, cfg.sd);
	if (cfg.sf)
		nl_put_u8(m, GPON_ATTR_SF, cfg.sf);
	if (cfg.to1)
		nl_put_u32(m, GPON_ATTR_TO1, cfg.to1);
	if (cfg.to2)
		nl_put_u32(m, GPON_ATTR_TO2, cfg.to2);
	if (nl_request(m, NULL, NULL))
		WARN("ONU control settings rejected");
}

int apply_config(bool restart_link)
{
	int cur = kernel_mode();
	bool mode_change = cur >= 0 && cur != cfg.mode;
	int ret;

	if (mode_change || restart_link || !cfg.enabled)
		link_set(false);

	if (mode_change) {
		ret = nl_cmd_u8(GPON_CMD_ONU_CTRL, GPON_ATTR_MODE,
				cfg.mode);
		if (ret) {
			ERR("cannot switch to %s: %s",
			    config_mode_name(cfg.mode), strerror(-ret));
			/* link_set(false) above: do not leave it down */
			if (cfg.enabled)
				link_set(true);
			return ret;
		}
		INFO("PON mode %s -> %s", config_mode_name(cur),
		     config_mode_name(cfg.mode));
	}

	apply_gpon();

	if (cfg.enabled && (mode_change || restart_link))
		link_set(true);

	return 0;
}
