
/*
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

#include "a_config.h"
#include "athdefs.h"
#include "a_types.h"
#include "a_osapi.h"
#include "a_debug.h"
#include "htc_api.h"
#include "common_drv.h"

/********* CREDIT DISTRIBUTION FUNCTIONS ******************************************/

#define NO_VO_SERVICE 1 /* currently WMI only uses 3 data streams, so we leave VO service inactive */

#ifdef NO_VO_SERVICE
#define DATA_SVCS_USED 3
#else
#define DATA_SVCS_USED 4
#endif

static void RedistributeCredits(COMMON_CREDIT_STATE_INFO *pCredInfo,
                                HTC_ENDPOINT_CREDIT_DIST *pEPDistList);

static void SeekCredits(COMMON_CREDIT_STATE_INFO *pCredInfo,
                        HTC_ENDPOINT_CREDIT_DIST *pEPDistList);

/* reduce an ep's credits back to a set limit */
static INLINE void ReduceCredits(COMMON_CREDIT_STATE_INFO *pCredInfo,
                                HTC_ENDPOINT_CREDIT_DIST  *pEpDist,
                                int                       Limit)
{
    int credits;

        /* set the new limit */
    pEpDist->TxCreditsAssigned = Limit;

    if (pEpDist->TxCredits <= Limit) {
        return;
    }

        /* figure out how much to take away */
    credits = pEpDist->TxCredits - Limit;
        /* take them away */
    pEpDist->TxCredits -= credits;
    pCredInfo->CurrentFreeCredits += credits;
}

/* give an endpoint some credits from the free credit pool */
#define GiveCredits(pCredInfo,pEpDist,credits)      \
{                                                   \
    (pEpDist)->TxCredits += (credits);              \
    (pEpDist)->TxCreditsAssigned += (credits);      \
    (pCredInfo)->CurrentFreeCredits -= (credits);   \
}


/* default credit init callback.
 * This function is called in the context of HTCStart() to setup initial (application-specific)
 * credit distributions */
static void ar6000_credit_init(void                     *Context,
                               HTC_ENDPOINT_CREDIT_DIST *pEPList,
                               int                      TotalCredits)
{
    HTC_ENDPOINT_CREDIT_DIST *pCurEpDist;
    int                      count;
    COMMON_CREDIT_STATE_INFO *pCredInfo = (COMMON_CREDIT_STATE_INFO *)Context;

    pCredInfo->CurrentFreeCredits = TotalCredits;
    pCredInfo->TotalAvailableCredits = TotalCredits;

    pCurEpDist = pEPList;

        /* run through the list and initialize */
    while (pCurEpDist != NULL) {

            /* set minimums for each endpoint */
        pCurEpDist->TxCreditsMin = pCurEpDist->TxCreditsPerMaxMsg;

        if (pCurEpDist->ServiceID == WMI_CONTROL_SVC) {
                /* give control service some credits */
            GiveCredits(pCredInfo,pCurEpDist,pCurEpDist->TxCreditsMin);
                /* control service is always marked active, it never goes inactive EVER */
            SET_EP_ACTIVE(pCurEpDist);
        } else if (pCurEpDist->ServiceID == WMI_DATA_BK_SVC) {
                /* this is the lowest priority data endpoint, save this off for easy access */
            pCredInfo->pLowestPriEpDist = pCurEpDist;
        }

        /* Streams have to be created (explicit | implicit)for all kinds
         * of traffic. BE endpoints are also inactive in the beginning.
         * When BE traffic starts it creates implicit streams that
         * redistributes credits.
         */

        /* note, all other endpoints have minimums set but are initially given NO credits.
         * Credits will be distributed as traffic activity demands */
        pCurEpDist = pCurEpDist->pNext;
    }

    if (pCredInfo->CurrentFreeCredits <= 0) {
        AR_DEBUG_PRINTF(ATH_LOG_INF, ("Not enough credits (%d) to do credit distributions \n", TotalCredits));
        A_ASSERT(FALSE);
        return;
    }

        /* reset list */
    pCurEpDist = pEPList;
        /* now run through the list and set max operating credit limits for everyone */
    while (pCurEpDist != NULL) {
        if (pCurEpDist->ServiceID == WMI_CONTROL_SVC) {
                /* control service max is just 1 max message */
            pCurEpDist->TxCreditsNorm = pCurEpDist->TxCreditsPerMaxMsg;
        } else {
                /* for the remaining data endpoints, we assume that each TxCreditsPerMaxMsg are
                 * the same.
                 * We use a simple calculation here, we take the remaining credits and
                 * determine how many max messages this can cover and then set each endpoint's
                 * normal value equal to half this amount.
                 * */
            count = (pCredInfo->CurrentFreeCredits/pCurEpDist->TxCreditsPerMaxMsg) * pCurEpDist->TxCreditsPerMaxMsg;
            count = count >> 1;
            count = max(count,pCurEpDist->TxCreditsPerMaxMsg);
                /* set normal */
            pCurEpDist->TxCreditsNorm = count;

        }
        pCurEpDist = pCurEpDist->pNext;
    }

}


/* default credit distribution callback
 * This callback is invoked whenever endpoints require credit distributions.
 * A lock is held while this function is invoked, this function shall NOT block.
 * The pEPDistList is a list of distribution structures in prioritized order as
 * defined by the call to the HTCSetCreditDistribution() api.
 *
 */
static void ar6000_credit_distribute(void                     *Context,
                                     HTC_ENDPOINT_CREDIT_DIST *pEPDistList,
                                     HTC_CREDIT_DIST_REASON   Reason)
{
    HTC_ENDPOINT_CREDIT_DIST *pCurEpDist;
    COMMON_CREDIT_STATE_INFO *pCredInfo = (COMMON_CREDIT_STATE_INFO *)Context;

    switch (Reason) {
        case HTC_CREDIT_DIST_SEND_COMPLETE :
            pCurEpDist = pEPDistList;
                /* we are given the start of the endpoint distribution list.
                 * There may be one or more endpoints to service.
                 * Run through the list and distribute credits */
            while (pCurEpDist != NULL) {

                if (pCurEpDist->TxCreditsToDist > 0) {
                        /* return the credits back to the endpoint */
                    pCurEpDist->TxCredits += pCurEpDist->TxCreditsToDist;
                        /* always zero out when we are done */
                    pCurEpDist->TxCreditsToDist = 0;

                    if (pCurEpDist->TxCredits > pCurEpDist->TxCreditsAssigned) {
                            /* reduce to the assigned limit, previous credit reductions
                             * could have caused the limit to change */
                        ReduceCredits(pCredInfo, pCurEpDist, pCurEpDist->TxCreditsAssigned);
                    }

                    if (pCurEpDist->TxCredits > pCurEpDist->TxCreditsNorm) {
                            /* oversubscribed endpoints need to reduce back to normal */
                        ReduceCredits(pCredInfo, pCurEpDist, pCurEpDist->TxCreditsNorm);
                    }
                }

                pCurEpDist = pCurEpDist->pNext;
            }

            A_ASSERT(pCredInfo->CurrentFreeCredits <= pCredInfo->TotalAvailableCredits);

            break;

        case HTC_CREDIT_DIST_ACTIVITY_CHANGE :
            RedistributeCredits(pCredInfo,pEPDistList);
            break;
        case HTC_CREDIT_DIST_SEEK_CREDITS :
            SeekCredits(pCredInfo,pEPDistList);
            break;
        case HTC_DUMP_CREDIT_STATE :
            AR_DEBUG_PRINTF(ATH_LOG_INF, ("Credit Distribution, total : %d, free : %d\n",
            								pCredInfo->TotalAvailableCredits, pCredInfo->CurrentFreeCredits));
            break;
        default:
            break;

    }

}

/* redistribute credits based on activity change */
static void RedistributeCredits(COMMON_CREDIT_STATE_INFO *pCredInfo,
                                HTC_ENDPOINT_CREDIT_DIST *pEPDistList)
{
    HTC_ENDPOINT_CREDIT_DIST *pCurEpDist = pEPDistList;

        /* walk through the list and remove credits from inactive endpoints */
    while (pCurEpDist != NULL) {

        if (pCurEpDist->ServiceID != WMI_CONTROL_SVC) {
            if (!IS_EP_ACTIVE(pCurEpDist)) {
                    /* EP is inactive, reduce credits back to zero */
                ReduceCredits(pCredInfo, pCurEpDist, 0);
            }
        }

        /* NOTE in the active case, we do not need to do anything further,
         * when an EP goes active and needs credits, HTC will call into
         * our distribution function using a reason code of HTC_CREDIT_DIST_SEEK_CREDITS  */

        pCurEpDist = pCurEpDist->pNext;
    }

    A_ASSERT(pCredInfo->CurrentFreeCredits <= pCredInfo->TotalAvailableCredits);

}

/* HTC has an endpoint that needs credits, pEPDist is the endpoint in question */
static void SeekCredits(COMMON_CREDIT_STATE_INFO *pCredInfo,
                        HTC_ENDPOINT_CREDIT_DIST *pEPDist)
{
    HTC_ENDPOINT_CREDIT_DIST *pCurEpDist;
    int                      credits = 0;
    int                      need;

    do {

        if (pEPDist->ServiceID == WMI_CONTROL_SVC) {
                /* we never oversubscribe on the control service, this is not
                 * a high performance path and the target never holds onto control
                 * credits for too long */
            break;
        }

        /* for all other services, we follow a simple algorithm of
         * 1. checking the free pool for credits
         * 2. checking lower priority endpoints for credits to take */

        if (pCredInfo->CurrentFreeCredits >= 2 * pEPDist->TxCreditsSeek) {
                /* try to give more credits than it needs */
            credits = 2 * pEPDist->TxCreditsSeek;
        } else {
                /* give what we can */
            credits = min(pCredInfo->CurrentFreeCredits,pEPDist->TxCreditsSeek);
        }

        if (credits >= pEPDist->TxCreditsSeek) {
                /* we found some to fullfill the seek request */
            break;
        }

        /* we don't have enough in the free pool, try taking away from lower priority services
         *
         * The rule for taking away credits:
         *   1. Only take from lower priority endpoints
         *   2. Only take what is allocated above the minimum (never starve an endpoint completely)
         *   3. Only take what you need.
         *
         * */

            /* starting at the lowest priority */
        pCurEpDist = pCredInfo->pLowestPriEpDist;

            /* work backwards until we hit the endpoint again */
        while (pCurEpDist != pEPDist) {
                /* calculate how many we need so far */
            need = pEPDist->TxCreditsSeek - pCredInfo->CurrentFreeCredits;

            if ((pCurEpDist->TxCreditsAssigned - need) > pCurEpDist->TxCreditsMin) {
                    /* the current one has been allocated more than it's minimum and it
                     * has enough credits assigned above it's minimum to fullfill our need
                     * try to take away just enough to fullfill our need */
                ReduceCredits(pCredInfo,
                              pCurEpDist,
                              pCurEpDist->TxCreditsAssigned - need);

                if (pCredInfo->CurrentFreeCredits >= pEPDist->TxCreditsSeek) {
                        /* we have enough */
                    break;
                }
            }

            pCurEpDist = pCurEpDist->pPrev;
        }

            /* return what we can get */
        credits = min(pCredInfo->CurrentFreeCredits,pEPDist->TxCreditsSeek);

    } while (FALSE);

        /* did we find some credits? */
    if (credits) {
            /* give what we can */
        GiveCredits(pCredInfo, pEPDist, credits);
    }

}

/* initialize and setup credit distribution */
A_STATUS ar6000_setup_credit_dist(HTC_HANDLE HTCHandle, COMMON_CREDIT_STATE_INFO *pCredInfo)
{
    HTC_SERVICE_ID servicepriority[5];

    A_MEMZERO(pCredInfo,sizeof(COMMON_CREDIT_STATE_INFO));

    servicepriority[0] = WMI_CONTROL_SVC;  /* highest */
    servicepriority[1] = WMI_DATA_VO_SVC;
    servicepriority[2] = WMI_DATA_VI_SVC;
    servicepriority[3] = WMI_DATA_BE_SVC;
    servicepriority[4] = WMI_DATA_BK_SVC; /* lowest */

        /* set callbacks and priority list */
    HTCSetCreditDistribution(HTCHandle,
                             pCredInfo,
                             ar6000_credit_distribute,
                             ar6000_credit_init,
                             servicepriority,
                             5);

    return A_OK;
}

