ARCH:=x86_64
BOARDNAME:=x86_64-v3
CPU_SUBTYPE:=v3

define Target/Description
        Build images for 64 bit systems that support v3 instructions.
        Requires v2 CPU features and AVX AVX2 BMI1 BMI2 F16C FMA LZCNT MOVBE OSXSAVE
endef
