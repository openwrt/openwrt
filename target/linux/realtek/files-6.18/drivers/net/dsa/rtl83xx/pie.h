/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef _OTTO_PIE_H
#define _OTTO_PIE_H

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
#define RTL839X_PIE_L3_RULES	(4 * PIE_BLOCK_SIZE)
#define RTL839X_PIE_L3_FIRST	(RTL839X_PIE_CUTLINE * PIE_BLOCK_SIZE - RTL839X_PIE_L3_RULES)

struct pie_rule;
struct rtl838x_switch_priv;

void otto_pie_838x_init(struct rtl838x_switch_priv *priv);
int otto_pie_838x_rule_read(struct rtl838x_switch_priv *priv, int idx, struct pie_rule *pr);
int otto_pie_838x_rule_write(struct rtl838x_switch_priv *priv, int idx, struct pie_rule *pr);
int otto_pie_838x_rule_add(struct rtl838x_switch_priv *priv, struct pie_rule *pr);
void otto_pie_838x_rule_rm(struct rtl838x_switch_priv *priv, struct pie_rule *pr);

void otto_pie_839x_init(struct rtl838x_switch_priv *priv);
int otto_pie_839x_rule_read(struct rtl838x_switch_priv *priv, int idx, struct pie_rule *pr);
int otto_pie_839x_rule_write(struct rtl838x_switch_priv *priv, int idx, struct pie_rule *pr);
int otto_pie_839x_rule_add(struct rtl838x_switch_priv *priv, struct pie_rule *pr);
void otto_pie_839x_rule_rm(struct rtl838x_switch_priv *priv, struct pie_rule *pr);
int otto_pie_839x_l3_rule_read(struct rtl838x_switch_priv *priv, int idx, struct pie_rule *pr);
int otto_pie_839x_l3_rule_write(struct rtl838x_switch_priv *priv, int idx, struct pie_rule *pr);
void otto_pie_839x_rule_dump(struct pie_rule *pr);

void otto_pie_930x_init(struct rtl838x_switch_priv *priv);
int otto_pie_930x_rule_write(struct rtl838x_switch_priv *priv, int idx, struct pie_rule *pr);
int otto_pie_930x_rule_add(struct rtl838x_switch_priv *priv, struct pie_rule *pr);
void otto_pie_930x_rule_rm(struct rtl838x_switch_priv *priv, struct pie_rule *pr);
void otto_pie_930x_rule_dump_raw(u32 r[]);

void otto_pie_931x_init(struct rtl838x_switch_priv *priv);
int otto_pie_931x_rule_write(struct rtl838x_switch_priv *priv, int idx, struct pie_rule *pr);
int otto_pie_931x_rule_add(struct rtl838x_switch_priv *priv, struct pie_rule *pr);
void otto_pie_931x_rule_rm(struct rtl838x_switch_priv *priv, struct pie_rule *pr);

#endif /* _OTTO_PIE_H */
