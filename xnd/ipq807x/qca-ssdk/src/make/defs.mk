DST_DIR=$(BLD_DIR)/$(MODULE_TYPE)

SUB_DIR=$(patsubst %/, %, $(dir $(wildcard ./*/Makefile)))

ifeq (,$(findstring $(LIB), $(COMPONENTS)))
  SRC_LIST=
endif

SRC_FILE=$(addprefix $(PRJ_PATH)/$(LOC_DIR)/, $(SRC_LIST))

OBJ_LIST=$(SRC_LIST:.c=.o)
OBJ_FILE=$(addprefix $(DST_DIR)/, $(OBJ_LIST))

DEP_LIST=$(SRC_LIST:.c=.d)
DEP_FILE=$(addprefix $(DST_DIR)/, $(DEP_LIST))

vpath %.c $(PRJ_PATH)/$(LOC_DIR)
vpath %.c $(PRJ_PATH)/app/nathelper/linux
vpath %.c $(PRJ_PATH)/app/nathelper/linux/lib
vpath %.o $(DST_DIR)
vpath %.d $(DST_DIR)

DEP_LOOP=$(foreach i, $(SUB_DIR), $(MAKE) -C $(i) dep || exit 1;)
OBJ_LOOP=$(foreach i, $(SUB_DIR), $(MAKE) -C $(i) obj || exit 1;)
CLEAN_LOOP=$(foreach i, $(SUB_DIR), $(MAKE) -C $(i) clean;)
CLEAN_OBJ_LOOP=$(foreach i, $(SUB_DIR), $(MAKE) -C $(i) clean_o;)
CLEAN_DEP_LOOP=$(foreach i, $(SUB_DIR), $(MAKE) -C $(i) clean_d;)

