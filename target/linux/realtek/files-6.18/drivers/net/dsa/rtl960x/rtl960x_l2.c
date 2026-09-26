// SPDX-License-Identifier: GPL-2.0-only
/*
 * Forwarding database interface for the RTL960x switch core.
 *
 * The L2 table is a hash table of 2048 entries plus a 64-entry CAM, keyed by
 * {MAC, CVID} for IVL entries and {MAC, FID} for SVL entries. The hardware
 * hashes the key itself, so adding and deleting an entry is a single
 * MAC-method write (a delete writes the same key back invalid), and a dump
 * walks a port's valid unicast entries with the ADDR_NEXT_UC_PORT method.
 *
 * Filtering databases follow the mv88e6xxx model: standalone ports learn in
 * FID 0, every VLAN-unaware bridge shares FID 1, and DSA FDB operations with
 * VID 0 map to SVL/FID 1 while non-zero VIDs map to IVL/CVID.
 */

#include <linux/bitfield.h>
#include <linux/etherdevice.h>
#include <linux/regmap.h>

#include "rtl960x_l2.h"
#include "rtl960x_table.h"

#define RTL960X_L2_ENTRY_SIZE			3	/* 32-bit words */
#define RTL960X_L2_NUM_ENTRIES			2112	/* 2048 hash + 64 CAM */
#define RTL960X_L2_AGE_MAX			7

/*
 * L2 unicast entry, 80 bits over three data words. Data word 0 and the low
 * half of word 1 hold the MAC address with mac_addr[0] in the most
 * significant byte. The FID straddles words 1 and 2.
 */
#define RTL960X_L2_UC_D0_MAC5_MSK		GENMASK(7, 0)
#define RTL960X_L2_UC_D0_MAC4_MSK		GENMASK(15, 8)
#define RTL960X_L2_UC_D0_MAC3_MSK		GENMASK(23, 16)
#define RTL960X_L2_UC_D0_MAC2_MSK		GENMASK(31, 24)
#define RTL960X_L2_UC_D1_MAC1_MSK		GENMASK(7, 0)
#define RTL960X_L2_UC_D1_MAC0_MSK		GENMASK(15, 8)
#define RTL960X_L2_UC_D1_CVID_MSK		GENMASK(27, 16)
#define RTL960X_L2_UC_D1_L3LOOKUP_MSK		BIT(28)
#define RTL960X_L2_UC_D1_NOT_SALEARN_MSK	BIT(29)
#define RTL960X_L2_UC_D1_IVL_MSK		BIT(30)
#define RTL960X_L2_UC_D1_FID_LO_MSK		BIT(31)
#define   RTL960X_L2_UC_FID_LO_MSK		BIT(0)
#define RTL960X_L2_UC_D2_FID_HI_MSK		BIT(0)
#define   RTL960X_L2_UC_FID_HI_MSK		BIT(1)
#define RTL960X_L2_UC_D2_CTAG_IF_MSK		BIT(1)
#define RTL960X_L2_UC_D2_SPA_MSK		GENMASK(5, 2)
#define RTL960X_L2_UC_D2_AGE_MSK		GENMASK(8, 6)
#define RTL960X_L2_UC_D2_SA_BLOCK_MSK		BIT(9)
#define RTL960X_L2_UC_D2_DA_BLOCK_MSK		BIT(10)
#define RTL960X_L2_UC_D2_ARP_USAGE_MSK		BIT(11)
#define RTL960X_L2_UC_D2_EXT_SPA_MSK		GENMASK(14, 12)
#define RTL960X_L2_UC_D2_VALID_MSK		BIT(15)

/*
 * L2 flush command registers. Writing the control register with a port mode
 * flush starts it for the ports set in the enable register; completion is
 * signalled when BUSY clears.
 */
#define RTL960X_L2_FLUSH_CTRL_REG		0x17044
#define   RTL960X_L2_FLUSH_CTRL_BUSY_MSK	BIT(0)
#define   RTL960X_L2_FLUSH_CTRL_MODE_MSK	GENMASK(2, 1)
#define   RTL960X_L2_FLUSH_CTRL_MODE_PORT	0
#define   RTL960X_L2_FLUSH_CTRL_STATIC_MSK	BIT(3)
#define   RTL960X_L2_FLUSH_CTRL_DYNAMIC_MSK	BIT(4)
#define RTL960X_L2_FLUSH_EN_REG			0x17048
#define   RTL960X_L2_FLUSH_EN_PORT_MSK		GENMASK(10, 0)

struct rtl960x_l2_uc_key {
	u8 mac_addr[ETH_ALEN];
	u16 vid;	/* CVID, the key with IVL */
	u8 fid;		/* filtering database, the key with SVL */
	bool ivl;
};

struct rtl960x_l2_uc {
	struct rtl960x_l2_uc_key key;
	u8 port;
	u8 ext_port;
	u8 age;

	bool valid;
	bool is_static;
	bool sa_block;
	bool da_block;
	bool arp_usage;
	bool l3lookup;
	bool ctag_if;
};

static void rtl960x_l2_data_to_uc(const u32 *data, struct rtl960x_l2_uc *uc)
{
	u32 val;

	uc->key.mac_addr[5] = FIELD_GET(RTL960X_L2_UC_D0_MAC5_MSK, data[0]);
	uc->key.mac_addr[4] = FIELD_GET(RTL960X_L2_UC_D0_MAC4_MSK, data[0]);
	uc->key.mac_addr[3] = FIELD_GET(RTL960X_L2_UC_D0_MAC3_MSK, data[0]);
	uc->key.mac_addr[2] = FIELD_GET(RTL960X_L2_UC_D0_MAC2_MSK, data[0]);
	uc->key.mac_addr[1] = FIELD_GET(RTL960X_L2_UC_D1_MAC1_MSK, data[1]);
	uc->key.mac_addr[0] = FIELD_GET(RTL960X_L2_UC_D1_MAC0_MSK, data[1]);
	uc->key.vid = FIELD_GET(RTL960X_L2_UC_D1_CVID_MSK, data[1]);
	uc->key.ivl = FIELD_GET(RTL960X_L2_UC_D1_IVL_MSK, data[1]);

	val = FIELD_GET(RTL960X_L2_UC_D1_FID_LO_MSK, data[1]);
	uc->key.fid = FIELD_PREP(RTL960X_L2_UC_FID_LO_MSK, val);
	val = FIELD_GET(RTL960X_L2_UC_D2_FID_HI_MSK, data[2]);
	uc->key.fid |= FIELD_PREP(RTL960X_L2_UC_FID_HI_MSK, val);

	uc->port = FIELD_GET(RTL960X_L2_UC_D2_SPA_MSK, data[2]);
	uc->ext_port = FIELD_GET(RTL960X_L2_UC_D2_EXT_SPA_MSK, data[2]);
	uc->age = FIELD_GET(RTL960X_L2_UC_D2_AGE_MSK, data[2]);
	uc->valid = FIELD_GET(RTL960X_L2_UC_D2_VALID_MSK, data[2]);
	uc->is_static = FIELD_GET(RTL960X_L2_UC_D1_NOT_SALEARN_MSK, data[1]);
	uc->sa_block = FIELD_GET(RTL960X_L2_UC_D2_SA_BLOCK_MSK, data[2]);
	uc->da_block = FIELD_GET(RTL960X_L2_UC_D2_DA_BLOCK_MSK, data[2]);
	uc->arp_usage = FIELD_GET(RTL960X_L2_UC_D2_ARP_USAGE_MSK, data[2]);
	uc->l3lookup = FIELD_GET(RTL960X_L2_UC_D1_L3LOOKUP_MSK, data[1]);
	uc->ctag_if = FIELD_GET(RTL960X_L2_UC_D2_CTAG_IF_MSK, data[2]);
}

static void rtl960x_l2_uc_to_data(const struct rtl960x_l2_uc *uc, u32 *data)
{
	u32 val;

	memset(data, 0, RTL960X_L2_ENTRY_SIZE * sizeof(*data));
	data[0] |= FIELD_PREP(RTL960X_L2_UC_D0_MAC5_MSK, uc->key.mac_addr[5]);
	data[0] |= FIELD_PREP(RTL960X_L2_UC_D0_MAC4_MSK, uc->key.mac_addr[4]);
	data[0] |= FIELD_PREP(RTL960X_L2_UC_D0_MAC3_MSK, uc->key.mac_addr[3]);
	data[0] |= FIELD_PREP(RTL960X_L2_UC_D0_MAC2_MSK, uc->key.mac_addr[2]);
	data[1] |= FIELD_PREP(RTL960X_L2_UC_D1_MAC1_MSK, uc->key.mac_addr[1]);
	data[1] |= FIELD_PREP(RTL960X_L2_UC_D1_MAC0_MSK, uc->key.mac_addr[0]);
	data[1] |= FIELD_PREP(RTL960X_L2_UC_D1_CVID_MSK, uc->key.vid);
	data[1] |= FIELD_PREP(RTL960X_L2_UC_D1_IVL_MSK, uc->key.ivl);

	val = FIELD_GET(RTL960X_L2_UC_FID_LO_MSK, uc->key.fid);
	data[1] |= FIELD_PREP(RTL960X_L2_UC_D1_FID_LO_MSK, val);
	val = FIELD_GET(RTL960X_L2_UC_FID_HI_MSK, uc->key.fid);
	data[2] |= FIELD_PREP(RTL960X_L2_UC_D2_FID_HI_MSK, val);

	data[1] |= FIELD_PREP(RTL960X_L2_UC_D1_L3LOOKUP_MSK, uc->l3lookup);
	data[1] |= FIELD_PREP(RTL960X_L2_UC_D1_NOT_SALEARN_MSK, uc->is_static);
	data[2] |= FIELD_PREP(RTL960X_L2_UC_D2_CTAG_IF_MSK, uc->ctag_if);
	data[2] |= FIELD_PREP(RTL960X_L2_UC_D2_SPA_MSK, uc->port);
	data[2] |= FIELD_PREP(RTL960X_L2_UC_D2_AGE_MSK, uc->age);
	data[2] |= FIELD_PREP(RTL960X_L2_UC_D2_SA_BLOCK_MSK, uc->sa_block);
	data[2] |= FIELD_PREP(RTL960X_L2_UC_D2_DA_BLOCK_MSK, uc->da_block);
	data[2] |= FIELD_PREP(RTL960X_L2_UC_D2_ARP_USAGE_MSK, uc->arp_usage);
	data[2] |= FIELD_PREP(RTL960X_L2_UC_D2_EXT_SPA_MSK, uc->ext_port);
	data[2] |= FIELD_PREP(RTL960X_L2_UC_D2_VALID_MSK, uc->valid);
}

/* DSA VID 0 is the VLAN-unaware database, shared by all bridges as FID 1. */
static void rtl960x_l2_uc_key_set(struct rtl960x_l2_uc_key *key,
				  const unsigned char mac_addr[static ETH_ALEN],
				  u16 vid)
{
	ether_addr_copy(key->mac_addr, mac_addr);
	key->ivl = !!vid;
	key->vid = vid;
	key->fid = vid ? 0 : 1;
}

/**
 * rtl960x_l2_get_next_uc() - get the next unicast L2 entry of a port
 * @priv: driver context
 * @addr: as input, the table address to start the walk from;
 *        as output, the address of the found entry
 * @port: restrict the walk to entries learned on or installed for this port
 * @entry: the found entry
 *
 * Reads the next valid unicast entry of @port at or after @addr. Once the
 * walk reaches the end of the table the hardware wraps around to the first
 * entry, so the output @addr is then lower than the input; callers must
 * detect that to terminate.
 *
 * Context: Can sleep.
 * Return: 0 on success, %-ENOENT if the table holds no unicast entry for
 * @port, or a negative error code on failure.
 */
int rtl960x_l2_get_next_uc(struct rtl960x_dsa *priv, u16 *addr, int port,
			   struct rtl960x_fdb_entry *entry)
{
	u32 data[RTL960X_L2_ENTRY_SIZE] = { 0 };
	struct rtl960x_l2_uc uc;
	int ret;

	ret = rtl960x_table_query(priv, RTL960X_TABLE_L2,
				  RTL960X_TABLE_OP_READ, addr,
				  RTL960X_TABLE_L2_METHOD_ADDR_NEXT_UC_PORT,
				  port, data, RTL960X_L2_ENTRY_SIZE);
	if (ret)
		return ret;

	rtl960x_l2_data_to_uc(data, &uc);

	ether_addr_copy(entry->mac_addr, uc.key.mac_addr);
	/*
	 * A hardware-learned SVL entry still records the CVID the frame was
	 * classified to, but its key is {MAC, FID}: report it in DSA's
	 * VLAN-unaware database, VID 0.
	 */
	entry->vid = uc.key.ivl ? uc.key.vid : 0;
	entry->is_static = uc.is_static;

	return 0;
}

/**
 * rtl960x_l2_add_uc() - add a static unicast entry
 * @priv: driver context
 * @port: destination port
 * @mac_addr: MAC address
 * @vid: DSA VID; 0 selects the VLAN-unaware database
 *
 * Context: Can sleep.
 * Return: 0 on success, %-ENOSPC if the hash bucket and the CAM are full,
 * or a negative error code on failure.
 */
int rtl960x_l2_add_uc(struct rtl960x_dsa *priv, int port,
		      const unsigned char mac_addr[static ETH_ALEN], u16 vid)
{
	u32 data[RTL960X_L2_ENTRY_SIZE];
	struct rtl960x_l2_uc uc = { 0 };
	u16 addr;
	int ret;

	rtl960x_l2_uc_key_set(&uc.key, mac_addr, vid);
	uc.port = port;
	/*
	 * Entries programmed by DSA (including those learned by the software
	 * bridge and installed through assisted learning) must be static: the
	 * NOT_SALEARN bit stops the hardware from moving them, and the age is
	 * kept at its maximum so the ageing timer never drops them.
	 */
	uc.is_static = true;
	uc.age = RTL960X_L2_AGE_MAX;
	uc.valid = true;
	rtl960x_l2_uc_to_data(&uc, data);

	/* Add the new entry or update an existing one */
	ret = rtl960x_table_query(priv, RTL960X_TABLE_L2,
				  RTL960X_TABLE_OP_WRITE, &addr, 0, 0,
				  data, RTL960X_L2_ENTRY_SIZE);

	/* No slot: the hash bucket and the CAM overflowed */
	if (ret == -ENOENT)
		return -ENOSPC;

	return ret;
}

/**
 * rtl960x_l2_del_uc() - delete a unicast entry
 * @priv: driver context
 * @mac_addr: MAC address
 * @vid: DSA VID; 0 selects the VLAN-unaware database
 *
 * A missing entry is not an error.
 *
 * Context: Can sleep.
 * Return: 0 on success, or a negative error code on failure.
 */
int rtl960x_l2_del_uc(struct rtl960x_dsa *priv,
		      const unsigned char mac_addr[static ETH_ALEN], u16 vid)
{
	u32 data[RTL960X_L2_ENTRY_SIZE];
	struct rtl960x_l2_uc uc = { 0 };
	u16 addr;
	int ret;

	rtl960x_l2_uc_key_set(&uc.key, mac_addr, vid);
	/*
	 * Unlike rtl8365mb, where age 0 deletes the entry, this table has a
	 * valid bit: writing the key back with it clear frees the slot.
	 */
	uc.valid = false;
	uc.age = 0;
	rtl960x_l2_uc_to_data(&uc, data);

	ret = rtl960x_table_query(priv, RTL960X_TABLE_L2,
				  RTL960X_TABLE_OP_WRITE, &addr, 0, 0,
				  data, RTL960X_L2_ENTRY_SIZE);
	if (ret == -ENOENT) {
		dev_dbg(priv->dev, "%s: %pM vid=%d missing\n", __func__,
			mac_addr, vid);
		return 0;
	}

	return ret;
}

/**
 * rtl960x_l2_flush() - flush the dynamic entries of a port
 * @priv: driver context
 * @port: port whose learned entries are dropped
 *
 * Context: Can sleep. Takes and releases &priv->table_lock.
 * Return: 0 on success, or a negative error code on failure.
 */
int rtl960x_l2_flush(struct rtl960x_dsa *priv, int port)
{
	u32 val;
	int ret;

	mutex_lock(&priv->table_lock);

	ret = regmap_write(priv->map, RTL960X_L2_FLUSH_CTRL_REG,
			   FIELD_PREP(RTL960X_L2_FLUSH_CTRL_MODE_MSK,
				      RTL960X_L2_FLUSH_CTRL_MODE_PORT) |
			   FIELD_PREP(RTL960X_L2_FLUSH_CTRL_DYNAMIC_MSK, 1));
	if (ret)
		goto out;

	ret = regmap_write(priv->map, RTL960X_L2_FLUSH_EN_REG,
			   FIELD_PREP(RTL960X_L2_FLUSH_EN_PORT_MSK, BIT(port)));
	if (ret)
		goto out;

	ret = regmap_read_poll_timeout(priv->map, RTL960X_L2_FLUSH_CTRL_REG,
				       val,
				       !FIELD_GET(RTL960X_L2_FLUSH_CTRL_BUSY_MSK, val),
				       10, 100000);

out:
	mutex_unlock(&priv->table_lock);

	return ret;
}

/**
 * rtl960x_port_fdb_add() - add a static FDB entry to a port database
 * @ds: DSA switch instance
 * @port: port index
 * @addr: MAC address
 * @vid: VLAN ID, 0 for the VLAN-unaware database
 * @db: database the entry belongs to
 *
 * Context: Can sleep. Takes and releases &priv->l2_lock.
 * Return: 0 on success, or a negative error code on failure.
 */
int rtl960x_port_fdb_add(struct dsa_switch *ds, int port,
			 const unsigned char *addr, u16 vid,
			 const struct dsa_db db)
{
	struct rtl960x_dsa *priv = ds->priv;
	int ret;

	if (is_multicast_ether_addr(addr))
		return -EOPNOTSUPP;

	if (db.type != DSA_DB_PORT && db.type != DSA_DB_BRIDGE)
		return -EOPNOTSUPP;

	dev_dbg(priv->dev, "%s: port:%d addr:%pM vid:%d dbtype:%d\n",
		__func__, port, addr, vid, db.type);

	mutex_lock(&priv->l2_lock);
	ret = rtl960x_l2_add_uc(priv, port, addr, vid);
	mutex_unlock(&priv->l2_lock);

	if (ret)
		dev_err(priv->dev, "fdb_add ERROR %pe\n", ERR_PTR(ret));

	return ret;
}

/**
 * rtl960x_port_fdb_del() - delete a static FDB entry from a port database
 * @ds: DSA switch instance
 * @port: port index
 * @addr: MAC address
 * @vid: VLAN ID, 0 for the VLAN-unaware database
 * @db: database the entry belongs to
 *
 * Context: Can sleep. Takes and releases &priv->l2_lock.
 * Return: 0 on success, or a negative error code on failure.
 */
int rtl960x_port_fdb_del(struct dsa_switch *ds, int port,
			 const unsigned char *addr, u16 vid,
			 const struct dsa_db db)
{
	struct rtl960x_dsa *priv = ds->priv;
	int ret;

	if (is_multicast_ether_addr(addr))
		return -EOPNOTSUPP;

	if (db.type != DSA_DB_PORT && db.type != DSA_DB_BRIDGE)
		return -EOPNOTSUPP;

	dev_dbg(priv->dev, "%s: port:%d addr:%pM vid:%d dbtype:%d\n",
		__func__, port, addr, vid, db.type);

	mutex_lock(&priv->l2_lock);
	ret = rtl960x_l2_del_uc(priv, addr, vid);
	mutex_unlock(&priv->l2_lock);

	if (ret)
		dev_err(priv->dev, "fdb_del ERROR %pe\n", ERR_PTR(ret));

	return ret;
}

/**
 * rtl960x_port_fdb_dump() - iterate over the FDB entries of a port
 * @ds: DSA switch instance
 * @port: port index
 * @cb: callback invoked for each entry
 * @data: opaque pointer passed to @cb
 *
 * Walks the unicast entries associated with @port and calls @cb for each.
 *
 * Context: Can sleep. Takes and releases &priv->l2_lock for the whole walk.
 * Return: 0 on success, or a negative error code on failure.
 */
int rtl960x_port_fdb_dump(struct dsa_switch *ds, int port,
			  dsa_fdb_dump_cb_t *cb, void *data)
{
	struct rtl960x_fdb_entry entry = { 0 };
	struct rtl960x_dsa *priv = ds->priv;
	u16 start_addr, addr = 0;
	int ret = 0;

	mutex_lock(&priv->l2_lock);
	while (true) {
		start_addr = addr;

		ret = rtl960x_l2_get_next_uc(priv, &addr, port, &entry);
		dev_dbg(priv->dev, "%s addr:%d mac:%pM vid:%d static:%d ret:%pe\n",
			__func__, addr, entry.mac_addr, entry.vid,
			entry.is_static, ERR_PTR(ret));

		if (ret == -ENOENT) {
			/* No entry for this port at all. */
			ret = 0;
			break;
		}

		if (ret)
			break;

		/*
		 * The walk wraps back to the first entry once it passes the
		 * end of the table; stop as soon as the address goes backwards.
		 */
		if (addr < start_addr)
			break;

		ret = cb(entry.mac_addr, entry.vid, entry.is_static, data);
		if (ret)
			break;

		addr++;
		if (addr >= RTL960X_L2_NUM_ENTRIES)
			break;
	}
	mutex_unlock(&priv->l2_lock);

	return ret;
}
