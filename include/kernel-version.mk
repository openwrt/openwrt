# Use the default kernel version if the Makefile doesn't override it

LINUX_RELEASE?=1

ifeq ($(LINUX_VERSION),3.3.8)
  LINUX_KERNEL_MD5SUM:=f1058f64eed085deb44f10cee8541d50
endif
ifeq ($(LINUX_VERSION),3.6.11)
  LINUX_KERNEL_MD5SUM:=3d602ad7f7984509c3e923a5ae90bc54
endif
ifeq ($(LINUX_VERSION),3.7.10)
  LINUX_KERNEL_MD5SUM:=09624c3702107076efcac5e1cd8a18ec
endif
ifeq ($(LINUX_VERSION),3.8.11)
  LINUX_KERNEL_MD5SUM:=3c4f4fc96bf702d1e9dfbf4d4b4cde9d
endif
ifeq ($(LINUX_VERSION),3.9)
  LINUX_KERNEL_MD5SUM:=4348c9b6b2eb3144d601e87c19d5d909
endif

# disable the md5sum check for unknown kernel versions
LINUX_KERNEL_MD5SUM?=x

split_version=$(subst ., ,$(1))
merge_version=$(subst $(space),.,$(1))
KERNEL_BASE=$(firstword $(subst -, ,$(LINUX_VERSION)))
KERNEL=$(call merge_version,$(wordlist 1,2,$(call split_version,$(KERNEL_BASE))))
ifeq ($(firstword $(call split_version,$(KERNEL_BASE))),2)
  KERNEL_PATCHVER=$(call merge_version,$(wordlist 1,3,$(call split_version,$(KERNEL_BASE))))
else
  KERNEL_PATCHVER=$(KERNEL)
endif

