include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/version.mk

PKG_NAME:=ipq-wifi
PKG_RELEASE:=1
PKG_FLAGS:=nonshared

include $(INCLUDE_DIR)/package.mk

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
endef

define Build/Compile
endef

# Use ath10k-bdencoder from https://github.com/qca/qca-swiss-army-knife.git
# to generate the board-* files here.
#
# This is intended to be used on an interim basis until device-specific
# board data for new devices is available through the upstream compilation
#
# Please send a mail with your device-specific board files upstream.
# You can find instructions and examples on the linux-wireless wiki:
# <https://wireless.wiki.kernel.org/en/users/drivers/ath10k/boardfiles>

ALLWIFIBOARDS:= \
	aruba_ap-365 \
	buffalo_wxr-5950ax12 \
	devolo_magic-2-wifi-next \
	dynalink_dl-wrx36 \
	edgecore_eap102 \
	edgecore_ecw5410 \
	edgecore_oap100 \
	edimax_cax1800 \
	extreme-networks_ws-ap3915i \
	glinet_gl-a1300 \
	glinet_gl-ap1300 \
	glinet_gl-s1300 \
	linksys_ea8300 \
	linksys_whw03v2 \
	p2w_r619ac \
	pakedge_wr-1 \
	qnap_301w \
	qxwlan_e2600ac-c1 \
	qxwlan_e2600ac-c2 \
	redmi_ax6 \
	sony_ncp-hg100-cellular \
	teltonika_rutx \
	xiaomi_ax3600 \
	xiaomi_ax9000 \
	zte_mf18a \
	zte_mf289f \
	zyxel_nbg7815

ALLWIFIPACKAGES:=$(foreach BOARD,$(ALLWIFIBOARDS),ipq-wifi-$(BOARD))

define Package/ipq-wifi-default
  SUBMENU:=ath10k Board-Specific Overrides
  SECTION:=firmware
  CATEGORY:=Firmware
  DEPENDS:=@(TARGET_ipq40xx||TARGET_ipq806x||TARGET_ipq807x)
  TITLE:=Custom Board
endef

define ipq-wifi-install-one-to
  $(INSTALL_DIR)  $(2)/lib/firmware/ath10k/$(3)/
  $(INSTALL_DATA) $(1) $(2)/lib/firmware/ath10k/$(3)/board-2.bin
endef

define ipq-wifi-install-ath11-one-to
  $(INSTALL_DIR)  $(2)/lib/firmware/ath11k/$(3)/
  $(INSTALL_DATA) $(1) $(2)/lib/firmware/ath11k/$(3)/board-2.bin
endef

define ipq-wifi-install-one
  $(if $(filter $(suffix $(1)),.QCA4019 .qca4019),\
    $(call ipq-wifi-install-one-to,$(1),$(2),QCA4019/hw1.0),\
  $(if $(filter $(suffix $(1)),.QCA9888 .qca9888),\
    $(call ipq-wifi-install-one-to,$(1),$(2),QCA9888/hw2.0),\
  $(if $(filter $(suffix $(1)),.QCA9889 .qca9889),\
    $(call ipq-wifi-install-one-to,$(1),$(2),QCA9887/hw1.0),\
  $(if $(filter $(suffix $(1)),.QCA9984 .qca9984),\
    $(call ipq-wifi-install-one-to,$(1),$(2),QCA9984/hw1.0),\
  $(if $(filter $(suffix $(1)),.QCA99X0 .qca99x0),\
    $(call ipq-wifi-install-one-to,$(1),$(2),QCA99X0/hw2.0),\
  $(if $(filter $(suffix $(1)),.IPQ8074 .ipq8074),\
    $(call ipq-wifi-install-ath11-one-to,$(1),$(2),IPQ8074/hw2.0),\
  $(if $(filter $(suffix $(1)),.QCN9074 .qcn9074),\
    $(call ipq-wifi-install-ath11-one-to,$(1),$(2),QCN9074/hw1.0),\
    $(error Unrecognized board-file suffix '$(suffix $(1))' for '$(1)')\
  )))))))

endef
# Blank line required at end of above define due to foreach context

define generate-ipq-wifi-package
  define Package/ipq-wifi-$(1)
    $(call Package/ipq-wifi-default)
    TITLE:=board-2.bin Overrides for $(2)
    CONFLICTS:=$(PREV_BOARD)
  endef

  define Package/ipq-wifi-$(1)/description
The $(2) requires board-specific, reference ("cal") data
that is not yet present in the upstream wireless firmware distribution.

This package supplies board-2.bin file(s) that, in the interim,
overwrite those supplied by the ath10k-firmware-* packages.

This is package is only necessary for the $(2).

Do not install it for any other device!
  endef

  define Package/ipq-wifi-$(1)/install-overlay
    $$$$(foreach IPQ_WIFI_BOARD_FILE,$$$$(wildcard board-$(1).*),\
      $$$$(call ipq-wifi-install-one,$$$$(IPQ_WIFI_BOARD_FILE),$$(1)))
  endef

  PREV_BOARD+=ipq-wifi-$(1)
endef

# Add board name to ALLWIFIBOARDS
# Place files in this directory as board-<devicename>.<qca4019|qca9888|qca9889|qca9984|qca99x0|ipq8074>
# Add $(eval $(call generate-ipq-wifi-package,<devicename>,<display name>))

$(eval $(call generate-ipq-wifi-package,aruba_ap-365,Aruba AP-365))
$(eval $(call generate-ipq-wifi-package,buffalo_wxr-5950ax12,Buffalo WXR-5950AX12))
$(eval $(call generate-ipq-wifi-package,devolo_magic-2-wifi-next,devolo Magic 2 WiFi next))
$(eval $(call generate-ipq-wifi-package,dynalink_dl-wrx36,Dynalink DL-WRX36))
$(eval $(call generate-ipq-wifi-package,edgecore_eap102,Edgecore EAP102))
$(eval $(call generate-ipq-wifi-package,edgecore_ecw5410,Edgecore ECW5410))
$(eval $(call generate-ipq-wifi-package,edgecore_oap100,Edgecore OAP100))
$(eval $(call generate-ipq-wifi-package,edimax_cax1800,Edimax CAX1800))
$(eval $(call generate-ipq-wifi-package,extreme-networks_ws-ap3915i,Edgecore OAP100))
$(eval $(call generate-ipq-wifi-package,glinet_gl-a1300,GL.iNet GL-A1300))
$(eval $(call generate-ipq-wifi-package,glinet_gl-ap1300,GL.iNet GL-AP1300))
$(eval $(call generate-ipq-wifi-package,glinet_gl-s1300,GL.iNet GL-S1300))
$(eval $(call generate-ipq-wifi-package,linksys_ea8300,Linksys EA8300))
$(eval $(call generate-ipq-wifi-package,linksys_whw03v2,Linksys WHW03 V2))
$(eval $(call generate-ipq-wifi-package,p2w_r619ac,P&W R619AC))
$(eval $(call generate-ipq-wifi-package,pakedge_wr-1,Pakedge WR-1))
$(eval $(call generate-ipq-wifi-package,qnap_301w,QNAP 301w))
$(eval $(call generate-ipq-wifi-package,qxwlan_e2600ac-c1,Qxwlan E2600AC C1))
$(eval $(call generate-ipq-wifi-package,qxwlan_e2600ac-c2,Qxwlan E2600AC C2))
$(eval $(call generate-ipq-wifi-package,redmi_ax6,Redmi AX6))
$(eval $(call generate-ipq-wifi-package,sony_ncp-hg100-cellular,Sony NCP-HG100/Cellular))
$(eval $(call generate-ipq-wifi-package,teltonika_rutx,Teltonika RUTX))
$(eval $(call generate-ipq-wifi-package,xiaomi_ax3600,Xiaomi AX3600))
$(eval $(call generate-ipq-wifi-package,xiaomi_ax9000,Xiaomi AX9000))
$(eval $(call generate-ipq-wifi-package,zte_mf18a,ZTE MF18A))
$(eval $(call generate-ipq-wifi-package,zte_mf289f,ZTE MF289F))
$(eval $(call generate-ipq-wifi-package,zyxel_nbg7815,Zyxel NBG7815))

$(foreach PACKAGE,$(ALLWIFIPACKAGES),$(eval $(call BuildPackage,$(PACKAGE))))
