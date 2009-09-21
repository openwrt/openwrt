
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

#include "a_config.h"
#include "athdefs.h"
#include "a_types.h"
#include "AR6Khwreg.h"
#include "targaddrs.h"
#include "a_osapi.h"
#include "hif.h"
#include "htc_api.h"
#include "bmi.h"
#include "bmi_msg.h"
#include "common_drv.h"
#include "a_debug.h"
#include "targaddrs.h"

#define HOST_INTEREST_ITEM_ADDRESS(target, item)    \
(((TargetType) == TARGET_TYPE_AR6001) ?     \
   AR6001_HOST_INTEREST_ITEM_ADDRESS(item) :    \
   AR6002_HOST_INTEREST_ITEM_ADDRESS(item))


/* Compile the 4BYTE version of the window register setup routine,
 * This mitigates host interconnect issues with non-4byte aligned bus requests, some
 * interconnects use bus adapters that impose strict limitations.
 * Since diag window access is not intended for performance critical operations, the 4byte mode should
 * be satisfactory even though it generates 4X the bus activity. */

#ifdef USE_4BYTE_REGISTER_ACCESS

    /* set the window address register (using 4-byte register access ). */
A_STATUS ar6000_SetAddressWindowRegister(HIF_DEVICE *hifDevice, A_UINT32 RegisterAddr, A_UINT32 Address)
{
    A_STATUS status;
    A_UINT8 addrValue[4];
    int i;

        /* write bytes 1,2,3 of the register to set the upper address bytes, the LSB is written
         * last to initiate the access cycle */

    for (i = 1; i <= 3; i++) {
            /* fill the buffer with the address byte value we want to hit 4 times*/
        addrValue[0] = ((A_UINT8 *)&Address)[i];
        addrValue[1] = addrValue[0];
        addrValue[2] = addrValue[0];
        addrValue[3] = addrValue[0];

            /* hit each byte of the register address with a 4-byte write operation to the same address,
             * this is a harmless operation */
        status = HIFReadWrite(hifDevice,
                              RegisterAddr+i,
                              addrValue,
                              4,
                              HIF_WR_SYNC_BYTE_FIX,
                              NULL);
        if (status != A_OK) {
            break;
        }
    }

    if (status != A_OK) {
        AR_DEBUG_PRINTF(ATH_LOG_ERR, ("Cannot write initial bytes of 0x%x to window reg: 0x%X \n",
             RegisterAddr, Address));
        return status;
    }

        /* write the address register again, this time write the whole 4-byte value.
         * The effect here is that the LSB write causes the cycle to start, the extra
         * 3 byte write to bytes 1,2,3 has no effect since we are writing the same values again */
    status = HIFReadWrite(hifDevice,
                          RegisterAddr,
                          (A_UCHAR *)(&Address),
                          4,
                          HIF_WR_SYNC_BYTE_INC,
                          NULL);

    if (status != A_OK) {
        AR_DEBUG_PRINTF(ATH_LOG_ERR, ("Cannot write 0x%x to window reg: 0x%X \n",
            RegisterAddr, Address));
        return status;
    }

    return A_OK;



}


#else

    /* set the window address register */
A_STATUS ar6000_SetAddressWindowRegister(HIF_DEVICE *hifDevice, A_UINT32 RegisterAddr, A_UINT32 Address)
{
    A_STATUS status;

        /* write bytes 1,2,3 of the register to set the upper address bytes, the LSB is written
         * last to initiate the access cycle */
    status = HIFReadWrite(hifDevice,
                          RegisterAddr+1,  /* write upper 3 bytes */
                          ((A_UCHAR *)(&Address))+1,
                          sizeof(A_UINT32)-1,
                          HIF_WR_SYNC_BYTE_INC,
                          NULL);

    if (status != A_OK) {
        AR_DEBUG_PRINTF(ATH_LOG_ERR, ("Cannot write initial bytes of 0x%x to window reg: 0x%X \n",
             RegisterAddr, Address));
        return status;
    }

        /* write the LSB of the register, this initiates the operation */
    status = HIFReadWrite(hifDevice,
                          RegisterAddr,
                          (A_UCHAR *)(&Address),
                          sizeof(A_UINT8),
                          HIF_WR_SYNC_BYTE_INC,
                          NULL);

    if (status != A_OK) {
        AR_DEBUG_PRINTF(ATH_LOG_ERR, ("Cannot write 0x%x to window reg: 0x%X \n",
            RegisterAddr, Address));
        return status;
    }

    return A_OK;
}

#endif

/*
 * Read from the AR6000 through its diagnostic window.
 * No cooperation from the Target is required for this.
 */
A_STATUS
ar6000_ReadRegDiag(HIF_DEVICE *hifDevice, A_UINT32 *address, A_UINT32 *data)
{
    A_STATUS status;

        /* set window register to start read cycle */
    status = ar6000_SetAddressWindowRegister(hifDevice,
                                             WINDOW_READ_ADDR_ADDRESS,
                                             *address);

    if (status != A_OK) {
        return status;
    }

        /* read the data */
    status = HIFReadWrite(hifDevice,
                          WINDOW_DATA_ADDRESS,
                          (A_UCHAR *)data,
                          sizeof(A_UINT32),
                          HIF_RD_SYNC_BYTE_INC,
                          NULL);
    if (status != A_OK) {
        AR_DEBUG_PRINTF(ATH_LOG_ERR, ("Cannot read from WINDOW_DATA_ADDRESS\n"));
        return status;
    }

    return status;
}


/*
 * Write to the AR6000 through its diagnostic window.
 * No cooperation from the Target is required for this.
 */
A_STATUS
ar6000_WriteRegDiag(HIF_DEVICE *hifDevice, A_UINT32 *address, A_UINT32 *data)
{
    A_STATUS status;

        /* set write data */
    status = HIFReadWrite(hifDevice,
                          WINDOW_DATA_ADDRESS,
                          (A_UCHAR *)data,
                          sizeof(A_UINT32),
                          HIF_WR_SYNC_BYTE_INC,
                          NULL);
    if (status != A_OK) {
        AR_DEBUG_PRINTF(ATH_LOG_ERR, ("Cannot write 0x%x to WINDOW_DATA_ADDRESS\n", *data));
        return status;
    }

        /* set window register, which starts the write cycle */
    return ar6000_SetAddressWindowRegister(hifDevice,
                                           WINDOW_WRITE_ADDR_ADDRESS,
                                           *address);
}

A_STATUS
ar6000_ReadDataDiag(HIF_DEVICE *hifDevice, A_UINT32 address,
                    A_UCHAR *data, A_UINT32 length)
{
    A_UINT32 count;
    A_STATUS status = A_OK;

    for (count = 0; count < length; count += 4, address += 4) {
        if ((status = ar6000_ReadRegDiag(hifDevice, &address,
                                         (A_UINT32 *)&data[count])) != A_OK)
        {
            break;
        }
    }

    return status;
}

A_STATUS
ar6000_WriteDataDiag(HIF_DEVICE *hifDevice, A_UINT32 address,
                     A_UCHAR *data, A_UINT32 length)
{
    A_UINT32 count;
    A_STATUS status = A_OK;

    for (count = 0; count < length; count += 4, address += 4) {
        if ((status = ar6000_WriteRegDiag(hifDevice, &address,
                                          (A_UINT32 *)&data[count])) != A_OK)
        {
            break;
        }
    }

    return status;
}

A_STATUS
ar6000_reset_device_skipflash(HIF_DEVICE *hifDevice)
{
    int i;
    struct forceROM_s {
        A_UINT32 addr;
        A_UINT32 data;
    };
    struct forceROM_s *ForceROM;
    int szForceROM;
    A_UINT32 instruction;

    static struct forceROM_s ForceROM_REV2[] = {
        /* NB: This works for old REV2 ROM (old). */
        {0x00001ff0, 0x175b0027}, /* jump instruction at 0xa0001ff0 */
        {0x00001ff4, 0x00000000}, /* nop instruction at 0xa0001ff4 */

        {MC_REMAP_TARGET_ADDRESS, 0x00001ff0}, /* remap to 0xa0001ff0 */
        {MC_REMAP_COMPARE_ADDRESS, 0x01000040},/* ...from 0xbfc00040 */
        {MC_REMAP_SIZE_ADDRESS, 0x00000000},   /* ...1 cache line */
        {MC_REMAP_VALID_ADDRESS, 0x00000001},  /* ...remap is valid */

        {LOCAL_COUNT_ADDRESS+0x10, 0}, /* clear BMI credit counter */

        {RESET_CONTROL_ADDRESS, RESET_CONTROL_WARM_RST_MASK},
    };

    static struct forceROM_s ForceROM_NEW[] = {
        /* NB: This works for AR6000 ROM REV3 and beyond.  */
        {LOCAL_SCRATCH_ADDRESS, AR6K_OPTION_IGNORE_FLASH},
        {LOCAL_COUNT_ADDRESS+0x10, 0}, /* clear BMI credit counter */
        {RESET_CONTROL_ADDRESS, RESET_CONTROL_WARM_RST_MASK},
    };

    /*
     * Examine a semi-arbitrary instruction that's different
     * in REV2 and other revisions.
     * NB: If a Host port does not require simultaneous support
     * for multiple revisions of Target ROM, this code can be elided.
     */
    (void)ar6000_ReadDataDiag(hifDevice, 0x01000040,
                              (A_UCHAR *)&instruction, 4);

    AR_DEBUG_PRINTF(ATH_LOG_ERR, ("instruction=0x%x\n", instruction));

    if (instruction == 0x3c1aa200) {
        /* It's an old ROM */
        ForceROM = ForceROM_REV2;
        szForceROM = sizeof(ForceROM_REV2)/sizeof(*ForceROM);
        AR_DEBUG_PRINTF(ATH_LOG_ERR, ("Using OLD method\n"));
    } else {
        ForceROM = ForceROM_NEW;
        szForceROM = sizeof(ForceROM_NEW)/sizeof(*ForceROM);
        AR_DEBUG_PRINTF(ATH_LOG_ERR, ("Using NEW method\n"));
    }

    AR_DEBUG_PRINTF(ATH_LOG_ERR, ("Force Target to execute from ROM....\n"));
    for (i = 0; i < szForceROM; i++)
    {
        if (ar6000_WriteRegDiag(hifDevice,
                                &ForceROM[i].addr,
                                &ForceROM[i].data) != A_OK)
        {
            AR_DEBUG_PRINTF(ATH_LOG_ERR, ("Cannot force Target to execute ROM!\n"));
            return A_ERROR;
        }
    }

    msleep(50); /* delay to allow dragon to come to BMI phase  */
    return A_OK;
}

/* reset device */
A_STATUS ar6000_reset_device(HIF_DEVICE *hifDevice, A_UINT32 TargetType)
{

#if !defined(DWSIM)
    A_STATUS status = A_OK;
    A_UINT32 address;
    A_UINT32 data;

    do {

        // address = RESET_CONTROL_ADDRESS;
        data = RESET_CONTROL_COLD_RST_MASK;

          /* Hardcode the address of RESET_CONTROL_ADDRESS based on the target type */
        if (TargetType == TARGET_TYPE_AR6001) {
            address = 0x0C000000;
        } else {
            if (TargetType == TARGET_TYPE_AR6002) {
                address = 0x00004000;
            } else {
                A_ASSERT(0);
            }
        }

        status = ar6000_WriteRegDiag(hifDevice, &address, &data);

        if (A_FAILED(status)) {
            break;
        }

        /*
         * Read back the RESET CAUSE register to ensure that the cold reset
         * went through.
         */
        msleep(2000); /* 2 second delay to allow things to settle down */


        // address = RESET_CAUSE_ADDRESS;
        /* Hardcode the address of RESET_CAUSE_ADDRESS based on the target type */
        if (TargetType == TARGET_TYPE_AR6001) {
            address = 0x0C0000CC;
        } else {
            if (TargetType == TARGET_TYPE_AR6002) {
                address = 0x000040C0;
            } else {
                A_ASSERT(0);
            }
        }

        data = 0;
        status = ar6000_ReadRegDiag(hifDevice, &address, &data);

        if (A_FAILED(status)) {
            break;
        }

        AR_DEBUG_PRINTF(ATH_LOG_ERR, ("Reset Cause readback: 0x%X \n",data));
        data &= RESET_CAUSE_LAST_MASK;
        if (data != 2) {
            AR_DEBUG_PRINTF(ATH_LOG_ERR, ("Unable to cold reset the target \n"));
        }

    } while (FALSE);

    if (A_FAILED(status)) {
        AR_DEBUG_PRINTF(ATH_LOG_ERR, ("Failed to reset target \n"));
    }
#endif
    return A_OK;
}

#define REG_DUMP_COUNT_AR6001   38  /* WORDs, derived from AR6001_regdump.h */
#define REG_DUMP_COUNT_AR6002   32  /* WORDs, derived from AR6002_regdump.h */


#if REG_DUMP_COUNT_AR6001 <= REG_DUMP_COUNT_AR6002
#define REGISTER_DUMP_LEN_MAX  REG_DUMP_COUNT_AR6002
#else
#define REGISTER_DUMP_LEN_MAX  REG_DUMP_COUNT_AR6001
#endif

void ar6000_dump_target_assert_info(HIF_DEVICE *hifDevice, A_UINT32 TargetType)
{
    A_UINT32 address;
    A_UINT32 regDumpArea = 0;
    A_STATUS status;
    A_UINT32 regDumpValues[REGISTER_DUMP_LEN_MAX];
    A_UINT32 regDumpCount = 0;
    A_UINT32 i;

    do {

            /* the reg dump pointer is copied to the host interest area */
        address = HOST_INTEREST_ITEM_ADDRESS(TargetType, hi_failure_state);

        if (TargetType == TARGET_TYPE_AR6001) {
                /* for AR6001, this is a fixed location because the ptr is actually stuck in cache,
                 * this may be fixed in later firmware versions */
            address = 0x18a0;
            regDumpCount = REG_DUMP_COUNT_AR6001;

        } else  if (TargetType == TARGET_TYPE_AR6002) {

            regDumpCount = REG_DUMP_COUNT_AR6002;

        } else {
            A_ASSERT(0);
        }

            /* read RAM location through diagnostic window */
        status = ar6000_ReadRegDiag(hifDevice, &address, &regDumpArea);

        if (A_FAILED(status)) {
            AR_DEBUG_PRINTF(ATH_DEBUG_ERR,("AR6K: Failed to get ptr to register dump area \n"));
            break;
        }

        AR_DEBUG_PRINTF(ATH_DEBUG_ERR,("AR6K: Location of register dump data: 0x%X \n",regDumpArea));

        if (regDumpArea == 0) {
                /* no reg dump */
            break;
        }

        if (TargetType == TARGET_TYPE_AR6001) {
            regDumpArea &= 0x0FFFFFFF;  /* convert to physical address in target memory */
        }

            /* fetch register dump data */
        status = ar6000_ReadDataDiag(hifDevice,
                                     regDumpArea,
                                     (A_UCHAR *)&regDumpValues[0],
                                     regDumpCount * (sizeof(A_UINT32)));

        if (A_FAILED(status)) {
            AR_DEBUG_PRINTF(ATH_DEBUG_ERR,("AR6K: Failed to get register dump \n"));
            break;
        }

        AR_DEBUG_PRINTF(ATH_DEBUG_ERR,("AR6K: Register Dump: \n"));

        for (i = 0; i < regDumpCount; i++) {
            AR_DEBUG_PRINTF(ATH_DEBUG_ERR,(" %d :  0x%8.8X \n",i, regDumpValues[i]));
        }

    } while (FALSE);

}

