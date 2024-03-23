ARCH:=x86_64
BOARDNAME:=x86_64-v4
CPU_SUBTYPE:=v4

define Target/Description
        Build images for 64 bit systems that support v4 instructions.
        Requires v3 CPU features and AVX512F AVX512BW AVX512CD AVX512DQ AVX512VL
endef
