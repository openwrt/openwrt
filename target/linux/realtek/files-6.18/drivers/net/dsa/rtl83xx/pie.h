/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef _OTTO_PIE_H
#define _OTTO_PIE_H

#include <linux/soc/realtek/otto_common.h>
#include <linux/types.h>

/* PIE actions */
#define PIE_ACT_COPY_TO_PORT	2
#define PIE_ACT_REDIRECT_TO_PORT 4
#define PIE_ACT_ROUTE_UC	6
#define PIE_ACT_VID_ASSIGN	0

#define PIE_BLOCK_SIZE 128
#define MAX_PIE_ENTRIES (18 * PIE_BLOCK_SIZE)
#define MAX_COUNTERS 2048

/* L3's rules are the last four of the RTL839x ingress blocks 0-8, after the
 * ingress ACL ones: the lower block wins the forwarding decision, and tc
 * ingress comes before routing.
 */
#define RTL839X_PIE_CUTLINE	9
#define RTL839X_PIE_L3_RULES	(RTL839X_PIE_CUTLINE * PIE_BLOCK_SIZE - RTL839X_PIE_L3_FIRST)

/* Fasttrack uses blocks 4..15 as VACL; blocks 0..3 retain priority for tc.
 * Split its twelve blocks equally between IPv4 rows and IPv6 row pairs.
 */
#define RTL930X_PIE_FLOW_FIRST	(4 * PIE_BLOCK_SIZE)
#define RTL930X_PIE_FLOW6_FIRST	(10 * PIE_BLOCK_SIZE)
#define RTL930X_PIE_FLOW_END	(16 * PIE_BLOCK_SIZE)

struct pie_rule;
struct otto_dsa_priv;
struct seq_file;

void otto_pie_838x_init(struct otto_dsa_priv *priv);
int otto_pie_838x_rule_read(struct otto_dsa_priv *priv, int idx, struct pie_rule *pr);
int otto_pie_838x_rule_write(struct otto_dsa_priv *priv, int idx, struct pie_rule *pr);
int otto_pie_838x_rule_add(struct otto_dsa_priv *priv, struct pie_rule *pr);
void otto_pie_838x_rule_rm(struct otto_dsa_priv *priv, struct pie_rule *pr);

void otto_pie_839x_init(struct otto_dsa_priv *priv);
int otto_pie_839x_rule_read(struct otto_dsa_priv *priv, int idx, struct pie_rule *pr);
int otto_pie_839x_rule_write(struct otto_dsa_priv *priv, int idx, struct pie_rule *pr);
int otto_pie_839x_rule_add(struct otto_dsa_priv *priv, struct pie_rule *pr);
void otto_pie_839x_rule_rm(struct otto_dsa_priv *priv, struct pie_rule *pr);
int otto_pie_839x_l3_rule_read(struct otto_dsa_priv *priv, int idx, struct pie_rule *pr);
int otto_pie_839x_l3_rule_write(struct otto_dsa_priv *priv, int idx, struct pie_rule *pr);
void otto_pie_839x_rule_dump(struct pie_rule *pr);

void otto_pie_930x_init(struct otto_dsa_priv *priv);
int otto_pie_930x_rule_write(struct otto_dsa_priv *priv, int idx, struct pie_rule *pr);
int otto_pie_930x_rule_add(struct otto_dsa_priv *priv, struct pie_rule *pr);
void otto_pie_930x_rule_rm(struct otto_dsa_priv *priv, struct pie_rule *pr);
void otto_pie_930x_rule_dump_raw(u32 r[]);
int otto_pie_930x_flow_init(struct otto_dsa_priv *priv);
int otto_pie_930x_flow_cleanup(struct otto_dsa_priv *priv);
int otto_pie_930x_flow_dump(struct otto_dsa_priv *priv, struct seq_file *s,
			  const struct pie_rule *pr);
int otto_pie_930x_flow_rule_add(struct otto_dsa_priv *priv, struct pie_rule *pr);
void otto_pie_930x_flow_rule_rm(struct otto_dsa_priv *priv, struct pie_rule *pr);

void otto_pie_931x_init(struct otto_dsa_priv *priv);
int otto_pie_931x_rule_write(struct otto_dsa_priv *priv, int idx, struct pie_rule *pr);
int otto_pie_931x_rule_add(struct otto_dsa_priv *priv, struct pie_rule *pr);
void otto_pie_931x_rule_rm(struct otto_dsa_priv *priv, struct pie_rule *pr);

#endif /* _OTTO_PIE_H */
