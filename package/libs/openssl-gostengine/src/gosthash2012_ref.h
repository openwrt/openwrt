/*
 * Portable implementation of core functions for GOST R 34.11-2012.
 *
 * Copyright (c) 2013 Cryptocom LTD.
 * This file is distributed under the same license as OpenSSL.
 *
 * Author: Alexey Degtyarev <alexey@renatasystems.org>
 *
 */

#ifdef __GOST3411_HAS_SSE2__
# error "GOST R 34.11-2012: portable implementation disabled in config.h"
#endif

#define X(x, y, z) { \
    z->QWORD[0] = x->QWORD[0] ^ y->QWORD[0]; \
    z->QWORD[1] = x->QWORD[1] ^ y->QWORD[1]; \
    z->QWORD[2] = x->QWORD[2] ^ y->QWORD[2]; \
    z->QWORD[3] = x->QWORD[3] ^ y->QWORD[3]; \
    z->QWORD[4] = x->QWORD[4] ^ y->QWORD[4]; \
    z->QWORD[5] = x->QWORD[5] ^ y->QWORD[5]; \
    z->QWORD[6] = x->QWORD[6] ^ y->QWORD[6]; \
    z->QWORD[7] = x->QWORD[7] ^ y->QWORD[7]; \
}

#ifndef __GOST3411_BIG_ENDIAN__
# define __XLPS_FOR for (_i = 0; _i <= 7; _i++)
# define _datai _i
#else
# define __XLPS_FOR for (_i = 7; _i >= 0; _i--)
# define _datai 7 - _i
#endif

#define XLPS(x, y, data) { \
    register unsigned long long r0, r1, r2, r3, r4, r5, r6, r7; \
    int _i; \
    \
    r0 = x->QWORD[0] ^ y->QWORD[0]; \
    r1 = x->QWORD[1] ^ y->QWORD[1]; \
    r2 = x->QWORD[2] ^ y->QWORD[2]; \
    r3 = x->QWORD[3] ^ y->QWORD[3]; \
    r4 = x->QWORD[4] ^ y->QWORD[4]; \
    r5 = x->QWORD[5] ^ y->QWORD[5]; \
    r6 = x->QWORD[6] ^ y->QWORD[6]; \
    r7 = x->QWORD[7] ^ y->QWORD[7]; \
    \
    \
    __XLPS_FOR {\
        data->QWORD[_datai]  = Ax[0][(r0 >> (_i << 3)) & 0xFF]; \
        data->QWORD[_datai] ^= Ax[1][(r1 >> (_i << 3)) & 0xFF]; \
        data->QWORD[_datai] ^= Ax[2][(r2 >> (_i << 3)) & 0xFF]; \
        data->QWORD[_datai] ^= Ax[3][(r3 >> (_i << 3)) & 0xFF]; \
        data->QWORD[_datai] ^= Ax[4][(r4 >> (_i << 3)) & 0xFF]; \
        data->QWORD[_datai] ^= Ax[5][(r5 >> (_i << 3)) & 0xFF]; \
        data->QWORD[_datai] ^= Ax[6][(r6 >> (_i << 3)) & 0xFF]; \
        data->QWORD[_datai] ^= Ax[7][(r7 >> (_i << 3)) & 0xFF]; \
    }\
}

#define ROUND(i, Ki, data) { \
    XLPS(Ki, (&C[i]), Ki); \
    XLPS(Ki, data, data); \
}
