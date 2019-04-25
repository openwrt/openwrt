#ifndef __KEYS_H_
#define __KEYS_H_INCLUDED

int xradio_alloc_key(struct xradio_common *hw_priv);
void xradio_free_key(struct xradio_common *hw_priv, int idx);
void xradio_free_keys(struct xradio_common *hw_priv);
int xradio_upload_keys(struct xradio_vif *priv);
int xradio_set_key(struct ieee80211_hw *dev, enum set_key_cmd cmd,
                   struct ieee80211_vif *vif, struct ieee80211_sta *sta,
                   struct ieee80211_key_conf *key);

#endif
