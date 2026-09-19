/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef RTL931X_STACK_NAMES_H
#define RTL931X_STACK_NAMES_H

#include <net/if.h>
#include <stddef.h>
#include <stdint.h>

int rtl931x_stack_name_ports(char interfaces[][IFNAMSIZ], size_t count,
			     uint8_t member);

#endif
