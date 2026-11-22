# SPDX-License-Identifier: GPL-2.0-only
# Siflower SF21 DPNS (NPU) dataplane drivers, based on the Siflower OpenWrt 22.03 BSP.
# Built as one kbuild tree so the inter-module symbols resolve without extra symvers handling.

obj-m += dpns_genl/ dpns_common/ dpns_l2/ dpns_vlan/ dpns_router/ dpns_nat/ dpns_tmu/ dpns/

# include paths and feature defines from the package Makefile
subdir-ccflags-y += $(DPNS_CFLAGS)
