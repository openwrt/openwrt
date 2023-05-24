#
# Copyright (C) 2016 Jiang Yutang <jiangyutang1978@gmail.com>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ls-rcw
PKG_VERSION:=lf-6.1.1-1.0.0
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/nxp-qoriq/rcw
PKG_SOURCE_VERSION:=lf-6.1.1-1.0.0
PKG_MIRROR_HASH:=3127285549a0a7dd3cded20c2fbfb67b06943faa8dab529f61936dc1bea177db

PKG_FLAGS:=nonshared

include $(INCLUDE_DIR)/package.mk

define Package/layerscape-rcw
  SECTION:=firmware
  CATEGORY:=Firmware
  TITLE:=NXP Layerscape RCW binaries
  DEPENDS:=@TARGET_layerscape
endef

BOARDS := \
  ls1012a-rdb \
  ls1012a-frdm \
  ls1012a-frwy-sdboot \
  ls1028a-rdb \
  ls1028a-rdb-sdboot \
  ls1043a-rdb \
  ls1043a-rdb-sdboot \
  ls1046a-frwy \
  ls1046a-frwy-sdboot \
  ls1046a-rdb \
  ls1046a-rdb-sdboot \
  ls1088a-rdb \
  ls1088a-rdb-sdboot \
  ls2088a-rdb \
  lx2160a-rdb \
  lx2160a-rdb-sdboot \
  ls1021a-twr

RCW_ls1012a-rdb         :=ls1012ardb/R_SPNH_3508/rcw_1000_default.bin
RCW_ls1012a-frdm        :=ls1012afrdm/N_SSNP_3305/rcw_800.bin
RCW_ls1012a-frwy-sdboot :=ls1012afrwy/N_SSNP_3305/rcw_1000_default.bin
RCW_ls1028a-rdb         :=ls1028ardb/R_SQPP_0x85bb/rcw_1300.bin
RCW_ls1028a-rdb-sdboot  :=ls1028ardb/R_SQPP_0x85bb/rcw_1500_sdboot.bin
RCW_ls1043a-rdb         :=ls1043ardb/RR_FQPP_1455/rcw_1600.bin
RCW_ls1043a-rdb-sdboot  :=ls1043ardb/RR_FQPP_1455/rcw_1600_sdboot.bin
RCW_ls1046a-frwy        :=ls1046afrwy/NN_NNQNNPNP_3040_0506/rcw_1600_qspiboot.bin
RCW_ls1046a-frwy-sdboot :=ls1046afrwy/NN_NNQNNPNP_3040_0506/rcw_1600_sdboot.bin
RCW_ls1046a-rdb         :=ls1046ardb/RR_FFSSPPPH_1133_5559/rcw_1800_qspiboot.bin
RCW_ls1046a-rdb-sdboot  :=ls1046ardb/RR_FFSSPPPH_1133_5559/rcw_1800_sdboot.bin
RCW_ls1088a-rdb         :=ls1088ardb/FCQQQQQQQQ_PPP_H_0x1d_0x0d/rcw_1600_qspi.bin
RCW_ls1088a-rdb-sdboot  :=ls1088ardb/FCQQQQQQQQ_PPP_H_0x1d_0x0d/rcw_1600_sd.bin
RCW_ls2088a-rdb         :=ls2088ardb/FFFFFFFF_PP_HH_0x2a_0x41/rcw_1800.bin
RCW_lx2160a-rdb         :=lx2160ardb_rev2/XGGFF_PP_HHHH_RR_19_5_2/rcw_2000_700_2900_19_5_2.bin
RCW_lx2160a-rdb-sdboot  :=lx2160ardb_rev2/XGGFF_PP_HHHH_RR_19_5_2/rcw_2000_700_2900_19_5_2.bin
RCW_ls1021a-twr         :=ls1021atwr/SSR_PNS_30/rcw_1200.bin

define Build/InstallDev
	$(INSTALL_DIR) $(STAGING_DIR_IMAGE)
	$(foreach board,$(BOARDS), \
		$(CP) $(PKG_BUILD_DIR)/$(RCW_$(board)) $(STAGING_DIR_IMAGE)/fsl_$(board)-rcw.bin;)
endef

$(eval $(call BuildPackage,layerscape-rcw))
