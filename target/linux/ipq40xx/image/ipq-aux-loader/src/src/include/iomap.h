/*
 *
 * Copyright (c) 2015, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of The Linux Foundation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _PLATFORM_IPQ40XX_IOMAP_H_
#define _PLATFORM_IPQ40XX_IOMAP_H_

#ifdef CONFIG_IPQ4XXX
#define CLK_CTL_BASE		0x01800000
#define GCNT_BASE		0x004a1000
#define TIMER_BASE		0x0B021000
#define UART2_DM_BASE		0x078b0000
#define UART1_DM_BASE		0x078af000
#define I2C0_BASE		0x078B7000
#define TLMM_BASE		0x01000000
#define GPIO_CONFIG_ADDR(x) (TLMM_BASE + (x)*0x1000)
#define GPIO_IN_OUT_ADDR(x) (TLMM_BASE + 0x4 + (x)*0x1000)

#endif /* CONFIG_IPQ4XXX */
#ifdef CONFIG_IPQ806X
#define UART7_DM_BASE         	0x16640000

#endif /* CONFIG_IPQ806X */

#endif /* _PLATFORM_IPQ40XX_IOMAP_H_ */
