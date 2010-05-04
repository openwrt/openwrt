#ifndef _OCF_UIO_H_
#define _OCF_UIO_H_

#include <linux/uio.h>

/*
 * The linux uio.h doesn't have all we need.  To be fully api compatible
 * with the BSD cryptodev,  we need to keep this around.  Perhaps this can
 * be moved back into the linux/uio.h
 *
 * Linux port done by David McCullough <david_mccullough@mcafee.com>
 * Copyright (C) 2006-2010 David McCullough
 * Copyright (C) 2004-2005 Intel Corporation.
 *
 * LICENSE TERMS
 *
 * The free distribution and use of this software in both source and binary
 * form is allowed (with or without changes) provided that:
 *
 *   1. distributions of this source code include the above copyright
 *      notice, this list of conditions and the following disclaimer;
 *
 *   2. distributions in binary form include the above copyright
 *      notice, this list of conditions and the following disclaimer
 *      in the documentation and/or other associated materials;
 *
 *   3. the copyright holder's name is not used to endorse products
 *      built using this software without specific written permission.
 *
 * ALTERNATIVELY, provided that this notice is retained in full, this product
 * may be distributed under the terms of the GNU General Public License (GPL),
 * in which case the provisions of the GPL apply INSTEAD OF those given above.
 *
 * DISCLAIMER
 *
 * This software is provided 'as is' with no explicit or implied warranties
 * in respect of its properties, including, but not limited to, correctness
 * and/or fitness for purpose.
 * ---------------------------------------------------------------------------
 */

struct uio {
	struct	iovec *uio_iov;
	int		uio_iovcnt;
	off_t	uio_offset;
	int		uio_resid;
#if 0
	enum	uio_seg uio_segflg;
	enum	uio_rw uio_rw;
	struct  thread *uio_td;
#endif
};

#endif
