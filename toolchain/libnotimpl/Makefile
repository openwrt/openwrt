include $(TOPDIR)/rules.mk

LIBNOTIMPL_DIR:=$(TOOL_BUILD_DIR)/libnotimpl

LIBNOTIMPL_SRCS+=./files/math.c
LIBNOTIMPL_OBJS:=$(patsubst ./files/%.c,$(LIBNOTIMPL_DIR)/%.o,$(LIBNOTIMPL_SRCS))

$(LIBNOTIMPL_DIR)/.prepared:
	mkdir -p $(LIBNOTIMPL_DIR)
	touch $@

$(LIBNOTIMPL_OBJS): $(LIBNOTIMPL_DIR)/%.o : ./files/%.c
	$(TARGET_CC) $(TARGET_CFLAGS) -c $< -o $@

$(LIBNOTIMPL_DIR)/libnotimpl.a: $(LIBNOTIMPL_OBJS)
	$(TARGET_CROSS)ar rc $(LIBNOTIMPL_DIR)/libnotimpl.a $(LIBNOTIMPL_OBJS)

$(STAGING_DIR)/usr/lib/libnotimpl.a: $(LIBNOTIMPL_DIR)/libnotimpl.a
	mkdir -p $(STAGING_DIR)/usr/lib
	$(CP) $< $@
	touch -c $@

source:
prepare: $(LIBNOTIMPL_DIR)/.prepared
compile: $(LIBNOTIMPL_DIR)/libnotimpl.a
install: $(STAGING_DIR)/usr/lib/libnotimpl.a
clean: 
	rm -rf \
		$(STAGING_DIR)/usr/lib/libnotimpl.a \
		$(LIBNOTIMPL_DIR) \
