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

#ifdef KVER32
#include <linux/kconfig.h> 
#include <generated/autoconf.h>
#else
#include <linux/autoconf.h>
#endif
#include <linux/string.h>


#include "nat_helper.h"

char hnat_log_en = HNAT_LOG_LEVEL_DISABLE;
char hnat_log_buffer[NAT_LOG_MAX_SIZE];

void hnat_log_msg(int level, char *string, ...)
{

	va_list   ptr;

	if(level < hnat_log_en)
		return;
	memset(hnat_log_buffer, 0, sizeof(hnat_log_buffer));
	va_start(ptr,string);
	vsnprintf(hnat_log_buffer,sizeof(hnat_log_buffer), string, ptr);
	va_end(ptr);
	aos_printk("%s\n", hnat_log_buffer);
}


sw_error_t
nat_helper_init(uint32_t dev_id, uint32_t portbmp)
{
	nat_helper_bg_task_init();
	host_helper_init(portbmp);
	napt_helper_init();
	nat_ipt_helper_init();

	aos_printk("Hello, nat helper module for 1.1!\n");

	return SW_OK;
}

sw_error_t
nat_helper_cleanup(uint32_t dev_id)
{
    host_helper_exit();
    napt_helper_exit();
    nat_ipt_helper_exit();
	nat_helper_bg_task_exit();

    aos_printk("Goodbye, nat helper module!\n");

    return SW_OK;
}

