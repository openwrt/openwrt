include $(TOPDIR)/rules.mk

PKG_NAME:=ath10k-ct-firmware
PKG_VERSION:=2020-03-25
PKG_RELEASE:=2

include $(INCLUDE_DIR)/package.mk

ATH10K_FIRMWARE_REV:=d622d160e9f552ead68d9ae81b715422892dc2ef
ATH10K_FIRMWARE_URL:=@GITHUB/kvalo/ath10k-firmware/$(ATH10K_FIRMWARE_REV)

QCA9887_BOARD_FILE:=ath10k-firmware-$(ATH10K_FIRMWARE_REV)-qca9887-board.bin
define Download/qca9887-board
  FILE:=$(QCA9887_BOARD_FILE)
  URL:=$(ATH10K_FIRMWARE_URL)/QCA9887/hw1.0
  URL_FILE:=board.bin
  HASH:=cf4df099f6ee05c181f55ce17297a1d32c61d725eb96246fd315ad5587c42426
endef
$(eval $(call Download,qca9887-board))

QCA988X_BOARD_FILE:=ath10k-firmware-$(ATH10K_FIRMWARE_REV)-qca988x-board.bin
define Download/qca988x-board
  FILE:=$(QCA988X_BOARD_FILE)
  URL:=$(ATH10K_FIRMWARE_URL)/QCA988X/hw2.0
  URL_FILE:=board.bin
  HASH:=5b5b380333c2dd3b6ce67f30e2f7008f4020bf594970d3b464fd8d4a80fcd880
endef
$(eval $(call Download,qca988x-board))

QCA99X0_BOARD_FILE:=ath10k-firmware-$(ATH10K_FIRMWARE_REV)-qca99x0-board.bin
define Download/qca99x0-board
  FILE:=$(QCA99X0_BOARD_FILE)
  URL:=$(ATH10K_FIRMWARE_URL)/QCA99X0/hw2.0
  URL_FILE:=boardData_AR900B_CUS239_5G_v2_001.bin
  HASH:=3bf7561ee373b369025dcd366d276d038a97d3397ccae41ce841d98a58b30aff
endef
$(eval $(call Download,qca99x0-board))

QCA99X0_BOARD2_REV:=ddcec9efd245da9365c474f513a855a55f3ac7fe
QCA99X0_BOARD2_FILE:=ath10k-firmware-$(QCA99X0_BOARD2_REV)-qca99x0-board-2.bin
define Download/qca99x0-board2
  FILE:=$(QCA99X0_BOARD2_FILE)
  URL:=https://source.codeaurora.org/quic/qsdk/oss/firmware/ath10k-firmware/plain/ath10k/QCA99X0/hw2.0
  URL_FILE:=board-2.bin?id=$(QCA99X0_BOARD2_REV)
  HASH:=03711ac21e60ef59d3815e235eb721c0c22851b5410299411085aa6f2af45401
endef
$(eval $(call Download,qca99x0-board2))

QCA9984_BOARD2_FILE:=ath10k-firmware-$(ATH10K_FIRMWARE_REV)-qca9984-board-2.bin
define Download/qca9984-board2
  FILE:=$(QCA9984_BOARD2_FILE)
  URL:=$(ATH10K_FIRMWARE_URL)/QCA9984/hw1.0
  URL_FILE:=board-2.bin
  HASH:=0d6d46cf0467185e3959ce3cb69e2415be6e48ab8a4bee3eb400edbe48cb9c25
endef
$(eval $(call Download,qca9984-board2))

QCA4019_BOARD2_FILE:=ath10k-firmware-$(ATH10K_FIRMWARE_REV)-qca4019-board-2.bin
define Download/qca4019-board2
  FILE:=$(QCA4019_BOARD2_FILE)
  URL:=$(ATH10K_FIRMWARE_URL)/QCA4019/hw1.0
  URL_FILE:=board-2.bin
  HASH:=94b66aa4ddbed5110a96364d3c7b4ebcb320e3ac4e8697660b277e76077bc338
endef
$(eval $(call Download,qca4019-board2))

QCA9888_BOARD2_FILE:=ath10k-firmware-$(ATH10K_FIRMWARE_REV)-qca9888-board-2.bin
define Download/qca9888-board2
  FILE:=$(QCA9888_BOARD2_FILE)
  URL:=$(ATH10K_FIRMWARE_URL)/QCA9888/hw2.0
  URL_FILE:=board-2.bin
  HASH:=5b871bb567f64525ca45adb88063211de472015d09e0f9aa3fa61ab71c8fdfd3
endef
$(eval $(call Download,qca9888-board2))

CT_FIRMWARE_FILE = $(1)-$($(1)_FIRMWARE_FILE_CT)
CT_FIRMWARE_FILE_FULL_HTT = $(1)-$($(1)_FIRMWARE_FILE_CT_FULL_HTT)

define Download/ct-firmware
  URL:=https://www.candelatech.com/downloads/$(2)
  FILE:=$(call CT_FIRMWARE_FILE,$(1))
  URL_FILE:=$($(1)_FIRMWARE_FILE_CT)
endef

define Download/ct-firmware-full-htt
  URL:=https://www.candelatech.com/downloads/$(2)
  FILE:=$(call CT_FIRMWARE_FILE_FULL_HTT,$(1))
  URL_FILE:=$($(1)_FIRMWARE_FILE_CT_FULL_HTT)
endef

QCA988X_FIRMWARE_FILE_CT:=firmware-2-ct-full-community-22.bin.lede.017
define Download/ath10k-firmware-qca988x-ct
  $(call Download/ct-firmware,QCA988X,)
  HASH:=2f0bf766e400a4c5726e77b128eb8c141ebaa778526fe2c7c5083f3b17659dbf
endef
$(eval $(call Download,ath10k-firmware-qca988x-ct))

QCA988X_FIRMWARE_FILE_CT_FULL_HTT:=firmware-2-ct-full-htt-mgt-community-22.bin.lede.017
define Download/ath10k-firmware-qca988x-ct-full-htt
  $(call Download/ct-firmware-full-htt,QCA988X,)
  HASH:=5e4285d5c6eee159a25ca14c6ce26022c32380bd7bafaedfc0c5de1510119007
endef
$(eval $(call Download,ath10k-firmware-qca988x-ct-full-htt))


QCA9887_FIRMWARE_FILE_CT:=firmware-2-ct-full-community-22.bin.lede.017
define Download/ath10k-firmware-qca9887-ct
  $(call Download/ct-firmware,QCA9887,ath10k-9887)
  HASH:=4fa30e0e1972ca3b06225a731df0f93a1b73ac67fea5bf54bb55dea3bbc0da6a
endef
$(eval $(call Download,ath10k-firmware-qca9887-ct))

QCA9887_FIRMWARE_FILE_CT_FULL_HTT:=firmware-2-ct-full-htt-mgt-community-22.bin.lede.017
define Download/ath10k-firmware-qca9887-ct-full-htt
  $(call Download/ct-firmware-full-htt,QCA9887,ath10k-9887)
  HASH:=dc681b6b1e45956e7c2e418ab05eee5c943d13e775209196d9bd931ff6493935
endef
$(eval $(call Download,ath10k-firmware-qca9887-ct-full-htt))


QCA99X0_FIRMWARE_FILE_CT:=firmware-5-ct-full-community-12.bin-lede.017
define Download/ath10k-firmware-qca99x0-ct
  $(call Download/ct-firmware,QCA99X0,ath10k-10-4b)
  HASH:=289ea845d4bbae6f36b3af2a13a5eaa07097f52d10f7b7306cfc9e2dd394f889
endef
$(eval $(call Download,ath10k-firmware-qca99x0-ct))

QCA99X0_FIRMWARE_FILE_CT_FULL_HTT:=firmware-5-ct-full-htt-mgt-community-12.bin-lede.017
define Download/ath10k-firmware-qca99x0-ct-full-htt
  $(call Download/ct-firmware-full-htt,QCA99X0,ath10k-10-4b)
  HASH:=adedcd3d379a910bc3a5257d75f8970e11319f4cd9c1b34440d35821602a8b9c
endef
$(eval $(call Download,ath10k-firmware-qca99x0-ct-full-htt))


QCA9984_FIRMWARE_FILE_CT:=firmware-5-ct-full-community-12.bin-lede.017
define Download/ath10k-firmware-qca9984-ct
  $(call Download/ct-firmware,QCA9984,ath10k-9984-10-4b)
  HASH:=8175be5b3946bddc042be018ff7713e67b41b59374ef4cdd183185b59274c91a
endef
$(eval $(call Download,ath10k-firmware-qca9984-ct))

QCA9984_FIRMWARE_FILE_CT_FULL_HTT:=firmware-5-ct-full-htt-mgt-community-12.bin-lede.017
define Download/ath10k-firmware-qca9984-ct-full-htt
  $(call Download/ct-firmware-full-htt,QCA9984,ath10k-9984-10-4b)
  HASH:=eb8b894cfe0d1aaa87f130bb7fd1815ef07b951c14df8a2ceaeb780df8f640e0
endef
$(eval $(call Download,ath10k-firmware-qca9984-ct-full-htt))


QCA4019_FIRMWARE_FILE_CT:=firmware-5-ct-full-community-12.bin-lede.017
define Download/ath10k-firmware-qca4019-ct
  $(call Download/ct-firmware,QCA4019,ath10k-4019-10-4b)
  HASH:=29e9f662c4cd287213877abfbb90fbabb5e32dd3710d3ade82aa94a0921972ae
endef
$(eval $(call Download,ath10k-firmware-qca4019-ct))

QCA4019_FIRMWARE_FILE_CT_FULL_HTT:=firmware-5-ct-full-htt-mgt-community-12.bin-lede.017
define Download/ath10k-firmware-qca4019-ct-full-htt
  $(call Download/ct-firmware-full-htt,QCA4019,ath10k-4019-10-4b)
  HASH:=559c911f23856b1d3d864ce714d1bef7262bf6638e93e057ecf8d5dba48ca1e6
endef
$(eval $(call Download,ath10k-firmware-qca4019-ct-full-htt))


QCA9888_FIRMWARE_FILE_CT:=firmware-5-ct-full-community-12.bin-lede.017
define Download/ath10k-firmware-qca9888-ct
  $(call Download/ct-firmware,QCA9888,ath10k-9888-10-4b)
  HASH:=b295880a8b08ec2680d85daaf5f20232a0e73d9cc579bf3efd7ffae24ea340d7
endef
$(eval $(call Download,ath10k-firmware-qca9888-ct))

QCA9888_FIRMWARE_FILE_CT_FULL_HTT:=firmware-5-ct-full-htt-mgt-community-12.bin-lede.017
define Download/ath10k-firmware-qca9888-ct-full-htt
  $(call Download/ct-firmware-full-htt,QCA9888,ath10k-9888-10-4b)
  HASH:=26fe7c00df10e93373a0f9f105e85d02bb8b1cdd629183ce22a5147138336aec
endef
$(eval $(call Download,ath10k-firmware-qca9888-ct-full-htt))


define Package/ath10k-ct-firmware-default
  SECTION:=firmware
  CATEGORY:=Firmware
  URL:=https://www.candelatech.com/ath10k.php
  DEPENDS:=
endef

define Package/ath10k-firmware-qca988x-ct
$(Package/ath10k-ct-firmware-default)
  TITLE:=ath10k CT 10.1 firmware for QCA988x devices
  SECTION:=firmware
  CATEGORY:=Firmware
  PROVIDES:=ath10k-firmware-qca988x
endef
define Package/ath10k-firmware-qca988x-ct-full-htt
$(Package/ath10k-ct-firmware-default)
  TITLE:=ath10k CT 10.1 full-htt-mgt fw for QCA988x
  SECTION:=firmware
  CATEGORY:=Firmware
  PROVIDES:=ath10k-firmware-qca988x
  DEPENDS:=+!PACKAGE_kmod-ath10k-ct-smallbuffers:kmod-ath10k-ct
endef

define Package/ath10k-firmware-qca9887-ct
$(Package/ath10k-ct-firmware-default)
  TITLE:=ath10k CT 10.1 firmware for QCA9887 devices
  SECTION:=firmware
  CATEGORY:=Firmware
  PROVIDES:=ath10k-firmware-qca9887
endef
define Package/ath10k-firmware-qca9887-ct-full-htt
$(Package/ath10k-ct-firmware-default)
  TITLE:=ath10k CT 10.1 full-htt-mgt fw for QCA9887
  SECTION:=firmware
  CATEGORY:=Firmware
  PROVIDES:=ath10k-firmware-qca9887
  DEPENDS:=+!PACKAGE_kmod-ath10k-ct-smallbuffers:kmod-ath10k-ct
endef

define Package/ath10k-firmware-qca99x0-ct
$(Package/ath10k-ct-firmware-default)
  TITLE:=ath10k CT 10.4 firmware for QCA99x0 devices
  SECTION:=firmware
  CATEGORY:=Firmware
  PROVIDES:=ath10k-firmware-qca99x0
endef
define Package/ath10k-firmware-qca99x0-ct-full-htt
$(Package/ath10k-ct-firmware-default)
  TITLE:=ath10k CT 10.4 full-htt-mgt fw for QCA99x0
  SECTION:=firmware
  CATEGORY:=Firmware
  PROVIDES:=ath10k-firmware-qca99x0
  DEPENDS:=+!PACKAGE_kmod-ath10k-ct-smallbuffers:kmod-ath10k-ct
endef

define Package/ath10k-firmware-qca9984-ct
$(Package/ath10k-ct-firmware-default)
  TITLE:=ath10k CT 10.4 firmware for QCA9984 devices
  SECTION:=firmware
  CATEGORY:=Firmware
  PROVIDES:=ath10k-firmware-qca9984
endef
define Package/ath10k-firmware-qca9984-ct-full-htt
$(Package/ath10k-ct-firmware-default)
  TITLE:=ath10k CT 10.4 full-htt-mgt fw for QCA9984
  SECTION:=firmware
  CATEGORY:=Firmware
  PROVIDES:=ath10k-firmware-qca9984
  DEPENDS:=+!PACKAGE_kmod-ath10k-ct-smallbuffers:kmod-ath10k-ct
endef

define Package/ath10k-firmware-qca4019-ct
$(Package/ath10k-ct-firmware-default)
  TITLE:=ath10k CT 10.4 firmware for QCA4018/9
  SECTION:=firmware
  CATEGORY:=Firmware
  PROVIDES:=ath10k-firmware-qca4019
endef
define Package/ath10k-firmware-qca4019-ct-full-htt
$(Package/ath10k-ct-firmware-default)
  TITLE:=ath10k CT 10.4 full-htt-mgt for QCA4018/9
  SECTION:=firmware
  CATEGORY:=Firmware
  PROVIDES:=ath10k-firmware-qca4019
  DEPENDS:=+!PACKAGE_kmod-ath10k-ct-smallbuffers:kmod-ath10k-ct
endef

define Package/ath10k-firmware-qca9888-ct
$(Package/ath10k-ct-firmware-default)
  TITLE:=ath10k CT 10.4 fw for QCA9886/8 devices
  SECTION:=firmware
  CATEGORY:=Firmware
  PROVIDES:=ath10k-firmware-qca9888
endef
define Package/ath10k-firmware-qca9888-ct-full-htt
$(Package/ath10k-ct-firmware-default)
  TITLE:=ath10k CT 10.4 full-htt-mgt fw for QCA9886/8
  SECTION:=firmware
  CATEGORY:=Firmware
  PROVIDES:=ath10k-firmware-qca9888
  DEPENDS:=+!PACKAGE_kmod-ath10k-ct-smallbuffers:kmod-ath10k-ct
endef


define Package/ath10k-firmware-qca9887-ct/description
Alternative ath10k firmware for QCA9887 from Candela Technologies.
Enables IBSS and other features.  See:
http://www.candelatech.com/ath10k-10.1.php
This firmware conflicts with the standard 9887 firmware, so select only
one.
endef
define Package/ath10k-firmware-qca9887-ct-full-htt/description
Alternative ath10k firmware for QCA9887 from Candela Technologies.
Uses normal HTT TX data path for management frames, which improves
stability in busy networks and fixes .11r authentication.
Enables IBSS and other features.  See:
http://www.candelatech.com/ath10k-10.1.php
This firmware selects and requires the ath10k-ct driver.
endef

define Package/ath10k-firmware-qca988x-ct/description
Alternative ath10k firmware for QCA988X from Candela Technologies.
Enables IBSS and other features.  See:
http://www.candelatech.com/ath10k-10.1.php
This firmware will NOT be used unless the standard ath10k-firmware-qca988x
is un-selected since the driver will try to load firmware-5.bin before
firmware-2.bin
endef
define Package/ath10k-firmware-qca988x-ct-full-htt/description
Alternative ath10k firmware for QCA988X from Candela Technologies.
Uses normal HTT TX data path for management frames, which improves
stability in busy networks and fixes .11r authentication.
Enables IBSS and other features.  See:
http://www.candelatech.com/ath10k-10.1.php
This firmware selects and requires the ath10k-ct driver.
endef

define Package/ath10k-firmware-qca99x0-ct/description
Alternative ath10k firmware for QCA99x0 from Candela Technologies.
Enables IBSS and other features.  See:
http://www.candelatech.com/ath10k-10.4.php
This firmware conflicts with the standard 99x0 firmware, so select only
one.
endef
define Package/ath10k-firmware-qca99x0-ct-full-htt/description
Alternative ath10k firmware for QCA99x0 from Candela Technologies.
Uses normal HTT TX data path for management frames, which improves
stability in busy networks and may be required for .11r authentication.
Enables IBSS and other features.  See:
http://www.candelatech.com/ath10k-10.4.php
This firmware selects and requires the ath10k-ct driver.
endef

define Package/ath10k-firmware-qca9984-ct/description
Alternative ath10k firmware for QCA9984 from Candela Technologies.
Enables IBSS and other features.  See:
http://www.candelatech.com/ath10k-10.4.php
This firmware conflicts with the standard 9984 firmware, so select only
one.
endef
define Package/ath10k-firmware-qca9984-ct-full-htt/description
Alternative ath10k firmware for QCA9984 from Candela Technologies.
Uses normal HTT TX data path for management frames, which improves
stability in busy networks and may be required for .11r authentication.
Enables IBSS and other features.  See:
http://www.candelatech.com/ath10k-10.4.php
This firmware selects and requires the ath10k-ct driver.
endef

define Package/ath10k-firmware-qca4019-ct/description
Alternative ath10k firmware for IPQ4019 radio from Candela Technologies.
Enables IBSS and other features.  Works with standard or ath10k-ct driver.
See:  http://www.candelatech.com/ath10k-10.4.php
endef
define Package/ath10k-firmware-qca4019-ct-full-htt/description
Alternative ath10k firmware for IPQ4019 radio from Candela Technologies.
Uses normal HTT TX data path for management frames, which improves
stability in busy networks and may be required for .11r authentication.
Enables IBSS and other features.
See:  http://www.candelatech.com/ath10k-10.4.php
This firmware selects and requires the ath10k-ct driver.
endef

define Package/ath10k-firmware-qca9888-ct/description
Alternative ath10k firmware for QCA9886 and QCA9888 from Candela Technologies.
Enables IBSS and other features.  See:
http://www.candelatech.com/ath10k-10.4.php
This firmware conflicts with the standard 9886 and 9888 firmware, so select only
one.
endef
define Package/ath10k-firmware-qca9888-ct-full-htt/description
Alternative ath10k firmware for QCA9886 and QCA9888 from Candela Technologies.
Uses normal HTT TX data path for management frames, which improves
stability in busy networks and may be required for .11r authentication.
Enables IBSS and other features.  See:
http://www.candelatech.com/ath10k-10.4.php
This firmware selects and requires the ath10k-ct driver.
endef


define Build/Compile

endef


define Package/ath10k-firmware-qca9887-ct/install
	$(INSTALL_DIR) $(1)/lib/firmware/ath10k/QCA9887/hw1.0
	$(INSTALL_DATA) \
		$(DL_DIR)/$(call CT_FIRMWARE_FILE,QCA9887) \
		$(1)/lib/firmware/ath10k/QCA9887/hw1.0/firmware-2.bin
	$(INSTALL_DATA) \
		$(DL_DIR)/$(QCA9887_BOARD_FILE) \
		$(1)/lib/firmware/ath10k/QCA9887/hw1.0/board.bin
endef
define Package/ath10k-firmware-qca9887-ct-full-htt/install
	$(INSTALL_DIR) $(1)/lib/firmware/ath10k/QCA9887/hw1.0
	$(INSTALL_DATA) \
		$(DL_DIR)/$(call CT_FIRMWARE_FILE_FULL_HTT,QCA9887) \
		$(1)/lib/firmware/ath10k/QCA9887/hw1.0/ct-firmware-2.bin
	$(INSTALL_DATA) \
		$(DL_DIR)/$(QCA9887_BOARD_FILE) \
		$(1)/lib/firmware/ath10k/QCA9887/hw1.0/board.bin
endef

define Package/ath10k-firmware-qca988x-ct/install
	$(INSTALL_DIR) $(1)/lib/firmware/ath10k/QCA988X/hw2.0
	$(INSTALL_DATA) \
		$(DL_DIR)/$(QCA988X_BOARD_FILE) \
		$(1)/lib/firmware/ath10k/QCA988X/hw2.0/board.bin
	$(INSTALL_DATA) \
		$(DL_DIR)/$(call CT_FIRMWARE_FILE,QCA988X) \
		$(1)/lib/firmware/ath10k/QCA988X/hw2.0/firmware-2.bin
endef
define Package/ath10k-firmware-qca988x-ct-full-htt/install
	$(INSTALL_DIR) $(1)/lib/firmware/ath10k/QCA988X/hw2.0
	$(INSTALL_DATA) \
		$(DL_DIR)/$(QCA988X_BOARD_FILE) \
		$(1)/lib/firmware/ath10k/QCA988X/hw2.0/board.bin
	$(INSTALL_DATA) \
		$(DL_DIR)/$(call CT_FIRMWARE_FILE_FULL_HTT,QCA988X) \
		$(1)/lib/firmware/ath10k/QCA988X/hw2.0/ct-firmware-2.bin
endef

define Package/ath10k-firmware-qca99x0-ct/install
	$(INSTALL_DIR) $(1)/lib/firmware/ath10k/QCA99X0/hw2.0
	$(INSTALL_DATA) \
		$(DL_DIR)/$(QCA99X0_BOARD2_FILE) \
		$(1)/lib/firmware/ath10k/QCA99X0/hw2.0/board-2.bin
	$(INSTALL_DATA) \
		$(DL_DIR)/$(QCA99X0_BOARD_FILE) \
		$(1)/lib/firmware/ath10k/QCA99X0/hw2.0/board.bin
	$(INSTALL_DATA) \
		$(DL_DIR)/$(call CT_FIRMWARE_FILE,QCA99X0) \
		$(1)/lib/firmware/ath10k/QCA99X0/hw2.0/firmware-5.bin
endef
define Package/ath10k-firmware-qca99x0-ct-full-htt/install
	$(INSTALL_DIR) $(1)/lib/firmware/ath10k/QCA99X0/hw2.0
	$(INSTALL_DATA) \
		$(DL_DIR)/$(QCA99X0_BOARD2_FILE) \
		$(1)/lib/firmware/ath10k/QCA99X0/hw2.0/board-2.bin
	$(INSTALL_DATA) \
		$(DL_DIR)/$(QCA99X0_BOARD_FILE) \
		$(1)/lib/firmware/ath10k/QCA99X0/hw2.0/board.bin
	$(INSTALL_DATA) \
		$(DL_DIR)/$(call CT_FIRMWARE_FILE_FULL_HTT,QCA99X0) \
		$(1)/lib/firmware/ath10k/QCA99X0/hw2.0/ct-firmware-5.bin
endef

define Package/ath10k-firmware-qca9984-ct/install
	$(INSTALL_DIR) $(1)/lib/firmware/ath10k/QCA9984/hw1.0
	$(INSTALL_DATA) \
		$(DL_DIR)/$(QCA9984_BOARD2_FILE) \
		$(1)/lib/firmware/ath10k/QCA9984/hw1.0/board-2.bin
	$(INSTALL_DATA) \
		$(DL_DIR)/$(call CT_FIRMWARE_FILE,QCA9984) \
		$(1)/lib/firmware/ath10k/QCA9984/hw1.0/firmware-5.bin
endef
define Package/ath10k-firmware-qca9984-ct-full-htt/install
	$(INSTALL_DIR) $(1)/lib/firmware/ath10k/QCA9984/hw1.0
	$(INSTALL_DATA) \
		$(DL_DIR)/$(QCA9984_BOARD2_FILE) \
		$(1)/lib/firmware/ath10k/QCA9984/hw1.0/board-2.bin
	$(INSTALL_DATA) \
		$(DL_DIR)/$(call CT_FIRMWARE_FILE_FULL_HTT,QCA9984) \
		$(1)/lib/firmware/ath10k/QCA9984/hw1.0/ct-firmware-5.bin
endef

define Package/ath10k-firmware-qca4019-ct/install
	$(INSTALL_DIR) $(1)/lib/firmware/ath10k/QCA4019/hw1.0
	$(INSTALL_DATA) \
		$(DL_DIR)/$(QCA4019_BOARD2_FILE) \
		$(1)/lib/firmware/ath10k/QCA4019/hw1.0/board-2.bin
	$(INSTALL_DATA) \
		$(DL_DIR)/$(call CT_FIRMWARE_FILE,QCA4019) \
		$(1)/lib/firmware/ath10k/QCA4019/hw1.0/firmware-5.bin
endef
define Package/ath10k-firmware-qca4019-ct-full-htt/install
	$(INSTALL_DIR) $(1)/lib/firmware/ath10k/QCA4019/hw1.0
	$(INSTALL_DATA) \
		$(DL_DIR)/$(QCA4019_BOARD2_FILE) \
		$(1)/lib/firmware/ath10k/QCA4019/hw1.0/board-2.bin
	$(INSTALL_DATA) \
		$(DL_DIR)/$(call CT_FIRMWARE_FILE_FULL_HTT,QCA4019) \
		$(1)/lib/firmware/ath10k/QCA4019/hw1.0/ct-firmware-5.bin
endef

define Package/ath10k-firmware-qca9888-ct/install
	$(INSTALL_DIR) $(1)/lib/firmware/ath10k/QCA9888/hw2.0
	ln -s \
		../../cal-pci-0000:01:00.0.bin \
		$(1)/lib/firmware/ath10k/QCA9888/hw2.0/board.bin
	$(INSTALL_DATA) \
		$(DL_DIR)/$(QCA9888_BOARD2_FILE) \
		$(1)/lib/firmware/ath10k/QCA9888/hw2.0/board-2.bin
	$(INSTALL_DATA) \
		$(DL_DIR)/$(call CT_FIRMWARE_FILE,QCA9888) \
		$(1)/lib/firmware/ath10k/QCA9888/hw2.0/firmware-5.bin
endef
define Package/ath10k-firmware-qca9888-ct-full-htt/install
	$(INSTALL_DIR) $(1)/lib/firmware/ath10k/QCA9888/hw2.0
	ln -s \
		../../cal-pci-0000:01:00.0.bin \
		$(1)/lib/firmware/ath10k/QCA9888/hw2.0/board.bin
	$(INSTALL_DATA) \
		$(DL_DIR)/$(QCA9888_BOARD2_FILE) \
		$(1)/lib/firmware/ath10k/QCA9888/hw2.0/board-2.bin
	$(INSTALL_DATA) \
		$(DL_DIR)/$(call CT_FIRMWARE_FILE_FULL_HTT,QCA9888) \
		$(1)/lib/firmware/ath10k/QCA9888/hw2.0/ct-firmware-5.bin
endef


$(eval $(call BuildPackage,ath10k-firmware-qca9887-ct))
$(eval $(call BuildPackage,ath10k-firmware-qca9887-ct-full-htt))
$(eval $(call BuildPackage,ath10k-firmware-qca988x-ct))
$(eval $(call BuildPackage,ath10k-firmware-qca988x-ct-full-htt))
$(eval $(call BuildPackage,ath10k-firmware-qca99x0-ct))
$(eval $(call BuildPackage,ath10k-firmware-qca99x0-ct-full-htt))
$(eval $(call BuildPackage,ath10k-firmware-qca9984-ct))
$(eval $(call BuildPackage,ath10k-firmware-qca9984-ct-full-htt))
$(eval $(call BuildPackage,ath10k-firmware-qca4019-ct))
$(eval $(call BuildPackage,ath10k-firmware-qca4019-ct-full-htt))
$(eval $(call BuildPackage,ath10k-firmware-qca9888-ct))
$(eval $(call BuildPackage,ath10k-firmware-qca9888-ct-full-htt))
