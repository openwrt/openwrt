// SPDX-License-Identifier: GPL-2.0-only

#include <linux/if_bridge.h>
#include <linux/kref.h>
#include <linux/mutex.h>
#include <net/dsa.h>
#include <net/switchdev.h>

#include "rtl-otto.h"
#include "stp.h"

#define RTL838X_VLAN_STP_CTRL			(0x3b20)
#define RTL839X_ST_CTRL				(0x27e4)
#define RTL930X_ST_CTRL				(0x8798)
#define RTL931X_ST_CTRL				(0x8000)

void rtldsa_838x_stp_init(void)
{
	sw_w32(0, RTL838X_VLAN_STP_CTRL);
}

void rtldsa_839x_stp_init(void)
{
	sw_w32(0, RTL839X_ST_CTRL);
}

void rtldsa_930x_stp_init(void)
{
	sw_w32(0, RTL930X_ST_CTRL);
}

void rtldsa_931x_stp_init(void)
{
	sw_w32(0, RTL931X_ST_CTRL);
}

int rtldsa_838x_stp_get(struct rtl838x_switch_priv *priv, u16 msti, int port)
{
	int idx = 1 - (port / 16);
	int bit = 2 * (port % 16);
	/* port < priv->r->cpu_port (RTL838X_CPU_PORT == 28), so idx is 0 or 1 */
	u32 buf[2];
	int state;

	otto_table_read(RTL8380_TBL_MSTI, msti, &buf);
	state = (buf[idx] >> bit) & 0x3;

	return state;
}

void rtl838x_stp_set(struct rtl838x_switch_priv *priv, u16 msti, int port, int state)
{
	int tbl = otto_table_acquire(RTL8380_TBL_MSTI);
	int idx = 1 - (port / 16);
	int bit = 2 * (port % 16);
	/* port < priv->r->cpu_port (RTL838X_CPU_PORT == 28), so idx is 0 or 1 */
	u32 buf[2];

	__otto_table_read(tbl, msti, &buf);
	buf[idx] = (buf[idx] & ~(0x3 << bit)) | (state << bit);
	__otto_table_write(tbl, msti, &buf);
	otto_table_release(tbl);
}

int rtldsa_839x_stp_get(struct rtl838x_switch_priv *priv, u16 msti, int port)
{
	int idx = 3 - ((port + 12) / 16);
	int bit = 2 * ((port + 12) % 16);
	/* port < priv->r->cpu_port (RTL839X_CPU_PORT == 52), so idx is 0..3 */
	u32 buf[4];
	int state;

	otto_table_read(RTL8390_TBL_MSTI, msti, &buf);
	state = (buf[idx] >> bit) & 0x3;

	return state;
}

void rtl839x_stp_set(struct rtl838x_switch_priv *priv, u16 msti, int port, int state)
{
	int tbl = otto_table_acquire(RTL8390_TBL_MSTI);
	int idx = 3 - ((port + 12) / 16);
	int bit = 2 * ((port + 12) % 16);
	/* port < priv->r->cpu_port (RTL839X_CPU_PORT == 52), so idx is 0..3 */
	u32 buf[4];

	__otto_table_read(tbl, msti, &buf);
	buf[idx] = (buf[idx] & ~(0x3 << bit)) | (state << bit);
	__otto_table_write(tbl, msti, &buf);
	otto_table_release(tbl);
}

int rtldsa_930x_stp_get(struct rtl838x_switch_priv *priv, u16 msti, int port)
{
	int idx = 1 - ((port + 3) / 16);
	int bit = 2 * ((port + 3) % 16);
	/* port ranges 0..RTL930X_CPU_PORT (28), so idx is always 0 or 1 */
	u32 buf[2];
	int state;

	otto_table_read(RTL9300_TBL_MSTI, msti, &buf);
	state = (buf[idx] >> bit) & 0x3;

	return state;
}

void rtl930x_stp_set(struct rtl838x_switch_priv *priv, u16 msti, int port, int state)
{
	int tbl = otto_table_acquire(RTL9300_TBL_MSTI);
	int idx = 1 - ((port + 3) / 16);
	int bit = 2 * ((port + 3) % 16);
	/* port ranges 0..RTL930X_CPU_PORT (28), so idx is always 0 or 1 */
	u32 buf[2];

	__otto_table_read(tbl, msti, &buf);
	buf[idx] = (buf[idx] & ~(0x3 << bit)) | (state << bit);
	__otto_table_write(tbl, msti, &buf);
	otto_table_release(tbl);
}

int rtldsa_931x_stp_get(struct rtl838x_switch_priv *priv, u16 msti, int port)
{
	int idx = 3 - ((port + 8) / 16);
	int bit = 2 * ((port + 8) % 16);
	/* port ranges 0..55 (RTL931x covers ports 0 to 55 only), so idx is 0..3 */
	u32 buf[4];
	int state;

	otto_table_read(RTL9310_TBL_MSTI, msti, &buf);
	state = (buf[idx] >> bit) & 0x3;

	return state;
}

void rtl931x_stp_set(struct rtl838x_switch_priv *priv, u16 msti, int port, int state)
{
	int tbl = otto_table_acquire(RTL9310_TBL_MSTI);
	int idx = 3 - ((port + 8) / 16);
	int bit = 2 * ((port + 8) % 16);
	/* port ranges 0..55 (RTL931x covers ports 0 to 55 only), so idx is 0..3 */
	u32 buf[4];

	__otto_table_read(tbl, msti, &buf);
	buf[idx] = (buf[idx] & ~(0x3 << bit)) | (state << bit);
	__otto_table_write(tbl, msti, &buf);
	otto_table_release(tbl);
}

static void rtldsa_port_xstp_state_set(struct rtl838x_switch_priv *priv, int port,
				       u8 state, u16 mst_slot)
				       __must_hold(&priv->reg_mutex)
{
	int hw_state;

	if (port >= priv->r->cpu_port)
		return;

	switch (state) {
	case BR_STATE_DISABLED:
		hw_state = 0;
		break;
	case BR_STATE_BLOCKING:
	case BR_STATE_LISTENING:
		hw_state = 1;
		break;
	case BR_STATE_LEARNING:
		hw_state = 2;
		break;
	case BR_STATE_FORWARDING:
		hw_state = 3;
		break;
	default:
		dev_err(priv->dev, "stp state %d not supported\n", state);
		return;
	}

	priv->r->stp_set(priv, mst_slot, port, hw_state);
}

/**
 * rtldsa_mst_init() - Initialize newly "allocated" MST HW slot
 * @priv: private data of rtldsa switch
 * @mst_slot: MST slot of MSTI
 */
static void rtldsa_mst_init(struct rtl838x_switch_priv *priv, u16 mst_slot)
			    __must_hold(&priv->reg_mutex)
{
	struct dsa_port *dp;
	unsigned int port;
	u8 state;

	dsa_switch_for_each_user_port(dp, priv->ds) {
		if (dp->bridge)
			state = BR_STATE_DISABLED;
		else
			state = dp->stp_state;

		port = dp->index;

		rtldsa_port_xstp_state_set(priv, port, state, mst_slot);
	}
}

/**
 * rtldsa_mst_find() - Find HW MST slot for MSTI (without reference counting)
 * @priv: private data of rtldsa switch
 * @msti: MSTI to search
 *
 * Return: found HW slot (unmodified reference count) or negative encoded error value
 */
static int rtldsa_mst_find(struct rtl838x_switch_priv *priv, u16 msti)
			   __must_hold(&priv->reg_mutex)
{
	unsigned int i;

	/* CIST is always mapped to 0 */
	if (msti == 0)
		return 0;

	if (msti > 4095)
		return -EINVAL;

	/* search for existing entry */
	for (i = 0; i < priv->r->n_mst - 1; i++) {
		if (priv->msts[i].msti != msti)
			continue;

		return i + 1;
	}

	return -ENOENT;
}

/**
 * rtldsa_mst_get() - Get (or allocate) HW MST slot for MSTI
 * @priv: private data of rtldsa switch
 * @msti: MSTI for which a HW slot is needed
 *
 * Return: allocated slot (with increased reference count) or negative encoded error value
 */
static int rtldsa_mst_get(struct rtl838x_switch_priv *priv, u16 msti)
			  __must_hold(&priv->reg_mutex)
{
	unsigned int i;
	int ret;

	ret = rtldsa_mst_find(priv, msti);

	/* CIST doesn't need reference counting */
	if (ret == 0)
		return ret;

	/* valid HW slot was found - refcount needs to be adjusted */
	if (ret > 0) {
		u16 index = ret - 1;

		kref_get(&priv->msts[index].refcount);
		return ret;
	}

	/* any error except "no entry found" cannot be handled */
	if (ret != -ENOENT)
		return ret;

	/* search for free slot */
	for (i = 0; i < priv->r->n_mst - 1; i++) {
		if (priv->msts[i].msti != 0)
			continue;

		kref_init(&priv->msts[i].refcount);
		priv->msts[i].msti = msti;

		rtldsa_mst_init(priv, i + 1);
		return i + 1;
	}

	return -ENOSPC;
}

/**
 * rtldsa_mst_recycle_slot() - Try to recycle old MST slot in case of -ENOSPC of rtldsa_mst_get()
 * @priv: private data of rtldsa switch
 * @msti: MSTI for which a HW slot is needed
 * @old_mst_slot: old mst slot which will be released "soon"
 *
 * If a VLAN should be moved from one MSTI to another one, it is possible that there are currently
 * not enough slots still available to perform a get+put operation. But if this slot is used
 * by a single VLAN anyway, it is not needed to really allocate a new slow - reassigning it to
 * the new MSTI is good enough.
 *
 * This is only allowed when holding the reg_mutex over both calls rtldsa_mst_get() and
 * rtldsa_mst_recycle(). After a rtldsa_mst_recycle() call, rtldsa_mst_put_slot() must no longer
 * be called for @old_mst_slot.
 *
 * Return: allocated slot (with increased reference count) or negative encoded error value
 */
static int rtldsa_mst_recycle_slot(struct rtl838x_switch_priv *priv, u16 msti, u16 old_mst_slot)
				   __must_hold(&priv->reg_mutex)
{
	u16 index;

	/* CIST is always mapped to 0 */
	if (msti == 0)
		return 0;

	if (old_mst_slot == 0)
		return -ENOSPC;

	if (msti > 4095)
		return -EINVAL;

	if (old_mst_slot >= priv->r->n_mst)
		return -EINVAL;

	index = old_mst_slot - 1;

	/* this slot is unused - should not happen because rtldsa_mst_get() searches for it */
	if (priv->msts[index].msti == 0)
		return -EINVAL;

	/* it is only allowed to swap when no other VLAN is using this MST slot */
	if (kref_read(&priv->msts[index].refcount) != 1)
		return -ENOSPC;

	priv->msts[index].msti = msti;
	return old_mst_slot;
}

static void rtldsa_mst_release_slot(struct kref *ref)
{
	struct rtldsa_mst *slot = container_of(ref, struct rtldsa_mst, refcount);

	slot->msti = 0;
}

/**
 * rtldsa_mst_put_slot() - Decrement VLAN use counter for MST slot
 * @priv: private data of rtldsa switch
 * @mst_slot: MST slot which should be put
 *
 * Return: false when MST slot reference counter was only decreased or an invalid @mst_slot was
 * given, true when @mst_slot is now unused
 */
bool rtldsa_mst_put_slot(struct rtl838x_switch_priv *priv, u16 mst_slot)
				__must_hold(&priv->reg_mutex)
{
	unsigned int index;

	/* CIST is always mapped to 0 and cannot be put */
	if (mst_slot == 0)
		return 0;

	if (mst_slot >= priv->r->n_mst)
		return 0;

	index = mst_slot - 1;

	/* this slot is unused and must not release a reference */
	if (priv->msts[index].msti == 0)
		return 0;

	return kref_put(&priv->msts[index].refcount, rtldsa_mst_release_slot);
}

/**
 * rtldsa_mst_replace() - Get HW slot for @msti and drop old HW slot
 * @priv: private data of rtldsa switch
 * @msti: MSTI for which a HW slot is needed
 * @old_mst_slot: old mst slot which will no longer be assigned to VLAN
 *
 * Return: allocated slot (with increased reference count) or negative encoded error value
 */
int rtldsa_mst_replace(struct rtl838x_switch_priv *priv, u16 msti, u16 old_mst_slot)
			      __must_hold(&priv->reg_mutex)
{
	int mst_slot_new;

	mst_slot_new = rtldsa_mst_get(priv, msti);
	if (mst_slot_new == -ENOSPC)
		return rtldsa_mst_recycle_slot(priv, msti, old_mst_slot);

	/* directly return errors and don't free old slot */
	if (mst_slot_new < 0)
		return mst_slot_new;

	rtldsa_mst_put_slot(priv, old_mst_slot);

	return mst_slot_new;
}

/* Set non-CIST port states while the caller holds reg_mutex. */
void rtldsa_port_non_cist_states_set(struct rtl838x_switch_priv *priv, int port, u8 state)
			      __must_hold(&priv->reg_mutex)
{
	unsigned int i;

	for (i = 1; i < priv->r->n_mst; i++)
		rtldsa_port_xstp_state_set(priv, port, state, i);
}

int rtldsa_port_get_stp_state(struct rtl838x_switch_priv *priv, int port)
{
	u32 msti = 0;
	int state;

	if (port >= priv->r->cpu_port)
		return -EINVAL;

	mutex_lock(&priv->reg_mutex);
	state = priv->r->stp_get(priv, msti, port);
	mutex_unlock(&priv->reg_mutex);

	return state;
}

void rtldsa_port_stp_state_set(struct dsa_switch *ds, int port, u8 state)
{
	struct rtl838x_switch_priv *priv = ds->priv;
	struct dsa_port *dp = dsa_to_port(ds, port);

	mutex_lock(&priv->reg_mutex);
	rtldsa_port_xstp_state_set(priv, port, state, 0);

	/* for unbridged ports, also force the same state to the MSTIs */
	if (!dp->bridge)
		rtldsa_port_non_cist_states_set(priv, port, state);

	mutex_unlock(&priv->reg_mutex);
}

int rtldsa_port_mst_state_set(struct dsa_switch *ds, int port,
			      const struct switchdev_mst_state *st)
{
	struct rtl838x_switch_priv *priv = ds->priv;
	int mst_slot;

	mutex_lock(&priv->reg_mutex);

	mst_slot = rtldsa_mst_find(priv, st->msti);
	if (mst_slot < 0) {
		mutex_unlock(&priv->reg_mutex);
		return mst_slot;
	}

	rtldsa_port_xstp_state_set(priv, port, st->state, mst_slot);
	mutex_unlock(&priv->reg_mutex);

	return 0;
}
