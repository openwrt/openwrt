/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef _OTTO_PIE_H
#define _OTTO_PIE_H

#include <linux/types.h>

struct pie_rule;
struct rtl838x_switch_priv;

void rtl838x_pie_init(struct rtl838x_switch_priv *priv);
int rtl838x_pie_rule_read(struct rtl838x_switch_priv *priv, int idx,
			  struct pie_rule *pr);
int rtl838x_pie_rule_write(struct rtl838x_switch_priv *priv, int idx,
			   struct pie_rule *pr);
int rtl838x_pie_rule_add(struct rtl838x_switch_priv *priv, struct pie_rule *pr);
void rtl838x_pie_rule_rm(struct rtl838x_switch_priv *priv, struct pie_rule *pr);

void rtl839x_pie_init(struct rtl838x_switch_priv *priv);
int rtl839x_pie_rule_read(struct rtl838x_switch_priv *priv, int idx,
			  struct pie_rule *pr);
int rtl839x_pie_rule_write(struct rtl838x_switch_priv *priv, int idx,
			   struct pie_rule *pr);
int rtl839x_pie_rule_add(struct rtl838x_switch_priv *priv, struct pie_rule *pr);
void rtl839x_pie_rule_rm(struct rtl838x_switch_priv *priv, struct pie_rule *pr);
void rtl839x_pie_rule_dump(struct pie_rule *pr);

void rtl930x_pie_init(struct rtl838x_switch_priv *priv);
int rtl930x_pie_rule_write(struct rtl838x_switch_priv *priv, int idx,
			   struct pie_rule *pr);
int rtl930x_pie_rule_add(struct rtl838x_switch_priv *priv, struct pie_rule *pr);
void rtl930x_pie_rule_rm(struct rtl838x_switch_priv *priv, struct pie_rule *pr);
void rtl930x_pie_rule_dump_raw(u32 r[]);

void rtl931x_pie_init(struct rtl838x_switch_priv *priv);
int rtl931x_pie_rule_write(struct rtl838x_switch_priv *priv, int idx,
			   struct pie_rule *pr);
int rtl931x_pie_rule_add(struct rtl838x_switch_priv *priv, struct pie_rule *pr);
void rtl931x_pie_rule_rm(struct rtl838x_switch_priv *priv, struct pie_rule *pr);

#endif /* _OTTO_PIE_H */
