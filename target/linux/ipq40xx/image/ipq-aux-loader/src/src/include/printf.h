/*
 * Copyright (C) 2001 MontaVista Software Inc.
 * Author: Jun Sun, jsun@mvista.com or jsun@junsun.net
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _printf_h_
#define _printf_h_

#ifdef DEBUG
#define debug(format, args...) printf(format, ##args)
#else
#define debug(...)
#endif /* DEBUG */

#include <stdarg.h>
int printf(const char *fmt, ...);

#endif /* _printf_h_ */
