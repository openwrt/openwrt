/*
This file is part of Wi-viz (http://wiviz.natetrue.com).

Wi-viz is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License v2 as published by
the Free Software Foundation.

Wi-viz is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wi-viz; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#include <stdio.h>
#include <pcap.h>
#include <signal.h>
#include <sys/time.h>
#include "wl_access.h"
#include "channelhopper.h"
#include "structs.h"

void ch_sig_handler(int i) {

  }

void channelHopper(wiviz_cfg * cfg) {
  int hopPos;
  int nc;

  //Turn off signal handling from parent process
  signal(SIGUSR1, &ch_sig_handler);
  signal(SIGUSR2, &ch_sig_handler);

  //Start hoppin'!
  hopPos = 0;
  while (1) {
    nc = cfg->channelHopSeq[hopPos];
    hopPos = (hopPos + 1) % cfg->channelHopSeqLen;
    //Set the channel
    fprintf(stderr, "It sets the channel to %i\n", nc);
    wl_ioctl(WL_DEVICE, WLC_SET_CHANNEL, &nc, 4);
    //Sleep
    usleep(cfg->channelDwellTime * 1000);
    }
  }
