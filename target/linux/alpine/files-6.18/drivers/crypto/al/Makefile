ccflags-y := -I$(srctree)/arch/arm/mach-alpine/include

obj-$(CONFIG_CRYPTO_DEV_AL_CRYPTO) += al_crypto.o

al_crypto-objs := al_crypto_main.o
al_crypto-objs += al_crypto_core.o
al_crypto-objs += al_crypto_skcipher.o
al_crypto-objs += al_crypto_aead.o
al_crypto-objs += al_crypto_hash.o
al_crypto-objs += al_crypto_crc.o
al_crypto-objs += al_hal_ssm_crypto.o
al_crypto-objs += al_hal_ssm_crc_memcpy.o
al_crypto-objs += al_crypto_sysfs.o
al_crypto-objs += al_crypto_module_params.o
