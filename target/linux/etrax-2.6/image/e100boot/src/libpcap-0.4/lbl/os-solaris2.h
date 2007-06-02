/*
 * Copyright (c) 1993, 1994, 1995, 1996, 1997
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that: (1) source code distributions
 * retain the above copyright notice and this paragraph in its entirety, (2)
 * distributions including binary code include the above copyright notice and
 * this paragraph in its entirety in the documentation or other materials
 * provided with the distribution, and (3) all advertising materials mentioning
 * features or use of this software display the following acknowledgement:
 * ``This product includes software developed by the University of California,
 * Lawrence Berkeley Laboratory and its contributors.'' Neither the name of
 * the University nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior
 * written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 * @(#) $Header: /usr/local/cvs/linux/tools/build/e100boot/libpcap-0.4/lbl/os-solaris2.h,v 1.1 1999/08/26 10:11:46 johana Exp $ (LBL)
 */

/* Prototypes missing in SunOS 5 */
int	daemon(int, int);
int	dn_expand(const u_char *, const u_char *, const u_char *, char *, int);
int	dn_skipname(const u_char *, const u_char *);
int	flock(int, int);
int	getdtablesize(void);
int	gethostname(char *, int);
int	getpagesize(void);
char	*getusershell(void);
char	*getwd(char *);
int	iruserok(u_int, int, char *, char *);
#ifdef __STDC__
struct	utmp;
void	login(struct utmp *);
#endif
int	logout(const char *);
int	res_query(const char *, int, int, u_char *, int);
int	setenv(const char *, const char *, int);
#if defined(_STDIO_H) && defined(HAVE_SETLINEBUF)
int	setlinebuf(FILE *);
#endif
int	sigblock(int);
int	sigsetmask(int);
char    *strerror(int);
int	snprintf(char *, size_t, const char *, ...);
int	strcasecmp(const char *, const char *);
void	unsetenv(const char *);
#ifdef __STDC__
struct	timeval;
#endif
int	utimes(const char *, struct timeval *);
