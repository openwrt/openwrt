/*
 * Copyright (c) 2004-2005 Atheros Communications Inc.
 * All rights reserved.
 *
 * This module implements frequently used wlan utilies
 *
 * $Id: //depot/sw/releases/olca2.0-GPL/host/wlan/src/wlan_utils.c#1 $
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

#include <a_config.h>
#include <athdefs.h>
#include <a_types.h>
#include <a_osapi.h>

/*
 * converts ieee channel number to frequency
 */
A_UINT16
wlan_ieee2freq(int chan)
{
    if (chan == 14) {
        return 2484;
    }
    if (chan < 14) {    /* 0-13 */
        return (2407 + (chan*5));
    }
    if (chan < 27) {    /* 15-26 */
        return (2512 + ((chan-15)*20));
    }
    return (5000 + (chan*5));
}

/*
 * Converts MHz frequency to IEEE channel number.
 */
A_UINT32
wlan_freq2ieee(A_UINT16 freq)
{
    if (freq == 2484)
        return 14;
    if (freq < 2484)
        return (freq - 2407) / 5;
    if (freq < 5000)
        return 15 + ((freq - 2512) / 20);
    return (freq - 5000) / 5;
}
