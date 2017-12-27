ARCH:=aarch64
SUBTARGET:=64
BOARDNAME:=ARMv8 Server
KERNELNAME:=Image

define Target/Description
	Build images for ARMv8 Server's and Virtual Machines, compliant to the ARM Server
  	Boot Base and Linaro VM Specifications.

	ARM machines supporting the ARM Embedded Boot Base specification should also work.
	
	These machines all use UEFI to boot, either with EDK or through u-boot's bootefi.

	Targets using ACPI and Device Tree are supported.
endef
