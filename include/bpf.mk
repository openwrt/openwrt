BPF_DEPENDS := @HAS_BPF_TOOLCHAIN +@NEED_BPF_TOOLCHAIN

CLANG_MIN_VER:=12

ifneq ($(CONFIG_USE_LLVM_HOST),)
  find-llvm-tool=$(firstword $(shell PATH='$(BPF_PATH)' command -v $(1) || echo '$(firstword $(1))-not-found'))

  BPF_TOOLCHAIN_HOST_PATH:=$(call qstrip,$(CONFIG_BPF_TOOLCHAIN_HOST_PATH))
  ifneq ($(BPF_TOOLCHAIN_HOST_PATH),)
    BPF_PATH:=$(BPF_TOOLCHAIN_HOST_PATH)/bin:$(PATH)
  else
    BPF_PATH:=$(PATH)
  endif
  CLANG:=$(call find-llvm-tool,clang clang-13 clang-12)
  LLVM_VER:=$(subst clang,,$(notdir $(CLANG)))

  BPF_PATH:=$(dir $(CLANG)):$(BPF_PATH)
  LLVM_LLC:=$(call find-llvm-tool,llc$(LLVM_VER))
  LLVM_DIS:=$(call find-llvm-tool,llvm-dis$(LLVM_VER))
  LLVM_OPT:=$(call find-llvm-tool,opt$(LLVM_VER))
  LLVM_STRIP:=$(call find-llvm-tool,llvm-strip$(LLVM_VER))
else
  LLVM_PATH:=/invalid

  ifneq ($(CONFIG_USE_LLVM_PREBUILT),)
    LLVM_PATH:=$(TOPDIR)/llvm-bpf/bin
  endif
  ifneq ($(CONFIG_USE_LLVM_BUILD),)
    LLVM_PATH:=$(STAGING_DIR_HOST)/llvm-bpf/bin
  endif

  CLANG:=$(LLVM_PATH)/clang
  LLVM_LLC:=$(LLVM_PATH)/llc
  LLVM_DIS:=$(LLVM_PATH)/llvm-dis
  LLVM_OPT:=$(LLVM_PATH)/opt
  LLVM_STRIP:=$(LLVM_PATH)/llvm-strip
endif

BPF_KARCH:=mips
BPF_ARCH:=mips$(if $(CONFIG_ARCH_64BIT),64)$(if $(CONFIG_BIG_ENDIAN),,el)
BPF_TARGET:=bpf$(if $(CONFIG_BIG_ENDIAN),eb,el)

BPF_HEADERS_DIR:=$(STAGING_DIR)/bpf-headers

BPF_KERNEL_INCLUDE := \
	-nostdinc -isystem $(TOOLCHAIN_ROOT_DIR)/lib/gcc/*/*/include \
	$(patsubst %,-isystem%,$(TOOLCHAIN_INC_DIRS)) \
	-I$(BPF_HEADERS_DIR)/arch/$(BPF_KARCH)/include \
	-I$(BPF_HEADERS_DIR)/arch/$(BPF_KARCH)/include/asm/mach-generic \
	-I$(BPF_HEADERS_DIR)/arch/$(BPF_KARCH)/include/generated \
	-I$(BPF_HEADERS_DIR)/include \
	-I$(BPF_HEADERS_DIR)/arch/$(BPF_KARCH)/include/uapi \
	-I$(BPF_HEADERS_DIR)/arch/$(BPF_KARCH)/include/generated/uapi \
	-I$(BPF_HEADERS_DIR)/include/uapi \
	-I$(BPF_HEADERS_DIR)/include/generated/uapi \
	-I$(BPF_HEADERS_DIR)/tools/lib \
	-I$(BPF_HEADERS_DIR)/tools/testing/selftests \
	-I$(BPF_HEADERS_DIR)/samples/bpf \
	-include linux/kconfig.h -include asm_goto_workaround.h

BPF_CFLAGS := \
	$(BPF_KERNEL_INCLUDE) -I$(PKG_BUILD_DIR) \
	-D__KERNEL__ -D__BPF_TRACING__ -DCONFIG_GENERIC_CSUM \
	-D__TARGET_ARCH_${BPF_KARCH} \
	-m$(if $(CONFIG_BIG_ENDIAN),big,little)-endian \
	-fno-stack-protector -Wall \
	-Wno-unused-value -Wno-pointer-sign \
	-Wno-compare-distinct-pointer-types \
	-Wno-gnu-variable-sized-type-not-at-end \
	-Wno-address-of-packed-member -Wno-tautological-compare \
	-Wno-unknown-warning-option \
	-fno-asynchronous-unwind-tables \
	-Wno-uninitialized -Wno-unused-variable \
	-Wno-unused-label \
	-O2 -emit-llvm -Xclang -disable-llvm-passes

ifneq ($(CONFIG_HAS_BPF_TOOLCHAIN),)
ifeq ($(DUMP)$(filter download refresh,$(MAKECMDGOALS)),)
  CLANG_VER:=$(shell $(CLANG) --target=$(BPF_TARGET) -dM -E - < /dev/null | grep __clang_major__ | cut -d' ' -f3)
  CLANG_VER_VALID:=$(shell [ "$(CLANG_VER)" -ge "$(CLANG_MIN_VER)" ] && echo 1 )
  ifeq ($(CLANG_VER_VALID),)
    $(error ERROR: LLVM/clang version too old. Minimum required: $(CLANG_MIN_VER), found: $(CLANG_VER))
  endif
endif
endif

define CompileBPF
	$(CLANG) -g -target $(BPF_ARCH)-linux-gnu $(BPF_CFLAGS) $(2) \
		-c $(1) -o $(patsubst %.c,%.bc,$(1))
	$(LLVM_OPT) -O2 -mtriple=$(BPF_TARGET) < $(patsubst %.c,%.bc,$(1)) > $(patsubst %.c,%.opt,$(1))
	$(LLVM_DIS) < $(patsubst %.c,%.opt,$(1)) > $(patsubst %.c,%.S,$(1))
	$(LLVM_LLC) -march=$(BPF_TARGET) -mcpu=v3 -filetype=obj -o $(patsubst %.c,%.o,$(1)) < $(patsubst %.c,%.S,$(1))
	$(CP) $(patsubst %.c,%.o,$(1)) $(patsubst %.c,%.debug.o,$(1))
	$(LLVM_STRIP) --strip-debug $(patsubst %.c,%.o,$(1))
endef

