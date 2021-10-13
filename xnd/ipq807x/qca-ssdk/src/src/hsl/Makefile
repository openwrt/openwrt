LOC_DIR=/src/hsl
LIB=HSL

include $(PRJ_PATH)/make/config.mk

SRC_LIST=hsl_dev.c hsl_port_prop.c hsl_api.c

ifeq (TRUE, $(IN_ACL))
	ifeq (SHIVA, $(CHIP_TYPE))
		SRC_LIST += hsl_acl.c
	endif
	ifeq (GARUDA, $(CHIP_TYPE))
		SRC_LIST += hsl_acl.c
	endif
	ifeq (ALL_CHIP, $(CHIP_TYPE))
		SRC_LIST += hsl_acl.c
	endif
	ifeq (NONHK_CHIP, $(CHIP_TYPE))
                SRC_LIST += hsl_acl.c
        endif
endif

ifeq (linux, $(OS))
	ifeq (KSLIB, $(MODULE_TYPE))
    ifneq (TRUE, $(KERNEL_MODE))
      SRC_LIST=hsl_dev.c hsl_api.c
	  endif
	endif
endif

ifeq (TRUE, $(API_LOCK))
  SRC_LIST += hsl_lock.c
endif

include $(PRJ_PATH)/make/components.mk
include $(PRJ_PATH)/make/defs.mk
include $(PRJ_PATH)/make/target.mk

all: dep obj
