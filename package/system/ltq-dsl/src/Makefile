obj-m = lantiq_mei.o lantiq_atm.o
lantiq_atm-objs := ifxmips_atm_core.o

CFLAGS_MODULE+=-DSOC_TYPE_XWAY
EXTRA_CFLAGS+=-DSOC_TYPE_XWAY
ifeq ($(BUILD_VARIANT),danube)
  CFLAGS_MODULE+=-DCONFIG_DANUBE
  EXTRA_CFLAGS+=-DCONFIG_DANUBE
  lantiq_atm-objs += ifxmips_atm_danube.o
endif
ifeq ($(BUILD_VARIANT),ase)
  CFLAGS_MODULE+=-DCONFIG_AMAZON_SE
  EXTRA_CFLAGS+=-DCONFIG_AMAZON_SE
  lantiq_atm-objs += ifxmips_atm_amazon_se.o
endif
ifeq ($(BUILD_VARIANT),ar9)
  CFLAGS_MODULE+=-DCONFIG_AR9
  EXTRA_CFLAGS+=-DCONFIG_AR9
  lantiq_atm-objs += ifxmips_atm_ar9.o
endif
ifeq ($(BUILD_VARIANT),vr9)
  CFLAGS_MODULE+=-DCONFIG_VR9
  EXTRA_CFLAGS+=-DCONFIG_VR9
  lantiq_atm-objs += ifxmips_atm_vr9.o
endif
