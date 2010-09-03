
define KernelPackage/crypto-mv_cesa
   TITLE:=Marvell crypto engine
   KCONFIG:=CONFIG_CRYPTO_DEV_MV_CESA
   FILES:=$(LINUX_DIR)/crypto/mv_cesa.ko
   AUTOLOAD:=$(call AutoLoad,09,mv_cesa)
   SUBMENU:=Cryptographic API modules
   DEPENDS:=kmod-crypto-core
endef

$(eval $(call KernelPackage,crypto-mv_cesa))
