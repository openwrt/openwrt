CPU_TYPE:=xlr
CPU_FLAGS_xlr:=-march=xlr -mabi=64
BOARDNAME:=xlr

define Target/Description
	Build firmware images for Broadcom/Netlogic XLR boards.
endef
