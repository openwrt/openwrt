// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2023 Olliver Schinagl <oliver@schinagl.nl>
 */

#include <linux/bitfield.h>
#include <linux/bitops.h>
#include <linux/init.h>
#include <linux/mod_devicetable.h>
#include <linux/printk.h>
#include <linux/regmap.h>
#include <linux/module.h>
#include <linux/string.h>
#include <linux/types.h>

MODULE_AUTHOR("Oliver Schinagl <oliver@schinagl.nl>");
MODULE_DESCRIPTION("Realtek ID driver");
MODULE_LICENSE("GPL"); /* GPLv2 or later */
