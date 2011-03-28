BOARDNAME:=Traverse Geos
FEATURES:=squashfs jffs2 ext4 pci usb gpio
ifeq ($(strip $(call CompareKernelPatchVer,$(KERNEL_PATCHVER),ge,2.6.38)),1)
  GEOS_GPIO:=gpio-cs5535-new
else
  GEOS_GPIO:=gpio-cs5535
endif
DEFAULT_PACKAGES += kmod-ata-via-sata \
            kmod-crypto-hw-geode kmod-crypto-ocf \
            kmod-$(GEOS_GPIO) kmod-gpio-nsc \
            kmod-wdt-geode kmod-hwmon-core kmod-hwmon-lm90 \
            kmod-8139cp kmod-solos-pci \
            kmod-i2c-core kmod-i2c-gpio \
            kmod-i2c-algo-bit kmod-i2c-algo-pca kmod-i2c-algo-pcf \
            kmod-usb-core kmod-usb2 kmod-usb-uhci \
            kmod-cfg80211 kmod-mac80211 \
            kmod-mppe kmod-pppoe kmod-pppoa kmod-pppol2tp \
            kmod-ath kmod-ath5k kmod-ath9k \
            soloscli linux-atm br2684ctl bridge ppp ppp-mod-pppoa \
            libopenssl ocf-crypto-headers zlib hwclock hostapd

CS5535_MASK:=0x0E000048

CFLAGS += -Os -pipe -march=k6-2 -fno-align-functions -fno-align-loops -fno-align-jumps \
      -fno-align-labels

define Target/Description
    Build firmware images for Traverse Geos board
endef

define KernelPackage/$(GEOS_GPIO)/install
     sed -i -r -e 's/$$$$$$$$/ mask=$(CS5535_MASK)/' $(1)/etc/modules.d/??-$(GEOS_GPIO)
endef
