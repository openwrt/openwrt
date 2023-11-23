# All needed files would be added to _HAL_INTFS_FILES, and it would include
# hal_g6/btc and all related files in directory hal_g6/btc/.
# Before include this makefile, be sure interface (CONFIG_*_HCI) and IC
# (CONFIG_RTL*) setting are all ready!

#HAL = hal_g6

ifeq ($(CONFIG_PHL_ARCH), y)
phl_path := phl/$(HAL)
phl_path_d1 := $(src)/phl/$(HAL)
else
phl_path := $(HAL)
phl_path_d1 := $(src)/$(HAL)
endif

# Base directory
path_halbtc_d1 := $(phl_path)/btc

halbtc-y += $(path_halbtc_d1)/hal_btc.o \
			$(path_halbtc_d1)/halbtc_def.o \
			$(path_halbtc_d1)/halbtc_action.o \
			$(path_halbtc_d1)/halbtc_fw.o \
			$(path_halbtc_d1)/halbtc_dbg_cmd.o

ifeq ($(CONFIG_RTL8852A), y)
ic := 8852a
# Level 2 directory
path_halbtc_8852a := $(path_halbtc_d1)/btc_$(ic)

halbtc-y += $(path_halbtc_8852a)/btc_8852a.o

endif

# 8852B/8852BP Support
ifneq ($(filter y,$(CONFIG_RTL8852B) $(CONFIG_RTL8852BP) $(CONFIG_RTL8851B)),)
ic := 8852b
# Level 2 directory
path_halbtc_8852b := $(path_halbtc_d1)/btc_$(ic)

halbtc-y += $(path_halbtc_8852b)/btc_8852b.o
endif

ifeq ($(CONFIG_RTL8852C), y)
ic := 8852c
# Level 2 directory
path_halbtc_8852c := $(path_halbtc_d1)/btc_$(ic)

halbtc-y += $(path_halbtc_8852c)/btc_8852c.o
endif

_BTC_FILES +=	$(halbtc-y)
