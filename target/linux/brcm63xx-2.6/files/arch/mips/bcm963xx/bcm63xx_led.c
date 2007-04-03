/*
<:copyright-gpl 
 Copyright 2002 Broadcom Corp. All Rights Reserved. 
 
 This program is free software; you can distribute it and/or modify it 
 under the terms of the GNU General Public License (Version 2) as 
 published by the Free Software Foundation. 
 
 This program is distributed in the hope it will be useful, but WITHOUT 
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
 FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License 
 for more details. 
 
 You should have received a copy of the GNU General Public License along 
 with this program; if not, write to the Free Software Foundation, Inc., 
 59 Temple Place - Suite 330, Boston MA 02111-1307, USA. 
:>
*/
/***************************************************************************
 * File Name  : bcm63xx_led.c
 *
 * Description: 
 *
 *    This file contains bcm963xx board led control API functions. 
 *
 *    To use it, do the following
 *
 *    1). define in the board.c the following led mappping (this is for 6345GW board):
 *        const LED_MAP_PAIR cLedMapping45GW[] =
 *        {   // led name     Initial state       physical pin (ledMask)
 *            {kLedUsb,       kLedStateOff,       GPIO_LED_PIN_7}, 
 *            {kLedAdsl,      kLedStateOff,       GPIO_LED_PIN_8},
 *            {kLedPPP,       kLedStateOff,       GPIO_LED_PIN_9},    // PPP and WanData share PIN_9
 *            {kLedWanData,   kLedStateOff,       GPIO_LED_PIN_9},
 *            {kLedWireless,  kLedStateOff,       GPIO_LED_PIN_10},
 *            {kLedEnd,       kLedStateOff,       0              } // NOTE: kLedEnd has to be at the end.
 *
 *    2). };To initialize led API and initial state of the leds, call the following function with the mapping 
 *        pointer from the above struct
 *
 *        boardLedInit((PLED_MAP_PAIR) &cLedMapping45R);
 *
 *    3). Sample call for kernel mode:
 *
 *        kerSysLedCtrl(kLedAdsl, kLedStateBlinkOnce);        // kLedxxx defines in board.h
 *
 *    4). Sample call for user mode
 *
 *        sysLedCtrl(kLedAdsl, kLedStateBlinkOnce);           // kLedxxx defines in board_api.h
 *
 *
 * Created on :  10/28/2002  seanl
 *
 ***************************************************************************/

/* Includes. */
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/capability.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/module.h>
#include <linux/netdevice.h>
#include <asm/uaccess.h>

#include <bcm_map_part.h>
#include <board.h>

#define k100ms              (HZ / 10)     // ~100 ms
#define kFastBlinkCount     0             // ~100ms
#define kSlowBlinkCount     5             // ~600ms

#define MAX_VIRT_LEDS       12

// uncomment // for debug led
//#define DEBUG_LED

// global variables:
struct timer_list gLedTimer;
int gTimerOn = FALSE;
int gLedCount = 0;

typedef struct ledinfo
{
    unsigned short ledMask;         // mask for led: ie. giop 10 = 0x0400
    unsigned short ledActiveLow;    // GPIO bit reset to turn on LED
    unsigned short ledMaskFail;     // mask for led: ie. giop 10 = 0x0400
    unsigned short ledActiveLowFail;// GPIO bit reset to turn on LED
    BOARD_LED_STATE ledState;       // current led state
    BOARD_LED_STATE savedLedState;  // used in blink once for restore to the orignal ledState
    int blinkCountDown;             // if == 0, do blink (toggle).  Is assgined value and dec by 1 at each timer.
} LED_INFO, *PLED_INFO;

static PLED_INFO gLed = NULL;
static PLED_INFO gpVirtLeds[MAX_VIRT_LEDS];
static HANDLE_LED_FUNC gLedHwFunc[MAX_VIRT_LEDS];
static HANDLE_LED_FUNC gLedHwFailFunc[MAX_VIRT_LEDS];

#if 0 /* BROKEN */
#if defined(CONFIG_BCM96348) || defined(CONFIG_BCM96338)
static int gLedOffInBridgeMode = 1;
#elif defined(CONFIG_BCM96345)
static int gLedOffInBridgeMode = 0;
#endif
#endif

void ledTimerExpire(void);
int initLedInfo( PLED_MAP_PAIR pCurMap, PLED_INFO pCurLed );

//**************************************************************************************
// LED operations
//**************************************************************************************

// turn led on and set the ledState
void ledOn(PLED_INFO pLed)
{
    if( pLed->ledMask )
    {
        GPIO->GPIODir |= pLed->ledMask;         // turn on the direction bit in case was turned off by some one
        if( pLed->ledActiveLow )
            GPIO->GPIOio  &= ~pLed->ledMask;    // turn on the led
        else
            GPIO->GPIOio  |= pLed->ledMask;     // turn on the led
        pLed->ledState = pLed->savedLedState = kLedStateOn;
    }
}


// turn led off and set the ledState
void ledOff(PLED_INFO pLed)
{
    if( pLed->ledMask )
    {
        GPIO->GPIODir |= pLed->ledMask;         // turn on the direction bit in case was turned off by some one
        if( pLed->ledActiveLow )
            GPIO->GPIOio  |= pLed->ledMask;     // turn off the led
        else
            GPIO->GPIOio  &= ~pLed->ledMask;    // turn off the led
        pLed->ledState = pLed->savedLedState = kLedStateOff;
    }
}

// turn led on and set the ledState
void ledOnFail(PLED_INFO pLed)
{
    if( pLed->ledMaskFail )
    {
        GPIO->GPIODir |= pLed->ledMaskFail;     // turn on the direction bit in case was turned off by some one
        if( pLed->ledActiveLowFail )
            GPIO->GPIOio  &= ~pLed->ledMaskFail;// turn on the led
        else
            GPIO->GPIOio  |= pLed->ledMaskFail; // turn on the led
        pLed->ledState = pLed->savedLedState = kLedStateFail;
    }
}


// turn led off and set the ledState
void ledOffFail(PLED_INFO pLed)
{
    if( pLed->ledMaskFail )
    {
        GPIO->GPIODir |= pLed->ledMaskFail;     // turn on the direction bit in case was turned off by some one
        if( pLed->ledActiveLowFail )
            GPIO->GPIOio  |= pLed->ledMaskFail; // turn off the led
        else
            GPIO->GPIOio  &= ~pLed->ledMaskFail;// turn off the led
        pLed->ledState = pLed->savedLedState = kLedStateOff;
    }
}


// toggle the led and return the current ledState
BOARD_LED_STATE ledToggle(PLED_INFO pLed)
{
    GPIO->GPIODir |= pLed->ledMask;         // turn on the direction bit in case was turned off by some one
    if (GPIO->GPIOio & pLed->ledMask)
    {
        GPIO->GPIOio &= ~(pLed->ledMask);
        return( (pLed->ledActiveLow) ? kLedStateOn : kLedStateOff );
    }
    else
    {
        GPIO->GPIOio |= pLed->ledMask;
        return( (pLed->ledActiveLow) ? kLedStateOff : kLedStateOn );
    }
}   


// led timer.  Will return if timer is already on
void ledTimerStart(void)
{
    if (gTimerOn)
        return;

#if defined(DEBUG_LED)
    printk("led: add_timer\n");
#endif

    init_timer(&gLedTimer);
    gLedTimer.function = (void*)ledTimerExpire;
    gLedTimer.expires = jiffies + k100ms;        // timer expires in ~100ms
    add_timer (&gLedTimer);
    gTimerOn = TRUE;
} 


// led timer expire kicks in about ~100ms and perform the led operation according to the ledState and
// restart the timer according to ledState
void ledTimerExpire(void)
{
    int i;
    PLED_INFO pCurLed;

    gTimerOn = FALSE;

    for (i = 0, pCurLed = gLed; i < gLedCount; i++, pCurLed++)
    {
#if defined(DEBUG_LED)
        printk("led[%d]: Mask=0x%04x, State = %d, blcd=%d\n", i, pCurLed->ledMask, pCurLed->ledState, pCurLed->blinkCountDown);
#endif
        switch (pCurLed->ledState)
        {
            case kLedStateOn:
            case kLedStateOff:
            case kLedStateFail:
                pCurLed->blinkCountDown = 0;            // reset the blink count down
                break;

            case kLedStateBlinkOnce:
                ledToggle(pCurLed);
                pCurLed->blinkCountDown = 0;                      // reset to 0
                pCurLed->ledState = pCurLed->savedLedState;
                if (pCurLed->ledState == kLedStateSlowBlinkContinues || 
                    pCurLed->ledState == kLedStateFastBlinkContinues)
                    ledTimerStart();                  // start timer if in blinkContinues stats
                break;

            case kLedStateSlowBlinkContinues:
                if (pCurLed->blinkCountDown-- == 0)
                {
                    pCurLed->blinkCountDown = kSlowBlinkCount;
                    ledToggle(pCurLed);
                }
                ledTimerStart();
                break;

            case kLedStateFastBlinkContinues:
                if (pCurLed->blinkCountDown-- == 0)
                {
                    pCurLed->blinkCountDown = kFastBlinkCount;
                    ledToggle(pCurLed);
                }
                ledTimerStart();
                break;

            default:
                printk("Invalid state = %d\n", pCurLed->ledState);
        }
    }
}

// initialize the gLedCount and allocate and fill gLed struct
void __init boardLedInit(PLED_MAP_PAIR cLedMapping)
{
    PLED_MAP_PAIR p1, p2;
    PLED_INFO pCurLed;
    int needTimer = FALSE;
    int alreadyUsed = 0;

#if defined(CONFIG_BCM96348) || defined(CONFIG_BCM96338)
    /* Set blink rate for BCM6348/BCM6338 hardware LEDs. */
    GPIO->LEDCtrl &= ~LED_INTERVAL_SET_MASK;
    GPIO->LEDCtrl |= LED_INTERVAL_SET_80MS;
#endif

    memset( gpVirtLeds, 0x00, sizeof(gpVirtLeds) );
    memset( gLedHwFunc, 0x00, sizeof(gLedHwFunc) );
    memset( gLedHwFailFunc, 0x00, sizeof(gLedHwFailFunc) );

    gLedCount = 0;

    // Check for multiple LED names and multiple LED GPIO pins that share the
    // same physical board LED.
    for( p1 = cLedMapping; p1->ledName != kLedEnd; p1++ )
    {
        alreadyUsed = 0;
        for( p2 = cLedMapping; p2 != p1; p2++ )
        {
            if( (p1->ledMask && p1->ledMask == p2->ledMask) ||
                (p1->ledMaskFail && p1->ledMaskFail == p2->ledMaskFail) )
            {
                alreadyUsed = 1;
                break;
            }
        }

        if( alreadyUsed == 0  )
            gLedCount++;
    }

    gLed = (PLED_INFO) kmalloc((gLedCount * sizeof(LED_INFO)), GFP_KERNEL);
    if( gLed == NULL )
    {
        printk( "LED memory allocation error.\n" );
        return;
    }

    memset( gLed, 0x00, gLedCount * sizeof(LED_INFO) );

    // initial the gLed with unique ledMask and initial state. If more than 1 ledNames share the physical led 
    // (ledMask) the first defined led's ledInitState will be used.
    pCurLed = gLed;
    for( p1 = cLedMapping; p1->ledName != kLedEnd; p1++ )
    {
        if( (int) p1->ledName > MAX_VIRT_LEDS )
            continue;

        alreadyUsed = 0;
        for( p2 = cLedMapping; p2 != p1; p2++ )
        {
            if( (p1->ledMask && p1->ledMask == p2->ledMask) ||
                (p1->ledMaskFail && p1->ledMaskFail == p2->ledMaskFail) )
            {
                alreadyUsed = 1;
                break;
            }
        }

        if( alreadyUsed == 0 )
        {
            // Initialize the board LED for the first time.
            needTimer = initLedInfo( p1, pCurLed );
            gpVirtLeds[(int) p1->ledName] = pCurLed;
            pCurLed++;
        }
        else
        {
            PLED_INFO pLed;
            for( pLed = gLed; pLed != pCurLed; pLed++ )
            {
                // Find the LED_INFO structure that has already been initialized.
                if((pLed->ledMask && pLed->ledMask == p1->ledMask) ||
                   (pLed->ledMaskFail && pLed->ledMaskFail==p1->ledMaskFail))
                {
                    // The board LED has already been initialized but possibly
                    // not completely initialized.
                    if( p1->ledMask )
                    {
                        pLed->ledMask = p1->ledMask;
                        pLed->ledActiveLow = p1->ledActiveLow;
                    }
                    if( p1->ledMaskFail )
                    {
                        pLed->ledMaskFail = p1->ledMaskFail;
                        pLed->ledActiveLowFail = p1->ledActiveLowFail;
                    }
                    gpVirtLeds[(int) p1->ledName] = pLed;
                    break;
                }
            }
        }
    }

    if (needTimer)
        ledTimerStart();

#if defined(DEBUG_LED)
    int i;
    for (i=0; i < gLedCount; i++)
        printk("initLed: led[%d]: mask=0x%04x, state=%d\n", i,(gLed+i)->ledMask, (gLed+i)->ledState);
#endif

}

// Initialize a structure that contains information about a physical board LED
// control.  The board LED may contain more than one GPIO pin to control a
// normal condition (green) or a failure condition (red).
int initLedInfo( PLED_MAP_PAIR pCurMap, PLED_INFO pCurLed )
{
    int needTimer = FALSE;
    pCurLed->ledState = pCurLed->savedLedState = pCurMap->ledInitState;
    pCurLed->ledMask = pCurMap->ledMask;
    pCurLed->ledActiveLow = pCurMap->ledActiveLow;
    pCurLed->ledMaskFail = pCurMap->ledMaskFail;
    pCurLed->ledActiveLowFail = pCurMap->ledActiveLowFail;

    switch (pCurLed->ledState)
    {
        case kLedStateOn:
            pCurLed->blinkCountDown = 0;            // reset the blink count down
            ledOn(pCurLed);
            break;
        case kLedStateOff:
            pCurLed->blinkCountDown = 0;            // reset the blink count down
            ledOff(pCurLed);
            break;
        case kLedStateFail:
            pCurLed->blinkCountDown = 0;            // reset the blink count down
            ledOnFail(pCurLed);
            break;
        case kLedStateBlinkOnce:
            pCurLed->blinkCountDown = 1;
            needTimer = TRUE;
            break;
        case kLedStateSlowBlinkContinues:
            pCurLed->blinkCountDown = kSlowBlinkCount;
            needTimer = TRUE;
            break;
        case kLedStateFastBlinkContinues:
            pCurLed->blinkCountDown = kFastBlinkCount;
            needTimer = TRUE;
            break;
        default:
            printk("Invalid state = %d\n", pCurLed->ledState);
    }

    return( needTimer );
}

#if 0 /* BROKEN */
// Determines if there is at least one interface in bridge mode.  Bridge mode
// is determined by the cfm convention of naming bridge interfaces nas17
// through nas24.
static int isBridgedProtocol(void)
{
    extern int dev_get(const char *name);
    const int firstBridgeId = 17;
    const int lastBridgeId = 24;
    int i;
    int ret = FALSE;
    char name[16];

    for( i = firstBridgeId; i <= lastBridgeId; i++ )
    {
        sprintf( name, "nas%d", i );

        if( dev_get(name) )
        {
            ret = TRUE;
            break;
        }
    }

    return(ret);
}
#endif

// led ctrl.  Maps the ledName to the corresponding ledInfoPtr and perform the led operation
void boardLedCtrl(BOARD_LED_NAME ledName, BOARD_LED_STATE ledState)
{
    PLED_INFO ledInfoPtr;

    // do the mapping from virtual to physical led
    if( (int) ledName < MAX_VIRT_LEDS )
        ledInfoPtr = gpVirtLeds[(int) ledName];
    else
        ledInfoPtr = NULL;

    if (ledInfoPtr == NULL)
        return;

    if( ledState != kLedStateFail && gLedHwFunc[(int) ledName] )
    {
        (*gLedHwFunc[(int) ledName]) (ledName, ledState);
        ledOffFail(ledInfoPtr);
        return;
    }
    else
        if( ledState == kLedStateFail && gLedHwFailFunc[(int) ledName] )
        {
            (*gLedHwFailFunc[(int) ledName]) (ledName, ledState);
            ledOff(ledInfoPtr);
            return;
        }

#if 0 /* BROKEN */
    // Do not blink the WAN Data LED if at least one interface is in bridge mode.
    if(gLedOffInBridgeMode == 1 && (ledName == kLedWanData || ledName == kLedPPP))
    {
        static int BridgedProtocol = -1;

        if( BridgedProtocol == -1 )
            BridgedProtocol = isBridgedProtocol();

        if( BridgedProtocol == TRUE )
            return;
    }
#endif

    // If the state is kLedStateFail and there is not a failure LED defined
    // in the board parameters, change the state to kLedStateFastBlinkContinues.
    if( ledState == kLedStateFail && ledInfoPtr->ledMaskFail == 0 )
        ledState = kLedStateFastBlinkContinues;

    switch (ledState)
    {
        case kLedStateOn:
            // First, turn off the complimentary (failure) LED GPIO.
            if( ledInfoPtr->ledMaskFail )
                ledOffFail(ledInfoPtr);
            else
                if( gLedHwFailFunc[(int) ledName] )
                    (*gLedHwFailFunc[(int) ledName]) (ledName, kLedStateOff);

            // Next, turn on the specified LED GPIO.
            ledOn(ledInfoPtr);
            break;

        case kLedStateOff: 
            // First, turn off the complimentary (failure) LED GPIO.
            if( ledInfoPtr->ledMaskFail )
                ledOffFail(ledInfoPtr);
            else
                if( gLedHwFailFunc[(int) ledName] )
                    (*gLedHwFailFunc[(int) ledName]) (ledName, kLedStateOff);

            // Next, turn off the specified LED GPIO.
            ledOff(ledInfoPtr);
            break;

        case kLedStateFail:
            // First, turn off the complimentary (normal) LED GPIO.
            if( ledInfoPtr->ledMask )
                ledOff(ledInfoPtr);
            else
                if( gLedHwFunc[(int) ledName] )
                    (*gLedHwFunc[(int) ledName]) (ledName, kLedStateOff);

            // Next, turn on (red) the specified LED GPIO.
            ledOnFail(ledInfoPtr);
            break;

        case kLedStateBlinkOnce:
            // skip blinkOnce if it is already in Slow/Fast blink continues state
            if (ledInfoPtr->savedLedState == kLedStateSlowBlinkContinues ||
                ledInfoPtr->savedLedState == kLedStateFastBlinkContinues)
                ;
            else
            {
                if (ledInfoPtr->blinkCountDown == 0)  // skip the call if it is 1
                {
                    ledToggle(ledInfoPtr);
                    ledInfoPtr->blinkCountDown = 1;  // it will be reset to 0 when timer expires
                    ledInfoPtr->ledState = kLedStateBlinkOnce;
                    ledTimerStart();
                }
            }
            break;

        case kLedStateSlowBlinkContinues:
            ledInfoPtr->blinkCountDown = kSlowBlinkCount;
            ledInfoPtr->ledState = kLedStateSlowBlinkContinues;
            ledInfoPtr->savedLedState = kLedStateSlowBlinkContinues;
            ledTimerStart();
            break;

        case kLedStateFastBlinkContinues:
            ledInfoPtr->blinkCountDown = kFastBlinkCount;
            ledInfoPtr->ledState = kLedStateFastBlinkContinues;
            ledInfoPtr->savedLedState = kLedStateFastBlinkContinues;
            ledTimerStart();
            break;

        default:
            printk("Invalid led state\n");
    }
}

// This function is called for an LED that is controlled by hardware.
void kerSysLedRegisterHwHandler( BOARD_LED_NAME ledName,
    HANDLE_LED_FUNC ledHwFunc, int ledFailType )
{
    if( (int) ledName < MAX_VIRT_LEDS )
    {
        if( ledFailType == 1 )
            gLedHwFailFunc[(int) ledName] = ledHwFunc;
        else
            gLedHwFunc[(int) ledName] = ledHwFunc;
    }
}

