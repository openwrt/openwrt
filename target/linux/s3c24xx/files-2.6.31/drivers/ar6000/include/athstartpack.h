/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * @file: athstartpack.h
 *
 * @abstract: start compiler-specific structure packing
 *
 * Copyright (c) 2004-2007 Atheros Communications Inc.
 * All rights reserved.
 *
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation;
 *
 *  Software distributed under the License is distributed on an "AS
 *  IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
 *  implied. See the License for the specific language governing
 *  rights and limitations under the License.
 *
 *
 *
 */

#ifdef VXWORKS
#endif /* VXWORKS */

#ifdef LINUX
#endif /* LINUX */

#ifdef QNX
#endif /* QNX */

#ifdef INTEGRITY
#include "integrity/athstartpack_integrity.h"
#endif /* INTEGRITY */

#ifdef NUCLEUS
#endif /* NUCLEUS */

#ifdef UNDER_CE
#include "../os/wince/include/athstartpack_wince.h"
#endif /* WINCE */

