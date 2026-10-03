ARCH:=x86_64
BOARDNAME:=x86_64-v2
CPU_SUBTYPE:=v2

define Target/Description
        Build images for 64 bit systems that support v2 instructions.
        Requires CPU features CMPXCHG16B LAHF-SAHF POPCNT SSE3 SSE4_1 SSE4_2 SSSE3
endef
