#ifndef HTC_DEBUG_H_
#define HTC_DEBUG_H_
/*
 *
 * Copyright (c) 2004-2007 Atheros Communications Inc.
 * All rights reserved.
 *
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation;
 *
 *  Software distributed under the License is distributed on an "AS
 *  IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
 *  implied. See the License for the specific language governing
 *  rights and limitations under the License.
 *
 *
 *
 */

/* ------- Debug related stuff ------- */
enum {
    ATH_DEBUG_SEND = 0x0001,
    ATH_DEBUG_RECV = 0x0002,
    ATH_DEBUG_SYNC = 0x0004,
    ATH_DEBUG_DUMP = 0x0008,
    ATH_DEBUG_IRQ  = 0x0010,
    ATH_DEBUG_TRC  = 0x0020,
    ATH_DEBUG_WARN = 0x0040,
    ATH_DEBUG_ERR  = 0x0080,
    ATH_DEBUG_ANY  = 0xFFFF,
};

#ifdef DEBUG

// TODO FIX usage of A_PRINTF!
#define AR_DEBUG_LVL_CHECK(lvl) (debughtc & (lvl))
#define AR_DEBUG_PRINTBUF(buffer, length, desc) do {   \
    if (debughtc & ATH_DEBUG_DUMP) {             \
        DebugDumpBytes(buffer, length,desc);               \
    }                                            \
} while(0)
#define PRINTX_ARG(arg...) arg
#define AR_DEBUG_PRINTF(flags, args) do {        \
    if (debughtc & (flags)) {                    \
        A_PRINTF(KERN_ALERT PRINTX_ARG args);    \
    }                                            \
} while (0)
#define AR_DEBUG_ASSERT(test) do {               \
    if (!(test)) {                               \
        AR_DEBUG_PRINTF(ATH_DEBUG_ERR, ("Debug Assert Caught, File %s, Line: %d, Test:%s \n",__FILE__, __LINE__,#test));         \
    }                                            \
} while(0)
extern int debughtc;
#else
#define AR_DEBUG_PRINTF(flags, args)
#define AR_DEBUG_PRINTBUF(buffer, length, desc)
#define AR_DEBUG_ASSERT(test)
#define AR_DEBUG_LVL_CHECK(lvl) 0
#endif

void DebugDumpBytes(A_UCHAR *buffer, A_UINT16 length, char *pDescription);

#endif /*HTC_DEBUG_H_*/
