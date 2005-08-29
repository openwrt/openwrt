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

#define HOST_TIMEOUT 300

#include "wl_access.h"
#include "structs.h"
#include "channelhopper.h"

#ifdef WIN32
#define OFFLINE
#endif
#ifndef __cplusplus
#define __cdecl
#endif

#define nonzeromac(x) memcmp(x, "\0\0\0\0\0\0", 6)

void dealWithPacket(wiviz_cfg * cfg, struct pcap_pkthdr * header, const u_char * packet);
wiviz_host * gotHost(wiviz_cfg * cfg, u_char * mac, host_type type);
void fprint_mac(FILE * outf, u_char * mac, char * extra);
void print_mac(u_char * mac, char * extra);
void print_host(FILE * outf, wiviz_host * host);
void __cdecl signal_handler(int);
void readWL(wiviz_cfg * cfg);
void reloadConfig();

wiviz_cfg * global_cfg;

////////////////////////////////////////////////////////////////////////////////
int main(int argc, char * * argv) {
  pcap_t *handle;                        
  char *dev;                                
  char errbuf[PCAP_ERRBUF_SIZE]; 
  int stop = 0;
  int oldMonitor, newMonitor;
  struct pcap_pkthdr header;          
  const u_char *packet;                 
  wiviz_cfg cfg;
  int i;
  int defaultHopSeq[] = { 1, 3, 6, 8, 11 };
  
  global_cfg = &cfg;
  signal(SIGUSR1, &signal_handler);
  signal(SIGUSR2, &signal_handler);

  fprintf(stderr, "Wi-Viz infogathering daemon by Nathan True\n");
  
  memset(&cfg, 0, sizeof(wiviz_cfg));
  cfg.numHosts = 0;
  cfg.lastKeepAlive = time(NULL);
  cfg.channelHopping = 0;
  cfg.channelDwellTime = 1000;
  cfg.channelHopSeqLen = 5;
  memcpy(cfg.channelHopSeq, defaultHopSeq, sizeof(defaultHopSeq));

  wl_ioctl(WL_DEVICE, WLC_GET_MAGIC, &i, 4);
	if (i != WLC_IOCTL_MAGIC) {
		fprintf(stderr, "Wireless magic not correct, not querying wl for info\n");
		cfg.readFromWl = 0;
	}
	else {
	  cfg.readFromWl = 1;
	  wl_ioctl(WL_DEVICE, WLC_GET_MONITOR, &oldMonitor, 4);
	  newMonitor = 1;
	  wl_ioctl(WL_DEVICE, WLC_SET_MONITOR, &newMonitor, 4);
	}

  reloadConfig();

#ifndef OFFLINE
  dev = "prism0";
  handle = pcap_open_live(dev, BUFSIZ, 1, 0, errbuf);
#else
  dev = "c:\\cifsroot\\wdump2.pcap";
  handle = pcap_open_offline(dev, errbuf);
#endif

	if (cfg.readFromWl) {
	  readWL(&cfg);
	}

  if (!handle) {
    fprintf(stderr, "Failure to open pcap!\nErr=%s\n", errbuf);
    return -1;
    }
  while (!stop) {
    packet = pcap_next(handle, &header);
    if (!packet) break;
    dealWithPacket(&cfg, &header, packet);
    if (time(NULL) - cfg.lastKeepAlive > 30) stop = 1;
    }

  signal_handler(SIGUSR1);

  if (cfg.channelHopperPID) kill(cfg.channelHopperPID, SIGKILL);

  for (i = 0; i < MAX_HOSTS; i++) {
    print_host(stderr, cfg.hosts + i);
    if (cfg.hosts[i].occupied) printf("\n");
    if (cfg.hosts[i].apInfo) free(cfg.hosts[i].apInfo);
    if (cfg.hosts[i].staInfo) free(cfg.hosts[i].staInfo);
    }

  wl_ioctl(WL_DEVICE, WLC_SET_MONITOR, &oldMonitor, 4);

  pcap_close(handle);
  return 0;
  }

////////////////////////////////////////////////////////////////////////////////
void writeJavascript() { 
  int i;
  FILE * outf;
  wiviz_host * h;

  outf = fopen("/tmp/wiviz-pipe", "w");
  if (!outf) {
    fprintf(stderr, "Failure to open output file\n");
    return;
    }

  global_cfg->lastKeepAlive = time(NULL);
  
  if(global_cfg->readFromWl) readWL(global_cfg);
  
  fprintf(outf, "top.hosts = new Array(\n");
  for (i = 0; i < MAX_HOSTS; i++) {
    h = global_cfg->hosts + i;
    if (h->occupied == 0) continue;
    if (time(NULL) - h->lastSeen > HOST_TIMEOUT) {
      h->occupied = 0;
      }
    fprintf(outf, "  new Array(");
    print_host(outf, h);
    fprintf(outf, "),\n");
    }
  fprintf(outf, "new Array());\n");
  fprintf(outf, "var cfg_string = 'channel-");
  if (global_cfg->channelHopping) {
    fprintf(outf, "hopping");
    }
  else {
    fprintf(outf, "%i", global_cfg->curChannel);
    }
  fprintf(outf, "';\ntop.wiviz_callback(top.hosts, cfg_string);\n");
  fclose(outf);
  }

////////////////////////////////////////////////////////////////////////////////
void reloadConfig() {
  FILE * cnf;
  wiviz_cfg * cfg = global_cfg;
  char filebuffer[512];
  char * fbptr, * p, * v, * vv;
  int fblen, val;
  int hopCfgChanged = 0;
  int newHopSeq[12];
  int newHopSeqLen = 0;

  fprintf(stderr, "Loading config file\n");

  cnf = fopen("/tmp/wiviz-cfg", "r");
  if (!cnf) {
    fprintf(stderr, "Wiviz: No config file (/tmp/wiviz-cfg) present, using defaults\n");
    return;
    }

  fblen = fread(filebuffer, 1, 512, cnf);
  fclose(cnf);
  if (fblen >= 512) {
    fprintf(stderr, "Error reading config file\n");
    return;
    }
  filebuffer[fblen] = 0;
  fprintf(stderr, "Read %i bytes from config file\n", fblen);

  fbptr = filebuffer;

  while (fbptr < filebuffer + fblen && *fbptr != 0) {
    p = fbptr;
    //Find end of parameter
    for (; *fbptr != '=' && *fbptr != 0; fbptr++);
    *fbptr = 0;
    v = ++fbptr;
    //Find end of value
    for (; *fbptr != '&' && *fbptr != 0; fbptr++);
    *(fbptr++) = 0;
    fprintf(stderr, "Config: %s=%s\n", p, v);
    //Apply configuration
    if (!strcmp(p, "channelsel")) {
      //Channel selector
      cfg->channelHopping = 0;
      if (!strcmp(v, "hop")) {
        //Set channel hopping
        cfg->channelHopping = 1;
        hopCfgChanged = 1;
        }
      else if (!strcmp(v, "nochange")) {
        //Don't change anything, read channel from wireless card
        readWL(cfg);
        }
      else {
        val = atoi(v);
        if (val < 1 || val > 14) {
          fprintf(stderr, "Channel setting in config file invalid (%i)\n", cfg->curChannel);
        }
        else {
          cfg->curChannel = val;
          if (cfg->readFromWl) {
            if (wl_ioctl(WL_DEVICE, WLC_SET_CHANNEL, &cfg->curChannel, 4) < 0) {
              fprintf(stderr, "Channel set to %i failed\n", cfg->curChannel);
              }
            }
          else {
            fprintf(stderr, "Can't set channel, no Broadcom wireless device present\n");
            }
          }
        }
      }
    if (!strcmp(p, "hopdwell")) {
      val = atoi(v);
      if (val < 100) val = 100;
      if (val > 30000) val = 30000;
      if (cfg->channelDwellTime != val) hopCfgChanged = 1;
      cfg->channelDwellTime = val;
      }
    if (!strcmp(p, "hopseq")) {
      cfg->channelHopSeqLen = 0;
      while (v < fbptr) {
        for (vv = v; *vv != ',' && *vv != 0; vv++);
        if (*vv == 0) {
          cfg->channelHopSeq[cfg->channelHopSeqLen++] = atoi(v);
          break;          
          }
        *vv = 0;
        cfg->channelHopSeq[cfg->channelHopSeqLen++] = atoi(v);
        v = vv + 1;
        }
      }
    /*
    if (!strcmp(p, "")) {
      }
    */
    }
  //Apply channel hopper settings
  if (cfg->channelHopping == 0 && cfg->channelHopperPID) {
    kill(cfg->channelHopperPID, SIGKILL);
    cfg->channelHopperPID = 0;
    }
  if (cfg->channelHopping == 1 && hopCfgChanged) {
    if (cfg->channelHopperPID) kill(cfg->channelHopperPID, SIGKILL);
    if ((cfg->channelHopperPID = fork()) == 0) {
      channelHopper(cfg);
      }
    }
  }

////////////////////////////////////////////////////////////////////////////////
void __cdecl signal_handler(int signum) {
  if (signum == SIGUSR1) writeJavascript();
  if (signum == SIGUSR2) reloadConfig();
  }

////////////////////////////////////////////////////////////////////////////////
void dealWithPacket(wiviz_cfg * cfg, struct pcap_pkthdr * header, const u_char * packet) {
  ieee802_11_hdr * hWifi;
  prism_hdr * hPrism;
  wiviz_host * host;
  wiviz_host * emergebss;
  host_type type = typeUnknown;
  int wfType;
  int rssi = 0;
  int to_ds, from_ds;
  prism_did * i;
  ieee_802_11_tag * e;
  ieee_802_11_mgt_frame * m;
  char * src = "\0\0\0\0\0\0";
  char * dst = "\0\0\0\0\0\0";
  char * bss = "\0\0\0\0\0\0";
  char * ssid = "";
  int channel = 0;
  int adhocbeacon = 0;
  u_char ssidlen = 0;
  ap_enc_type encType = aetUnknown;

  if (!packet) return;
  if (header->len < sizeof(prism_hdr) + sizeof(ieee802_11_hdr)) return;
  hPrism = (prism_hdr *) packet;
  hWifi = (ieee802_11_hdr *) (packet + (hPrism->msg_length));

  //Parse the prism DIDs
  i = (prism_did *)((char *)hPrism + sizeof(prism_hdr));
  while ((int)i < (int)hWifi) {
    if (i->did == pdn_rssi) rssi = *(int *)(i+1);
    i = (prism_did *) ((int)(i+1) + i->length);
    }

  //Establish the frame type
  wfType = ((hWifi->frame_control & 0xF0) >> 4) + ((hWifi->frame_control & 0xC) << 2);
  switch (wfType) {
    case mgt_assocRequest:
    case mgt_reassocRequest:
    case mgt_probeRequest:
      type = typeSta;
      src=hWifi->addr2;
      dst=hWifi->addr1;
      break;
    case mgt_assocResponse:
    case mgt_reassocResponse:
    case mgt_probeResponse:
    case mgt_beacon:
      src=hWifi->addr2;
      dst=hWifi->addr1;
      bss=hWifi->addr3;
      type = typeAP;
      break;
    }
  to_ds = hWifi->flags & IEEE80211_TO_DS;
  from_ds = hWifi->flags & IEEE80211_FROM_DS;
  if ((wfType & 0xF0) == 0x20 && (wfType & 0xF) < 4) {
    //Data frame
    src=hWifi->addr2;
    dst=hWifi->addr1;
    if (!from_ds) type = typeSta;
      else type = typeAP;
    if (!to_ds && !from_ds) bss = hWifi->addr3;
    if (to_ds && !from_ds) bss = hWifi->addr1;
    if (!to_ds && from_ds) bss = hWifi->addr2;
    }
  if (type == typeUnknown) return;

  //Parse the 802.11 tags
  if (wfType == mgt_probeResponse || wfType == mgt_beacon) {
    m = (ieee_802_11_mgt_frame *) (hWifi + 1);
    if (m->caps & MGT_CAPS_IBSS) {
      type = typeSta;
      adhocbeacon = 1;
      }
    if (m->caps & MGT_CAPS_WEP) encType = aetEncWEP;
    else encType = aetUnencrypted;
    e = (ieee_802_11_tag *) ((int) m + sizeof(ieee_802_11_mgt_frame));
    while ((u_int)e < (u_int)packet + header->len) {
      if (e->tag == tagSSID) {
        ssidlen = e->length;
        ssid = (char *)(e + 1);
        }
      if (e->tag == tagChannel) {
        channel = *(char *)(e + 1);
        }
      if (e->tag == tagVendorSpecific) {
        if (e->length >= 4 && memcmp(e + 1, "\x00\x50\xf2\x01", 4) == 0) {
          //WPA encryption
          encType = aetEncWPA;
          }
        }
      e = (ieee_802_11_tag *) ((int)(e + 1) + e->length);
      }
    }
  
  //Look up the host in the hash table
  host = gotHost(cfg, src, type);

  //Add any info we received
  if (host->RSSI) {
    host->RSSI = host->RSSI * 9 / 10 + (-rssi * 10);
    }
  else {
    host->RSSI = -rssi * 100;
    }
  if (type == typeSta) {
    if (nonzeromac(bss)) {
      memcpy(host->staInfo->connectedBSSID, bss, 6);
      host->staInfo->state = ssAssociated;
      emergebss = gotHost(cfg, bss, typeAP);
      if (emergebss->RSSI == 0) emergebss->RSSI = 10000;
      memcpy(emergebss->apInfo->bssid, bss, 6);
      if (adhocbeacon) {
        emergebss->type = typeAdhocHub;
        if (ssidlen > 0 && ssidlen <= 32) {
          memcpy(emergebss->apInfo->ssid, ssid, ssidlen);
          emergebss->apInfo->ssidlen = ssidlen;
          }
        if (channel) emergebss->apInfo->channel = channel;
        emergebss->apInfo->flags = hWifi->flags;
        emergebss->RSSI = host->RSSI;
        if (encType != aetUnknown) emergebss->apInfo->encryption = encType;
        }
      }
    if (wfType == mgt_probeRequest && host->staInfo->state == ssUnknown) host->staInfo->state = ssUnassociated;
    }
  if (type == typeAP) {
    if (nonzeromac(bss)) {
      memcpy(host->apInfo->bssid, bss, 6);
      }
    if (ssidlen > 0 && ssidlen <= 32) {
      memcpy(host->apInfo->ssid, ssid, ssidlen);
      host->apInfo->ssidlen = ssidlen;
      }
    if (channel) host->apInfo->channel = channel;
    host->apInfo->flags = hWifi->flags;
    if (encType != aetUnknown) host->apInfo->encryption = encType;
    }
  }

////////////////////////////////////////////////////////////////////////////////
void print_mac(u_char * mac, char * extra) {
  fprint_mac(stdout, mac, extra);
  }

////////////////////////////////////////////////////////////////////////////////
void fprint_mac(FILE * outf, u_char * mac, char * extra) {
  fprintf(outf, "%02X:%02X:%02X:%02X:%02X:%02X%s",
      mac[0] & 0xFF,
      mac[1] & 0xFF,
      mac[2] & 0xFF,
      mac[3] & 0xFF,
      mac[4] & 0xFF,
      mac[5] & 0xFF,
      extra);
  }

////////////////////////////////////////////////////////////////////////////////
#define MAX_PROBES MAX_HOSTS/2
wiviz_host * gotHost(wiviz_cfg * cfg, u_char * mac, host_type type) {
  int i = (mac[5] + (mac[4] << 8)) % MAX_HOSTS;
  int c = 0;
  wiviz_host * h = cfg->hosts + i;
  while (h->occupied && memcmp(h->mac, mac, 6)) {
    i++; h++; c++;
    if (i >= MAX_HOSTS) {
      i = 0;
      h = cfg->hosts;
      }
    if (c > MAX_PROBES) break;
    } 
  if (!h->occupied) {
    fprintf(stderr, "New host, ");
    fprint_mac(stderr, mac, ", type=");
    fprintf(stderr, "%s\n", (type==typeAP) ? "AP" : ((type==typeSta) ? "Sta" : "Unk"));
    }
  h->occupied = 1;
  h->lastSeen = time(NULL);
  h->type = type;
  memcpy(h->mac, mac, 6);
  if (h->type == typeAP && !h->apInfo) {
    h->apInfo = (ap_info *) malloc(sizeof(ap_info));
    memset(h->apInfo, 0, sizeof(ap_info));
    }
  if (h->type == typeSta && !h->staInfo) {
    h->staInfo = (sta_info *) malloc(sizeof(sta_info));
    memset(h->staInfo, 0, sizeof(sta_info));
    }
  return h;
  }

////////////////////////////////////////////////////////////////////////////////
void print_host(FILE * outf, wiviz_host * host) {
  int i;

  if (!host->occupied) return;
  fprintf(outf, "'");
  fprint_mac(outf, host->mac, "'");
  fprintf(outf, ", -%i, '", host->RSSI / 100);
  switch (host->type) {
    case typeAP:  fprintf(outf, "ap"); break;
    case typeSta: fprintf(outf, "sta"); break;
    case typeAdhocHub: fprintf(outf, "adhoc"); break;
    }
  if (host->type == typeSta) {
    switch(host->staInfo->state) {
      case ssAssociated:
        fprintf(outf, "-assoc-");
        fprint_mac(outf, host->staInfo->connectedBSSID, "");
        break;
      case ssUnassociated:
        fprintf(outf, "-unassoc");
      }
    }
  if (host->type == typeAP || host->type == typeAdhocHub) {
    fprintf(outf, "-channel-%i-ssid-", host->apInfo->channel & 0xFF);
    for (i = 0; i < host->apInfo->ssidlen; i++) {
      fprintf(outf, "\\x%02X", *((char *)host->apInfo->ssid + i) & 0xFF);
    	}
    switch (host->apInfo->encryption) {
      case aetUnknown: fprintf(outf, "-?enc-?alg"); break;
      case aetUnencrypted: fprintf(outf, "-unenc-na"); break;
      case aetEncUnknown: fprintf(outf, "-enc-unknown"); break;
      case aetEncWEP: fprintf(outf, "-enc-wep"); break;
      case aetEncWPA: fprintf(outf, "-enc-wpa"); break;
      }
    }
  fprintf(outf, "', %i", time(0) - host->lastSeen);
  }

////////////////////////////////////////////////////////////////////////////////
#define MAX_STA_COUNT 64
void readWL(wiviz_cfg * cfg) {
	int ap, i;
	wiviz_host * host, * sta;
	uchar mac[6];
	wlc_ssid_t ssid; 
	channel_info_t channel;
	maclist_t * macs;
  sta_rssi_t starssi;
	
	get_mac(WL_DEVICE, mac);
	printf("AP mac: ");
	print_mac(mac, "\n");
	if (!nonzeromac(mac)) return;
	wl_ioctl(WL_DEVICE, WLC_GET_AP, &ap, 4);
	if (ap) {
		host = gotHost(cfg, mac, typeAP);
		wl_ioctl(WL_DEVICE, WLC_GET_BSSID, host->apInfo->bssid, 6);
		wl_ioctl(WL_DEVICE, WLC_GET_SSID, &ssid, sizeof(wlc_ssid_t));
		memcpy(host->apInfo->ssid, ssid.SSID, 32);
		host->apInfo->ssidlen = ssid.SSID_len;
		host->RSSI = 0;
		wl_ioctl(WL_DEVICE, WLC_GET_CHANNEL, &channel, sizeof(channel_info_t));
		host->apInfo->channel = channel.hw_channel;
		macs = (maclist_t *) malloc(4 + MAX_STA_COUNT * sizeof(ether_addr_t));
		macs->count = MAX_STA_COUNT;
		if (wl_ioctl(WL_DEVICE, WLC_GET_ASSOCLIST, macs, 4 + MAX_STA_COUNT * sizeof(ether_addr_t)) > -1) {
			for (i = 0; i < macs->count; i++) {
			  sta = gotHost(cfg, (char *)&macs->ea[i], typeSta);
        memcpy(starssi.mac, &macs->ea[i], 6);
        starssi.RSSI = 3000;
        starssi.zero_ex_forty_one = 0x41;
				if (wl_ioctl(WL_DEVICE, WLC_GET_RSSI, &starssi, 12) < 0) printf("rssifail\n");
				sta->RSSI = -starssi.RSSI * 100;
				sta->staInfo->state = ssAssociated;
				memcpy(sta->staInfo->connectedBSSID, host->apInfo->bssid, 6);
			}
		}
	}
	else {
		host = gotHost(cfg, mac, typeSta);
		host->RSSI = 0;
		if (wl_ioctl(WL_DEVICE, WLC_GET_BSSID, &host->staInfo->connectedBSSID, 6) < 0) {
		  host->staInfo->state = ssUnassociated;
		}
		else {
		  host->staInfo->state = ssAssociated;
		}
	}
  if (wl_ioctl(WL_DEVICE, WLC_GET_CHANNEL, &channel, sizeof(channel_info_t)) >= 0) {
    cfg->curChannel = channel.hw_channel;
    fprintf(stderr, "Current channel is %i\n", cfg->curChannel);
    }
}





