# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.

define Profile/SOM9331
   NAME:=OpenEmbed SOM9331
   PACKAGES:=kmod-usb-core kmod-usb2 kmod-usb-storage kmod-i2c-core kmod-i2c-gpio-custom kmod-spi-bitbang kmod-spi-dev kmod-spi-gpio kmod-spi-gpio-custom kmod-usb-serial
endef

define Profile/SOM9331/Description
   Package set optimized for the OpenEmbed SOM9331.
endef
$(eval $(call Profile,SOM9331))

