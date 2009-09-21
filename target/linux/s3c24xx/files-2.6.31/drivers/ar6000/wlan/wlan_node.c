/*-
 * Copyright (c) 2001 Atsushi Onoe
 * Copyright (c) 2002-2004 Sam Leffler, Errno Consulting
 * Copyright (c) 2004-2005 Atheros Communications
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * Alternatively, this software may be distributed under the terms of the
 * GNU General Public License ("GPL") version 2 as published by the Free
 * Software Foundation.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * $Id: //depot/sw/releases/olca2.0-GPL/host/wlan/src/wlan_node.c#1 $
 */
/*
 * IEEE 802.11 node handling support.
 */
#include <a_config.h>
#include <athdefs.h>
#include <a_types.h>
#include <a_osapi.h>
#include <a_debug.h>
#include <ieee80211.h>
#include <wlan_api.h>
#include <ieee80211_node.h>
#include <htc_api.h>
#include <wmi.h>
#include <wmi_api.h>

static void wlan_node_timeout(A_ATH_TIMER arg);
static bss_t * _ieee80211_find_node(struct ieee80211_node_table *nt,
                                    const A_UINT8 *macaddr);

bss_t *
wlan_node_alloc(struct ieee80211_node_table *nt, int wh_size)
{
    bss_t *ni;

    ni = A_MALLOC_NOWAIT(sizeof(bss_t));

    if (ni != NULL) {
        ni->ni_buf = A_MALLOC_NOWAIT(wh_size);
        if (ni->ni_buf == NULL) {
            A_FREE(ni);
            ni = NULL;
            return ni;
        }
    } else {
        return ni;
    }

    /* Make sure our lists are clean */
    ni->ni_list_next = NULL;
    ni->ni_list_prev = NULL;
    ni->ni_hash_next = NULL;
    ni->ni_hash_prev = NULL;

    //
    // ni_scangen never initialized before and during suspend/resume of winmobile, customer (LG/SEMCO) identified
    // that some junk has been stored in this, due to this scan list didn't properly updated
    //
    ni->ni_scangen	 = 0;

    return ni;
}

void
wlan_node_free(bss_t *ni)
{
    if (ni->ni_buf != NULL) {
        A_FREE(ni->ni_buf);
    }
    A_FREE(ni);
}

void
wlan_setup_node(struct ieee80211_node_table *nt, bss_t *ni,
                const A_UINT8 *macaddr)
{
    int hash;

    A_MEMCPY(ni->ni_macaddr, macaddr, IEEE80211_ADDR_LEN);
    hash = IEEE80211_NODE_HASH(macaddr);
    ieee80211_node_initref(ni);     /* mark referenced */

    ni->ni_tstamp = A_GET_MS(WLAN_NODE_INACT_TIMEOUT_MSEC);
    IEEE80211_NODE_LOCK_BH(nt);

    /* Insert at the end of the node list */
    ni->ni_list_next = NULL;
    ni->ni_list_prev = nt->nt_node_last;
    if(nt->nt_node_last != NULL)
    {
        nt->nt_node_last->ni_list_next = ni;
    }
    nt->nt_node_last = ni;
    if(nt->nt_node_first == NULL)
    {
        nt->nt_node_first = ni;
    }

    /* Insert into the hash list i.e. the bucket */
    if((ni->ni_hash_next = nt->nt_hash[hash]) != NULL)
    {
        nt->nt_hash[hash]->ni_hash_prev = ni;
    }
    ni->ni_hash_prev = NULL;
    nt->nt_hash[hash] = ni;

    if (!nt->isTimerArmed) {
        A_TIMEOUT_MS(&nt->nt_inact_timer, WLAN_NODE_INACT_TIMEOUT_MSEC, 0);
        nt->isTimerArmed = TRUE;
    }

    IEEE80211_NODE_UNLOCK_BH(nt);
}

static bss_t *
_ieee80211_find_node(struct ieee80211_node_table *nt,
    const A_UINT8 *macaddr)
{
    bss_t *ni;
    int hash;

    IEEE80211_NODE_LOCK_ASSERT(nt);

    hash = IEEE80211_NODE_HASH(macaddr);
    for(ni = nt->nt_hash[hash]; ni; ni = ni->ni_hash_next) {
        if (IEEE80211_ADDR_EQ(ni->ni_macaddr, macaddr)) {
            ieee80211_node_incref(ni);  /* mark referenced */
            return ni;
        }
    }
    return NULL;
}

bss_t *
wlan_find_node(struct ieee80211_node_table *nt, const A_UINT8 *macaddr)
{
    bss_t *ni;

    IEEE80211_NODE_LOCK(nt);
    ni = _ieee80211_find_node(nt, macaddr);
    IEEE80211_NODE_UNLOCK(nt);
    return ni;
}

/*
 * Reclaim a node.  If this is the last reference count then
 * do the normal free work.  Otherwise remove it from the node
 * table and mark it gone by clearing the back-reference.
 */
void
wlan_node_reclaim(struct ieee80211_node_table *nt, bss_t *ni)
{
    IEEE80211_NODE_LOCK(nt);

    if(ni->ni_list_prev == NULL)
    {
        /* First in list so fix the list head */
        nt->nt_node_first = ni->ni_list_next;
    }
    else
    {
        ni->ni_list_prev->ni_list_next = ni->ni_list_next;
    }

    if(ni->ni_list_next == NULL)
    {
        /* Last in list so fix list tail */
        nt->nt_node_last = ni->ni_list_prev;
    }
    else
    {
        ni->ni_list_next->ni_list_prev = ni->ni_list_prev;
    }

    if(ni->ni_hash_prev == NULL)
    {
        /* First in list so fix the list head */
        int hash;
        hash = IEEE80211_NODE_HASH(ni->ni_macaddr);
        nt->nt_hash[hash] = ni->ni_hash_next;
    }
    else
    {
        ni->ni_hash_prev->ni_hash_next = ni->ni_hash_next;
    }

    if(ni->ni_hash_next != NULL)
    {
        ni->ni_hash_next->ni_hash_prev = ni->ni_hash_prev;
    }
    wlan_node_free(ni);

    IEEE80211_NODE_UNLOCK(nt);
}

static void
wlan_node_dec_free(bss_t *ni)
{
    if (ieee80211_node_dectestref(ni)) {
        wlan_node_free(ni);
    }
}

void
wlan_free_allnodes(struct ieee80211_node_table *nt)
{
    bss_t *ni;

    while ((ni = nt->nt_node_first) != NULL) {
        wlan_node_reclaim(nt, ni);
    }
}

void
wlan_iterate_nodes(struct ieee80211_node_table *nt, wlan_node_iter_func *f,
                   void *arg)
{
    bss_t *ni;
    A_UINT32 gen;

    gen = ++nt->nt_scangen;

    IEEE80211_NODE_LOCK(nt);
    for (ni = nt->nt_node_first; ni; ni = ni->ni_list_next) {
        if (ni->ni_scangen != gen) {
            ni->ni_scangen = gen;
            (void) ieee80211_node_incref(ni);
            (*f)(arg, ni);
            wlan_node_dec_free(ni);
        }
    }
    IEEE80211_NODE_UNLOCK(nt);
}

/*
 * Node table support.
 */
void
wlan_node_table_init(void *wmip, struct ieee80211_node_table *nt)
{
    int i;

    AR_DEBUG_PRINTF(ATH_DEBUG_WLAN, ("node table = 0x%x\n", (A_UINT32)nt));
    IEEE80211_NODE_LOCK_INIT(nt);

    nt->nt_node_first = nt->nt_node_last = NULL;
    for(i = 0; i < IEEE80211_NODE_HASHSIZE; i++)
    {
        nt->nt_hash[i] = NULL;
    }
    A_INIT_TIMER(&nt->nt_inact_timer, wlan_node_timeout, nt);
    nt->isTimerArmed = FALSE;
    nt->nt_wmip = wmip;
}

static void
wlan_node_timeout(A_ATH_TIMER arg)
{
    struct ieee80211_node_table *nt = (struct ieee80211_node_table *)arg;
    bss_t *bss, *nextBss;
    A_UINT8 myBssid[IEEE80211_ADDR_LEN], reArmTimer = FALSE;

    wmi_get_current_bssid(nt->nt_wmip, myBssid);

    bss = nt->nt_node_first;
    while (bss != NULL)
    {
        nextBss = bss->ni_list_next;
        if (A_MEMCMP(myBssid, bss->ni_macaddr, sizeof(myBssid)) != 0)
        {

            if (bss->ni_tstamp <= A_GET_MS(0))
            {
               /*
                * free up all but the current bss - if set
                */
                wlan_node_reclaim(nt, bss);
            }
            else
            {
                /*
                 * Re-arm timer, only when we have a bss other than
                 * current bss AND it is not aged-out.
                 */
                reArmTimer = TRUE;
            }
        }
        bss = nextBss;
    }

    if(reArmTimer)
        A_TIMEOUT_MS(&nt->nt_inact_timer, WLAN_NODE_INACT_TIMEOUT_MSEC, 0);

    nt->isTimerArmed = reArmTimer;
}

void
wlan_node_table_cleanup(struct ieee80211_node_table *nt)
{
    A_UNTIMEOUT(&nt->nt_inact_timer);
    A_DELETE_TIMER(&nt->nt_inact_timer);
    wlan_free_allnodes(nt);
    IEEE80211_NODE_LOCK_DESTROY(nt);
}

bss_t *
wlan_find_Ssidnode (struct ieee80211_node_table *nt, A_UCHAR *pSsid,
					A_UINT32 ssidLength, A_BOOL bIsWPA2)
{
    bss_t   *ni = NULL;
	A_UCHAR *pIESsid = NULL;

    IEEE80211_NODE_LOCK (nt);

    for (ni = nt->nt_node_first; ni; ni = ni->ni_list_next) {
		pIESsid = ni->ni_cie.ie_ssid;
		if (pIESsid[1] <= 32) {

			// Step 1 : Check SSID
			if (0x00 == memcmp (pSsid, &pIESsid[2], ssidLength)) {

				// Step 2 : if SSID matches, check WPA or WPA2
				if (TRUE == bIsWPA2 && NULL != ni->ni_cie.ie_rsn) {
					ieee80211_node_incref (ni);  /* mark referenced */
					IEEE80211_NODE_UNLOCK (nt);
					return ni;
				}
				if (FALSE == bIsWPA2 && NULL != ni->ni_cie.ie_wpa) {
					ieee80211_node_incref(ni);  /* mark referenced */
					IEEE80211_NODE_UNLOCK (nt);
					return ni;
				}
			}
		}
    }

    IEEE80211_NODE_UNLOCK (nt);

    return NULL;
}

void
wlan_node_return (struct ieee80211_node_table *nt, bss_t *ni)
{
	IEEE80211_NODE_LOCK (nt);
	wlan_node_dec_free (ni);
	IEEE80211_NODE_UNLOCK (nt);
}
