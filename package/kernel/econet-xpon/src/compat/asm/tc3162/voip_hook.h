/*
 * compat shim for <asm/tc3162/voip_hook.h>
 * Only pulled in under TCSUPPORT_VOIP && RTP_RX_SHORTCUT, which are NOT defined
 * in the GPON-only build.  Empty shim keeps the #include resolvable.
 */
#ifndef _COMPAT_ASM_TC3162_VOIP_HOOK_H_
#define _COMPAT_ASM_TC3162_VOIP_HOOK_H_
#endif
