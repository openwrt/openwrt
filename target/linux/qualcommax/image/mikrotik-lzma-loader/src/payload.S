/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright (C) 2026 André Valentin <avalentin@marcant.net>
 *
 * Embedded payload: LZMA compressed kernel and DTB, in the same PT_LOAD as
 * the code. See loader.lds.
 */
	.section .payload.kernel, "a"
	.incbin "kernel.img.lzma"

	.section .payload.dtb, "a"
	.incbin "payload.dtb"
