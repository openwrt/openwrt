BOARDNAME:=NXP QorIQ (PowerPC e5500, glibc)
ARCH_PACKAGES:=powerpc64_e5500_glibc

define Target/Description
	Build firmware images for NXP QorIQ boards with e5500 cores using
	glibc. Uses the same kernel configuration as the generic subtarget.
endef
