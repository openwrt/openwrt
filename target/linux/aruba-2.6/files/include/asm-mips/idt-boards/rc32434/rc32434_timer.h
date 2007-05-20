/**************************************************************************
 *
 *  BRIEF MODULE DESCRIPTION
 *   Definitions for timer registers
 *
 *  Copyright 2004 IDT Inc. (rischelp@idt.com)
 *         
 *  This program is free software; you can redistribute  it and/or modify it
 *  under  the terms of  the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the  License, or (at your
 *  option) any later version.
 *
 *  THIS  SOFTWARE  IS PROVIDED   ``AS  IS'' AND   ANY  EXPRESS OR IMPLIED
 *  WARRANTIES,   INCLUDING, BUT NOT  LIMITED  TO, THE IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN
 *  NO  EVENT  SHALL   THE AUTHOR  BE    LIABLE FOR ANY   DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 *  NOT LIMITED   TO, PROCUREMENT OF  SUBSTITUTE GOODS  OR SERVICES; LOSS OF
 *  USE, DATA,  OR PROFITS; OR  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 *  ANY THEORY OF LIABILITY, WHETHER IN  CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  You should have received a copy of the  GNU General Public License along
 *  with this program; if not, write  to the Free Software Foundation, Inc.,
 *  675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *
 **************************************************************************
 * May 2004 rkt,neb.
 *
 * Initial Release
 *
 * 
 *
 **************************************************************************
 */

#ifndef __IDT_TIM_H__
#define __IDT_TIM_H__

enum
{
	TIM0_PhysicalAddress	= 0x18028000,
	TIM_PhysicalAddress	= TIM0_PhysicalAddress,		// Default

	TIM0_VirtualAddress	= 0xb8028000,
	TIM_VirtualAddress	= TIM0_VirtualAddress,		// Default
} ;

enum
{
	TIM_Count = 3,
} ;

struct TIM_CNTR_s
{
  u32 count ;
  u32 compare ;
  u32 ctc ;	//use CTC_
} ;

typedef struct TIM_s
{
  struct TIM_CNTR_s	tim [TIM_Count] ;
  u32			rcount ;	//use RCOUNT_
  u32			rcompare ;	//use RCOMPARE_
  u32			rtc ;		//use RTC_
} volatile * TIM_t ;

enum
{
  CTC_en_b	= 0,		
  CTC_en_m	= 0x00000001,
  CTC_to_b	= 1,		 
  CTC_to_m	= 0x00000002,
  
  RCOUNT_count_b		= 0,	     
  RCOUNT_count_m		= 0x0000ffff,
  RCOMPARE_compare_b	= 0,	   
  RCOMPARE_compare_m	= 0x0000ffff,
  RTC_ce_b		= 0,		
  RTC_ce_m		= 0x00000001,
  RTC_to_b		= 1,		
  RTC_to_m		= 0x00000002,
  RTC_rqe_b		= 2,		
  RTC_rqe_m		= 0x00000004,
  
} ;
#endif	// __IDT_TIM_H__

