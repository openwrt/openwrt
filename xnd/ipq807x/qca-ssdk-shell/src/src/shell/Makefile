LOC_DIR=src/shell
LIB=SHELL

include $(PRJ_PATH)/make/config.mk
include $(PRJ_PATH)/make/components.mk

SRC_LIST=$(wildcard *.c)
ifeq (,$(findstring SHELL, $(COMPONENTS)))
all: dep obj
else
all: dep obj lib
endif

include $(PRJ_PATH)/make/defs.mk
include $(PRJ_PATH)/make/target.mk

ifeq (TRUE, $(API_LOCK))
    PT_LIB=-lpthread
else
    PT_LIB=
endif

lib:
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) $(LDFLAGS) $(EXTRA_LDFLAGS) $(OBJ_FILE) $(BIN_DIR)/$(US_MOD)_$(RUNMODE).a -o $(DST_DIR)/$(SHELLOBJ) $(PT_LIB)
