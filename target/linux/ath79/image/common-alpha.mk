DEVICE_VARS += ALPHA_KEY_IV

define alpha-xor
	$(shell X="$(1)"; Y="$(2)"; \
		for((i=0; i<$${#X}; i++)); do \
			printf -v x "%d" "'$${X:i:1}"; \
			Yi=$$((i % $${#Y})); \
			printf -v y "%d" "'$${Y:Yi:1}"; \
			printf "%02x" $$((x ^ y ^ (i+1))); \
		done)
endef

define Build/alpha_encimg
	$(eval mangled_key :=$(call alpha-xor,$(word 1,$(ALPHA_KEY_IV)),$(SEAMA_SIGNATURE)))
	$(eval mangled_iv  :=$(call alpha-xor,$(word 2,$(ALPHA_KEY_IV)),$(SEAMA_SIGNATURE)))
	let \
		size="$$(stat -c%s $@)" \
		pad="(16 - (size % 16))"; \
		dd bs=$$pad count=1 if=/dev/zero 2>/dev/null >> $@; \
		printf '%08x' $$size | sed 's/../\\\\x&/g' | xargs echo -ne >> $@; \
		dd bs=12 count=1 if=/dev/zero 2>/dev/null >> $@
	$(STAGING_DIR_HOST)/bin/openssl aes-256-cbc -nosalt -nopad \
		-K $(mangled_key) -iv $(mangled_iv) -in $@ -out $@.aes
	mv $@.aes $@
endef

define Device/alpha-encimg
  IMAGES += factory.bin recovery.bin
  IMAGE/recovery.bin := $$(IMAGE/default) | pad-rootfs -x 64 | seama | \
	seama-seal | check-size
  IMAGE/factory.bin := $$(IMAGE/recovery.bin) | alpha_encimg
endef
