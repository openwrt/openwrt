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


#if 0
Wireless Module Interface (WMI) Documentaion

   This section describes the format and the usage model for WMI control and
   data messages between the host and the AR6000-based targets. The header
   file include/wmi.h contains all command and event manifest constants as
   well as structure typedefs for each set of command and reply parameters.

Data Frames

   The data payload transmitted and received by the target follows RFC-1042
   encapsulation and thus starts with an 802.2-style LLC-SNAP header. The
   WLAN module completes 802.11 encapsulation of the payload, including the
   MAC header, FCS, and WLAN security related fields. At the interface to the
   message transport (HTC), a data frame is encapsulated in a WMI message.

WMI Message Structure

   The WMI protocol leverages an 802.3-style Ethernet header in communicating
   the source and destination information between the host and the AR6000
   modules using a 14-byte 802.3 header ahead of the 802.2-style payload. In
   addition, the WMI protocol adds a header to all data messages:

    {
    INT8    rssi
                  The RSSI of the received packet and its units are shown in db above the
                      noise floor, and the noise floor is shown in dbm.
    UINT8   info
                      Contains information on message type and user priority. Message type
                      differentiates between a data packet and a synchronization message.
    } WMI_DATA_HDR

   User priority contains the 802.1d user priority info from host to target. Host
   software translates the host Ethernet format to 802.3 format prior to Tx and
   802.3 format to host format in the Rx direction. The host does not transmit the
   FCS that follows the data. MsgType differentiates between a regular data
   packet (msgType=0) and a synchronization message (msgType=1).

Data Endpoints

   The AR6000 chipset provides several data endpoints to support quality of
   service (QoS) and maintains separate queues and separate DMA engines for
   each data endpoint. A data endpoint can be bi-directional.

   Best effort (BE) class traffic uses the default data endpoint (2). The host can
   establish up to two additional data endpoints for other traffic classes. Once
   such a data endpoint is established, it sends and receives corresponding QoS
   traffic in a manner similar to the default data endpoint.

   If QoS is desired over the interconnect, host software must classify each data
   packet and place it on the appropriate data endpoint. The information
   required to classify data is generally available in-band as an 802.1p/q style
   tag or as the ToS field in the IP header. The information may also be available
   out-of-band depending on the host DDI.

Connection States

   Table B-1 describes the AR6000 WLAN connection states:

   Table B-1.  AR6000 Connection States

Connection State
    Description

   DISCONNECTED
    In this state, the AR6000 device is not connected to a wireless
   network. The device is in this state after reset when it sends the
   WIRELESS MODULE “READY” EVENT, after it processes a
   DISCONNECT command, and when it loses its link with the
   access point (AP) that it was connected to. The device signals a
   transition to the DISCONNECTED state with a “DISCONNECT”
   event.

CONNECTED
    In this state, the AR6000 device is connected to wireless networks.
   The device enters this state after successfully processing a
   CONNECT, which establishes a connection with a wireless
   network. The device signals a transition to the CONNECTED state
   with a “CONNECT” event.


Message Types

   WMI uses commands, replies, and events for the control and configuration of
   the AR6000 device. The control protocol is asynchronous. Table B-2 describes
   AR6000 message types:

Table B-2.  AR6000 Message Types

Message Type
    Description

Commands
    Control messages that flow from the host to the device

Replies/Events
    Control messages that flow from the device to the host.

   The device issues a reply to some WMI commands, but not to others.
   The payload in a reply is command-specific, and some commands do
   not trigger a reply message at all. Events are control messages issued
   by the device to signal the occurrence of an asynchronous event.


WMI Message Format

   All WMI control commands, replies and events use the header format:

   WMI_CMD_HDR Header Format
   {
        UINT16 id
                 This 16-bit constant identifies which WMI command the host is issuing,
                 which command the target is replying to, or which event has occurred.
        WMI_CMD_HDR
   }


   A variable-size command-, reply-, or event-specific payload follows the
   header. Over the interconnect, all fields in control messages (including
   WMI_CMD_HDR and the command specific payload) use 32-bit little Endian
   byte ordering and fields are packed. The AR6000 device always executes
   commands in order, and the host may send multiple commands without
   waiting for previous commands to complete. A majority of commands are
   processed to completion once received. Other commands trigger a longer
   duration activity whose completion is signaled to the host through an event.

Command Restrictions

   Some commands may only be issued when the AR6000 device is in a certain
   state. The host is required to wait for an event signaling a state transition
   before such a command can be issued. For example, if a command requires
   the device to be in the CONNECTED state, then the host is required to wait
   for a “CONNECT” event before it issues that command.

   The device ignores any commands inappropriate for its current state. If the
   command triggers a reply, the device generates an error reply. Otherwise, the
   device silently ignores the inappropriate command.

Command and Data Synchronization

   WMI provides a mechanism for a host to advise the device of necessary
   synchronization between commands and data. The device implements
   synchronization; no implicit synchronization exists between endpoints.

   The host controls synchronization using the “SYNCHRONIZE” command
   over the control channel and synchronization messages over data channels.
   The device stops each data channel upon receiving a synchronization message
   on that channel, processing all data packets received prior to that message.
   After the device receives synchronization messages for each data endpoint
   and the “SYNCHRONIZE” command, it resumes all channels.

   When the host must guarantee a command executes before processing new
   data packets, it first issues the command, then issues the “SYNCHRONIZE”
   command and sends synchronization messages on data channels. When the
   host must guarantee the device has processed all old data packets before a
   processing a new command, it issues a “SYNCHRONIZE” command and
   synchronization messages on all data channels, then issues the desired
   command.



WMI Commands

   ADD_BAD_AP
    Cause the AR6000 device to avoid a particular AP
   ADD_CIPHER_KEY
    Add or replace any of the four AR6000 encryption keys
   ADD_WOW_PATTERN
    Used to add a pattern to the WoW pattern list
   CLR_RSSI_SNR
    Clear the current calculated RSSI and SNR value
   CONNECT_CMD
    Request that the AR6000 device establish a wireless connection
        with the specified SSID
   CREATE_PSTREAM
    Create prioritized data endpoint between the host and device
   DELETE_BAD_AP
    Clear an entry in the bad AP table
   DELETE_CIPHER_KEY
    Delete a previously added cipher key
   DELETE_PSTREAM
    Delete a prioritized data endpoint
   DELETE_WOW_PATTERN
    Remove a pre-specified pattern from the WoW pattern list
   EXTENSION
    WMI message interface command
   GET_BIT_RATE
    Retrieve rate most recently used by the AR6000
   GET_CHANNEL_LIST
    Retrieve list of channels used by the AR6000
   GET_FIXRATES
    Retrieves the rate-mask set via the SET_FIXRATES command.
   GET_PMKID_LIST_CMD
    Retrieve the firmware list of PMKIDs
   GET_ROAM_DATA
    Internal use for data collection; available in special build only
   GET_ROAM_TBL
    Retrieve the roaming table maintained on the target
   GET_TARGET_STATS
    Request that the target send the statistics it maintains
   GET_TX_PWR
    Retrieve the current AR6000 device Tx power levels
   GET_WOW_LIST
    Retrieve the current list of WoW patterns
   LQ_THRESHOLD_PARAMS
    Set the link quality thresholds
   OPT_TX_FRAME
    Send a special frame (special feature)
   RECONNECT
    Request a reconnection to a BSS
   RSSI_THRESHOLD_PARAMS
    Configure how the AR6000 device monitors and reports signal
       strength (RSSI) of the connected BSS
   SCAN_PARAMS
    Determine dwell time and changes scanned channels
   SET_ACCESS_PARAMS
    Set access parameters for the wireless network
   SET_ADHOC_BSSID
    Set the BSSID for an ad hoc network
   SET_AKMP_PARAMS
    Set multiPMKID mode
   SET_APPIE
    Add application-specified IE to a management frame
   SET_ASSOC_INFO
    Specify the IEs the device should add to association or
        reassociation requests
   SET_AUTH_MODE
    Set 802.11 authentication mode of reconnection
   SET_BEACON_INT
    Set the beacon interval for an ad hoc network
   SET_BIT_RATE
    Set the AR6000 to a specific fixed bit rate
   SET_BMISS_TIME
    Set the beacon miss time
   SET_BSS_FILTER
    Inform the AR6000 of network types about which it wants to
        receive information using a “BSSINFO” event
   SET_BT_PARAMS
    Set the status of a Bluetooth stream (SCO or A2DP) or set
        Bluetooth coexistence register parameters
   SET_BT_STATUS
    Set the status of a Bluetooth stream (SCO or A2DP)
   SET_CHANNEL_PARAMETERS
    Configure WLAN channel parameters
   SET_DISC_TIMEOUT
    Set the amount of time the AR6000 spends attempting to
        reestablish a connection
   SET_FIXRATES
    Set the device to a specific fixed PHY rate (supported subset)
   SET_HALPARAM
    Internal AR6000 command to set certain hardware parameters
   SET_HOST_SLEEP_MODE
    Set the host mode to asleep or awake
   SET_IBSS_PM_CAPS
    Support a non-standard power management scheme for an
        ad hoc network
   SET_LISTEN_INT
    Request a listen interval
   SET_LPREAMBLE
    Override the short preamble capability of the AR6000 device
   SET_MAX_SP_LEN
    Set the maximum service period
   SET_OPT_MODE
    Set the special mode on/off (special feature)
   SET_PMKID
    Set the pairwise master key ID (PMKID)
   SET_PMKID_LIST_CMD
    Configure the firmware list of PMKIDs
   SET_POWER_MODE
    Set guidelines on trade-off between power utilization
   SET_POWER_PARAMS
    Configure power parameters
   SET_POWERSAVE_PARAMS
    Set the two AR6000 power save timers
   SET_PROBED_SSID
    Provide list of SSIDs the device should seek
   SET_REASSOC_MODE
    Specify whether the disassociated frame should be sent upon
        reassociation
   SET_RETRY_LIMITS
    Limit how many times the device tries to send a frame
   SET_ROAM_CTRL
    Control roaming behavior
   SET_RTS
    Determine when RTS should be sent
   SET_SCAN_PARAMS
    Set the AR6000 scan parameters
   SET_TKIP_COUNTERMEASURES
    Enable/disable reports of TKIP MIC errors
   SET_TX_PWR
    Specify the AR6000 device Tx power levels
   SET_VOICE_PKT_SIZE
    Set voice packet size
   SET_WMM
    Override the AR6000 WMM capability
   SET_WMM_TXOP
    Configure TxOP bursting when sending traffic to a WMM-
    capable AP
   SET_WOW_MODE
    Enable/disable WoW mode
   SET_WSC_STATUS
    Enable/disable profile check in cserv when the WPS protocol
    is in progress
   SNR_THRESHOLD_PARAMS
    Configure how the device monitors and reports SNR of BSS
   START_SCAN
    Start a long or short channel scan
   SYNCHRONIZE
    Force a synchronization point between command and data
    paths
   TARGET_REPORT_ERROR_BITMASK
    Control “ERROR_REPORT” events from the AR6000




Name
    ADD_BAD_AP

Synopsis
    The host uses this command to cause the AR6000 to avoid a particular AP. The
    AR6000 maintain a table with up to two APs to avoid. An ADD_BAD_AP command
    adds or replaces the specified entry in this bad AP table.

    If the AR6000 are currently connected to the AP specified in this command, they
    disassociate.

Command
    wmiconfig eth1 --badap <bssid> <badApIndex>

Command Parameters
    UINT8 badApIndex    Index [0...1] that identifies which entry in the
                        bad AP table to use


    UINT8 bssid[6]  MAC address of the AP to avoid

Command Values
    badApIndex = 0, 1    Entry in the bad AP table to use

Reset Value
    The bad AP table is cleared

Restrictions
    None

See Also
    “DELETE_BAD_AP” on page B-13

=====================================================================
Name
    ADD_CIPHER_KEY

Synopsis
    The host uses this command to add/replace any of four encryption keys on the
    AR6000. The ADD_CIPHER_KEY command is issued after the CONNECT event
    has been received by the host for all dot11Auth modes except for SHARED_AUTH.
    When the dot11AuthMode is SHARED_AUTH, then the ADD_CIPHER_KEY
    command should be issued before the “CONNECT” command.

Command
    wmiconfig eth1 --cipherkey <keyIndex> <keyType> <keyUsage>
                            <keyLength> <keyopctrl> <keyRSC> <key>

Command Parameters
    UINT8 keyIndex      Index (0...3) of the key to add/replace;
                        uniquely identifies the key
    UINT8 keyType    CRYPTO_TYPE
    UINT8 keyUsage   Specifies usage parameters of the key when
                     keyType = WEP_CRYPT
    UINT8 keyLength  Length of the key in bytes
    UINT8 keyOpCtrl  bit[0] = Initialize TSC (default),
                     bit[1] = Initialize RSC
    UINT8 keyRSC[8]  Key replay sequence counter (RSC) initial
                     value the device should use
    UINT8 key[32]    Key material used for this connection
    Command Values
    {
        NONE_CRYPT = 1
        WEP_CRYPT  = 2
        TKIP_CRYPT = 3
        AES_CRYPT  = 4
        KEY_OP_INIT_TSC   0x01
        KEY_OP_INIT_RSC   0x02
        KEY_OP_INIT_VAL   0x03
                Default is to Initialize the TSC
        KEY_OP_VALID_MASK 0x04
                Two operations defined
    } CRYPTO_TYPE

    {
        PAIRWISE_USAGE  = 0  Set if the key is used for unicast traffic only
        GROUP_USAGE     = 1  Set if the key is used to receive multicast
                              traffic (also set for static WEP keys)
        TX_USAGE        = 2  Set for the GROUP key used to transmit frames
                All others are reserved
    } KEY_USAGE

Reset Value
    The four available keys are disabled.

Restrictions
    The cipher should correspond to the encryption mode specified in the “CONNECT”
    command.

See Also
    “DELETE_CIPHER_KEY”

=====================================================================


Name
    ADD_WOW_PATTERN

Synopsis
    The host uses this command to add a pattern to the WoW pattern list; used for
    pattern-matching for host wakeups by the WoW module. If the host mode is asleep
    and WoW is enabled, all packets are matched against the existing WoW patterns. If a
    packet matches any of the patterns specified, the target will wake up the host. All
    non-matching packets are discarded by the target without being sent up to the host.

Command
    wmiconfig –addwowpattern <list-id> <filter-size> <filter-offset>
    <pattern> <mask>

Command Parameters
    A_UINT8  filter_list_id    ID of the list that is to include the new pattern
    A_UINT8  filter_size       Size of the new pattern
    A_UINT8  filter_offset     Offset at which the pattern matching for this
                                new pattern should begin at
    A_UINT8  filter[1]         Byte stream that contains both the pattern and
                                the mask of the new WoW wake-up pattern

Reply Parameters
    None

Reset Value
    None defined (default host mode is awake)

Restrictions
    None

See Also
    “DELETE_WOW_PATTERN”

=====================================================================


Name
    CLR_RSSI_SNR

Synopsis
    Clears the current calculated RSSI and SNR value. RSSI and SNR are reported by
    running-average value. This command will clear the history and have a fresh start
    for the running-average mechanism.

Command
    wmiconfig eth1 --cleanRssiSnr

Command Parameters
    None

Reply Parameters
    None

Reset Value
    None defined

Restrictions
    None

=====================================================================

Name
    CONNECT_CMD

Synopsis
    New connect control information (connectCtrl) is added, with 32 possible modifiers.

    CONNECT_SEND_REASSOC
        Valid only for a host-controlled connection to a
        particular AP. If this bit is set, a reassociation frame is
        sent. If this bit is clear, an association request frame is
        sent to the AP.

    CONNECT_IGNORE_WPAx_GROUP_CIPHER
        No group key is issued in the CONNECT command,
        so use the group key advertised by the AP. In a target-
        initiated roaming situation this allows a STA to roam
        between APs that support different multicast ciphers.

    CONNECT_PROFILE_MATCH_DONE
        In a host-controlled connection case, it is possible that
        during connect, firmware may not have the
        information for a profile match (e.g, when the AP
        supports hidden SSIDs and the device may not
        transmit probe requests during connect). By setting
        this bit in the connection control information, the
        firmware waits for a beacon from the AP with the
        BSSID supplied in the CONNECT command. No
        additional profile checks are done.

    CONNECT_IGNORE_AAC_BEACON
        Ignore the Admission Capacity information in the
        beacon of the AP

    CONNECT_ASSOC_POLICY_USER
        When set, the CONNECT_SEND_REASSOC setting
        determines if an Assoc or Reassoc is sent to an AP

Command
    wmiconfig --setconnectctrl <ctrl flags bitmask>

Command Parameters
    typedef struct{
    A_UINT8 networktype;
    A_UINT8 dot11authmode;
    A_UINT8 authmode;
    A_UINT8 pairwiseCryptoType; /*CRYPTO_TYPE*/
    A_UINT8 pairwiseCryptoLen;
    A_UINT8 groupCryptoType; /*CRYPTO_TYPE*/
    A_UINT8 groupCryptoLen;
    A_UINT8 ssidLength;
    A_UCHAR ssid[WMI_MAX_SSID_LEN];
    A_UINT16 channel;
    A_UINT8 bssid[AUTH_MAC_LEN];
    A_UINT8 ctrl_flags; /*WMI_CONNECT_CTRL_FLAGS_BITS*/
    } WMI_CONNECT_CMD;

    ctrl flags bitmask
        = 0x0001 CONNECT_ASSOC_POLICY_USER
            Assoc frames are sent using the policy specified by
            the flag
        = 0x0002 CONNECT_SEND_REASSOC
            Send Reassoc frame while connecting, otherwise send
            assoc frames
        = 0x0004 CONNECT_IGNORE_WPAx_GROUP_CIPHER
            Ignore WPAx group cipher for WPA/WPA2
        = 0x0008 CONNECT_PROFILE_MATCH_DONE
            Ignore any profile check
        = 0x0010 CONNECT_IGNORE_AAC_BEACON
            Ignore the admission control information in the
            beacon
        ... CONNECT_CMD, continued
        Command Values
        typedef enum {
            INFRA_NETWORK       = 0x01,
            ADHOC_NETWORK       = 0x02,
            ADHOC_CREATOR       = 0x04,
        } NETWORK_TYPE;

        typedef enum {
            OPEN_AUTH           = 0x01,
            SHARED_AUTH         = 0x02,
            LEAP_AUTH           = 0x04,
        } DOT11_AUTH_MODE;
        typedef enum {
            NONE_AUTH           = 0x01,
            WPA_AUTH            = 0x02,
            WPA_PSK_AUTH        = 0x03,
            WPA2_AUTH           = 0x04,
            WPA2_PSK_AUTH       = 0x05,
            WPA_AUTH_CCKM       = 0x06,
            WPA2_AUTH_CCKM      = 0x07,
        } AUTH_MODE;
        typedef enum {
            NONE_CRYPT          = 0x01,
            WEP_CRYPT           = 0x02,
            TKIP_CRYPT          = 0x03,
            AES_CRYPT           = 0x04,
        } CRYPTO_TYPE;
        typedef enum {
            CONNECT_ASSOC_POLICY_USER = 0x0001,
            CONNECT_SEND_REASSOC = 0x0002,
            CONNECT_IGNORE_WPAx_GROUP_CIPHER = 0x0004,
            CONNECT_PROFILE_MATCH_DONE = 0x0008,
            CONNECT_IGNORE_AAC_BEACON = 0x0010,
        } WMI_CONNECT_CTRL_FLAGS_BITS;

    pairwiseCryptoLen and groupCryptoLen are valid when the respective
    CryptoTypesis WEP_CRYPT, otherwise this value should be 0. This is the length in
    bytes.

Reset Value
    None defined

Restrictions
    None

=====================================================================


Name
    CREATE_PSTREAM

Synopsis
    The host uses this command to create a new prioritized data endpoint between the
    host and the AR6000 device that carries a prioritized stream of data. If the AP that the
    device connects to requires TSPEC stream establishment, the device requests the
    corresponding TSPEC with the AP. The maximum and minimum service interval
    ranges from 0 – 0x7FFFFFFF (ms), where 0 = disabled. The device does not send a
    reply event for this command, as it is always assumed the command has succeeded.
    An AP admission control response comes to the host via a WMI_CAC_INDICATION
    event, once the response for the ADDTS frame comes.

    Examples of cases where reassociation is generated (when WMM) and cases where
    ADDTS is generated (when WMM and enabling ACM) are when:
        Changing UAPSD flags in WMM mode, reassociation is generated
        Changing the interval of sending auto QoS Null frame in WMM mode;
            reassociation is not generated
        Issuing a command with same previous parameters in WMM mode and enabling
            ACM, an ADDTS request is generated
        Changing the interval of a QoS null frame sending in WMM mode and enabling
            ACM, an ADDTS request is generated
        Issuing the command in disconnected state, reassociation or ADDTS is not
            generated but the parameters are available after (re)association

Command
    --createqos <user priority> <direction> <traffic class>
<trafficType> <voice PS capability> <min service interval> <max
service interval> <inactivity interval> <suspension interval>
<service start time> <tsid> <nominal MSDU> <max MSDU> <min data
rate> <mean data rate> <peak data rate> <max burst size> <delay
bound> <min phy rate> <sba> <medium time> where:

    <user priority>
    802.1D user priority range (0–7)
    <direction>
        = 0    Tx (uplink) traffic
        = 1    Rx (downlink) traffic
        = 2    Bi-directional traffic
    <traffic class>
        = 1    BK
        = 2    VI
        = 3    VO
    <trafficType>
        = 0    Aperiodic
        = 1    Periodic
    <voice PS capability>
        Specifies whether the voice power save mechanism
    (APSD if AP supports it or legacy/simulated APSD
    [using PS-Poll]) should be used
        = 0    Disable voice power save for traffic class
        = 1    Enable APSD voice power save for traffic class
        = 2    Enable voice power save for all traffic classes
    <min service interval>
        (In ms)
    <max service interval>
        Inactivity interval (in ms) (0 = Infinite)
    <suspension interval>
        (In ms)
    <service start time>
        Service start time
    <tsid>
        TSID range (0–15)
    <nominal MSDU>
        Nominal MAC SDU size
    <max MSDU>
        Maximum MAC SDU size
    <min data rate>
        Minimum data rate (in bps)
    <mean data rate>
        Mean data rate (in bps)
    <peak data rate>
        Peak data rate (in bps)
    <max burst size>
        Maximum burst size (in bps)
    <delay bound>
        Delay bound
    <min phy rate>
        Minimum PHY rate (in bps)
    <sba>
        Surplus bandwidth allowance
    <medium time>
        Medium time in TU of 32-ms periods per sec
    ... CREATE_PSTREAM (continued)

Command Parameters
    UINT8 trafficClass    TRAFFIC_CLASS value
    UINT8 traffic
    Direction
    DIR_TYPE value
    UINT8    rxQueueNum
        AR6000 device mailbox index (2 or 3)
        corresponding to the endpoint the host
        wishes to use to receive packets for the
        prioritized stream
    UINT8 trafficType  TRAFFIC_TYPE value
    UINT8 voicePS
Capability
    VOICEPS_CAP_TYPE value
    UINT8 tsid          Traffic stream ID
    UINT8 userPriority  802.1D user priority
    UINT16 nominalMSDU  Nominal MSDU in octets
    UINT16 maxMSDU      Maximum MSDU in octets
    UINT32 minServiceInt Minimum service interval: the min.
                            period of traffic specified (in ms)
    UINT32 maxServiceInt  Maximum service interval: the max.
                        period of traffic specified (in ms)
    UINT32 inactivityInt Indicates how many ms an established
                        stream is inactive before the prioritized
                        data endpoint is taken down and the
                        corresponding T-SPEC deleted
    UINT32 suspensionInt  Suspension interval (in ms)
    UINT32 service       StartTime Service start time
    UINT32 minDataRate  Minimum data rate (in bps)
    UINT32 meanDataRate Mean data rate (in bps)
    UINT32 peakDataRate Peak data rate (in bps)
    UINT32 maxBurstSize
    UINT32 delayBound
    UINT32 minPhyRate   Minimum PHY rate for TSPEC (in bps)
    UINT32 sba          Surplus bandwidth allowance
    UINT32 mediumTime   Medium TSPEC time (in units of 32 ms)
Command Values
    {
        WMM_AC_BE = 0   Best Effort
        WMM_AC_BK = 1   Background
        WMM_AC_VI = 2   Video
        WMM_AC_VO = 3   Voice
        All other values reserved
    } TRAFFIC_CLASS
    {
        UPLINK_TRAFFIC   = 0  From the AR6000 device to the AP
        DOWNLINK_TRAFFIC = 1  From the AP to the AR6000 device
        BIDIR_TRAFFIC    = 2  Bi-directional traffic
        All other values reserved
    } DIR_TYPE
    {
        DISABLE_FOR_THIS_AC = 0
        ENABLE_FOR_THIS_AC  = 1
        ENABLE_FOR_ALL_AC   = 2
        All other values reserved
    } VOICEPS_CAP_TYPE

    ... CREATE_PSTREAM (continued)


        VI  BE   BK    Supported, Y/N?
   0    0    0    0    Y
   0    0    0    1    Y
   0    0    1    0    N
   0    0    1    1    N
   0    1    0    0    Y
   0    1    0    1    Y
   0    1    1    0    N
   0    1    1    1    N
   1    0    0    0    Y
   1    0    0    1    Y
   1    0    1    0    N
   1    1    0    0    N
   1    1    0    1    Y
   1    1    0    0    N
   1    1    1    0    N
   1    1    1    1    Y

Reset Value
    No pstream is present after reset; each of the BE, BK, VI,VO pstreams must be created
    (either implicitly by data flow or explicitly by user)

Restrictions
    This command can only be issued when the device is in the CONNECTED state. If
    the device receives the command while in DISCONNECTED state, it replies with a
    failure indication. At most four prioritized data endpoints can be created, one for
    each AC.

See Also
    “DELETE_PSTREAM”
=====================================================================

Name
    DELETE_BAD_AP

Synopsis
    The host uses this command to clear a particular entry in the bad AP table

Command
    wmiconfig eth1 --rmAP [--num=<index>] // used to clear a badAP
    entry. num is index from 0-3

Command Parameters
    UINT8  badApIndex   Index [0...n] that identifies the entry in the bad
                        AP table to delete

Command Values
    badApIndex   = 0, 1, 2, 3
            Entry in the bad AP table

Reset Value
    None defined

Restrictions
    None

See Also
    “ADD_BAD_AP”

=====================================================================


Name
    DELETE_CIPHER_KEY

Synopsis
    The host uses this command to delete a key that was previously added with the
    “ADD_CIPHER_KEY” command.

Command
    TBD

Command Parameters
    UINT8   keyIndex    Index (0...3) of the key to be deleted

Command Values
    keyIndex    = 0, 1,2, 3    Key to delete

Reset Value
    None

Restrictions
    The host should not delete a key that is currently in use by the AR6000.

See Also
    “ADD_CIPHER_KEY”

=====================================================================

Name
    DELETE_PSTREAM

Synopsis
    The host uses this command to delete a prioritized data endpoint created by a
    previous “CREATE_PSTREAM” command

Command
    --deleteqos <trafficClass> <tsid>, where:

    <traffic class>
        = 0    BE
        = 1    BK
        = 2    VI
        = 3    VO
    <tsid>
        The TSpec ID; use the -qosqueue option
        to get the active TSpec IDs for each traffic class

Command Parameters
    A_UINT8    trafficClass    Indicate the traffic class of the stream
                            being deleted

Command Values
    {
        WMM_AC_BE = 0    Best effort
        WMM_AC_BK = 1    Background
        WMM_AC_VI = 2    Video
        WMM_AC_VO = 3    Voice
    } TRAFFIC CLASS

    0-15 for TSID

Reply Values
    N/A

Restrictions
    This command should only be issued after a “CREATE_PSTREAM” command has
    successfully created a prioritized stream

See Also
    “CREATE_PSTREAM”

=====================================================================


Name
    DELETE_WOW_PATTERN

Synopsis
    The host uses this command to remove a pre-specified pattern from the
    WoW pattern list.

Command
    wmiconfig –delwowpattern <list-id> <pattern-id>

Command Parameters
    A_UINT8    filter_list_id    ID of the list that contains the WoW filter
                                 pattern to delete
    A_UINT8    filter_id    ID of the WoW filter pattern to delete

Reply Parameters
    None



Reset Value
    None defined

Restrictions
    None

See Also
    “ADD_WOW_PATTERN”

=====================================================================


Name
    EXTENSION

Synopsis
    The WMI message interface is used mostly for wireless control messages to a wireless
    module applicable to wireless module management regardless of the target platform
    implementation. However, some commands only peripherally related to wireless
    management are desired during operation. These wireless extension commands may
    be platform-specific or implementation-dependent.

Command
    N/A

Command Parameters
    Command-specific

Command Values
    Command-specific

Reply Parameters
    Command-specific

Reset Values
    None defined

Restrictions
    None defined

=====================================================================


Name
    GET_BIT_RATE

Synopsis
    Used by the host to obtain the rate most recently used by the AR6000 device

Command
    wmiconfig eth1 --getfixrates

Command Parameters
    None



Reply Parameters
    INT8
    rateIndex
    See the “SET_BIT_RATE” command

Reset Values
    None

Restrictions
    This command should only be used during development/debug; it is not intended
for use in production. It is only valid when the device is in the CONNECTED state

See Also
    “SET_BIT_RATE”

=====================================================================


Name
    GET_CHANNEL_LIST

Synopsis
    Used by the host uses to retrieve the list of channels that can be used by the device
    while in the current wireless mode and in the current regulatory domain.

Command
    TBD

Command Parameters
    None

Reply Parameters
    UINT8    reserved    Reserved
    UINT8    numberOfChannels    Number of channels the reply contains
    UINT16    channelList[numberOfChannels]  Array of channel frequencies (in MHz)

Reset Values
    None defined

Restrictions
    The maximum number of channels that can be reported are 32

=====================================================================


Name
    GET_FIXRATES

Synopsis
    Clears the current calculated RSSI and SNR value. RSSI and SNR are reported by
    running-average value. This command will clear the history and have a fresh start for
    the running-average mechanism.

Synopsis
    This returns rate-mask set via WMI_SET_FIXRATES to retrieve the current fixed rate
    that the AR6001 or AR6001 is using. See “SET_FIXRATES”.

Command
    wmiconfig eth1 --getfixrates

Command Parameters
    A_UINT16    fixRateMask;    Note: if this command is used prior to
                using WMI_SET_FIXRATES, AR6000
                returns 0xffff as fixRateMask, indicating
                all the rates are enabled

Reply Parameters
    None

Reset Value
    None defined

Restrictions
    None

See Also
    “SET_FIXRATES”

=====================================================================



Name
    GET_PMKID_LIST_CMD

Synopsis
    Retrieves the list of PMKIDs on the firmware. The
    WMI_GET_PMKID_LIST_EVENT is generated by the firmware.

Command
    TBD

Command Parameters

Reset Values
    None

Restrictions
    None

See Also
    SET_PMKID_LIST_CMD GET_PMKID_LIST_EVENT

=====================================================================


Name
    GET_ROAM_TBL

Synopsis
    Retrieve the roaming table maintained on the target. The response is reported
    asynchronously through the ROAM_TBL_EVENT.

Command
    wmiconfig --getroamtable <roamctrl> <info>

Command Parameters
    A_UINT8    roamCtrlType;
    A_UINT16   roamMode
    A_UINT16   numEntries
    WMI_BSS_ROAM_INFO bssRoamInfo[1]

Reply Value
    Reported asynchronously through the ROAM_TBL_EVENT

Reset Value
    None defined

Restrictions
    None

See Also
    SET_KEEPALIVE

=====================================================================


Name
    GET_TARGET_STATS

Synopsis
    The host uses this command to request that the target send the statistics that it
    maintains. The statistics obtained from the target are accrued in the host every time
    the GET_TARGET_STATS command is issued. The --clearStats option is added to
    clear the target statistics maintained in the host.

Command
    wmiconfig --getTargetStats --clearStats

Command Parameters
    TARGET_STATS    targetStats
    WMI_TARGET_STATS
    UINT8   clearStats


Reply Value
    RSSI return value (0–100)

Reset Values
    All statistics are cleared (zeroed)

Restrictions
    The --getTargetStats option must be used; the --clearStats option is also available also


=====================================================================

Name
    GET_TX_PWR

Synopsis
    The host uses this command to retrieve the current Tx power level

Command
    wmiconfig -i eth1 --getpower

Command Parameters
    None

Reply Parameters
    UINT16 dbM    The current Tx power level specified in dbM

Reset Values
    The maximum permitted by the regulatory domain

Restrictions
    None

See Also
    “SET_TX_PWR”

=====================================================================


Name
    GET_WOW_LIST

Synopsis
    The host uses this command to retrieve the current list of WoW patterns.

Command
    wmiconfig –getwowlist <list-id>

Command Parameters
    A_UINT8 filter_list_id    ID of the list of WoW patterns to retrieve

Reply Value(s)
    A_UINT16  num_filters    Number of WoW patterns contained in the list
    A_UINT8   wow_mode    Current mode of WoW (enabled or disabled)
    A_UINT8    host_mode    Current host mode (asleep or awake)
    WOW_FILTER    wow_filters[1]
        Contents of the WoW filter pattern list
        (contains mask, pattern, offset and size
    information for each of the patterns)

Reset Value
    None defined

Restrictions
    None

See Also
    “SET_WSC_STATUS”

=====================================================================


Name
    LQ_THRESHOLD_PARAMS

Synopsis
    Sets Link Quality thresholds, the sampling will happen at every unicast data frame
    Tx if a certain threshold is met, and the corresponding event will be sent to the host.

Command
    --lqThreshold <enable> <upper_threshold_1> ...
    <upper_threshold_4> <lower_threshold_1> ... <lower_threshold_4>

Command Parameters
    <enable>    = 0    Disable link quality sampling
                = 1    Enable link quality sampling
    <upper_threshold_x>  Above thresholds (value in [0,100]), in
                    ascending order
    <lower_threshold_x> Below thresholds (value in [0,100]), in
                    ascending order

Command Values
    See command parameters

Reset Value
    None defined

Restrictions
    None

=====================================================================


Name
    OPT_TX_FRAME

Synopsis
    Special feature, sends a special frame.

Command
    wmiconfig --sendframe <frmType> <dstaddr> <bssid> <optIEDatalen>
    <optIEData>

Command Parameters
    {
        A_UINT16    optIEDataLen;
        A_UINT8    frmType;
        A_UINT8    dstAddr[ATH_MAC_LEN];
        A_UINT8    bssid[ATH_MAC_LEN];
        A_UINT8    optIEData[1];
    } WMI_OPT_TX_FRAME_CMD;

Command Values
    <frmtype>   = 1    Probe request frame
                = 2    Probe response frame
                = 3    CPPP start
                = 4    CPPP stop

Reset Value
    None defined

Restrictions
    Send a special frame only when special mode is on.

=====================================================================


Name
    RECONNECT

Synopsis
    This command requests a reconnection to a BSS to which the AR6000 device was
    formerly connected

Command
    TBD

Command Parameters
    UINT16    channel    Provides a hint as to which channel was
                        used for a previous connection
    UINT8    bssid[6]    If set, indicates which BSSID to connect to

Command Values
    None

Reset Values
    None

Restrictions
    None

See Also
    “CONNECT_CMD”

=====================================================================


Name
    RSSI_THRESHOLD_PARAMS

Synopsis
    Configures how the AR6000 device monitors and reports signal strength (RSSI) of the
    connected BSS, which is used as a link quality metric. The four RSSI threshold sets (in
    dbM) of the host specification divide the signal strength range into six segments.
    When signal strength increases or decreases across one of the boundaries, an
    RSSI_THRESHOLD event is signaled to the host. The host may then choose to take
    action (such as influencing roaming).

Command
    wmiconfig eth1 --rssiThreshold <weight> <pollTime>
        <above_threshold_val_1> ... <above_threshold_tag_6>
        <above_threshold_val_6>
        <below_threshold_tag_1> <below_threshold_val_1> ...
        <below_threshold_tag_6> <below_threshold_val_6>

Command Parameters
    UINT8    weight    Range in [1, 16] used to calculate average RSSI
    UINT32   pollTime   RSSI (signal strength) sampling frequency in
                seconds (if pollTime = 0, single strength
        sampling is disabled)
    USER_RSS__THOLD tholds[12]  Thresholds (6 x 2)

Command Values
    None defined

Reset Values
    pollTime is 0, and sampling is disabled

Restrictions
    Can only be issued if the AR6000 device is connected


=====================================================================

Name
    SCAN_PARAMS

Synopsis
    The minact parameter determines the minimum active channel dwell time, within
    which if the STA receives any beacon, it remains on that channel until the maxact
    channel dwell time. If the STA does not receive a beacon within the minact dwell
    time, it switches to scan the next channel.

Command
    wmiconfig -scan -minact=<ms> --maxact=<ms>

Command Parameters
    UINT16    maxact    Channel dwell time (in ms), default = 0
    UINT16    minact    Channel dwell time (in ms), default = 105

Command Values
    See channel parameters

Reset Values
    None defined

Restrictions
    The minact value should be greater than 0; maxact should be between 5–65535 ms
    and greater than minact

=====================================================================


Name
    SET_ACCESS_PARAMS

Synopsis
    Allows the host to set access parameters for the wireless network. A thorough
    understanding of IEEE 802.11 is required to properly manipulate these parameters.

Command
    wmiconfig eth1 --acparams --txop <limit> --cwmin <0-15>
    --cwmax <0-15> --aifsn<0-15>

Command Parameters
    UINT16    txop    The maximum time (expressed in units of
                        32 ms) the device can spend transmitting
                        after acquiring the right to transmit
    UINT8    eCWmin    Minimum contention window
    UINT8    eCWmax    Maximum contention window
    UINT8    aifsn    The arbitration inter-frame space number

Command Values
    None

Reset Values
    Reasonable defaults that vary, between endpoints (prioritized streams)

Restrictions
    None

=====================================================================


Name
    SET_ADHOC_BSSID

Synopsis
    Allows the host to set the BSSID for an ad hoc network. If a network with this BSSID
    is not found, the target creates an ad hoc network with this BSSID after the connect
    WMI command is triggered (e.g., by the SIOCSIWESSID IOCTL).

Command
    wmiconfig eth1 --adhocbssid <bssid>

Command Parameters
    A_UINT8     bssid[ATH_MAC_LEN]    BSSID is specified in xx:xx:xx:xx:xx:xx format

Command Values
    None

Reset Values
    None

Restrictions
    None

=====================================================================


Name
    SET_AKMP_PARAMS

Synopsis
    Enables or disables multi PMKID mode.

Command
    wmiconfig eth1 --setakmp --multipmkid=<on/off>

Command Parameters
    typedef struct {
      A_UINT32    akmpInfo;
    } WMI_SET_AKMP_PARAMS_CMD;

Command Values
    akmpInfo;
    bit[0] = 0
        MultiPMKID mode is disabled and PMKIDs that
        were set using the WMI_SET_PMKID_CMD are
        used in the [Re]AssocRequest frame.
     bit[0] = 1
        MultiPMKID mode is enabled and PMKIDs issued
        by the WMI_SET_PMKID_LIST_CMD are used in
        the next [Re]AssocRequest sent to the AP.

Reset Values
    MultiPMKID mode is disabled

Restrictions
    None

=====================================================================


Name
    SET_APPIE

Synopsis
    Add an application-specified IE to a management frame. The maximum length is
    76 bytes. Including the length and the element ID, this translates to 78 bytes.

Command
    wmiconfig --setappie <frame> <IE>, where:

    frame
        One of beacon, probe, respon, assoc

    IE
        A hex string beginning with DD (if = 0, no
        IE is sent in the management frame)

Command Parameters
    mgmtFrmType;
        A WMI_MGMT_FRAME_TYPE

    ieLen;
        Length of the IE to add to the GMT frame

Command Values
    None

Reset Value
    None defined

Restrictions
    Supported only for the probe request and association request management frame
types. Also, only one IE can be added per management frame type.

=====================================================================


Name
    SET_ASSOC_INFO

Synopsis
    The host uses this command to specify any information elements (IEs) it wishes the
    AR6000 device to add to all future association and reassociation requests. IEs must be
    correct and are used as is by the device. IEs specified through this command are
    cleared with a DISCONNECT.

Command
    wmiconfig eth1 --setAssocIe <IE>

Command Parameters
    UINT8    ieType    Used directly in 802.11 frames
    UINT8    bufferSize    Size of assocInfo (in bytes) ranging from
                        0–240. If = 0, previously set IEs are cleared.
    UINT8    assocInfo[bufferSize]    Used directly in 802.11 frames

Command Values
    None

Reset Values
    IEs are cleared

Restrictions
    This command can only be issued in the DISCONNECTED state

=====================================================================


Name
    SET_AUTHMODE

Synopsis
    Sets the 802.11 authentication mode of reconnection

Command
    wmiconfig eth1 --setauthmode <mode>

Command Parameters
    UINT8    mode

Command Values
    mode    = 0x00    Proceed with authentication during reconnect
            = 0x01    Do not proceed with authentication during reconnect

Reset Values
    Authentication

Restrictions
    None

=====================================================================


Name
    SET_BEACON_INT

Synopsis
    Sets the beacon interval for an ad hoc network. Beacon interval selection may have an
    impact on power savings. To some degree, a longer interval reduces power
    consumption but also decreases throughput. A thorough understanding of IEEE
    802.11 ad hoc networks is required to use this command effectively.

Command
    wmiconfig eth1 --ibssconintv

Command Parameters
    UINT16    beaconInterval    Specifies the beacon interval in TU units (1024 ms)

Command Values
    None

Reset Values
    The default beacon interval is 100 TUs (102.4 ms)

Restrictions
    This command can only be issued before the AR6000 device starts an ad hoc network

See Also
    “SET_IBSS_PM_CAPS”

=====================================================================


Name
    SET_BIT_RATE

Synopsis
    The host uses this command to set the AR6000 device to a specific fixed rate.

Command
    wmiconfig eth1 --setfixrates <rate_0> ... <rate_n>

Command Parameters
    INT8    rateIndex
    A WMI_BIT_RATE value
    {
        RATE_AUTO    = -1
        RATE_1Mb     = 0
        RATE_2Mb     = 1
        RATE_5_5M    = 2
        RATE_11Mb    = 3
        RATE_6Mb     = 4
        RATE_9Mb     = 5
        RATE_12Mb    = 6
        RATE_18Mb    = 7
        RATE_24Mb    = 8
        RATE_36Mb    = 9
        RATE_48Mb    = 10
        RATE_54Mb    = 11
      } WMI_BIT_RATE


Command Values
    See command parameters

Reset Values
    The dynamic rate is determined by the AR6000 device

Restrictions
    This command is intended for use only during development/debug; it is not
intended for use in production

See Also
    “GET_BIT_RATE”

=====================================================================


Name
    SET_BMISS_TIME

Synopsis
    This command sets the beacon miss (BMISS) time, which the AR6000 hardware use
    to recognize missed beacons. When an excessive number (15) of consecutive beacons
    are missed, the AR6000 consider switching to a different BSS. The time can be
    specified in number of beacons or in TUs.

Command(s)
    wmiconfig eth1 --setbmissbeacons=<val>
    wmiconfig eth1 --setbmisstime=<val>

Command Parameters
    UINT16    bmissTime    Specifies the beacon miss time
                            [1000...5000] in TUs (1024 ms)
    UINT16    bmissbeacons  Specifies the number of beacons [5...50]

Command Values
    None

Reset Values
    bmissTime is 1500 TUs (1536 ms)

Restrictions
    None

=====================================================================


Name
    SET_BSS_FILTER

Synopsis
    The host uses this to inform the AR6000 device of the types of networks about which
    it wants to receive information from the “BSSINFO” event. As the device performs
    either foreground or background scans, it applies the filter and sends “BSSINFO”
    events only for the networks that pass the filter. If any of the  bssFilter or the ieMask
    filter matches, a BSS Info is sent to the host. The ieMask currently is used as a match
    for the IEs in the beacons, probe reponses and channel switch action management
    frame. See also “Scan and Roam” on page C-1.

    The BSS filter command has been enhanced to support IE based filtering. The IEs can
    be specified as a bitmask through this command using this enum.

Command
    wmiconfig eth1 –filter = <filter> --ieMask 0x<mask>

Command Parameters
    UINT8    BssFilter

 Command Values
    typedef struct {
        A_UINT8    bssFilter;    See WMI_BSS_FILTER
        A_UINT32    ieMask;
    } __ATTRIB_PACK WMI_BSS_FILTER_CMD;

    The ieMask can take this combination of values:

    enum {
        BSS_ELEMID_CHANSWITCH   = 0x01
        BSS_ELEMID_ATHEROS      = 0x02,
    }

Reply Value
    None

Reset Value
    BssFilter = NONE_BSS_FILTER (0)

Restrictions
    None

See Also
    “CONNECT_CMD”

=====================================================================


Name
    SET_BT_PARAMS

Synopsis
    This command is used to set the status of a Bluetooth stream or set Bluetooth
    coexistence register parameters. The stream may be an SCO or an A2DP stream and
    its status can be started/stopped/suspended/resumed.

Command
    wmiconfig –setBTparams <paramType> <params>

Command Parameters
    struct {
       union {
            BT_PARAMS_SCO    scoParams;
            BT_PARAMS_A2DP   a2dpParams;
            BT_PARAMS_MISC   miscParams;
            BT_COEX_REGS     regs;
        } info;
         A_UINT8 paramType;
        struct {
            A_UINT8 noSCOPkts;    Number of SCO packets between consecutive PS-POLLs
            A_UINT8 pspollTimeout;
            A_UINT8 stompbt;
        } BT_PARAMS_SCO;
        struct {
            A2DP BT stream parameters
            A_UINT32   period;
            A_UINT32   dutycycle;
            A_UINT8    stompbt;
        } BT_PARAMS_A2DP;
        struct {
            union {
               WLAN_PROTECT_POLICY_TYPE   protectParams;
               A_UINT16    wlanCtrlFlags;
            }info;
        A_UINT8 paramType;
        } BT_PARAMS_MISC;
        struct {
            BT coexistence registers values
            A_UINT32    mode;     Coexistence mode
            A_UINT32    scoWghts; WLAN and BT weights
            A_UINT32    a2dpWghts;
            A_UINT32    genWghts;
            A_UINT32    mode2;    Coexistence mode2
            A_UINT8    setVal;
        } BT_COEX_REGS;

Command Values
    None defined

Reset Value
    None

Restrictions
    None

=====================================================================


Name
    SET_BT_STATUS

Synopsis
    Sets the status of a Bluetooth stream. The stream may be a SCO or an A2DP stream
    and its status can be started/stopped/suspended/resumed.

Command
    wmiconfig –setBTstatus <streamType> <status>

Command Parameters
    {
        A_UINT8        streamType;    Stream type
        A_UINT8        status;    Stream status
    }WMI_SET_BT_STATUS_CMD;

Command Values
    {
        BT_STREAM_UNDEF    = 0
        BT_STREAM_SCO
        SCO stream
        BT_STREAM_A2DP
        A2DP stream
        BT_STREAM_MAX
    } BT_STREAM_TYPE;

    {
        BT_STATUS_UNDEF    = 0
        BT_STATUS_START
        BT_STATUS_STOP
        BT_STATUS_RESUME
        BT_STATUS_SUSPEND
        BT_STATUS_MAX
    } BT_STREAM_STATUS;

Reset Value
    None defined

Restrictions
    None

=====================================================================


Name
    SET_CHANNEL_PARAMETERS

Synopsis
    Configures various WLAN parameters related to channels, sets the wireless mode,
    and can restrict the AR6000 device to a subset of available channels. The list of
    available channels varies depending on the wireless mode and the regulatory
    domain. The device never operates on a channel outside of its regulatory domain. The
    device starts to scan the list of channels right after this command.

Command
    wmiconfig eth1 --wmode <mode> <list>

Command Parameters
    UINT8    phyMode    See Values below.
    UINT8    numberOfChannels
        Number of channels in the channel array that
        follows. If = 0, then the device uses all of the
        channels permitted by the regulatory domain
        and by the specified phyMode.
    UINT16    channel[numberOfChannels]
        Array listing the subset of channels (expressed
        as frequencies in MHz) the host wants the
        device to use. Any channel not permitted by
        the specified phyMode or by the specified
        regulatory domain is ignored by the device.

Command Values
    phyMode = {
        Wireless mode
        11a    = 0x01
        11g    = 0x02
        11ag   = 0x03
        11b    = 0x04
        11g only    = 0x05
        }

Reset Values
    phyMode
    11ag
    802.11a/g modules
        11g
    802.11g module
    channels
        Defaults to all channels permitted by the
        current regulatory domain.

Restrictions
    This command, if issued, should be issued soon after reset and prior to the first
    connection. This command should only be issued in the DISCONNECTED state.

=====================================================================


Name
    SET_DISC_TIMEOUT

Synopsis
    The host uses this command to configure the amount of time that the AR6000 should
    spend when it attempts to reestablish a connection after losing link with its current
    BSS. If this time limit is exceeded, the AR6000 send a “DISCONNECT” event. After
    sending the “DISCONNECT” event the AR6000 continues to attempt to reestablish a
    connection, but they do so at the interval corresponding to a foreground scan as
    established by the “SET_SCAN_PARAMS” command.

    A timeout value of 0 indicates that the AR6000 will disable all autonomous roaming,
    so that the AR6000 will not perform any scans after sending a “DISCONNECT”
    event to the host. The state is maintained until a shutdown or host sets different
    timeout value from 0.

Command
    wmiconfig eth1 --disc=<timeout in seconds>

Command Parameters
    UINT8    disconnectTimeout
        Specifies the time limit (in seconds) after
        which a failure to reestablish a connection
        results in a “DISCONNECT” event

Command Values
    None

Reset Values
    disconnectTimeout is 10 seconds

Restrictions
    This command can only be issued while in a DISCONNECTED state

=====================================================================


Name
    SET_FIXRATES

Synopsis
    By default, the AR6000 device uses all PHY rates based on mode of operation. If the
    host application requires the device to use subset of supported rates, it can set those
    rates with this command. In 802.11g mode, the AR6000 device takes the entire
    802.11g basic rate set and the rates specified with this command and uses it as the
    supported rate set.

    This rate set is advertised in the probe request and the assoc/re-assoc request as
    supported rates. Upon successful association, the device modifies the rate set pool
    using the: intersection of AP-supported rates with the union of the 802.11g basic rate
    set and rates set using this command. The device picks transmission rates from this
    pool based on a rate control algorithm.

Command
    TBD

Command Parameters
    A_UINT16    fixRateMask;
        The individual bit is an index for rate table,
        and setting the that index to 1 would set that
        corresponding rate. E.g., fixRateMask = 9
        (1001) sets 1 Mbps and 11 Mbps.

Command Values
    None

Reset Value
    None defined

Restrictions
    None

See Also
    “GET_FIXRATES”

=====================================================================


Name
    SET_WHAL_PARAM

Synopsis
    An internal AR6000 command that is used to set certain hardware parameters. The
    description of this command is in $WORKAREA/include/halapi.h.

Command
    TBD

Command Parameters
    ATH_HAL_SETCABTO_CMDID
        Sets the timeout waiting for the multicast
        traffic after a DTIM beacon (in TUs).

Command Values
    None

Reset Value
    Default = 10 TUs

Restrictions
    This command should be executed before issuing a connect command.

=====================================================================


Name
    SET_HOST_SLEEP_MODE

Synopsis
    The host uses this command to set the host mode to asleep or awake. All packets are
    delivered to the host when the host mode is awake. When host mode is asleep, only if
    WoW is enabled and the incoming packet matches one of the specified WoW
    patterns, will the packet be delivered to the host. The host will also be woken up by
    the target for pattern-matching packets and important events.

Command
    wmiconfig –sethostmode=<asleep/awake>

Command Parameters
    A_BOOL    awake    Set the host mode to awake
    A_BOOL    asleep   Set the host mode to asleep

Command Values
    1 = awake, 0 = asleep

Reset Value
    None defined (default host mode is awake)

Restrictions
    None


=====================================================================

Name
    SET_IBSS_PM_CAPS

Synopsis
    Used to support a non-standard power management scheme for an ad hoc wireless
    network consisting of up to eight stations (STAs) that support this form of power
    saving (e.g., Atheros-based STAs). A thorough understanding of IEEE 802.11 ad hoc
    networks is required to use this command effectively.

Command
    wmiconfig eth1 --ibsspmcaps --ps=<enable/disable>
        --aw=<ATIM Windows in ms>
        --ttl=<Time to live in number of beacon periods>
        --to=<timeout in ms>

Command Parameters
    UINT8    power_saving
        = 0
        The non-standard power saving scheme is
        disabled and maximum throughput (with no
        power saving) is obtained.

        = 1
        Ad hoc power saving scheme is enabled (but
        throughput may be decreased)

    UINT16    atim_windows
        Specifies the length (in ms) of the ad hoc traffic
        indication message (ATIM) windows used in an ad
        hoc network. All Atheros-based STAs that join the
        network use this duration ATIM window.

    The duration is communicated between wireless
    STAs through an IE in beacons and probe responses.

    The host sets atim_windows to control trade-offs
    between power use and throughput. The value
    chosen should be based on the beacon interval (see
    the “SET_BEACON_INT” command) on the
    expected number of STAs in the IBSS, and on the
    amount of traffic and traffic patterns between STAs.

    UINT16    timeout_value
        Specifies the timeout (in ms). The value is the same
        for all ad hoc connections, but tracks separately for
        each.

        Applicable only for a beacon period and used to
        derive actual timeout values on the Tx and Rx sides.
        On the Tx side, the value defines a window during
        which the STA accepts the frame(s) from the host for a
        particular connection. Until closed, the window
        restarts with every frame received from the host. On
        the Rx side, indicates the time until which the STA
        continues accepting frames from a particular
        connection. The value resets with every frame
        received. The value can be used to determine the
        trade off between throughput and power.
        Default = 10 ms

    UINT8    ttl
        Specifies the value in number of beacon periods. The
        value is used to set a limit on the time until which a
        frame is kept alive in the AR6001 before being
        discarded. Default = 5

Command Values
    None

Reset Values
    By default, power_saving is enabled with atim_window = 20 ms

Restrictions
    Can only be issued before the AR6000 starts an ad hoc network

See Also
    “SET_BEACON_INT”

=====================================================================



Name
    SET_LISTEN_INT

Synopsis
    The host uses this command to request a listen interval, which determines how often
    the AR6000 device should wake up and listen for traffic. The listen interval can be set
    by the TUs or by the number of beacons. The device may not be able to comply with
    the request (e.g., if the beacon interval is greater than the requested listen interval, the
    device sets the listen interval to the beacon interval). The actual listen interval used
    by the device is available in the “CONNECT” event.

Command
    wmiconfig eth1 --listen=<#of TUs, can range from 15 to 3000>

    --listenbeacons=<#of beacons, can range from 1 to 50>

Command Parameters
    UINT16    listenInterval
        Specifies the listen interval in Kms
        (1024 ms), ranging from 100 to 1000

    UINT16    listenbeacons
        Specifies the listen interval in beacons,
        ranging from 1 to 50

Command Values
    None

Reset Values
    The device sets the listen interval equal to the beacon interval of the AP it associates
    to.

Restrictions
    None

=====================================================================


Name
    SET_LPREAMBLE

Synopsis
    Overrides the short preamble capability of the AR6000 device

Command
    TBD

Command Parameters
    WMI_LPREAMBLE_DISABLED
        The device is short-preamble capable

    WMI_LPREAMBLE_ENABLED
        The device supports only the long-
    preamble mode

Command Values
    None

Reset Value
    None defined

Restrictions
    None


=====================================================================

Name
    SET_MAX_SP_LEN

Synopsis
    Set the maximum service period; indicates the number of packets the AR6001 can
    receive from the AP when triggered

Command
    wmiconfig eth1 --setMaxSPLength <maxSPLen>

Command Parameters
    UINT8    maxSPLen
        An APSD_SP_LEN_TYPE value

Command Values
    {
        DELIVER_ALL_PKT  = 0x0
        DELIVER_2_PKT    = 0x1
        DELIVER_4_PKT    = 0x2
        DELIVER_6_PKT    = 0x3
    }APSD_SP_LEN_TYPE


Reset Values
    maxSPLen is DELIVER_ALL_PKT

Restrictions
    None

=====================================================================


Name
    SET_OPT_MODE

Synopsis
    Special feature, sets the special mode on/off

Command
    wmiconfig eth1 --mode <mode>
    Set the optional mode, where mode is special or off

Command Parameters
    enum {
        SPECIAL_OFF
        SPECIAL_ON
    } OPT_MODE_TYPE;

Command Values

Reset Value
    Mode = Off

Restrictions
    None

=====================================================================


Name
    SET_PMKID

Synopsis
    The host uses this command to enable or disable a pairwise master key ID (PMKID)
    in the AR6000 PMKID cache. The AR6000 clears its PMKID cache on receipt of a
    DISCONNECT command from the host. Individual entries in the cache might be
    deleted as the AR6000 detect new APs and decides to remove old ones.

Command
    wmiconfig eth1 --setbsspmkid --bssid=<aabbccddeeff>
    --bsspmkid=<pmkid>

Command Parameters
    UINT8    bssid[6]
        The MAC address of the AP that the
        PMKID corresponds to (6 bytes in hex
        format)

    UINT8    enable
        Either PMKID_DISABLE (0) to disable
        the PMKID or PMKID_ENABLE (1) to
        enable it (16 bytes in hex format)

    UINT8    pmkid[16]
        Meaningful only if enable is
        PMKID_ENABLE, when it is the PMKID
        that the AR6000 should use on the next
        reassociation with the specified AP

Command Values
    enable
    = 0 (disable), 1 (enable)
    PKMID enabled/disabled

Reset Values
    None defined

Restrictions
    Only supported in infrastructure networks

=====================================================================


Name
    SET_PMKID_LIST_CMD

Synopsis
    Configures the list of PMKIDs on the firmware.

Command
    wmiconfig --setpmkidlist --numpmkid=<n> --pmkid=<pmkid_1>
    ... --pmkid=<pmkid_n>

    Where n is the number of pmkids (maximum = 8) and pmkid_i is the ith pmkid (16
    bytes in hex format)

Command Parameters
    {
        A_UINT8 pmkid[WMI_PMKID_LEN];
    } __ATTRIB_PACK WMI_PMKID;

    {
        A_UINT32 numPMKID;
        WMI_PMKID pmkidList[WMI_MAX_PMKID_CACHE];
    } __ATTRIB_PACK WMI_SET_PMKID_LIST_CMD;

Command Values
    None

Reset Values
    None

Restrictions
    Supported only in infrastructure modes

=====================================================================


Name
    SET_POWER_MODE

Synopsis
    The host uses this command to provide the AR6000 device with guidelines on the
    desired trade-off between power utilization and performance.

        In normal power mode, the device enters a sleep state if they have nothing to do,
        which conserves power but may cost performance as it can take up to 2 ms to
        resume operation after leaving sleep state.

        In maximum performance mode, the device never enters sleep state, thus no time
        is spent waking up, resulting in higher power consumption and better
        performance.

Command
    TBD

Command Parameters
    UINT8    powerMode
        WMI_POWER_MODE value
    {
        REC_POWER   = 1
            (Recommended setting) Tries to conserve
            power without sacrificing performance
        MAX_PERF_POWER    = 2
            Setting that maximizes performance at
            the expense of power

            All other values are reserved
    } WMI_POWER_MODE

Command Values
    See command parameters

Reset Values
    powerMode is REC_POWER

Restrictions
    This command should only be issued in the DISCONNECTED state for the
    infrastructure network.

            For a PM-disabled ad hoc network, the power mode should remain in
            MAX_PERF_POWER.

            For a PM-enabled ad hoc network, the device can have REC_POWER or
            MAX_PERF_POWER set, but either way it must follow the power save ad hoc
            protocol. The host can change power modes in the CONNECTED state.

    Host changes to the PS setting when the STA is off the home channel take no effect
    and cause a TARGET_PM_FAIL event.

=====================================================================


Name
    SET_POWER_PARAMS

Synopsis
    The host uses this command to configure power parameters

Command
    wmiconfig eth1 --pmparams --it=<ms> --np=<number of PS POLL>
    --dp=<DTIM policy: ignore/normal/stick>

Command Parameters
    UINT16    idle_period
        Length of time (in ms) the AR6000 device
        remains awake after frame Rx/Tx before going
        to SLEEP state

    UINT16    pspoll_number
            The number of PowerSavePoll (PS-poll)
            messages the device should send before
            notifying the AP it is awake

    UINT16    dtim_policy
            A WMI_POWER_PARAMS_CMD value

    {
        IGNORE_DTIM    =1
            The device does not listen to any content after
            beacon (CAB) traffic
        NORMAL_DTIM    = 2
            DTIM period follows the listen interval (e.g., if
            the listen interval is 4 and the DTIM period is 2,
            the device wakes up every fourth beacon)
        STICK_DTIM    = 3
            Device attempt to receive all CAB traffic (e.g., if
            the DTIM period is 2 and the listen interval is 4,
            the device wakes up every second beacon)
    } WMI_POWER_PARAMS_CMD

Command Parameters
    See command parameters

Reset Values
    idle_period
        200 ms

    pspoll_number
    = 1

    dtim_policy
    = NORMAL_DTIM

Restrictions
    None

=====================================================================


Name
    SET_POWERSAVE_PARAMS

Synopsis
    Set the two AR6000 power save timers (PS-POLL timer and APSD trigger timer) and
    the two ASPD TIM policies

Command
    wmiconfig eth1--psparams --psPollTimer=<psPollTimeout in ms>
    --triggerTimer=<triggerTimeout in ms> --apsdTimPolicy=<ignore/
    adhere> --simulatedAPSDTimPolicy=<ignore/adhere>

Command Parameters
    typedef struct {
        A_UINT16    psPollTimeout;
            Timeout (in ms) after sending PS-POLL; the
            AR6000 device sleeps if it does not receive a
            data packet from the AP

        A_UINT16    triggerTimeout;
            Timeout (in ms) after sending a trigger; the
            device sleeps if it does not receive any data
            or null frame from the AP

        APSD_TIM_POLICY    apsdTimPolicy;
            TIM behavior with queue APSD enabled

        APSD_TIM_POLICY    simulatedAPSD

        TimPolicy;
            TIM behavior with simulated APSD
            enabled

    typedef enum {
        IGNORE_TIM_ALL_QUEUES_APSD = 0,
        PROCESS_TIM_ALL_QUEUES_APSD = 1,
        IGNORE_TIM_SIMULATED_APSD = 2,
        POWERSAVE_TIMERS_POLICY = 3,
    } APSD_TIM_POLICY;

Command Values
    None

Reset Values
    psPollTimeout is 50 ms; triggerTimeout is 10 ms;
    apsdTimPolicy = IGNORE_TIM_ALL_QUEUES_APSD;
    simulatedAPSDTimPolicy = POWERSAVE_TIMERS_POLICY

Restrictions
    When this command is used, all parameters must be set; this command does not
    allow setting only one parameter.

=====================================================================


Name
    SET_PROBED_SSID

Synopsis
    The host uses this command to provide a list of up to MAX_PROBED_SSID_INDEX
    (six) SSIDs that the AR6000 device should actively look for. It lists the active SSID
    table. By default, the device actively looks for only the SSID specified in the
    “CONNECT_CMD” command, and only when the regulatory domain allows active
    probing. With this command, specified SSIDs are probed for, even if they are hidden.

Command
    wmiconfig eth1 --ssid=<ssid> [--num=<index>]

Command Parameters
    {
        A_UINT8    numSsids
            A number from 0 to
            MAX_PROBED_SSID_INDEX indicating
            the active SSID table entry index for this
            command (if the specified entry index
            already has an SSID, the SSID specified in
            this command replaces it)

        WMI_PROBED_SSID_INFO    probedSSID[1]
    } WMI_PROBED_SSID_CMD

    {
        A_UINT8    flag
            WMI_SSID_FLAG indicates the current
            entry in the active SSID table
        A_UINT8    ssidLength
            Length of the specified SSID in bytes.
            If = 0, the entry corresponding to the
            index is erased
        A_UINT8    ssid[32]
            SSID string actively probed for when
            permitted by the regulatory domain
    } WMI_PROBED_SSID_INFO

Command Values
    WMI_SSID_FLAG
    {
         DISABLE_SSID_FLAG    = 0
            Disables entry
        SPECIFIC_SSID_FLAG    = 1
            Probes specified SSID
        ANY_SSID_FLAG    = 2
            Probes for any SSID
    } WMI_SSID_FLAG

Reset Value
    The entries are unused.

Restrictions
    None

=====================================================================


Name
    SET_REASSOC_MODE

Synopsis
    Specify whether the disassociated frame should be sent or not upon reassociation.

Command
    wmiconfig eth1 --setreassocmode <mode>

Command Parameters
    UINT8    mode

Command Values
    mode
        = 0x00
        Send disassoc to a previously connected AP
        upon reassociation
        = 0x01
        Do not send disassoc to previously connected
        AP upon reassociation

Reset Values
    None defined

Restrictions
    None


=====================================================================

Name
    SET_RETRY_LIMITS

Synopsis
    Allows the host to influence the number of times that the AR6000 device should
    attempt to send a frame before they give up.

Command
    wmiconfig --setretrylimits <frameType> <trafficClass> <maxRetries>
    <enableNotify>

Command Parameters
    {
        UINT8    frameType
            A WMI_FRAMETYPE specifying
            which type of frame is of interest.
        UINT8    trafficClass
            Specifies a traffic class (see
            “CREATE_PSTREAM”). This
            parameter is only significant when
            frameType = DATA_FRAMETYPE.
        UINT8    maxRetries
            Maximum number of times the
            device attempts to retry a frame Tx,
            ranging from WMI_MIN_RETRIES
            (2) to WMI_MAX_RETRIES (15). If
            the special value 0 is used,
            maxRetries is set to 15.
        A_UINT8    enableNotify
            Notify when enabled
    } WMI_RETRY_LIMIT_INFO

    {
        A_UINT8    numEntries
        WMI_RETRY_LIMIT_INFO  retryLimitInfo[1]
    } WMI_SET_RETRY_LIMITS_CMD

Command Values
    {
        MGMT_FRAMETYPE    = 0    Management frame
        CONTROL_FRAMETYPE = 1    Control frame
        DATA_FRAMETYPE    = 2    Data frame
    } WMI_FRAMETYPE

Reset Values
    Retries are set to 15

Restrictions
    None

=====================================================================


Name
    SET_ROAM_CTRL

Synopsis
    Affects how the AR6000 device selects a BSS. The host uses this command to set and
    enable low RSSI scan parameters. The time period of low RSSI background scan is
    mentioned in scan period. Low RSSI scan is triggered when the current RSSI
    threshold (75% of current RSSI) is equal to or less than scan threshold.

    Low RSSI roam is triggered when the current RSSI threshold falls below the roam
    threshold and roams to a better AP by the end of the scan cycle. During Low RSSI
    roam, if the STA finds a new AP with an RSSI greater than roam RSSI to floor, during
    scan, it roams immediately to it instead of waiting for the end of the scan cycle. See
    also “Scan and Roam” on page C-1.

Command
    wmiconfig --roam <roamctrl> <info>, where info is <scan period>
    <scan threshold> <roam threshold> <roam rssi floor>

Command Parameters
    A_UINT8    roamCtrlType;

Command Values
    WMI_FORCE_ROAM     = 1
    Roam to the specified BSSID

    WMI_SET_ROAM_MODE  = 2
    Default, progd bias, no roam

    WMI_SET_HOST_BIAS  = 3
    Set the host bias

    WMI_SET_LOWRSSI_SCAN_PARAMS  = 4
    Info parameters

    A_UINT8    bssid[ATH_MAC_LEN];
    WMI_FORCE_ROAM

    A_UINT8    roamMode;
    WMI_SET_ROAM_MODE

    A_UINT8    bssBiasInfo;
    WMI_SET_HOST_BIAS

    A_UINT16    lowrssi_scan_period;
    WMI_SET_LOWRSSI_SCAN_PARAMS

    A_INT16
    lowrssi_scan_threshold;
    WMI_SET_LOWRSSI_SCAN_PARAMS

    A_INT16    lowrssi_roam_threshold;
    WMI_SET_LOWRSSI_SCAN_PARAMS

    A_UINT8    roam_rssi_floor;
    WMI_SET_LOWRSSI_SCAN_PARAMS

Reset Value
    None defined (default lowrssi scan is disabled. Enabled only when scan period is set.)

Restrictions
    None

=====================================================================


Name
    SET_RTS

Synopsis
    Decides when RTS should be sent.

Command
    wmiconfig eth1 --setRTS <pkt length threshold>

Command Parameters
    A_UINT16
    threshold;
    Command parameter threshold in bytes. An RTS is
    sent if the data length is more than this threshold.
    The default is to NOT send RTS.

Command Values
    None

Reset Value
    Not to send RTS.

Restrictions
    None


=====================================================================

Name
    SET_SCAN_PARAMS

Synopsis
    The host uses this command to set the AR6000 scan parameters, including the duty
    cycle for both foreground and background scanning. Foreground scanning takes
    place when the AR6000 device is not connected, and discovers all available wireless
    networks to find the best BSS to join. Background scanning takes place when the
    device is already connected to a network and scans for potential roaming candidates
    and maintains them in order of best to worst. A second priority of background
    scanning is to find new wireless networks.

    The device initiates a scan when necessary. For example, a foreground scan is always
    started on receipt of a “CONNECT_CMD” command or when the device cannot find
    a BSS to connect to. Foreground scanning is disabled by default until receipt of a
    CONNECT command. Background scanning is enabled by default and occurs every
    60 seconds after the device is connected.

    The device implements a binary backoff interval for foreground scanning when it
    enters the DISCONNECTED state after losing connectivity with an AP or when a
    CONNECT command is received. The first interval is ForegroundScanStartPeriod,
    which doubles after each scan until the interval reaches ForegroundScanEndPeriod.
    If the host terminates a connection with DISCONNECT, the foreground scan period
    is ForegroundScanEndPeriod. All scan intervals are measured from the time a full
    scan ends to the time the next full scan starts. The host starts a scan by issuing a
    “START_SCAN” command. See also “Scan and Roam” on page C-1.

Command
    wmiconfig eth1 --scan --fgstart=<sec> --fgend=<sec> --bg=<sec> --
    act=<msec> --pas=<msec> --sr=<short scan ratio> --scanctrlflags
    <connScan> <scanConnected> <activeScan> <reportBSSINFO>

Command Parameters
    UINT16    fgStartPeriod
        First interval used by the device when it
        disconnects from an AP or receives a
        CONNECT command, specified in seconds (0–
        65535). If = 0, the device uses the reset value.
        If = 65535, the device disables foreground
        scanning.

    UINT16    fgEndPeriod
        The maximum interval the device waits between
        foreground scans specified in seconds (from
        ForegroundScanStartPeriod to 65535). If = 0, the
        device uses the reset value.

    UINT16    bgScanPeriod
        The period of background scan specified in
        seconds (0–65535). By default, it is set to the reset
        value of 60 seconds. If 0 or 65535 is specified, the
        device disables background scanning.

    UINT16    maxactChDwellTime
        The period of time the device stays on a
        particular channel while active scanning. It is
        specified in ms (10–65535). If the special value of
        0 is specified, the device uses the reset value.

    UINT16    PasChDwellTime
        The period of time the device remains on a
        particular channel while passive scanning. It is
        specified in ms (10–65535). If the special value of
        0 is specified, the device uses the reset value.

    UINT8    shortScanRatio
        Number of short scans to perform for each
        long scan.

    UINT8    scanCtrlFlasgs

    UINT16    minactChDwellTime
        Specified in ms

    UINT32    maxDFSchActTime
        The maximum time a DFS channel can stay
        active before being marked passive, specified in
        ms.

Command Values
    None

Reset Values
    ForegroundScanStart
Period
        1 sec

    ForegroundScanEndPeriod
        60 sec

    BackgroundScanPeriod
        60 sec

    ActiveChannelDwellTime
        105 ms

=====================================================================


Name
    SET_TKIP_COUNTERMEASURES

Synopsis
    The host issues this command to tell the target whether to enable or disable TKIP
    countermeasures.

Command
    TBD

Command Parameters
    UINT8    WMI_TKIP_CM_ENABLE
        Enables the countermeasures


    UINT8    TKIP_CM_DISABLE
        Disables the countermeasures

Command Values
    None

Reset Values
    By default, TKIP MIC reporting is disabled

Restrictions
    None

=====================================================================


Name
    SET_TX_PWR

Synopsis
    The host uses this command to specify the Tx power level of the AR6000. Cannot be
    used to exceed the power limit permitted by the regulatory domain. The maximum
    output power is limited in the chip to 31.5 dBm; the range is 0 – 31.5 dbm.

Command
    wmiconfig --power <dbM>

Command Parameters
    UINT8    dbM
        The desired Tx power specified in dbM.
        If = 0, the device chooses the maximum
        permitted by the regulatory domain.

Command Values
    None

Reset Values
    The maximum permitted by the regulatory domain

Restrictions
    None

See Also
    “GET_TX_PWR”


=====================================================================

Name
    SET_VOICE_PKT_SIZE

Synopsis
    If an AP does not support WMM, it has no way to differentiate voice from data.
    Because the voice packet is typically small, packet in size less than voicePktSize are
    assumed to be voice, otherwise it is treated as data.

Command
    wmiconfig eth1 --setVoicePktSize <size-in-bytes>

Command Parameters
    UINT16    voicePktSize
        Packet size in octets

Command Values
    None

Reset Values
    voicePktSize default is 400 bytes

Restrictions
    No effect if WMM is unavailable


=====================================================================

Name
    SET_WMM

Synopsis
    Overrides the AR6000 device WMM capability

Command
    wmiconfig eth1 --setwmm <enable>

Command Parameters
    WMI_WMM_ENABLED
        Enables WMM

    WMI_WMM_DISABLED
        Disables WMM support

Command Values
    0 = disabled
    1 = enabled

Reset Value
    WMM Disabled

Restrictions
    None


=====================================================================

Name
    SET_WMM_TXOP

Synopsis
    Configures TxOP Bursting when sending traffic to a WMM capable AP

Command
    wmiconfig eth1 --txopbursting <burstEnable>

    <burstEnable>
        = 0
    Disallow TxOp bursting

        = 1
    Allow TxOp bursting

Command Parameters
    txopEnable
        = WMI_TXOP_DISABLED
        Disabled

        = WMI_TXOP_ENABLED
        Enabled

Command Values
    txopEnable
        = 0    Disabled

        = 1    Enabled

Reset Value
    Bursting is off by default

Restrictions
    None

=====================================================================


Name
    SET_WOW_MODE

Synopsis
    The host uses this command to enable or disable the WoW mode. When WoW mode
    is enabled and the host is asleep, pattern matching takes place at the target level.
    Only packets that match any of the pre-specified WoW filter patterns, will be passed
    up to the host. The host will also be woken up by the target. Packets which do not
    match any of the WoW patterns are discarded.

Command
    wmiconfig –setwowmode <enable/disable>

Command Parameters
    A_BOOL    enable_wow
        Enable or disable WoW:

Command Values
        = 0
    Disable WoW

        = 1
    Enable WoW

Reset Value
    None defined (default WoW mode is disabled).

Restrictions
    None

See Also
    “GET_WOW_LIST”


=====================================================================

Name
    SET_WSC_STATUS

Synopsis
    The supplicant uses this command to inform the target about the status of the WSC
    registration protocol. During the WSC registration protocol, a flag is set so the target
    bypasses some of the checks in the CSERV module. At the end of the registration, this
    flag is reset.

Command
    N/A

Command Parameters
    A_BOOL status
        = 1    WSC registration in progress
        = 0    WSC protocol not running

Reply Parameters
    None

Reset Value
    None defined (default = 0)

Restrictions
    None


=====================================================================

Name
    SNR_THRESHOLD_PARAMS

Synopsis
    Configures how the AR6000 device monitors and reports SNR of the connected BSS,
    used as a link quality metric.

Command
    --snrThreshold <weight> <upper_threshold_1> ...
    <upper_threshold_4> <lower_threshold_1> ... <lower_threshold_4>
    <pollTimer>

Command Parameters
    <weight>
        Share with rssiThreshold. Range in [1, 16], used
        in the formula to calculate average RSSI

    <upper_threshold_x>
        Above thresholds expressed in db, in ascending
        order

    <lower_threshold_x>
        Below thresholds expressed in db, in ascending
        order

    <pollTimer>
        The signal strength sampling frequency in
        seconds. If polltime = 0, signal strength
        sampling is disabled

Command Values
    None

Reset Value
    None defined

Restrictions
    None

=====================================================================


Name
    START_SCAN

Synopsis
    The host uses this command to start a long or short channel scan. All future scans are
    relative to the time the AR6000 device processes this command. The device performs
    a channel scan on receipt of this command, even if a scan was already in progress.
    The host uses this command when it wishes to refresh its cached database of wireless
    networks. The isLegacy field will be removed (0 for now) because it is achieved by
    setting CONNECT_PROFILE_MATCH_DONE in the CONNECT command. See also
    “Scan and Roam”

Command
    wmiconfig eth1 --startscan <scan type> <forcefgscan> 0
    <homeDwellTime> <forceScanInterval>

Command Parameters
    UINT8    scanType
        WMI_SCAN_TYPE

Command Values
    {
        WMI_LONG_SCAN    =0x0
            Requests a full scan
        WMI_SHORT_SCAN   =0x1
            Requests a short scan
    } WMI_SCAN_TYPE

    A_BOOL    forceFgScan
        forceFgScan
        = 0
        Disable the foreground scan

        forceFgScan
        = 1
        Forces a foreground scan

    A_UINT32    homeDwellTime
        Maximum duration in the home
        channel (in ms)

    A_UINT32     forceScanInterval
        Time interval between scans (in ms)

    A_UINT32     scanType
        WMI_SCAN_TYPE

Reset Value
    Disable forcing foreground scan

Restrictions
    isLegacy field will no longer be supported (pass as 0 for now)


=====================================================================

Name
    SYNCHRONIZE

Synopsis
    The host uses this command to force a synchronization point between the command
    and data paths

Command
    TBD

Command Parameters
    None



Command Values
    None



Reset Values
    None



Restrictions
    None


=====================================================================

Name
    TARGET_ERROR_REPORT_BITMASK

Synopsis
    Allows the host to control “ERROR_REPORT” events from the AR6000 device.

        If error reporting is disabled for an error type, a count of errors of that type is
        maintained by the device.

        If error reporting is enabled for an error type, an “ERROR_REPORT” event is
        sent when an error occurs and the error report bit is cleared.

    Error counts for each error type are available through the “GET_TARGET_STATS”
    command.

Command
    wmiconfig eth1 --setErrorReportingBitmask

Command Parameters
    UINT32    bitmask
        Represents the set of
        WMI_TARGET_ERROR_VAL error types
        enabled for reporting

Command Values
    {
        WMI_TARGET_PM_ERR_FAIL    = 0x00000001
            Power save fails (only two cases):
               Retry out of null function/QoS null
               function to associated AP for PS
               indication'
               Host changes the PS setting when
               STA is off home channel

        WMI_TARGET_KEY_NOT_FOUND    = 0x00000002
            No cipher key
        WMI_TARGET_DECRYPTION_ERR   = 0x00000004
            Decryption error
        WMI_TARGET_BMISS            = 0x00000008
            Beacon miss
        WMI_PSDISABLE_NODE_JOIN     = 0x00000010
            A non-PS-enabled STA joined the
            PS-enabled network
        WMI_TARGET_COM_ERR          = 0x00000020
            Host/target communication error
        WMI_TARGET_FATAL_ERR        = 0x00000040
            Fatal error
    } WMI_TARGET_ERROR_VAL

Reset Values
    Bitmask is 0, and all error reporting is disabled

Restrictions
    None


=====================================================================
WMI Events

Event
    Description
    Page


BSSINFO
    Contains information describing BSSs collected during a scan

CAC_EVENTID
    Indicates signalling events in admission control

CMDERROR
    The AR6000 device encounters an error while attempting to process
    a command

CONNECT
    The device has connected to a wireless network

DISCONNECT
    The device lost connectivity with a wireless network

ERROR_REPORT
    An error has occurred for which the host previously requested
    notification with the command
    “TARGET_ERROR_REPORT_BITMASK”

EXTENSION
    WMI extension event

GET_PMKID_LIST_EVENT
    Created in response to a “GET_PMKID_LIST_CMD” command

GET_WOW_LIST_EVENT
    Response to the wmiconfig “GET_WOW_LIST” command to
    retrieve the configured WoW patterns

NEIGHBOR_REPORT
    Neighbor APs that match the current profile were detected

OPT_RX_FRAME_EVENT
    (Special feature) informs the host of the reception of a special frame

PSTREAM_TIMEOUT
    A prioritized stream has been idle for a specified interval

READY
    The AR6000 device is ready to accept commands

REGDOMAIN
    The regulatory domain has changed

REPORT_ROAM_DATA_EVENT
    Reports the roam time calculations made by the device
    (generated with a special build)
    —

REPORT_STATISTICS
    Reply to a “GET_TARGET_STATS” command

ROAM_TBL_EVENT
    Reports the roam table

RSSI_THRESHOLD
    Signal strength from the connected AP has crossed the threshold
    defined in the “RSSI_THRESHOLD_PARAMS” command

SCAN_COMPLETE_EVENT
    A scan has completed (added status SCAN_ABORTED in release 2.0)

TEST_EVENT
    Event generated by the TCMD

TKIP_MICERROR
    TKIP MIC errors were detected

=====================================================================

Name
    BSSINFO

Synopsis
    Contains information describing one or more BSSs as collected during a scan.
    Information includes the BSSID, SSID, RSSI, network type, channel, supported rates,
    and IEs. BSSINFO events are sent only after the device receives a beacon or probe-
    response frame that pass the filter specified in the “SET_BSS_FILTER” command.
    BSSINFO events consist of a small header followed by a copy of the beacon or probe
    response frame. The 802.11 header is not present. For formats of beacon and probe-
    response frames please consult the IEEE 802.11 specification.

    The beacons or probe responses containing the IE specified by the
    WMI_BSS_FILTER_CMD are passed to the host through the
    WMI_BSSINFO_EVENT. The event carries a 32-bit bitmask that indicates the IEs that
    were detected in the management frame. The frame type field has been extended to
    indicate action management frames. This would be helpful to route these frames
    through the same event mechanism as used by the beacon processing function.

    If the bssFilter in the SET_BSS_FILTER matches, then the ieMask is not relevant
    because the BSSINFO event is sent to the host. If the bssFilter doesnot match in the
    beacons/probe respones, then the ieMask match dictates whether the BSSINFO
    event is sent to the host. In the case of action management frames, the ieMask is the
    filter that is applied.

Event ID
    0x1004

Event Parameters
    typedef struct {
        A_UINT16    channel;
            Specifies the frequency (in MHz) where the
            frame was received
        A_UINT8    frameType;
            A WMI_BI_FTYPE value
        A_UINT8    snr;
        A_INT16    rssi;
            Indicates signal strength
        A_UINT8    bssid[ATH_MAC_LEN];
        A_UINT32    ieMask;
    } _ATTRIB_PACK_WMI_BSS_INFO_HDR;

    Beacon or Probe Response Frame

Event Values
    {
        BEACON_FTYPE    = 0x1
            Indicates a beacon frame
        PROBERESP_FTYPE
            Indicates a probe response frame
        ACTION_MGMT_FTYPE
     } WMI_BI_FTYPE

=====================================================================

Name
    CAC_EVENTID

Synopsis
    Indicates signalling events in admission control. Events are generated when
    admission is accepted, rejected, or deleted by either the host or the AP. If the AP does
    not respond to an admission request within a timeout of 500 ms, an event is
    generated to the host.

Event ID
    0x1011

Event Parameters
    UINT8
    ac
    Access class pertaining to the
signalling

    UINT8    cac_indication
        Type of indication; indications are
        listed in WMI_CAC_INDICATION

    UINT8    statusCode
        AP response status code for a
        request

    UINT8    tspecSuggestion[63]
        Suggested TSPEC from AP

Event Values
    {
        CAC_INDICATION_ADMISSION = 0x00
        CAC_INDICATION_ADMISSION_RESP    = 0x01
        CAC_INDICATION_DELETE    = 0x02
        CAC_INDICATION_NO_RESP   = 0x03
    } WMI_CAC_INDICATION


=====================================================================


Name
    CMDERROR

Synopsis
    Indicates that the AR6000 device encountered an error while attempting to process a
    command. This error is fatal and indicates that the device requires a reset.

Event ID
    0x1005

Event Parameters
    UINT16    commandId
        Corresponds to the command which generated
        the error
    UINT8    errorCode
        A WMI_ERROR_CODE value

Event Values
    {
        INVALID_PARAM    = 1
            Invalid parameter
        ILLEGAL_STATE    = 2
            Illegal state
        INTERNAL_ERROR   = 3
            Internal Error
        All other values reserved
    } WMI_ERROR_CODE


=====================================================================


Name
    CONNECT

Synopsis
    Signals that the AR6000 connected to a wireless network. Connection occurs due to a
    “CONNECT” command or roaming to a new AP. For infrastructure networks, shows
    that the AR6000 successfully performed 802.11 authentication and AP association.

Event ID
    0x1002

Event Parameters
    UINT16    channel
        Channel frequency (in MHz) of the network the
        AR6000 are connected to

    UINT8    bssid[6]
        MAC address of the AP the AR6000 are
        connected to or the BSSID of the ad hoc
        network

    UINT16    listenInterval
        Listen interval (in Kms) that the AR6000 are
        using

    UINT 8    beaconIeLen
        Length (in bytes) of the beacon IEs

    UINT8    assocInfo
        Pointer to an array containing beacon IEs,
        followed first by association request IEs then by
        association response IEs

    UINT8    assocReqLen
        Length (in bytes) of the assocReqIEs array

    UINT8    assocRespLen
        Length (in bytes) of the assocRespIEs array

Event Values
    None defined

=====================================================================


Name
    DISCONNECT

Synopsis
    Signals that the AR6000 device lost connectivity with the wireless network.
    DISCONENCT is generated when the device fails to complete a “CONNECT”
    command or as a result of a transition from a connected state to disconnected state.

    After sending the “DISCONNECT” event the device continually tries to re-establish
    a connection. A LOST_LINK occurs when STA cannot receive beacons within the
    specified time for the SET_BMISS_TIME command.

Event ID
    0x1003

Event Parameters
    UINT8    disconnect
        Reason
        A WMI_DISCONNECT_REASON value

    UINT8    bssid[6]
        Indicates which BSS the device was connected to

    UINT8    assocRespLen
        Length of the 802.11 association response frame
        that triggered this event, or 0 if not applicable

    UINT8    assocInfo[assocRespLen]
        Copy of the 802.11 association response frame

Event Values
    {
        NO_NETWORK_AVAIL    =0x01
            Indicates that the device was unable to
            establish or find the desired network
        LOST_LINK    =0x02
            Indicates the devices is no longer receiving
            beacons from the BSS it was previously
            connected to

        DISCONNECT_CMD  =0x03
            Indicates a “DISCONNECT” command was
            processed
        BSS_DISCONNECTED =0x04
            Indicates the BSS explicitly disconnected the
            device. Possible mechanisms include the AP
            sending 802.11 management frames
            (e.g., disassociate or deauthentication
            messages).
        AUTH_FAILED    =0x05
            Indicates that the device failed 802.11
            authentication with the BSS
        ASSOC_FAILED    =0x06
            Indicates that the device failed 802.11
            association with the BSS
        NO_RESOURCES_AVAIL    =0x07
            Indicates that a connection failed because the
            AP had insufficient resources to complete the
            connection
        CSERV_DISCONNECT    =0x08
            Indicates that the device’s connection services
            module decided to disconnect from a BSS,
            which can happen for a variety of reasons (e.g.,
            the host marks the current connected AP as a
            bad AP).
        INVALID_PROFILE    =0x0A
            Indicates that an attempt was made to
            reconnect to a BSS that no longer matches the
            current profile
        All other values are reserved
    } WMI_DISCONNECT_REASON


=====================================================================


Name
    ERROR_REPORT

Synopsis
    Signals that a type of error has occurred for which the host previously requested
    notification through the “TARGET_ERROR_REPORT_BITMASK” command.

Event ID
    0x100D

Event Parameters
    UINT32    errorVal
            WMI_TARGET_ERROR_VAL value. See
            “TARGET_ERROR_REPORT_BITMASK”.

Event Values
    errorVal
    = 0x00000001
    Power save fails

        = 0x00000002
    No cipher key

        = 0x00000004
    Decryption error

        = 0x00000008
    Beacon miss

        = 0x00000010
    A non-power save disabled node has joined
    the PS-enabled network


=====================================================================


Name
    EXTENSION

Synopsis
    The WMI is used mostly for wireless control messages to a wireless module that
    apply to wireless module management regardless of the target platform
    implementation. However, some events peripherally related to wireless management
    are desired during operation. These wireless extension events may be platform-
    specific or implementation-dependent. See “WMI Extension Commands”


Event ID
    0x1010


=====================================================================


Name
    GET_PMKID_LIST_EVENT

Synopsis
    Generated by firmware in response to a “GET_PMKID_LIST_CMD” command.

Event Parameters
    typedef struct {
        A_UINT32    numPMKID;
            Contains the number of PMKIDs in the reply
        WMI_PMKID   pmkidList[1];
    } __ATTRIB_PACK WMI_PMKID_LIST_REPLY;

Event Values
    None


=====================================================================


Name
    GET_WOW_LIST_EVENT

Synopsis
    Response to the wmiconfig –getwowlist command to retrieve the configured Wake on
    Wireless patterns

Event ID
    0x10018

Event Parameters
    {

    A_UINT8    num_filters
        Total number of patterns in the list
    A_UINT8    this_filter_num
        The filter number
    A_UINT8    wow_mode
        Shows whether WoW is enabled or disabled
    A_UINT8    host_mode
        Shows whether the host is asleep or awake
    WOW_FILTER    wow_filters[1]
        List of WoW filters (pattern and mask data bytes)
    } WMI_GET_WOW_LIST_REPLY;

    {
        Each wow_filter_list element shows:
    A_UINT8    wow_valid_filter
        Whether the filter is valid
    A_UINT8    wow_filter_list_id
        Filter List ID (23 = default)
    A_UINT8    wow_filter_size
        Size in bytes of the filter
    A_UINT8    wow_filter_offset
        Offset of the pattern to search in the data packet
    A_UINT8    wow_filter_mask[MASK_SIZE]
        The mask to be applied to the pattern
    A_UINT8    wow_filter_pattern[WOW_PATTERN_SIZE]
        The pattern that to match to wake up the host
    } WOW_FILTER

Event Values
    None

=====================================================================



Name
    NEIGHBOR_REPORT

Synopsis
    Indicates the existence of neighbor APs that match the current profile. The host uses
    this event to populate the PMKID cache on the AR6000 and/or to perform
    preauthentication. This event is only generated in infrastructure mode.

    A total of numberOfAps pairs of bssid/bssFlags exist, one pair for each AP.

Event ID
    0x1008

Event Parameters
    UINT8    numberOfAps
        The number of APs reported about in
        this event
    {
        UINT8    bssid[6]
            MAC address of a neighbor AP
        UINT8    bssFlags
            A WMI_BSS_FLAGS value
    }[numberOfAps]


Event Values
    {
        WMI_DEFAULT_BSS_FLAGS    = 0
            Logical OR of 1 or more
        WMI_BSS_FLAGS
        WMI_PREAUTH_CAPABLE_BSS
            = 1
            Indicates that this AP is capable of
            preauthentication
        WMI_PMKID_VALID_BSS
            = 2
            Indicates that the AR6000 have a
            valid pairwise master key for this AP
    } WMI_BSS_FLAGS


=====================================================================



Name
    OPT_RX_FRAME_EVENT

Synopsis
    Special feature, informs host of the reception of a special frame.

Event ID
    0x100E

Event Parameters
    {
        A_UINT16    channel;
        A_UINT8    frameType;
        A_INT8    snr;
        A_UINT8    srcAddr[ATH_MAC_LEN];
        A_UINT8    bssid[ATH_MAC_LEN];
        }WMI_OPT_RX_INFO_HDR

Event Values
    None

=====================================================================



Name
    PSTREAM_TIMEOUT

Synopsis
    Indicates that a priority stream that got created as a result of priority-marked data
    flow (priority marked in IP TOS) being idle for the default inactivity interval period
    (specified in the “CREATE_PSTREAM” command) used for priority streams created
    implicitly by the driver. This event is not indicated for user-created priority streams.
    User-created priority streams exist until the users delete them explicitly. They do not
    timeout due to data inactivity.

Event ID
    0x1007

Event Parameters
    A_UINT8
    trafficClass
    Indicated the traffic class of priority
    stream that timed out

Event Values
    {
        WMM_AC_BE    = 0
            Best effort
        WMM_AC_BK    = 1
            Background
        WMM_AC_VI    = 2
            Video
        WMM_AC_VO     = 3
            Voice
    } TRAFFIC CLASS


=====================================================================

Name
    READY

Synopsis
    Indicates that the AR6000 device is prepared to accept commands. It is sent once after
    power on or reset. It also indicates the MAC address of the device.

Event ID
    0x1001

Event Parameters
    UINT8    macAddr[6]
        Device MAC address
    UINT8    phyCapability
        A WMI_PHY_CAPABILITY value. Indicates the
        capabilities of the device wireless module’s radio

Event Values
    {
        WMI_11A_CAPABILITY       = 1
        WMI_11G_CAPABILITY       = 2
        WMI_11AG_CAPABILITY      = 3
    } WMI_PHY_CAPABILITY


=====================================================================

Name
    REGDOMAIN

Synopsis
    Indicates that the regulatory domain has changed. It initially occurs when the
    AR6000 device reads the board data information. The regulatory domain can also
    change when the device is a world-mode SKU. In this case, the regulatory domain is
    based on the country advertised by APs per the IEEE 802.11d specification. A
    potential side effect of a regulatory domain change is a change in the list of available
    channels. Any channel restrictions that exist as a result of a previous
    “SET_CHANNEL_PARAMETERS” command are lifted.

Event ID
    0x1006

Event Parameters
    UINT32    regDomain
        The range of 0x0000 – 0x00FF
        corresponds to an ISO country code.

    Other regCodes are reserved for world
    mode settings and specific regulatory
    domains.

Event Values
    None


=====================================================================



Name
    REPORT_STATISTICS

Synopsis
    A reply to a “GET_TARGET_STATS” command.

Event ID
    0x100B

Event Parameters
    When the statistics are sent to the host, the AR6001 clear them so that a new set of
    statistics are collected for the next report.

    UINT32    tx_packets
    UINT32    tx_bytes
    UINT32    tx_unicast_pkts
    UINT32    tx_unicast_bytes
    UINT32    tx_multicast_pkts
    UINT32    tx_multicast_bytes
    UINT32    tx_broadcast_pkts
    UINT32    tx_broadcast_bytes
    UINT32    tx_rts_success_cnt
    UINT32    tx_packet_per_ac[4]
        Tx packets per AC: [0] = BE, [1] = BK,
        [2] = VI, [3] = VO
    UINT32    tx_errors
        Number of packets which failed Tx, due
        to all failures
    ... REPORT_STATISTICS, continued
    UINT32    tx_failed_cnt
        Number of data packets that failed Tx
    UINT32    tx_retry_cnt
        Number of Tx retries for all packets
    UINT32    tx_rts_fail_cnt
        Number of RTS Tx failed count
    UINT32    rx_packets
    UINT32    rx_bytes
    UINT32    rx_unicast_pkts
    UINT32    rx_unicast_bytes
    UINT32    rx_multicast_pkts
    UINT32    rx_multicast_bytes
    UINT32    rx_broadcast_pkts
    UINT32    rx_broadcast_bytes
    UINT32    rx_fragment_pkt
        Number of fragmented packets received
    UINT32    rx_errors
        Number of Rx errors due to all failures
    UINT32    rx_crcerr
        Number of Rx errors due to CRC errors
    UINT32    rx_key_cache_miss
        Number of Rx errors due to a key not
        being plumbed
    UINT32    rx_decrypt_err
        Number of Rx errors due to decryption
        failure
    UINT32    rx_duplicate_frames
        Number of duplicate frames received
    UINT32    tkip_local_mic_failure
        Number of TKIP MIC errors detected
    UINT32    tkip_counter_measures_invoked
        Number of times TKIP countermeasures
        were invoked
    UINT32    tkip_replays
        Number of frames that replayed a TKIP
        encrypted frame received earlier
    UINT32    tkip_format_errors
        Number of frames that did not conform
        to the TKIP frame format
    UINT32    ccmp_format_errors
        Number of frames that did not conform
        to the CCMP frame format
    UINT32    ccmp_replays
        Number of frames that replayed a CCMP
        encrypted frame received earlier
    UINT32    power_save_failure_cnt
        Number of failures that occurred when
        the AR6001 could not go to sleep
    UINT32    cs_bmiss_cnt
        Number of BMISS interrupts since
    connection
    UINT32    cs_lowRssi_cnt
        Number of the times the RSSI went below
        the low RSSI threshold
    UINT16    cs_connect_cnt
        Number of connection times
    UINT16    cs_disconnect_cnt
        Number of disconnection times
    UINT8    cs_aveBeacon_rssi
        The current averaged value of the RSSI
        from the beacons of the connected BSS
    UINT8    cs_lastRoam_msec
        Time that the last roaming took, in ms.
        This time is the difference between
        roaming start and actual connection.

Event Values
    None defined


=====================================================================

Name
    ROAM_TBL_EVENT

Synopsis
    Reports the roam table, which contains the current roam mode and this information
    for every BSS:

Event ID
    0x100F

Event Parameters
    A_UINT8     bssid[ATH_MAC_LEN];
        BSSID
    A_UINT8     rssi
        Averaged RSSI
    A_UINT8     rssidt
        Change in RSSI
    A_UINT8     last_rssi
        Last recorded RSSI
    A_UINT8     roam_util
        Utility value used in roaming decision
    A_UINT8     util
        Base utility with the BSS
    A_UINT8     bias
        Host configured for this BSS

Event Values
    roamMode
        Current roam mode

        = 1
    RSSI based roam

        = 2
    Host bias-based roam

        = 3
    Lock to the current BSS

        = 4
    Autonomous roaming disabled


=====================================================================

Name
    RSSI_THRESHOLD

Synopsis
    Alerts the host that the signal strength from the connected AP has crossed a
    interesting threshold as defined in a previous “RSSI_THRESHOLD_PARAMS”
    command.

Event ID
    0x100C

Event Parameters
    UINT8    range
        A WMI_RSSI_THRESHOLD_VAL
        value, which indicates the range of
        the average signal strength

Event Values
    {
        WMI_RSSI_LOWTHRESHOLD_BELOW_LOWERVAL  = 1
        WMI_RSSI_LOWTHRESHOLD_LOWERVAL        = 2
        WMI_RSSI_LOWTHRESHOLD_UPPERVAL        = 3
        WMI_RSSI_HIGHTHRESHOLD_LOWERVAL       = 4
        WMI_RSSI_HIGHTHRESHOLD_HIGHERVAL      = 5
    } WMI_RSSI_THRESHOLD_VAL


=====================================================================

Name
    SCAN_COMPLETE_EVENT

Synopsis
    Indicates the scan status. if the Scan was not completed, this event is generated with
    the status A_ECANCELED.

Event ID
    0x100A

Event Parameters
    A_UINT8    scanStatus

Event Values
    {
        #define     SCAN_ABORTED 16
        #define     SCAN_COMPLETED 0
        A_UINT8     scanStatus
            A_OK or A_ECANCELED
    } WMI_SCAN_COMPLETE_EVENT;


=====================================================================

Name
    TEST_EVENT

Synopsis
    The TCMD application uses a single WMI event (WMI_TEST_EVENTID) to
    communicate events from target to host. The events are parsed by the TCMD
    application and WMI layer is oblivious of it.

Event ID
    0x1016

Event Parameters
    WMI_TEST_EVENTID


Event Values
    None


=====================================================================



Name
    TKIP_MICERR

Synopsis
    Indicates that TKIP MIC errors were detected.

Event ID
    0x1009

Event Parameters
    UINT8    keyid
        Indicates the TKIP key ID

    UINT8    ismcast
        0 = Unicast
        1 = Multicast

Event Values
    See event parameters

=====================================================================

WMI Extension Commands

The WMI EXTENSION command is used to multiplex a collection of
commands that:

        Are not generic wireless commands
        May be implementation-specific
        May be target platform-specific
        May be optional for a host implementation

    An extension command is sent to the AR6000 targets like any other WMI
command message and uses the WMI_EXTENSION. The first field of the
payload for this EXTENSION command is another commandId, sometimes
called the subcommandId, which indicates which extension command is
being used. A subcommandId-specific payload follows the subcommandId.

All extensions (subcommandIds) are listed in the header file include/wmix.h.
See also “WMI Extension Events” on page B-58.


WMI Extension Commands


GPIO_INPUT_GET
    Read GPIO pins configured for input

GPIO_INTR_ACK
    Acknowledge and re-arm GPIO interrupts reported earlier

GPIO_OUTPUT_SET
    Manage output on GPIO pins configured for output

GPIO_REGISTER_GET
    Read an arbitrary GPIO register

GPIO_REGISTER_SET
    Dynamically change GPIO configuration

SET_LQTHRESHOLD
    Set link quality thresholds; the sampling happens at every unicast
    data frame Tx, if certain thresholds are met, and corresponding
    events are sent to the host


=====================================================================

Name
    GPIO_INPUT_GET

Synopsis
    Allows the host to read GPIO pins that are configured for input. The values read are
    returned through a “GPIO_DATA” extension event.

NOTE: Support for GPIO is optional.

Command
    N/A

Command Parameters
    None



Reply Parameters
    None


Reset Value
    None



Restrictions
    None

=====================================================================


Name
    GPIO_INTR_ACK

Synopsis
    The host uses this command to acknowledge and to re-arm GPIO interrupts reported
    through an earlier “GPIO_INTR” extension event. A single “GPIO_INTR_ACK”
    command should be used to acknowledge all GPIO interrupts that the host knows to
    be outstanding (if pending interrupts are not acknowledged through
    “GPIO_INTR_ACK”, another “GPIO_INTR” extension event is raised).

NOTE: Support for GPIO is optional.

Command
    N/A

Command Parameters
    UINT32    ack_mask
        A mask of interrupting GPIO pins (e.g., ack_mask
        bit [3] acknowledges an interrupt from the pin GPIO3).

Command Values
    None

Reset Value
    None

Restrictions
    The host should acknowledge only interrupts about which it was notified.


=====================================================================

Name
    GPIO_OUTPUT_SET

Synopsis
    Manages output on GPIO pins configured for output.

    Conflicts between set_mask and clear_mask or enable_mask and disable_mask result
    in undefined behavior.

NOTE: Support for GPIO is optional.

Command
    N/A

Command Parameters
    UINT32    set_mask
        Specifies which pins should drive a 1 out
    UINT32    clear_mask
        Specifies which pins should drive a 0 out
    UINT32    enable_mask
        Specifies which pins should be enabled for output
    UINT32    disable_mask
        Specifies which pins should be disabled for output

Command Values
    None


Reset Value
    None


Restrictions
    None



=====================================================================


Name
    GPIO_REGISTER_GET

Synopsis
    Allows the host to read an arbitrary GPIO register. It is intended for use during
    bringup/debug. The target responds to this command with a “GPIO_DATA” event.

NOTE: Support for GPIO is optional.

Command
    N/A

Command Parameters
    UINT32
    gpioreg_id
    Specifies a GPIO register identifier, as defined
in include/AR6000/AR6000_gpio.h

Reply Parameters
    None

Reset Value
    N/A

Restrictions
    None


=====================================================================

Name
    GPIO_REGISTER_SET

Synopsis
    Allows the host to dynamically change GPIO configuration (usually handled
    statically through the GPIO configuration DataSet).

NOTE: Support for GPIO is optional.

Command
    N/A

Command Parameters
    UINT32    gpioreg_id
        Specifies a GPIO register identifier, as defined in
        include/AR6000/AR6000_gpio.h
    UINT32    value
        Specifies a value to write to the specified
        GPIO register

Command Values
    None


Reset Value
    Initial hardware configuration is as defined in the AR6001 or AR6002 ROCmTM
    Single-Chip MAC/BB/Radio for 2.4/5 GHz Embedded WLAN Applications data sheet. This
    configuration is modified by the GPIO Configuration DataSet, if one exists.

Restrictions
    None


=====================================================================


Name
    SET_LQTHRESHOLD

Synopsis
    Set link quality thresholds, the sampling happens at every unicast data frame Tx, if
    certain threshold is met, corresponding event will be sent to host.

Command
    wmiconfig eth1 --lqThreshold <enable> <upper_threshold_1>...
    <upper_threshold_4> <lower_threshold_1>... <lower_threshold_4>

Command Parameters
    A_UINT8   enable;
    A_UINT8   thresholdAbove1_Val;
    A_UINT8   thresholdAbove2_Val;
    A_UINT8   thresholdAbove3_Val;
    A_UINT8   thresholdAbove4_Val;
    A_UINT8   thresholdBelow1_Val;
    A_UINT8   thresholdBelow2_Val;
    A_UINT8   thresholdBelow3_Val;
    A_UINT8   thresholdBelow4_Val;

Command Values
    enable
    = 0
    Disable link quality sampling

        = 1
    Enable link quality sampling


    thresholdAbove_Val
    [1...4]
    Above thresholds (value in [0,100]), in ascending
    order threshold

    Below_Val [1...4] = below thresholds (value
    in [0,100]), in ascending order

Reset Values
    None

Restrictions
    None

=====================================================================
WMI Extension Events

The WMI EXTENSION event is used for a collection of events that:

    Are not generic wireless events
    May be implementation-specific
    May be target platform-specific
    May be optional for a host implementation

    An extension event is sent from the AR6000 device targets to the host just like
any other WMI event message, using the WMI_EXTENSION_EVENTID. The
first field of the payload for this “EXTENSION” event is another commandId
(sometimes called the subcommandId) that indicates which “EXTENSION”
event is being used. A subcommandId-specific payload follows the
subcommandId.

All extensions (subcommandIds) are listed in the header file include/wmix.h.
See also “WMI Extension Commands” on page B-55.


WMI Extension Events


GPIO_ACK
    Acknowledges a host set command has been processed by the device

GPIO_DATA
    Response to a host’s request for data

GPIO_INTR
    Signals that GPIO interrupts are pending


=====================================================================

Name
    GPIO_ACK

Synopsis
    Acknowledges that a host set command (either “GPIO_OUTPUT_SET” or
    “GPIO_REGISTER_SET”) has been processed by the AR6000 device.

NOTE: Support for GPIO is optional.

Event ID
    N/A

Event Parameters
    None


Event Values
    None

=====================================================================


Name
    GPIO_DATA

Synopsis
    The AR6000 device uses this event to respond to the host’s earlier request for data
    (through either a “GPIO_REGISTER_GET” or a “GPIO_INPUT_GET” command).

NOTE: Support for GPIO is optional.

Event ID
    N/A

Event Parameters
    UINT32    value
        Holds the data of interest, which is either a register value
        (in the case of “GPIO_REGISTER_GET”) or a mask of
        pin inputs (in the case of “GPIO_INPUT_GET”).
    UINT32    reg_id
        Indicates which register was read (in the case of
        “GPIO_REGISTER_GET”) or is GPIO_ID_NONE (in the
        case of “GPIO_INPUT_GET”)

Event Values
    None


=====================================================================



Name
    GPIO_INTR

Synopsis
    The AR6000 device raises this event to signal that GPIO interrupts are pending.
    These GPIOs may be interrupts that occurred after the last “GPIO_INTR_ACK”
    command was issued, or may be GPIO interrupts that the host failed to acknowledge
    in the last “GPIO_INTR_ACK”. The AR6000 will not raise another GPIO_INTR
    event until this event is acknowledged through a “GPIO_INTR_ACK” command.

NOTE: Support for GPIO is optional.

Event ID
    N/A

Event Parameters
    UINT32    intr_mask
        Indicates which GPIO interrupts are currently pending

    UINT32    input_values
        A recent copy of the GPIO input values, taken at the
        time the most recent GPIO interrupt was processed

Event Values
    None



=====================================================================
#endif
