/* SPDX-License-Identifier: (GPL-2.0-only OR MIT) */
/*
 * Copyright (C) 2026 John Crispin <john@phrozen.org>
 */

#ifndef _DT_BINDINGS_MAILBOX_DSPG_DVF99_SEC_H
#define _DT_BINDINGS_MAILBOX_DSPG_DVF99_SEC_H

#define DVF99_SEC_OVERWRITE	(1 << 0)
#define DVF99_SEC_POLLED	(1 << 1)

#define DVF99_SEC_LATENCY	0
#define DVF99_SEC_DRAM		1
#define DVF99_SEC_PWR_DOWN	2
#define DVF99_SEC_CLK_CMD	3
#define DVF99_SEC_CLK_ARG	4
#define DVF99_SEC_RST_CMD	5
#define DVF99_SEC_COMA(n)	(16 + (n))
#define DVF99_SEC_CSS_PANIC	40
#define DVF99_SEC_CSS_RELOAD	41
#define DVF99_SEC_CSS_COUNTER	42
#define DVF99_SEC_CSS_SHUTDOWN	43

#endif
