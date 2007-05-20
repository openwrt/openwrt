/**************************************************************************
 *
 *  BRIEF MODULE DESCRIPTION
 *   Definitions for DMA controller.
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
 * May 2004 rkt, neb.
 *
 * Initial Release
 *
 * 
 *
 **************************************************************************
 */

#ifndef __IDT_DMA_V_H__
#define __IDT_DMA_V_H__

#include  <asm/idt-boards/rc32434/rc32434_dma.h> 
#include  <asm/idt-boards/rc32434/rc32434.h>

#define DMA_CHAN_OFFSET  0x14
#define IS_DMA_USED(X) (((X) & (DMAD_f_m | DMAD_d_m | DMAD_t_m)) != 0)
#define DMA_COUNT(count)   \
  ((count) & DMAD_count_m)

#define DMA_HALT_TIMEOUT 500


static inline int rc32434_halt_dma(DMA_Chan_t ch)
{
	int timeout=1;
	if (rc32434_readl(&ch->dmac) & DMAC_run_m) {
		rc32434_writel(0, &ch->dmac); 
		
		for (timeout = DMA_HALT_TIMEOUT; timeout > 0; timeout--) {
			if (rc32434_readl(&ch->dmas) & DMAS_h_m) {
				rc32434_writel(0, &ch->dmas);  
				break;
			}
		}

	}
	
	return timeout ? 0 : 1;
}

static inline void rc32434_start_dma(DMA_Chan_t ch, u32 dma_addr)
{
	rc32434_writel(0, &ch->dmandptr); 
	rc32434_writel(dma_addr, &ch->dmadptr);
}

static inline void rc32434_chain_dma(DMA_Chan_t ch, u32 dma_addr)
{
	rc32434_writel(dma_addr, &ch->dmandptr);
}

#endif	// __IDT_DMA_V_H__







