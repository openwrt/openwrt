// SPDX-License-Identifier: GPL-2.0
/*
 * LED trigger for network interfaces.
 *
 * - Aggregated per-family (lan/wan/wlan).
 * - Family and flags are taken from device tree properties:
 *     - "family" : simple family string "lan" | "wan" | "wlan"
 *     - "mode"   : any combination of "link", "tx", "rx" flags
 *   Priority/combination:
 *     * If "mode" present: flags come from "mode" and take precedence.
 *       Family is taken from "family" if present, otherwise from the LED name.
 *     * If only "family" present: use its family and default flags = link+tx+rx.
 *     * If neither present: fall back to LED device name parsing.
 *
 * - Suffix "-online" is valid ONLY in the LED name (label), e.g. "green:wlan-online".
 *   It indicates the online variant but is applied only when DT "mode" is absent.
 *
 * Behaviour:
 * - wlan (normal): blink/solid driven by throughput table
 * - lan/wan (normal): one-shot blink on TX/RX packet change
 * - *-online variants: steady ON while any interface of the family has carrier
 *
 * Interfaces are auto-tracked by name match (lan0, wan1, wlan2, phy0, wl1, ath0, ra0...).
 * Up to MAX_IFACES (16) interfaces per family.
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/jiffies.h>
#include <linux/leds.h>
#include <linux/mutex.h>
#include <linux/netdevice.h>
#include <linux/workqueue.h>
#include <linux/of.h>
#include <linux/list.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/device.h>
#include <linux/ctype.h>
#include <linux/err.h>
#include "../leds.h"

#define MAX_IFACES 16
#define DEFAULT_INTERVAL_MS 50
/* Work reschedule period = 2 * DEFAULT_INTERVAL_MS; named explicitly
 * to avoid silent divergence between the zero-iface and normal paths.
 */
#define WORK_INTERVAL_MS (2 * DEFAULT_INTERVAL_MS)

enum net_trig_type {
	NET_TRIG_LAN = 0,
	NET_TRIG_WAN,
	NET_TRIG_WLAN,
	NET_TRIG_TYPE_MAX,
};

static const char * const type_names[] = {
	[NET_TRIG_LAN] = "lan",
	[NET_TRIG_WAN] = "wan",
	[NET_TRIG_WLAN] = "wlan",
};

/* labels indexed by bitmask: (link<<2)|(tx<<1)|(rx<<0) */
static const char *const labels[] = {
	"",              /* 0 */
	"(rx)",          /* 1 */
	"(tx)",          /* 2 */
	"(tx rx)",       /* 3 */
	"(link)",        /* 4 */
	"(link rx)",     /* 5 */
	"(link tx)",     /* 6 */
	"(link tx rx)",  /* 7 */
};

/* wlan throughput table */
static const struct {
	u32 throughput;
	unsigned long on_ms;
	unsigned long off_ms;
} wlan_tpt_table[] = {
	{   64, 200, 800 },
	{  512, 200, 300 },
	{ 2048, 200, 150 },
	{10000, 200,  50 },
	{54000, 100,  50 },
};

struct net_mgr {
	enum net_trig_type type; /* family: lan/wan/wlan */

	struct mutex lock;
	struct notifier_block notifier;
	struct delayed_work work;

	struct net_device *devs[MAX_IFACES];
	int dev_slot_limit;

	u64 agg_tx_packets;
	u64 agg_rx_packets;
	u64 agg_tx_bytes;
	u64 agg_rx_bytes;

	struct list_head leds;
};

struct net_led {
	struct list_head node;
	struct led_classdev *led_cdev;
	struct net_mgr *mgr;

	u64 last_tx_packets;
	u64 last_rx_packets;
	u64 last_tx_bytes;
	u64 last_rx_bytes;

	bool link;
	bool tx;
	bool rx;
};

static ssize_t net_flag_show(struct device *dev, struct device_attribute *attr, char *buf);
static ssize_t net_flag_store(struct device *dev, struct device_attribute *attr,
			      const char *buf, size_t count);
static ssize_t net_dev_show(struct device *dev, struct device_attribute *attr, char *buf);
static ssize_t net_dev_store(struct device *dev, struct device_attribute *attr,
			     const char *buf, size_t count);

/* sysfs attributes */
static DEVICE_ATTR(link,   0644, net_flag_show, net_flag_store);
static DEVICE_ATTR(tx,     0644, net_flag_show, net_flag_store);
static DEVICE_ATTR(rx,     0644, net_flag_show, net_flag_store);
static DEVICE_ATTR(family, 0644, net_dev_show,  net_dev_store);

/* sysfs attributes: link, tx, rx (per-LED) */
static struct attribute *net_attrs[] = {
	&dev_attr_link.attr,
	&dev_attr_tx.attr,
	&dev_attr_rx.attr,
	&dev_attr_family.attr,
	NULL,
};

static const struct attribute_group net_attr_group = {
	.attrs = net_attrs,
};

static struct net_mgr mgrs[NET_TRIG_TYPE_MAX];

static int net_activate(struct led_classdev *led_cdev);
static void net_deactivate(struct led_classdev *led_cdev);

static struct led_trigger network_trigger = {
	.name = "network",
	.activate = net_activate,
	.deactivate = net_deactivate,
	/* Use .groups + led_trigger_get_drvdata/set_drvdata; avoid device_create_file/remove_file. */
	.groups = (const struct attribute_group *[]) { &net_attr_group, NULL },
};

/* helpers */
static inline void led_set_off_full(struct led_classdev *led, bool on)
{
	led_stop_software_blink(led);
	led_set_brightness(led, on ? LED_FULL : LED_OFF);
}

static inline void led_set_oneshot_ms(struct led_classdev *led, unsigned long ms, bool invert)
{
	led_stop_software_blink(led);
	led_blink_set_oneshot(led, &ms, &ms, invert);
}

/* helper: check that next char after prefix is digit, '-' or NUL */
static inline bool next_char_ok(const char *s, size_t pos)
{
	char c = s[pos];
	return c == '\0' || c == '-' || (c >= '0' && c <= '9');
}

/* name matching: lan/wan; wlan matched by various wifi prefixes with restriction
 * additionally accept ath (Atheros) and ra/rai (Ralink/MediaTek) prefixes.
 * For lan and wan require next char to be digit/'-' or end to avoid accidental matches.
 */
static bool name_matches_type(const char *name, enum net_trig_type type)
{
	if (!name)
		return false;
	switch (type) {

	case NET_TRIG_LAN:
		/* accept "lan", "lanX", "lan-X" */
		if (!strncmp(name, "lan", 3))
			return next_char_ok(name, 3);
		return false;
	case NET_TRIG_WAN:
		/* accept "wan", "wanX", "wan-X" */
		if (!strncmp(name, "wan", 3))
			return next_char_ok(name, 3);
		return false;
	case NET_TRIG_WLAN:
		/* accept "phyX", "wlX"/"wl-..." or "wlan" and common driver prefixes */
		if (!strncmp(name, "phy", 3))
			return next_char_ok(name, 3);
		if (!strncmp(name, "wlan", 4))
			return next_char_ok(name, 4);
		if (!strncmp(name, "wl", 2))
			return next_char_ok(name, 2);
		/* Atheros (athX) */
		if (!strncmp(name, "ath", 3))
			return next_char_ok(name, 3);
		/* Ralink/MediaTek: check "rai" (e.g. rai0) first, then "ra" (ra0) */
		if (!strncmp(name, "rai", 3))
			return next_char_ok(name, 3);
		if (!strncmp(name, "ra", 2))
			return next_char_ok(name, 2);
		return false;
	default:
		return false;
	}
}

/* Extract family type (lan/wan/wlan) from a string.
 * If "online" is supplied, also detects and strips the "-online" suffix.
 * Returns enum net_trig_type or -1 on error.
 */
static int parse_family_token(const char *fn, bool *online)
{
	size_t len;
	const size_t sufflen = sizeof("-online") - 1;

	if (online)
		*online = false;

	if (!fn)
		return -1;

	len = strlen(fn);

	if (online && (len > sufflen) && !strcmp(fn + len - sufflen, "-online")) {
		*online = true;
		len -= sufflen;
	}

	if (len == 3 && !strncmp(fn, "lan", 3))
		return NET_TRIG_LAN;
	if (len == 3 && !strncmp(fn, "wan", 3))
		return NET_TRIG_WAN;
	if (len == 4 && !strncmp(fn, "wlan", 4))
		return NET_TRIG_WLAN;

	return -1;
}

/* Parse only flags from a string: recognized tokens are "link", "tx", "rx".
 * Any other token => -EINVAL. If no flags found, set all three true.
 * Suitable for DT "mode" (strict) or parsing flags from name when no mode present.
 */
static int parse_flags_from_string(const char *fn, bool *link, bool *tx, bool *rx)
{
	size_t len;
	const char *buf = NULL;
	size_t buflen = 0;
	size_t i;
	bool found = false;

	if (!fn || !link || !tx || !rx)
		return -EINVAL;

	*link = false;
	*tx = false;
	*rx = false;

	len = strlen(fn);
	for (i = 0; i < len; i++) {
		if (fn[i] == ' ' || fn[i] == '\t' || fn[i] == ',') {
			buf = NULL;
			buflen = 0;
			continue;
		}

		if (!buf) {
			buf = fn + i;
			buflen = 0;
		}
		buflen++;

		if ((i + 1 == len) || fn[i + 1] == ' ' || fn[i + 1] == '\t' || fn[i + 1] == ',') {
			if (buflen == 4 && !strncmp(buf, "link", 4)) {
				*link = true;
				found = true;
			} else if (buflen == 2 && !strncmp(buf, "tx", 2)) {
				*tx = true;
				found = true;
			} else if (buflen == 2 && !strncmp(buf, "rx", 2)) {
				*rx = true;
				found = true;
			} else {
				/* unknown token */
				return -EINVAL;
			}

			buf = NULL;
			buflen = 0;
		}
	}

	/* default: if no flags found, set all true */
	if (!found) {
		*link = true;
		*tx = true;
		*rx = true;
	}

	return 0;
}

/* safe stats read wrapper */
static void get_dev_stats_safe(struct net_device *dev, struct rtnl_link_stats64 *st)
{
	memset(st, 0, sizeof(*st));
	dev_get_stats(dev, st);
}

/* Update single LED according to manager aggregates and per-LED flags.
 * any_online indicates whether any tracked interface currently has carrier.
 * If LED is online-only (link && !tx && !rx) it is driven directly by any_online.
 */
static void update_led(struct net_led *e, struct net_mgr *m, bool any_online)
{
	unsigned long on_ms, off_ms;
	struct led_classdev *led = e->led_cdev;

	/* defensive: avoid deref if led unexpectedly NULL */
	if (!led)
		return;

	/* If LED requested online-only, reflect any_online */
	if (e->link && !e->tx && !e->rx) {
		led_set_off_full(led, any_online);
		return;
	}

	/* For non-online-only LEDs: if no tracked interface has carrier,
	 * keep the LED off and reset history baseline to avoid spurious deltas
	 * when carrier later returns.
	 */
	if (!any_online) {
		led_set_off_full(led, false);

		e->last_tx_packets = m->agg_tx_packets;
		e->last_rx_packets = m->agg_rx_packets;
		e->last_tx_bytes = m->agg_tx_bytes;
		e->last_rx_bytes = m->agg_rx_bytes;

		return;
	}

	/* non-online-only behaviour depends on family */
	if (m->type == NET_TRIG_WLAN) {
		/* throughput-driven */
		u64 bytes_delta = 0;
		u64 kbps = 0;

		if (e->tx && m->agg_tx_bytes >= e->last_tx_bytes)
			bytes_delta += m->agg_tx_bytes - e->last_tx_bytes;
		if (e->rx && m->agg_rx_bytes >= e->last_rx_bytes)
			bytes_delta += m->agg_rx_bytes - e->last_rx_bytes;

		/* Avoid overflow when multiplying bytes_delta; cap to max u64. */
		if (bytes_delta > ULLONG_MAX / 8) {
			kbps = ULLONG_MAX;
		} else {
			kbps = div64_u64(bytes_delta * 8, WORK_INTERVAL_MS);
		}

		if (kbps == 0)
			led_set_off_full(led, e->link);
		else {
			int idx = 0, t;
			for (t = 0; t < ARRAY_SIZE(wlan_tpt_table); t++) {
				if (kbps >= wlan_tpt_table[t].throughput)
					idx = t;
				else
					break;
			}

			on_ms = wlan_tpt_table[idx].on_ms;
			off_ms = wlan_tpt_table[idx].off_ms;
			if (e->link)
				led_blink_set(led, &on_ms, &off_ms);
			else
				led_blink_set(led, &off_ms, &on_ms);
		}

		e->last_tx_bytes = m->agg_tx_bytes;
		e->last_rx_bytes = m->agg_rx_bytes;
	} else {
		/* LAN/WAN: oneshot on packet-count change */
		u64 tx_sum = m->agg_tx_packets;
		u64 rx_sum = m->agg_rx_packets;

		if ((e->tx && tx_sum != e->last_tx_packets) ||
		    (e->rx && rx_sum != e->last_rx_packets)) {
			unsigned long ms = DEFAULT_INTERVAL_MS;
			led_set_oneshot_ms(led, ms, e->link);
		} else {
			led_set_off_full(led, e->link);
		}
		e->last_tx_packets = tx_sum;
		e->last_rx_packets = rx_sum;
	}
}

/* core work: aggregate + update LEDs */
static void net_mgr_work(struct work_struct *work)
{
	struct net_mgr *m = container_of(work, struct net_mgr, work.work);
	bool any_online = false;
	int i;

	mutex_lock(&m->lock);

	/* aggregate */
	m->agg_tx_packets = m->agg_rx_packets = 0;
	m->agg_tx_bytes = m->agg_rx_bytes = 0;

	/* detect any tracked-interface online (carrier) and sum stats */
	for (i = 0; i < m->dev_slot_limit; i++) {
		struct net_device *dev = m->devs[i];
		struct rtnl_link_stats64 st;

		if (!dev)
			continue;

		get_dev_stats_safe(dev, &st);
		m->agg_tx_packets += st.tx_packets;
		m->agg_rx_packets += st.rx_packets;
		m->agg_tx_bytes += st.tx_bytes;
		m->agg_rx_bytes += st.rx_bytes;

		if (netif_running(dev) && netif_carrier_ok(dev))
			any_online = true;
	}

	/* update each subscribed LED according to its flags and family */
	{
		struct net_led *e;
		list_for_each_entry(e, &m->leds, node)
			update_led(e, m, any_online);
	}

	mutex_unlock(&m->lock);
	schedule_delayed_work(&m->work, msecs_to_jiffies(WORK_INTERVAL_MS));
}

/* Remove device and compact trailing NULLs in devs[].
 * Caller must hold m->lock (or other serializing lock)!
 */
static void net_mgr_remove_dev(struct net_mgr *m, int hole)
{
	int last = m->dev_slot_limit - 1;

	/* Trim trailing NULL slots first */
	while (last > hole && !m->devs[last]) {
		last--;
		m->dev_slot_limit--;
	}

	if (last > hole) {
		m->devs[hole] = m->devs[last];
		m->devs[last] = NULL;
	}
	m->dev_slot_limit--;
}

/* notifier: manage tracked devices */
static int net_mgr_notify(struct notifier_block *nb, unsigned long event, void *ptr)
{
	struct netdev_notifier_info *info = ptr;
	struct net_device *dev = NULL;
	/* to_put collects any reference that must be dropped after mutex release */
	struct net_device *to_put = NULL;
	struct net_mgr *m = container_of(nb, struct net_mgr, notifier);
	int i, id = -1, newid;

	if (event != NETDEV_REGISTER && event != NETDEV_UNREGISTER &&
	    event != NETDEV_CHANGENAME)
		return NOTIFY_DONE;

	if (!info)
		return NOTIFY_DONE;

	dev = info->dev;

	if (!dev)
		return NOTIFY_DONE;

	mutex_lock(&m->lock);
	for (i = 0; i < m->dev_slot_limit; i++) {
		if (m->devs[i] == dev) {
			id = i;
			break;
		}
	}

	switch (event) {
	case NETDEV_UNREGISTER:
		if (id >= 0 && m->devs[id]) {
			to_put = m->devs[id];
			m->devs[id] = NULL;
			pr_info("%s - interface %s unregistered\n", type_names[m->type], dev->name);
			net_mgr_remove_dev(m, id);
		}
		break;
	case NETDEV_CHANGENAME:
		if (id >= 0 && !name_matches_type(dev->name, m->type)) {
			if (m->devs[id]) {
				to_put = m->devs[id];
				m->devs[id] = NULL;
				pr_info("%s - interface renamed to %s (no longer matches), untracked\n",
					type_names[m->type], dev->name);
				net_mgr_remove_dev(m, id);
			}
			break;
		}
		fallthrough;
	case NETDEV_REGISTER:
		if (id < 0 && name_matches_type(dev->name, m->type)) {
			newid = -1;
			for (i = 0; i < m->dev_slot_limit; i++) {
				if (!m->devs[i]) {
					newid = i;
					break;
				}
			}
			if (newid < 0 && m->dev_slot_limit < MAX_IFACES)
				newid = m->dev_slot_limit++;
			if (newid >= 0) {
				dev_hold(dev);
				m->devs[newid] = dev;
				pr_info("%s - interface %s registered\n", type_names[m->type], dev->name);
			}
		}
		break;
	}
	mutex_unlock(&m->lock);

	/* drop reference outside the lock to avoid lock inversion */
	if (to_put)
		dev_put(to_put);

	return NOTIFY_DONE;
}

static struct net_mgr *net_mgr_get(enum net_trig_type type)
{
	if (type >= NET_TRIG_TYPE_MAX)
		return NULL;
	return &mgrs[type];
}

static ssize_t net_flag_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct net_led *entry;
	int val;

	entry = led_trigger_get_drvdata(dev);
	if (!entry)
		return -ENODEV;

	if (attr == &dev_attr_link)
		val = entry->link;
	else if (attr == &dev_attr_tx)
		val = entry->tx;
	else if (attr == &dev_attr_rx)
		val = entry->rx;
	else
		return -EINVAL;

	return sysfs_emit(buf, "%d\n", val);
}

static ssize_t net_flag_store(struct device *dev, struct device_attribute *attr,
			      const char *buf, size_t count)
{
	struct net_led *entry;
	struct net_mgr *mgr;
	bool val;
	int ret;

	entry = led_trigger_get_drvdata(dev);
	if (!entry)
		return -ENODEV;

	ret = kstrtobool(buf, &val);
	if (ret)
		return ret;

	mgr = entry->mgr;
	if (!mgr)
		return -ENODEV;

	/* Safe to lock mgr->lock now. Manager is always alive. */
	mutex_lock(&mgr->lock);

	if (attr == &dev_attr_link)
		entry->link = val;
	else if (attr == &dev_attr_tx)
		entry->tx = val;
	else if (attr == &dev_attr_rx)
		entry->rx = val;
	else {
		ret = -EINVAL;
		goto out_unlock;
	}

	ret = count;

	/* Request immediate update - safely accessing mgr->work now */
	schedule_delayed_work(&mgr->work, 0);

	pr_info("LED %s - network trigger flags changed to %s%s%s\n",
		dev_name(entry->led_cdev->dev),
		entry->link ? "link " : "",
		entry->tx ? "tx " : "",
		entry->rx ? "rx" : "");

out_unlock:
	mutex_unlock(&mgr->lock);

	return ret;
}

static ssize_t net_dev_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct net_led *entry;
	struct net_mgr *mgr;
	ssize_t ret;

	entry = led_trigger_get_drvdata(dev);
	if (!entry)
		return -ENODEV;

	mgr = entry->mgr;
	if (!mgr)
		return -ENODEV;

	/* From this moment, safe to dereference mgr */
	ret = sysfs_emit(buf, "%s\n", type_names[mgr->type]);

	return ret;
}

static ssize_t net_dev_store(struct device *dev, struct device_attribute *attr,
			     const char *buf, size_t count)
{
	char tmp[32];
	char *trimmed;
	size_t len = count;
	struct net_led *entry;
	int parsed;
	struct net_mgr *old_mgr = NULL, *new_mgr;
	size_t i;

	entry = led_trigger_get_drvdata(dev);
	if (!entry)
		return -ENODEV;

	old_mgr = entry->mgr;
	if (!old_mgr)
		return -ENODEV;

	if (len == 0 || len >= sizeof(tmp))
		return -EINVAL;

	/* copy raw input from sysfs buffer and ensure null-termination */
	memcpy(tmp, buf, len);
	tmp[len] = '\0';

	/* trim leading/trailing spaces, tabs, and newlines (\r, \n) */
	trimmed = strim(tmp);
	len = strlen(trimmed);

	/* reject internal spaces/tabs inside the token */
	for (i = 0; i < len; i++) {
		if (trimmed[i] == ' ' || trimmed[i] == '\t')
			return -EINVAL;
	}

	parsed = parse_family_token(trimmed, NULL);
	if (parsed < 0)
		return -EINVAL;

	if (old_mgr->type == parsed) {
		return count; /* no change */
	}

	new_mgr = net_mgr_get(parsed);
	if (!new_mgr)
		return -EINVAL;

	/* Move entry between manager lists.
	 * To avoid races, take both manager locks while
	 * manipulating lists and updating history.
	 */
	if (old_mgr < new_mgr) {
		mutex_lock(&old_mgr->lock);
		mutex_lock(&new_mgr->lock);
	} else if (old_mgr > new_mgr) {
		mutex_lock(&new_mgr->lock);
		mutex_lock(&old_mgr->lock);
	} else {
		mutex_lock(&old_mgr->lock);
	}

	/* Verify entry still belongs to old_mgr */
	if (entry->mgr != old_mgr) {
		if (old_mgr != new_mgr)
			mutex_unlock(&new_mgr->lock);
		mutex_unlock(&old_mgr->lock);
		return -ENODEV;
	}

	list_del(&entry->node);
	list_add_tail(&entry->node, &new_mgr->leds);
	entry->mgr = new_mgr;

	/* initialize history so future reads won't see a spurious delta */
	entry->last_tx_packets = new_mgr->agg_tx_packets;
	entry->last_rx_packets = new_mgr->agg_rx_packets;
	entry->last_tx_bytes   = new_mgr->agg_tx_bytes;
	entry->last_rx_bytes   = new_mgr->agg_rx_bytes;

	pr_info("LED %s - network trigger family changed to %s\n",
		dev_name(entry->led_cdev->dev), type_names[parsed]);

	/* request immediate update - use local new_mgr, not entry->mgr (no lock held) */
	schedule_delayed_work(&new_mgr->work, 0);

	if (old_mgr != new_mgr)
		mutex_unlock(&new_mgr->lock);
	mutex_unlock(&old_mgr->lock);

	return count;
}

/* deactivate: detach led from manager */
static void net_deactivate(struct led_classdev *led_cdev)
{
	struct net_led *entry;
	struct net_mgr *m;

	entry = led_trigger_get_drvdata(led_cdev->dev);
	if (!entry)
		return;

	m = entry->mgr;
	if (!m) {
		/* defensive: shouldn't happen, but avoid crash */
		led_set_trigger_data(led_cdev, NULL);
		kfree(entry);
		return;
	}

	mutex_lock(&m->lock);

	if (entry->mgr == m) {
		list_del(&entry->node);
		mutex_unlock(&m->lock);

		led_set_off_full(led_cdev, false);
		led_set_trigger_data(led_cdev, NULL);

		pr_info("LED %s - trigger %s%s detached\n",
			dev_name(led_cdev->dev),
			type_names[m->type],
			labels[(entry->link << 2) | (entry->tx << 1) | (entry->rx << 0)]);
	} else {
		mutex_unlock(&m->lock);
	}
	kfree(entry);
}

/* activate/deactivate: attach led to manager and remember flags */
static int net_activate(struct led_classdev *led_cdev)
{
	const char *fn = NULL;
	const char *dt_family = NULL;
	const char *dt_mode = NULL;
	int parsed = -1;
	bool link = false;
	bool tx = false;
	bool rx = false;
	bool online;
	struct net_mgr *m;
	struct net_led *entry;
	const char *name;
	const char *sep;
	int ret;

	if (!led_cdev) {
		pr_err("network: net_activate called with NULL led_cdev\n");
		return -EINVAL;
	}
	if (!led_cdev->dev) {
		pr_err("network: LED device is NULL, aborting activate\n");
		return -EINVAL;
	}
	name = dev_name(led_cdev->dev);
	if (!name || !*name) {
		pr_err("network: LED has no name, aborting activate\n");
		return -EINVAL;
	}
	if (led_trigger_get_drvdata(led_cdev->dev)) {
		pr_warn("network: LED %s already has trigger_data set, refusing attach\n",
			name);
		return -EBUSY;
	}

	if (led_cdev->dev->of_node) {
		of_property_read_string(led_cdev->dev->of_node, "family", &dt_family);
		of_property_read_string(led_cdev->dev->of_node, "mode", &dt_mode);
	}

	/* function part from name (after last ':') */
	sep = strrchr(name, ':');
	if (sep && sep[1] != '\0')
		fn = sep + 1;
	else
		fn = name;

	if (dt_mode) {
		/* DT mode present: strict parsing, empty string is invalid */
		if (*dt_mode == '\0')
			return -EINVAL;
		ret = parse_flags_from_string(dt_mode, &link, &tx, &rx);
		if (ret)
			return -EINVAL;

		/* family: from dt_family if present, otherwise from name */
		if (dt_family) {
			parsed = parse_family_token(dt_family, NULL);
			if (parsed < 0) {
				pr_info("network: invalid family '%s' for LED %s\n", dt_family, name);
				return -EINVAL;
			}
		} else {
			/* parse family from name; online flag parsed but not used here */
			parsed = parse_family_token(fn, &online);
			if (parsed < 0) {
				pr_info("network: no family in name and no family for LED %s\n", name);
				return -EINVAL;
			}
		}
	} else if (dt_family) {
		/* Only dt_family present: use its family. Flags depend on name "-online" */
		parsed = parse_family_token(dt_family, NULL);
		if (parsed < 0) {
			pr_info("network: invalid family '%s' for LED %s\n", dt_family, name);
			return -EINVAL;
		}
		/* test whether name contains "-online" */
		parse_family_token(fn, &online);
		if (online) {
			/* name indicated online variant and no mode -> online-only */
			link = true;
			tx = false;
			rx = false;
		} else {
			link = tx = rx = true;
		}
	} else {
		/* No DT properties: family and flags come from the LED name.
		 * If name had "-online" -> online-only.
		 */
		parsed = parse_family_token(fn, &online);
		if (parsed < 0) {
			pr_info("network: unknown function '%s' for LED %s\n", fn ?: "<NULL>", name);
			return -EINVAL;
		}
		if (online) {
			/* online variant in name => online-only */
			link = true;
			tx = false;
			rx = false;
		} else {
			link = tx = rx = true;
		}
	}

	m = net_mgr_get(parsed);
	if (!m)
		return -EINVAL;

	entry = kzalloc(sizeof(*entry), GFP_KERNEL);
	if (!entry) {
		return -ENOMEM;
	}

	entry->led_cdev = led_cdev;
	entry->mgr = m;
	entry->last_tx_packets = entry->last_rx_packets = 0;
	entry->last_tx_bytes = entry->last_rx_bytes = 0;
	entry->link = link;
	entry->tx = tx;
	entry->rx = rx;

	/* attach under manager lock and initialize history to current aggregates */
	mutex_lock(&m->lock);
	list_add_tail(&entry->node, &m->leds);

	/* initialize history so future reads won't see a spurious delta */
	entry->last_tx_packets = m->agg_tx_packets;
	entry->last_rx_packets = m->agg_rx_packets;
	entry->last_tx_bytes   = m->agg_tx_bytes;
	entry->last_rx_bytes   = m->agg_rx_bytes;
	mutex_unlock(&m->lock);

	/* attach per-LED trigger data using LED core helper */
	led_set_trigger_data(led_cdev, entry);

	pr_info("LED %s - trigger %s%s attached\n",
		name,
		type_names[m->type],
		labels[(entry->link << 2) | (entry->tx << 1) | (entry->rx << 0)]);
	return 0;
}

/* Static managers need one-time init before trigger registration.
 */
static int __init net_trig_init(void)
{
	int i;

	for (i = 0; i < NET_TRIG_TYPE_MAX; i++) {
		struct net_mgr *m = &mgrs[i];
		m->type = i;
		mutex_init(&m->lock);
		INIT_LIST_HEAD(&m->leds);
		INIT_DELAYED_WORK(&m->work, net_mgr_work);
		m->notifier.notifier_call = net_mgr_notify;
		m->notifier.priority = 0;
		register_netdevice_notifier(&m->notifier);
		schedule_delayed_work(&m->work, 0);
	}

	return led_trigger_register(&network_trigger);
}

static void __exit net_trig_exit(void)
{
	int i, j;

	led_trigger_unregister(&network_trigger);

	for (i = 0; i < NET_TRIG_TYPE_MAX; i++) {
		struct net_mgr *m = &mgrs[i];
		cancel_delayed_work_sync(&m->work);
		unregister_netdevice_notifier(&m->notifier);
		mutex_lock(&m->lock);
		for (j = 0; j < m->dev_slot_limit; j++) {
			if (m->devs[j]) {
				dev_put(m->devs[j]);
				m->devs[j] = NULL;
			}
		}
		mutex_unlock(&m->lock);
	}
}

module_init(net_trig_init);
module_exit(net_trig_exit);

MODULE_AUTHOR("Mieczyslaw Nalewaj <namiltd@yahoo.com>");
MODULE_DESCRIPTION("LED trigger for network interfaces - aggregated by family; supports link/tx/rx and -online");
MODULE_LICENSE("GPL");
