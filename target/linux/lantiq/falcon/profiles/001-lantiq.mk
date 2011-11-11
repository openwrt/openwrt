define Profile/EASY98000
  NAME:=EASY98000
  PACKAGES:= \
	kmod-dm9000 \
	kmod-i2c-core kmod-i2c-algo-bit kmod-i2c-gpio kmod-eeprom-at24 \
	kmod-spi-bitbang kmod-spi-gpio kmod-eeprom-at25
endef

define Profile/EASY98000/Description
	Lantiq EASY98000 evalkit
endef

$(eval $(call Profile,EASY98000))

define Profile/EASY98020
  NAME:=EASY98020
endef

define Profile/EASY98020/Description
	Lantiq EASY98020 evalkit
endef

$(eval $(call Profile,EASY98020))

