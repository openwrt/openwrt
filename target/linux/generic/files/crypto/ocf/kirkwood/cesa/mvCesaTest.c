/*******************************************************************************
Copyright (C) Marvell International Ltd. and its affiliates

This software file (the "File") is owned and distributed by Marvell
International Ltd. and/or its affiliates ("Marvell") under the following
alternative licensing terms.  Once you have made an election to distribute the
File under one of the following license alternatives, please (i) delete this
introductory statement regarding license alternatives, (ii) delete the two
license alternatives that you have not elected to use and (iii) preserve the
Marvell copyright notice above.

********************************************************************************
Marvell Commercial License Option

If you received this File from Marvell and you have entered into a commercial
license agreement (a "Commercial License") with Marvell, the File is licensed
to you under the terms of the applicable Commercial License.

********************************************************************************
Marvell GPL License Option

If you received this File from Marvell, you may opt to use, redistribute and/or
modify this File in accordance with the terms and conditions of the General
Public License Version 2, June 1991 (the "GPL License"), a copy of which is
available along with the File in the license.txt file or by writing to the Free
Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 or
on the worldwide web at http://www.gnu.org/licenses/gpl.txt.

THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED
WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY
DISCLAIMED.  The GPL License provides additional details about this warranty
disclaimer.
********************************************************************************
Marvell BSD License Option

If you received this File from Marvell, you may opt to use, redistribute and/or
modify this File under the following licensing terms.
Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    *   Redistributions of source code must retain the above copyright notice,
        this list of conditions and the following disclaimer.

    *   Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.

    *   Neither the name of Marvell nor the names of its contributors may be
        used to endorse or promote products derived from this software without
        specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/

#include "mvOs.h"

#if defined(MV_VXWORKS)

#include "sysLib.h"
#include "logLib.h"
#include "tickLib.h"
#include "intLib.h"
#include "config.h"


SEM_ID      cesaSemId = NULL;
SEM_ID      cesaWaitSemId = NULL;

#define CESA_TEST_LOCK(flags)       flags = intLock()
#define CESA_TEST_UNLOCK(flags)     intUnlock(flags)

#define CESA_TEST_WAIT_INIT()       cesaWaitSemId = semBCreate(SEM_Q_PRIORITY, SEM_EMPTY)
#define CESA_TEST_WAKE_UP()         semGive(cesaWaitSemId)
#define CESA_TEST_WAIT(cond, ms)    semTake(cesaWaitSemId, (sysClkRateGet()*ms)/1000)

#define CESA_TEST_TICK_GET()        tickGet()
#define CESA_TEST_TICK_TO_MS(tick)  (((tick)*1000)/sysClkRateGet())

#elif defined(MV_LINUX)

#include <linux/wait.h>
wait_queue_head_t   cesaTest_waitq;
spinlock_t          cesaLock;

#define CESA_TEST_LOCK(flags)       spin_lock_irqsave( &cesaLock, flags)
#define CESA_TEST_UNLOCK(flags)     spin_unlock_irqrestore( &cesaLock, flags);

#define CESA_TEST_WAIT_INIT()       init_waitqueue_head(&cesaTest_waitq)
#define CESA_TEST_WAKE_UP()         wake_up(&cesaTest_waitq)
#define CESA_TEST_WAIT(cond, ms)    wait_event_timeout(cesaTest_waitq, (cond), msecs_to_jiffies(ms))

#define CESA_TEST_TICK_GET()        jiffies
#define CESA_TEST_TICK_TO_MS(tick)  jiffies_to_msecs(tick)

#elif defined(MV_NETBSD)

#include <sys/param.h>
#include <sys/kernel.h>
static int	cesaLock;

#define	CESA_TEST_LOCK(flags)		flags = splnet()
#define	CESA_TEST_UNLOCK(flags)		splx(flags)

#define	CESA_TEST_WAIT_INIT()		/* nothing */
#define	CESA_TEST_WAKE_UP()		wakeup(&cesaLock)
#define	CESA_TEST_WAIT(cond, ms)	\
do {					\
	while (!(cond))			\
		tsleep(&cesaLock, PWAIT, "cesatest",mstohz(ms)); \
} while (/*CONSTCOND*/0)

#define	CESA_TEST_TICK_GET()		hardclock_ticks
#define	CESA_TEST_TICK_TO_MS(tick)	((1000/hz)*(tick))

#define	request_irq(i,h,t,n,a)	\
	!mv_intr_establish((i),IPL_NET,(int(*)(void *))(h),(a))

#else
#error "Only Linux, VxWorks, or NetBSD OS are supported"
#endif

#include "mvDebug.h"

#include "mvSysHwConfig.h"
#include "boardEnv/mvBoardEnvLib.h"
#include "ctrlEnv/sys/mvCpuIf.h"
#include "cntmr/mvCntmr.h"
#include "cesa/mvCesa.h"
#include "cesa/mvCesaRegs.h"
#include "cesa/mvMD5.h"
#include "cesa/mvSHA1.h"

#if defined(CONFIG_MV646xx)
#include "marvell_pic.h"
#endif

#define MV_CESA_USE_TIMER_ID    0
#define CESA_DEF_BUF_SIZE       1500
#define CESA_DEF_BUF_NUM        1
#define CESA_DEF_SESSION_NUM    32

#define CESA_DEF_ITER_NUM       100

#define CESA_DEF_REQ_SIZE       256


/* CESA Tests Debug */
#undef CESA_TEST_DEBUG

#ifdef CESA_TEST_DEBUG

#   define CESA_TEST_DEBUG_PRINT(msg)   mvOsPrintf msg
#   define CESA_TEST_DEBUG_CODE(code)   code

typedef struct
{
    int             type;       /* 0 - isrEmpty, 1 - cesaReadyGet, 2 - cesaAction */
    MV_U32          timeStamp;
    MV_U32          cause;
    MV_U32          realCause;
    MV_U32          dmaCause;
    int             resources;
    MV_CESA_REQ*    pReqReady;
    MV_CESA_REQ*    pReqEmpty;
    MV_CESA_REQ*    pReqProcess;
} MV_CESA_TEST_TRACE;

#define MV_CESA_TEST_TRACE_SIZE      25

static int cesaTestTraceIdx = 0;
static MV_CESA_TEST_TRACE    cesaTestTrace[MV_CESA_TEST_TRACE_SIZE];

static void cesaTestTraceAdd(int type, MV_U32 cause)
{
    cesaTestTrace[cesaTestTraceIdx].type = type;
    cesaTestTrace[cesaTestTraceIdx].cause = cause;
    cesaTestTrace[cesaTestTraceIdx].realCause = MV_REG_READ(MV_CESA_ISR_CAUSE_REG);
    cesaTestTrace[cesaTestTraceIdx].dmaCause = MV_REG_READ(IDMA_CAUSE_REG);
    cesaTestTrace[cesaTestTraceIdx].resources = cesaReqResources;
    cesaTestTrace[cesaTestTraceIdx].pReqReady = pCesaReqReady;
    cesaTestTrace[cesaTestTraceIdx].pReqEmpty = pCesaReqEmpty;
    cesaTestTrace[cesaTestTraceIdx].pReqProcess = pCesaReqProcess;
    cesaTestTrace[cesaTestTraceIdx].timeStamp = mvCntmrRead(MV_CESA_USE_TIMER_ID);
    cesaTestTraceIdx++;
    if(cesaTestTraceIdx == MV_CESA_TEST_TRACE_SIZE)
        cesaTestTraceIdx = 0;
}

#else

#   define CESA_TEST_DEBUG_PRINT(msg)
#   define CESA_TEST_DEBUG_CODE(code)

#endif /* CESA_TEST_DEBUG */

int                 cesaExpReqId=0;
int                 cesaCbIter=0;

int                 cesaIdx;
int                 cesaIteration;
int                 cesaRateSize;
int                 cesaReqSize;
unsigned long       cesaTaskId;
int                 cesaBufNum;
int                 cesaBufSize;
int                 cesaCheckOffset;
int                 cesaCheckSize;
int                 cesaCheckMode;
int                 cesaTestIdx;
int                 cesaCaseIdx;


MV_U32      cesaTestIsrCount = 0;
MV_U32      cesaTestIsrMissCount = 0;

MV_U32      cesaCryptoError = 0;
MV_U32      cesaReqIdError  = 0;
MV_U32      cesaError = 0;

char*       cesaHexBuffer = NULL;

char*       cesaBinBuffer = NULL;
char*       cesaExpBinBuffer = NULL;

char*       cesaInputHexStr  = NULL;
char*       cesaOutputHexStr = NULL;

MV_BUF_INFO         cesaReqBufs[CESA_DEF_REQ_SIZE];

MV_CESA_COMMAND*    cesaCmdRing;
MV_CESA_RESULT      cesaResult;

int                 cesaTestFull = 0;

MV_BOOL             cesaIsReady = MV_FALSE;
MV_U32              cesaCycles = 0;
MV_U32              cesaBeginTicks = 0;
MV_U32              cesaEndTicks = 0;
MV_U32              cesaRate = 0;
MV_U32              cesaRateAfterDot = 0;

void 		    *cesaTestOSHandle = NULL;

enum
{
    CESA_FAST_CHECK_MODE = 0,
    CESA_FULL_CHECK_MODE,
    CESA_NULL_CHECK_MODE,
    CESA_SHOW_CHECK_MODE,
    CESA_SW_SHOW_CHECK_MODE,
    CESA_SW_NULL_CHECK_MODE,

    CESA_MAX_CHECK_MODE
};

enum
{
    DES_TEST_TYPE         = 0,
    TRIPLE_DES_TEST_TYPE  = 1,
    AES_TEST_TYPE         = 2,
    MD5_TEST_TYPE         = 3,
    SHA_TEST_TYPE         = 4,
    COMBINED_TEST_TYPE    = 5,

    MAX_TEST_TYPE
};

/* Tests data base */
typedef struct
{
    short           sid;
    char            cryptoAlgorithm;    /* DES/3DES/AES */
    char            cryptoMode;         /* ECB or CBC */
    char            macAlgorithm;       /* MD5 / SHA1 */
    char            operation;          /* CRYPTO/HMAC/CRYPTO+HMAC/HMAC+CRYPTO */
    char            direction;          /* ENCODE(SIGN)/DECODE(VERIFY) */
    unsigned char*  pCryptoKey;
    int             cryptoKeySize;
    unsigned char*  pMacKey;
    int             macKeySize;
    const char*     name;

} MV_CESA_TEST_SESSION;

typedef struct
{
    MV_CESA_TEST_SESSION*   pSessions;
    int                     numSessions;

} MV_CESA_TEST_DB_ENTRY;

typedef struct
{
    char*           plainHexStr;
    char*           cipherHexStr;
    unsigned char*  pCryptoIV;
    int             cryptoLength;
    int             macLength;
    int             digestOffset;

} MV_CESA_TEST_CASE;

typedef struct
{
    int     size;
    const char* outputHexStr;

} MV_CESA_SIZE_TEST;

static unsigned char    cryptoKey1[] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
                                        0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
                                        0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef};

static unsigned char    cryptoKey7[] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef};
static unsigned char    iv1[]        = {0x12, 0x34, 0x56, 0x78, 0x90, 0xab, 0xcd, 0xef};


static unsigned char    cryptoKey2[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};

static unsigned char    cryptoKey3[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
                                        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};

static unsigned char    cryptoKey4[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
                                        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                                        0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f};

static unsigned char    cryptoKey5[] = {0x56, 0xe4, 0x7a, 0x38, 0xc5, 0x59, 0x89, 0x74,
                                        0xbc, 0x46, 0x90, 0x3d, 0xba, 0x29, 0x03, 0x49};


static unsigned char    key3des1[]   = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF,
                                        0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0x01,
                                        0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0x01, 0x23};

/*  Input ASCII string: The quick brown fox jump  */
static char  plain3des1[]           =   "54686520717566636B2062726F776E20666F78206A756D70";
static char  cipher3des1[]          =   "A826FD8CE53B855FCCE21C8112256FE668D5C05DD9B6B900";

static unsigned char    key3des2[]  = {0x62, 0x7f, 0x46, 0x0e, 0x08, 0x10, 0x4a, 0x10,
                                       0x43, 0xcd, 0x26, 0x5d, 0x58, 0x40, 0xea, 0xf1,
                                       0x31, 0x3e, 0xdf, 0x97, 0xdf, 0x2a, 0x8a, 0x8c};

static unsigned char    iv3des2[]   = {0x8e, 0x29, 0xf7, 0x5e, 0xa7, 0x7e, 0x54, 0x75};

static char  plain3des2[]           = "326a494cd33fe756";

static char  cipher3desCbc2[]       = "8e29f75ea77e5475"
                                      "b22b8d66de970692";

static unsigned char    key3des3[]  = {0x37, 0xae, 0x5e, 0xbf, 0x46, 0xdf, 0xf2, 0xdc,
                                       0x07, 0x54, 0xb9, 0x4f, 0x31, 0xcb, 0xb3, 0x85,
                                       0x5e, 0x7f, 0xd3, 0x6d, 0xc8, 0x70, 0xbf, 0xae};

static unsigned char    iv3des3[]   = {0x3d, 0x1d, 0xe3, 0xcc, 0x13, 0x2e, 0x3b, 0x65};

static char  plain3des3[]           = "84401f78fe6c10876d8ea23094ea5309";

static char  cipher3desCbc3[]       = "3d1de3cc132e3b65"
                                      "7b1f7c7e3b1c948ebd04a75ffba7d2f5";

static unsigned char    iv5[]        = {0x8c, 0xe8, 0x2e, 0xef, 0xbe, 0xa0, 0xda, 0x3c,
                                        0x44, 0x69, 0x9e, 0xd7, 0xdb, 0x51, 0xb7, 0xd9};

static unsigned char    aesCtrKey[]  = {0x76, 0x91, 0xBE, 0x03, 0x5E, 0x50, 0x20, 0xA8,
                                        0xAC, 0x6E, 0x61, 0x85, 0x29, 0xF9, 0xA0, 0xDC};

static unsigned char    mdKey1[]     = {0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b,
                                        0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b};

static unsigned char    mdKey2[]     = {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
                                        0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa};

static unsigned char    shaKey1[]    = {0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b,
                                        0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b,
                                        0x0b, 0x0b, 0x0b, 0x0b};

static unsigned char    shaKey2[]    = {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
                                        0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
                                        0xaa, 0xaa, 0xaa, 0xaa};

static unsigned char    mdKey4[]     = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                                        0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10};

static unsigned char    shaKey4[]    = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                                        0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10,
                                        0x11, 0x12, 0x13, 0x14};


static MV_CESA_TEST_SESSION   desTestSessions[] =
{
/*000*/ {-1, MV_CESA_CRYPTO_DES, MV_CESA_CRYPTO_ECB,
             MV_CESA_MAC_NULL, MV_CESA_CRYPTO_ONLY,
             MV_CESA_DIR_ENCODE,
             cryptoKey7, sizeof(cryptoKey7)/sizeof(cryptoKey7[0]),
             NULL, 0,
             "DES ECB encode",
        },
/*001*/ {-1, MV_CESA_CRYPTO_DES, MV_CESA_CRYPTO_ECB,
             MV_CESA_MAC_NULL, MV_CESA_CRYPTO_ONLY,
             MV_CESA_DIR_DECODE,
             cryptoKey7, sizeof(cryptoKey7)/sizeof(cryptoKey7[0]),
             NULL, 0,
             "DES ECB decode",
        },
/*002*/ {-1, MV_CESA_CRYPTO_DES, MV_CESA_CRYPTO_CBC,
             MV_CESA_MAC_NULL, MV_CESA_CRYPTO_ONLY,
             MV_CESA_DIR_ENCODE,
             cryptoKey7, sizeof(cryptoKey7)/sizeof(cryptoKey7[0]),
             NULL, 0,
             "DES CBC encode"
        },
/*003*/ {-1, MV_CESA_CRYPTO_DES, MV_CESA_CRYPTO_CBC,
             MV_CESA_MAC_NULL, MV_CESA_CRYPTO_ONLY,
             MV_CESA_DIR_DECODE,
             cryptoKey7, sizeof(cryptoKey7)/sizeof(cryptoKey7[0]),
             NULL, 0,
             "DES CBC decode"
        },
/*004*/ {-1, MV_CESA_CRYPTO_NULL, MV_CESA_CRYPTO_ECB,
             MV_CESA_MAC_NULL, MV_CESA_CRYPTO_ONLY,
             MV_CESA_DIR_ENCODE,
             NULL, 0, NULL, 0,
              "NULL Crypto Algorithm encode"
        },
};


static MV_CESA_TEST_SESSION   tripleDesTestSessions[] =
{
/*100*/ {-1, MV_CESA_CRYPTO_3DES, MV_CESA_CRYPTO_ECB,
             MV_CESA_MAC_NULL, MV_CESA_CRYPTO_ONLY,
             MV_CESA_DIR_ENCODE,
             cryptoKey1, sizeof(cryptoKey1)/sizeof(cryptoKey1[0]),
             NULL, 0,
             "3DES ECB encode",
        },
/*101*/ {-1, MV_CESA_CRYPTO_3DES, MV_CESA_CRYPTO_ECB,
             MV_CESA_MAC_NULL, MV_CESA_CRYPTO_ONLY,
             MV_CESA_DIR_DECODE,
             cryptoKey1, sizeof(cryptoKey1)/sizeof(cryptoKey1[0]),
             NULL, 0,
             "3DES ECB decode",
        },
/*102*/ {-1, MV_CESA_CRYPTO_3DES, MV_CESA_CRYPTO_CBC,
             MV_CESA_MAC_NULL, MV_CESA_CRYPTO_ONLY,
             MV_CESA_DIR_ENCODE,
             cryptoKey1, sizeof(cryptoKey1)/sizeof(cryptoKey1[0]),
             NULL, 0,
             "3DES CBC encode"
        },
/*103*/ {-1, MV_CESA_CRYPTO_3DES, MV_CESA_CRYPTO_CBC,
             MV_CESA_MAC_NULL, MV_CESA_CRYPTO_ONLY,
             MV_CESA_DIR_DECODE,
             cryptoKey1, sizeof(cryptoKey1)/sizeof(cryptoKey1[0]),
             NULL, 0,
             "3DES CBC decode"
        },
/*104*/ {-1, MV_CESA_CRYPTO_3DES, MV_CESA_CRYPTO_ECB,
             MV_CESA_MAC_NULL, MV_CESA_CRYPTO_ONLY,
             MV_CESA_DIR_ENCODE,
             key3des1, sizeof(key3des1),
             NULL, 0,
             "3DES ECB encode"
        },
/*105*/ {-1, MV_CESA_CRYPTO_3DES, MV_CESA_CRYPTO_CBC,
             MV_CESA_MAC_NULL, MV_CESA_CRYPTO_ONLY,
             MV_CESA_DIR_ENCODE,
             key3des2, sizeof(key3des2),
             NULL, 0,
             "3DES ECB encode"
        },
/*106*/ {-1, MV_CESA_CRYPTO_3DES, MV_CESA_CRYPTO_CBC,
             MV_CESA_MAC_NULL, MV_CESA_CRYPTO_ONLY,
             MV_CESA_DIR_ENCODE,
             key3des3, sizeof(key3des3),
             NULL, 0,
             "3DES ECB encode"
        },
};


static MV_CESA_TEST_SESSION   aesTestSessions[] =
{
/*200*/ {-1, MV_CESA_CRYPTO_AES, MV_CESA_CRYPTO_ECB,
             MV_CESA_MAC_NULL, MV_CESA_CRYPTO_ONLY,
             MV_CESA_DIR_ENCODE,
             cryptoKey2, sizeof(cryptoKey2)/sizeof(cryptoKey2[0]),
             NULL, 0,
             "AES-128 ECB encode"
        },
/*201*/ {-1, MV_CESA_CRYPTO_AES, MV_CESA_CRYPTO_ECB,
             MV_CESA_MAC_NULL, MV_CESA_CRYPTO_ONLY,
             MV_CESA_DIR_DECODE,
             cryptoKey2, sizeof(cryptoKey2)/sizeof(cryptoKey2[0]),
             NULL, 0,
             "AES-128 ECB decode"
        },
/*202*/ {-1, MV_CESA_CRYPTO_AES, MV_CESA_CRYPTO_CBC,
             MV_CESA_MAC_NULL, MV_CESA_CRYPTO_ONLY,
             MV_CESA_DIR_ENCODE,
             cryptoKey5, sizeof(cryptoKey5)/sizeof(cryptoKey5[0]),
             NULL, 0,
             "AES-128 CBC encode"
        },
/*203*/ {-1, MV_CESA_CRYPTO_AES, MV_CESA_CRYPTO_CBC,
             MV_CESA_MAC_NULL, MV_CESA_CRYPTO_ONLY,
             MV_CESA_DIR_DECODE,
             cryptoKey5, sizeof(cryptoKey5)/sizeof(cryptoKey5[0]),
             NULL, 0,
             "AES-128 CBC decode"
        },
/*204*/ {-1, MV_CESA_CRYPTO_AES, MV_CESA_CRYPTO_ECB,
             MV_CESA_MAC_NULL, MV_CESA_CRYPTO_ONLY,
             MV_CESA_DIR_ENCODE,
             cryptoKey3, sizeof(cryptoKey3)/sizeof(cryptoKey3[0]),
             NULL, 0,
             "AES-192 ECB encode"
        },
/*205*/ {-1, MV_CESA_CRYPTO_AES, MV_CESA_CRYPTO_ECB,
             MV_CESA_MAC_NULL, MV_CESA_CRYPTO_ONLY,
             MV_CESA_DIR_DECODE,
             cryptoKey3, sizeof(cryptoKey3)/sizeof(cryptoKey3[0]),
             NULL, 0,
             "AES-192 ECB decode"
        },
/*206*/ {-1, MV_CESA_CRYPTO_AES, MV_CESA_CRYPTO_ECB,
             MV_CESA_MAC_NULL, MV_CESA_CRYPTO_ONLY,
             MV_CESA_DIR_ENCODE,
             cryptoKey4, sizeof(cryptoKey4)/sizeof(cryptoKey4[0]),
             NULL, 0,
             "AES-256 ECB encode"
        },
/*207*/ {-1, MV_CESA_CRYPTO_AES, MV_CESA_CRYPTO_ECB,
             MV_CESA_MAC_NULL, MV_CESA_CRYPTO_ONLY,
             MV_CESA_DIR_DECODE,
             cryptoKey4, sizeof(cryptoKey4)/sizeof(cryptoKey4[0]),
             NULL, 0,
             "AES-256 ECB decode"
        },
/*208*/ {-1, MV_CESA_CRYPTO_AES, MV_CESA_CRYPTO_CTR,
             MV_CESA_MAC_NULL, MV_CESA_CRYPTO_ONLY,
             MV_CESA_DIR_ENCODE,
             aesCtrKey, sizeof(aesCtrKey)/sizeof(aesCtrKey[0]),
             NULL, 0,
             "AES-128 CTR encode"
        },
};


static MV_CESA_TEST_SESSION   md5TestSessions[] =
{
/*300*/ {-1, MV_CESA_CRYPTO_NULL, MV_CESA_CRYPTO_ECB,
             MV_CESA_MAC_HMAC_MD5, MV_CESA_MAC_ONLY,
             MV_CESA_DIR_ENCODE,
             NULL, 0,
             mdKey1, sizeof(mdKey1),
             "HMAC-MD5 Generate Signature"
        },
/*301*/ {-1, MV_CESA_CRYPTO_NULL, MV_CESA_CRYPTO_ECB,
             MV_CESA_MAC_HMAC_MD5, MV_CESA_MAC_ONLY,
             MV_CESA_DIR_DECODE,
             NULL, 0,
             mdKey1, sizeof(mdKey1),
             "HMAC-MD5 Verify Signature"
        },
/*302*/ {-1, MV_CESA_CRYPTO_NULL, MV_CESA_CRYPTO_ECB,
             MV_CESA_MAC_HMAC_MD5, MV_CESA_MAC_ONLY,
             MV_CESA_DIR_ENCODE,
             NULL, 0,
             mdKey2, sizeof(mdKey2),
             "HMAC-MD5 Generate Signature"
        },
/*303*/ {-1, MV_CESA_CRYPTO_NULL, MV_CESA_CRYPTO_ECB,
             MV_CESA_MAC_HMAC_MD5, MV_CESA_MAC_ONLY,
             MV_CESA_DIR_DECODE,
             NULL, 0,
             mdKey2, sizeof(mdKey2),
             "HMAC-MD5 Verify Signature"
        },
/*304*/ {-1, MV_CESA_CRYPTO_NULL, MV_CESA_CRYPTO_ECB,
             MV_CESA_MAC_HMAC_MD5, MV_CESA_MAC_ONLY,
             MV_CESA_DIR_ENCODE,
             NULL, 0,
             mdKey4, sizeof(mdKey4),
             "HMAC-MD5 Generate Signature"
        },
/*305*/ {-1, MV_CESA_CRYPTO_NULL, MV_CESA_CRYPTO_ECB,
             MV_CESA_MAC_MD5, MV_CESA_MAC_ONLY,
             MV_CESA_DIR_ENCODE,
             NULL, 0,
             NULL, 0,
             "HASH-MD5 Generate Signature"
        },
};


static MV_CESA_TEST_SESSION   shaTestSessions[] =
{
/*400*/ {-1, MV_CESA_CRYPTO_NULL, MV_CESA_CRYPTO_ECB,
             MV_CESA_MAC_HMAC_SHA1, MV_CESA_MAC_ONLY,
             MV_CESA_DIR_ENCODE,
             NULL, 0,
             shaKey1, sizeof(shaKey1),
             "HMAC-SHA1 Generate Signature"
        },
/*401*/ {-1, MV_CESA_CRYPTO_NULL, MV_CESA_CRYPTO_ECB,
             MV_CESA_MAC_HMAC_SHA1, MV_CESA_MAC_ONLY,
             MV_CESA_DIR_DECODE,
             NULL, 0,
             shaKey1, sizeof(shaKey1),
             "HMAC-SHA1 Verify Signature"
        },
/*402*/ {-1, MV_CESA_CRYPTO_NULL, MV_CESA_CRYPTO_ECB,
             MV_CESA_MAC_HMAC_SHA1, MV_CESA_MAC_ONLY,
             MV_CESA_DIR_ENCODE,
             NULL, 0,
             shaKey2, sizeof(shaKey2),
             "HMAC-SHA1 Generate Signature"
        },
/*403*/ {-1, MV_CESA_CRYPTO_NULL, MV_CESA_CRYPTO_ECB,
             MV_CESA_MAC_HMAC_SHA1, MV_CESA_MAC_ONLY,
             MV_CESA_DIR_DECODE,
             NULL, 0,
             shaKey2, sizeof(shaKey2),
             "HMAC-SHA1 Verify Signature"
        },
/*404*/ {-1, MV_CESA_CRYPTO_NULL, MV_CESA_CRYPTO_ECB,
             MV_CESA_MAC_HMAC_SHA1, MV_CESA_MAC_ONLY,
             MV_CESA_DIR_ENCODE,
             NULL, 0,
             shaKey4, sizeof(shaKey4),
             "HMAC-SHA1 Generate Signature"
        },
/*405*/ {-1, MV_CESA_CRYPTO_NULL, MV_CESA_CRYPTO_ECB,
             MV_CESA_MAC_SHA1, MV_CESA_MAC_ONLY,
             MV_CESA_DIR_ENCODE,
             NULL, 0,
             NULL, 0,
             "HASH-SHA1 Generate Signature"
        },
};

static MV_CESA_TEST_SESSION   combinedTestSessions[] =
{
/*500*/ {-1, MV_CESA_CRYPTO_DES, MV_CESA_CRYPTO_ECB,
             MV_CESA_MAC_HMAC_MD5, MV_CESA_CRYPTO_THEN_MAC,
             MV_CESA_DIR_ENCODE,
             cryptoKey1, MV_CESA_DES_KEY_LENGTH,
             mdKey4, sizeof(mdKey4),
             "DES + MD5 encode"
        },
/*501*/ {-1, MV_CESA_CRYPTO_DES, MV_CESA_CRYPTO_ECB,
             MV_CESA_MAC_HMAC_SHA1, MV_CESA_CRYPTO_THEN_MAC,
             MV_CESA_DIR_ENCODE,
             cryptoKey1, MV_CESA_DES_KEY_LENGTH,
             shaKey4, sizeof(shaKey4),
             "DES + SHA1 encode"
        },
/*502*/ {-1, MV_CESA_CRYPTO_3DES, MV_CESA_CRYPTO_ECB,
             MV_CESA_MAC_HMAC_MD5, MV_CESA_CRYPTO_THEN_MAC,
             MV_CESA_DIR_ENCODE,
             cryptoKey1, sizeof(cryptoKey1)/sizeof(cryptoKey1[0]),
             mdKey4, sizeof(mdKey4),
             "3DES + MD5 encode"
        },
/*503*/ {-1, MV_CESA_CRYPTO_3DES, MV_CESA_CRYPTO_ECB,
             MV_CESA_MAC_HMAC_SHA1, MV_CESA_CRYPTO_THEN_MAC,
             MV_CESA_DIR_ENCODE,
             cryptoKey1, sizeof(cryptoKey1)/sizeof(cryptoKey1[0]),
             shaKey4, sizeof(shaKey4),
             "3DES + SHA1 encode"
        },
/*504*/ {-1, MV_CESA_CRYPTO_3DES, MV_CESA_CRYPTO_CBC,
             MV_CESA_MAC_HMAC_MD5, MV_CESA_CRYPTO_THEN_MAC,
             MV_CESA_DIR_ENCODE,
             cryptoKey1, sizeof(cryptoKey1)/sizeof(cryptoKey1[0]),
             mdKey4, sizeof(mdKey4),
             "3DES CBC + MD5 encode"
        },
/*505*/ {-1, MV_CESA_CRYPTO_3DES, MV_CESA_CRYPTO_CBC,
             MV_CESA_MAC_HMAC_SHA1, MV_CESA_CRYPTO_THEN_MAC,
             MV_CESA_DIR_ENCODE,
             cryptoKey1, sizeof(cryptoKey1)/sizeof(cryptoKey1[0]),
             shaKey4, sizeof(shaKey4),
             "3DES CBC + SHA1 encode"
        },
/*506*/ {-1, MV_CESA_CRYPTO_AES, MV_CESA_CRYPTO_CBC,
             MV_CESA_MAC_HMAC_MD5, MV_CESA_CRYPTO_THEN_MAC,
             MV_CESA_DIR_ENCODE,
             cryptoKey5, sizeof(cryptoKey5)/sizeof(cryptoKey5[0]),
             mdKey4, sizeof(mdKey4),
             "AES-128 CBC + MD5 encode"
        },
/*507*/ {-1, MV_CESA_CRYPTO_AES, MV_CESA_CRYPTO_CBC,
             MV_CESA_MAC_HMAC_SHA1, MV_CESA_CRYPTO_THEN_MAC,
             MV_CESA_DIR_ENCODE,
             cryptoKey5, sizeof(cryptoKey5)/sizeof(cryptoKey5[0]),
             shaKey4, sizeof(shaKey4),
             "AES-128 CBC + SHA1 encode"
        },
/*508*/ {-1, MV_CESA_CRYPTO_3DES, MV_CESA_CRYPTO_ECB,
             MV_CESA_MAC_HMAC_MD5, MV_CESA_MAC_THEN_CRYPTO,
             MV_CESA_DIR_DECODE,
             cryptoKey1, sizeof(cryptoKey1)/sizeof(cryptoKey1[0]),
             mdKey4, sizeof(mdKey4),
             "HMAC-MD5 + 3DES decode"
        },
};


static MV_CESA_TEST_DB_ENTRY cesaTestsDB[MAX_TEST_TYPE+1] =
{
    { desTestSessions,       sizeof(desTestSessions)/sizeof(desTestSessions[0]) },
    { tripleDesTestSessions, sizeof(tripleDesTestSessions)/sizeof(tripleDesTestSessions[0]) },
    { aesTestSessions,       sizeof(aesTestSessions)/sizeof(aesTestSessions[0]) },
    { md5TestSessions,       sizeof(md5TestSessions)/sizeof(md5TestSessions[0]) },
    { shaTestSessions,       sizeof(shaTestSessions)/sizeof(shaTestSessions[0]) },
    { combinedTestSessions,  sizeof(combinedTestSessions)/sizeof(combinedTestSessions[0]) },
    { NULL,                  0 }
};


char  cesaNullPlainHexText[]   = "000000000000000000000000000000000000000000000000";

char  cesaPlainAsciiText[]     = "Now is the time for all ";
char  cesaPlainHexEbc[]        = "4e6f77206973207468652074696d6520666f7220616c6c20";
char  cesaCipherHexEcb[]       = "3fa40e8a984d48156a271787ab8883f9893d51ec4b563b53";
char  cesaPlainHexCbc[]        = "1234567890abcdef4e6f77206973207468652074696d6520666f7220616c6c20";
char  cesaCipherHexCbc[]       = "1234567890abcdefe5c7cdde872bf27c43e934008c389c0f683788499a7c05f6";

char  cesaAesPlainHexEcb[]     = "000102030405060708090a0b0c0d0e0f";
char  cesaAes128cipherHexEcb[] = "0a940bb5416ef045f1c39458c653ea5a";
char  cesaAes192cipherHexEcb[] = "0060bffe46834bb8da5cf9a61ff220ae";
char  cesaAes256cipherHexEcb[] = "5a6e045708fb7196f02e553d02c3a692";

char  cesaAsciiStr1[]          = "Hi There";
char  cesaDataHexStr1[]        = "4869205468657265";
char  cesaHmacMd5digestHex1[]  = "9294727a3638bb1c13f48ef8158bfc9d";
char  cesaHmacSha1digestHex1[] = "b617318655057264e28bc0b6fb378c8ef146be00";
char  cesaDataAndMd5digest1[]  = "48692054686572659294727a3638bb1c13f48ef8158bfc9d";
char  cesaDataAndSha1digest1[] = "4869205468657265b617318655057264e28bc0b6fb378c8ef146be00";

char  cesaAesPlainText[]       = "a0a1a2a3a4a5a6a7a8a9aaabacadaeaf"
                                 "b0b1b2b3b4b5b6b7b8b9babbbcbdbebf"
                                 "c0c1c2c3c4c5c6c7c8c9cacbcccdcecf"
                                 "d0d1d2d3d4d5d6d7d8d9dadbdcdddedf";

char  cesaAes128CipherCbc[]    = "c30e32ffedc0774e6aff6af0869f71aa"
                                 "0f3af07a9a31a9c684db207eb0ef8e4e"
                                 "35907aa632c3ffdf868bb7b29d3d46ad"
                                 "83ce9f9a102ee99d49a53e87f4c3da55";

char  cesaAesIvPlainText[]     = "8ce82eefbea0da3c44699ed7db51b7d9"
                                 "a0a1a2a3a4a5a6a7a8a9aaabacadaeaf"
                                 "b0b1b2b3b4b5b6b7b8b9babbbcbdbebf"
                                 "c0c1c2c3c4c5c6c7c8c9cacbcccdcecf"
                                 "d0d1d2d3d4d5d6d7d8d9dadbdcdddedf";

char  cesaAes128IvCipherCbc[]  = "8ce82eefbea0da3c44699ed7db51b7d9"
                                 "c30e32ffedc0774e6aff6af0869f71aa"
                                 "0f3af07a9a31a9c684db207eb0ef8e4e"
                                 "35907aa632c3ffdf868bb7b29d3d46ad"
                                 "83ce9f9a102ee99d49a53e87f4c3da55";

char  cesaAesCtrPlain[]        = "00E0017B27777F3F4A1786F000000001"
                                 "000102030405060708090A0B0C0D0E0F"
                                 "101112131415161718191A1B1C1D1E1F"
                                 "20212223";

char  cesaAesCtrCipher[]       = "00E0017B27777F3F4A1786F000000001"
                                 "C1CF48A89F2FFDD9CF4652E9EFDB72D7"
                                 "4540A42BDE6D7836D59A5CEAAEF31053"
                                 "25B2072F";



/* Input cesaHmacHex3 is '0xdd' repeated 50 times */
char  cesaHmacMd5digestHex3[]  = "56be34521d144c88dbb8c733f0e8b3f6";
char  cesaHmacSha1digestHex3[] = "125d7342b9ac11cd91a39af48aa17b4f63f175d3";
char  cesaDataHexStr3[50*2+1]          = "";
char  cesaDataAndMd5digest3[sizeof(cesaDataHexStr3)+sizeof(cesaHmacMd5digestHex3)+8*2+1] = "";
char  cesaDataAndSha1digest3[sizeof(cesaDataHexStr3)+sizeof(cesaHmacSha1digestHex3)+8*2+1] = "";

/* Ascii string is "abc" */
char hashHexStr3[] = "616263";
char hashMd5digest3[] = "900150983cd24fb0d6963f7d28e17f72";
char hashSha1digest3[] = "a9993e364706816aba3e25717850c26c9cd0d89d";

char hashHexStr80[]     = "31323334353637383930"
                          "31323334353637383930"
                          "31323334353637383930"
                          "31323334353637383930"
                          "31323334353637383930"
                          "31323334353637383930"
                          "31323334353637383930"
                          "31323334353637383930";

char hashMd5digest80[]           = "57edf4a22be3c955ac49da2e2107b67a";

char tripleDesThenMd5digest80[]  = "b7726a03aad490bd6c5a452a89a1b271";
char tripleDesThenSha1digest80[] = "b2ddeaca91030eab5b95a234ef2c0f6e738ff883";

char cbc3desThenMd5digest80[]    = "6f463057e1a90e0e91ae505b527bcec0";
char cbc3desThenSha1digest80[]   = "1b002ed050be743aa98860cf35659646bb8efcc0";

char cbcAes128ThenMd5digest80[]  = "6b6e863ac5a71d15e3e9b1c86c9ba05f";
char cbcAes128ThenSha1digest80[] = "13558472d1fc1c90dffec6e5136c7203452d509b";


static MV_CESA_TEST_CASE  cesaTestCases[] =
{
 /*     plainHexStr          cipherHexStr               IV    crypto  mac     digest */
 /*                                                           Length  Length  Offset */
 /*0*/ { NULL,               NULL,                      NULL,   0,      0,      -1  },
 /*1*/ { cesaPlainHexEbc,    cesaCipherHexEcb,          NULL,   24,     0,      -1  },
 /*2*/ { cesaPlainHexCbc,    cesaCipherHexCbc,          NULL,   24,     0,      -1  },
 /*3*/ { cesaAesPlainHexEcb, cesaAes128cipherHexEcb,    NULL,   16,     0,      -1  },
 /*4*/ { cesaAesPlainHexEcb, cesaAes192cipherHexEcb,    NULL,   16,     0,      -1  },
 /*5*/ { cesaAesPlainHexEcb, cesaAes256cipherHexEcb,    NULL,   16,     0,      -1  },
 /*6*/ { cesaDataHexStr1,    cesaHmacMd5digestHex1,     NULL,   0,      8,      -1  },
 /*7*/ { NULL,               cesaDataAndMd5digest1,     NULL,   0,      8,      -1  },
 /*8*/ { cesaDataHexStr3,    cesaHmacMd5digestHex3,     NULL,   0,      50,     -1  },
 /*9*/ { NULL,               cesaDataAndMd5digest3,     NULL,   0,      50,     -1  },
/*10*/ { cesaAesPlainText,   cesaAes128IvCipherCbc,     iv5,    64,     0,      -1  },
/*11*/ { cesaDataHexStr1,    cesaHmacSha1digestHex1,    NULL,   0,      8,      -1  },
/*12*/ { NULL,               cesaDataAndSha1digest1,    NULL,   0,      8,      -1  },
/*13*/ { cesaDataHexStr3,    cesaHmacSha1digestHex3,    NULL,   0,      50,     -1  },
/*14*/ { NULL,               cesaDataAndSha1digest3,    NULL,   0,      50,     -1  },
/*15*/ { hashHexStr3,        hashMd5digest3,            NULL,   0,      3,      -1  },
/*16*/ { hashHexStr3,        hashSha1digest3,           NULL,   0,      3,      -1  },
/*17*/ { hashHexStr80,       tripleDesThenMd5digest80,  NULL,   80,     80,     -1  },
/*18*/ { hashHexStr80,       tripleDesThenSha1digest80, NULL,   80,     80,     -1  },
/*19*/ { hashHexStr80,       cbc3desThenMd5digest80,    iv1,    80,     80,     -1  },
/*20*/ { hashHexStr80,       cbc3desThenSha1digest80,   iv1,    80,     80,     -1  },
/*21*/ { hashHexStr80,       cbcAes128ThenMd5digest80,  iv5,    80,     80,     -1  },
/*22*/ { hashHexStr80,       cbcAes128ThenSha1digest80, iv5,    80,     80,     -1  },
/*23*/ { cesaAesCtrPlain,    cesaAesCtrCipher,          NULL,   36,     0,      -1  },
/*24*/ { cesaAesIvPlainText, cesaAes128IvCipherCbc,     NULL,   64,     0,      -1  },
/*25*/ { plain3des1,         cipher3des1,               NULL,   0,      0,      -1  },
/*26*/ { plain3des2,         cipher3desCbc2,            iv3des2,0,      0,      -1  },
/*27*/ { plain3des3,         cipher3desCbc3,            iv3des3,0,      0,      -1  },
};


/* Key         = 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
 *               0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa
 * Input 0xdd repeated "size" times
 */
static MV_CESA_SIZE_TEST     mdMultiSizeTest302[] =
{
    { 80,   "7a031a640c14a4872814930b1ef3a5b2" },
    { 512,  "5488e6c5a14dc72a79f28312ca5b939b" },
    { 1000, "d00814f586a8b78a05724239d2531821" },
    { 1001, "bf07df7b7f49d3f5b5ecacd4e9e63281" },
    { 1002, "1ed4a1a802e87817a819d4e37bb4d0f7" },
    { 1003, "5972ab64a4f265ee371dac2f2f137f90" },
    { 1004, "71f95e7ec3aa7df2548e90898abdb28e" },
    { 1005, "e082790b4857fcfc266e92e59e608814" },
    { 1006, "9500f02fd8ac7fde8b10e4fece9a920d" },
    { 1336, "e42edcce57d0b75b01aa09d71427948b" },
    { 1344, "bb5454ada0deb49ba0a97ffd60f57071" },
    { 1399, "0f44d793e744b24d53f44f295082ee8c" },
    { 1400, "359de8a03a9b707928c6c60e0e8d79f1" },
    { 1401, "e913858b484cbe2b384099ea88d8855b" },
    { 1402, "d9848a164af53620e0540c1d7d87629e" },
    { 1403, "0c9ee1c2c9ef45e9b625c26cbaf3e822" },
    { 1404, "12edd4f609416e3c936170360561b064" },
    { 1405, "7fc912718a05446395345009132bf562" },
    { 1406, "882f17425e579ff0d85a91a59f308aa0" },
    { 1407, "005cae408630a2fb5db82ad9db7e59da" },
    { 1408, "64655f8b404b3fea7a3e3e609bc5088f" },
    { 1409, "4a145284a7f74e01b6bb1a0ec6a0dd80" },
    { 2048, "67caf64475650732def374ebb8bde3fd" },
    { 2049, "6c84f11f472825f7e6cd125c2981884b" },
    { 2050, "8999586754a73a99efbe4dbad2816d41" },
    { 2051, "ba6946b610e098d286bc81091659dfff" },
    { 2052, "d0afa01c92d4d13def2b024f36faed83" },
    { 3072, "61d8beac61806afa2585d74a9a0e6974" },
    { 3074, "f6501a28dcc24d1e4770505c51a87ed3" },
    { 3075, "ea4a6929be67e33e61ff475369248b73" },
    { 4048, "aa8c4d68f282a07e7385acdfa69f4bed" },
    { 4052, "afb5ed2c0e1d430ea59e59ed5ed6b18a" },
    { 4058, "9e8553f9bdd43aebe0bd729f0e600c99" },
    { 6144, "f628f3e5d183fe5cdd3a5abee39cf872" },
    { 6150, "89a3efcea9a2f25f919168ad4a1fd292" },
    { 6400, "cdd176b7fb747873efa4da5e32bdf88f" },
    { 6528, "b1d707b027354aca152c45ee559ccd3f" },
    { 8192, "c600ea4429ac47f9941f09182166e51a" },
    {16384, "16e8754bfbeb4c649218422792267a37" },
    {18432, "0fd0607521b0aa8b52219cfbe215f63e" },
    { 0, NULL },
};

/* Key         = 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
 *               0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10
 * InputHexStr = "31323334353637383930" (ASCII = "1234567890")
 */
static MV_CESA_SIZE_TEST     mdMultiSizeTest304[] =
{
    { 80,   "a456c4723fee6068530af5a2afa71627" },
    { 512,  "f85c2a2344f5de68b432208ad13e5794" },
    { 1000, "35464d6821fd4a293a41eb84e274c8c5" },
    { 1001, "c08eedbdce60cceb54bc2d732bb32c8b" },
    { 1002, "5664f71800c011cc311cb6943339c1b8" },
    { 1003, "779c723b044c585dc7802b13e8501bdc" },
    { 1004, "55e500766a2c307bc5c5fdd15e4cacd4" },
    { 1005, "d5f978954f5c38529d1679d2b714f068" },
    { 1006, "cd3efc827ce628b7281b72172693abf9" },
    { 1336, "6f04479910785878ae6335b8d1e87edf" },
    { 1344, "b6d27b50c2bce1ba2a8e1b5cc4324368" },
    { 1399, "65f70a1d4c86e5eaeb0704c8a7816795" },
    { 1400, "3394b5adc4cb3ff98843ca260a44a88a" },
    { 1401, "3a06f3582033a66a4e57e0603ce94e74" },
    { 1402, "e4d97f5ed51edc48abfa46eeb5c31752" },
    { 1403, "3d05e40b080ee3bedf293cb87b7140e7" },
    { 1404, "8cf294fc3cd153ab18dccb2a52cbf244" },
    { 1405, "d1487bd42f6edd9b4dab316631159221" },
    { 1406, "0527123b6bf6936cf5d369dc18c6c70f" },
    { 1407, "3224a06639db70212a0cd1ae1fcc570a" },
    { 1408, "a9e13335612c0356f5e2c27086e86c43" },
    { 1409, "a86d1f37d1ed8a3552e9a4f04dceea98" },
    { 2048, "396905c9b961cd0f6152abfb69c4449c" },
    { 2049, "49f39bff85d9dcf059fadb89efc4a70f" },
    { 2050, "3a2b4823bc4d0415656550226a63e34a" },
    { 2051, "dec60580d406c782540f398ad0bcc7e0" },
    { 2052, "32f76610a14310309eb748fe025081bf" },
    { 3072, "45edc1a42bf9d708a621076b63b774da" },
    { 3074, "9be1b333fe7c0c9f835fb369dc45f778" },
    { 3075, "8c06fcac7bd0e7b7a17fd6508c09a549" },
    { 4048, "0ddaef848184bf0ad98507a10f1e90e4" },
    { 4052, "81976bcaeb274223983996c137875cb8" },
    { 4058, "0b0a7a1c82bc7cbc64d8b7cd2dc2bb22" },
    { 6144, "1c24056f52725ede2dff0d7f9fc9855f" },
    { 6150, "b7f4b65681c4e43ee68ca466ca9ca4ec" },
    { 6400, "443bbaab9f7331ddd4bf11b659cd43c8" },
    { 6528, "216f44f23047cfee03a7a64f88f9a995" },
    { 8192, "ac7a993b2cad54879dba1bde63e39097" },
    { 8320, "55ed7be9682d6c0025b3221a62088d08" },
    {16384, "c6c722087653b62007aea668277175e5" },
    {18432, "f1faca8e907872c809e14ffbd85792d6" },
    { 0, NULL },
};

/* HASH-MD5
 * InputHexStr = "31323334353637383930" (ASCII = "1234567890")
 *               repeated "size" times
 */
static MV_CESA_SIZE_TEST     mdMultiSizeTest305[] =
{
    { 80,   "57edf4a22be3c955ac49da2e2107b67a" },
    { 512,  "c729ae8f0736cc377a9767a660eaa04e" },
    { 1000, "f1257a8659eb92d36fe14c6bf3852a6a" },
    { 1001, "f8a46fe8ea04fdc8c7de0e84042d3878" },
    { 1002, "da188dd67bff87d58aa3c02af2d0cc0f" },
    { 1003, "961753017feee04c9b93a8e51658a829" },
    { 1004, "dd68c4338608dcc87807a711636bf2af" },
    { 1005, "e338d567d3ce66bf69ada29658a8759b" },
    { 1006, "443c9811e8b92599b0b149e8d7ec700a" },
    { 1336, "89a98511706008ba4cbd0b4a24fa5646" },
    { 1344, "335a919805f370b9e402a62c6fe01739" },
    { 1399, "5d18d0eddcd84212fe28d812b5e80e3b" },
    { 1400, "6b695c240d2dffd0dffc99459ca76db6" },
    { 1401, "49590f61298a76719bc93a57a30136f5" },
    { 1402, "94c2999fa3ef1910a683d69b2b8476f2" },
    { 1403, "37073a02ab00ecba2645c57c228860db" },
    { 1404, "1bcd06994fce28b624f0c5fdc2dcdd2b" },
    { 1405, "11b93671a64c95079e8cf9e7cddc8b3d" },
    { 1406, "4b6695772a4c66313fa4871017d05f36" },
    { 1407, "d1539b97fbfda1c075624e958de19c5b" },
    { 1408, "b801b9b69920907cd018e8063092ede9" },
    { 1409, "b765f1406cfe78e238273ed01bbcaf7e" },
    { 2048, "1d7e2c64ac29e2b3fb4c272844ed31f5" },
    { 2049, "71d38fac49c6b1f4478d8d88447bcdd0" },
    { 2050, "141c34a5592b1bebfa731e0b23d0cdba" },
    { 2051, "c5e1853f21c59f5d6039bd13d4b380d8" },
    { 2052, "dd44a0d128b63d4b5cccd967906472d7" },
    { 3072, "37d158e33b21390822739d13db7b87fe" },
    { 3074, "aef3b209d01d39d0597fe03634bbf441" },
    { 3075, "335ffb428eabf210bada96d74d5a4012" },
    { 4048, "2434c2b43d798d2819487a886261fc64" },
    { 4052, "ac2fa84a8a33065b2e92e36432e861f8" },
    { 4058, "856781f85616c341c3533d090c1e1e84" },
    { 6144, "e5d134c652c18bf19833e115f7a82e9b" },
    { 6150, "a09a353be7795fac2401dac5601872e6" },
    { 6400, "08b9033ac6a1821398f50af75a2dbc83" },
    { 6528, "3d47aa193a8540c091e7e02f779e6751" },
    { 8192, "d3164e710c0626f6f395b38f20141cb7" },
    { 8320, "b727589d9183ff4e8491dd24466974a3" },
    {16384, "3f54d970793d2274d5b20d10a69938ac" },
    {18432, "f558511dcf81985b7a1bb57fad970531" },
    { 0, NULL },
};


/* Key         = 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
 *               0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa
 *               0xaa, 0xaa, 0xaa, 0xaa
 * InputHexStr = "31323334353637383930" (ASCII = "1234567890")
 */
static MV_CESA_SIZE_TEST     shaMultiSizeTest402[] =
{
    { 80,   "e812f370e659705a1649940d1f78cd7af18affd3" },
    { 512,  "e547f886b2c15d995ed76a8a924cb408c8080f66" },
    { 1000, "239443194409f1a5342ecde1a092c8f3a3ed790a" },
    { 1001, "f278ab9a102850a9f48dc4e9e6822afe2d0c52b5" },
    { 1002, "8bcc667df5ab6ece988b3af361d09747c77f4e72" },
    { 1003, "0fae6046c7dc1d3e356b25af836f6077a363f338" },
    { 1004, "0ea48401cc92ae6bc92ae76685269cb0167fbe1a" },
    { 1005, "ecbcd7c879b295bafcd8766cbeac58cc371e31d1" },
    { 1006, "eb4a4a3d07d1e9a15e6f1ab8a9c47f243e27324c" },
    { 1336, "f5950ee1d77c10e9011d2149699c9366fe52529c" },
    { 1344, "b04263604a63c351b0b3b9cf1785b4bdba6c8838" },
    { 1399, "8cb1cff61d5b784045974a2fc69386e3b8d24218" },
    { 1400, "9bb2f3fcbeddb2b90f0be797cd647334a2816d51" },
    { 1401, "23ae462a7a0cb440f7445791079a5d75a535dd33" },
    { 1402, "832974b524a4d3f9cc2f45a3cabf5ccef65cd2aa" },
    { 1403, "d1c683742fe404c3c20d5704a5430e7832a7ec95" },
    { 1404, "867c79042e64f310628e219d8b85594cd0c7adc3" },
    { 1405, "c9d81d49d13d94358f56ccfd61af02b36c69f7c3" },
    { 1406, "0df43daab2786172f9b8d07d61f14a070cf1287a" },
    { 1407, "0fd8f3ad7f169534b274d4c66bbddd89f759e391" },
    { 1408, "3987511182b18473a564436003139b808fa46343" },
    { 1409, "ef667e063c9e9f539a8987a8d0bd3066ee85d901" },
    { 2048, "921109c99f3fedaca21727156d5f2b4460175327" },
    { 2049, "47188600dd165eb45f27c27196d3c46f4f042c1b" },
    { 2050, "8831939904009338de10e7fa670847041387807d" },
    { 2051, "2f8ebb5db2997d614e767be1050366f3641e7520" },
    { 2052, "669e51cd730dae158d3bef8adba075bd95a0d011" },
    { 3072, "cfee66cfd83abc8451af3c96c6b35a41cc6c55f5" },
    { 3074, "216ea26f02976a261b7d21a4dd3085157bedfabd" },
    { 3075, "bd612ebba021fd8e012b14c3bd60c8c5161fabc0" },
    { 4048, "c2564c1fdf2d5e9d7dde7aace2643428e90662e8" },
    { 4052, "91ce61fe924b445dfe7b5a1dcd10a27caec16df6" },
    { 4058, "db2a9be5ee8124f091c7ebd699266c5de223c164" },
    { 6144, "855109903feae2ba3a7a05a326b8a171116eb368" },
    { 6150, "37520bb3a668294d9c7b073e7e3daf8fee248a78" },
    { 6400, "60a353c841b6d2b1a05890349dad2fa33c7536b7" },
    { 6528, "9e53a43a69bb42d7c8522ca8bd632e421d5edb36" },
    { 8192, "a918cb0da862eaea0a33ee0efea50243e6b4927c" },
    { 8320, "29a5dcf55d1db29cd113fcf0572ae414f1c71329" },
    {16384, "6fb27966138e0c8d5a0d65ace817ebd53633cee1" },
    {18432, "ca09900d891c7c9ae2a559b10f63a217003341c1" },
    { 0, NULL },
};

/* Key         = 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
 *               0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10
 *               0x11, 0x12, 0x13, 0x14
 * InputHexStr = "31323334353637383930" (ASCII = "1234567890")
 */
static MV_CESA_SIZE_TEST     shaMultiSizeTest404[] =
{
    { 80,   "beaf20a34b06a87558d156c0949bc3957d40222e" },
    { 512,  "3353955358d886bc2940a3c7f337ff7dafb59c7b" },
    { 1000, "8737a542c5e9b2b6244b757ebb69d5bd602a829f" },
    { 1001, "fd9e7582d8a5d3c9fe3b923e4e6a41b07a1eb4d4" },
    { 1002, "a146d14a6fc3c274ff600568f4d75b977989e00d" },
    { 1003, "be22601bbc027ddef2dec97d30b3dc424fd803c5" },
    { 1004, "3e71fe99b2fe2b7bfdf4dbf0c7f3da25d7ea35e7" },
    { 1005, "2c422735d7295408fddd76f5e8a83a2a8da13df3" },
    { 1006, "6d875319049314b61855101a647b9ba3313428e6" },
    { 1336, "c1631ea80bad9dc43a180712461b65a0598c711c" },
    { 1344, "816069bf91d34581005746e2e0283d0f9c7b7605" },
    { 1399, "4e139866dc61cfcb8b67ca2ebd637b3a538593af" },
    { 1400, "ff2a0f8dd2b02c5417910f6f55d33a78e081a723" },
    { 1401, "ab00c12be62336964cbce31ae97fe2a0002984d5" },
    { 1402, "61349e7f999f3a1acc56c3e9a5060a9c4a7b05b6" },
    { 1403, "3edbc0f61e435bc1317fa27d840076093fb79353" },
    { 1404, "d052c6dfdbe63d45dab23ef9893e2aa4636aca1e" },
    { 1405, "0cc16b7388d67bf0add15a31e6e6c753cfae4987" },
    { 1406, "c96ba7eaad74253c38c22101b558d2850b1d1b90" },
    { 1407, "3445428a40d2c6556e7c55797ad8d323b61a48d9" },
    { 1408, "8d6444f937a09317c89834187b8ea9b8d3a8c56b" },
    { 1409, "c700acd3ecd19014ea2bdb4d42510c467e088475" },
    { 2048, "ee27d2a0cb77470c2f496212dfd68b5bb7b04e4b" },
    { 2049, "683762d7a02983b26a6d046e6451d9cd82c25932" },
    { 2050, "0fd20f1d55a9ee18363c2a6fd54aa13aee69992f" },
    { 2051, "86c267d8cc4bc8d59090e4f8b303da960fd228b7" },
    { 2052, "452395ae05b3ec503eea34f86fc0832485ad97c1" },
    { 3072, "75198e3cfd0b9bcff2dabdf8e38e6fdaa33ca49a" },
    { 3074, "4e24785ef080141ce4aab4675986d9acea624d7c" },
    { 3075, "3a20c5978dd637ec0e809bf84f0d9ccf30bc65bf" },
    { 4048, "3c32da256be7a7554922bf5fed51b0d2d09e59ad" },
    { 4052, "fff898426ea16e54325ae391a32c6c9bce4c23c0" },
    { 4058, "c800b9e562e1c91e1310116341a3c91d37f848ec" },
    { 6144, "d91d509d0cc4376c2d05bf9a5097717a373530e6" },
    { 6150, "d957030e0f13c5df07d9eec298542d8f94a07f12" },
    { 6400, "bb745313c3d7dc17b3f955e5534ad500a1082613" },
    { 6528, "77905f80d9ca82080bbb3e5654896dabfcfd1bdb" },
    { 8192, "5237fd9a81830c974396f99f32047586612ff3c0" },
    { 8320, "57668e28d5f2dba0839518a11db0f6af3d7e08bf" },
    {16384, "62e093fde467f0748087beea32e9af97d5c61241" },
    {18432, "845fb33130c7d6ea554fd5aacb9c50cf7ccb5929" },
    { 0, NULL },
};

/* HASH-SHA1
 * InputHexStr = "31323334353637383930" (ASCII = "1234567890")
 *               repeated "size" times
 */
static MV_CESA_SIZE_TEST     shaMultiSizeTest405[] =
{
    { 80,   "50abf5706a150990a08b2c5ea40fa0e585554732" },
    { 512,  "f14516a08948fa27917a974d219741a697ba0087" },
    { 1000, "0bd18c378d5788817eb4f1e5dc07d867efa5cbf4" },
    { 1001, "ca29b85c35db1b8aef83c977893a11159d1b7aa2" },
    { 1002, "d83bc973eaaedb8a31437994dabbb3304b0be086" },
    { 1003, "2cf7bbef0acd6c00536b5c58ca470df9a3a90b6c" },
    { 1004, "e4375d09b1223385a8a393066f8209acfd936a80" },
    { 1005, "1029b38043e027745d019ce1d2d68e3d8b9d8f99" },
    { 1006, "deea16dcebbd8ac137e2b984deb639b9fb5e9680" },
    { 1336, "ea031b065fff63dcfb6a41956e4777520cdbc55d" },
    { 1344, "b52096c6445e6c0a8355995c70dc36ae186c863c" },
    { 1399, "cde2f6f8379870db4b32cf17471dc828a8dbff2b" },
    { 1400, "e53ff664064bc09fe5054c650806bd42d8179518" },
    { 1401, "d1156db5ddafcace64cdb510ff0d4af9b9a8ad64" },
    { 1402, "34ede0e9a909dd84a2ae291539105c0507b958e1" },
    { 1403, "a772ca3536da77e6ad3251e4f9e1234a4d7b87c0" },
    { 1404, "29740fd2b04e7a8bfd32242db6233156ad699948" },
    { 1405, "65b17397495b70ce4865dad93bf991b74c97cce1" },
    { 1406, "a7ee89cd0754061fdb91af7ea6abad2c69d542e3" },
    { 1407, "3eebf82f7420188e23d328b7ce93580b279a5715" },
    { 1408, "e08d3363a8b9a490dfb3a4c453452b8f114deeec" },
    { 1409, "95d74df739181a4ff30b8c39e28793a36598e924" },
    { 2048, "aa40262509c2abf84aab0197f83187fc90056d91" },
    { 2049, "7dec28ef105bc313bade8d9a7cdeac58b99de5ea" },
    { 2050, "d2e30f77ec81197de20f56588a156094ecb88450" },
    { 2051, "6b22ccc874833e96551a39da0c0edcaa0d969d92" },
    { 2052, "f843141e57875cd669af58744bc60aa9ea59549c" },
    { 3072, "09c5fedeaa62c132e673cc3c608a00142273d086" },
    { 3074, "b09e95eea9c7b1b007a58accec488301901a7f3d" },
    { 3075, "e6226b77b4ada287a8c9bbcf4ed71eec5ce632dc" },
    { 4048, "e99394894f855821951ddddf5bfc628547435f5c" },
    { 4052, "32d2f1af38be9cfba6cd03d55a254d0b3e1eb382" },
    { 4058, "d906552a4f2aca3a22e1fecccbcd183d7289d0ef" },
    { 6144, "2e7f62d35a860988e1224dc0543204af19316041" },
    { 6150, "d6b89698ee133df46fec9d552fadc328aa5a1b51" },
    { 6400, "dff50e90c46853988fa3a4b4ce5dda6945aae976" },
    { 6528, "9e63ec0430b96db02d38bc78357a2f63de2ab7f8" },
    { 8192, "971eb71ed60394d5ab5abb12e88420bdd41b5992" },
    { 8320, "91606a31b46afeaac965cecf87297e791b211013" },
    {16384, "547f830a5ec1f5f170ce818f156b1002cabc7569" },
    {18432, "f16f272787f3b8d539652e4dc315af6ab4fda0ef" },
    { 0, NULL },
};

/* CryptoKey   = 0x01234567, 0x89abcdef,
 *               0x01234567, 0x89abcdef,
 *               0x01234567, 0x89abcdef;
 * MacKey      = 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
 *               0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10
 * InputHexStr = "31323334353637383930" (ASCII = "1234567890")
 * Note: only sizes aligned to 3DES block size (8 bytes) allowed
 */
static MV_CESA_SIZE_TEST     tripleDesMdMultiSizeTest502[] =
{
    {   64, "9586962a2aaaef28803dec2e17807a7f" },
    {   80, "b7726a03aad490bd6c5a452a89a1b271" },
    {  352, "f1ed9563aecc3c0d2766eb2bed3b4e4c" },
    {  512, "0f9decb11ab40fe86f4d4d9397bc020e" },
    { 1000, "3ba69deac12cab8ff9dff7dbd9669927" },
    { 1336, "6cf47bf1e80e03e2c1d0945bc50d37d2" },
    { 1344, "4be388dab21ceb3fa1b8d302e9b821f7" },
    { 1400, "a58b79fb21dd9bfc6ec93e3b99fb0ef1" },
    { 1408, "8bc97379fc2ac3237effcdd4f7a86528" },
    { 2048, "1339f03ab3076f25a20bc4cba16eb5bf" },
    { 3072, "731204d2d90c4b36ae41f5e1fb874288" },
    { 4048, "c028d998cfda5642547b7e1ed5ea16e4" },
    { 6144, "b1b19cd910cc51bd22992f1e59f1e068" },
    { 6400, "44e4613496ba622deb0e7cb768135a2f" },
    { 6528, "3b06b0a86f8db9cd67f9448dfcf10549" },
    { 8192, "d581780b7163138a0f412be681457d82" },
    {16384, "03b8ac05527faaf1bed03df149c65ccf" },
    {18432, "677c8a86a41dab6c5d81b85b8fb10ff6" },
    { 0, NULL },
};


/* CryptoKey   = 0x01234567, 0x89abcdef,
 *               0x01234567, 0x89abcdef,
 *               0x01234567, 0x89abcdef;
 * MacKey      = 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
 *               0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10
 *               0x11, 0x12, 0x13, 0x14
 * InputHexStr = "31323334353637383930" (ASCII = "1234567890")
 * Note: only sizes aligned to 3DES block size (8 bytes) allowed
 */
static MV_CESA_SIZE_TEST     tripleDesShaMultiSizeTest503[] =
{
    {   64, "44a1e9bcbfc1429630d9ea68b7a48b0427a684f2" },
    {   80, "b2ddeaca91030eab5b95a234ef2c0f6e738ff883" },
    {  352, "4b91864c7ff629bdff75d9726421f76705452aaf" },
    {  512, "6dd37faceeb2aa98ba74f4242ed6734a4d546af5" },
    { 1000, "463661c30300be512a9df40904f0757cde5f1141" },
    { 1336, "b931f831d9034fe59c65176400b039fe9c1f44a5" },
    { 1344, "af8866b1cd4a4887d6185bfe72470ffdfb3648e1" },
    { 1400, "49c6caf07296d5e31d2504d088bc5b20c3ee7cdb" },
    { 1408, "fcae8deedbc6ebf0763575dc7e9de075b448a0f4" },
    { 2048, "edece5012146c1faa0dd10f50b183ba5d2af58ac" },
    { 3072, "5b83625adb43a488b8d64fecf39bb766818547b7" },
    { 4048, "d2c533678d26c970293af60f14c8279dc708bfc9" },
    { 6144, "b8f67af4f991b08b725f969b049ebf813bfacc5c" },
    { 6400, "d9a6c7f746ac7a60ef2edbed2841cf851c25cfb0" },
    { 6528, "376792b8c8d18161d15579fb7829e6e3a27e9946" },
    { 8192, "d890eabdca195b34ef8724b28360cffa92ae5655" },
    {16384, "a167ee52639ec7bf19aee9c6e8f76667c14134b9" },
    {18432, "e4396ab56f67296b220985a12078f4a0e365d2cc" },
    { 0, NULL },
};

/* CryptoKey   = 0x01234567, 0x89abcdef,
 *               0x01234567, 0x89abcdef,
 *               0x01234567, 0x89abcdef
 * IV          = 0x12345678, 0x90abcdef
 * MacKey      = 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
 *               0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10
 * InputHexStr = "31323334353637383930" (ASCII = "1234567890")
 * Note: only sizes aligned to 3DES block size (8 bytes) allowed
 */
static MV_CESA_SIZE_TEST     cbc3desMdMultiSizeTest504[] =
{
    {   64, "8d10e00802460ede0058c139ba48bd2d" },
    {   80, "6f463057e1a90e0e91ae505b527bcec0" },
    {  352, "4938d48bdf86aece2c6851e7c6079788" },
    {  512, "516705d59f3cf810ebf2a13a23a7d42e" },
    { 1000, "a5a000ee5c830e67ddc6a2d2e5644b31" },
    { 1336, "44af60087b74ed07950088efbe3b126a" },
    { 1344, "1f5b39e0577920af731dabbfcf6dfc2a" },
    { 1400, "6804ea640e29b9cd39e08bc37dbce734" },
    { 1408, "4fb436624b02516fc9d1535466574bf9" },
    { 2048, "c909b0985c423d8d86719f701e9e83db" },
    { 3072, "cfe0bc34ef97213ee3d3f8b10122db21" },
    { 4048, "03ea10b5ae4ddeb20aed6af373082ed1" },
    { 6144, "b9a0ff4f87fc14b3c2dc6f0ed0998fdf" },
    { 6400, "6995f85d9d4985dd99e974ec7dda9dd6" },
    { 6528, "bbbb548ce2fa3d58467f6a6a5168a0e6" },
    { 8192, "afe101fbe745bb449ae4f50d10801456" },
    {16384, "9741706d0b1c923340c4660ff97cacdf" },
    {18432, "b0217becb73cb8f61fd79c7ce9d023fb" },
    { 0, NULL },
};


/* CryptoKey   = 0x01234567, 0x89abcdef,
 *               0x01234567, 0x89abcdef,
 *               0x01234567, 0x89abcdef;
 * IV          = 0x12345678, 0x90abcdef
 * MacKey      = 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
 *               0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10
 *               0x11, 0x12, 0x13, 0x14
 * InputHexStr = "31323334353637383930" (ASCII = "1234567890")
 * Note: only sizes aligned to 3DES block size (8 bytes) allowed
 */
static MV_CESA_SIZE_TEST     cbc3desShaMultiSizeTest505[] =
{
    {   64, "409187e5bdb0be4a7754ca3747f7433dc4f01b98" },
    {   80, "1b002ed050be743aa98860cf35659646bb8efcc0" },
    {  352, "6cbf7ebe50fa4fa6eecc19eca23f9eae553ccfff" },
    {  512, "cfb5253fb4bf72b743320c30c7e48c54965853b0" },
    { 1000, "95e04e1ca2937e7c5a9aba9e42d2bcdb8a7af21f" },
    { 1336, "3b5c1f5eee5837ebf67b83ae01405542d77a6627" },
    { 1344, "2b3d42ab25615437f98a1ee310b81d07a02badc2" },
    { 1400, "7f8687df7c1af44e4baf3c934b6cca5ab6bc993e" },
    { 1408, "473a581c5f04f7527d50793c845471ac87e86430" },
    { 2048, "e41d20cae7ebe34e6e828ed62b1e5734019037bb" },
    { 3072, "275664afd7a561d804e6b0d204e53939cde653ae" },
    { 4048, "0d220cc5b34aeeb46bbbd637dde6290b5a8285a3" },
    { 6144, "cb393ddcc8b1c206060625b7d822ef9839e67bc5" },
    { 6400, "dd3317e2a627fc04800f74a4b05bfda00fab0347" },
    { 6528, "8a74c3b2441ab3f5a7e08895cc432566219a7c41" },
    { 8192, "b8e6ef3a549ed0e005bd5b8b1a5fe6689e9711a7" },
    {16384, "55f59404008276cdac0e2ba0d193af2d40eac5ce" },
    {18432, "86ae6c4fc72369a54cce39938e2d0296cd9c6ec5" },
    { 0, NULL },
};


/* CryptoKey   = 0x01234567, 0x89abcdef,
 *               0x01234567, 0x89abcdef,
 *               0x01234567, 0x89abcdef
 * IV          = 0x12345678, 0x90abcdef
 * MacKey      = 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
 *               0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10
 * InputHexStr = "31323334353637383930" (ASCII = "1234567890")
 * Note: only sizes aligned to AES block size (16 bytes) allowed
 */
static MV_CESA_SIZE_TEST     cbcAes128md5multiSizeTest506[] =
{
    {   16, "7ca4c2ba866751598720c5c4aa0d6786" },
    {   64, "7dba7fb988e80da609b1fea7254bced8" },
    {   80, "6b6e863ac5a71d15e3e9b1c86c9ba05f" },
    {  352, "a1ceb9c2e3021002400d525187a9f38c" },
    {  512, "596c055c1c55db748379223164075641" },
    { 1008, "f920989c02f3b3603f53c99d89492377" },
    { 1344, "2e496b73759d77ed32ea222dbd2e7b41" },
    { 1408, "7178c046b3a8d772efdb6a71c4991ea4" },
    { 2048, "a917f0099c69eb94079a8421714b6aad" },
    { 3072, "693cd5033d7f5391d3c958519fa9e934" },
    { 4048, "139dca91bcff65b3c40771749052906b" },
    { 6144, "428d9cef6df4fb70a6e9b6bbe4819e55" },
    { 6400, "9c0b909e76daa811e12b1fc17000a0c4" },
    { 6528, "ad876f6297186a7be1f1b907ed860eda" },
    { 8192, "479cbbaca37dd3191ea1f3e8134a0ef4" },
    {16384, "60fda559c74f91df538100c9842f2f15" },
    {18432, "4a3eb1cba1fa45f3981270953f720c42" },
    { 0, NULL },
};


/* CryptoKey   = 0x01234567, 0x89abcdef,
 *               0x01234567, 0x89abcdef,
 *               0x01234567, 0x89abcdef;
 * IV          = 0x12345678, 0x90abcdef
 * MacKey      = 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
 *               0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10
 *               0x11, 0x12, 0x13, 0x14
 * InputHexStr = "31323334353637383930" (ASCII = "1234567890")
 * Note: only sizes aligned to AES block size (16 bytes) allowed
 */
static MV_CESA_SIZE_TEST     cbcAes128sha1multiSizeTest507[] =
{
    {   16, "9aa8dc1c45f0946daf78057fa978759c625c1fee" },
    {   64, "9f588fc1ede851e5f8b20256abc9979465ae2189" },
    {   80, "13558472d1fc1c90dffec6e5136c7203452d509b" },
    {  352, "6b93518e006cfaa1f7adb24615e7291fb0a27e06" },
    {  512, "096874951a77fbbf333e49d80c096ee2016e09bd" },
    { 1008, "696fc203c2e4b5ae0ec5d1db3f623c490bc6dbac" },
    { 1344, "79bf77509935ccd3528caaac6a5eb6481f74029b" },
    { 1408, "627f9462b95fc188e8cfa7eec15119bdc5d4fcf1" },
    { 2048, "3d50d0c005feba92fe41502d609fced9c882b4d1" },
    { 3072, "758807e5b983e3a91c06fb218fe0f73f77111e94" },
    { 4048, "ca90e85242e33f005da3504416a52098d0d31fb2" },
    { 6144, "8044c1d4fd06642dfc46990b4f18b61ef1e972cf" },
    { 6400, "166f1f4ea57409f04feba9fb1e39af0e00bd6f43" },
    { 6528, "0389016a39485d6e330f8b4215ddf718b404f7e9" },
    { 8192, "6df7ee2a8b61d6f7f860ce8dbf778f0c2a5b508b" },
    {16384, "a70a6d8dfa1f91ded621c3dbaed34162bc48783f" },
    {18432, "8dfad627922ce15df1eed10bdbed49244efa57db" },
    { 0, NULL },
};


void    cesaTestPrintStatus(void);


/*------------------------- LOCAL FUNCTIONs ---------------------------------*/
MV_STATUS testCmd(int sid, int iter, MV_CESA_COMMAND* pCmd,
                  MV_CESA_TEST_SESSION* pTestSession, MV_U8* pIV, int ivSize);
MV_STATUS testClose(int idx);
MV_STATUS testOpen(int idx);
void close_session(int sid);
void cesaTestCheckReady(const MV_CESA_RESULT *r);
void cesaCheckReady(MV_CESA_RESULT* r);
void printTestResults(int idx, MV_STATUS status, int checkMode);
void cesaLastResult(void);
void cesaTestPrintReq(int req, int offset, int size);

void cesaTestPrintStatus(void);
void cesaTestPrintSession(int idx);
void sizeTest(int testIdx, int iter, int checkMode);
void multiTest(int iter, int reqSize, int checkMode);
void oneTest(int testIdx, int caseIdx,int iter, int reqSize, int checkMode);
void multiSizeTest(int idx, int iter, int checkMode, char* inputData);
void cesaTest(int iter, int reqSize, int checkMode);
void cesaOneTest(int testIdx, int caseIdx,int iter, int reqSize, int checkMode);
void combiTest(int iter, int reqSize, int checkMode);
void shaTest(int iter, int reqSize, int checkMode);
void mdTest(int iter, int reqSize, int checkMode);
void aesTest(int iter, int reqSize, int checkMode);
void tripleDesTest(int iter, int reqSize, int checkMode);
void desTest(int iter, int reqSize, int checkMode);
void cesaTestStop(void);
MV_STATUS testRun(int idx, int caseIdx, int iter,int reqSize, int checkMode);
void cesaTestStart(int bufNum, int bufSize);


static MV_U32      getRate(MV_U32* remainder)
{
    MV_U32     kBits, milliSec, rate;

    milliSec = 0;
    if( (cesaEndTicks - cesaBeginTicks) > 0)
    {
        milliSec = CESA_TEST_TICK_TO_MS(cesaEndTicks - cesaBeginTicks);
    }
    if(milliSec == 0)
    {
        if(remainder != NULL)
            *remainder = 0;
        return 0;
    }

    kBits = (cesaIteration*cesaRateSize*8)/1000;
    rate = kBits/milliSec;
    if(remainder != NULL)
        *remainder = ((kBits % milliSec)*10)/milliSec;

    return rate;
}

static char*    extractMbuf(MV_CESA_MBUF *pMbuf,
                            int offset, int size, char* hexStr)
{
    mvCesaCopyFromMbuf((MV_U8*)cesaBinBuffer, pMbuf, offset, size);
    mvBinToHex((const MV_U8*)cesaBinBuffer, hexStr, size);

    return hexStr;
}

static MV_BOOL  cesaCheckMbuf(MV_CESA_MBUF *pMbuf,
                          const char* hexString, int offset,
                          int checkSize)
{
    MV_BOOL     isFailed = MV_FALSE;
    MV_STATUS   status;
    int         size = strlen(hexString)/2;
    int         checkedSize = 0;
/*
    mvOsPrintf("cesaCheckMbuf: pMbuf=%p, offset=%d, checkSize=%d, mBufSize=%d\n",
                pMbuf, offset, checkSize, pMbuf->mbufSize);
*/
    if(pMbuf->mbufSize < (checkSize + offset))
    {
        mvOsPrintf("checkSize (%d) is too large: offset=%d, mbufSize=%d\n",
                    checkSize, offset, pMbuf->mbufSize);
        return MV_TRUE;
    }
    status = mvCesaCopyFromMbuf((MV_U8*)cesaBinBuffer, pMbuf, offset, checkSize);
    if(status != MV_OK)
    {
        mvOsPrintf("CesaTest: Can't copy %d bytes from Mbuf=%p to checkBuf=%p\n",
                    checkSize, pMbuf, cesaBinBuffer);
        return MV_TRUE;
    }
/*
    mvDebugMemDump(cesaBinBuffer, size, 1);
*/
    mvHexToBin(hexString, (MV_U8*)cesaExpBinBuffer, size);

    /* Compare buffers */
    while(checkSize > checkedSize)
    {
        size = MV_MIN(size, (checkSize - checkedSize));
        if(memcmp(cesaExpBinBuffer, &cesaBinBuffer[checkedSize], size) != 0)
        {
            mvOsPrintf("CheckMbuf failed: checkSize=%d, size=%d, checkedSize=%d\n",
                        checkSize, size, checkedSize);
            mvDebugMemDump(&cesaBinBuffer[checkedSize], size, 1);
            mvDebugMemDump(cesaExpBinBuffer, size, 1);

            isFailed = MV_TRUE;
            break;
        }
        checkedSize += size;
    }

    return isFailed;
}

static MV_STATUS    cesaSetMbuf(MV_CESA_MBUF *pMbuf,
                        const char* hexString,
                        int offset, int reqSize)
{
    MV_STATUS   status = MV_OK;
    int         copySize, size = strlen(hexString)/2;

    mvHexToBin(hexString, (MV_U8*)cesaBinBuffer, size);

    copySize = 0;
    while(reqSize > copySize)
    {
        size = MV_MIN(size, (reqSize - copySize));

        status = mvCesaCopyToMbuf((MV_U8*)cesaBinBuffer, pMbuf, offset+copySize, size);
        if(status != MV_OK)
        {
            mvOsPrintf("cesaSetMbuf Error: Copy %d of %d bytes to MBuf\n",
                        copySize, reqSize);
            break;
        }
        copySize += size;
    }
    pMbuf->mbufSize = offset+copySize;
    return status;
}

static  MV_CESA_TEST_SESSION* getTestSessionDb(int idx, int* pTestIdx)
{
    int                 testIdx, dbIdx = idx/100;

    if(dbIdx > MAX_TEST_TYPE)
    {
        mvOsPrintf("Wrong index %d - No such test type\n", idx);
        return NULL;
    }
    testIdx = idx % 100;

    if(testIdx >= cesaTestsDB[dbIdx].numSessions)
    {
        mvOsPrintf("Wrong index %d - No such test\n", idx);
        return NULL;
    }
    if(pTestIdx != NULL)
        *pTestIdx = testIdx;

    return  cesaTestsDB[dbIdx].pSessions;
}

/* Debug */
void    cesaTestPrintReq(int req, int offset, int size)
{
    MV_CESA_MBUF*   pMbuf;

    mvOsPrintf("cesaTestPrintReq: req=%d, offset=%d, size=%d\n", 
                req, offset, size);
    mvDebugMemDump(cesaCmdRing, 128, 4);

    pMbuf = cesaCmdRing[req].pSrc;
    mvCesaDebugMbuf("src", pMbuf, offset,size);
    pMbuf = cesaCmdRing[req].pDst;
    mvCesaDebugMbuf("dst", pMbuf, offset, size);

    cesaTestPrintStatus();
}

void    cesaLastResult(void)
{
        mvOsPrintf("Last Result: ReqId = %d, SessionId = %d, rc = (%d)\n",
                (MV_U32)cesaResult.pReqPrv, cesaResult.sessionId,
                cesaResult.retCode);
}

void    printTestResults(int idx, MV_STATUS status, int checkMode)
{
    int                     testIdx;
    MV_CESA_TEST_SESSION*   pTestSessions = getTestSessionDb(idx, &testIdx);

    if(pTestSessions == NULL)
        return;

    mvOsPrintf("%-35s %4dx%-4d : ", pTestSessions[testIdx].name,
            cesaIteration, cesaReqSize);
    if( (status == MV_OK)      &&
        (cesaCryptoError == 0) &&
        (cesaError == 0)       &&
        (cesaReqIdError == 0) )
    {
        mvOsPrintf("Passed, Rate=%3u.%u Mbps (%5u cpp)\n", 
                     cesaRate, cesaRateAfterDot, cesaEndTicks - cesaBeginTicks);
    }
    else
    {
        mvOsPrintf("Failed, Status = 0x%x\n", status);
        if(cesaCryptoError > 0)
            mvOsPrintf("cryptoError : %d\n", cesaCryptoError);
        if(cesaReqIdError > 0)
            mvOsPrintf("reqIdError  : %d\n", cesaReqIdError);
        if(cesaError > 0)
            mvOsPrintf("cesaError  : %d\n", cesaError);
    }
    if(cesaTestIsrMissCount > 0)
        mvOsPrintf("cesaIsrMissed  : %d\n", cesaTestIsrMissCount);
}

void cesaCheckReady(MV_CESA_RESULT* r)
{
    int             reqId;
    MV_CESA_MBUF    *pMbuf;
    MV_BOOL         isFailed;

    cesaResult  =  *r;
    reqId = (int)cesaResult.pReqPrv;
    pMbuf = cesaCmdRing[reqId].pDst;

/*
    mvOsPrintf("cesaCheckReady: reqId=%d, checkOffset=%d, checkSize=%d\n",
                    reqId, cesaCheckOffset, cesaCheckSize);
*/
    /* Check expected reqId */
    if(reqId != cesaExpReqId)
    {
        cesaReqIdError++;
/*
        mvOsPrintf("CESA reqId Error: cbIter=%d (%d), reqId=%d, expReqId=%d\n",
                    cesaCbIter, cesaIteration, reqId, cesaExpReqId);
*/
    }
    else
    {
        if( (cesaCheckMode == CESA_FULL_CHECK_MODE) ||
            (cesaCheckMode == CESA_FAST_CHECK_MODE) )
        {
            if(cesaResult.retCode != MV_OK)
            {
                cesaError++;

                mvOsPrintf("CESA Error: cbIter=%d (%d), reqId=%d, rc=%d\n",
                            cesaCbIter, cesaIteration, reqId, cesaResult.retCode);
            }
            else
            {
                if( (cesaCheckSize > 0) && (cesaOutputHexStr != NULL) )
                {
                    /* Check expected output */

                    isFailed = cesaCheckMbuf(pMbuf, cesaOutputHexStr, cesaCheckOffset, cesaCheckSize);
                    if(isFailed)
                    {
                        mvOsPrintf("CESA Crypto Error: cbIter=%d (%d), reqId=%d\n",
                                    cesaCbIter, cesaIteration, reqId);

                        CESA_TEST_DEBUG_PRINT(("Error: reqId=%d, reqSize=%d, checkOffset=%d, checkSize=%d\n",
                                    reqId, cesaReqSize, cesaCheckOffset, cesaCheckSize));

                        CESA_TEST_DEBUG_PRINT(("Output str: %s\n", cesaOutputHexStr));

                        CESA_TEST_DEBUG_CODE( mvCesaDebugMbuf("error", pMbuf, 0, cesaCheckOffset+cesaCheckSize) );

                        cesaCryptoError++;
                    }
                }
            }
        }
    }
    if(cesaCheckMode == CESA_SHOW_CHECK_MODE)
    {
        extractMbuf(pMbuf, cesaCheckOffset, cesaCheckSize, cesaHexBuffer);
        mvOsPrintf("%4d, %s\n", cesaCheckOffset, cesaHexBuffer);
    }

    cesaCbIter++;
    if(cesaCbIter >= cesaIteration)
    {
        cesaCbIter = 0;
        cesaExpReqId = 0;
        cesaIsReady = MV_TRUE;

        cesaEndTicks = CESA_TEST_TICK_GET();
        cesaRate = getRate(&cesaRateAfterDot);
    }
    else
    {
        cesaExpReqId = reqId + 1;
        if(cesaExpReqId == CESA_DEF_REQ_SIZE)
            cesaExpReqId = 0;
    }
}


#ifdef MV_NETBSD
static int cesaTestReadyIsr(void *arg)
#else
#ifdef __KERNEL__
static irqreturn_t cesaTestReadyIsr( int irq , void *dev_id)
#endif
#ifdef MV_VXWORKS
void   cesaTestReadyIsr(void)
#endif
#endif
{
    MV_U32          cause;
    MV_STATUS       status;
    MV_CESA_RESULT  result;

    cesaTestIsrCount++;
    /* Clear cause register */
    cause = MV_REG_READ(MV_CESA_ISR_CAUSE_REG);
    if( (cause & MV_CESA_CAUSE_ACC_DMA_ALL_MASK) == 0)
    {
        mvOsPrintf("cesaTestReadyIsr: cause=0x%x\n", cause);
#ifdef MV_NETBSD
        return 0;
#else
#ifdef __KERNEL__
        return 1;
#else
        return;
#endif
#endif
    }

    MV_REG_WRITE(MV_CESA_ISR_CAUSE_REG, 0);

    while(MV_TRUE)
    {
        /* Get Ready requests */
        status = mvCesaReadyGet(&result);
        if(status == MV_OK)
            cesaCheckReady(&result);

        break;
    }
    if( (cesaTestFull == 1) && (status != MV_BUSY) )
    {
        cesaTestFull = 0;
        CESA_TEST_WAKE_UP();
    }

#ifdef __KERNEL__
    return 1;
#endif
}

void
cesaTestCheckReady(const MV_CESA_RESULT *r)
{
	MV_CESA_RESULT result = *r;

	cesaCheckReady(&result);

	if (cesaTestFull == 1) {
		cesaTestFull = 0;
		CESA_TEST_WAKE_UP();
	}
}

static INLINE int   open_session(MV_CESA_OPEN_SESSION* pOs)
{
    MV_U16      sid;
    MV_STATUS   status;

    status = mvCesaSessionOpen(pOs, (short*)&sid);
    if(status != MV_OK)
    {
        mvOsPrintf("CesaTest: Can't open new session - status = 0x%x\n",
                    status);
        return -1;
    }

    return  (int)sid;
}

void close_session(int sid)
{
    MV_STATUS   status;

    status = mvCesaSessionClose(sid);
    if(status != MV_OK)
    {
        mvOsPrintf("CesaTest: Can't close session %d - status = 0x%x\n",
                    sid, status);
    }
}

MV_STATUS testOpen(int idx)
{
    MV_CESA_OPEN_SESSION    os;
    int                     sid, i, testIdx;
    MV_CESA_TEST_SESSION*   pTestSession;
    MV_U16          digestSize = 0;

    pTestSession = getTestSessionDb(idx, &testIdx);
    if(pTestSession == NULL)
    {
        mvOsPrintf("Test %d is not exist\n", idx);
        return MV_BAD_PARAM;
    }
    pTestSession = &pTestSession[testIdx];

    if(pTestSession->sid != -1)
    {
        mvOsPrintf("Session for test %d already created: sid=%d\n",
                    idx, pTestSession->sid);
        return MV_OK;
    }

    os.cryptoAlgorithm = pTestSession->cryptoAlgorithm;
    os.macMode = pTestSession->macAlgorithm;
    switch(os.macMode)
    {
        case MV_CESA_MAC_MD5:
        case MV_CESA_MAC_HMAC_MD5:
            digestSize = MV_CESA_MD5_DIGEST_SIZE;
            break;

        case MV_CESA_MAC_SHA1:
        case MV_CESA_MAC_HMAC_SHA1:
            digestSize = MV_CESA_SHA1_DIGEST_SIZE;
            break;

        case MV_CESA_MAC_NULL:
            digestSize = 0;
    }
    os.cryptoMode = pTestSession->cryptoMode;
    os.direction = pTestSession->direction;
    os.operation = pTestSession->operation;

    for(i=0; i<pTestSession->cryptoKeySize; i++)
        os.cryptoKey[i] = pTestSession->pCryptoKey[i];

    os.cryptoKeyLength = pTestSession->cryptoKeySize;

    for(i=0; i<pTestSession->macKeySize; i++)
        os.macKey[i] = pTestSession->pMacKey[i];

    os.macKeyLength = pTestSession->macKeySize;
    os.digestSize = digestSize;

    sid = open_session(&os);
    if(sid == -1)
    {
        mvOsPrintf("Can't open session for test %d: rc=0x%x\n",
                    idx, cesaResult.retCode);
        return cesaResult.retCode;
    }
    CESA_TEST_DEBUG_PRINT(("Opened session: sid = %d\n", sid));
    pTestSession->sid = sid;
    return MV_OK;
}

MV_STATUS   testClose(int idx)
{
    int                     testIdx;
    MV_CESA_TEST_SESSION*   pTestSession;

    pTestSession = getTestSessionDb(idx, &testIdx);
    if(pTestSession == NULL)
    {
        mvOsPrintf("Test %d is not exist\n", idx);
        return MV_BAD_PARAM;
    }
    pTestSession = &pTestSession[testIdx];

    if(pTestSession->sid == -1)
    {
        mvOsPrintf("Test session %d is not opened\n", idx);
        return MV_NO_SUCH;
    }

    close_session(pTestSession->sid);
    pTestSession->sid = -1;

    return MV_OK;
}

MV_STATUS testCmd(int sid, int iter, MV_CESA_COMMAND* pCmd,
             MV_CESA_TEST_SESSION* pTestSession, MV_U8* pIV, int ivSize)
{
    int                 cmdReqId = 0;
    int                 i;
    MV_STATUS           rc = MV_OK;
    char                ivZeroHex[] = "0000";

    if(iter == 0)
        iter = CESA_DEF_ITER_NUM;

    if(pCmd == NULL)
    {
        mvOsPrintf("testCmd failed: pCmd=NULL\n");
        return MV_BAD_PARAM;
    }
    pCmd->sessionId = sid;

    cesaCryptoError = 0;
    cesaReqIdError = 0;
    cesaError = 0;
    cesaTestIsrMissCount = 0;
    cesaIsReady = MV_FALSE;
    cesaIteration = iter;

    if(cesaInputHexStr == NULL)
        cesaInputHexStr = cesaPlainHexEbc;

    for(i=0; i<CESA_DEF_REQ_SIZE; i++)
    {
        pCmd->pSrc = (MV_CESA_MBUF*)(cesaCmdRing[i].pSrc);
        if(pIV != NULL)
        {
            /* If IV from SA - set IV in Source buffer to zeros */
            cesaSetMbuf(pCmd->pSrc, ivZeroHex, 0, pCmd->cryptoOffset);
            cesaSetMbuf(pCmd->pSrc, cesaInputHexStr, pCmd->cryptoOffset,
                        (cesaReqSize - pCmd->cryptoOffset));
        }
        else
        {
            cesaSetMbuf(pCmd->pSrc, cesaInputHexStr, 0, cesaReqSize);
        }
        pCmd->pDst = (MV_CESA_MBUF*)(cesaCmdRing[i].pDst);
        cesaSetMbuf(pCmd->pDst, cesaNullPlainHexText, 0, cesaReqSize);

        memcpy(&cesaCmdRing[i], pCmd, sizeof(*pCmd));
    }

    if(cesaCheckMode == CESA_SW_SHOW_CHECK_MODE)
    {
        MV_U8   pDigest[MV_CESA_MAX_DIGEST_SIZE];

        if(pTestSession->macAlgorithm == MV_CESA_MAC_MD5)
        {
            mvMD5(pCmd->pSrc->pFrags[0].bufVirtPtr, pCmd->macLength, pDigest);
            mvOsPrintf("SW HASH_MD5: reqSize=%d, macLength=%d\n",
                        cesaReqSize, pCmd->macLength);
            mvDebugMemDump(pDigest, MV_CESA_MD5_DIGEST_SIZE, 1);
            return MV_OK;
        }
        if(pTestSession->macAlgorithm == MV_CESA_MAC_SHA1)
        {
            mvSHA1(pCmd->pSrc->pFrags[0].bufVirtPtr, pCmd->macLength, pDigest);
            mvOsPrintf("SW HASH_SHA1: reqSize=%d, macLength=%d\n",
                        cesaReqSize, pCmd->macLength);
            mvDebugMemDump(pDigest, MV_CESA_SHA1_DIGEST_SIZE, 1);
            return MV_OK;
        }
    }

    cesaBeginTicks = CESA_TEST_TICK_GET();
    CESA_TEST_DEBUG_CODE( memset(cesaTestTrace, 0, sizeof(cesaTestTrace));
                     cesaTestTraceIdx = 0;
    );

    if(cesaCheckMode == CESA_SW_NULL_CHECK_MODE)
    {
        volatile MV_U8   pDigest[MV_CESA_MAX_DIGEST_SIZE];

        for(i=0; i<iter; i++)
        {
            if(pTestSession->macAlgorithm == MV_CESA_MAC_MD5)
            {
                mvMD5(pCmd->pSrc->pFrags[0].bufVirtPtr, pCmd->macLength, (unsigned char*)pDigest);
            }
            if(pTestSession->macAlgorithm == MV_CESA_MAC_SHA1)
            {
                mvSHA1(pCmd->pSrc->pFrags[0].bufVirtPtr, pCmd->macLength, (MV_U8 *)pDigest);
            }
        }
        cesaEndTicks = CESA_TEST_TICK_GET();
        cesaRate = getRate(&cesaRateAfterDot);
        cesaIsReady = MV_TRUE;

        return MV_OK;
    }

    /*cesaTestIsrCount = 0;*/
    /*mvCesaDebugStatsClear();*/

#ifndef MV_NETBSD
    MV_REG_WRITE(MV_CESA_ISR_CAUSE_REG, 0);
#endif

    for(i=0; i<iter; i++)
    {
        unsigned long flags;

        pCmd = &cesaCmdRing[cmdReqId];
        pCmd->pReqPrv = (void*)cmdReqId;

        CESA_TEST_LOCK(flags);

        rc = mvCesaAction(pCmd);
        if(rc == MV_NO_RESOURCE)
            cesaTestFull = 1;

        CESA_TEST_UNLOCK(flags);

        if(rc == MV_NO_RESOURCE)
        {
            CESA_TEST_LOCK(flags);
            CESA_TEST_WAIT( (cesaTestFull == 0), 100);
            CESA_TEST_UNLOCK(flags);
            if(cesaTestFull == 1)
            {
                mvOsPrintf("CESA Test timeout: i=%d, iter=%d, cesaTestFull=%d\n",
                            i, iter, cesaTestFull);
                cesaTestFull = 0;
                return MV_TIMEOUT;
            }

            CESA_TEST_LOCK(flags);

            rc = mvCesaAction(pCmd);

            CESA_TEST_UNLOCK(flags);
        }
        if( (rc != MV_OK) && (rc != MV_NO_MORE) )
        {
            mvOsPrintf("mvCesaAction failed: rc=%d\n", rc);
            return rc;
        }

        cmdReqId++;
        if(cmdReqId >= CESA_DEF_REQ_SIZE)
            cmdReqId = 0;

#ifdef MV_LINUX
        /* Reschedule each 16 requests */
        if( (i & 0xF) == 0)
            schedule();
#endif
    }
    return MV_OK;
}

void    cesaTestStart(int bufNum, int bufSize)
{
    int             i, j, idx;
    MV_CESA_MBUF    *pMbufSrc, *pMbufDst;
    MV_BUF_INFO     *pFragsSrc, *pFragsDst;
    char            *pBuf;
#ifndef MV_NETBSD
    int             numOfSessions, queueDepth;
    char            *pSram;
    MV_STATUS       status;
    MV_CPU_DEC_WIN  addrDecWin;
#endif

    cesaCmdRing = mvOsMalloc(sizeof(MV_CESA_COMMAND) * CESA_DEF_REQ_SIZE);
    if(cesaCmdRing == NULL)
    {
        mvOsPrintf("testStart: Can't allocate %ld bytes of memory\n",
                sizeof(MV_CESA_COMMAND) * CESA_DEF_REQ_SIZE);
        return;
    }
    memset(cesaCmdRing, 0, sizeof(MV_CESA_COMMAND) * CESA_DEF_REQ_SIZE);

    if(bufNum == 0)
        bufNum = CESA_DEF_BUF_NUM;

    if(bufSize == 0)
        bufSize = CESA_DEF_BUF_SIZE;

    cesaBufNum = bufNum;
    cesaBufSize = bufSize;
    mvOsPrintf("CESA test started: bufNum = %d, bufSize = %d\n",
                bufNum, bufSize);

    cesaHexBuffer = mvOsMalloc(2*bufNum*bufSize);
    if(cesaHexBuffer == NULL)
    {
        mvOsPrintf("testStart: Can't malloc %d bytes for cesaHexBuffer.\n",
                    2*bufNum*bufSize);
        return;
    }
    memset(cesaHexBuffer, 0, (2*bufNum*bufSize));

    cesaBinBuffer = mvOsMalloc(bufNum*bufSize);
    if(cesaBinBuffer == NULL)
    {
        mvOsPrintf("testStart: Can't malloc %d bytes for cesaBinBuffer\n",
                    bufNum*bufSize);
        return;
    }
    memset(cesaBinBuffer, 0, (bufNum*bufSize));

    cesaExpBinBuffer = mvOsMalloc(bufNum*bufSize);
    if(cesaExpBinBuffer == NULL)
    {
        mvOsPrintf("testStart: Can't malloc %d bytes for cesaExpBinBuffer\n",
                    bufNum*bufSize);
        return;
    }
    memset(cesaExpBinBuffer, 0, (bufNum*bufSize));

    CESA_TEST_WAIT_INIT();

    pMbufSrc = mvOsMalloc(sizeof(MV_CESA_MBUF) * CESA_DEF_REQ_SIZE);
    pFragsSrc = mvOsMalloc(sizeof(MV_BUF_INFO) * bufNum * CESA_DEF_REQ_SIZE);

    pMbufDst = mvOsMalloc(sizeof(MV_CESA_MBUF) * CESA_DEF_REQ_SIZE);
    pFragsDst = mvOsMalloc(sizeof(MV_BUF_INFO) * bufNum * CESA_DEF_REQ_SIZE);

    if( (pMbufSrc == NULL) || (pFragsSrc == NULL) ||
        (pMbufDst == NULL) || (pFragsDst == NULL) )
    {
        mvOsPrintf("testStart: Can't malloc Src and Dst pMbuf and pFrags structures.\n");
        /* !!!! Dima cesaTestCleanup();*/
        return;
    }

    memset(pMbufSrc, 0, sizeof(MV_CESA_MBUF) * CESA_DEF_REQ_SIZE);
    memset(pFragsSrc, 0, sizeof(MV_BUF_INFO) * bufNum * CESA_DEF_REQ_SIZE);

    memset(pMbufDst, 0, sizeof(MV_CESA_MBUF) * CESA_DEF_REQ_SIZE);
    memset(pFragsDst, 0, sizeof(MV_BUF_INFO) * bufNum * CESA_DEF_REQ_SIZE);

    mvOsPrintf("Cesa Test Start: pMbufSrc=%p, pFragsSrc=%p, pMbufDst=%p, pFragsDst=%p\n",
                pMbufSrc, pFragsSrc, pMbufDst, pFragsDst);

    idx = 0;
    for(i=0; i<CESA_DEF_REQ_SIZE; i++)
    {
        pBuf = mvOsIoCachedMalloc(cesaTestOSHandle,bufSize * bufNum * 2,
				  &cesaReqBufs[i].bufPhysAddr,
				  &cesaReqBufs[i].memHandle);
		if(pBuf == NULL)
    	{
        	mvOsPrintf("testStart: Can't malloc %d bytes for pBuf\n",
                    bufSize * bufNum * 2);
        	return;
    	}

        memset(pBuf, 0, bufSize * bufNum * 2);
        mvOsCacheFlush(cesaTestOSHandle,pBuf, bufSize * bufNum * 2);
        if(pBuf == NULL)
        {
            mvOsPrintf("cesaTestStart: Can't allocate %d bytes for req_%d buffers\n",
                        bufSize * bufNum * 2, i);
            return;
        }

        cesaReqBufs[i].bufVirtPtr = (MV_U8*)pBuf;
        cesaReqBufs[i].bufSize =  bufSize * bufNum * 2;

        cesaCmdRing[i].pSrc = &pMbufSrc[i];
        cesaCmdRing[i].pSrc->pFrags = &pFragsSrc[idx];
        cesaCmdRing[i].pSrc->numFrags = bufNum;
        cesaCmdRing[i].pSrc->mbufSize = 0;

        cesaCmdRing[i].pDst = &pMbufDst[i];
        cesaCmdRing[i].pDst->pFrags = &pFragsDst[idx];
        cesaCmdRing[i].pDst->numFrags = bufNum;
        cesaCmdRing[i].pDst->mbufSize = 0;

        for(j=0; j<bufNum; j++)
        {
            cesaCmdRing[i].pSrc->pFrags[j].bufVirtPtr = (MV_U8*)pBuf;
            cesaCmdRing[i].pSrc->pFrags[j].bufSize = bufSize;
            pBuf += bufSize;
            cesaCmdRing[i].pDst->pFrags[j].bufVirtPtr = (MV_U8*)pBuf;
            cesaCmdRing[i].pDst->pFrags[j].bufSize = bufSize;
            pBuf += bufSize;
        }
        idx += bufNum;
    }

#ifndef MV_NETBSD
    if (mvCpuIfTargetWinGet(CRYPT_ENG, &addrDecWin) == MV_OK)
        pSram = (char*)addrDecWin.addrWin.baseLow;
    else
    {
        mvOsPrintf("mvCesaInit: ERR. mvCpuIfTargetWinGet failed\n");
        return;
    }

#ifdef MV_CESA_NO_SRAM
    pSram = mvOsMalloc(4*1024+8);
    if(pSram == NULL)
    {
        mvOsPrintf("CesaTest: can't allocate %d bytes for SRAM simulation\n",
                4*1024+8);
        /* !!!! Dima cesaTestCleanup();*/
        return;
    }
    pSram = (MV_U8*)MV_ALIGN_UP((MV_U32)pSram, 8);
#endif /* MV_CESA_NO_SRAM */

    numOfSessions = CESA_DEF_SESSION_NUM;
    queueDepth = CESA_DEF_REQ_SIZE - MV_CESA_MAX_CHAN;

    status = mvCesaInit(numOfSessions, queueDepth, pSram, NULL);
    if(status != MV_OK)
    {
        mvOsPrintf("mvCesaInit is Failed: status = 0x%x\n", status);
        /* !!!! Dima cesaTestCleanup();*/
        return;
    }
#endif /* !MV_NETBSD */

    /* Prepare data for tests */
    for(i=0; i<50; i++)
        strcat((char*)cesaDataHexStr3, "dd");

    strcpy((char*)cesaDataAndMd5digest3,  cesaDataHexStr3);
    strcpy((char*)cesaDataAndSha1digest3, cesaDataHexStr3);

    /* Digest must be 8 byte aligned */
    for(; i<56; i++)
    {
        strcat((char*)cesaDataAndMd5digest3, "00");
        strcat((char*)cesaDataAndSha1digest3, "00");
    }
    strcat((char*)cesaDataAndMd5digest3,  cesaHmacMd5digestHex3);
    strcat((char*)cesaDataAndSha1digest3, cesaHmacSha1digestHex3);

#ifndef MV_NETBSD
    MV_REG_WRITE( MV_CESA_ISR_CAUSE_REG, 0);
    MV_REG_WRITE( MV_CESA_ISR_MASK_REG, MV_CESA_CAUSE_ACC_DMA_MASK);
#endif

#ifdef MV_VXWORKS
    {
        MV_STATUS       status;

        status = intConnect((VOIDFUNCPTR *)INT_LVL_CESA, cesaTestReadyIsr, (int)NULL);
        if (status != OK)
        {
            mvOsPrintf("CESA: Can't connect CESA (%d) interrupt, status=0x%x \n",
                        INT_LVL_CESA, status);
            /* !!!! Dima cesaTestCleanup();*/
            return;
        }
        cesaSemId = semMCreate(SEM_Q_PRIORITY | SEM_INVERSION_SAFE | SEM_DELETE_SAFE);
        if(cesaSemId == NULL)
        {
            mvOsPrintf("cesaTestStart: Can't create semaphore\n");
            return;
        }
        intEnable(INT_LVL_CESA);
    }
#endif /* MV_VXWORKS */

#if !defined(MV_NETBSD) && defined(__KERNEL__)
        if( request_irq(CESA_IRQ, cesaTestReadyIsr, (SA_INTERRUPT) , "cesa_test", NULL ) ) 
        {
            mvOsPrintf( "cannot assign irq\n" );
            /* !!!! Dima cesaTestCleanup();*/
            return;
        }
        spin_lock_init( &cesaLock );
#endif
}

MV_STATUS   testRun(int idx, int caseIdx, int iter,
                    int reqSize, int checkMode)
{
    int                     testIdx, count, sid, digestSize;
    int                     blockSize;
    MV_CESA_TEST_SESSION*   pTestSession;
    MV_CESA_COMMAND         cmd;
    MV_STATUS               status;

    memset(&cmd, 0, sizeof(cmd));

    pTestSession = getTestSessionDb(idx, &testIdx);
    if(pTestSession == NULL)
    {
        mvOsPrintf("Test %d is not exist\n", idx);
        return MV_BAD_PARAM;
    }
    pTestSession = &pTestSession[testIdx];

    sid = pTestSession->sid;
    if(sid == -1)
    {
        mvOsPrintf("Test %d is not opened\n", idx);
        return MV_BAD_STATE;
    }
    switch(pTestSession->cryptoAlgorithm)
    {
        case MV_CESA_CRYPTO_DES:
        case MV_CESA_CRYPTO_3DES:
            blockSize = MV_CESA_DES_BLOCK_SIZE;
            break;

        case MV_CESA_CRYPTO_AES:
            blockSize = MV_CESA_AES_BLOCK_SIZE;
            break;

        case MV_CESA_CRYPTO_NULL:
            blockSize = 0;
            break;

        default:
            mvOsPrintf("cesaTestRun: Bad CryptoAlgorithm=%d\n",
                pTestSession->cryptoAlgorithm);
        return MV_BAD_PARAM;
    }
    switch(pTestSession->macAlgorithm)
    {
        case MV_CESA_MAC_MD5:
        case MV_CESA_MAC_HMAC_MD5:
            digestSize = MV_CESA_MD5_DIGEST_SIZE;
            break;

        case MV_CESA_MAC_SHA1:
        case MV_CESA_MAC_HMAC_SHA1:
            digestSize = MV_CESA_SHA1_DIGEST_SIZE;
            break;
        default:
            digestSize = 0;
    }

    if(iter == 0)
        iter = CESA_DEF_ITER_NUM;

    if(pTestSession->direction == MV_CESA_DIR_ENCODE)
    {
        cesaOutputHexStr = cesaTestCases[caseIdx].cipherHexStr;
        cesaInputHexStr = cesaTestCases[caseIdx].plainHexStr;
    }
    else
    {
        cesaOutputHexStr = cesaTestCases[caseIdx].plainHexStr;
        cesaInputHexStr = cesaTestCases[caseIdx].cipherHexStr;
    }

    cmd.sessionId = sid;
    if(checkMode == CESA_FAST_CHECK_MODE)
    {
        cmd.cryptoLength = cesaTestCases[caseIdx].cryptoLength;
        cmd.macLength = cesaTestCases[caseIdx].macLength;
    }
    else
    {
        cmd.cryptoLength = reqSize;
        cmd.macLength = reqSize;
    }
    cesaRateSize = cmd.cryptoLength;
    cesaReqSize = cmd.cryptoLength;
    cmd.cryptoOffset = 0;
    if(pTestSession->operation != MV_CESA_MAC_ONLY)
    {
        if( (pTestSession->cryptoMode == MV_CESA_CRYPTO_CBC) ||
            (pTestSession->cryptoMode == MV_CESA_CRYPTO_CTR) )
        {
            cmd.ivOffset = 0;
            cmd.cryptoOffset = blockSize;
            if(cesaTestCases[caseIdx].pCryptoIV == NULL)
            {
                cmd.ivFromUser = 1;
            }
            else
            {
                cmd.ivFromUser = 0;
                mvCesaCryptoIvSet(cesaTestCases[caseIdx].pCryptoIV, blockSize);
            }
            cesaReqSize = cmd.cryptoOffset + cmd.cryptoLength;
        }
    }

/*
    mvOsPrintf("ivFromUser=%d, cryptoLength=%d, cesaReqSize=%d, cryptoOffset=%d\n",
                cmd.ivFromUser, cmd.cryptoLength, cesaReqSize, cmd.cryptoOffset);
*/
    if(pTestSession->operation != MV_CESA_CRYPTO_ONLY)
    {
        cmd.macOffset = cmd.cryptoOffset;

        if(cesaTestCases[caseIdx].digestOffset == -1)
        {
            cmd.digestOffset = cmd.macOffset + cmd.macLength;
            cmd.digestOffset = MV_ALIGN_UP(cmd.digestOffset, 8);
        }
        else
        {
            cmd.digestOffset = cesaTestCases[caseIdx].digestOffset;
        }
        if( (cmd.digestOffset + digestSize) > cesaReqSize)
            cesaReqSize = cmd.digestOffset + digestSize;
    }

    cesaCheckMode = checkMode;

    if(checkMode == CESA_NULL_CHECK_MODE)
    {
        cesaCheckSize = 0;
        cesaCheckOffset = 0;
    }
    else
    {
        if(pTestSession->operation == MV_CESA_CRYPTO_ONLY)
        {
            cesaCheckOffset = 0;
            cesaCheckSize = cmd.cryptoLength;
        }
        else
        {
            cesaCheckSize = digestSize;
            cesaCheckOffset = cmd.digestOffset;
        }
    }
/*
    mvOsPrintf("reqSize=%d, checkSize=%d, checkOffset=%d, checkMode=%d\n",
                cesaReqSize, cesaCheckSize, cesaCheckOffset, cesaCheckMode);

    mvOsPrintf("blockSize=%d, ivOffset=%d, ivFromUser=%d, crOffset=%d, crLength=%d\n",
                blockSize, cmd.ivOffset, cmd.ivFromUser,
                cmd.cryptoOffset, cmd.cryptoLength);

    mvOsPrintf("macOffset=%d, digestOffset=%d, macLength=%d\n",
                cmd.macOffset, cmd.digestOffset, cmd.macLength);
*/
    status = testCmd(sid, iter, &cmd, pTestSession,
                     cesaTestCases[caseIdx].pCryptoIV, blockSize);

    if(status != MV_OK)
        return status;

    /* Wait when all callbacks is received */
    count = 0;
    while(cesaIsReady == MV_FALSE)
    {
        mvOsSleep(10);
        count++;
        if(count > 100)
        {
            mvOsPrintf("testRun: Timeout occured\n");
            return MV_TIMEOUT;
        }
    }

    return MV_OK;
}


void cesaTestStop(void)
{
    MV_CESA_MBUF    *pMbufSrc, *pMbufDst;
    MV_BUF_INFO     *pFragsSrc, *pFragsDst;
    int             i;

    /* Release all allocated memories */
    pMbufSrc = (MV_CESA_MBUF*)(cesaCmdRing[0].pSrc);
    pFragsSrc = cesaCmdRing[0].pSrc->pFrags;

    pMbufDst = (MV_CESA_MBUF*)(cesaCmdRing[0].pDst);
    pFragsDst = cesaCmdRing[0].pDst->pFrags;

    mvOsFree(pMbufSrc);
    mvOsFree(pMbufDst);
    mvOsFree(pFragsSrc);
    mvOsFree(pFragsDst);

    for(i=0; i<CESA_DEF_REQ_SIZE; i++)
    {
        mvOsIoCachedFree(cesaTestOSHandle,cesaReqBufs[i].bufSize,
			 cesaReqBufs[i].bufPhysAddr,cesaReqBufs[i].bufVirtPtr,
			 cesaReqBufs[i].memHandle);
    }
    cesaDataHexStr3[0] = '\0';
}

void    desTest(int iter, int reqSize, int checkMode)
{
    int         mode, i;
    MV_STATUS   status;

    mode = checkMode;
    if(checkMode == CESA_FULL_CHECK_MODE)
        mode = CESA_FAST_CHECK_MODE;
    i = iter;
    if(mode != CESA_NULL_CHECK_MODE)
        i = 1;

    testOpen(0);
    testOpen(1);
    testOpen(2);
    testOpen(3);

/* DES / ECB mode / Encrypt only */
    status = testRun(0, 1, iter, reqSize, checkMode);
    printTestResults(0, status, checkMode);

/* DES / ECB mode / Decrypt only */
    status = testRun(1, 1, iter, reqSize, checkMode);
    printTestResults(1, status, checkMode);

/* DES / CBC mode / Encrypt only */
    status = testRun(2, 2, i, reqSize, mode);
    printTestResults(2, status, mode);

/* DES / CBC mode / Decrypt only */
    status = testRun(3, 2, iter, reqSize, mode);
    printTestResults(3, status, mode);

    testClose(0);
    testClose(1);
    testClose(2);
    testClose(3);
}

void    tripleDesTest(int iter, int reqSize, int checkMode)
{
    int         mode, i;
    MV_STATUS   status;

    mode = checkMode;
    if(checkMode == CESA_FULL_CHECK_MODE)
        mode = CESA_FAST_CHECK_MODE;
    i = iter;
    if(mode != CESA_NULL_CHECK_MODE)
        i = 1;

    testOpen(100);
    testOpen(101);
    testOpen(102);
    testOpen(103);

/* 3DES / ECB mode / Encrypt only */
    status = testRun(100, 1, iter, reqSize, checkMode);
    printTestResults(100, status, checkMode);

/* 3DES / ECB mode / Decrypt only */
    status = testRun(101, 1, iter, reqSize, checkMode);
    printTestResults(101, status, checkMode);

/* 3DES / CBC mode / Encrypt only */
    status = testRun(102, 2, i, reqSize, mode);
    printTestResults(102, status, mode);

/* 3DES / CBC mode / Decrypt only */
    status = testRun(103, 2, iter, reqSize, mode);
    printTestResults(103, status, mode);

    testClose(100);
    testClose(101);
    testClose(102);
    testClose(103);
}

void    aesTest(int iter, int reqSize, int checkMode)
{
    MV_STATUS   status;
    int         mode, i;

    mode = checkMode;
    if(checkMode == CESA_FULL_CHECK_MODE)
        mode = CESA_FAST_CHECK_MODE;

    i = iter;
    if(mode != CESA_NULL_CHECK_MODE)
        i = 1;

    testOpen(200);
    testOpen(201);
    testOpen(202);
    testOpen(203);
    testOpen(204);
    testOpen(205);
    testOpen(206);
    testOpen(207);
    testOpen(208);

/* AES-128 Encode ECB mode */
    status = testRun(200, 3, iter, reqSize, checkMode);
    printTestResults(200, status, checkMode);

/* AES-128 Decode ECB mode */
    status = testRun(201, 3, iter, reqSize, checkMode);
    printTestResults(201, status, checkMode);

/* AES-128 Encode CBC mode (IV from SA) */
    status = testRun(202, 10, i, reqSize, mode);
    printTestResults(202, status, mode);

/* AES-128 Encode CBC mode (IV from User) */
    status = testRun(202, 24, i, reqSize, mode);
    printTestResults(202, status, mode);

/* AES-128 Decode CBC mode */
    status = testRun(203, 24, iter, reqSize, mode);
    printTestResults(203, status, checkMode);

/* AES-192 Encode ECB mode */
    status = testRun(204, 4, iter, reqSize, checkMode);
    printTestResults(204, status, checkMode);

/* AES-192 Decode ECB mode */
    status = testRun(205, 4, iter, reqSize, checkMode);
    printTestResults(205, status, checkMode);

/* AES-256 Encode ECB mode */
    status = testRun(206, 5, iter, reqSize, checkMode);
    printTestResults(206, status, checkMode);

/* AES-256 Decode ECB mode */
    status = testRun(207, 5, iter, reqSize, checkMode);
    printTestResults(207, status, checkMode);

#if defined(MV_LINUX)
/* AES-128 Encode CTR mode */
    status = testRun(208, 23, iter, reqSize, mode);
    printTestResults(208, status, checkMode);
#endif
    testClose(200);
    testClose(201);
    testClose(202);
    testClose(203);
    testClose(204);
    testClose(205);
    testClose(206);
    testClose(207);
    testClose(208);
}


void    mdTest(int iter, int reqSize, int checkMode)
{
    int         mode;
    MV_STATUS   status;

    if(iter == 0)
        iter = CESA_DEF_ITER_NUM;

    mode = checkMode;
    if(checkMode == CESA_FULL_CHECK_MODE)
        mode = CESA_FAST_CHECK_MODE;

    testOpen(300);
    testOpen(301);
    testOpen(302);
    testOpen(303);
    testOpen(305);

/* HMAC-MD5 Generate signature test */
    status = testRun(300, 6, iter, reqSize, mode);
    printTestResults(300, status, checkMode);

/* HMAC-MD5 Verify Signature test */
    status = testRun(301, 7, iter, reqSize, mode);
    printTestResults(301, status, checkMode);

/* HMAC-MD5 Generate signature test */
    status = testRun(302, 8, iter, reqSize, mode);
    printTestResults(302, status, checkMode);

/* HMAC-MD5 Verify Signature test */
    status = testRun(303, 9, iter, reqSize, mode);
    printTestResults(303, status, checkMode);

/* HASH-MD5 Generate signature test */
    status = testRun(305, 15, iter, reqSize, mode);
    printTestResults(305, status, checkMode);

    testClose(300);
    testClose(301);
    testClose(302);
    testClose(303);
    testClose(305);
}

void    shaTest(int iter, int reqSize, int checkMode)
{
    int         mode;
    MV_STATUS   status;

    if(iter == 0)
        iter = CESA_DEF_ITER_NUM;

    mode = checkMode;
    if(checkMode == CESA_FULL_CHECK_MODE)
        mode = CESA_FAST_CHECK_MODE;

    testOpen(400);
    testOpen(401);
    testOpen(402);
    testOpen(403);
    testOpen(405);

/* HMAC-SHA1 Generate signature test */
    status = testRun(400, 11, iter, reqSize, mode);
    printTestResults(400, status, checkMode);

/* HMAC-SHA1 Verify Signature test */
    status = testRun(401, 12, iter, reqSize, mode);
    printTestResults(401, status, checkMode);

/* HMAC-SHA1 Generate signature test */
    status = testRun(402, 13, iter, reqSize, mode);
    printTestResults(402, status, checkMode);

/* HMAC-SHA1 Verify Signature test */
    status = testRun(403, 14, iter, reqSize, mode);
    printTestResults(403, status, checkMode);

/* HMAC-SHA1 Generate signature test */
    status = testRun(405, 16, iter, reqSize, mode);
    printTestResults(405, status, checkMode);

    testClose(400);
    testClose(401);
    testClose(402);
    testClose(403);
    testClose(405);
}

void    combiTest(int iter, int reqSize, int checkMode)
{
    MV_STATUS   status;
    int         mode, i;

    mode = checkMode;
    if(checkMode == CESA_FULL_CHECK_MODE)
        mode = CESA_FAST_CHECK_MODE;

    if(iter == 0)
        iter = CESA_DEF_ITER_NUM;

    i = iter;
    if(mode != CESA_NULL_CHECK_MODE)
        i = 1;

    testOpen(500);
    testOpen(501);
    testOpen(502);
    testOpen(503);
    testOpen(504);
    testOpen(505);
    testOpen(506);
    testOpen(507);

/* DES ECB + MD5 encode test */
    status = testRun(500, 17, iter, reqSize, mode);
    printTestResults(500, status, mode);

/* DES ECB + SHA1 encode test */
    status = testRun(501, 18, iter, reqSize, mode);
    printTestResults(501, status, mode);

/* 3DES ECB + MD5 encode test */
    status = testRun(502, 17, iter, reqSize, mode);
    printTestResults(502, status, mode);

/* 3DES ECB + SHA1 encode test */
    status = testRun(503, 18, iter, reqSize, mode);
    printTestResults(503, status, mode);

/* 3DES CBC + MD5 encode test */
    status = testRun(504, 19, i, reqSize, mode);
    printTestResults(504, status, mode);

/* 3DES CBC + SHA1 encode test */
    status = testRun(505, 20, i, reqSize, mode);
    printTestResults(505, status, mode);

/* AES-128 CBC + MD5 encode test */
    status = testRun(506, 21, i, reqSize, mode);
    printTestResults(506, status, mode);

/* AES-128 CBC + SHA1 encode test */
    status = testRun(507, 22, i, reqSize, mode);
    printTestResults(507, status, mode);

    testClose(500);
    testClose(501);
    testClose(502);
    testClose(503);
    testClose(504);
    testClose(505);
    testClose(506);
    testClose(507);
}

void    cesaOneTest(int testIdx, int caseIdx,
                    int iter, int reqSize, int checkMode)
{
    MV_STATUS   status;

    if(iter == 0)
        iter = CESA_DEF_ITER_NUM;

    mvOsPrintf("test=%d, case=%d, size=%d, iter=%d\n",
                testIdx, caseIdx, reqSize, iter);

    status = testOpen(testIdx);

    status = testRun(testIdx, caseIdx, iter, reqSize, checkMode);
    printTestResults(testIdx, status, checkMode);
    status = testClose(testIdx);

}

void    cesaTest(int iter, int reqSize, int checkMode)
{
    if(iter == 0)
        iter = CESA_DEF_ITER_NUM;

    mvOsPrintf("%d iteration\n", iter);
    mvOsPrintf("%d size\n\n", reqSize);

/* DES tests */
    desTest(iter, reqSize, checkMode);

/* 3DES tests */
    tripleDesTest(iter, reqSize, checkMode);

/* AES tests */
    aesTest(iter, reqSize, checkMode);

/* MD5 tests */
    mdTest(iter, reqSize, checkMode);

/* SHA-1 tests */
    shaTest(iter, reqSize, checkMode);
}

void    multiSizeTest(int idx, int iter, int checkMode, char* inputData)
{
    MV_STATUS               status;
    int                     i;
    MV_CESA_SIZE_TEST*      pMultiTest;

    if( testOpen(idx) != MV_OK)
        return;

    if(iter == 0)
        iter = CESA_DEF_ITER_NUM;

    if(checkMode == CESA_SHOW_CHECK_MODE)
    {
        iter = 1;
    }
    else
        checkMode = CESA_FULL_CHECK_MODE;

    cesaTestCases[0].plainHexStr = inputData;
    cesaTestCases[0].pCryptoIV = NULL;

    switch(idx)
    {
        case 302:
            pMultiTest = mdMultiSizeTest302;
            if(inputData == NULL)
                cesaTestCases[0].plainHexStr = cesaDataHexStr3;
            break;

        case 304:
            pMultiTest = mdMultiSizeTest304;
            if(inputData == NULL)
                cesaTestCases[0].plainHexStr = hashHexStr80;
            break;

        case 305:
            pMultiTest = mdMultiSizeTest305;
            if(inputData == NULL)
                cesaTestCases[0].plainHexStr = hashHexStr80;
            break;

        case 402:
            pMultiTest = shaMultiSizeTest402;
            if(inputData == NULL)
                cesaTestCases[0].plainHexStr = hashHexStr80;
            break;

        case 404:
            pMultiTest = shaMultiSizeTest404;
            if(inputData == NULL)
                cesaTestCases[0].plainHexStr = hashHexStr80;
            break;

        case 405:
            pMultiTest = shaMultiSizeTest405;
            if(inputData == NULL)
                cesaTestCases[0].plainHexStr = hashHexStr80;
            break;

        case 502:
            pMultiTest = tripleDesMdMultiSizeTest502;
            if(inputData == NULL)
                cesaTestCases[0].plainHexStr = hashHexStr80;
            break;

        case 503:
            pMultiTest = tripleDesShaMultiSizeTest503;
            if(inputData == NULL)
                cesaTestCases[0].plainHexStr = hashHexStr80;
            break;

        case 504:
            iter = 1;
            pMultiTest = cbc3desMdMultiSizeTest504;
            cesaTestCases[0].pCryptoIV = iv1;
            if(inputData == NULL)
                cesaTestCases[0].plainHexStr = hashHexStr80;
            break;

        case 505:
            iter = 1;
            pMultiTest = cbc3desShaMultiSizeTest505;
            cesaTestCases[0].pCryptoIV = iv1;
            if(inputData == NULL)
                cesaTestCases[0].plainHexStr = hashHexStr80;
            break;

        case 506:
            iter = 1;
            pMultiTest = cbcAes128md5multiSizeTest506;
            cesaTestCases[0].pCryptoIV = iv5;
            if(inputData == NULL)
                cesaTestCases[0].plainHexStr = hashHexStr80;
            break;

        case 507:
            iter = 1;
            pMultiTest = cbcAes128sha1multiSizeTest507;
            cesaTestCases[0].pCryptoIV = iv5;
            if(inputData == NULL)
                cesaTestCases[0].plainHexStr = hashHexStr80;
            break;

        default:
            iter = 1;
            checkMode = CESA_SHOW_CHECK_MODE;
            pMultiTest = mdMultiSizeTest302;
            if(inputData == NULL)
                cesaTestCases[0].plainHexStr = hashHexStr80;
    }
    i = 0;
    while(pMultiTest[i].outputHexStr != NULL)
    {
        cesaTestCases[0].cipherHexStr = (char *)pMultiTest[i].outputHexStr;
        status = testRun(idx, 0, iter, pMultiTest[i].size,
                         checkMode);
        if(checkMode != CESA_SHOW_CHECK_MODE)
        {
            cesaReqSize = pMultiTest[i].size;
            printTestResults(idx, status, checkMode);
        }
        if(status != MV_OK)
            break;
        i++;
    }
    testClose(idx);
/*
    mvCesaDebugStatus();
    cesaTestPrintStatus();
*/
}

void    open_session_test(int idx, int caseIdx, int iter)
{
    int         reqIdError, cryptoError, openErrors, i;
    int         openErrDisp[100];
    MV_STATUS   status;

    memset(openErrDisp, 0, sizeof(openErrDisp));
    openErrors = 0;
    reqIdError = 0;
    cryptoError = 0;
    for(i=0; i<iter; i++)
    {
        status = testOpen(idx);
        if(status != MV_OK)
        {
            openErrors++;
            openErrDisp[status]++;
        }
        else
        {
            testRun(idx, caseIdx, 1, 0, CESA_FAST_CHECK_MODE);
            if(cesaCryptoError > 0)
                cryptoError++;
            if(cesaReqIdError > 0)
                reqIdError++;

            testClose(idx);
        }
    }
    if(cryptoError > 0)
        mvOsPrintf("cryptoError : %d\n", cryptoError);
    if(reqIdError > 0)
        mvOsPrintf("reqIdError  : %d\n", reqIdError);

    if(openErrors > 0)
    {
        mvOsPrintf("Open Errors = %d\n", openErrors);
        for(i=0; i<100; i++)
        {
            if(openErrDisp[i] != 0)
                mvOsPrintf("Error %d - occurs %d times\n", i, openErrDisp[i]);
        }
    }
}


void    loopback_test(int idx, int iter, int size, char* pPlainData)
{
}


#if defined(MV_VXWORKS)
int testMode = 0;
unsigned __TASKCONV cesaTask(void* args)
{
    int reqSize = cesaReqSize;

    if(testMode == 0)
    {
        cesaOneTest(cesaTestIdx, cesaCaseIdx, cesaIteration,
                    reqSize, cesaCheckMode);
    }
    else
    {
        if(testMode == 1)
        {
            cesaTest(cesaIteration, reqSize, cesaCheckMode);
            combiTest(cesaIteration, reqSize, cesaCheckMode);
        }
        else
        {
            multiSizeTest(cesaIdx, cesaIteration, cesaCheckMode, NULL);
        }
    }
    return 0;
}

void oneTest(int testIdx, int caseIdx,
              int iter, int reqSize, int checkMode)
{
    long    rc;

    cesaIteration = iter;
    cesaReqSize = cesaRateSize = reqSize;
    cesaCheckMode = checkMode;
    testMode = 0;
    cesaTestIdx = testIdx;
    cesaCaseIdx = caseIdx;
    rc = mvOsTaskCreate("CESA_T", 100, 4*1024, cesaTask, NULL, &cesaTaskId);
    if (rc != MV_OK)
    {
        mvOsPrintf("hMW: Can't create CESA multiCmd test task, rc = %ld\n", rc);
    }
}

void multiTest(int iter, int reqSize, int checkMode)
{
    long    rc;

    cesaIteration = iter;
    cesaCheckMode = checkMode;
    cesaReqSize = reqSize;
    testMode = 1;
    rc = mvOsTaskCreate("CESA_T", 100, 4*1024, cesaTask, NULL, &cesaTaskId);
    if (rc != MV_OK)
    {
        mvOsPrintf("hMW: Can't create CESA multiCmd test task, rc = %ld\n", rc);
    }
}

void sizeTest(int testIdx, int iter, int checkMode)
{
    long    rc;

    cesaIteration = iter;
        cesaCheckMode = checkMode;
        testMode = 2;
        cesaIdx = testIdx;
    rc = mvOsTaskCreate("CESA_T", 100, 4*1024, cesaTask, NULL, &cesaTaskId);
    if (rc != MV_OK)
    {
        mvOsPrintf("hMW: Can't create CESA test task, rc = %ld\n", rc);
    }
}

#endif /* MV_VXWORKS */

extern void    mvCesaDebugSA(short sid, int mode);
void    cesaTestPrintSession(int idx)
{
    int                     testIdx;
    MV_CESA_TEST_SESSION*   pTestSession;

    pTestSession = getTestSessionDb(idx, &testIdx);
    if(pTestSession == NULL)
    {
        mvOsPrintf("Test %d is not exist\n", idx);
        return;
    }
    pTestSession = &pTestSession[testIdx];

    if(pTestSession->sid == -1)
    {
        mvOsPrintf("Test session %d is not opened\n", idx);
        return;
    }

    mvCesaDebugSA(pTestSession->sid, 1);
}

void    cesaTestPrintStatus(void)
{
    mvOsPrintf("\n\t Cesa Test Status\n\n");

    mvOsPrintf("isrCount=%d\n",
                cesaTestIsrCount);

#ifdef CESA_TEST_DEBUG
    {
        int i, j;
        j = cesaTestTraceIdx;
        mvOsPrintf("No  Type  Cause   rCause   iCause   Res     Time     pReady    pProc    pEmpty\n");
        for(i=0; i<MV_CESA_TEST_TRACE_SIZE; i++)
        {
            mvOsPrintf("%02d.  %d   0x%04x  0x%04x   0x%04x   0x%02x   0x%02x   %02d   0x%06x  %p  %p  %p\n",
                j, cesaTestTrace[j].type, cesaTestTrace[j].cause, cesaTestTrace[j].realCause,
                cesaTestTrace[j].dmaCause, cesaTestTrace[j].resources, cesaTestTrace[j].timeStamp,
                cesaTestTrace[j].pReqReady, cesaTestTrace[j].pReqProcess, cesaTestTrace[j].pReqEmpty);
            j++;
            if(j == MV_CESA_TEST_TRACE_SIZE)
                j = 0;
        }
    }
#endif /* CESA_TEST_DEBUG */
}
