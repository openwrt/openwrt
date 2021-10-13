
ifeq (linux, $(OS))
  CC=$(TOOL_PATH)/$(TOOLPREFIX)gcc
  AR=$(TOOL_PATH)/$(TOOLPREFIX)ar
  LD=$(TOOL_PATH)/$(TOOLPREFIX)ld
  STRIP=$(TOOL_PATH)/$(TOOLPREFIX)strip
  MAKE=make -S
  CP=cp
  MKDIR=mkdir
  RM=rm
  PERL=perl
endif
