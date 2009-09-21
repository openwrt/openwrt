#ifndef __REGDUMP_H__
#define __REGDUMP_H__
/*
 * Copyright (c) 2004-2007 Atheros Communications Inc.
 * All rights reserved.
 *
 * $ATH_LICENSE_HOSTSDK0_C$
 *
 */
#if defined(AR6001)
#include "AR6001/AR6001_regdump.h"
#endif
#if defined(AR6002)
#include "AR6002/AR6002_regdump.h"
#endif

#if !defined(__ASSEMBLER__)
/*
 * Target CPU state at the time of failure is reflected
 * in a register dump, which the Host can fetch through
 * the diagnostic window.
 */
struct register_dump_s {
    A_UINT32 target_id;               /* Target ID */
    A_UINT32 assline;                 /* Line number (if assertion failure) */
    A_UINT32 pc;                      /* Program Counter at time of exception */
    A_UINT32 badvaddr;                /* Virtual address causing exception */
    CPU_exception_frame_t exc_frame;  /* CPU-specific exception info */

    /* Could copy top of stack here, too.... */
};
#endif /* __ASSEMBLER__ */
#endif /* __REGDUMP_H__ */
