// SPDX-License-Identifier: GPL-2.0-only

#include <linux/bitops.h>
#include <linux/cleanup.h>
#include <linux/debugfs.h>
#include <linux/etherdevice.h>
#include <linux/seq_file.h>
#include <linux/soc/realtek/otto_table.h>
#include <linux/string.h>

#include "fdb.h"
#include "l2.h"
#include "l3.h"
#include "lag.h"
#include "rtl-otto.h"

#define RTL930X_PORT_IGNORE 0x3f
#define RTLDSA_L2_L3_REFCOUNT_MAX	0x7fff

/* RTL931x hashes its second block into rows the fib_entries count does not
 * reach, so an index can fall outside the map and is simply not tracked.
 */
struct rtldsa_l2_uc *otto_l2_uc_lookup(struct otto_dsa_priv *priv, int idx)
{
	if (idx < 0 || idx >= priv->r->fib_entries)
		return NULL;

	return &priv->l2_uc_map[idx];
}

/* The count pins at its ceiling rather than wrapping: an entry that reached it
 * is never freed by the driver again, which leaks a row but never hands a live
 * one to somebody else.
 */
static void otto_l2_uc_get(struct otto_dsa_priv *priv, int idx)
{
	struct rtldsa_l2_uc *m = otto_l2_uc_lookup(priv, idx);

	if (!m)
		return;

	if (m->l3_refcount == RTLDSA_L2_L3_REFCOUNT_MAX) {
		dev_warn_once(priv->dev, "L2 entry %d has too many routes to count\n", idx);
		return;
	}

	m->l3_refcount++;
}

static void otto_l2_uc_put(struct otto_dsa_priv *priv, int idx)
{
	struct rtldsa_l2_uc *m = otto_l2_uc_lookup(priv, idx);

	if (!m || !m->l3_refcount || m->l3_refcount == RTLDSA_L2_L3_REFCOUNT_MAX)
		return;

	m->l3_refcount--;
}

/* Give the row back once nothing forwards through it any more. @e has to be a
 * fresh read of the row at nh->l2_id.
 */
static void otto_l2_uc_release_row(struct otto_dsa_priv *priv,
				   struct otto_l3_nexthop *nh,
				   struct rtl838x_l2_entry *e)
{
	struct rtldsa_l2_uc *m = otto_l2_uc_lookup(priv, nh->l2_id);

	/* Another route is still forwarding through this entry: it has to stay
	 * exactly as it is, next hop and route id included.
	 */
	if (m && m->l3_refcount)
		return;

	/* The bridge put this address here as well, so the entry stays; it
	 * just stops being a next hop.
	 */
	if (e->is_static && (!m || !m->fdb_ref))
		e->valid = false;
	e->next_hop = false;
	/* A DMAC entry or a route id takes that field on the families that
	 * keep one, so what goes back is the relay VID, which the row still
	 * carries either way.
	 */
	e->vid = e->rvid;

	priv->r->write_l2_entry_using_hash(priv, nh->l2_id >> 2, nh->l2_id & 0x3, e);
}

/* Release a reference taken on a remembered index. The switch drops rows on
 * its own, by ageing and by the per-port flush the bridge asks for, and
 * whoever claims one next counts itself from zero: that count is not ours to
 * spend. Search on the seed the reference was taken on, because the caller has
 * already overwritten the address.
 */
static void otto_l2_uc_put_row(struct otto_dsa_priv *priv,
			       struct otto_l3_nexthop *nh)
{
	struct rtl838x_l2_entry e = {};

	if (otto_l2_find_hash_entry(priv, nh->l2_seed, true, &e) != nh->l2_id)
		return;

	if (!e.next_hop)
		return;

	otto_l2_uc_put(priv, nh->l2_id);
	otto_l2_uc_release_row(priv, nh, &e);
}

/* Add an L2 nexthop entry for the L3 routing system / PIE forwarding in the SoC
 * Use VID and MAC in rtl838x_l2_entry to find the entry the switch holds for the
 * gateway and mark it as a nexthop by setting its nexthop bit
 * Called from the L3 layer
 * The index in the L2 hash table is filled into nh->l2_id;
 */
int otto_l2_nexthop_add(struct otto_dsa_priv *priv, struct otto_l3_nexthop *nh)
{
	struct rtl838x_l2_entry e = {};
	u64 seed = priv->r->l2_hash_seed(nh->mac, nh->rvid);
	int idx;

	pr_debug("%s searching for %08llx vid %d, seed: %016llx\n",
		 __func__, nh->mac, nh->rvid, seed);

	/* The search, the count and the write are one step: anything else may
	 * claim the entry we settled on in between.
	 */
	guard(mutex)(&priv->reg_mutex);

	idx = otto_l2_find_hash_entry(priv, seed, false, &e);
	if (idx < 0) {
		pr_err("%s: No more L2 forwarding entries available\n", __func__);
		return -1;
	}

	/* With a next hop that has no port, an RTL930x delivers every forwarded
	 * frame twice, once from the switch and once from the Linux stack, and
	 * an RTL839x cuts the gateway off from the switch. A row invented here
	 * would be static, and nothing would replace it: dsa_user_fdb_event()
	 * drops an address the software bridge learns on an offloaded port
	 * before the driver sees it. The caller traps the route, leaves the
	 * slot to the address, and lets the switch learn it.
	 */
	if (!e.valid) {
		if (nh->l2_installed)
			otto_l2_uc_put_row(priv, nh);
		nh->l2_installed = false;
		return -ENOENT;
	}

	/* Found an existing entry, make it a nexthop entry */
	if (nh->l2_installed && nh->l2_id != idx)
		otto_l2_uc_put_row(priv, nh);

	if (!nh->l2_installed || nh->l2_id != idx)
		otto_l2_uc_get(priv, idx);

	nh->l2_id = idx;
	nh->l2_seed = seed;
	nh->port = e.port;
	nh->rvid = e.rvid;
	nh->dev_id = e.stack_dev;
	/* If the entry is already a valid next hop entry, don't change it, but
	 * for the DMAC entry it names: every route through the entry holds that
	 * one, and the entry may still name one given up while it stayed a next
	 * hop, or, on RTL839x, the row of the device's old MAC.
	 */
	if (e.next_hop) {
		if (e.nh_route_id == nh->dmac_id)
			return 0;
		e.nh_route_id = nh->dmac_id;
		priv->r->write_l2_entry_using_hash(priv, idx >> 2, idx & 0x3, &e);
		return 0;
	}
	e.next_hop = true;
	e.nh_route_id = nh->dmac_id;		/* DMAC entry or route ID takes place of VID */
	e.nh_vlan_target = false;

	priv->r->write_l2_entry_using_hash(priv, idx >> 2, idx & 0x3, &e);

	return 0;
}

bool otto_l2_nexthop_current(struct otto_dsa_priv *priv, struct otto_l3_nexthop *nh)
{
	struct rtl838x_l2_entry e = {};

	if (!nh->l2_installed)
		return false;

	guard(mutex)(&priv->reg_mutex);

	return otto_l2_find_hash_entry(priv, nh->l2_seed, true, &e) == nh->l2_id &&
	       e.next_hop && e.port == nh->port;
}

/* Removes a Layer 2 next hop entry in the forwarding database
 * If it was static, the entire entry is removed, otherwise the nexthop bit is cleared
 * and we wait until the entry ages out
 */
int otto_l2_nexthop_del(struct otto_dsa_priv *priv, struct otto_l3_nexthop *nh)
{
	struct rtl838x_l2_entry e = {};
	u32 key = nh->l2_id >> 2;
	int i = nh->l2_id & 0x3;
	int idx;

	guard(mutex)(&priv->reg_mutex);

	dev_dbg(priv->dev, "next hop %d sits at key %d, index %d\n", nh->l2_id, key, i);

	/* Search on the seed the installer claimed the row on, because the
	 * caller replaces the address before every install. Landing on the
	 * recorded index answers both questions at once: the row still holds
	 * the address that was installed, and it is still the same row. A
	 * negative index means the address has left the bucket altogether.
	 */
	idx = otto_l2_find_hash_entry(priv, nh->l2_seed, true, &e);
	if (idx != nh->l2_id) {
		dev_err(priv->dev, "next hop %d is at %d now, not removing it\n",
			nh->l2_id, idx);
		return -ESTALE;
	}

	if (!e.next_hop) {
		dev_err(priv->dev, "next hop %d is no longer one, leaving it alone\n",
			nh->l2_id);
		return -ESTALE;
	}

	otto_l2_uc_put(priv, nh->l2_id);
	otto_l2_uc_release_row(priv, nh, &e);

	return 0;
}

static void rtldsa_setup_l2_uc_entry(struct rtl838x_l2_entry *e, int port,
				     int vid, u64 mac)
{
	bool next_hop = e->valid && e->next_hop;
	u16 nh_route_id = next_hop ? e->nh_route_id : 0;

	memset(e, 0, sizeof(*e));

	e->type = L2_UNICAST;
	e->valid = true;

	/* A route may be forwarding through this address already, and the id
	 * it is known by lives nowhere else.
	 */
	e->next_hop = next_hop;
	e->nh_route_id = nh_route_id;

	e->age = 3;
	e->is_static = true;

	e->port = port;

	e->rvid = e->vid = vid;
	e->is_ip_mc = e->is_ipv6_mc = false;
	u64_to_ether_addr(mac, e->mac);
}

int rtldsa_port_fdb_add(struct dsa_switch *ds, int port,
			       const unsigned char *addr, u16 vid,
			       const struct dsa_db db)
{
	struct otto_dsa_priv *priv = ds->priv;
	u64 mac = ether_addr_to_u64(addr);
	struct rtl838x_l2_entry e;
	int err = 0, idx;
	u64 seed = priv->r->l2_hash_seed(mac, vid);
	int lag_group = otto_lag_find_group_from_port(priv, port);

	if (lag_group >= 0 && priv->r->prepare_lag_fdb) {
		priv->r->prepare_lag_fdb(&e, lag_group);
	} else {
		if (priv->lag_non_primary & BIT_ULL(port)) {
			pr_debug("%s: %d is lag slave but prepare_lag_fdb is not supported. ignore\n",
				 __func__, port);
			return 0;
		}
	}

	mutex_lock(&priv->reg_mutex);

	idx = otto_l2_find_hash_entry(priv, seed, false, &e);

	/* Found an existing or empty entry */
	if (idx >= 0) {
		struct rtldsa_l2_uc *m = otto_l2_uc_lookup(priv, idx);

		if (m) {
			/* A slot nobody had claimed carries whatever its last
			 * owner left behind.
			 */
			if (!e.valid)
				*m = (struct rtldsa_l2_uc){};
			m->fdb_ref = true;
		}

		rtldsa_setup_l2_uc_entry(&e, port, vid, mac);
		priv->r->write_l2_entry_using_hash(priv, idx >> 2, idx & 0x3, &e);
		goto out;
	}

	/* Hash buckets full, try CAM */
	idx = otto_l2_find_cam_entry(priv, seed, false, &e);

	if (idx >= 0) {
		rtldsa_setup_l2_uc_entry(&e, port, vid, mac);
		priv->r->write_cam(idx, &e);
		goto out;
	}

	err = -ENOTSUPP;

out:
	mutex_unlock(&priv->reg_mutex);

	return err;
}

int rtldsa_port_fdb_del(struct dsa_switch *ds, int port,
			       const unsigned char *addr, u16 vid,
			       const struct dsa_db db)
{
	struct otto_dsa_priv *priv = ds->priv;
	u64 mac = ether_addr_to_u64(addr);
	struct rtl838x_l2_entry e;
	int err = 0, idx;
	u64 seed = priv->r->l2_hash_seed(mac, vid);
	int lag_group = otto_lag_find_group_from_port(priv, port);

	if (lag_group >= 0 && priv->r->prepare_lag_fdb)
		priv->r->prepare_lag_fdb(&e, lag_group);

	pr_debug("In %s, mac %llx, vid: %d\n", __func__, mac, vid);

	mutex_lock(&priv->reg_mutex);

	idx = otto_l2_find_hash_entry(priv, seed, true, &e);

	if (idx >= 0) {
		struct rtldsa_l2_uc *m = otto_l2_uc_lookup(priv, idx);

		pr_debug("Found entry index %d, key %d and bucket %d\n", idx, idx >> 2, idx & 3);

		if (m)
			m->fdb_ref = false;

		/* A route still forwarding through this address keeps it, as
		 * the next hop it already is, reachable through no port of its
		 * own.
		 */
		if (m && m->l3_refcount && e.next_hop) {
			e.port = priv->r->port_ignore;
			e.age = 0;
		} else {
			e.valid = false;
		}

		priv->r->write_l2_entry_using_hash(priv, idx >> 2, idx & 0x3, &e);
		goto out;
	}

	/* Check CAM for spillover from hash buckets */
	idx = otto_l2_find_cam_entry(priv, seed, true, &e);

	if (idx >= 0) {
		e.valid = false;
		priv->r->write_cam(idx, &e);
		goto out;
	}
	err = -ENOENT;

out:
	mutex_unlock(&priv->reg_mutex);

	return err;
}

int rtldsa_port_fdb_dump(struct dsa_switch *ds, int port,
				dsa_fdb_dump_cb_t *cb, void *data)
{
	struct otto_dsa_priv *priv = ds->priv;
	int uc_rows, cam_rows;

	uc_rows = otto_table_rows(priv->r->l2_uc_tbl);
	if (uc_rows < 0)
		return uc_rows;

	cam_rows = otto_table_rows(priv->r->l2_cam_tbl);
	if (cam_rows < 0)
		return cam_rows;

	mutex_lock(&priv->reg_mutex);

	for (int i = 0; i < uc_rows; i++) {
		struct rtl838x_l2_entry e = {};

		if (!((i + 1) % 64))
			cond_resched();

		priv->r->read_l2_entry_using_hash(i >> 2, i & 0x3, &e);

		if (!e.valid || e.type != L2_UNICAST || e.is_trunk)
			continue;

		if (e.port == port || e.port == RTL930X_PORT_IGNORE)
			cb(e.mac, e.rvid, e.is_static, data);
	}

	for (int i = 0; i < cam_rows; i++) {
		struct rtl838x_l2_entry e = {};

		priv->r->read_cam(i, &e);

		if (!e.valid || e.type != L2_UNICAST || e.is_trunk)
			continue;

		if (e.port == port)
			cb(e.mac, e.rvid, e.is_static, data);
	}

	mutex_unlock(&priv->reg_mutex);

	return 0;
}

static void otto_l2_dump_entry(struct seq_file *m, struct otto_dsa_priv *priv,
			       struct rtl838x_l2_entry *e)
{
	u64 portmask;

	if (e->type == L2_UNICAST) {
		seq_puts(m, "L2_UNICAST\n");

		seq_printf(m, "  mac %02x:%02x:%02x:%02x:%02x:%02x vid %u rvid %u\n",
			   e->mac[0], e->mac[1], e->mac[2], e->mac[3], e->mac[4], e->mac[5],
			   e->vid, e->rvid);

		seq_printf(m, "  port %d age %d", e->port, e->age);
		if (e->is_trunk) {
			if (e->trunk < ARRAY_SIZE(priv->lags_port_members))
				seq_printf(m, "  trunk %d trunk_members: 0x%08llx non-primary: 0x%08llx primary-port: %d",
					   e->trunk,
					   priv->lags_port_members[e->trunk],
					   priv->lag_non_primary,
					   priv->lag_primary[e->trunk]);
			else
				seq_printf(m, "  trunk %d (out of range)", e->trunk);
		}
		if (e->is_static)
			seq_puts(m, " static");
		if (e->block_da)
			seq_puts(m, " block_da");
		if (e->block_sa)
			seq_puts(m, " block_sa");
		if (e->suspended)
			seq_puts(m, " suspended");
		if (e->next_hop)
			seq_printf(m, " next_hop route_id %u", e->nh_route_id);
		seq_puts(m, "\n");

	} else {
		if (e->type == L2_MULTICAST) {
			seq_puts(m, "L2_MULTICAST\n");

			seq_printf(m, "  mac %02x:%02x:%02x:%02x:%02x:%02x vid %u rvid %u\n",
				   e->mac[0], e->mac[1], e->mac[2], e->mac[3], e->mac[4], e->mac[5],
				   e->vid, e->rvid);
		}

		if (e->type == IP4_MULTICAST || e->type == IP6_MULTICAST) {
			seq_puts(m, (e->type == IP4_MULTICAST) ?
				 "IP4_MULTICAST\n" : "IP6_MULTICAST\n");

			seq_printf(m, "  gip %08x sip %08x vid %u rvid %u\n",
				   e->mc_gip, e->mc_sip, e->vid, e->rvid);
		}

		portmask = priv->r->read_mcast_pmask(e->mc_portmask_index);
		seq_printf(m, "  index %u ports", e->mc_portmask_index);
		for (int i = 0; i < 64; i++) {
			if (portmask & BIT_ULL(i))
				seq_printf(m, " %d", i);
		}
		seq_puts(m, "\n");
	}

	seq_puts(m, "\n");
}

static int otto_l2_fdb_show(struct seq_file *m, void *v)
{
	struct otto_dsa_priv *priv = m->private;
	int uc_rows, cam_rows;

	uc_rows = otto_table_rows(priv->r->l2_uc_tbl);
	if (uc_rows < 0)
		return uc_rows;

	cam_rows = otto_table_rows(priv->r->l2_cam_tbl);
	if (cam_rows < 0)
		return cam_rows;

	guard(mutex)(&priv->reg_mutex);

	/* With a position below 4, read_l2_entry_using_hash() reads row
	 * (bucket << 2 | pos) on every family, so this walk visits each row
	 * once, the second hash block of RTL930x/RTL931x included.
	 */
	for (int i = 0; i < uc_rows; i++) {
		struct rtl838x_l2_entry e = {};

		priv->r->read_l2_entry_using_hash(i >> 2, i & 0x3, &e);

		if (e.valid) {
			seq_printf(m, "Hash table bucket %d index %d ", i >> 2, i & 0x3);
			otto_l2_dump_entry(m, priv, &e);
		}

		if (!((i + 1) % 64))
			cond_resched();
	}

	for (int i = 0; i < cam_rows; i++) {
		struct rtl838x_l2_entry e = {};

		priv->r->read_cam(i, &e);

		if (!e.valid)
			continue;

		seq_printf(m, "CAM index %d ", i);
		otto_l2_dump_entry(m, priv, &e);
	}

	return 0;
}
DEFINE_SHOW_ATTRIBUTE(otto_l2_fdb);

static void otto_l2_dbgfs_remove(void *data)
{
	debugfs_remove_recursive(data);
}

#define RTLDSA_L2_DBG_ROOT_DIR	"realtek_otto_l2"

void otto_l2_dbgfs_init(struct otto_dsa_priv *priv)
{
	struct device *dev = priv->dev;
	struct dentry *root;

	root = debugfs_create_dir(RTLDSA_L2_DBG_ROOT_DIR, NULL);
	if (IS_ERR(root)) {
		/* -ENODEV is a kernel built without debugfs, not a failure */
		if (PTR_ERR(root) != -ENODEV)
			dev_warn(dev, "could not create %s debugfs directory\n",
				 RTLDSA_L2_DBG_ROOT_DIR);
		return;
	}

	if (devm_add_action_or_reset(dev, otto_l2_dbgfs_remove, root))
		return;

	debugfs_create_file("fdb", 0400, root, priv, &otto_l2_fdb_fops);
}
