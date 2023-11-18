# All needed files would be added to _HAL_INTFS_FILES, and it would include
# hal_g6/phy/bb and all related files in directory hal_g6/phy/bb/.
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
path_halbb_d1 := $(phl_path)/phy/bb

halbb-y += $(path_halbb_d1)/halbb.o \
			$(path_halbb_d1)/halbb_api.o \
			$(path_halbb_d1)/halbb_rua_tbl.o \
			$(path_halbb_d1)/halbb_auto_dbg.o\
			$(path_halbb_d1)/halbb_cfo_trk.o \
			$(path_halbb_d1)/halbb_ch_info.o \
			$(path_halbb_d1)/halbb_cmn_rpt.o \
			$(path_halbb_d1)/halbb_dbcc.o \
			$(path_halbb_d1)/halbb_dbg.o \
			$(path_halbb_d1)/halbb_dbg_cmd.o \
			$(path_halbb_d1)/halbb_dfs.o \
			$(path_halbb_d1)/halbb_edcca.o \
			$(path_halbb_d1)/halbb_env_mntr.o \
			$(path_halbb_d1)/halbb_hw_cfg.o \
			$(path_halbb_d1)/halbb_init.o \
			$(path_halbb_d1)/halbb_interface.o \
			$(path_halbb_d1)/halbb_la_mode.o \
			$(path_halbb_d1)/halbb_math_lib.o \
			$(path_halbb_d1)/halbb_mp.o \
			$(path_halbb_d1)/halbb_spur_suppress.o \
			$(path_halbb_d1)/halbb_plcp_gen.o \
			$(path_halbb_d1)/halbb_plcp_tx.o \
			$(path_halbb_d1)/halbb_plcp_tx_7.o \
			$(path_halbb_d1)/halbb_pmac_setting.o \
			$(path_halbb_d1)/halbb_psd.o \
			$(path_halbb_d1)/halbb_ul_tb_ctrl.o \
			$(path_halbb_d1)/halbb_physts.o \
			$(path_halbb_d1)/halbb_physts_7.o \
			$(path_halbb_d1)/halbb_pwr_ctrl.o \
			$(path_halbb_d1)/halbb_ra.o \
			$(path_halbb_d1)/halbb_statistics.o \
			$(path_halbb_d1)/halbb_ant_div.o \
			$(path_halbb_d1)/halbb_dig.o \
			$(path_halbb_d1)/halbb_fwofld.o \
			$(path_halbb_d1)/halbb_dyn_csi_rsp.o \
			$(path_halbb_d1)/halbb_path_div.o \
			$(path_halbb_d1)/halbb_dyn_1r_cca.o \
			$(path_halbb_d1)/halbb_snif.o\
			$(path_halbb_d1)/halbb_dyn_dtr.o\
			$(path_halbb_d1)/halbb_dbg_cnsl_out.o

ifeq ($(CONFIG_RTL8852A), y)
bb_ic := 8852a
# Level 2 directory
path_halbb_8852a := $(path_halbb_d1)/halbb_$(bb_ic)

halbb-y += $(path_halbb_8852a)/halbb_8852a.o \
			$(path_halbb_8852a)/halbb_8852a_api.o \
			$(path_halbb_8852a)/halbb_hwimg_8852a.o \
			$(path_halbb_8852a)/halbb_reg_cfg_8852a.o
endif

ifeq ($(CONFIG_RTL8852A), y)
bb_ic := 8852a_2
# Level 2 directory
path_halbb_8852a_2 := $(path_halbb_d1)/halbb_$(bb_ic)

halbb-y += $(path_halbb_8852a_2)/halbb_8852a_2.o \
			$(path_halbb_8852a_2)/halbb_8852a_2_api.o \
			$(path_halbb_8852a_2)/halbb_hwimg_8852a_2.o \
			$(path_halbb_8852a_2)/halbb_reg_cfg_8852a_2.o
endif

ifeq ($(CONFIG_RTL8852B), y)
bb_ic := 8852b
# Level 2 directory
path_halbb_8852b := $(path_halbb_d1)/halbb_$(bb_ic)

halbb-y += $(path_halbb_8852b)/halbb_8852b.o \
			$(path_halbb_8852b)/halbb_8852b_api.o \
			$(path_halbb_8852b)/halbb_8852b_fwofld_api.o \
			$(path_halbb_8852b)/halbb_hwimg_8852b.o \
			$(path_halbb_8852b)/halbb_reg_cfg_8852b.o
endif

ifeq ($(CONFIG_RTL8852C), y)
bb_ic := 8852c
# Level 2 directory
path_halbb_8852c := $(path_halbb_d1)/halbb_$(bb_ic)

halbb-y += $(path_halbb_8852c)/halbb_8852c.o \
			$(path_halbb_8852c)/halbb_8852c_api.o \
			$(path_halbb_8852c)/halbb_8852c_fwofld_api.o \
			$(path_halbb_8852c)/halbb_hwimg_8852c.o \
			$(path_halbb_8852c)/halbb_reg_cfg_8852c.o \
			$(path_halbb_8852c)/halbb_8852c_dcra.o 
endif

ifeq ($(CONFIG_RTL8832BR), y)
bb_ic := 8192xb
# Level 2 directory
path_halbb_8192xb := $(path_halbb_d1)/halbb_$(bb_ic)

halbb-y += $(path_halbb_8192xb)/halbb_8192xb.o \
			$(path_halbb_8192xb)/halbb_8192xb_api.o \
			$(path_halbb_8192xb)/halbb_hwimg_8192xb.o \
			$(path_halbb_8192xb)/halbb_reg_cfg_8192xb.o
endif

ifeq ($(CONFIG_RTL8192XB), y)
bb_ic := 8192xb
# Level 2 directory
path_halbb_8192xb := $(path_halbb_d1)/halbb_$(bb_ic)

halbb-y += $(path_halbb_8192xb)/halbb_8192xb.o \
			$(path_halbb_8192xb)/halbb_8192xb_api.o \
			$(path_halbb_8192xb)/halbb_hwimg_8192xb.o \
			$(path_halbb_8192xb)/halbb_reg_cfg_8192xb.o
endif

ifeq ($(CONFIG_RTL8852BP), y)
bb_ic := 8852b
# Level 2 directory
path_halbb_8852b := $(path_halbb_d1)/halbb_$(bb_ic)

halbb-y += $(path_halbb_8852b)/halbb_8852b.o \
			$(path_halbb_8852b)/halbb_8852b_api.o \
			$(path_halbb_8852b)/halbb_8852b_fwofld_api.o \
			$(path_halbb_8852b)/halbb_hwimg_8852b.o \
			$(path_halbb_8852b)/halbb_reg_cfg_8852b.o
endif

ifeq ($(CONFIG_RTL8922A), y)
bb_ic := 8922a
# Level 2 directory
path_halbb_8922a := $(path_halbb_d1)/halbb_$(bb_ic)
path_halbb_bbmcu := $(path_halbb_d1)/bbmcu

halbb-y += $(path_halbb_8922a)/halbb_8922a.o \
			$(path_halbb_8922a)/halbb_8922a_api.o \
            $(path_halbb_8922a)/halbb_hwimg_8922a.o \
            $(path_halbb_8922a)/halbb_reg_cfg_8922a.o

halbb-y += $(path_halbb_bbmcu)/bbmcu.o\
            $(path_halbb_bbmcu)/bbmcu_init.o
endif

ifeq ($(CONFIG_RTL8851B), y)
bb_ic := 8851b
# Level 2 directory
path_halbb_8851b := $(path_halbb_d1)/halbb_$(bb_ic)

halbb-y += $(path_halbb_8851b)/halbb_8851b.o \
			$(path_halbb_8851b)/halbb_8851b_api.o \
			$(path_halbb_8851b)/halbb_8851b_fwofld_api.o \
			$(path_halbb_8851b)/halbb_hwimg_8851b.o \
			$(path_halbb_8851b)/halbb_reg_cfg_8851b.o
endif

_HAL_BB_FILES +=	$(halbb-y)
