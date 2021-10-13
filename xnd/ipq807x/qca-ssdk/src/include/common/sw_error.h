/*
 * Copyright (c) 2012, The Linux Foundation. All rights reserved.
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all copies.
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */



#ifndef _SW_ERROR_H
#define _SW_ERROR_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

    typedef enum {
        SW_OK              = 0,       /* Operation succeeded                 */
        SW_FAIL            = -1,      /* Operation failed                    */
        SW_BAD_VALUE       = -2,      /* Illegal value                       */
        SW_OUT_OF_RANGE    = -3,      /* Value is out of range               */
        SW_BAD_PARAM       = -4,      /* Illegal parameter(s)                */
        SW_BAD_PTR         = -5,      /* Illegal pointer value               */
        SW_BAD_LEN         = -6,      /* Wrong length                        */
        SW_BAD_STATE       = -7,      /* Wrong state of state machine        */
        SW_READ_ERROR      = -8,      /* Read operation failed               */
        SW_WRITE_ERROR     = -9,      /* Write operation failed              */
        SW_CREATE_ERROR    = -10,     /* Fail in creating an entry           */
        SW_DELETE_ERROR    = -11,     /* Fail in deleteing an entry          */
        SW_NOT_FOUND       = -12,     /* Entry not found                     */
        SW_NO_CHANGE       = -13,     /* The parameter(s) is the same        */
        SW_NO_MORE         = -14,     /* No more entry found                 */
        SW_NO_SUCH         = -15,     /* No such entry                       */
        SW_ALREADY_EXIST   = -16,     /* Tried to create existing entry      */
        SW_FULL            = -17,     /* Table is full                       */
        SW_EMPTY           = -18,     /* Table is empty                      */
        SW_NOT_SUPPORTED   = -19,     /* This request is not support         */
        SW_NOT_IMPLEMENTED = -20,     /* This request is not implemented     */
        SW_NOT_INITIALIZED = -21,     /* The item is not initialized         */
        SW_BUSY            = -22,     /* Operation is still running          */
        SW_TIMEOUT         = -23,     /* Operation Time Out                  */
        SW_DISABLE         = -24,     /* Operation is disabled               */
        SW_NO_RESOURCE     = -25,     /* Resource not available (memory ...) */
        SW_INIT_ERROR      = -26,     /* Error occured while INIT process    */
        SW_NOT_READY       = -27,     /* The other side is not ready yet     */
        SW_OUT_OF_MEM      = -28,     /* Cpu memory allocation failed.       */
        SW_ABORTED         = -29      /* Operation has been aborted.         */
    } sw_error_t;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _SW_ERROR_H */

