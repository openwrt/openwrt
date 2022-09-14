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
	devolo_magic-2-wifi-next \
	edgecore_ecw5410 \
	edgecore_oap100 \
	extreme-networks_ws-ap3915i \
	glinet_gl-ap1300 \
	glinet_gl-s1300 \
	linksys_ea8300 \
	p2w_r619ac \
	pakedge_wr-1 \
	qxwlan_e2600ac-c1 \
	qxwlan_e2600ac-c2 \
	sony_ncp-hg100-cellular \
	teltonika_rutx \
	zte_mf289f

ALLWIFIPACKAGES:=$(foreach BOARD,$(ALLWIFIBOARDS),ipq-wifi-$(BOARD))

define Package/ipq-wifi-default
  SUBMENU:=ath10k Board-Specific Overrides
  SECTION:=firmware
  CATEGORY:=Firmware
  DEPENDS:=@(TARGET_ipq40xx||TARGET_ipq806x)
  TITLE:=Custom Board
endef

define ipq-wifi-install-one-to
  $(INSTALL_DIR)  $(2)/lib/firmware/ath10k/$(3)/
  $(INSTALL_DATA) $(1) $(2)/lib/firmware/ath10k/$(3)/board-2.bin
endef

define ipq-wifi-install-one
  $(if $(filter $(suffix $(1)),.QCA4019 .qca4019),\
    $(call ipq-wifi-install-one-to,$(1),$(2),QCA4019/hw1.0),\
  $(if $(filter $(suffix $(1)),.QCA9888 .qca9888),\
    $(call ipq-wifi-install-one-to,$(1),$(2),QCA9888/hw2.0),\
  $(if $(filter $(suffix $(1)),.QCA9984 .qca9984),\
    $(call ipq-wifi-install-one-to,$(1),$(2),QCA9984/hw1.0),\
    $(error Unrecognized board-file suffix '$(suffix $(1))' for '$(1)')\
  )))

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
# Place files in this directory as board-<devicename>.<qca4019|qca9888|qca9984>
# Add $(eval $(call generate-ipq-wifi-package,<devicename>,<display name>))

$(eval $(call generate-ipq-wifi-package,aruba_ap-365,Aruba AP-365))
$(eval $(call generate-ipq-wifi-package,devolo_magic-2-wifi-next,devolo Magic 2 WiFi next))
$(eval $(call generate-ipq-wifi-package,edgecore_ecw5410,Edgecore ECW5410))
$(eval $(call generate-ipq-wifi-package,edgecore_oap100,Edgecore OAP100))
$(eval $(call generate-ipq-wifi-package,extreme-networks_ws-ap3915i,Edgecore OAP100))
$(eval $(call generate-ipq-wifi-package,glinet_gl-ap1300,GL.iNet GL-AP1300))
$(eval $(call generate-ipq-wifi-package,glinet_gl-s1300,GL.iNet GL-S1300))
$(eval $(call generate-ipq-wifi-package,linksys_ea8300,Linksys EA8300))
$(eval $(call generate-ipq-wifi-package,p2w_r619ac,P&W R619AC))
$(eval $(call generate-ipq-wifi-package,pakedge_wr-1,Pakedge WR-1))
$(eval $(call generate-ipq-wifi-package,qxwlan_e2600ac-c1,Qxwlan E2600AC C1))
$(eval $(call generate-ipq-wifi-package,qxwlan_e2600ac-c2,Qxwlan E2600AC C2))
$(eval $(call generate-ipq-wifi-package,sony_ncp-hg100-cellular,Sony NCP-HG100/Cellular))
$(eval $(call generate-ipq-wifi-package,teltonika_rutx,Teltonika RUTX))
$(eval $(call generate-ipq-wifi-package,zte_mf289f,ZTE MF289F))

$(foreach PACKAGE,$(ALLWIFIPACKAGES),$(eval $(call BuildPackage,$(PACKAGE))))
