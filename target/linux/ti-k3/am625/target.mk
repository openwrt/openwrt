ARCH:=aarch64
SUBTARGET:=am625
BOARDNAME:=AM62x (TI K3)
CPU_TYPE:=cortex-a53

define Target/Description
	Build firmware images for TI AM62x-based boards, starting with the
	BeagleBoard BeaglePlay.
endef
