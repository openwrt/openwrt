/************************************************************************
 *
 * Copyright (c) 2007
 * Infineon Technologies AG
 * St. Martin Strasse 53; 81669 Muenchen; Germany
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 *
 ************************************************************************/

#ifndef __SVIP_PMS_H
#define __SVIP_PMS_H

void svip_sys1_clk_enable(u32 mask);
int svip_sys1_clk_is_enabled(u32 mask);

void svip_sys2_clk_enable(u32 mask);
int svip_sys2_clk_is_enabled(u32 mask);

#endif
