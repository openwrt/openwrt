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
//Structure definitions for wireless packets

#define MAX_HOSTS 257

#ifdef DEFINE_TYPES
typedef unsigned short u_short;
typedef unsigned char u_char;
typedef unsigned int u_int;
#endif

typedef enum {
  mgt_assocRequest = 0,
  mgt_assocResponse = 1,
  mgt_reassocRequest = 2,
  mgt_reassocResponse = 3,
  mgt_probeRequest = 4,
  mgt_probeResponse = 5,
  mgt_beacon = 8,
  mgt_disassoc = 10,
  mgt_auth = 11,
  mgt_deauth = 12
  } wifi_frametype;

typedef struct ieee802_11_hdr {
  u_char frame_control;
  u_char flags;
#define IEEE80211_TO_DS 0x01
#define IEEE80211_FROM_DS 0x02
#define IEEE80211_MORE_FRAG 0x04
#define IEEE80211_RETRY 0x08
#define IEEE80211_PWR_MGT 0x10
#define IEEE80211_MORE_DATA 0x20
#define IEEE80211_WEP_FLAG 0x40
#define IEEE80211_ORDER_FLAG 0x80
  u_short duration;
  u_char addr1[6];
  u_char addr2[6];
  u_char addr3[6];
  u_short frag_and_seq;
  } ieee802_11_hdr;

typedef struct {
  u_char timestamp[8];
  u_short bcn_interval;
  u_short caps;
#define MGT_CAPS_AP 0x1
#define MGT_CAPS_IBSS 0x2
#define MGT_CAPS_WEP 0x10
  } ieee_802_11_mgt_frame;

typedef struct {
  u_char tag;
  u_char length;
  } ieee_802_11_tag;

typedef enum {
  tagSSID = 0,
  tagRates = 1,
  tagChannel = 3,
  tagVendorSpecific = 0xDD
  } i81tag;

typedef struct prism_hdr {
  u_int msg_code;
  u_int msg_length;
  char cap_device[16];
  //char dids[0];
  } prism_hdr;

typedef struct prism_did {
  u_short did;
  u_short status1;
  u_short status2;
  u_short length;
  //int value[0];
  } prism_did;

typedef enum prism_did_num {
  pdn_host_time = 0x1041,
  pdn_mac_time = 0x2041,
  pdn_rssi = 0x4041,
  pdn_sq = 0x5041,
  pdn_datarate = 0x8041,
  pdn_framelen = 0xa041
  } prism_did_num;



//Structure definitions for data collection

typedef enum {
  typeUnknown,
  typeAP,
  typeSta,
  typeAdhocHub
  } host_type;

typedef enum {
  ssUnknown,
  ssUnassociated,
  ssAssociated
  } sta_state;

typedef enum {
  aetUnknown,
  aetUnencrypted,
  aetEncUnknown,
  aetEncWEP,
  aetEncWPA
  } ap_enc_type;

typedef struct {
  u_char bssid[6];
  char * ssid[32];
  u_char ssidlen;
  u_char channel;
  u_short flags;
  ap_enc_type encryption;
  } ap_info;

typedef struct {
  sta_state state;
  u_char connectedBSSID[6];
  } sta_info;

typedef struct {
  u_char occupied;
  u_char mac[6];
  host_type type;
  time_t lastSeen;
  int RSSI;
  ap_info * apInfo;
  sta_info * staInfo;
  } wiviz_host;

//Primary config struct
typedef struct {
  wiviz_host hosts[MAX_HOSTS];
  int numHosts;
  int readFromWl;
  time_t lastKeepAlive;
  int channelHopping;
  int channelDwellTime;
  int channelHopSeq[14];
  int channelHopSeqLen;
  int curChannel;
  int channelHopperPID;
  } wiviz_cfg;





