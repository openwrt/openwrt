
define Profile/marduk
    NAME:=Basic platform profile for Marduk
    PACKAGES:=kmod-i2c wpan-tools tcpdump uhttpd
endef

define Profile/marduk/Description
        Package set for basic platform support profile for Marduk
        board
endef

$(eval $(call Profile,marduk))
