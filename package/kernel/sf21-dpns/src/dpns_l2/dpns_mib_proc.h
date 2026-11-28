
#ifndef _L2_PROCFS_H_
#define _L2_PROCFS_H_
#include "dpns_l2.h"

#define CRC_LEN 	4
#define PREAMBLE_LEN 	8
#define IFG_LEN 	12

u16 crc16_custom(const u8 *buf, size_t len, u8 poly_sel);
u8 sf_ts_mode(MAC_t *priv, const u8 *mac, u16 vlan_id,
                                int nat_id, u16 soft_key_crc);
int dpns_mac_hw_search(MAC_t *priv, const u8 *dsmac, u16 vid, u32 *result);
bool sf_search_ts_entry(MAC_t *priv, u8 *mac, u16 vlan_id, int nat_id, u16 soft_key_crc);
int sf_del_ts_info(MAC_t *priv, const u8 *mac, u16 vid, int nat_id, u16 soft_key_crc);

int dpns_mib(MAC_t* priv);

int dpns_mib_proc_init(MAC_t *priv);
int dpns_mib_proc_exit(void);
#endif
