# All needed files would be added to _HAL_INTFS_FILES, and it would include
# hal/hal_halmac.c and all related files in directory hal/halmac/.
# Before include this makefile, be sure interface (CONFIG_*_HCI) and IC
# (CONFIG_RTL*) setting are all ready!

HAL = hal_g6

ifeq ($(CONFIG_PHL_ARCH), y)
phl_path := phl/hal_g6
phl_path_d1 := $(src)/phl/$(HAL)
else
phl_path := hal_g6
phl_path_d1 := $(src)/$(HAL)
endif

# Base directory
path_hm := $(phl_path)/mac
# Level 1 directory
path_hm_d1 := $(path_hm)/mac_ax
path_fw_d1 := $(path_hm)/fw_ax

ifeq ($(CONFIG_PCI_HCI), y)
pci := y
endif
ifeq ($(CONFIG_SDIO_HCI), y)
sdio := y
endif
ifeq ($(CONFIG_USB_HCI), y)
usb := y
endif

halmac-y +=		$(path_hm)/mac_ax.o

# Modify level 1 directory if needed
#			$(path_hm_d1)/fwdl.o

halmac-y +=		$(path_hm_d1)/addr_cam.o \
			$(path_hm_d1)/beacon.o \
			$(path_hm_d1)/cmac_tx.o \
			$(path_hm_d1)/coex.o \
			$(path_hm_d1)/cpuio.o \
			$(path_hm_d1)/dbcc.o \
			$(path_hm_d1)/dbgpkg.o \
			$(path_hm_d1)/dbgport_hw.o \
			$(path_hm_d1)/dbg_cmd.o \
			$(path_hm_d1)/dle.o \
			$(path_hm_d1)/efuse.o \
			$(path_hm_d1)/fwcmd.o \
			$(path_hm_d1)/fwdl.o \
			$(path_hm_d1)/fwofld.o \
			$(path_hm_d1)/gpio.o \
			$(path_hm_d1)/hci_fc.o \
			$(path_hm_d1)/hdr_conv.o \
			$(path_hm_d1)/hw_seq.o \
			$(path_hm_d1)/h2c_agg.o \
			$(path_hm_d1)/hw.o \
			$(path_hm_d1)/hwamsdu.o \
			$(path_hm_d1)/init.o \
			$(path_hm_d1)/la_mode.o \
			$(path_hm_d1)/mcc.o \
			$(path_hm_d1)/mport.o \
			$(path_hm_d1)/phy_rpt.o \
			$(path_hm_d1)/power_saving.o \
			$(path_hm_d1)/pwr.o \
			$(path_hm_d1)/p2p.o \
			$(path_hm_d1)/role.o \
			$(path_hm_d1)/rx_filter.o \
			$(path_hm_d1)/rx_forwarding.o \
			$(path_hm_d1)/ser.o \
			$(path_hm_d1)/security_cam.o \
			$(path_hm_d1)/sounding.o \
			$(path_hm_d1)/status.o \
			$(path_hm_d1)/sta_sch.o \
			$(path_hm_d1)/tblupd.o \
			$(path_hm_d1)/tcpip_checksum_offload.o \
			$(path_hm_d1)/trx_desc.o \
			$(path_hm_d1)/trxcfg.o \
			$(path_hm_d1)/twt.o \
			$(path_hm_d1)/wowlan.o \
			$(path_hm_d1)/flash.o \
			$(path_hm_d1)/spatial_reuse.o \
			$(path_hm_d1)/pwr_seq_func.o \
			$(path_hm_d1)/phy_misc.o \
			$(path_hm_d1)/mac_priv.o \
			$(path_hm_d1)/secure_boot.o \
			$(path_hm_d1)/eco_patch_check.o \
			$(path_hm_d1)/common.o \
			$(path_hm_d1)/nan.o \
			$(path_hm_d1)/otpkeysinfo.o \

halmac-$(pci) += 	$(path_hm_d1)/_pcie.o
halmac-$(usb) += 	$(path_hm_d1)/_usb.o
halmac-$(sdio) +=	$(path_hm_d1)/_sdio.o

ifeq ($(CONFIG_RTL8852A), y)
mac_ic := 8852a
# Level 2 directory
path_hm2 := $(path_hm_d1)/mac_$(mac_ic)

halmac-y	+=	$(path_hm2)/cmac_tx_$(mac_ic).o \
				$(path_hm2)/fwcmd_$(mac_ic).o \
				$(path_hm2)/gpio_$(mac_ic).o \
				$(path_hm2)/init_$(mac_ic).o \
				$(path_hm2)/mac_priv_$(mac_ic).o \
				$(path_hm2)/pwr_seq_$(mac_ic).o \
				$(path_hm2)/rrsr_$(mac_ic).o \
				$(path_hm2)/trx_desc_$(mac_ic).o \
				$(path_hm2)/tblupd_$(mac_ic).o \
				$(path_hm2)/phy_rpt_$(mac_ic).o \
				$(path_hm2)/coex_$(mac_ic).o \
				$(path_hm2)/dbgpkg_$(mac_ic).o \
				$(path_hm2)/efuse_$(mac_ic).o \
				$(path_hm2)/hwamsdu_$(mac_ic).o \
				$(path_hm2)/hdr_conv_rx_$(mac_ic).o \
				$(path_hm2)/hci_fc_$(mac_ic).o \
				$(path_hm2)/dle_$(mac_ic).o \

halmac-$(usb) += 	$(path_hm2)/_usb_$(mac_ic).o
halmac-$(pci) += 	$(path_hm2)/_pcie_$(mac_ic).o
halmac-$(sdio) += 	$(path_hm2)/_sdio_$(mac_ic).o
# fw files
path_fw := $(path_fw_d1)/rtl$(mac_ic)

halmac-y	+=	$(path_fw)/hal$(mac_ic)_fw.o \
				$(path_fw)/hal$(mac_ic)_fw_log.o
endif

# 8852B/8852BP Support
ifneq ($(filter y,$(CONFIG_RTL8852B) $(CONFIG_RTL8852BP)),)
mac_ic := 8852b
# Level 2 directory
path_hm2 := $(path_hm_d1)/mac_$(mac_ic)

halmac-y	+=	$(path_hm2)/cmac_tx_$(mac_ic).o \
				$(path_hm2)/fwcmd_$(mac_ic).o \
				$(path_hm2)/gpio_$(mac_ic).o \
				$(path_hm2)/init_$(mac_ic).o \
				$(path_hm2)/mac_priv_$(mac_ic).o \
				$(path_hm2)/pwr_seq_$(mac_ic).o \
				$(path_hm2)/pwr_seq_func_$(mac_ic).o \
				$(path_hm2)/trx_desc_$(mac_ic).o \
				$(path_hm2)/tblupd_$(mac_ic).o \
				$(path_hm2)/rrsr_$(mac_ic).o \
				$(path_hm2)/coex_$(mac_ic).o \
				$(path_hm2)/phy_rpt_$(mac_ic).o \
				$(path_hm2)/dbgpkg_$(mac_ic).o \
				$(path_hm2)/efuse_$(mac_ic).o \
				$(path_hm2)/hwamsdu_$(mac_ic).o \
				$(path_hm2)/hdr_conv_rx_$(mac_ic).o \
				$(path_hm2)/hci_fc_$(mac_ic).o \
				$(path_hm2)/dle_$(mac_ic).o \

halmac-$(usb) += 	$(path_hm2)/_usb_$(mac_ic).o
halmac-$(pci) += 	$(path_hm2)/_pcie_$(mac_ic).o
halmac-$(sdio) += 	$(path_hm2)/_sdio_$(mac_ic).o
# fw files
path_fw := $(path_fw_d1)/rtl$(mac_ic)

halmac-y	+=	$(path_fw)/hal$(mac_ic)_fw.o \
				$(path_fw)/hal$(mac_ic)_fw_log.o
endif

ifeq ($(CONFIG_RTL8852C), y)
mac_ic := 8852c
# Level 2 directory
path_hm2 := $(path_hm_d1)/mac_$(mac_ic)

halmac-y	+=	$(path_hm2)/cmac_tx_$(mac_ic).o \
				$(path_hm2)/fwcmd_$(mac_ic).o \
				$(path_hm2)/gpio_$(mac_ic).o \
				$(path_hm2)/init_$(mac_ic).o \
				$(path_hm2)/mac_priv_$(mac_ic).o \
				$(path_hm2)/pwr_seq_$(mac_ic).o \
				$(path_hm2)/pwr_seq_func_$(mac_ic).o \
				$(path_hm2)/rrsr_$(mac_ic).o \
				$(path_hm2)/ser_$(mac_ic).o \
				$(path_hm2)/security_cam_$(mac_ic).o \
				$(path_hm2)/trx_desc_$(mac_ic).o \
				$(path_hm2)/tblupd_$(mac_ic).o \
				$(path_hm2)/coex_$(mac_ic).o \
				$(path_hm2)/phy_rpt_$(mac_ic).o \
				$(path_hm2)/dbgpkg_$(mac_ic).o \
				$(path_hm2)/efuse_$(mac_ic).o \
				$(path_hm2)/hwamsdu_$(mac_ic).o \
				$(path_hm2)/hdr_conv_rx_$(mac_ic).o \
				$(path_hm2)/hci_fc_$(mac_ic).o \
				$(path_hm2)/dle_$(mac_ic).o \

halmac-$(usb) += 	$(path_hm2)/_usb_$(mac_ic).o
halmac-$(pci) += 	$(path_hm2)/_pcie_$(mac_ic).o
halmac-$(sdio) += 	$(path_hm2)/_sdio_$(mac_ic).o
# fw files
path_fw := $(path_fw_d1)/rtl$(mac_ic)

halmac-y	+=	$(path_fw)/hal$(mac_ic)_fw.o \
				$(path_fw)/hal$(mac_ic)_fw_log.o
endif

# 8832BR/8192XB Support
ifneq ($(filter y,$(CONFIG_RTL8832BR) $(CONFIG_RTL8192XB)),)
mac_ic_92xb := 8192xb
# Level 2 directory
path_hm2_92xb := $(path_hm_d1)/mac_$(mac_ic_92xb)

halmac-y	+=	$(path_hm2_92xb)/pwr_seq_func_$(mac_ic_92xb).o \
			$(path_hm2_92xb)/trx_desc_$(mac_ic_92xb).o \
			$(path_hm2_92xb)/security_cam_$(mac_ic_92xb).o \
			$(path_hm2_92xb)/ser_$(mac_ic_92xb).o \
			$(path_hm2_92xb)/tblupd_$(mac_ic_92xb).o \
			$(path_hm2_92xb)/gpio_$(mac_ic_92xb).o \
			$(path_hm2_92xb)/init_$(mac_ic_92xb).o \
			$(path_hm2_92xb)/pwr_seq_$(mac_ic_92xb).o \
			$(path_hm2_92xb)/rrsr_$(mac_ic_92xb).o \
			$(path_hm2_92xb)/mac_priv_$(mac_ic_92xb).o \
			$(path_hm2_92xb)/cmac_tx_$(mac_ic_92xb).o \
			$(path_hm2_92xb)/fwcmd_$(mac_ic_92xb).o \
			$(path_hm2_92xb)/coex_$(mac_ic_92xb).o \
			$(path_hm2_92xb)/phy_rpt_$(mac_ic_92xb).o \
			$(path_hm2_92xb)/dbgpkg_$(mac_ic_92xb).o \
			$(path_hm2_92xb)/efuse_$(mac_ic_92xb).o \
			$(path_hm2_92xb)/hwamsdu_$(mac_ic_92xb).o \
			$(path_hm2_92xb)/hdr_conv_rx_$(mac_ic_92xb).o \
			$(path_hm2_92xb)/hci_fc_$(mac_ic_92xb).o \
			$(path_hm2_92xb)/dle_$(mac_ic_92xb).o \

halmac-$(usb) += 	$(path_hm2_92xb)/_usb_$(mac_ic_92xb).o
halmac-$(pci) += 	$(path_hm2_92xb)/_pcie_$(mac_ic_92xb).o
halmac-$(sdio) += 	$(path_hm2_92xb)/_sdio_$(mac_ic_92xb).o
# fw files
path_fw_92xb := $(path_fw_d1)/rtl$(mac_ic_92xb)

halmac-y	+=	$(path_fw_92xb)/hal$(mac_ic_92xb)_fw.o \
				$(path_fw_92xb)/hal$(mac_ic_92xb)_fw_log.o
endif

_HAL_MAC_FILES +=	$(halmac-y)

# 8851b Support
ifeq ($(CONFIG_RTL8851B), y)
mac_ic := 8851b
# Level 2 directory
path_hm2 := $(path_hm_d1)/mac_$(mac_ic)

halmac-y	+=	$(path_hm2)/cmac_tx_$(mac_ic).o \
				$(path_hm2)/fwcmd_$(mac_ic).o \
				$(path_hm2)/gpio_$(mac_ic).o \
				$(path_hm2)/init_$(mac_ic).o \
				$(path_hm2)/mac_priv_$(mac_ic).o \
				$(path_hm2)/pwr_seq_$(mac_ic).o \
				$(path_hm2)/pwr_seq_func_$(mac_ic).o \
				$(path_hm2)/trx_desc_$(mac_ic).o \
				$(path_hm2)/tblupd_$(mac_ic).o \
				$(path_hm2)/rrsr_$(mac_ic).o \
				$(path_hm2)/coex_$(mac_ic).o \
				$(path_hm2)/phy_rpt_$(mac_ic).o \
				$(path_hm2)/dbgpkg_$(mac_ic).o \
				$(path_hm2)/efuse_$(mac_ic).o \
				$(path_hm2)/hwamsdu_$(mac_ic).o \
				$(path_hm2)/hdr_conv_rx_$(mac_ic).o \
				$(path_hm2)/hci_fc_$(mac_ic).o \
				$(path_hm2)/dle_$(mac_ic).o \

halmac-$(usb) += 	$(path_hm2)/_usb_$(mac_ic).o
halmac-$(pci) += 	$(path_hm2)/_pcie_$(mac_ic).o
halmac-$(sdio) += 	$(path_hm2)/_sdio_$(mac_ic).o
# fw files
path_fw := $(path_fw_d1)/rtl$(mac_ic)

halmac-y	+=	$(path_fw)/hal$(mac_ic)_fw.o \
				$(path_fw)/hal$(mac_ic)_fw_log.o
endif

ifeq ($(CONFIG_RTL8851E), y)
mac_ic := 8851e
# Level 2 directory
path_hm2 := $(path_hm_d1)/mac_$(mac_ic)

halmac-y	+=	$(path_hm2)/cmac_tx_$(mac_ic).o \
				$(path_hm2)/fwcmd_$(mac_ic).o \
				$(path_hm2)/gpio_$(mac_ic).o \
				$(path_hm2)/init_$(mac_ic).o \
				$(path_hm2)/mac_priv_$(mac_ic).o \
				$(path_hm2)/pwr_seq_$(mac_ic).o \
				$(path_hm2)/pwr_seq_func_$(mac_ic).o \
				$(path_hm2)/rrsr_$(mac_ic).o \
				$(path_hm2)/ser_$(mac_ic).o \
				$(path_hm2)/security_cam_$(mac_ic).o \
				$(path_hm2)/trx_desc_$(mac_ic).o \
				$(path_hm2)/tblupd_$(mac_ic).o \
				$(path_hm2)/coex_$(mac_ic).o \
				$(path_hm2)/phy_rpt_$(mac_ic).o \
				$(path_hm2)/dbgpkg_$(mac_ic).o \
				$(path_hm2)/efuse_$(mac_ic).o \
				$(path_hm2)/hwamsdu_$(mac_ic).o \
				$(path_hm2)/hdr_conv_rx_$(mac_ic).o \
				$(path_hm2)/hci_fc_$(mac_ic).o \
				$(path_hm2)/dle_$(mac_ic).o \

halmac-$(usb) += 	$(path_hm2)/_usb_$(mac_ic).o
halmac-$(pci) += 	$(path_hm2)/_pcie_$(mac_ic).o
halmac-$(sdio) += 	$(path_hm2)/_sdio_$(mac_ic).o
# fw files
path_fw := $(path_fw_d1)/rtl$(mac_ic)

halmac-y	+=	$(path_fw)/hal$(mac_ic)_fw.o \
				$(path_fw)/hal$(mac_ic)_fw_log.o
endif

ifeq ($(CONFIG_RTL8852D), y)
mac_ic := 8852d
# Level 2 directory
path_hm2 := $(path_hm_d1)/mac_$(mac_ic)

halmac-y	+=	$(path_hm2)/cmac_tx_$(mac_ic).o \
				$(path_hm2)/fwcmd_$(mac_ic).o \
				$(path_hm2)/gpio_$(mac_ic).o \
				$(path_hm2)/init_$(mac_ic).o \
				$(path_hm2)/mac_priv_$(mac_ic).o \
				$(path_hm2)/pwr_seq_$(mac_ic).o \
				$(path_hm2)/pwr_seq_func_$(mac_ic).o \
				$(path_hm2)/rrsr_$(mac_ic).o \
				$(path_hm2)/ser_$(mac_ic).o \
				$(path_hm2)/security_cam_$(mac_ic).o \
				$(path_hm2)/trx_desc_$(mac_ic).o \
				$(path_hm2)/tblupd_$(mac_ic).o \
				$(path_hm2)/coex_$(mac_ic).o \
				$(path_hm2)/phy_rpt_$(mac_ic).o \
				$(path_hm2)/dbgpkg_$(mac_ic).o \
				$(path_hm2)/efuse_$(mac_ic).o \
				$(path_hm2)/hwamsdu_$(mac_ic).o \
				$(path_hm2)/hdr_conv_rx_$(mac_ic).o \
				$(path_hm2)/hci_fc_$(mac_ic).o \
				$(path_hm2)/dle_$(mac_ic).o \

halmac-$(usb) += 	$(path_hm2)/_usb_$(mac_ic).o
halmac-$(pci) += 	$(path_hm2)/_pcie_$(mac_ic).o
halmac-$(sdio) += 	$(path_hm2)/_sdio_$(mac_ic).o
# fw files
path_fw := $(path_fw_d1)/rtl$(mac_ic)

halmac-y	+=	$(path_fw)/hal$(mac_ic)_fw.o \
				$(path_fw)/hal$(mac_ic)_fw_log.o
endif