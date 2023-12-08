/******************************************************************************
 *
 * Copyright(c) 2019 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 *****************************************************************************/
#ifndef _PLTFM_OPS_H_
#define _PLTFM_OPS_H_

#ifdef PHL_PLATFORM_WINDOWS
#include "pltfm_ops_windows.h"
#elif defined(PHL_PLATFORM_LINUX)
#include "pltfm_ops_linux.h"
#elif defined(PHL_PLATFORM_MACOS)
#include "pltfm_ops_macos.h"
#else
#include "pltfm_ops_none.h"
#endif

#endif /*_PLTFM_OPS_H_*/
