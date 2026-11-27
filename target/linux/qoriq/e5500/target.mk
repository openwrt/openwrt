BOARDNAME:=NXP QorIQ (PowerPC e5500, glibc)
ARCH_PACKAGES:=powerpc64_e5500_glibc

define Target/Description
	Build firmware images for NXP QorIQ boards with e5500 cores using
	glibc and an e5500-specific kernel configuration.
endef
