# SPDX-License-Identifier: GPL-2.0-only
obj-$(CONFIG_RTK_CLK_COMMON) += clk-rtk.o

clk-rtk-y += common.o freq_table.o
clk-rtk-y += of-conf.o
clk-rtk-y += clk-regmap-mux.o
clk-rtk-y += clk-regmap-gate.o
clk-rtk-y += clk-pll.o
clk-rtk-$(CONFIG_RTK_CLK_PLL_MMC) += clk-pll-mmc.o
clk-rtk-y += reset.o

obj-$(CONFIG_COMMON_CLK_RTD1295) += clk-rtd1295-cc.o
obj-$(CONFIG_COMMON_CLK_RTD1295) += clk-rtd1295-ic.o
obj-$(CONFIG_COMMON_CLK_RTD1619B) += clk-rtd1619b-cc.o
obj-$(CONFIG_COMMON_CLK_RTD1619B) += clk-rtd1619b-ic.o
obj-$(CONFIG_COMMON_CLK_RTD1319) += clk-rtd1319-cc.o
obj-$(CONFIG_COMMON_CLK_RTD1319) += clk-rtd1319-ic.o
