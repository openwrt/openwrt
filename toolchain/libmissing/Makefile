include $(TOPDIR)/rules.mk

LIBMISSING_DIR:=$(TOOL_BUILD_DIR)/libmissing

LIBMISSING_SRCS+=./files/math.c
LIBMISSING_OBJS:=$(patsubst ./files/%.c,$(LIBMISSING_DIR)/%.o,$(LIBMISSING_SRCS))

$(LIBMISSING_DIR)/.prepared:
	mkdir -p $(LIBMISSING_DIR)
	touch $@

$(LIBMISSING_OBJS): $(LIBMISSING_DIR)/%.o : ./files/%.c
	$(TARGET_CC) $(TARGET_CFLAGS) -c $< -o $@

$(LIBMISSING_DIR)/libmissing.a: $(LIBMISSING_OBJS)
	$(TARGET_CROSS)ar rc $(LIBMISSING_DIR)/libmissing.a $(LIBMISSING_OBJS)

$(STAGING_DIR)/usr/lib/libmissing.a: $(LIBMISSING_DIR)/libmissing.a
	mkdir -p $(STAGING_DIR)/usr/lib
	cp -fpR $< $@
	touch -c $@

source:
prepare: $(LIBMISSING_DIR)/.prepared
compile: $(LIBMISSING_DIR)/libmissing.a
install: $(STAGING_DIR)/usr/lib/libmissing.a
clean: 
	rm -rf \
		$(STAGING_DIR)/usr/lib/libmissing.a \
		$(LIBMISSING_DIR) \
