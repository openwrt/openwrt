#include <net/mac80211.h>

#include "xradio.h"
#include "sta.h"
#include "keys.h"

int xradio_alloc_key(struct xradio_common *hw_priv)
{
	int idx;

	idx = ffs(~hw_priv->key_map) - 1;
	if (idx < 0 || idx > WSM_KEY_MAX_INDEX)
		return -1;

	hw_priv->key_map |= BIT(idx);
	hw_priv->keys[idx].entryIndex = idx;
	return idx;
}

void xradio_free_key(struct xradio_common *hw_priv, int idx)
{
	BUG_ON(!(hw_priv->key_map & BIT(idx)));
	memset(&hw_priv->keys[idx], 0, sizeof(hw_priv->keys[idx]));
	hw_priv->key_map &= ~BIT(idx);
}

void xradio_free_keys(struct xradio_common *hw_priv)
{
	memset(&hw_priv->keys, 0, sizeof(hw_priv->keys));
	hw_priv->key_map = 0;
}

int xradio_upload_keys(struct xradio_vif *priv)
{
	struct xradio_common *hw_priv = xrwl_vifpriv_to_hwpriv(priv);
	int idx, ret = 0;


	for (idx = 0; idx <= WSM_KEY_MAX_IDX; ++idx)
		if (hw_priv->key_map & BIT(idx)) {
			ret = wsm_add_key(hw_priv, &hw_priv->keys[idx], priv->if_id);
			if (ret < 0)
				break;
		}
	return ret;
}

int xradio_set_key(struct ieee80211_hw *dev, enum set_key_cmd cmd,
                   struct ieee80211_vif *vif, struct ieee80211_sta *sta,
                   struct ieee80211_key_conf *key)
{
#ifdef XRADIO_DISABLE_HW_CRYPTO
	wiphy_info(dev->wiphy, "hw crypto is disabled, ignoring key request\n");
	return -EOPNOTSUPP;
#else
	int ret = -EOPNOTSUPP;
	struct xradio_common *hw_priv = dev->priv;
	struct xradio_vif *priv = xrwl_get_vif_from_ieee80211(vif);

	wiphy_dbg(dev->wiphy, "vif %d: set_key cmd %d\n", priv->if_id, (int) cmd);
	
	mutex_lock(&hw_priv->conf_mutex);

	if (cmd == SET_KEY) {
		u8 *peer_addr = NULL;
		int pairwise = (key->flags & IEEE80211_KEY_FLAG_PAIRWISE) ? 1 : 0;
		int idx = xradio_alloc_key(hw_priv);
		struct wsm_add_key *wsm_key = &hw_priv->keys[idx];

		if (idx < 0) {
			wiphy_err(dev->wiphy, "xradio_alloc_key failed!\n");
			ret = -EINVAL;
			goto finally;
		}

		BUG_ON(pairwise && !sta);
		if (sta)
			peer_addr = sta->addr;

		key->flags |= IEEE80211_KEY_FLAG_PUT_IV_SPACE;

		priv->cipherType = key->cipher;
		switch (key->cipher) {
		case WLAN_CIPHER_SUITE_WEP40:
		case WLAN_CIPHER_SUITE_WEP104:
			if (key->keylen > 16) {
				xradio_free_key(hw_priv, idx);
				wiphy_err(dev->wiphy, "keylen too long=%d!\n", key->keylen);
				ret = -EINVAL;
				goto finally;
			}

			if (pairwise) {
				wsm_key->type = WSM_KEY_TYPE_WEP_PAIRWISE;
				memcpy(wsm_key->wepPairwiseKey.peerAddress, peer_addr, ETH_ALEN);
				memcpy(wsm_key->wepPairwiseKey.keyData, &key->key[0], key->keylen);
				wsm_key->wepPairwiseKey.keyLength = key->keylen;
			} else {
				wsm_key->type = WSM_KEY_TYPE_WEP_DEFAULT;
				memcpy(wsm_key->wepGroupKey.keyData, &key->key[0], key->keylen);
				wsm_key->wepGroupKey.keyLength = key->keylen;
				wsm_key->wepGroupKey.keyId     = key->keyidx;
			}
			break;
		case WLAN_CIPHER_SUITE_TKIP:
			if (pairwise) {
				wsm_key->type = WSM_KEY_TYPE_TKIP_PAIRWISE;
				memcpy(wsm_key->tkipPairwiseKey.peerAddress, peer_addr, ETH_ALEN);
				memcpy(wsm_key->tkipPairwiseKey.tkipKeyData, &key->key[0], 16);
				memcpy(wsm_key->tkipPairwiseKey.txMicKey, &key->key[16], 8);
				memcpy(wsm_key->tkipPairwiseKey.rxMicKey, &key->key[24], 8);
			} else {
				size_t mic_offset = (priv->mode == NL80211_IFTYPE_AP) ? 16 : 24;
				wsm_key->type = WSM_KEY_TYPE_TKIP_GROUP;
				memcpy(wsm_key->tkipGroupKey.tkipKeyData,&key->key[0],  16);
				memcpy(wsm_key->tkipGroupKey.rxMicKey, &key->key[mic_offset], 8);

				/* TODO: Where can I find TKIP SEQ? */
				memset(wsm_key->tkipGroupKey.rxSeqCounter, 0, 8);
				wsm_key->tkipGroupKey.keyId = key->keyidx;
			}
			break;
		case WLAN_CIPHER_SUITE_CCMP:
			if (pairwise) {
				wsm_key->type = WSM_KEY_TYPE_AES_PAIRWISE;
				memcpy(wsm_key->aesPairwiseKey.peerAddress, peer_addr, ETH_ALEN);
				memcpy(wsm_key->aesPairwiseKey.aesKeyData, &key->key[0], 16);
				wiphy_debug(dev->wiphy, "CCMP_PAIRWISE keylen=%d!\n",
						key->keylen);
			} else {
				wsm_key->type = WSM_KEY_TYPE_AES_GROUP;
				memcpy(wsm_key->aesGroupKey.aesKeyData, &key->key[0], 16);
				/* TODO: Where can I find AES SEQ? */
				memset(wsm_key->aesGroupKey.rxSeqCounter, 0, 8);
				wsm_key->aesGroupKey.keyId = key->keyidx;
			}
			break;
#ifdef CONFIG_XRADIO_WAPI_SUPPORT
		case WLAN_CIPHER_SUITE_SMS4:
			if (pairwise) {
				wsm_key->type = WSM_KEY_TYPE_WAPI_PAIRWISE;
				memcpy(wsm_key->wapiPairwiseKey.peerAddress, peer_addr, ETH_ALEN);
				memcpy(wsm_key->wapiPairwiseKey.wapiKeyData, &key->key[0],  16);
				memcpy(wsm_key->wapiPairwiseKey.micKeyData, &key->key[16], 16);
				wsm_key->wapiPairwiseKey.keyId = key->keyidx;
				sta_printk(XRADIO_DBG_NIY,"%s: WAPI_PAIRWISE keylen=%d!\n",
			               __func__, key->keylen);
			} else {
				wsm_key->type = WSM_KEY_TYPE_WAPI_GROUP;
				memcpy(wsm_key->wapiGroupKey.wapiKeyData, &key->key[0],  16);
				memcpy(wsm_key->wapiGroupKey.micKeyData,  &key->key[16], 16);
				wsm_key->wapiGroupKey.keyId = key->keyidx;
				sta_printk(XRADIO_DBG_NIY,"%s: WAPI_GROUP keylen=%d!\n",
			               __func__, key->keylen);
			}
			break;
#endif /* CONFIG_XRADIO_WAPI_SUPPORT */
		default:
			wiphy_err(dev->wiphy, "key->cipher unknown(%d)!\n", key->cipher);
			xradio_free_key(hw_priv, idx);
			ret = -EOPNOTSUPP;
			goto finally;
		}
		ret = WARN_ON(wsm_add_key(hw_priv, wsm_key, priv->if_id));
		if (!ret)
			key->hw_key_idx = idx;
		else
			xradio_free_key(hw_priv, idx);

		if (!ret && (pairwise || wsm_key->type == WSM_KEY_TYPE_WEP_DEFAULT) && 
		    (priv->filter4.enable & 0x2))
			xradio_set_arpreply(dev, vif);
	} else if (cmd == DISABLE_KEY) {
		struct wsm_remove_key wsm_key = {
			.entryIndex = key->hw_key_idx,
		};

		if (wsm_key.entryIndex > WSM_KEY_MAX_IDX) {
			ret = -EINVAL;
			goto finally;
		}

		xradio_free_key(hw_priv, wsm_key.entryIndex);
		ret = wsm_remove_key(hw_priv, &wsm_key, priv->if_id);
	} else {
		wiphy_err(dev->wiphy, "Unsupported command\n");
	}

finally:
	mutex_unlock(&hw_priv->conf_mutex);
	return ret;
#endif // XRADIO_DISABLE_HW_CRYPTO
}
