/*!***************************************************************************
*!
*! FILE NAME  : serial.h
*!
*! DESCRIPTION: Serial boot stuff for e100boot.
*!
*! ---------------------------------------------------------------------------
*! HISTORY
*!
*! DATE         NAME               CHANGES
*! ----         ----               -------
*! 1996         Ronny Raneup       Initial version
*! 2002 05 02   Ronny Ranerup      Moved it into this file
*! ---------------------------------------------------------------------------
*! (C) Copyright 1999, 2000, 2001, 2002 Axis Communications AB, LUND, SWEDEN
*!***************************************************************************/
/**************************  Include files  ********************************/
/**************************  Type definitions  *****************************/
/**************************  Global variables  *****************************/

extern int set_baudrate;	/* Baudrate for serial boot (after bootloader). */

/**************************  Constants and macros  *************************/
/**************************  Function prototypes  **************************/
int                     SetupSerial             (void);
void 			SerBoot			(void);
