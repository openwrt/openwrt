/*****************************************************************************
 **   FILE NAME       : ifxhcd_es.c
 **   PROJECT         : IFX USB sub-system V3
 **   MODULES         : IFX USB sub-system Host and Device driver
 **   SRC VERSION     : 1.0
 **   DATE            : 1/Jan/2009
 **   AUTHOR          : Chen, Howard
 **   DESCRIPTION     : The file contain function to enable host mode USB-IF Electrical Test function.
 *****************************************************************************/

/*!
 \file ifxhcd_es.c
 \ingroup IFXUSB_DRIVER_V3
 \brief The file contain function to enable host mode USB-IF Electrical Test function.
*/

#include <linux/version.h>
#include "ifxusb_version.h"

#include <linux/kernel.h>

#include <linux/errno.h>

#include <linux/dma-mapping.h>

#include "ifxusb_plat.h"
#include "ifxusb_regs.h"
#include "ifxusb_cif.h"
#include "ifxhcd.h"


#ifdef __WITH_HS_ELECT_TST__
	/*
	 * Quick and dirty hack to implement the HS Electrical Test
	 * SINGLE_STEP_GET_DEVICE_DESCRIPTOR feature.
	 *
	 * This code was copied from our userspace app "hset". It sends a
	 * Get Device Descriptor control sequence in two parts, first the
	 * Setup packet by itself, followed some time later by the In and
	 * Ack packets. Rather than trying to figure out how to add this
	 * functionality to the normal driver code, we just hijack the
	 * hardware, using these two function to drive the hardware
	 * directly.
	 */


	void do_setup(ifxusb_core_if_t *_core_if)
	{

		ifxusb_core_global_regs_t *global_regs    = _core_if->core_global_regs;
		ifxusb_host_global_regs_t *hc_global_regs = _core_if->host_global_regs;
		ifxusb_hc_regs_t          *hc_regs        = _core_if->hc_regs[0];
		uint32_t                  *data_fifo      = _core_if->data_fifo[0];

		gint_data_t    gintsts;
		hctsiz_data_t  hctsiz;
		hcchar_data_t  hcchar;
		haint_data_t   haint;
		hcint_data_t   hcint;


		/* Enable HAINTs */
		ifxusb_wreg(&hc_global_regs->haintmsk, 0x0001);

		/* Enable HCINTs */
		ifxusb_wreg(&hc_regs->hcintmsk, 0x04a3);

		/* Read GINTSTS */
		gintsts.d32 = ifxusb_rreg(&global_regs->gintsts);
		//fprintf(stderr, "GINTSTS: %08x\n", gintsts.d32);

		/* Read HAINT */
		haint.d32 = ifxusb_rreg(&hc_global_regs->haint);
		//fprintf(stderr, "HAINT: %08x\n", haint.d32);

		/* Read HCINT */
		hcint.d32 = ifxusb_rreg(&hc_regs->hcint);
		//fprintf(stderr, "HCINT: %08x\n", hcint.d32);

		/* Read HCCHAR */
		hcchar.d32 = ifxusb_rreg(&hc_regs->hcchar);
		//fprintf(stderr, "HCCHAR: %08x\n", hcchar.d32);

		/* Clear HCINT */
		ifxusb_wreg(&hc_regs->hcint, hcint.d32);

		/* Clear HAINT */
		ifxusb_wreg(&hc_global_regs->haint, haint.d32);

		/* Clear GINTSTS */
		ifxusb_wreg(&global_regs->gintsts, gintsts.d32);

		/* Read GINTSTS */
		gintsts.d32 = ifxusb_rreg(&global_regs->gintsts);
		//fprintf(stderr, "GINTSTS: %08x\n", gintsts.d32);

		/*
		 * Send Setup packet (Get Device Descriptor)
		 */

		/* Make sure channel is disabled */
		hcchar.d32 = ifxusb_rreg(&hc_regs->hcchar);
		if (hcchar.b.chen) {
			//fprintf(stderr, "Channel already enabled 1, HCCHAR = %08x\n", hcchar.d32);
			hcchar.b.chdis = 1;
	//		hcchar.b.chen = 1;
			ifxusb_wreg(&hc_regs->hcchar, hcchar.d32);
			//sleep(1);
			mdelay(1000);

			/* Read GINTSTS */
			gintsts.d32 = ifxusb_rreg(&global_regs->gintsts);
			//fprintf(stderr, "GINTSTS: %08x\n", gintsts.d32);

			/* Read HAINT */
			haint.d32 = ifxusb_rreg(&hc_global_regs->haint);
			//fprintf(stderr, "HAINT: %08x\n", haint.d32);

			/* Read HCINT */
			hcint.d32 = ifxusb_rreg(&hc_regs->hcint);
			//fprintf(stderr, "HCINT: %08x\n", hcint.d32);

			/* Read HCCHAR */
			hcchar.d32 = ifxusb_rreg(&hc_regs->hcchar);
			//fprintf(stderr, "HCCHAR: %08x\n", hcchar.d32);

			/* Clear HCINT */
			ifxusb_wreg(&hc_regs->hcint, hcint.d32);

			/* Clear HAINT */
			ifxusb_wreg(&hc_global_regs->haint, haint.d32);

			/* Clear GINTSTS */
			ifxusb_wreg(&global_regs->gintsts, gintsts.d32);

			hcchar.d32 = ifxusb_rreg(&hc_regs->hcchar);
			//if (hcchar.b.chen) {
			//	fprintf(stderr, "** Channel _still_ enabled 1, HCCHAR = %08x **\n", hcchar.d32);
			//}
		}

		/* Set HCTSIZ */
		hctsiz.d32 = 0;
		hctsiz.b.xfersize = 8;
		hctsiz.b.pktcnt = 1;
		hctsiz.b.pid = IFXUSB_HC_PID_SETUP;
		ifxusb_wreg(&hc_regs->hctsiz, hctsiz.d32);

		/* Set HCCHAR */
		hcchar.d32 = ifxusb_rreg(&hc_regs->hcchar);
		hcchar.b.eptype = IFXUSB_EP_TYPE_CTRL;
		hcchar.b.epdir = 0;
		hcchar.b.epnum = 0;
		hcchar.b.mps = 8;
		hcchar.b.chen = 1;
		ifxusb_wreg(&hc_regs->hcchar, hcchar.d32);

		/* Fill FIFO with Setup data for Get Device Descriptor */
		ifxusb_wreg(data_fifo++, 0x01000680);
		ifxusb_wreg(data_fifo++, 0x00080000);

		gintsts.d32 = ifxusb_rreg(&global_regs->gintsts);
		//fprintf(stderr, "Waiting for HCINTR intr 1, GINTSTS = %08x\n", gintsts.d32);

		/* Wait for host channel interrupt */
		do {
			gintsts.d32 = ifxusb_rreg(&global_regs->gintsts);
		} while (gintsts.b.hcintr == 0);

		//fprintf(stderr, "Got HCINTR intr 1, GINTSTS = %08x\n", gintsts.d32);

		/* Disable HCINTs */
		ifxusb_wreg(&hc_regs->hcintmsk, 0x0000);

		/* Disable HAINTs */
		ifxusb_wreg(&hc_global_regs->haintmsk, 0x0000);

		/* Read HAINT */
		haint.d32 = ifxusb_rreg(&hc_global_regs->haint);
		//fprintf(stderr, "HAINT: %08x\n", haint.d32);

		/* Read HCINT */
		hcint.d32 = ifxusb_rreg(&hc_regs->hcint);
		//fprintf(stderr, "HCINT: %08x\n", hcint.d32);

		/* Read HCCHAR */
		hcchar.d32 = ifxusb_rreg(&hc_regs->hcchar);
		//fprintf(stderr, "HCCHAR: %08x\n", hcchar.d32);

		/* Clear HCINT */
		ifxusb_wreg(&hc_regs->hcint, hcint.d32);

		/* Clear HAINT */
		ifxusb_wreg(&hc_global_regs->haint, haint.d32);

		/* Clear GINTSTS */
		ifxusb_wreg(&global_regs->gintsts, gintsts.d32);

		/* Read GINTSTS */
		gintsts.d32 = ifxusb_rreg(&global_regs->gintsts);
		//fprintf(stderr, "GINTSTS: %08x\n", gintsts.d32);
	}

	void do_in_ack(ifxusb_core_if_t *_core_if)
	{

		ifxusb_core_global_regs_t *global_regs    = _core_if->core_global_regs;
		ifxusb_host_global_regs_t *hc_global_regs = _core_if->host_global_regs;
		ifxusb_hc_regs_t          *hc_regs        = _core_if->hc_regs[0];
		uint32_t                  *data_fifo      = _core_if->data_fifo[0];

		gint_data_t        gintsts;
		hctsiz_data_t      hctsiz;
		hcchar_data_t      hcchar;
		haint_data_t       haint;
		hcint_data_t       hcint;
		grxsts_data_t      grxsts;

		/* Enable HAINTs */
		ifxusb_wreg(&hc_global_regs->haintmsk, 0x0001);

		/* Enable HCINTs */
		ifxusb_wreg(&hc_regs->hcintmsk, 0x04a3);

		/* Read GINTSTS */
		gintsts.d32 = ifxusb_rreg(&global_regs->gintsts);
		//fprintf(stderr, "GINTSTS: %08x\n", gintsts.d32);

		/* Read HAINT */
		haint.d32 = ifxusb_rreg(&hc_global_regs->haint);
		//fprintf(stderr, "HAINT: %08x\n", haint.d32);

		/* Read HCINT */
		hcint.d32 = ifxusb_rreg(&hc_regs->hcint);
		//fprintf(stderr, "HCINT: %08x\n", hcint.d32);

		/* Read HCCHAR */
		hcchar.d32 = ifxusb_rreg(&hc_regs->hcchar);
		//fprintf(stderr, "HCCHAR: %08x\n", hcchar.d32);

		/* Clear HCINT */
		ifxusb_wreg(&hc_regs->hcint, hcint.d32);

		/* Clear HAINT */
		ifxusb_wreg(&hc_global_regs->haint, haint.d32);

		/* Clear GINTSTS */
		ifxusb_wreg(&global_regs->gintsts, gintsts.d32);

		/* Read GINTSTS */
		gintsts.d32 = ifxusb_rreg(&global_regs->gintsts);
		//fprintf(stderr, "GINTSTS: %08x\n", gintsts.d32);

		/*
		 * Receive Control In packet
		 */

		/* Make sure channel is disabled */
		hcchar.d32 = ifxusb_rreg(&hc_regs->hcchar);
		if (hcchar.b.chen) {
			//fprintf(stderr, "Channel already enabled 2, HCCHAR = %08x\n", hcchar.d32);
			hcchar.b.chdis = 1;
			hcchar.b.chen = 1;
			ifxusb_wreg(&hc_regs->hcchar, hcchar.d32);
			//sleep(1);
			mdelay(1000);

			/* Read GINTSTS */
			gintsts.d32 = ifxusb_rreg(&global_regs->gintsts);
			//fprintf(stderr, "GINTSTS: %08x\n", gintsts.d32);

			/* Read HAINT */
			haint.d32 = ifxusb_rreg(&hc_global_regs->haint);
			//fprintf(stderr, "HAINT: %08x\n", haint.d32);

			/* Read HCINT */
			hcint.d32 = ifxusb_rreg(&hc_regs->hcint);
			//fprintf(stderr, "HCINT: %08x\n", hcint.d32);

			/* Read HCCHAR */
			hcchar.d32 = ifxusb_rreg(&hc_regs->hcchar);
			//fprintf(stderr, "HCCHAR: %08x\n", hcchar.d32);

			/* Clear HCINT */
			ifxusb_wreg(&hc_regs->hcint, hcint.d32);

			/* Clear HAINT */
			ifxusb_wreg(&hc_global_regs->haint, haint.d32);

			/* Clear GINTSTS */
			ifxusb_wreg(&global_regs->gintsts, gintsts.d32);

			hcchar.d32 = ifxusb_rreg(&hc_regs->hcchar);
			//if (hcchar.b.chen) {
			//	fprintf(stderr, "** Channel _still_ enabled 2, HCCHAR = %08x **\n", hcchar.d32);
			//}
		}

		/* Set HCTSIZ */
		hctsiz.d32 = 0;
		hctsiz.b.xfersize = 8;
		hctsiz.b.pktcnt = 1;
		hctsiz.b.pid = IFXUSB_HC_PID_DATA1;
		ifxusb_wreg(&hc_regs->hctsiz, hctsiz.d32);

		/* Set HCCHAR */
		hcchar.d32 = ifxusb_rreg(&hc_regs->hcchar);
		hcchar.b.eptype = IFXUSB_EP_TYPE_CTRL;
		hcchar.b.epdir = 1;
		hcchar.b.epnum = 0;
		hcchar.b.mps = 8;
		hcchar.b.chen = 1;
		ifxusb_wreg(&hc_regs->hcchar, hcchar.d32);

		gintsts.d32 = ifxusb_rreg(&global_regs->gintsts);
		//fprintf(stderr, "Waiting for RXSTSQLVL intr 1, GINTSTS = %08x\n", gintsts.d32);

		/* Wait for receive status queue interrupt */
		do {
			gintsts.d32 = ifxusb_rreg(&global_regs->gintsts);
		} while (gintsts.b.rxstsqlvl == 0);

		//fprintf(stderr, "Got RXSTSQLVL intr 1, GINTSTS = %08x\n", gintsts.d32);

		/* Read RXSTS */
		grxsts.d32 = ifxusb_rreg(&global_regs->grxstsp);
		//fprintf(stderr, "GRXSTS: %08x\n", grxsts.d32);

		/* Clear RXSTSQLVL in GINTSTS */
		gintsts.d32 = 0;
		gintsts.b.rxstsqlvl = 1;
		ifxusb_wreg(&global_regs->gintsts, gintsts.d32);

		switch (grxsts.hb.pktsts) {
			case IFXUSB_HSTS_DATA_UPDT:
				/* Read the data into the host buffer */
				if (grxsts.hb.bcnt > 0) {
					int i;
					int word_count = (grxsts.hb.bcnt + 3) / 4;

					for (i = 0; i < word_count; i++) {
						(void)ifxusb_rreg(data_fifo++);
					}
				}

				//fprintf(stderr, "Received %u bytes\n", (unsigned)grxsts.hb.bcnt);
				break;

			default:
				//fprintf(stderr, "** Unexpected GRXSTS packet status 1 **\n");
				break;
		}

		gintsts.d32 = ifxusb_rreg(&global_regs->gintsts);
		//fprintf(stderr, "Waiting for RXSTSQLVL intr 2, GINTSTS = %08x\n", gintsts.d32);

		/* Wait for receive status queue interrupt */
		do {
			gintsts.d32 = ifxusb_rreg(&global_regs->gintsts);
		} while (gintsts.b.rxstsqlvl == 0);

		//fprintf(stderr, "Got RXSTSQLVL intr 2, GINTSTS = %08x\n", gintsts.d32);

		/* Read RXSTS */
		grxsts.d32 = ifxusb_rreg(&global_regs->grxstsp);
		//fprintf(stderr, "GRXSTS: %08x\n", grxsts.d32);

		/* Clear RXSTSQLVL in GINTSTS */
		gintsts.d32 = 0;
		gintsts.b.rxstsqlvl = 1;
		ifxusb_wreg(&global_regs->gintsts, gintsts.d32);

		switch (grxsts.hb.pktsts) {
			case IFXUSB_HSTS_XFER_COMP:
				break;

			default:
				//fprintf(stderr, "** Unexpected GRXSTS packet status 2 **\n");
				break;
		}

		gintsts.d32 = ifxusb_rreg(&global_regs->gintsts);
		//fprintf(stderr, "Waiting for HCINTR intr 2, GINTSTS = %08x\n", gintsts.d32);

		/* Wait for host channel interrupt */
		do {
			gintsts.d32 = ifxusb_rreg(&global_regs->gintsts);
		} while (gintsts.b.hcintr == 0);

		//fprintf(stderr, "Got HCINTR intr 2, GINTSTS = %08x\n", gintsts.d32);

		/* Read HAINT */
		haint.d32 = ifxusb_rreg(&hc_global_regs->haint);
		//fprintf(stderr, "HAINT: %08x\n", haint.d32);

		/* Read HCINT */
		hcint.d32 = ifxusb_rreg(&hc_regs->hcint);
		//fprintf(stderr, "HCINT: %08x\n", hcint.d32);

		/* Read HCCHAR */
		hcchar.d32 = ifxusb_rreg(&hc_regs->hcchar);
		//fprintf(stderr, "HCCHAR: %08x\n", hcchar.d32);

		/* Clear HCINT */
		ifxusb_wreg(&hc_regs->hcint, hcint.d32);

		/* Clear HAINT */
		ifxusb_wreg(&hc_global_regs->haint, haint.d32);

		/* Clear GINTSTS */
		ifxusb_wreg(&global_regs->gintsts, gintsts.d32);

		/* Read GINTSTS */
		gintsts.d32 = ifxusb_rreg(&global_regs->gintsts);
		//fprintf(stderr, "GINTSTS: %08x\n", gintsts.d32);

	//	usleep(100000);
	//	mdelay(100);
		mdelay(1);

		/*
		 * Send handshake packet
		 */

		/* Read HAINT */
		haint.d32 = ifxusb_rreg(&hc_global_regs->haint);
		//fprintf(stderr, "HAINT: %08x\n", haint.d32);

		/* Read HCINT */
		hcint.d32 = ifxusb_rreg(&hc_regs->hcint);
		//fprintf(stderr, "HCINT: %08x\n", hcint.d32);

		/* Read HCCHAR */
		hcchar.d32 = ifxusb_rreg(&hc_regs->hcchar);
		//fprintf(stderr, "HCCHAR: %08x\n", hcchar.d32);

		/* Clear HCINT */
		ifxusb_wreg(&hc_regs->hcint, hcint.d32);

		/* Clear HAINT */
		ifxusb_wreg(&hc_global_regs->haint, haint.d32);

		/* Clear GINTSTS */
		ifxusb_wreg(&global_regs->gintsts, gintsts.d32);

		/* Read GINTSTS */
		gintsts.d32 = ifxusb_rreg(&global_regs->gintsts);
		//fprintf(stderr, "GINTSTS: %08x\n", gintsts.d32);

		/* Make sure channel is disabled */
		hcchar.d32 = ifxusb_rreg(&hc_regs->hcchar);
		if (hcchar.b.chen) {
			//fprintf(stderr, "Channel already enabled 3, HCCHAR = %08x\n", hcchar.d32);
			hcchar.b.chdis = 1;
			hcchar.b.chen = 1;
			ifxusb_wreg(&hc_regs->hcchar, hcchar.d32);
			//sleep(1);
			mdelay(1000);

			/* Read GINTSTS */
			gintsts.d32 = ifxusb_rreg(&global_regs->gintsts);
			//fprintf(stderr, "GINTSTS: %08x\n", gintsts.d32);

			/* Read HAINT */
			haint.d32 = ifxusb_rreg(&hc_global_regs->haint);
			//fprintf(stderr, "HAINT: %08x\n", haint.d32);

			/* Read HCINT */
			hcint.d32 = ifxusb_rreg(&hc_regs->hcint);
			//fprintf(stderr, "HCINT: %08x\n", hcint.d32);

			/* Read HCCHAR */
			hcchar.d32 = ifxusb_rreg(&hc_regs->hcchar);
			//fprintf(stderr, "HCCHAR: %08x\n", hcchar.d32);

			/* Clear HCINT */
			ifxusb_wreg(&hc_regs->hcint, hcint.d32);

			/* Clear HAINT */
			ifxusb_wreg(&hc_global_regs->haint, haint.d32);

			/* Clear GINTSTS */
			ifxusb_wreg(&global_regs->gintsts, gintsts.d32);

			hcchar.d32 = ifxusb_rreg(&hc_regs->hcchar);
			//if (hcchar.b.chen) {
			//	fprintf(stderr, "** Channel _still_ enabled 3, HCCHAR = %08x **\n", hcchar.d32);
			//}
		}

		/* Set HCTSIZ */
		hctsiz.d32 = 0;
		hctsiz.b.xfersize = 0;
		hctsiz.b.pktcnt = 1;
		hctsiz.b.pid = IFXUSB_HC_PID_DATA1;
		ifxusb_wreg(&hc_regs->hctsiz, hctsiz.d32);

		/* Set HCCHAR */
		hcchar.d32 = ifxusb_rreg(&hc_regs->hcchar);
		hcchar.b.eptype = IFXUSB_EP_TYPE_CTRL;
		hcchar.b.epdir = 0;
		hcchar.b.epnum = 0;
		hcchar.b.mps = 8;
		hcchar.b.chen = 1;
		ifxusb_wreg(&hc_regs->hcchar, hcchar.d32);

		gintsts.d32 = ifxusb_rreg(&global_regs->gintsts);
		//fprintf(stderr, "Waiting for HCINTR intr 3, GINTSTS = %08x\n", gintsts.d32);

		/* Wait for host channel interrupt */
		do {
			gintsts.d32 = ifxusb_rreg(&global_regs->gintsts);
		} while (gintsts.b.hcintr == 0);

		//fprintf(stderr, "Got HCINTR intr 3, GINTSTS = %08x\n", gintsts.d32);

		/* Disable HCINTs */
		ifxusb_wreg(&hc_regs->hcintmsk, 0x0000);

		/* Disable HAINTs */
		ifxusb_wreg(&hc_global_regs->haintmsk, 0x0000);

		/* Read HAINT */
		haint.d32 = ifxusb_rreg(&hc_global_regs->haint);
		//fprintf(stderr, "HAINT: %08x\n", haint.d32);

		/* Read HCINT */
		hcint.d32 = ifxusb_rreg(&hc_regs->hcint);
		//fprintf(stderr, "HCINT: %08x\n", hcint.d32);

		/* Read HCCHAR */
		hcchar.d32 = ifxusb_rreg(&hc_regs->hcchar);
		//fprintf(stderr, "HCCHAR: %08x\n", hcchar.d32);

		/* Clear HCINT */
		ifxusb_wreg(&hc_regs->hcint, hcint.d32);

		/* Clear HAINT */
		ifxusb_wreg(&hc_global_regs->haint, haint.d32);

		/* Clear GINTSTS */
		ifxusb_wreg(&global_regs->gintsts, gintsts.d32);

		/* Read GINTSTS */
		gintsts.d32 = ifxusb_rreg(&global_regs->gintsts);
		//fprintf(stderr, "GINTSTS: %08x\n", gintsts.d32);
	}
#endif //__WITH_HS_ELECT_TST__

