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

#ALLWIFIBOARDS:=<devicename>
# Please send a mail with your device-specific board files upstream.
# You can find instructions and examples on the linux-wireless wiki:
# <https://wireless.wiki.kernel.org/en/users/drivers/ath10k/boardfiles>
ALLWIFIBOARDS:=alfa-network_ap120c-ac asus_map-ac2200 avm_fritzbox-7530 avm_fritzrepeater-3000 engenius_eap1300 engenius_ens620ext linksys_ea6350v3 qxwlan_e2600ac
ALLWIFIPACKAGES:=$(foreach BOARD,$(ALLWIFIBOARDS),ipq-wifi-$(BOARD))

define Package/ipq-wifi-default
  SUBMENU:=ath10k IPQ4019 Boarddata
  SECTION:=firmware
  CATEGORY:=Firmware
  DEPENDS:=@TARGET_ipq40xx
  TITLE:=Custom Board
endef

define generate-ipq-wifi-package
  define Package/ipq-wifi-$(1)
    $(call Package/ipq-wifi-default)
    TITLE:=Board for $(3)
    CONFLICTS:=$(PREV_BOARD)
  endef

  define Package/ipq-wifi-$(1)/description
This device custom package board-2.bin overwrites the board-2.bin
file which is supplied by the ath10k-firmware-qca4019 package.

This is package is only necessary for the $(3).
Don't install it for any other device!
  endef

  define Package/ipq-wifi-$(1)/install-overlay
	$(INSTALL_DIR) $$(1)/lib/firmware/ath10k/QCA4019/hw1.0
	$(INSTALL_DATA) ./$(2) $$(1)/lib/firmware/ath10k/QCA4019/hw1.0/board-2.bin
  endef

  PREV_BOARD+=ipq-wifi-$(1)
endef

#$(eval $(call generate-ipq-wifi-package,<devicename>,<filename>,<displayname>))
$(eval $(call generate-ipq-wifi-package,alfa-network_ap120c-ac,board-alfa-network_ap120c-ac.bin,ALFA Network AP120C-AC))
$(eval $(call generate-ipq-wifi-package,asus_map-ac2200,board-map-ac2200.bin,ASUS MAP-AC2200))
$(eval $(call generate-ipq-wifi-package,engenius_eap1300,board-engenius_eap1300.bin,EnGenius EAP1300))
$(eval $(call generate-ipq-wifi-package,engenius_ens620ext,board-engenius_ens620ext.bin,EnGenius ENS620EXT))
$(eval $(call generate-ipq-wifi-package,avm_fritzbox-7530,board-avm_fritzbox-7530.bin,AVM FRITZ!Box 7530))
$(eval $(call generate-ipq-wifi-package,avm_fritzrepeater-3000,board-avm_fritzrepeater-3000.bin,AVM FRITZ!Repeater 3000))
$(eval $(call generate-ipq-wifi-package,linksys_ea6350v3,board-linksys_ea6350v3.bin,Linksys EA6350v3))
$(eval $(call generate-ipq-wifi-package,qxwlan_e2600ac,board-qxwlan-e2600ac.bin,Qxwlan E2600AC))

$(foreach PACKAGE,$(ALLWIFIPACKAGES),$(eval $(call BuildPackage,$(PACKAGE))))
