include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/version.mk

PKG_NAME:=ath12k-firmware
PKG_MAINTAINER:=Sidhant Goel <sidhant@hexcod.in>
PKG_LICENSE:=MIT
PKG_LICENSE_FILES:=LICENSE.qca_firmware
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=git@github.com:sidhantgoel/firmware_qca-wireless.git
PKG_SOURCE_DATE:=2026-03-03
PKG_SOURCE_VERSION:=c7b7a979b30e9fbaf2c41b4ad96a4c4d67846927
PKG_MIRROR_HASH:=446cd7b9892f531bc665b618f11e3abfc907271cdc407fafe794891520c0affd
PKG_FLAGS:=nonshared

include $(INCLUDE_DIR)/package.mk

define Build/Compile
endef

ALLATH12KFIRMWARES := \
	tplink_archer-be800

ALLATH12KFIRMWAREPACKAGES:=$(foreach BOARD,$(ALLATH12KFIRMWARES),ath12k-firmware-$(BOARD))

# Use ath12k-fwencoder from https://github.com/qca/qca-swiss-army-knife.git
# to generate the firmware-* files here.
#
# This is intended to be used on an interim basis until device-specific
# firmware data for new devices is available through the upstream compilation

define Package/ath12k-firmware-default
  SUBMENU:=ath12k Firmware-Specific Overrides
  SECTION:=firmware
  CATEGORY:=Firmware
  DEPENDS:=@(TARGET_qualcommbe)
  TITLE:=Custom Firmware
endef

define ath12k-install-firmware-one-to
  $(INSTALL_DIR)  $(2)/lib/firmware/ath12k/$(3)/
  $(INSTALL_DATA) $(1) $(2)/lib/firmware/ath12k/$(3)/firmware-2.bin
endef

define ath12k-install-firmware-one
  $(if $(filter $(suffix $(1)),.QCN9274 .qcn9274),\
    $(call ath12k-install-firmware-one-to,$(1),$(2),QCN9274/hw2.0),\
    $(error Unrecognized board-file suffix '$(suffix $(1))' for '$(1)')\
  )

endef
# Blank line required at end of above define due to foreach context

define generate-ath12k-firmware-package
  define Package/ath12k-firmware-$(1)
    $(call Package/ath12k-firmware-default)
    TITLE:=firmware-2.bin Overrides for $(2)
    CONFLICTS:=$(PREV_FIRMWARE) ath12k-firmware-qcn9274
  endef

  define Package/ath12k-firmware-$(1)/description
The $(2) requires board-specific, reference ("cal") data
that is not yet present in the upstream wireless firmware distribution.

This package supplies firmware-2.bin file(s) that, in the interim,
overwrite those supplied by the ath12k-firmware-* packages.

This is package is only necessary for the $(2).

Do not install it for any other device!
  endef

  define Package/ath12k-firmware-$(1)/install-overlay
    $$$$(foreach ATH12K_FIRMWARE_FILE,$$$$(wildcard $(PKG_BUILD_DIR)/firmware-$(1).*),\
      $$$$(call ath12k-install-firmware-one,$$$$(ATH12K_FIRMWARE_FILE),$$(1)))
  endef

  PREV_FIRMWARE+=ath12k-firmware-$(1)
endef

# To add local files for testing:
# 1. create a new directory in the ath12k-firmware package directory called "files".
# 2. place there the needed firmware files.
#
# To send a firmware file upstream:
# 1. create a pull request in https://github.com/openwrt/firmware_qca-wireless or
#    send a patch to the mailing list adding the tag firmware/qca-wireless before
#     the PATCH tag.
# 2. create a pull request in the main openwrt repository bumping the ath12k-firmware
#    package to the new version and adding the required lines to add the board file.
#
# Changes needed to the ath12k-firmware Makefile:
# 1. Add firmware name to ALLATH12KFIRMWARES
# 2. Add $(eval $(call generate-ath12k-firmware-package,<devicename>,<display name>))
#
# Firmware files should follow this name structure:
#   firmware-<devicename>.<qca4019|qca9888|qca9889|qca9984|qca99x0|ipq6018|ipq8074>

$(eval $(call generate-ath12k-firmware-package,tplink_archer-be800,TP-Link Archer BE800))

$(foreach PACKAGE,$(ALLATH12KFIRMWAREPACKAGES),$(eval $(call BuildPackage,$(PACKAGE))))
