obj-m += sf_crypto.o
sf_crypto-objs := core.o aes.o
sf_crypto-$(CONFIG_CRYPTO_SIFLOWER_HASH) += sha1.o sha256.o sha512.o md5.o

ccflags-y += -DCONFIG_CRYPTO_SIFLOWER_HASH -Wno-error -Wno-missing-prototypes \
	-Wno-missing-declarations -Wno-unused-variable -Wno-unused-label
