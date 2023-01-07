/*
 * Copyright (C) 2018 Marco d'Itri
 *
 * Inspired by log.c from the cowdancer package by James Clarke.
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

#ifndef __LOG_H__
#define __LOG_H__

#include <stdarg.h>

#include "utils.h"

typedef enum log_level {
    /* the value of the first 3 bits matches the syslog(3) levels */
    log_emerg = 0,
    log_alert = 1,
    log_crit = 2,
    log_err = 3,
    log_warning = 4,
    log_notice = 5,
    log_info = 6,
    log_debug = 7,
    log_nothing = 8,

    /* the following entries are bit flags to be OR'ed with the level */
    log_stderr = 0x10,
    log_syslog = 0x20,
    log_strerror = 0x40,
} log_level;

/* remove the flags from log_level */
#define LOG_LEVEL_MASK 0xf

log_level log_get_filter_level(void);

void log_set_options(log_level filter_level_new);

__attribute__ ((format(printf, 2, 3)))
void log_printf(log_level level, const char *format, ...);

__attribute__ ((format(printf, 3, 4), noreturn))
void log_printf_exit(int status, log_level level, const char *format, ...);

__attribute__ ((format(printf, 2, 3)))
void log_printf_err(log_level level, const char *format, ...);

__attribute__ ((format(printf, 3, 4), noreturn))
void log_printf_err_exit(int status, log_level level, const char *format, ...);

#define err_sys(...)  log_printf_err_exit(1, log_err, __VA_ARGS__)

#endif				/* !__LOG_H__ */
