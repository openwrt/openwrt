########### OS_DEP PATH  #################################
_OS_INTFS_FILES :=	os_dep/osdep_service.o \
			os_dep/osdep_service_linux.o \
			os_dep/linux/rtw_cfg.o \
			os_dep/linux/os_intfs.o \
			os_dep/linux/ioctl_linux.o \
			os_dep/linux/xmit_linux.o \
			os_dep/linux/mlme_linux.o \
			os_dep/linux/recv_linux.o \
			os_dep/linux/ioctl_cfg80211.o \
			os_dep/linux/rtw_cfgvendor.o \
			os_dep/linux/os_ch_utils.o \
			os_dep/linux/wifi_regd.o \
			os_dep/linux/rtw_android.o \
			os_dep/linux/rtw_proc.o \
			os_dep/linux/nlrtw.o \
			os_dep/linux/rtw_rhashtable.o

ifeq ($(CONFIG_HWSIM), y)
	_OS_INTFS_FILES += os_dep/linux/hwsim/medium/local.o
	_OS_INTFS_FILES += os_dep/linux/hwsim/medium/sock_udp.o
	_OS_INTFS_FILES += os_dep/linux/hwsim/medium/loopback.o
	_OS_INTFS_FILES += os_dep/linux/hwsim/core.o
	_OS_INTFS_FILES += os_dep/linux/hwsim/txrx.o
	_OS_INTFS_FILES += os_dep/linux/hwsim/netdev.o
	_OS_INTFS_FILES += os_dep/linux/hwsim/cfg80211.o
	_OS_INTFS_FILES += os_dep/linux/hwsim/platform_dev.o

	_OS_INTFS_FILES += os_dep/linux/$(HCI_NAME)_ops_linux.o
else
	_OS_INTFS_FILES += os_dep/linux/$(HCI_NAME)_intf.o
	_OS_INTFS_FILES += os_dep/linux/$(HCI_NAME)_ops_linux.o
endif

ifeq ($(CONFIG_MP_INCLUDED), y)
_OS_INTFS_FILES += os_dep/linux/ioctl_mp.o \
		os_dep/linux/ioctl_efuse.o
endif

ifeq ($(CONFIG_SDIO_HCI), y)
_OS_INTFS_FILES += os_dep/linux/custom_gpio_linux.o
endif

ifeq ($(CONFIG_GSPI_HCI), y)
_OS_INTFS_FILES += os_dep/linux/custom_gpio_linux.o
endif

########### CORE PATH  #################################
_CORE_FILES :=	core/rtw_cmd.o \
		core/rtw_security.o \
		core/rtw_debug.o \
		core/rtw_io.o \
		core/rtw_ioctl_query.o \
		core/rtw_ioctl_set.o \
		core/rtw_ieee80211.o \
		core/rtw_mlme.o \
		core/rtw_mlme_ext.o \
		core/rtw_sec_cam.o \
		core/rtw_mi.o \
		core/rtw_wlan_util.o \
		core/rtw_vht.o \
		core/rtw_he.o \
		core/rtw_eht.o \
		core/rtw_pwrctrl.o \
		core/rtw_rf.o \
		core/rtw_chplan.o \
		core/rtw_regdb_$(CONFIG_RTW_REGDB).o \
		core/rtw_chset.o \
		core/rtw_dfs.o \
		core/rtw_txpwr.o \
		core/monitor/rtw_radiotap.o \
		core/rtw_recv.o \
		core/rtw_recv_shortcut.o \
		core/rtw_sta_mgt.o \
		core/rtw_ap.o \
		core/rtw_csa.o \
		core/wds/rtw_wds.o \
		core/mesh/rtw_mesh.o \
		core/mesh/rtw_mesh_pathtbl.o \
		core/mesh/rtw_mesh_hwmp.o \
		core/rtw_xmit.o	\
		core/rtw_xmit_shortcut.o \
		core/rtw_p2p.o \
		core/rtw_tdls.o \
		core/rtw_br_ext.o \
		core/rtw_sreset.o \
		core/rtw_rm.o \
		core/rtw_rm_fsm.o \
		core/rtw_rm_util.o \
		core/rtw_trx.o \
		core/rtw_beamforming.o \
		core/rtw_scan.o
		#core/efuse/rtw_efuse.o

_CORE_FILES +=	core/rtw_phl.o \
		core/rtw_phl_cmd.o

EXTRA_CFLAGS += -I$(src)/core/crypto
_CORE_FILES += core/crypto/aes-internal.o \
		core/crypto/aes-internal-enc.o \
		core/crypto/aes-gcm.o \
		core/crypto/aes-ccm.o \
		core/crypto/aes-omac1.o \
		core/crypto/ccmp.o \
		core/crypto/gcmp.o \
		core/crypto/aes-siv.o \
		core/crypto/aes-ctr.o \
		core/crypto/sha256-internal.o \
		core/crypto/sha256.o \
		core/crypto/sha256-prf.o \
		core/crypto/rtw_crypto_wrap.o \
		core/rtw_swcrypto.o		

ifeq ($(CONFIG_WOWLAN), y)
_CORE_FILES += core/rtw_wow.o
endif

ifeq ($(CONFIG_PCI_HCI), y)
_CORE_FILES += core/rtw_trx_pci.o
endif

ifeq ($(CONFIG_USB_HCI), y)
_CORE_FILES += core/rtw_trx_usb.o
endif

ifeq ($(CONFIG_SDIO_HCI), y)
_CORE_FILES += core/rtw_sdio.o
endif

ifeq ($(CONFIG_FPGA_INCLUDED), y)
_CORE_FILES += core/rtw_fpga.o
endif

ifeq ($(CONFIG_MP_INCLUDED), y)
_CORE_FILES += core/rtw_mp.o
endif

ifeq ($(CONFIG_WAPI_SUPPORT), y)
_CORE_FILES += core/rtw_wapi.o	\
					core/rtw_wapi_sms4.o
endif

ifeq ($(CONFIG_BTC), y)
_CORE_FILES += core/rtw_btc.o
endif

ifeq ($(CONFIG_RTW_MBO), y)
_CORE_FILES +=	core/rtw_mbo.o core/rtw_ft.o core/rtw_wnm.o
endif

OBJS += $(_OS_INTFS_FILES) $(_CORE_FILES)
