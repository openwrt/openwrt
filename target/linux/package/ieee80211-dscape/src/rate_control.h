/*
 * Copyright 2002-2005, Instant802 Networks, Inc.
 * Copyright 2005, Devicescape Software, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef RATE_CONTROL
#define RATE_CONTROL

#define RATE_CONTROL_NUM_DOWN 20
#define RATE_CONTROL_NUM_UP   15


struct rate_control_extra {
	/* values from rate_control_get_rate() to the caller: */
	struct ieee80211_rate *probe; /* probe with this rate, or NULL for no
				       * probing */
	int startidx, endidx, rateidx;
	struct ieee80211_rate *nonerp;
	int nonerp_idx;

	/* parameters from the caller to rate_control_get_rate(): */
	int mgmt_data; /* this is data frame that is used for management
			* (e.g., IEEE 802.1X EAPOL) */
	u16 ethertype;
};


struct rate_control_ops {
	const char *name;
	void (*tx_status)(struct net_device *dev, struct sk_buff *skb,
			  struct ieee80211_tx_status *status);
	struct ieee80211_rate *
	(*get_rate)(struct net_device *dev, struct sk_buff *skb,
		    struct rate_control_extra *extra);
	void (*rate_init)(struct ieee80211_local *local, struct sta_info *sta);
	void (*clear)(void *priv);
	int (*status_sta)(struct ieee80211_local *local,
			  struct sta_info *sta, char *buf);
	int (*status_global)(struct ieee80211_local *local, char *buf);

	void * (*alloc)(struct ieee80211_local *local);
	void (*free)(void *priv);
	void * (*alloc_sta)(void);
	void (*free_sta)(void *priv);
};


int ieee80211_rate_control_register(struct rate_control_ops *ops);
void ieee80211_rate_control_unregister(struct rate_control_ops *ops);


static inline void rate_control_tx_status(struct net_device *dev,
					  struct sk_buff *skb,
					  struct ieee80211_tx_status *status)
{
	struct ieee80211_local *local = dev->priv;
	local->rate_ctrl->tx_status(dev, skb, status);
}


static inline struct ieee80211_rate *
rate_control_get_rate(struct net_device *dev, struct sk_buff *skb,
		      struct rate_control_extra *extra)
{
	struct ieee80211_local *local = dev->priv;
	return local->rate_ctrl->get_rate(dev, skb, extra);
}


static inline void rate_control_rate_init(struct ieee80211_local *local,
					  struct sta_info *sta)
{
	local->rate_ctrl->rate_init(local, sta);
}


static inline void rate_control_clear(struct ieee80211_local *local)
{
	local->rate_ctrl->clear(local->rate_ctrl_priv);
}


static inline int rate_control_status_sta(struct ieee80211_local *local,
					  struct sta_info *sta, char *buf)
{
	return local->rate_ctrl->status_sta(local, sta, buf);
}


static inline int rate_control_status_global(struct ieee80211_local *local,
					     char *buf)
{
	return local->rate_ctrl->status_global(local, buf);
}


static inline void * rate_control_alloc(struct ieee80211_local *local)
{
	return local->rate_ctrl->alloc(local);
}


static inline void rate_control_free(struct ieee80211_local *local)
{
	if (local->rate_ctrl == NULL || local->rate_ctrl_priv == NULL)
		return;
	local->rate_ctrl->free(local->rate_ctrl_priv);
	local->rate_ctrl_priv = NULL;
}


static inline void * rate_control_alloc_sta(struct ieee80211_local *local)
{
	return local->rate_ctrl->alloc_sta();
}


static inline void rate_control_free_sta(struct ieee80211_local *local,
					 void *priv)
{
	local->rate_ctrl->free_sta(priv);
}

#endif /* RATE_CONTROL */
