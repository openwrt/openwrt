#ifndef __REALTEK_CLK_OF_CONF_H
#define __REALTEK_CLK_OF_CONF_H

struct device_node;
struct regmap;

void of_rtk_clk_setup_crt(struct device_node *np, struct regmap *regmap);

#endif
