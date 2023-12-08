########### HAL PATH  #################################

ifeq ($(CONFIG_PHL_ARCH), y)
phl_path := phl/
phl_path_d1 := $(src)/phl/$(HAL)
else
phl_path :=
phl_path_d1 := $(src)/$(HAL)
endif

_HAL_FILES :=	$(phl_path)$(HAL)/hal_api_mac.o \
			$(phl_path)$(HAL)/hal_api_bb.o \
			$(phl_path)$(HAL)/hal_api_rf.o \
			$(phl_path)$(HAL)/hal_api_btc.o \
			$(phl_path)$(HAL)/hal_api_efuse.o \
			$(phl_path)$(HAL)/hal_com_i.o \
			$(phl_path)$(HAL)/hal_init.o \
			$(phl_path)$(HAL)/hal_io.o \
			$(phl_path)$(HAL)/hal_rx.o \
			$(phl_path)$(HAL)/hal_tx.o \
			$(phl_path)$(HAL)/hal_sta.o \
			$(phl_path)$(HAL)/hal_cam.o \
			$(phl_path)$(HAL)/hal_csi_buffer.o \
			$(phl_path)$(HAL)/hal_beamform.o \
			$(phl_path)$(HAL)/hal_sound.o \
			$(phl_path)$(HAL)/hal_chan.o \
			$(phl_path)$(HAL)/hal_str_proc.o \
			$(phl_path)$(HAL)/hal_fw.o \
			$(phl_path)$(HAL)/hal_cap.o \
			$(phl_path)$(HAL)/hal_ser.o \
			$(phl_path)$(HAL)/hal_ps.o \
			$(phl_path)$(HAL)/hal_c2h.o \
			$(phl_path)$(HAL)/hal_dbcc.o \
			$(phl_path)$(HAL)/hal_chan_info.o \
			$(phl_path)$(HAL)/hal_wow.o \
			$(phl_path)$(HAL)/hal_ld_file.o \
			$(phl_path)$(HAL)/hal_regulation.o \
			$(phl_path)$(HAL)/hal_led.o \
			$(phl_path)$(HAL)/hal_trx_mit.o \
			$(phl_path)$(HAL)/hal_acs.o \
			$(phl_path)$(HAL)/hal_mcc.o \
			$(phl_path)$(HAL)/hal_api.o \
			$(phl_path)$(HAL)/hal_twt.o \
			$(phl_path)$(HAL)/hal_notify.o \
			$(phl_path)$(HAL)/hal_p2pps.o \
			$(phl_path)$(HAL)/hal_thermal.o \
			$(phl_path)$(HAL)/hal_txpwr.o \
			$(phl_path)$(HAL)/hal_ext_tx_pwr_lmt.o \
			$(phl_path)$(HAL)/hal_dfs.o \
			$(phl_path)$(HAL)/hal_scanofld.o

ifeq ($(CONFIG_PCI_HCI), y)
_HAL_FILES += $(phl_path)$(HAL)/hal_pci.o
endif
ifeq ($(CONFIG_USB_HCI), y)
_HAL_FILES += $(phl_path)$(HAL)/hal_usb.o
endif
ifeq ($(CONFIG_SDIO_HCI), y)
_HAL_FILES += $(phl_path)$(HAL)/hal_sdio.o
endif
########### hal test #################################
ifeq ($(CONFIG_PHL_TEST_SUITE), y)
_HAL_FILES += $(phl_path)$(HAL)/test/hal_test_module.o
_HAL_FILES += $(phl_path)$(HAL)/test/mp/hal_test_mp.o
_HAL_FILES += $(phl_path)$(HAL)/test/mp/hal_test_mp_cal.o
_HAL_FILES += $(phl_path)$(HAL)/test/mp/hal_test_mp_config.o
_HAL_FILES += $(phl_path)$(HAL)/test/mp/hal_test_mp_efuse.o
_HAL_FILES += $(phl_path)$(HAL)/test/mp/hal_test_mp_reg.o
_HAL_FILES += $(phl_path)$(HAL)/test/mp/hal_test_mp_rx.o
_HAL_FILES += $(phl_path)$(HAL)/test/mp/hal_test_mp_tx.o
_HAL_FILES += $(phl_path)$(HAL)/test/mp/hal_test_mp_txpwr.o
endif

_HAL_FILES += $(phl_path)$(HAL)/efuse/hal_efuse.o
include $(phl_path_d1)/mac/mac.mk

ifeq ($(USE_TRUE_PHY), y)
include $(phl_path_d1)/phy/bb/bb.mk
include $(phl_path_d1)/phy/rf/rf.mk
endif

########### HALBTC #######################################
include $(phl_path_d1)/btc/btc.mk
########### HAL_RTL8852A #################################
ifeq ($(CONFIG_RTL8852A), y)
include $(phl_path_d1)/$(IC_NAME)/rtl8852a.mk
endif
########### HAL_RTL8852B #################################
ifneq ($(filter y,$(CONFIG_RTL8852B) $(CONFIG_RTL8852BP)),)
include $(phl_path_d1)/rtl8852b/rtl8852b.mk
endif
########### HAL_RTL8852C #################################
ifeq ($(CONFIG_RTL8852C), y)
include $(phl_path_d1)/rtl8852c/rtl8852c.mk
endif
########### HAL_RTL8192XB #################################
ifneq ($(filter y,$(CONFIG_RTL8832BR) $(CONFIG_RTL8192XB)),)
include $(phl_path_d1)/rtl8192xb/rtl8192xb.mk
endif
########### HAL_RTL8982A #################################
ifeq ($(CONFIG_RTL8982A), y)
include $(phl_path_d1)/$(IC_NAME)/rtl8982a.mk
endif
########### HAL_RTL8851B #################################
ifeq ($(CONFIG_RTL8851B), y)
include $(phl_path_d1)/$(IC_NAME)/rtl8851b.mk
endif

OBJS += $(_HAL_FILES) $(_HAL_MAC_FILES) $(_BTC_FILES) $(_HAL_IC_FILES)
OBJS += $(_HAL_BB_FILES) $(_HAL_RF_FILES) $(_HAL_EFUSE_FILES)
