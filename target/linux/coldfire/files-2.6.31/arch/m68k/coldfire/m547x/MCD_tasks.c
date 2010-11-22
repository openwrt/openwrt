/*
 * drivers/dma/MCD_tasks.c
 *
 * Copyright (C) 2004-2009 Freescale Semiconductor, Inc. All Rights Reserved.
 * Kurt Mahan <kmahan@freescale.com>
 * Shrek Wu b16972@freescale.com
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include "MCD_dma.h"

u32 MCD_varTab0[];
u32 MCD_varTab1[];
u32 MCD_varTab2[];
u32 MCD_varTab3[];
u32 MCD_varTab4[];
u32 MCD_varTab5[];
u32 MCD_varTab6[];
u32 MCD_varTab7[];
u32 MCD_varTab8[];
u32 MCD_varTab9[];
u32 MCD_varTab10[];
u32 MCD_varTab11[];
u32 MCD_varTab12[];
u32 MCD_varTab13[];
u32 MCD_varTab14[];
u32 MCD_varTab15[];

u32 MCD_funcDescTab0[];
#ifdef MCD_INCLUDE_EU
u32 MCD_funcDescTab1[];
u32 MCD_funcDescTab2[];
u32 MCD_funcDescTab3[];
u32 MCD_funcDescTab4[];
u32 MCD_funcDescTab5[];
u32 MCD_funcDescTab6[];
u32 MCD_funcDescTab7[];
u32 MCD_funcDescTab8[];
u32 MCD_funcDescTab9[];
u32 MCD_funcDescTab10[];
u32 MCD_funcDescTab11[];
u32 MCD_funcDescTab12[];
u32 MCD_funcDescTab13[];
u32 MCD_funcDescTab14[];
u32 MCD_funcDescTab15[];
#endif

u32 MCD_contextSave0[];
u32 MCD_contextSave1[];
u32 MCD_contextSave2[];
u32 MCD_contextSave3[];
u32 MCD_contextSave4[];
u32 MCD_contextSave5[];
u32 MCD_contextSave6[];
u32 MCD_contextSave7[];
u32 MCD_contextSave8[];
u32 MCD_contextSave9[];
u32 MCD_contextSave10[];
u32 MCD_contextSave11[];
u32 MCD_contextSave12[];
u32 MCD_contextSave13[];
u32 MCD_contextSave14[];
u32 MCD_contextSave15[];

u32 MCD_realTaskTableSrc[] =
{
    0x00000000,
    0x00000000,
    (u32)MCD_varTab0,   /* Task 0 Variable Table */
    (u32)MCD_funcDescTab0,  /* Task 0 Function Descriptor Table & Flags */
    0x00000000,
    0x00000000,
    (u32)MCD_contextSave0,  /* Task 0 context save space */
    0x00000000,
    0x00000000,
    0x00000000,
    (u32)MCD_varTab1,   /* Task 1 Variable Table */
#ifdef MCD_INCLUDE_EU
    (u32)MCD_funcDescTab1,  /* Task 1 Function Descriptor Table & Flags */
#else
    (u32)MCD_funcDescTab0,  /* Task 0 Function Descriptor Table & Flags */
#endif
    0x00000000,
    0x00000000,
    (u32)MCD_contextSave1,  /* Task 1 context save space */
    0x00000000,
    0x00000000,
    0x00000000,
    (u32)MCD_varTab2,   /* Task 2 Variable Table */
#ifdef MCD_INCLUDE_EU
    (u32)MCD_funcDescTab2,  /* Task 2 Function Descriptor Table & Flags */
#else
    (u32)MCD_funcDescTab0,  /* Task 0 Function Descriptor Table & Flags */
#endif
    0x00000000,
    0x00000000,
    (u32)MCD_contextSave2,  /* Task 2 context save space */
    0x00000000,
    0x00000000,
    0x00000000,
    (u32)MCD_varTab3,   /* Task 3 Variable Table */
#ifdef MCD_INCLUDE_EU
    (u32)MCD_funcDescTab3,  /* Task 3 Function Descriptor Table & Flags */
#else
    (u32)MCD_funcDescTab0,  /* Task 0 Function Descriptor Table & Flags */
#endif
    0x00000000,
    0x00000000,
    (u32)MCD_contextSave3,  /* Task 3 context save space */
    0x00000000,
    0x00000000,
    0x00000000,
    (u32)MCD_varTab4,   /* Task 4 Variable Table */
#ifdef MCD_INCLUDE_EU
    (u32)MCD_funcDescTab4,  /* Task 4 Function Descriptor Table & Flags */
#else
    (u32)MCD_funcDescTab0,  /* Task 0 Function Descriptor Table & Flags */
#endif
    0x00000000,
    0x00000000,
    (u32)MCD_contextSave4,  /* Task 4 context save space */
    0x00000000,
    0x00000000,
    0x00000000,
    (u32)MCD_varTab5,   /* Task 5 Variable Table */
#ifdef MCD_INCLUDE_EU
    (u32)MCD_funcDescTab5,  /* Task 5 Function Descriptor Table & Flags */
#else
    (u32)MCD_funcDescTab0,  /* Task 0 Function Descriptor Table & Flags */
#endif
    0x00000000,
    0x00000000,
    (u32)MCD_contextSave5,  /* Task 5 context save space */
    0x00000000,
    0x00000000,
    0x00000000,
    (u32)MCD_varTab6,   /* Task 6 Variable Table */
#ifdef MCD_INCLUDE_EU
    (u32)MCD_funcDescTab6,  /* Task 6 Function Descriptor Table & Flags */
#else
    (u32)MCD_funcDescTab0,  /* Task 0 Function Descriptor Table & Flags */
#endif
    0x00000000,
    0x00000000,
    (u32)MCD_contextSave6,  /* Task 6 context save space */
    0x00000000,
    0x00000000,
    0x00000000,
    (u32)MCD_varTab7,   /* Task 7 Variable Table */
#ifdef MCD_INCLUDE_EU
    (u32)MCD_funcDescTab7,  /* Task 7 Function Descriptor Table & Flags */
#else
    (u32)MCD_funcDescTab0,  /* Task 0 Function Descriptor Table & Flags */
#endif
    0x00000000,
    0x00000000,
    (u32)MCD_contextSave7,  /* Task 7 context save space */
    0x00000000,
    0x00000000,
    0x00000000,
    (u32)MCD_varTab8,   /* Task 8 Variable Table */
#ifdef MCD_INCLUDE_EU
    (u32)MCD_funcDescTab8,  /* Task 8 Function Descriptor Table & Flags */
#else
    (u32)MCD_funcDescTab0,  /* Task 0 Function Descriptor Table & Flags */
#endif
    0x00000000,
    0x00000000,
    (u32)MCD_contextSave8,  /* Task 8 context save space */
    0x00000000,
    0x00000000,
    0x00000000,
    (u32)MCD_varTab9,   /* Task 9 Variable Table */
#ifdef MCD_INCLUDE_EU
    (u32)MCD_funcDescTab9,  /* Task 9 Function Descriptor Table & Flags */
#else
    (u32)MCD_funcDescTab0,  /* Task 0 Function Descriptor Table & Flags */
#endif
    0x00000000,
    0x00000000,
    (u32)MCD_contextSave9,  /* Task 9 context save space */
    0x00000000,
    0x00000000,
    0x00000000,
    (u32)MCD_varTab10,  /* Task 10 Variable Table */
#ifdef MCD_INCLUDE_EU
    (u32)MCD_funcDescTab10, /* Task 10 Function Descriptor Table & Flags */
#else
    (u32)MCD_funcDescTab0,  /* Task 0 Function Descriptor Table & Flags */
#endif
    0x00000000,
    0x00000000,
    (u32)MCD_contextSave10, /* Task 10 context save space */
    0x00000000,
    0x00000000,
    0x00000000,
    (u32)MCD_varTab11,  /* Task 11 Variable Table */
#ifdef MCD_INCLUDE_EU
    (u32)MCD_funcDescTab11, /* Task 11 Function Descriptor Table & Flags */
#else
    (u32)MCD_funcDescTab0,  /* Task 0 Function Descriptor Table & Flags */
#endif
    0x00000000,
    0x00000000,
    (u32)MCD_contextSave11, /* Task 11 context save space */
    0x00000000,
    0x00000000,
    0x00000000,
    (u32)MCD_varTab12,  /* Task 12 Variable Table */
#ifdef MCD_INCLUDE_EU
    (u32)MCD_funcDescTab12, /* Task 12 Function Descriptor Table & Flags */
#else
    (u32)MCD_funcDescTab0,  /* Task 0 Function Descriptor Table & Flags */
#endif
    0x00000000,
    0x00000000,
    (u32)MCD_contextSave12, /* Task 12 context save space */
    0x00000000,
    0x00000000,
    0x00000000,
    (u32)MCD_varTab13,  /* Task 13 Variable Table */
#ifdef MCD_INCLUDE_EU
    (u32)MCD_funcDescTab13, /* Task 13 Function Descriptor Table & Flags */
#else
    (u32)MCD_funcDescTab0,  /* Task 0 Function Descriptor Table & Flags */
#endif
    0x00000000,
    0x00000000,
    (u32)MCD_contextSave13, /* Task 13 context save space */
    0x00000000,
    0x00000000,
    0x00000000,
    (u32)MCD_varTab14,  /* Task 14 Variable Table */
#ifdef MCD_INCLUDE_EU
    (u32)MCD_funcDescTab14, /* Task 14 Function Descriptor Table & Flags */
#else
    (u32)MCD_funcDescTab0,  /* Task 0 Function Descriptor Table & Flags */
#endif
    0x00000000,
    0x00000000,
    (u32)MCD_contextSave14, /* Task 14 context save space */
    0x00000000,
    0x00000000,
    0x00000000,
    (u32)MCD_varTab15,  /* Task 15 Variable Table */
#ifdef MCD_INCLUDE_EU
    (u32)MCD_funcDescTab15, /* Task 15 Function Descriptor Table & Flags */
#else
    (u32)MCD_funcDescTab0,  /* Task 0 Function Descriptor Table & Flags */
#endif
    0x00000000,
    0x00000000,
    (u32)MCD_contextSave15, /* Task 15 context save space */
    0x00000000,
};


u32 MCD_varTab0[] =
{   /* Task 0 Variable Table */
    0x00000000, /* var[0] */
    0x00000000, /* var[1] */
    0x00000000, /* var[2] */
    0x00000000, /* var[3] */
    0x00000000, /* var[4] */
    0x00000000, /* var[5] */
    0x00000000, /* var[6] */
    0x00000000, /* var[7] */
    0x00000000, /* var[8] */
    0x00000000, /* var[9] */
    0x00000000, /* var[10] */
    0x00000000, /* var[11] */
    0x00000000, /* var[12] */
    0x00000000, /* var[13] */
    0x00000000, /* var[14] */
    0x00000000, /* var[15] */
    0x00000000, /* var[16] */
    0x00000000, /* var[17] */
    0x00000000, /* var[18] */
    0x00000000, /* var[19] */
    0x00000000, /* var[20] */
    0x00000000, /* var[21] */
    0x00000000, /* var[22] */
    0x00000000, /* var[23] */
    0xe0000000, /* inc[0] */
    0x20000000, /* inc[1] */
    0x2000ffff, /* inc[2] */
    0x00000000, /* inc[3] */
    0x00000000, /* inc[4] */
    0x00000000, /* inc[5] */
    0x00000000, /* inc[6] */
    0x00000000, /* inc[7] */
};


u32 MCD_varTab1[] =
{   /* Task 1 Variable Table */
    0x00000000, /* var[0] */
    0x00000000, /* var[1] */
    0x00000000, /* var[2] */
    0x00000000, /* var[3] */
    0x00000000, /* var[4] */
    0x00000000, /* var[5] */
    0x00000000, /* var[6] */
    0x00000000, /* var[7] */
    0x00000000, /* var[8] */
    0x00000000, /* var[9] */
    0x00000000, /* var[10] */
    0x00000000, /* var[11] */
    0x00000000, /* var[12] */
    0x00000000, /* var[13] */
    0x00000000, /* var[14] */
    0x00000000, /* var[15] */
    0x00000000, /* var[16] */
    0x00000000, /* var[17] */
    0x00000000, /* var[18] */
    0x00000000, /* var[19] */
    0x00000000, /* var[20] */
    0x00000000, /* var[21] */
    0x00000000, /* var[22] */
    0x00000000, /* var[23] */
    0xe0000000, /* inc[0] */
    0x20000000, /* inc[1] */
    0x2000ffff, /* inc[2] */
    0x00000000, /* inc[3] */
    0x00000000, /* inc[4] */
    0x00000000, /* inc[5] */
    0x00000000, /* inc[6] */
    0x00000000, /* inc[7] */
};

u32 MCD_varTab2[] =
{   /* Task 2 Variable Table */
    0x00000000, /* var[0] */
    0x00000000, /* var[1] */
    0x00000000, /* var[2] */
    0x00000000, /* var[3] */
    0x00000000, /* var[4] */
    0x00000000, /* var[5] */
    0x00000000, /* var[6] */
    0x00000000, /* var[7] */
    0x00000000, /* var[8] */
    0x00000000, /* var[9] */
    0x00000000, /* var[10] */
    0x00000000, /* var[11] */
    0x00000000, /* var[12] */
    0x00000000, /* var[13] */
    0x00000000, /* var[14] */
    0x00000000, /* var[15] */
    0x00000000, /* var[16] */
    0x00000000, /* var[17] */
    0x00000000, /* var[18] */
    0x00000000, /* var[19] */
    0x00000000, /* var[20] */
    0x00000000, /* var[21] */
    0x00000000, /* var[22] */
    0x00000000, /* var[23] */
    0xe0000000, /* inc[0] */
    0x20000000, /* inc[1] */
    0x2000ffff, /* inc[2] */
    0x00000000, /* inc[3] */
    0x00000000, /* inc[4] */
    0x00000000, /* inc[5] */
    0x00000000, /* inc[6] */
    0x00000000, /* inc[7] */
};

u32 MCD_varTab3[] =
{   /* Task 3 Variable Table */
    0x00000000, /* var[0] */
    0x00000000, /* var[1] */
    0x00000000, /* var[2] */
    0x00000000, /* var[3] */
    0x00000000, /* var[4] */
    0x00000000, /* var[5] */
    0x00000000, /* var[6] */
    0x00000000, /* var[7] */
    0x00000000, /* var[8] */
    0x00000000, /* var[9] */
    0x00000000, /* var[10] */
    0x00000000, /* var[11] */
    0x00000000, /* var[12] */
    0x00000000, /* var[13] */
    0x00000000, /* var[14] */
    0x00000000, /* var[15] */
    0x00000000, /* var[16] */
    0x00000000, /* var[17] */
    0x00000000, /* var[18] */
    0x00000000, /* var[19] */
    0x00000000, /* var[20] */
    0x00000000, /* var[21] */
    0x00000000, /* var[22] */
    0x00000000, /* var[23] */
    0xe0000000, /* inc[0] */
    0x20000000, /* inc[1] */
    0x2000ffff, /* inc[2] */
    0x00000000, /* inc[3] */
    0x00000000, /* inc[4] */
    0x00000000, /* inc[5] */
    0x00000000, /* inc[6] */
    0x00000000, /* inc[7] */
};

u32 MCD_varTab4[] =
{   /* Task 4 Variable Table */
    0x00000000, /* var[0] */
    0x00000000, /* var[1] */
    0x00000000, /* var[2] */
    0x00000000, /* var[3] */
    0x00000000, /* var[4] */
    0x00000000, /* var[5] */
    0x00000000, /* var[6] */
    0x00000000, /* var[7] */
    0x00000000, /* var[8] */
    0x00000000, /* var[9] */
    0x00000000, /* var[10] */
    0x00000000, /* var[11] */
    0x00000000, /* var[12] */
    0x00000000, /* var[13] */
    0x00000000, /* var[14] */
    0x00000000, /* var[15] */
    0x00000000, /* var[16] */
    0x00000000, /* var[17] */
    0x00000000, /* var[18] */
    0x00000000, /* var[19] */
    0x00000000, /* var[20] */
    0x00000000, /* var[21] */
    0x00000000, /* var[22] */
    0x00000000, /* var[23] */
    0xe0000000, /* inc[0] */
    0x20000000, /* inc[1] */
    0x2000ffff, /* inc[2] */
    0x00000000, /* inc[3] */
    0x00000000, /* inc[4] */
    0x00000000, /* inc[5] */
    0x00000000, /* inc[6] */
    0x00000000, /* inc[7] */
};

u32 MCD_varTab5[] =
{   /* Task 5 Variable Table */
    0x00000000, /* var[0] */
    0x00000000, /* var[1] */
    0x00000000, /* var[2] */
    0x00000000, /* var[3] */
    0x00000000, /* var[4] */
    0x00000000, /* var[5] */
    0x00000000, /* var[6] */
    0x00000000, /* var[7] */
    0x00000000, /* var[8] */
    0x00000000, /* var[9] */
    0x00000000, /* var[10] */
    0x00000000, /* var[11] */
    0x00000000, /* var[12] */
    0x00000000, /* var[13] */
    0x00000000, /* var[14] */
    0x00000000, /* var[15] */
    0x00000000, /* var[16] */
    0x00000000, /* var[17] */
    0x00000000, /* var[18] */
    0x00000000, /* var[19] */
    0x00000000, /* var[20] */
    0x00000000, /* var[21] */
    0x00000000, /* var[22] */
    0x00000000, /* var[23] */
    0xe0000000, /* inc[0] */
    0x20000000, /* inc[1] */
    0x2000ffff, /* inc[2] */
    0x00000000, /* inc[3] */
    0x00000000, /* inc[4] */
    0x00000000, /* inc[5] */
    0x00000000, /* inc[6] */
    0x00000000, /* inc[7] */
};

u32 MCD_varTab6[] =
{   /* Task 6 Variable Table */
    0x00000000, /* var[0] */
    0x00000000, /* var[1] */
    0x00000000, /* var[2] */
    0x00000000, /* var[3] */
    0x00000000, /* var[4] */
    0x00000000, /* var[5] */
    0x00000000, /* var[6] */
    0x00000000, /* var[7] */
    0x00000000, /* var[8] */
    0x00000000, /* var[9] */
    0x00000000, /* var[10] */
    0x00000000, /* var[11] */
    0x00000000, /* var[12] */
    0x00000000, /* var[13] */
    0x00000000, /* var[14] */
    0x00000000, /* var[15] */
    0x00000000, /* var[16] */
    0x00000000, /* var[17] */
    0x00000000, /* var[18] */
    0x00000000, /* var[19] */
    0x00000000, /* var[20] */
    0x00000000, /* var[21] */
    0x00000000, /* var[22] */
    0x00000000, /* var[23] */
    0xe0000000, /* inc[0] */
    0x20000000, /* inc[1] */
    0x2000ffff, /* inc[2] */
    0x00000000, /* inc[3] */
    0x00000000, /* inc[4] */
    0x00000000, /* inc[5] */
    0x00000000, /* inc[6] */
    0x00000000, /* inc[7] */
};

u32 MCD_varTab7[] =
{   /* Task 7 Variable Table */
    0x00000000, /* var[0] */
    0x00000000, /* var[1] */
    0x00000000, /* var[2] */
    0x00000000, /* var[3] */
    0x00000000, /* var[4] */
    0x00000000, /* var[5] */
    0x00000000, /* var[6] */
    0x00000000, /* var[7] */
    0x00000000, /* var[8] */
    0x00000000, /* var[9] */
    0x00000000, /* var[10] */
    0x00000000, /* var[11] */
    0x00000000, /* var[12] */
    0x00000000, /* var[13] */
    0x00000000, /* var[14] */
    0x00000000, /* var[15] */
    0x00000000, /* var[16] */
    0x00000000, /* var[17] */
    0x00000000, /* var[18] */
    0x00000000, /* var[19] */
    0x00000000, /* var[20] */
    0x00000000, /* var[21] */
    0x00000000, /* var[22] */
    0x00000000, /* var[23] */
    0xe0000000, /* inc[0] */
    0x20000000, /* inc[1] */
    0x2000ffff, /* inc[2] */
    0x00000000, /* inc[3] */
    0x00000000, /* inc[4] */
    0x00000000, /* inc[5] */
    0x00000000, /* inc[6] */
    0x00000000, /* inc[7] */
};

u32 MCD_varTab8[] =
{   /* Task 8 Variable Table */
    0x00000000, /* var[0] */
    0x00000000, /* var[1] */
    0x00000000, /* var[2] */
    0x00000000, /* var[3] */
    0x00000000, /* var[4] */
    0x00000000, /* var[5] */
    0x00000000, /* var[6] */
    0x00000000, /* var[7] */
    0x00000000, /* var[8] */
    0x00000000, /* var[9] */
    0x00000000, /* var[10] */
    0x00000000, /* var[11] */
    0x00000000, /* var[12] */
    0x00000000, /* var[13] */
    0x00000000, /* var[14] */
    0x00000000, /* var[15] */
    0x00000000, /* var[16] */
    0x00000000, /* var[17] */
    0x00000000, /* var[18] */
    0x00000000, /* var[19] */
    0x00000000, /* var[20] */
    0x00000000, /* var[21] */
    0x00000000, /* var[22] */
    0x00000000, /* var[23] */
    0xe0000000, /* inc[0] */
    0x20000000, /* inc[1] */
    0x2000ffff, /* inc[2] */
    0x00000000, /* inc[3] */
    0x00000000, /* inc[4] */
    0x00000000, /* inc[5] */
    0x00000000, /* inc[6] */
    0x00000000, /* inc[7] */
};

u32 MCD_varTab9[] =
{   /* Task 9 Variable Table */
    0x00000000, /* var[0] */
    0x00000000, /* var[1] */
    0x00000000, /* var[2] */
    0x00000000, /* var[3] */
    0x00000000, /* var[4] */
    0x00000000, /* var[5] */
    0x00000000, /* var[6] */
    0x00000000, /* var[7] */
    0x00000000, /* var[8] */
    0x00000000, /* var[9] */
    0x00000000, /* var[10] */
    0x00000000, /* var[11] */
    0x00000000, /* var[12] */
    0x00000000, /* var[13] */
    0x00000000, /* var[14] */
    0x00000000, /* var[15] */
    0x00000000, /* var[16] */
    0x00000000, /* var[17] */
    0x00000000, /* var[18] */
    0x00000000, /* var[19] */
    0x00000000, /* var[20] */
    0x00000000, /* var[21] */
    0x00000000, /* var[22] */
    0x00000000, /* var[23] */
    0xe0000000, /* inc[0] */
    0x20000000, /* inc[1] */
    0x2000ffff, /* inc[2] */
    0x00000000, /* inc[3] */
    0x00000000, /* inc[4] */
    0x00000000, /* inc[5] */
    0x00000000, /* inc[6] */
    0x00000000, /* inc[7] */
};

u32 MCD_varTab10[] =
{   /* Task 10 Variable Table */
    0x00000000, /* var[0] */
    0x00000000, /* var[1] */
    0x00000000, /* var[2] */
    0x00000000, /* var[3] */
    0x00000000, /* var[4] */
    0x00000000, /* var[5] */
    0x00000000, /* var[6] */
    0x00000000, /* var[7] */
    0x00000000, /* var[8] */
    0x00000000, /* var[9] */
    0x00000000, /* var[10] */
    0x00000000, /* var[11] */
    0x00000000, /* var[12] */
    0x00000000, /* var[13] */
    0x00000000, /* var[14] */
    0x00000000, /* var[15] */
    0x00000000, /* var[16] */
    0x00000000, /* var[17] */
    0x00000000, /* var[18] */
    0x00000000, /* var[19] */
    0x00000000, /* var[20] */
    0x00000000, /* var[21] */
    0x00000000, /* var[22] */
    0x00000000, /* var[23] */
    0xe0000000, /* inc[0] */
    0x20000000, /* inc[1] */
    0x2000ffff, /* inc[2] */
    0x00000000, /* inc[3] */
    0x00000000, /* inc[4] */
    0x00000000, /* inc[5] */
    0x00000000, /* inc[6] */
    0x00000000, /* inc[7] */
};

u32 MCD_varTab11[] =
{   /* Task 11 Variable Table */
    0x00000000, /* var[0] */
    0x00000000, /* var[1] */
    0x00000000, /* var[2] */
    0x00000000, /* var[3] */
    0x00000000, /* var[4] */
    0x00000000, /* var[5] */
    0x00000000, /* var[6] */
    0x00000000, /* var[7] */
    0x00000000, /* var[8] */
    0x00000000, /* var[9] */
    0x00000000, /* var[10] */
    0x00000000, /* var[11] */
    0x00000000, /* var[12] */
    0x00000000, /* var[13] */
    0x00000000, /* var[14] */
    0x00000000, /* var[15] */
    0x00000000, /* var[16] */
    0x00000000, /* var[17] */
    0x00000000, /* var[18] */
    0x00000000, /* var[19] */
    0x00000000, /* var[20] */
    0x00000000, /* var[21] */
    0x00000000, /* var[22] */
    0x00000000, /* var[23] */
    0xe0000000, /* inc[0] */
    0x20000000, /* inc[1] */
    0x2000ffff, /* inc[2] */
    0x00000000, /* inc[3] */
    0x00000000, /* inc[4] */
    0x00000000, /* inc[5] */
    0x00000000, /* inc[6] */
    0x00000000, /* inc[7] */
};

u32 MCD_varTab12[] =
{   /* Task 12 Variable Table */
    0x00000000, /* var[0] */
    0x00000000, /* var[1] */
    0x00000000, /* var[2] */
    0x00000000, /* var[3] */
    0x00000000, /* var[4] */
    0x00000000, /* var[5] */
    0x00000000, /* var[6] */
    0x00000000, /* var[7] */
    0x00000000, /* var[8] */
    0x00000000, /* var[9] */
    0x00000000, /* var[10] */
    0x00000000, /* var[11] */
    0x00000000, /* var[12] */
    0x00000000, /* var[13] */
    0x00000000, /* var[14] */
    0x00000000, /* var[15] */
    0x00000000, /* var[16] */
    0x00000000, /* var[17] */
    0x00000000, /* var[18] */
    0x00000000, /* var[19] */
    0x00000000, /* var[20] */
    0x00000000, /* var[21] */
    0x00000000, /* var[22] */
    0x00000000, /* var[23] */
    0xe0000000, /* inc[0] */
    0x20000000, /* inc[1] */
    0x2000ffff, /* inc[2] */
    0x00000000, /* inc[3] */
    0x00000000, /* inc[4] */
    0x00000000, /* inc[5] */
    0x00000000, /* inc[6] */
    0x00000000, /* inc[7] */
};

u32 MCD_varTab13[] =
{   /* Task 13 Variable Table */
    0x00000000, /* var[0] */
    0x00000000, /* var[1] */
    0x00000000, /* var[2] */
    0x00000000, /* var[3] */
    0x00000000, /* var[4] */
    0x00000000, /* var[5] */
    0x00000000, /* var[6] */
    0x00000000, /* var[7] */
    0x00000000, /* var[8] */
    0x00000000, /* var[9] */
    0x00000000, /* var[10] */
    0x00000000, /* var[11] */
    0x00000000, /* var[12] */
    0x00000000, /* var[13] */
    0x00000000, /* var[14] */
    0x00000000, /* var[15] */
    0x00000000, /* var[16] */
    0x00000000, /* var[17] */
    0x00000000, /* var[18] */
    0x00000000, /* var[19] */
    0x00000000, /* var[20] */
    0x00000000, /* var[21] */
    0x00000000, /* var[22] */
    0x00000000, /* var[23] */
    0xe0000000, /* inc[0] */
    0x20000000, /* inc[1] */
    0x2000ffff, /* inc[2] */
    0x00000000, /* inc[3] */
    0x00000000, /* inc[4] */
    0x00000000, /* inc[5] */
    0x00000000, /* inc[6] */
    0x00000000, /* inc[7] */
};

u32 MCD_varTab14[] =
{   /* Task 14 Variable Table */
    0x00000000, /* var[0] */
    0x00000000, /* var[1] */
    0x00000000, /* var[2] */
    0x00000000, /* var[3] */
    0x00000000, /* var[4] */
    0x00000000, /* var[5] */
    0x00000000, /* var[6] */
    0x00000000, /* var[7] */
    0x00000000, /* var[8] */
    0x00000000, /* var[9] */
    0x00000000, /* var[10] */
    0x00000000, /* var[11] */
    0x00000000, /* var[12] */
    0x00000000, /* var[13] */
    0x00000000, /* var[14] */
    0x00000000, /* var[15] */
    0x00000000, /* var[16] */
    0x00000000, /* var[17] */
    0x00000000, /* var[18] */
    0x00000000, /* var[19] */
    0x00000000, /* var[20] */
    0x00000000, /* var[21] */
    0x00000000, /* var[22] */
    0x00000000, /* var[23] */
    0xe0000000, /* inc[0] */
    0x20000000, /* inc[1] */
    0x2000ffff, /* inc[2] */
    0x00000000, /* inc[3] */
    0x00000000, /* inc[4] */
    0x00000000, /* inc[5] */
    0x00000000, /* inc[6] */
    0x00000000, /* inc[7] */
};

u32 MCD_varTab15[] =
{   /* Task 15 Variable Table */
    0x00000000, /* var[0] */
    0x00000000, /* var[1] */
    0x00000000, /* var[2] */
    0x00000000, /* var[3] */
    0x00000000, /* var[4] */
    0x00000000, /* var[5] */
    0x00000000, /* var[6] */
    0x00000000, /* var[7] */
    0x00000000, /* var[8] */
    0x00000000, /* var[9] */
    0x00000000, /* var[10] */
    0x00000000, /* var[11] */
    0x00000000, /* var[12] */
    0x00000000, /* var[13] */
    0x00000000, /* var[14] */
    0x00000000, /* var[15] */
    0x00000000, /* var[16] */
    0x00000000, /* var[17] */
    0x00000000, /* var[18] */
    0x00000000, /* var[19] */
    0x00000000, /* var[20] */
    0x00000000, /* var[21] */
    0x00000000, /* var[22] */
    0x00000000, /* var[23] */
    0xe0000000, /* inc[0] */
    0x20000000, /* inc[1] */
    0x2000ffff, /* inc[2] */
    0x00000000, /* inc[3] */
    0x00000000, /* inc[4] */
    0x00000000, /* inc[5] */
    0x00000000, /* inc[6] */
    0x00000000, /* inc[7] */
};

u32 MCD_funcDescTab0[] =
{   /* Task 0 Function Descriptor Table */
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0xa0045670, /* mainFunc(), EU# 3 */
    0xa0000000, /* rsduFunc(), EU# 3 */
    0xa0000000, /* crcAccumVal(), EU# 3 */
    0x20000000, /* setCrcAccum(), EU# 3 */
    0x21800000, /* and(), EU# 3 */
    0x21e00000, /* or(), EU# 3 */
    0x20400000, /* add(), EU# 3 */
    0x20500000, /* sub(), EU# 3 */
    0x205a0000, /* andNot(), EU# 3 */
    0x20a00000, /* shiftR(), EU# 3 */
    0x202fa000, /* andReadyBit(), EU# 3 */
    0x202f9000, /* andNotReadyBit(), EU# 3 */
    0x202ea000, /* andWrapBit(), EU# 3 */
    0x202da000, /* andLastBit(), EU# 3 */
    0x202e2000, /* andInterruptBit(), EU# 3 */
    0x202f2000, /* andCrcRestartBit(), EU# 3 */
};

#ifdef MCD_INCLUDE_EU
u32 MCD_funcDescTab1[] =
{   /* Task 1 Function Descriptor Table */
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0xa0045670, /* mainFunc(), EU# 3 */
    0xa0000000, /* rsduFunc(), EU# 3 */
    0xa0000000, /* crcAccumVal(), EU# 3 */
    0x20000000, /* setCrcAccum(), EU# 3 */
    0x21800000, /* and(), EU# 3 */
    0x21e00000, /* or(), EU# 3 */
    0x20400000, /* add(), EU# 3 */
    0x20500000, /* sub(), EU# 3 */
    0x205a0000, /* andNot(), EU# 3 */
    0x20a00000, /* shiftR(), EU# 3 */
    0x202fa000, /* andReadyBit(), EU# 3 */
    0x202f9000, /* andNotReadyBit(), EU# 3 */
    0x202ea000, /* andWrapBit(), EU# 3 */
    0x202da000, /* andLastBit(), EU# 3 */
    0x202e2000, /* andInterruptBit(), EU# 3 */
    0x202f2000, /* andCrcRestartBit(), EU# 3 */
};

u32 MCD_funcDescTab2[] =
{   /* Task 2 Function Descriptor Table */
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0xa0045670, /* mainFunc(), EU# 3 */
    0xa0000000, /* rsduFunc(), EU# 3 */
    0xa0000000, /* crcAccumVal(), EU# 3 */
    0x20000000, /* setCrcAccum(), EU# 3 */
    0x21800000, /* and(), EU# 3 */
    0x21e00000, /* or(), EU# 3 */
    0x20400000, /* add(), EU# 3 */
    0x20500000, /* sub(), EU# 3 */
    0x205a0000, /* andNot(), EU# 3 */
    0x20a00000, /* shiftR(), EU# 3 */
    0x202fa000, /* andReadyBit(), EU# 3 */
    0x202f9000, /* andNotReadyBit(), EU# 3 */
    0x202ea000, /* andWrapBit(), EU# 3 */
    0x202da000, /* andLastBit(), EU# 3 */
    0x202e2000, /* andInterruptBit(), EU# 3 */
    0x202f2000, /* andCrcRestartBit(), EU# 3 */
};

u32 MCD_funcDescTab3[] =
{   /* Task 3 Function Descriptor Table */
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0xa0045670, /* mainFunc(), EU# 3 */
    0xa0000000, /* rsduFunc(), EU# 3 */
    0xa0000000, /* crcAccumVal(), EU# 3 */
    0x20000000, /* setCrcAccum(), EU# 3 */
    0x21800000, /* and(), EU# 3 */
    0x21e00000, /* or(), EU# 3 */
    0x20400000, /* add(), EU# 3 */
    0x20500000, /* sub(), EU# 3 */
    0x205a0000, /* andNot(), EU# 3 */
    0x20a00000, /* shiftR(), EU# 3 */
    0x202fa000, /* andReadyBit(), EU# 3 */
    0x202f9000, /* andNotReadyBit(), EU# 3 */
    0x202ea000, /* andWrapBit(), EU# 3 */
    0x202da000, /* andLastBit(), EU# 3 */
    0x202e2000, /* andInterruptBit(), EU# 3 */
    0x202f2000, /* andCrcRestartBit(), EU# 3 */
};

u32 MCD_funcDescTab4[] =
{   /* Task 4 Function Descriptor Table */
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0xa0045670, /* mainFunc(), EU# 3 */
    0xa0000000, /* rsduFunc(), EU# 3 */
    0xa0000000, /* crcAccumVal(), EU# 3 */
    0x20000000, /* setCrcAccum(), EU# 3 */
    0x21800000, /* and(), EU# 3 */
    0x21e00000, /* or(), EU# 3 */
    0x20400000, /* add(), EU# 3 */
    0x20500000, /* sub(), EU# 3 */
    0x205a0000, /* andNot(), EU# 3 */
    0x20a00000, /* shiftR(), EU# 3 */
    0x202fa000, /* andReadyBit(), EU# 3 */
    0x202f9000, /* andNotReadyBit(), EU# 3 */
    0x202ea000, /* andWrapBit(), EU# 3 */
    0x202da000, /* andLastBit(), EU# 3 */
    0x202e2000, /* andInterruptBit(), EU# 3 */
    0x202f2000, /* andCrcRestartBit(), EU# 3 */
};

u32 MCD_funcDescTab5[] =
{   /* Task 5 Function Descriptor Table */
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0xa0045670, /* mainFunc(), EU# 3 */
    0xa0000000, /* rsduFunc(), EU# 3 */
    0xa0000000, /* crcAccumVal(), EU# 3 */
    0x20000000, /* setCrcAccum(), EU# 3 */
    0x21800000, /* and(), EU# 3 */
    0x21e00000, /* or(), EU# 3 */
    0x20400000, /* add(), EU# 3 */
    0x20500000, /* sub(), EU# 3 */
    0x205a0000, /* andNot(), EU# 3 */
    0x20a00000, /* shiftR(), EU# 3 */
    0x202fa000, /* andReadyBit(), EU# 3 */
    0x202f9000, /* andNotReadyBit(), EU# 3 */
    0x202ea000, /* andWrapBit(), EU# 3 */
    0x202da000, /* andLastBit(), EU# 3 */
    0x202e2000, /* andInterruptBit(), EU# 3 */
    0x202f2000, /* andCrcRestartBit(), EU# 3 */
};

u32 MCD_funcDescTab6[] =
{   /* Task 6 Function Descriptor Table */
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0xa0045670, /* mainFunc(), EU# 3 */
    0xa0000000, /* rsduFunc(), EU# 3 */
    0xa0000000, /* crcAccumVal(), EU# 3 */
    0x20000000, /* setCrcAccum(), EU# 3 */
    0x21800000, /* and(), EU# 3 */
    0x21e00000, /* or(), EU# 3 */
    0x20400000, /* add(), EU# 3 */
    0x20500000, /* sub(), EU# 3 */
    0x205a0000, /* andNot(), EU# 3 */
    0x20a00000, /* shiftR(), EU# 3 */
    0x202fa000, /* andReadyBit(), EU# 3 */
    0x202f9000, /* andNotReadyBit(), EU# 3 */
    0x202ea000, /* andWrapBit(), EU# 3 */
    0x202da000, /* andLastBit(), EU# 3 */
    0x202e2000, /* andInterruptBit(), EU# 3 */
    0x202f2000, /* andCrcRestartBit(), EU# 3 */
};

u32 MCD_funcDescTab7[] =
{   /* Task 7 Function Descriptor Table */
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0xa0045670, /* mainFunc(), EU# 3 */
    0xa0000000, /* rsduFunc(), EU# 3 */
    0xa0000000, /* crcAccumVal(), EU# 3 */
    0x20000000, /* setCrcAccum(), EU# 3 */
    0x21800000, /* and(), EU# 3 */
    0x21e00000, /* or(), EU# 3 */
    0x20400000, /* add(), EU# 3 */
    0x20500000, /* sub(), EU# 3 */
    0x205a0000, /* andNot(), EU# 3 */
    0x20a00000, /* shiftR(), EU# 3 */
    0x202fa000, /* andReadyBit(), EU# 3 */
    0x202f9000, /* andNotReadyBit(), EU# 3 */
    0x202ea000, /* andWrapBit(), EU# 3 */
    0x202da000, /* andLastBit(), EU# 3 */
    0x202e2000, /* andInterruptBit(), EU# 3 */
    0x202f2000, /* andCrcRestartBit(), EU# 3 */
};

u32 MCD_funcDescTab8[] =
{   /* Task 8 Function Descriptor Table */
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0xa0045670, /* mainFunc(), EU# 3 */
    0xa0000000, /* rsduFunc(), EU# 3 */
    0xa0000000, /* crcAccumVal(), EU# 3 */
    0x20000000, /* setCrcAccum(), EU# 3 */
    0x21800000, /* and(), EU# 3 */
    0x21e00000, /* or(), EU# 3 */
    0x20400000, /* add(), EU# 3 */
    0x20500000, /* sub(), EU# 3 */
    0x205a0000, /* andNot(), EU# 3 */
    0x20a00000, /* shiftR(), EU# 3 */
    0x202fa000, /* andReadyBit(), EU# 3 */
    0x202f9000, /* andNotReadyBit(), EU# 3 */
    0x202ea000, /* andWrapBit(), EU# 3 */
    0x202da000, /* andLastBit(), EU# 3 */
    0x202e2000, /* andInterruptBit(), EU# 3 */
    0x202f2000, /* andCrcRestartBit(), EU# 3 */
};

u32 MCD_funcDescTab9[] =
{   /* Task 9 Function Descriptor Table */
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0xa0045670, /* mainFunc(), EU# 3 */
    0xa0000000, /* rsduFunc(), EU# 3 */
    0xa0000000, /* crcAccumVal(), EU# 3 */
    0x20000000, /* setCrcAccum(), EU# 3 */
    0x21800000, /* and(), EU# 3 */
    0x21e00000, /* or(), EU# 3 */
    0x20400000, /* add(), EU# 3 */
    0x20500000, /* sub(), EU# 3 */
    0x205a0000, /* andNot(), EU# 3 */
    0x20a00000, /* shiftR(), EU# 3 */
    0x202fa000, /* andReadyBit(), EU# 3 */
    0x202f9000, /* andNotReadyBit(), EU# 3 */
    0x202ea000, /* andWrapBit(), EU# 3 */
    0x202da000, /* andLastBit(), EU# 3 */
    0x202e2000, /* andInterruptBit(), EU# 3 */
    0x202f2000, /* andCrcRestartBit(), EU# 3 */
};

u32 MCD_funcDescTab10[] =
{   /* Task 10 Function Descriptor Table */
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0xa0045670, /* mainFunc(), EU# 3 */
    0xa0000000, /* rsduFunc(), EU# 3 */
    0xa0000000, /* crcAccumVal(), EU# 3 */
    0x20000000, /* setCrcAccum(), EU# 3 */
    0x21800000, /* and(), EU# 3 */
    0x21e00000, /* or(), EU# 3 */
    0x20400000, /* add(), EU# 3 */
    0x20500000, /* sub(), EU# 3 */
    0x205a0000, /* andNot(), EU# 3 */
    0x20a00000, /* shiftR(), EU# 3 */
    0x202fa000, /* andReadyBit(), EU# 3 */
    0x202f9000, /* andNotReadyBit(), EU# 3 */
    0x202ea000, /* andWrapBit(), EU# 3 */
    0x202da000, /* andLastBit(), EU# 3 */
    0x202e2000, /* andInterruptBit(), EU# 3 */
    0x202f2000, /* andCrcRestartBit(), EU# 3 */
};

u32 MCD_funcDescTab11[] =
{   /* Task 11 Function Descriptor Table */
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0xa0045670, /* mainFunc(), EU# 3 */
    0xa0000000, /* rsduFunc(), EU# 3 */
    0xa0000000, /* crcAccumVal(), EU# 3 */
    0x20000000, /* setCrcAccum(), EU# 3 */
    0x21800000, /* and(), EU# 3 */
    0x21e00000, /* or(), EU# 3 */
    0x20400000, /* add(), EU# 3 */
    0x20500000, /* sub(), EU# 3 */
    0x205a0000, /* andNot(), EU# 3 */
    0x20a00000, /* shiftR(), EU# 3 */
    0x202fa000, /* andReadyBit(), EU# 3 */
    0x202f9000, /* andNotReadyBit(), EU# 3 */
    0x202ea000, /* andWrapBit(), EU# 3 */
    0x202da000, /* andLastBit(), EU# 3 */
    0x202e2000, /* andInterruptBit(), EU# 3 */
    0x202f2000, /* andCrcRestartBit(), EU# 3 */
};

u32 MCD_funcDescTab12[] =
{   /* Task 12 Function Descriptor Table */
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0xa0045670, /* mainFunc(), EU# 3 */
    0xa0000000, /* rsduFunc(), EU# 3 */
    0xa0000000, /* crcAccumVal(), EU# 3 */
    0x20000000, /* setCrcAccum(), EU# 3 */
    0x21800000, /* and(), EU# 3 */
    0x21e00000, /* or(), EU# 3 */
    0x20400000, /* add(), EU# 3 */
    0x20500000, /* sub(), EU# 3 */
    0x205a0000, /* andNot(), EU# 3 */
    0x20a00000, /* shiftR(), EU# 3 */
    0x202fa000, /* andReadyBit(), EU# 3 */
    0x202f9000, /* andNotReadyBit(), EU# 3 */
    0x202ea000, /* andWrapBit(), EU# 3 */
    0x202da000, /* andLastBit(), EU# 3 */
    0x202e2000, /* andInterruptBit(), EU# 3 */
    0x202f2000, /* andCrcRestartBit(), EU# 3 */
};

u32 MCD_funcDescTab13[] =
{   /* Task 13 Function Descriptor Table */
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0xa0045670, /* mainFunc(), EU# 3 */
    0xa0000000, /* rsduFunc(), EU# 3 */
    0xa0000000, /* crcAccumVal(), EU# 3 */
    0x20000000, /* setCrcAccum(), EU# 3 */
    0x21800000, /* and(), EU# 3 */
    0x21e00000, /* or(), EU# 3 */
    0x20400000, /* add(), EU# 3 */
    0x20500000, /* sub(), EU# 3 */
    0x205a0000, /* andNot(), EU# 3 */
    0x20a00000, /* shiftR(), EU# 3 */
    0x202fa000, /* andReadyBit(), EU# 3 */
    0x202f9000, /* andNotReadyBit(), EU# 3 */
    0x202ea000, /* andWrapBit(), EU# 3 */
    0x202da000, /* andLastBit(), EU# 3 */
    0x202e2000, /* andInterruptBit(), EU# 3 */
    0x202f2000, /* andCrcRestartBit(), EU# 3 */
};

u32 MCD_funcDescTab14[] =
{   /* Task 14 Function Descriptor Table */
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0xa0045670, /* mainFunc(), EU# 3 */
    0xa0000000, /* rsduFunc(), EU# 3 */
    0xa0000000, /* crcAccumVal(), EU# 3 */
    0x20000000, /* setCrcAccum(), EU# 3 */
    0x21800000, /* and(), EU# 3 */
    0x21e00000, /* or(), EU# 3 */
    0x20400000, /* add(), EU# 3 */
    0x20500000, /* sub(), EU# 3 */
    0x205a0000, /* andNot(), EU# 3 */
    0x20a00000, /* shiftR(), EU# 3 */
    0x202fa000, /* andReadyBit(), EU# 3 */
    0x202f9000, /* andNotReadyBit(), EU# 3 */
    0x202ea000, /* andWrapBit(), EU# 3 */
    0x202da000, /* andLastBit(), EU# 3 */
    0x202e2000, /* andInterruptBit(), EU# 3 */
    0x202f2000, /* andCrcRestartBit(), EU# 3 */
};

u32 MCD_funcDescTab15[] =
{   /* Task 15 Function Descriptor Table */
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0xa0045670, /* mainFunc(), EU# 3 */
    0xa0000000, /* rsduFunc(), EU# 3 */
    0xa0000000, /* crcAccumVal(), EU# 3 */
    0x20000000, /* setCrcAccum(), EU# 3 */
    0x21800000, /* and(), EU# 3 */
    0x21e00000, /* or(), EU# 3 */
    0x20400000, /* add(), EU# 3 */
    0x20500000, /* sub(), EU# 3 */
    0x205a0000, /* andNot(), EU# 3 */
    0x20a00000, /* shiftR(), EU# 3 */
    0x202fa000, /* andReadyBit(), EU# 3 */
    0x202f9000, /* andNotReadyBit(), EU# 3 */
    0x202ea000, /* andWrapBit(), EU# 3 */
    0x202da000, /* andLastBit(), EU# 3 */
    0x202e2000, /* andInterruptBit(), EU# 3 */
    0x202f2000, /* andCrcRestartBit(), EU# 3 */
};
#endif /*MCD_INCLUDE_EU*/

u32 MCD_contextSave0[128];  /* Task 0 context save space */
u32 MCD_contextSave1[128];  /* Task 1 context save space */
u32 MCD_contextSave2[128];  /* Task 2 context save space */
u32 MCD_contextSave3[128];  /* Task 3 context save space */
u32 MCD_contextSave4[128];  /* Task 4 context save space */
u32 MCD_contextSave5[128];  /* Task 5 context save space */
u32 MCD_contextSave6[128];  /* Task 6 context save space */
u32 MCD_contextSave7[128];  /* Task 7 context save space */
u32 MCD_contextSave8[128];  /* Task 8 context save space */
u32 MCD_contextSave9[128];  /* Task 9 context save space */
u32 MCD_contextSave10[128]; /* Task 10 context save space */
u32 MCD_contextSave11[128]; /* Task 11 context save space */
u32 MCD_contextSave12[128]; /* Task 12 context save space */
u32 MCD_contextSave13[128]; /* Task 13 context save space */
u32 MCD_contextSave14[128]; /* Task 14 context save space */
u32 MCD_contextSave15[128]; /* Task 15 context save space */

u32 MCD_ChainNoEu_TDT[];
u32 MCD_SingleNoEu_TDT[];
#ifdef MCD_INCLUDE_EU
u32 MCD_ChainEu_TDT[];
u32 MCD_SingleEu_TDT[];
#endif
u32 MCD_ENetRcv_TDT[];
u32 MCD_ENetXmit_TDT[];

u32 MCD_modelTaskTableSrc[] =
{
    (u32)MCD_ChainNoEu_TDT,
    (u32)&((u8 *)MCD_ChainNoEu_TDT)[0x0000016c],
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    (u32)MCD_SingleNoEu_TDT,
    (u32)&((u8 *)MCD_SingleNoEu_TDT)[0x000000d4],
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
#ifdef MCD_INCLUDE_EU
    (u32)MCD_ChainEu_TDT,
    (u32)&((u8 *)MCD_ChainEu_TDT)[0x000001b4],
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    (u32)MCD_SingleEu_TDT,
    (u32)&((u8 *)MCD_SingleEu_TDT)[0x00000124],
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
#endif
    (u32)MCD_ENetRcv_TDT,
    (u32)&((u8 *)MCD_ENetRcv_TDT)[0x000000a4],
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    (u32)MCD_ENetXmit_TDT,
    (u32)&((u8 *)MCD_ENetXmit_TDT)[0x000000d0],
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
};
u32 MCD_ChainNoEu_TDT[] =
{
    0x80004000,
    0x8118801b,
    0xb8c60018,
    0x10002b10,
    0x7000000d,
    0x018cf89f,
    0x6000000a,
    0x080cf89f,
    0x000001f8,
    0x98180364,
    0x8118801b,
    0xf8c6001a,
    0xb8c6601b,
    0x10002710,
    0x00000f18,
    0xb8c6001d,
    0x10001310,
    0x60000007,
    0x014cf88b,
    0x98c6001c,
    0x00000710,
    0x98c70018,
    0x10001f10,
    0x0000c818,
    0x000001f8, /* 0060(:0):      NOP */
    0xc1476018,
    0xc003231d,
    0x811a601b,
    0xc1862102,
    0x849be009,
    0x03fed7b8,
    0xda9b001b,
    0x9b9be01b,
    0x1000cb20,
    0x70000006,
    0x088cf88f,
    0x1000cb28,
    0x70000006,
    0x088cf88f,
    0x1000cb30,
    0x70000006,
    0x088cf88f,
    0x1000cb38,
    0x0000c728,
    0x000001f8, /* 00B0(:0):      NOP */
    0xc1476018,
    0xc003241d,
    0x811a601b,
    0xda9b001b,
    0x9b9be01b,
    0x0000d3a0,
    0xc1862102,
    0x849be009,
    0x0bfed7b8,
    0xda9b001b,
    0x9b9be01b,
    0x1000cb20,
    0x70000006,
    0x088cf88f,
    0x1000cb28,
    0x70000006,
    0x088cf88f,
    0x1000cb30,
    0x70000006,
    0x088cf88f,
    0x1000cb38,
    0x0000c728,
    0x000001f8, /* 010C(:0):      NOP */
    0x8118801b,
    0xd8c60018,
    0x98c6601c,
    0x6000000b,
    0x0c8cfc9f,
    0x000001f8, /* 0124(:0):      NOP */
    0xa146001e,
    0x10000b08,
    0x10002050,
    0xb8c60018,
    0x10002b10,
    0x7000000a,
    0x080cf89f,
    0x6000000d,
    0x018cf89f,
    0x000001f8, /* 014C(:0):      NOP */
    0x8618801b,
    0x7000000e,
    0x084cf21f,
    0xd8990336,
    0x8019801b,
    0x040001f8,
    0x000001f8, /* 0168(:0):      NOP */
    0x000001f8, /* 016C(:0):    NOP */
};
u32 MCD_SingleNoEu_TDT[] =
{
    0x8198001b,
    0x7000000d,
    0x080cf81f,
    0x8198801b,
    0x6000000e,
    0x084cf85f,
    0x000001f8, /* 0018(:0):    NOP */
    0x8298001b,
    0x7000000d,
    0x010cf81f,
    0x6000000e,
    0x018cf81f,
    0xc202601b,
    0xc002221c,
    0x809a601b,
    0xc10420c2,
    0x839be009,
    0x03fed7b8,
    0xda9b001b,
    0x9b9be01b,
    0x70000006,
    0x088cf889,
    0x1000cb28,
    0x70000006,
    0x088cf889,
    0x1000cb30,
    0x70000006,
    0x088cf889,
    0x0000cb38,
    0x000001f8, /* 0074(:0):    NOP */
    0xc202601b,
    0xc002229c,
    0x809a601b,
    0xda9b001b,
    0x9b9be01b,
    0x0000d3a0,
    0xc10420c2,
    0x839be009,
    0x0bfed7b8,
    0xda9b001b,
    0x9b9be01b,
    0x70000006,
    0x088cf889,
    0x1000cb28,
    0x70000006,
    0x088cf889,
    0x1000cb30,
    0x70000006,
    0x088cf889,
    0x0000cb38,
    0x000001f8, /* 00C8(:0):    NOP */
    0xc318022d,
    0x8018801b,
    0x040001f8,
};
#ifdef MCD_INCLUDE_EU
u32 MCD_ChainEu_TDT[] =
{
    0x80004000,
    0x8198801b,
    0xb8c68018,
    0x10002f10,
    0x7000000d,
    0x01ccf89f,
    0x6000000a,
    0x080cf89f,
    0x000001f8,
    0x981803a4,
    0x8198801b,
    0xf8c6801a,
    0xb8c6e01b,
    0x10002b10,
    0x00001318,
    0xb8c6801d,
    0x10001710,
    0x60000007,
    0x018cf88c,
    0x98c6801c,
    0x00000b10,
    0x98c78018,
    0x10002310,
    0x0000c820,
    0x000001f8, /* 0060(:0):      NOP */
    0x8698801b,
    0x7000000f,
    0x084cf2df,
    0xd899042d,
    0x8019801b,
    0x60000003,
    0x2cd7c7df, /* 007C(:979):        DRD2B2: EU3(var13)  */
    0xd8990364,
    0x8019801b,
    0x60000003,
    0x2c17c7df, /* 008C(:981):        DRD2B2: EU3(var1)  */
    0x000001f8, /* 0090(:0):      NOP */
    0xc1c7e018,
    0xc003a35e,
    0x819a601b,
    0xc206a142,
    0x851be009,
    0x63fe0000,
    0x0d4cfddf,
    0xda9b001b,
    0x9b9be01b,
    0x70000002,
    0x004cf81f,
    0x1000cb20,
    0x70000006,
    0x088cf891,
    0x1000cb28,
    0x70000006,
    0x088cf891,
    0x1000cb30,
    0x70000006,
    0x088cf891,
    0x1000cb38,
    0x0000c728,
    0x000001f8, /* 00EC(:0):      NOP */
    0xc1c7e018,
    0xc003a49e,
    0x819a601b,
    0xda9b001b,
    0x9b9be01b,
    0x0000d3a0,
    0xc206a142,
    0x851be009,
    0x6bfe0000,
    0x0d4cfddf,
    0xda9b001b,
    0x9b9be01b,
    0x70000002,
    0x004cf81f,
    0x1000cb20,
    0x70000006,
    0x088cf891,
    0x1000cb28,
    0x70000006,
    0x088cf891,
    0x1000cb30,
    0x70000006,
    0x088cf891,
    0x1000cb38,
    0x0000c728,
    0x000001f8, /* 0154(:0):      NOP */
    0x8198801b,
    0xd8c68018,
    0x98c6e01c,
    0x6000000b,
    0x0c8cfc9f,
    0x0000cc08,
    0xa1c6801e,
    0x10000f08,
    0x10002458,
    0xb8c68018,
    0x10002f10,
    0x7000000a,
    0x080cf89f,
    0x6000000d,
    0x01ccf89f,
    0x000001f8, /* 0194(:0):      NOP */
    0x8698801b,
    0x7000000e,
    0x084cf25f,
    0xd899037f,
    0x8019801b,
    0x040001f8,
    0x000001f8, /* 01B0(:0):      NOP */
    0x000001f8, /* 01B4(:0):    NOP */
};
u32 MCD_SingleEu_TDT[] =
{
    0x8218001b,
    0x7000000d,
    0x080cf81f,
    0x8218801b,
    0x6000000e,
    0x084cf85f,
    0x000001f8, /* 0018(:0):    NOP */
    0x8318001b,
    0x7000000d,
    0x014cf81f,
    0x6000000e,
    0x01ccf81f,
    0x8498001b,
    0x7000000f,
    0x080cf19f,
    0xd81882a4,
    0x8019001b,
    0x60000003,
    0x2c97c7df,
    0xd818826d,
    0x8019001b,
    0x60000003,
    0x2c17c7df,
    0x000001f8, /* 005C(:0):    NOP */
    0xc282e01b,
    0xc002a25e,
    0x811a601b,
    0xc184a102,
    0x841be009,
    0x63fe0000,
    0x0d4cfddf,
    0xda9b001b,
    0x9b9be01b,
    0x70000002,
    0x004cf99f,
    0x70000006,
    0x088cf88b,
    0x1000cb28,
    0x70000006,
    0x088cf88b,
    0x1000cb30,
    0x70000006,
    0x088cf88b,
    0x0000cb38,
    0x000001f8, /* 00B0(:0):    NOP */
    0xc282e01b,
    0xc002a31e,
    0x811a601b,
    0xda9b001b,
    0x9b9be01b,
    0x0000d3a0,
    0xc184a102,
    0x841be009,
    0x6bfe0000,
    0x0d4cfddf,
    0xda9b001b,
    0x9b9be01b,
    0x70000002,
    0x004cf99f,
    0x70000006,
    0x088cf88b,
    0x1000cb28,
    0x70000006,
    0x088cf88b,
    0x1000cb30,
    0x70000006,
    0x088cf88b,
    0x0000cb38,
    0x000001f8, /* 0110(:0):    NOP */
    0x8144801c,
    0x0000c008,
    0xc398027f,
    0x8018801b,
    0x040001f8,
};
#endif
u32 MCD_ENetRcv_TDT[] =
{
    0x80004000,
    0x81988000,
    0x10000788,
    0x6000000a,
    0x080cf05f,
    0x98180209,
    0x81c40004,
    0x7000000e,
    0x010cf05f,
    0x7000000c,
    0x01ccf05f,
    0x70000004,
    0x014cf049,
    0x70000004,
    0x004cf04a,
    0x00000b88,
    0xc4030150,
    0x8119e012,
    0x03e0cf90,
    0x81188000,
    0x000ac788,
    0xc4030000,
    0x8199e000,
    0x63e00004,
    0x084cfc8b,
    0xd8990000,
    0x9999e000,
    0x60000005,
    0x0cccf841,
    0x81c60000,
    0xc399021b,
    0x80198000,
    0x00008400,
    0x00000f08,
    0x81988000,
    0x10000788,
    0x6000000a,
    0x080cf05f,
    0xc2188209,
    0x80190000,
    0x040001f8,
    0x000001f8,
};
u32 MCD_ENetXmit_TDT[] =
{
    0x80004000,
    0x81988000,
    0x10000788,
    0x6000000a,
    0x080cf05f,
    0x98180309,
    0x80004003,
    0x81c60004,
    0x7000000e,
    0x014cf05f,
    0x7000000c,
    0x028cf05f,
    0x7000000d,
    0x018cf05f,
    0x70000004,
    0x01ccf04d,
    0x10000b90,
    0x60000004,
    0x020cf0a1,
    0xc3188312,
    0x83c70000,
    0x00001f10,
    0xc583a3c3,
    0x81042325,
    0x03e0c798,
    0xd8990000,
    0x9999e000,
    0x000acf98,
    0xd8992306,
    0x9999e03f,
    0x03eac798,
    0xd8990000,
    0x9999e000,
    0x000acf98,
    0xd8990000,
    0x99832302,
    0x0beac798,
    0x81988000,
    0x6000000b,
    0x0c4cfc5f,
    0x81c80000,
    0xc5190312,
    0x80198000,
    0x00008400,
    0x00000f08,
    0x81988000,
    0x10000788,
    0x6000000a,
    0x080cf05f,
    0xc2988309,
    0x80190000,
    0x040001f8,
    0x000001f8,
};

#ifdef MCD_INCLUDE_EU
MCD_bufDesc MCD_singleBufDescs[NCHANNELS];
#endif
