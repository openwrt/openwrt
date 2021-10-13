#****************************************************************************
#
#	Copyright (c) 2014 Qualcomm Atheros, Inc.
#	All Rights Reserved.
#	Qualcomm Atheros Confidential and Proprietary.
#
#****************************************************************************/

ifneq ($(strip $(TOOLPREFIX)),)
export  CROSS:=$(TOOLPREFIX)
endif

THERMALD_INSTALL_ROOT := $(THERMALDIR)/install

ifndef INSTALL_ROOT
INSTALL_ROOT=$(THERMALD_INSTALL_ROOT)
endif

export CC = $(CROSS)gcc
export CFLAGS += -O2 -Wall -DIPQ_806x -c
export STRIP = $(CROSS)strip
export SOURCES= \
		thermal.c \
		thermal_config.c \
		thermal_util.c \
		thermal_monitor.c \
		thermal_actions.c \
		sensors-ipq.c \
		tsens-sensor.c
export OBJECTS=$(SOURCES:.c=.o)
export EXECUTABLE=thermald

LIBS += -lpthread -ldl
CFLAGS += -L$(INSTALL_ROOT)/lib $(TARGET_CPPFLAGS) $(TARGET_CFLAGS) \
		-fstack-protector-all -fpie
LDFLAGS += $(TARGET_LDFLAGS) -pie

# What we build by default:
ALL = $(EXECUTABLE)

# RULES ---------------------------------------------------------------

# Making default targets:
all: local install
	@echo All done in `pwd`

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LIB_PATH) $(LIBS) $(LDFLAGS) $(OBJECTS) -o $@
	@echo Build $@ successufully...

.c.o:
	$(CC) $(INCLUDE) $(CFLAGS) $(LDFLAGS) $< -o $@

clean:
	rm -rf *.o *.d $(EXECUTABLE)

local: $(SOURCES) $(EXECUTABLE)
	@echo Build $@ successufully...

# Doing installation (see comments at top of this file)
install: local
	mkdir -p $(INSTALL_ROOT)/usr/sbin/
	cp -a -f $(ALL) $(INSTALL_ROOT)/usr/sbin/
	mkdir -p $(INSTALL_ROOT)/etc/thermal
	cp -a -f ipq-thermald-806?.conf $(INSTALL_ROOT)/etc/thermal/
	@echo Installed outputs from `pwd`

# Remove all generated files
#clean: default_clean # from Makefile.rules
clean:
	rm -rf install/usr/sbin/thermal
	rm -rf ./$(ALL)

# END --------------------------------------------------------------------
