/*-
 * Copyright (c) 2006 SPARTA, Inc.
 * All rights reserved.
 *
 * This software was developed by SPARTA ISSO under SPAWAR contract
 * N66001-04-C-6019 ("SEFOS").
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __FreeBSD__
#include <osreldate.h>
#endif

#if !defined(__linux__) && !defined(__OpenBSD__) && !(defined(__APPLE__) && __DARWIN_C_LEVEL >= 200809L) && !(defined(__FreeBSD__) && __FreeBSD_version >= 800000)
/*
 * Emulate glibc getline() via BSD fgetln().
 * Note that outsize is not changed unless memory is allocated.
 */
static inline ssize_t
getline(char **outbuf, size_t *outsize, FILE *fp)
{
	size_t len;

#ifndef __CYGWIN__
	char *buf;
	buf = fgetln(fp, &len);
#else
	char buf[512];
	fgets(buf, sizeof(buf), fp);	
	len = strlen(buf);
#endif
	if (buf == NULL)
		return (-1);

	/* Assumes realloc() accepts NULL for ptr (C99) */
	if (*outbuf == NULL || *outsize < len + 1) {
		void *tmp = realloc(*outbuf, len + 1);
		if (tmp == NULL)
			return (-1);
		*outbuf = tmp;
		*outsize = len + 1;
	}
	memcpy(*outbuf, buf, len);
	(*outbuf)[len] = '\0';
	return (len);
}
#endif
