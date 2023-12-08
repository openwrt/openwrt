# All needed files would be added to _HAL_INTFS_FILES, and it would include
# hal_g6/phy/rf and all related files in directory hal_g6/phy/rf/.
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
path_halrf_d1 := $(phl_path)/phy/rf

halrf-y += $(path_halrf_d1)/halrf.o \
			$(path_halrf_d1)/halrf_pmac.o \
			$(path_halrf_d1)/halrf_api.o \
			$(path_halrf_d1)/halrf_dbg.o \
			$(path_halrf_d1)/halrf_dbg_cmd.o \
			$(path_halrf_d1)/halrf_ex.o \
			$(path_halrf_d1)/halrf_hw_cfg.o \
			$(path_halrf_d1)/halrf_init.o \
			$(path_halrf_d1)/halrf_interface.o \
			$(path_halrf_d1)/halrf_pwr_table.o \
			$(path_halrf_d1)/halrf_iqk.o \

ifeq ($(CONFIG_RTL8852A), y)
rf_ic := 8852a

# Level 2 directory
path_halrf_8852a := $(path_halrf_d1)/halrf_$(rf_ic)

halrf-y += $(path_halrf_8852a)/halrf_8852a.o \
			$(path_halrf_8852a)/halrf_8852a_api.o \
			$(path_halrf_8852a)/halrf_hwimg_8852a.o\
			$(path_halrf_8852a)/halrf_txgapk_8852a.o\
			$(path_halrf_8852a)/halrf_iqk_8852a.o\
			$(path_halrf_8852a)/halrf_reg_cfg_8852a.o\
			$(path_halrf_8852a)/halrf_dpk_8852a.o\
			$(path_halrf_8852a)/halrf_dack_8852a.o\
			$(path_halrf_8852a)/halrf_set_pwr_table_8852a.o\
			$(path_halrf_8852a)/halrf_efuse_8852a.o\
			$(path_halrf_8852a)/halrf_tssi_8852a.o\
			$(path_halrf_8852a)/halrf_kfree_8852a.o\
			$(path_halrf_8852a)/halrf_psd_8852a.o
endif

ifeq ($(CONFIG_RTL8852B), y)
rf_ic := 8852b

# Level 2 directory
path_halrf_8852b := $(path_halrf_d1)/halrf_$(rf_ic)

halrf-y += $(path_halrf_8852b)/halrf_8852b.o\
			$(path_halrf_8852b)/halrf_8852b_api.o \
			$(path_halrf_8852b)/halrf_hwimg_8852b.o\
			$(path_halrf_8852b)/halrf_txgapk_8852b.o\
			$(path_halrf_8852b)/halrf_iqk_8852b.o\
			$(path_halrf_8852b)/halrf_reg_cfg_8852b.o\
			$(path_halrf_8852b)/halrf_dack_8852b.o \
			$(path_halrf_8852b)/halrf_dpk_8852b.o \
			$(path_halrf_8852b)/halrf_set_pwr_table_8852b.o\
			$(path_halrf_8852b)/halrf_efuse_8852b.o\
			$(path_halrf_8852b)/halrf_tssi_8852b.o\
			$(path_halrf_8852b)/halrf_kfree_8852b.o\
			$(path_halrf_8852b)/halrf_psd_8852b.o
endif

ifeq ($(CONFIG_RTL8852C), y)
rf_ic := 8852c

# Level 2 directory
path_halrf_8852c := $(path_halrf_d1)/halrf_$(rf_ic)

halrf-y += $(path_halrf_8852c)/halrf_8852c.o \
			$(path_halrf_8852c)/halrf_8852c_api.o \
			$(path_halrf_8852c)/halrf_hwimg_8852c.o\
			$(path_halrf_8852c)/halrf_txgapk_8852c.o\
			$(path_halrf_8852c)/halrf_iqk_8852c.o\
			$(path_halrf_8852c)/halrf_reg_cfg_8852c.o \
			$(path_halrf_8852c)/halrf_set_pwr_table_8852c.o\
			$(path_halrf_8852c)/halrf_dack_8852c.o \
			$(path_halrf_8852c)/halrf_dpk_8852c.o \
			$(path_halrf_8852c)/halrf_efuse_8852c.o\
			$(path_halrf_8852c)/halrf_tssi_8852c.o\
			$(path_halrf_8852c)/halrf_kfree_8852c.o\
			$(path_halrf_8852c)/halrf_psd_8852c.o
endif

ifeq ($(CONFIG_RTL8832BR), y)
rf_ic := 8832br

# Level 2 directory
path_halrf_8832br := $(path_halrf_d1)/halrf_$(rf_ic)

halrf-y += $(path_halrf_8832br)/halrf_8832br.o \
			$(path_halrf_8832br)/halrf_8832br_api.o \
			$(path_halrf_8832br)/halrf_hwimg_8832br.o\
			$(path_halrf_8832br)/halrf_iqk_8832br.o\
			$(path_halrf_8832br)/halrf_reg_cfg_8832br.o \
			$(path_halrf_8832br)/halrf_set_pwr_table_8832br.o \
			$(path_halrf_8832br)/halrf_dack_8832br.o \
			$(path_halrf_8832br)/halrf_dpk_8832br.o \
			$(path_halrf_8832br)/halrf_efuse_8832br.o \
			$(path_halrf_8832br)/halrf_txgapk_8832br.o \
			$(path_halrf_8832br)/halrf_tssi_8832br.o \
			$(path_halrf_8832br)/halrf_kfree_8832br.o\
			$(path_halrf_8832br)/halrf_psd_8832br.o
endif

ifeq ($(CONFIG_RTL8192XB), y)
rf_ic := 8192xb

# Level 2 directory
path_halrf_8192xb := $(path_halrf_d1)/halrf_$(rf_ic)

halrf-y += $(path_halrf_8192xb)/halrf_8192xb.o \
			$(path_halrf_8192xb)/halrf_8192xb_api.o \
			$(path_halrf_8192xb)/halrf_hwimg_8192xb.o\
			$(path_halrf_8192xb)/halrf_iqk_8192xb.o\
			$(path_halrf_8192xb)/halrf_reg_cfg_8192xb.o \
			$(path_halrf_8192xb)/halrf_set_pwr_table_8192xb.o \
			$(path_halrf_8192xb)/halrf_dack_8192xb.o \
			$(path_halrf_8192xb)/halrf_dpk_8192xb.o \
			$(path_halrf_8192xb)/halrf_efuse_8192xb.o \
			$(path_halrf_8192xb)/halrf_txgapk_8192xb.o \
			$(path_halrf_8192xb)/halrf_tssi_8192xb.o \
			$(path_halrf_8192xb)/halrf_kfree_8192xb.o\
			$(path_halrf_8192xb)/halrf_psd_8192xb.o
endif

ifeq ($(CONFIG_RTL8852BP), y)
rf_ic := 8852bp

# Level 2 directory
path_halrf_8852bp := $(path_halrf_d1)/halrf_$(rf_ic)

halrf-y += $(path_halrf_8852bp)/halrf_8852bp.o \
			$(path_halrf_8852bp)/halrf_8852bp_api.o \
			$(path_halrf_8852bp)/halrf_hwimg_8852bp.o\
			$(path_halrf_8852bp)/halrf_iqk_8852bp.o\
			$(path_halrf_8852bp)/halrf_reg_cfg_8852bp.o \
			$(path_halrf_8852bp)/halrf_set_pwr_table_8852bp.o\
			$(path_halrf_8852bp)/halrf_dack_8852bp.o \
			$(path_halrf_8852bp)/halrf_dpk_8852bp.o \
			$(path_halrf_8852bp)/halrf_efuse_8852bp.o\
			$(path_halrf_8852bp)/halrf_txgapk_8852bp.o \
			$(path_halrf_8852bp)/halrf_tssi_8852bp.o\
			$(path_halrf_8852bp)/halrf_kfree_8852bp.o\
			$(path_halrf_8852bp)/halrf_psd_8852bp.o
endif

ifeq ($(CONFIG_RTL8730A), y)
rf_ic := 8730a

# Level 2 directory
path_halrf_8730a := $(path_halrf_d1)/halrf_$(rf_ic)

halrf-y += $(path_halrf_8730a)/halrf_8730a.o \
			$(path_halrf_8730a)/halrf_8730a.o \
			$(path_halrf_8730a)/halrf_hwimg_8730a.o\
			$(path_halrf_8730a)/halrf_iqk_8730a.o\
			$(path_halrf_8730a)/halrf_reg_cfg_8730a.o \
			$(path_halrf_8730a)/halrf_set_pwr_table_8730a.o\
			$(path_halrf_8730a)/halrf_dack_8730a.o \
			$(path_halrf_8730a)/halrf_dpk_8730a.o \
			$(path_halrf_8730a)/halrf_efuse_8730a.o\
			$(path_halrf_8730a)/halrf_tssi_8730a.o\
			$(path_halrf_8730a)/halrf_kfree_8730a.o\
			$(path_halrf_8730a)/halrf_psd_8730a.o
endif

ifeq ($(CONFIG_RTL8851B), y)
rf_ic := 8851b

# Level 2 directory
path_halrf_8851b := $(path_halrf_d1)/halrf_$(rf_ic)

halrf-y += $(path_halrf_8851b)/halrf_8851b.o \
			$(path_halrf_8851b)/halrf_8851b_api.o \
			$(path_halrf_8851b)/halrf_hwimg_8851b.o\
			$(path_halrf_8851b)/halrf_iqk_8851b.o\
			$(path_halrf_8851b)/halrf_reg_cfg_8851b.o \
			$(path_halrf_8851b)/halrf_set_pwr_table_8851b.o\
			$(path_halrf_8851b)/halrf_dack_8851b.o \
			$(path_halrf_8851b)/halrf_dpk_8851b.o \
			$(path_halrf_8851b)/halrf_efuse_8851b.o\
			$(path_halrf_8851b)/halrf_tssi_8851b.o\
			$(path_halrf_8851b)/halrf_kfree_8851b.o\
			$(path_halrf_8851b)/halrf_psd_8851b.o
endif

_HAL_RF_FILES +=	$(halrf-y)
