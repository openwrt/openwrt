#ifndef _DPNS_NAT_ILKP4_H_
#define _DPNS_NAT_ILKP4_H_

#include <nat.h>

int dpns_nat_ilkp_init(struct dpns_nat_priv *priv);
void dpns_nat_ilkp_exit(struct dpns_nat_priv *priv);
int dpns_nat_add_napt4(struct dpns_nat_priv *priv, struct dpns_nat_entry *entry,
		       bool is_lf, bool is_dnat, struct nat_ipv4_data *data);
void dpns_nat_rm_ilkp4_hw(struct dpns_nat_priv *priv, struct dpns_nat_entry *entry);

int dpns_nat_add_napt6(struct dpns_nat_priv *priv, struct dpns_nat_entry *entry,
		       bool is_lf, bool is_dnat, struct nat_ipv6_data *data);
void dpns_nat_rm_ilkp6_hw(struct dpns_nat_priv *priv, struct dpns_nat_entry *entry);
void dpns_ip6_hton(u32 *dst, u32 *src);
u16 crc16_custom(const u8 *buf, size_t len, u8 poly_sel);
void dpns_nat_free_ilkp4_entry(struct dpns_nat_priv *priv, int nat_id);
void dpns_nat_free_ilkp6_entry(struct dpns_nat_priv *priv, int nat_id);
void dpns_nat_rm_ihash(struct dpns_nat_priv *priv, struct dpns_nat_entry *entry);
#endif
