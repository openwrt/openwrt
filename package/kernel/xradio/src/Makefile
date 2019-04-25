# Standalone Makefile - uncomment for out-of-tree compilation
CONFIG_WLAN_VENDOR_XRADIO := m
CONFIG_XRADIO_USE_EXTENSIONS := y
#CONFIG_XRADIO_WAPI_SUPPORT := n

# Kernel part

obj-$(CONFIG_WLAN_VENDOR_XRADIO) += xradio_wlan.o

xradio_wlan-objs := \
	fwio.o \
	tx.o \
	rx.o \
	main.o \
	queue.o \
	hwio.o \
	bh.o \
	wsm.o \
	sta.o \
	ap.o \
	keys.o \
	scan.o \
	module.o \
	sdio.o \
	pm.o \
	ht.o \
	p2p.o

ccflags-$(CONFIG_WLAN_VENDOR_XRADIO) += -DMCAST_FWDING
ccflags-$(CONFIG_WLAN_VENDOR_XRADIO) += -DXRADIO_SUSPEND_RESUME_FILTER_ENABLE
ccflags-$(CONFIG_WLAN_VENDOR_XRADIO) += -DAP_AGGREGATE_FW_FIX
ccflags-$(CONFIG_WLAN_VENDOR_XRADIO) += -DAP_HT_CAP_UPDATE
ccflags-$(CONFIG_WLAN_VENDOR_XRADIO) += -DAP_HT_COMPAT_FIX
ccflags-$(CONFIG_WLAN_VENDOR_XRADIO) += -DCONFIG_XRADIO_DUMP_ON_ERROR

ccflags-$(CONFIG_WLAN_VENDOR_XRADIO) += -DCONFIG_XRADIO_SUSPEND_POWER_OFF

# Extra IE for probe response from upper layer is needed in P2P GO
# For offloading probe response to FW, the extra IE must be included
# in the probe response template
ccflags-$(CONFIG_WLAN_VENDOR_XRADIO) += -DPROBE_RESP_EXTRA_IE

# Modified by wzw
ccflags-$(CONFIG_WLAN_VENDOR_XRADIO) += -DTES_P2P_0002_ROC_RESTART
ccflags-$(CONFIG_WLAN_VENDOR_XRADIO) += -DTES_P2P_000B_EXTEND_INACTIVITY_CNT
ccflags-$(CONFIG_WLAN_VENDOR_XRADIO) += -DTES_P2P_000B_DISABLE_EAPOL_FILTER
ccflags-$(CONFIG_WLAN_VENDOR_XRADIO) += -DXRADIO_USE_LONG_DTIM_PERIOD
ccflags-$(CONFIG_WLAN_VENDOR_XRADIO) += -DXRADIO_USE_LONG_KEEP_ALIVE_PERIOD

#ccflags-$(CONFIG_WLAN_VENDOR_XRADIO) += -DDEBUG
#ccflags-$(CONFIG_WLAN_VENDOR_XRADIO) += -DXRADIO_DISABLE_HW_CRYPTO

ldflags-$(CONFIG_WLAN_VENDOR_XRADIO) += --strip-debug

