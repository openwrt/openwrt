#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define KernelPackage/ata-ledtrig
  TITLE:=LED trigger for ATA port
  DEPENDS:=@TARGET_kirkwood
  KCONFIG:=CONFIG_ATA_LEDS=y
  $(call AddDepends/ata)
endef

define KernelPackage/ata-ledtrig/description
  Dummy package which adds LED trigger for each registered ATA port.
endef

$(eval $(call KernelPackage,ata-ledtrig))
