/*
 * compat shim for <asm/tc3162/cmdparse.h>  (TrendChip CLI command parser)
 * Pulled in transitively via epon/epon.h (kept only so the shared PonSysData
 * struct's EPON sub-fields resolve).  No cmdparse symbols are used by the
 * GPON build, so this is intentionally empty.  TODO: drop with EPON
 */
#ifndef _COMPAT_ASM_TC3162_CMDPARSE_H_
#define _COMPAT_ASM_TC3162_CMDPARSE_H_
#endif
