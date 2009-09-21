#ifndef _GPIO_API_H_
#define _GPIO_API_H_
/*
 * Copyright 2005 Atheros Communications, Inc.,  All Rights Reserved.
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
 * Host-side General Purpose I/O API.
 *
 * $Id: //depot/sw/releases/olca2.0-GPL/host/include/gpio_api.h#1 $
 */

/*
 * Send a command to the Target in order to change output on GPIO pins.
 */
A_STATUS wmi_gpio_output_set(struct wmi_t *wmip,
                             A_UINT32 set_mask,
                             A_UINT32 clear_mask,
                             A_UINT32 enable_mask,
                             A_UINT32 disable_mask);

/*
 * Send a command to the Target requesting input state of GPIO pins.
 */
A_STATUS wmi_gpio_input_get(struct wmi_t *wmip);

/*
 * Send a command to the Target to change the value of a GPIO register.
 */
A_STATUS wmi_gpio_register_set(struct wmi_t *wmip,
                               A_UINT32 gpioreg_id,
                               A_UINT32 value);

/*
 * Send a command to the Target to fetch the value of a GPIO register.
 */
A_STATUS wmi_gpio_register_get(struct wmi_t *wmip, A_UINT32 gpioreg_id);

/*
 * Send a command to the Target, acknowledging some GPIO interrupts.
 */
A_STATUS wmi_gpio_intr_ack(struct wmi_t *wmip, A_UINT32 ack_mask);

#endif /* _GPIO_API_H_ */
