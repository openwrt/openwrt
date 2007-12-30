/*
 * Definitions for Open Firmware client interface on 32-bit system.
 * OF Cell size is 4. Integer properties are encoded big endian,
 * as with all OF implementations.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#ifndef _OFW_H
#define _OFW_H

extern int ofw(char *, int, int, ...); 

#endif
