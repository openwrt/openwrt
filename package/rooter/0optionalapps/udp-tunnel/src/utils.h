/*
 * Copyright (C) 2018 Marco d'Itri
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#ifndef __UTILS_H__
#define __UTILS_H__

#if !defined __GNUC__ && !defined __attribute__
#define __attribute__(x) /*NOTHING*/
#endif

#ifndef AI_IDN
#define AI_IDN 0
#endif

#ifndef AI_ADDRCONFIG
#define AI_ADDRCONFIG 0
#endif

#ifndef SD_LISTEN_FDS_START
#define SD_LISTEN_FDS_START 0
#define sd_listen_fds(a) 0
#define sd_notify(a, b)
#define sd_is_socket(a, b, c, d) -1
#endif

#if defined __GLIBC__ || (defined __APPLE__ && defined __MACH__)
#define HAVE_GETOPT_LONG
#endif

#ifdef HAVE_GETOPT_LONG
#define GETOPT_LONGISH(c, v, o, l, i) getopt_long(c, v, o, l, i)
#else
#define GETOPT_LONGISH(c, v, o, l, i) getopt(c, v, o)
#endif

#define NOFAIL(ptr) do_nofail((ptr), __FILE__, __LINE__)

void *do_nofail(void *ptr, const char *file, const int line);

#endif
