/*
 * Copyright (c) 2014, 2016-2017, The Linux Foundation. All rights reserved.
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


/**
 * @defgroup fal_stp FAL_STP
 * @{
 */
#ifndef _FAL_STP_H_
#define _FAL_STP_H_

#ifdef __cplusplus
extern "C" {
#endif                          /* __cplusplus */

#include "common/sw.h"
#include "fal/fal_type.h"

#define FAL_SINGLE_STP_ID 0

    /**
    @brief This enum defines port state for spanning tree.
    */
    typedef enum {
        FAL_STP_DISABLED = 0,   /**<   disable state*/
        FAL_STP_BLOCKING,        /**<   blocking state*/
        FAL_STP_LISTENING,      /**<   listening state*/
        FAL_STP_LEARNING,       /**<    learning state*/
        FAL_STP_FORWARDING,     /**<   forwarding state*/
        FAL_STP_STATE_BUTT
    }
    fal_stp_state_t;

/*
 * These two #define lines are used to keep them for the
 * compatibility of previous project.
 */
#define FAL_STP_BLOKING FAL_STP_BLOCKING
#define FAL_STP_FARWARDING FAL_STP_FORWARDING

enum {
	FUNC_STP_PORT_STATE_SET = 0,
	FUNC_STP_PORT_STATE_GET,
};


    sw_error_t
    fal_stp_port_state_set(a_uint32_t dev_id, a_uint32_t st_id,
                           fal_port_t port_id, fal_stp_state_t state);



    sw_error_t
    fal_stp_port_state_get(a_uint32_t dev_id, a_uint32_t st_id,
                           fal_port_t port_id, fal_stp_state_t * state);


#ifdef __cplusplus
}
#endif                          /* __cplusplus */
#endif                          /* _FAL_STP_H_ */

/**
 * @}
 */
