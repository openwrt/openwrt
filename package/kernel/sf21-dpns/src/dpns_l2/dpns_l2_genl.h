#ifndef _L2_DEBUG_H_
#define _L2_DEBUG_H_

#include "dpns_l2.h"

u16 crc16_custom(const u8 *buf, size_t len, u8 poly_sel);
void sf_destroy_tslist(MAC_t *priv);
int sf_del_ts_info(MAC_t *priv, const u8 *mac, u16 vid, int nat_id, u16 soft_key_crc);
int dpns_mac_hw_search(MAC_t *priv, const u8 *dsmac, u16 vid, u32 *result);

int sf_mac_del_entry(MAC_t* priv, const u8 *dmac, u16 vlan_id, bool is_switchdev_event, bool is_netdev_event);

void sf_mac_spl_unlimit(MAC_t *priv, u16 spl_index, u32 credit);
bool sf_search_ts_entry(MAC_t *priv, u8 *mac, u16 vlan_id, int nat_id, u16 soft_key_crc);

void se_mac_table_dump(tbl_mac_t *table, u32 index);
void mac_iso_table_dump(MAC_t *priv, u8 iport_num);
int se_l2_mac_table_dump(MAC_t *priv);
int se_l2_hash_dump(MAC_t *priv);
void mac_spl_table_dump(MAC_t *priv, u8 index);

int sf_set_l2_mib_en(MAC_t *priv, u8 *dsmac, u16 vlan_id, u8 l2_mib_mode,
		     u16 mib_index, int mib_op);
int sf_set_l2_spl_en(MAC_t *priv, u8 *dsmac, u16 vlan_id,
		bool l2_spl_mode, u8 spl_index, u32 scredit, u32 dcredit);

int l2_mac_genl_msg_recv(struct genl_info *info, void *buf, size_t buflen);
void sf_mac_num_dump(MAC_t *priv);
#endif
