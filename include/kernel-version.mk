# Use the default kernel version if the Makefile doesn't override it

LINUX_RELEASE?=1

LINUX_VERSION-3.3  = .8
LINUX_VERSION-3.8  = .13
LINUX_VERSION-3.10 = .49
LINUX_VERSION-3.13 = .7
LINUX_VERSION-3.14 = .18

LINUX_KERNEL_MD5SUM-3.3.8   = f1058f64eed085deb44f10cee8541d50
LINUX_KERNEL_MD5SUM-3.8.13  = 2af19d06cd47ec459519159cdd10542d
LINUX_KERNEL_MD5SUM-3.10.49 = 9774e12764e740d49c80eda77d0ef3eb
LINUX_KERNEL_MD5SUM-3.13.7  = 370adced5e5c1cb1d0d621c2dae2723f
LINUX_KERNEL_MD5SUM-3.14.18 = 3b002a06d0964d8101abec768c2d4eef

ifdef KERNEL_PATCHVER
  LINUX_VERSION:=$(KERNEL_PATCHVER)$(strip $(LINUX_VERSION-$(KERNEL_PATCHVER)))
endif

split_version=$(subst ., ,$(1))
merge_version=$(subst $(space),.,$(1))
KERNEL_BASE=$(firstword $(subst -, ,$(LINUX_VERSION)))
KERNEL=$(call merge_version,$(wordlist 1,2,$(call split_version,$(KERNEL_BASE))))
KERNEL_PATCHVER ?= $(KERNEL)

# disable the md5sum check for unknown kernel versions
LINUX_KERNEL_MD5SUM:=$(LINUX_KERNEL_MD5SUM-$(strip $(LINUX_VERSION)))
LINUX_KERNEL_MD5SUM?=x
