define Profile/EASY98000NOR
  NAME:=Lantiq Falcon - EASY98000NOR
  PACKAGES:= \
	kmod-dm9000 \
	kmod-i2c-core kmod-i2c-algo-bit kmod-i2c-gpio kmod-eeprom-at24 \
	kmod-spi-bitbang kmod-spi-gpio kmod-eeprom-at25
endef

$(eval $(call Profile,EASY98000NOR))

define Profile/EASY98000SFLASH
  NAME:=Lantiq Falcon - EASY98000SFLASH
  PACKAGES:= \
	kmod-dm9000 \
	kmod-i2c-core kmod-i2c-algo-bit kmod-i2c-gpio kmod-eeprom-at24 \
	kmod-spi-bitbang kmod-spi-gpio kmod-eeprom-at25
endef

$(eval $(call Profile,EASY98000SFLASH))
