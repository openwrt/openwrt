#ifndef __BMI_MSG_H__
#define __BMI_MSG_H__
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

/*
 * Bootloader Messaging Interface (BMI)
 *
 * BMI is a very simple messaging interface used during initialization
 * to read memory, write memory, execute code, and to define an
 * application entry PC.
 *
 * It is used to download an application to AR6K, to provide
 * patches to code that is already resident on AR6K, and generally
 * to examine and modify state.  The Host has an opportunity to use
 * BMI only once during bootup.  Once the Host issues a BMI_DONE
 * command, this opportunity ends.
 *
 * The Host writes BMI requests to mailbox0, and reads BMI responses
 * from mailbox0.   BMI requests all begin with a command
 * (see below for specific commands), and are followed by
 * command-specific data.
 *
 * Flow control:
 * The Host can only issue a command once the Target gives it a
 * "BMI Command Credit", using AR6K Counter #4.  As soon as the
 * Target has completed a command, it issues another BMI Command
 * Credit (so the Host can issue the next command).
 *
 * BMI handles all required Target-side cache flushing.
 */


/* Maximum data size used for BMI transfers */
#define BMI_DATASZ_MAX                     32

/* BMI Commands */

#define BMI_NO_COMMAND                      0

#define BMI_DONE                            1
        /*
         * Semantics: Host is done using BMI
         * Request format:
         *    A_UINT32      command (BMI_DONE)
         * Response format: none
         */

#define BMI_READ_MEMORY                     2
        /*
         * Semantics: Host reads AR6K memory
         * Request format:
         *    A_UINT32      command (BMI_READ_MEMORY)
         *    A_UINT32      address
         *    A_UINT32      length, at most BMI_DATASZ_MAX
         * Response format:
         *    A_UINT8       data[length]
         */

#define BMI_WRITE_MEMORY                    3
        /*
         * Semantics: Host writes AR6K memory
         * Request format:
         *    A_UINT32       command (BMI_WRITE_MEMORY)
         *    A_UINT32      address
         *    A_UINT32      length, at most BMI_DATASZ_MAX
         *    A_UINT8       data[length]
         * Response format: none
         */

#define BMI_EXECUTE                         4
        /*
         * Semantics: Causes AR6K to execute code
         * Request format:
         *    A_UINT32      command (BMI_EXECUTE)
         *    A_UINT32      address
         *    A_UINT32      parameter
         * Response format:
         *    A_UINT32      return value
         */

#define BMI_SET_APP_START                   5
        /*
         * Semantics: Set Target application starting address
         * Request format:
         *    A_UINT32      command (BMI_SET_APP_START)
         *    A_UINT32      address
         * Response format: none
         */

#define BMI_READ_SOC_REGISTER               6
        /*
         * Semantics: Read a 32-bit Target SOC register.
         * Request format:
         *    A_UINT32      command (BMI_READ_REGISTER)
         *    A_UINT32      address
         * Response format:
         *    A_UINT32      value
         */

#define BMI_WRITE_SOC_REGISTER              7
        /*
         * Semantics: Write a 32-bit Target SOC register.
         * Request format:
         *    A_UINT32      command (BMI_WRITE_REGISTER)
         *    A_UINT32      address
         *    A_UINT32      value
         *
         * Response format: none
         */

#define BMI_GET_TARGET_ID                  8
#define BMI_GET_TARGET_INFO                8
        /*
         * Semantics: Fetch the 4-byte Target information
         * Request format:
         *    A_UINT32      command (BMI_GET_TARGET_ID/INFO)
         * Response format1 (old firmware):
         *    A_UINT32      TargetVersionID
         * Response format2 (newer firmware):
         *    A_UINT32      TARGET_VERSION_SENTINAL
         *    struct bmi_target_info;
         */

struct bmi_target_info {
    A_UINT32 target_info_byte_count; /* size of this structure */
    A_UINT32 target_ver;             /* Target Version ID */
    A_UINT32 target_type;            /* Target type */
};
#define TARGET_VERSION_SENTINAL 0xffffffff
#define TARGET_TYPE_AR6001 1
#define TARGET_TYPE_AR6002 2


#define BMI_ROMPATCH_INSTALL               9
        /*
         * Semantics: Install a ROM Patch.
         * Request format:
         *    A_UINT32      command (BMI_ROMPATCH_INSTALL)
         *    A_UINT32      Target ROM Address
         *    A_UINT32      Target RAM Address
         *    A_UINT32      Size, in bytes
         *    A_UINT32      Activate? 1-->activate;
         *                            0-->install but do not activate
         * Response format:
         *    A_UINT32      PatchID
         */

#define BMI_ROMPATCH_UNINSTALL             10
        /*
         * Semantics: Uninstall a previously-installed ROM Patch,
         * automatically deactivating, if necessary.
         * Request format:
         *    A_UINT32      command (BMI_ROMPATCH_UNINSTALL)
         *    A_UINT32      PatchID
         *
         * Response format: none
         */

#define BMI_ROMPATCH_ACTIVATE              11
        /*
         * Semantics: Activate a list of previously-installed ROM Patches.
         * Request format:
         *    A_UINT32      command (BMI_ROMPATCH_ACTIVATE)
         *    A_UINT32      rompatch_count
         *    A_UINT32      PatchID[rompatch_count]
         *
         * Response format: none
         */

#define BMI_ROMPATCH_DEACTIVATE            12
        /*
         * Semantics: Deactivate a list of active ROM Patches.
         * Request format:
         *    A_UINT32      command (BMI_ROMPATCH_DEACTIVATE)
         *    A_UINT32      rompatch_count
         *    A_UINT32      PatchID[rompatch_count]
         *
         * Response format: none
         */


#endif /* __BMI_MSG_H__ */
