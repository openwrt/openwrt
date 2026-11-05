/* SPDX-License-Identifier: GPL-2.0 */
#include "omci.h"

static const struct omci_attr_def omciAttriDescriptListOnuData[] = {
	{  1,   1, 3, 4, 0, "MIB data sync" },
};

static const struct omci_attr_def omciAttriDescriptListCardholder[] = {
	{  1,   1, 1, 4, 0, "Actual unit type" },
	{  2,   1, 3, 4, 0, "Expected unit type" },
	{  3,   1, 3, 4, 0, "Expected port count" },
	{  4,  20, 3, 5, 0, "Expected equipment id" },
	{  5,  20, 3, 5, 0, "Actual equipment id" },
	{  6,   2, 1, 1, 0, "Protection pointer" },
	{  7,   1, 1, 6, 0, "Protection switch" },
	{  8,   1, 3, 4, 0, "ARC" },
	{  9,   1, 3, 4, 0, "ARC interval" },
};

static const struct omci_attr_def omciAttriDescriptListCircuitPack[] = {
	{  1,   1, 1, 4, 0, "Type" },
	{  2,   1, 1, 4, 0, "Number of ports" },
	{  3,   8, 1, 5, 0, "Serial number" },
	{  4,  14, 1, 5, 0, "Version" },
	{  5,   4, 1, 5, 0, "Vendor id" },
	{  6,   1, 3, 4, 0, "Administrative state" },
	{  7,   1, 1, 4, 0, "Operational state" },
	{  8,   1, 3, 4, 0, "Bridged or IP ind" },
	{  9,  20, 1, 5, 0, "Equipment id" },
	{ 10,   1, 3, 4, 0, "Card configuration" },
	{ 11,   1, 1, 4, 0, "Total T-CONT buffer" },
	{ 12,   1, 1, 4, 0, "Total priority queue" },
	{ 13,   1, 1, 4, 0, "Total traffic scheduler" },
	{ 14,   4, 3, 2, 0, "Power shed override" },
};

static const struct omci_attr_def omciAttriDescriptListOltG[] = {
	{  1,   4, 3, 4, 0, "OLT vendor id" },
	{  2,  20, 3, 5, 0, "Equipment id" },
	{  3,  14, 3, 5, 0, "Version" },
	{  4,  14, 3, 5, 0, "Time of day" },
};

static const struct omci_attr_def omciAttriDescriptListExtensionPackage[] = {
	{  1,   2, 3, 4, 0, "Environmental sense" },
	{  2,   2, 3, 4, 0, "Contact closure" },
};

static const struct omci_attr_def omciAttriDescriptListOnuG[] = {
	{  1,   4, 1, 5, 0, "Vendor id" },
	{  2,  14, 1, 5, 0, "Version" },
	{  3,   8, 1, 5, 0, "Serial number" },
	{  4,   1, 1, 6, 0, "TM option" },
	{  5,   1, 1, 4, 0, "Deprecated" },
	{  6,   1, 3, 4, 0, "Battery backup" },
	{  7,   1, 3, 4, 0, "Administrative state" },
	{  8,   1, 1, 4, OMCI_ATTR_AVC, "Operational state" },
	{  9,   1, 1, 4, 0, "ONU survival time" },
	{ 10,  24, 1, 5, OMCI_ATTR_AVC, "Logical ONU ID" },
	{ 11,  12, 1, 5, OMCI_ATTR_AVC, "Logical password" },
	{ 12,   1, 3, 6, 0, "Credentials status" },
	{ 13,   1, 1, 2, 0, "Extended TC-layer" },
};

static const struct omci_attr_def omciAttriDescriptListOnu2G[] = {
	{  1,  20, 1, 5, 0, "Equipment id" },
	{  2,   1, 1, 6, OMCI_ATTR_AVC, "OMCC version" },
	{  3,   2, 1, 5, 0, "Vendor product code" },
	{  4,   1, 1, 6, 0, "Security capability" },
	{  5,   1, 3, 6, 0, "Security mode" },
	{  6,   2, 1, 4, 0, "Total priority queue" },
	{  7,   1, 1, 4, 0, "Total traffic scheduler" },
	{  8,   1, 1, 4, 0, "Deprecated" },
	{  9,   2, 1, 4, 0, "Total GEM port-ID" },
	{ 10,   4, 1, 4, 0, "SysUpTime" },
	{ 11,   2, 1, 2, 0, "Connectivity capability" },
	{ 12,   1, 3, 2, 0, "Connectivity mode" },
	{ 13,   2, 1, 2, 0, "QoS configuration" },
	{ 14,   2, 3, 4, 0, "PQ scale factor" },
};

static const struct omci_attr_def omciAttriDescriptListTcont[] = {
	{  1,   2, 3, 4, 0, "Alloc-ID" },
	{  2,   1, 1, 4, 0, "Deprecated" },
	{  3,   1, 3, 4, 0, "Policy" },
};

static const struct omci_attr_def omciAttriDescriptListAniG[] = {
	{  1,   1, 1, 4, 0, "SR indication" },
	{  2,   2, 1, 4, 0, "Total T-CONT number" },
	{  3,   2, 3, 4, 0, "GEM block length" },
	{  4,   1, 1, 6, 0, "Piggyback DBA report" },
	{  5,   1, 1, 4, 0, "Deprecated" },
	{  6,   1, 3, 4, 0, "SF threshold" },
	{  7,   1, 3, 4, 0, "SD threshold" },
	{  8,   1, 3, 4, OMCI_ATTR_AVC, "ARC" },
	{  9,   1, 3, 4, 0, "ARC interval" },
	{ 10,   2, 1, 4, 0, "Optical signal level" },
	{ 11,   1, 3, 4, 0, "Lower optical threshold" },
	{ 12,   1, 3, 4, 0, "Upper optical threshold" },
	{ 13,   2, 1, 4, 0, "ONU response time" },
	{ 14,   2, 1, 4, 0, "Transmit optical level" },
	{ 15,   1, 3, 4, 0, "Lower transmit optical" },
	{ 16,   1, 3, 4, 0, "Upper transmit optical" },
};

static const struct omci_attr_def omciAttriDescriptListGemPortCtp[] = {
	{  1,   2, 7, 4, 0, "Port-ID" },
	{  2,   2, 7, 4, 0, "T-CONT pointer" },
	{  3,   1, 7, 4, 0, "Direction" },
	{  4,   2, 7, 4, 0, "TMPointerUp" },
	{  5,   2, 7, 4, 0, "TDPointerUp" },
	{  6,   1, 1, 4, 0, "UNIcounter" },
	{  7,   2, 7, 4, 0, "PQPointerDown" },
	{  8,   1, 1, 4, 0, "Encryption state" },
	{  9,   2, 7, 4, 0, "TDPointerDown" },
	{ 10,   2, 3, 4, 0, "Encryption key ring" },
};

static const struct omci_attr_def omciAttriDescriptListThresholdData1[] = {
	{  1,   4, 7, 4, 0, "Threshold value 1" },
	{  2,   4, 7, 4, 0, "Threshold value 2" },
	{  3,   4, 7, 4, 0, "Threshold value 3" },
	{  4,   4, 7, 4, 0, "Threshold value 4" },
	{  5,   4, 7, 4, 0, "Threshold value 5" },
	{  6,   4, 7, 4, 0, "Threshold value 6" },
	{  7,   4, 7, 4, 0, "Threshold value 7" },
};

static const struct omci_attr_def omciAttriDescriptListThresholdData2[] = {
	{  1,   4, 7, 4, 0, "Threshold value 8" },
	{  2,   4, 7, 4, 0, "Threshold value 9" },
	{  3,   4, 7, 4, 0, "Threshold value 10" },
	{  4,   4, 7, 4, 0, "Threshold value 11" },
	{  5,   4, 7, 4, 0, "Threshold value 12" },
	{  6,   4, 7, 4, 0, "Threshold value 13" },
	{  7,   4, 7, 4, 0, "Threshold value 14" },
};

static const struct omci_attr_def omciAttriDescriptListOmci[] = {
	{  1,   0, 1, 7, 0, "ME type table" },
	{  2,   0, 1, 7, 0, "Message type table" },
};

static const struct omci_attr_def omciAttriDescriptListSoftwareImage[] = {
	{  1,  14, 1, 5, 0, "Version" },
	{  2,   1, 1, 4, 0, "Is committed" },
	{  3,   1, 1, 4, 0, "Is active" },
	{  4,   1, 1, 4, 0, "Is valid" },
	{  5,  25, 1, 5, 0, "Product code" },
	{  6,  16, 1, 4, 0, "Image hash" },
};

static const struct omci_attr_def omciAttriDescriptListMBEthnetFrameExtendedPM[] = {
	{  1,   1, 1, 4, 0, "interval end time" },
	{  2,  16, 1, 4, 0, "control block" },
	{  3,   4, 1, 4, 0, "drop events" },
	{  4,   4, 1, 4, 0, "octets" },
	{  5,   4, 1, 4, 0, "frames" },
	{  6,   4, 1, 4, 0, "broadcast frames" },
	{  7,   4, 1, 4, 0, "multicast frames" },
	{  8,   4, 1, 4, 0, "crc errored frames" },
	{  9,   4, 1, 4, 0, "undersize frames" },
	{ 10,   4, 1, 4, 0, "oversize frames" },
	{ 11,   4, 1, 4, 0, "frames 64 octets" },
	{ 12,   4, 1, 4, 0, "frames 65 to 127 octets" },
	{ 13,   4, 1, 4, 0, "frames 128 to 255 octets" },
	{ 14,   4, 1, 4, 0, "frames 256 to 511 octets" },
	{ 15,   4, 1, 4, 0, "frames 512 to 1023 octets" },
	{ 16,   4, 1, 4, 0, "frames 1024 to 1518 octets" },
};

static const struct omci_attr_def omciAttriDescriptListMBServiceProfile[] = {
	{  1,   1, 7, 4, 0, "Spanning tree ind" },
	{  2,   1, 7, 4, 0, "Learning ind" },
	{  3,   1, 7, 4, 0, "Port bridging ind" },
	{  4,   2, 7, 4, 0, "Priority" },
	{  5,   2, 7, 4, 0, "Max age" },
	{  6,   2, 7, 4, 0, "Hello time" },
	{  7,   2, 7, 4, 0, "Forward delay" },
	{  8,   1, 7, 4, 0, "Unknown MAC addr discard" },
	{  9,   1, 7, 4, 0, "MAC learning depth" },
	{ 10,   4, 7, 4, 0, "Dynamic filter age time" },
};

static const struct omci_attr_def omciAttriDescriptListMBConnData[] = {
	{  1,   6, 1, 4, 0, "Bridge MAC address" },
	{  2,   2, 1, 4, 0, "Bridge priority" },
	{  3,   8, 1, 4, 0, "Designated root" },
	{  4,   4, 1, 4, 0, "Root path cost" },
	{  5,   1, 1, 4, 0, "Bridge port count" },
	{  6,   2, 1, 4, 0, "Root port num" },
	{  7,   2, 1, 4, 0, "Hello time" },
	{  8,   2, 1, 4, 0, "Forward delay" },
};

static const struct omci_attr_def omciAttriDescriptListMBPortConData[] = {
	{  1,   2, 7, 1, 0, "Bridge id pointer" },
	{  2,   1, 7, 4, 0, "Port num" },
	{  3,   1, 7, 4, 0, "TP type" },
	{  4,   2, 7, 1, 0, "TP pointer" },
	{  5,   2, 7, 4, 0, "Port priority" },
	{  6,   2, 7, 4, 0, "Port path cost" },
	{  7,   1, 7, 4, 0, "Port ST ind" },
	{  8,   1, 7, 4, 0, "Deprecated 1" },
	{  9,   1, 7, 4, 0, "Deprecated 2" },
	{ 10,   6, 1, 4, 0, "Port MAC Address" },
	{ 11,   2, 3, 1, 0, "Outbound TD pointer" },
	{ 12,   2, 3, 1, 0, "Inbound TD pointer" },
	{ 13,   1, 7, 4, 0, "MAC learning depth" },
};

static const struct omci_attr_def omciAttriDescriptListMBPortData[] = {
	{  1,   8, 1, 7, 0, "Bridge table" },
};

static const struct omci_attr_def omciAttriDescriptListMBPortPMHD[] = {
	{  1,   1, 1, 4, 0, "Interval end time" },
	{  2,   2, 7, 4, 0, "Threshold data 1/2 id" },
	{  3,   4, 1, 4, 0, "Forwarded frame cnt" },
	{  4,   4, 1, 4, 0, "Delay exceeded discard cnt" },
	{  5,   4, 1, 4, 0, "MTU exceeded discard cnt" },
	{  6,   4, 1, 4, 0, "Received frame cnt" },
	{  7,   4, 1, 4, 0, "Received and discarded cnt" },
};

static const struct omci_attr_def omciAttriDescriptList8021pMapperProfile[] = {
	{  1,   2, 7, 1, 0, "TP pointer" },
	{  2,   2, 7, 1, 0, "TP pointer for P-bit 0" },
	{  3,   2, 7, 1, 0, "TP pointer for P-bit 1" },
	{  4,   2, 7, 1, 0, "TP pointer for P-bit 2" },
	{  5,   2, 7, 1, 0, "TP pointer for P-bit 3" },
	{  6,   2, 7, 1, 0, "TP pointer for P-bit 4" },
	{  7,   2, 7, 1, 0, "TP pointer for P-bit 5" },
	{  8,   2, 7, 1, 0, "TP pointer for P-bit 6" },
	{  9,   2, 7, 1, 0, "TP pointer for P-bit 7" },
	{ 10,   1, 7, 4, 0, "Unmarked frame option" },
	{ 11,  24, 3, 5, 0, "DSCP to P bit mapping" },
	{ 12,   1, 7, 4, 0, "Default P bit assumption" },
	{ 13,   1, 7, 4, 0, "TP type" },
};

static const struct omci_attr_def omciAttriDescriptListVlanTagFilterData[] = {
	{  1,  24, 7, 5, 0, "VLAN filter list" },
	{  2,   1, 7, 4, 0, "Forward operation" },
	{  3,   1, 7, 4, 0, "Number of entries" },
};

static const struct omci_attr_def omciAttriDescriptListVlanTagExtendOperation[] = {
	{  1,   1, 7, 4, 0, "Association type" },
	{  2,   2, 1, 4, 0, "VLAN tag table Max size" },
	{  3,   2, 3, 4, 0, "Input TPID" },
	{  4,   2, 3, 4, 0, "Output TPID" },
	{  5,   1, 3, 4, 0, "Downstream mode" },
	{  6,  16, 3, 7, 0, "VLAN tag operation table" },
	{  7,   2, 7, 4, 0, "Associated ME pointer" },
	{  8,  24, 3, 4, 0, "DSCP to P bit mapping" },
};

static const struct omci_attr_def omciAttriDescriptListMulticastProfile[] = {
	{  1,   1, 7, 4, 0, "IGMP version" },
	{  2,   1, 7, 4, 0, "IGMP function" },
	{  3,   1, 7, 4, 0, "immediate leave" },
	{  4,   2, 7, 4, 0, "Upstream IGMP TCI" },
	{  5,   1, 7, 4, 0, "Upsteam IGMP tag control" },
	{  6,   4, 7, 4, 0, "Upstream IGMP rate" },
	{  7,  24, 3, 7, 0, "Dynamic access control table" },
	{  8,  24, 3, 7, 0, "static access control table" },
	{  9,  10, 1, 7, 0, "Lost groups list table" },
	{ 10,   1, 7, 4, 0, "Robustness" },
	{ 11,   4, 7, 4, 0, "Querier IP address" },
	{ 12,   4, 7, 4, 0, "Query interval" },
	{ 13,   4, 7, 4, 0, "Query max response time" },
	{ 14,   4, 3, 4, 0, "Last member query interval" },
	{ 15,   1, 3, 4, 0, "Unauthorized join request" },
	{ 16,   3, 7, 4, 0, "Downstream IGMP and TCI" },
};

static const struct omci_attr_def omciAttriDescriptListMulticasSubscribertCfgInfo[] = {
	{  1,   1, 7, 4, 0, "ME type" },
	{  2,   2, 7, 1, 0, "operations profile pointer" },
	{  3,   2, 7, 4, 0, "Max simultaneous groups" },
	{  4,   4, 7, 4, 0, "Max multicast bandwidth" },
	{  5,   1, 7, 4, 0, "Bandwidth enforcement" },
	{  6,  20, 3, 7, 0, "service package table" },
	{  7,  22, 3, 7, 0, "preview groups table" },
};

static const struct omci_attr_def omciAttriDescriptListMulticastSubscriberMonitor[] = {
	{  1,   1, 7, 4, 0, "ME type" },
	{  2,   4, 1, 4, 0, "Current multicast bandwidth" },
	{  3,   4, 1, 4, 0, "Join messages counter" },
	{  4,   4, 1, 4, 0, "Bandwidth exceeded counter" },
	{  5,  24, 1, 7, 0, "IPv4 active group table" },
	{  6,  58, 1, 7, 0, "IPv6 active group table" },
};

static const struct omci_attr_def omciAttriDescriptList_EthFrameUpPMHD[] = {
	{  1,   1, 1, 4, 0, "Interval end time" },
	{  2,   2, 7, 4, 0, "Threshold data 1/2 id" },
	{  3,   4, 1, 4, 0, "Drop events" },
	{  4,   4, 1, 4, 0, "Octets" },
	{  5,   4, 1, 4, 0, "Packets" },
	{  6,   4, 1, 4, 0, "Broadcast packets" },
	{  7,   4, 1, 4, 0, "Multicast packets" },
	{  8,   4, 1, 4, 0, "CRC errored packets" },
	{  9,   4, 1, 4, 0, "Undersize packets" },
	{ 10,   4, 1, 4, 0, "Oversize packets" },
	{ 11,   4, 1, 4, 0, "Packets 64 octets" },
	{ 12,   4, 1, 4, 0, "Packets 65~127 octets" },
	{ 13,   4, 1, 4, 0, "Packets 128~255 octets" },
	{ 14,   4, 1, 4, 0, "Packets 256~511 octets" },
	{ 15,   4, 1, 4, 0, "Packets 512~1023 octets" },
	{ 16,   4, 1, 4, 0, "Packets 1024~1518 octets" },
};

static const struct omci_attr_def omciAttriDescriptListEthFrameDownPMHD[] = {
	{  1,   1, 1, 4, 0, "Interval end time" },
	{  2,   2, 7, 4, 0, "Threshold data 1/2 id" },
	{  3,   4, 1, 4, 0, "Drop events" },
	{  4,   4, 1, 4, 0, "Octets" },
	{  5,   4, 1, 4, 0, "Packets" },
	{  6,   4, 1, 4, 0, "Broadcast packets" },
	{  7,   4, 1, 4, 0, "Multicast packets" },
	{  8,   4, 1, 4, 0, "CRC errored packets" },
	{  9,   4, 1, 4, 0, "Undersize packets" },
	{ 10,   4, 1, 4, 0, "Oversize packets" },
	{ 11,   4, 1, 4, 0, "Packets 64 octets" },
	{ 12,   4, 1, 4, 0, "Packets 65~127 octets" },
	{ 13,   4, 1, 4, 0, "Packets 128~255 octets" },
	{ 14,   4, 1, 4, 0, "Packets 256~511 octets" },
	{ 15,   4, 1, 4, 0, "Packets 512~1023 octets" },
	{ 16,   4, 1, 4, 0, "Packets 1024~1518 octets" },
};

static const struct omci_attr_def omciAttriDescriptListPriorityQueue[] = {
	{  1,   1, 1, 4, 0, "Queue cofig option" },
	{  2,   2, 1, 4, 0, "Max queue size" },
	{  3,   2, 3, 4, 0, "Allocated queue size" },
	{  4,   2, 3, 4, 0, "Discard counter reset interval" },
	{  5,   2, 3, 4, 0, "Threshold for discarded blocks" },
	{  6,   4, 3, 4, 0, "Related port" },
	{  7,   2, 3, 4, 0, "Traffic scheduler pointer" },
	{  8,   1, 3, 4, 0, "Weight" },
	{  9,   2, 3, 4, 0, "Back pressure operation" },
	{ 10,   4, 3, 4, 0, "Back pressure time" },
	{ 11,   2, 3, 4, 0, "Back pressure occur threshold" },
	{ 12,   2, 3, 4, 0, "Back pressure clear threshold" },
	{ 13,   8, 3, 4, 0, "Packet drop thresholds" },
	{ 14,   2, 3, 4, 0, "Packet drop max_p" },
	{ 15,   1, 3, 4, 0, "Queue drop w_q" },
	{ 16,   1, 3, 4, 0, "Drop precedence colour marking" },
};

static const struct omci_attr_def omciAttriDescriptListTrafficScheduler[] = {
	{  1,   2, 3, 4, 0, "T-CONT pointer" },
	{  2,   2, 1, 4, 0, "Traffic Scheduler pointer" },
	{  3,   1, 3, 4, 0, "Policy" },
	{  4,   1, 3, 4, 0, "Priority_Weight" },
};

static const struct omci_attr_def omciAttriDescriptListTrafficDescriptor[] = {
	{  1,   4, 7, 4, 0, "CIR" },
	{  2,   4, 7, 4, 0, "PIR" },
	{  3,   4, 7, 4, 0, "CBS" },
	{  4,   4, 7, 4, 0, "PBS" },
	{  5,   1, 7, 4, 0, "Colour mode" },
	{  6,   1, 7, 4, 0, "Ingress colour marking" },
	{  7,   1, 7, 4, 0, "Egress colour marking" },
	{  8,   1, 5, 4, 0, "Meter type" },
};

static const struct omci_attr_def omciAttriDescriptListGemportNetworkCtpPm[] = {
	{  1,   1, 1, 4, 0, "Interval end time" },
	{  2,   2, 7, 4, 0, "Threshold data 1/2 id" },
	{  3,   4, 1, 4, 0, "Transmitted GEM frames" },
	{  4,   4, 1, 4, 0, "Received GEM frames" },
	{  5,   8, 1, 4, 0, "Received payload bytes" },
	{  6,   8, 1, 4, 0, "Transmitted payload bytes" },
	{  7,   8, 1, 4, 0, "Encryption key errors" },
};

static const struct omci_attr_def omciAttriDescriptListGALEthernetProfile[] = {
	{  1,   2, 7, 4, 0, "max gem payload size" },
};

static const struct omci_attr_def omciAttriDescriptListFecPM[] = {
	{  1,   1, 1, 4, 0, "interval end time" },
	{  2,   2, 7, 1, 0, "threshold data 1/2 id" },
	{  3,   4, 1, 4, 0, "corrected bytes" },
	{  4,   4, 1, 4, 0, "corrected code words" },
	{  5,   4, 1, 4, 0, "uncorrected code words" },
	{  6,   4, 1, 4, 0, "total code words" },
	{  7,   2, 1, 4, 0, "fec seconds" },
};

static const struct omci_attr_def omciAttriDescriptListGemIWTP[] = {
	{  1,   2, 7, 1, 0, "gem port network ctp conn ptr" },
	{  2,   1, 7, 6, 0, "iw option" },
	{  3,   2, 7, 1, 0, "service profile ptr" },
	{  4,   2, 7, 1, 0, "iw termination point ptr" },
	{  5,   1, 1, 4, 0, "pptp counter" },
	{  6,   1, 1, 4, OMCI_ATTR_AVC, "operational state" },
	{  7,   2, 7, 1, 0, "gal profile ptr" },
	{  8,   1, 3, 6, 0, "gal loopback cfg" },
};

static const struct omci_attr_def omciAttriDescriptListMulticastGemIWTP[] = {
	{  1,   2, 7, 1, 0, "gem port network ctp conn ptr" },
	{  2,   1, 7, 6, 0, "iw option" },
	{  3,   2, 7, 1, 0, "service profile ptr" },
	{  4,   2, 7, 4, 0, "not used 1" },
	{  5,   1, 1, 4, 0, "pptp counter" },
	{  6,   1, 1, 4, OMCI_ATTR_AVC, "operational state" },
	{  7,   2, 7, 1, 0, "gal profile ptr" },
	{  8,   1, 7, 4, 0, "not used 2" },
	{  9,  12, 3, 7, 0, "ipv4 multicast addr table" },
};

static const struct omci_attr_def omciAttriDescriptListDot1agMaintenanceDomain[] = {
	{  1,   1, 7, 4, 0, "MD level" },
	{  2,   1, 7, 6, 0, "MD name format" },
	{  3,  25, 3, 5, 0, "MD name 1" },
	{  4,  25, 3, 5, 0, "MD name 2" },
	{  5,   1, 7, 6, 0, "MHF creation" },
	{  6,   1, 7, 6, 0, "sender id permission" },
};

static const struct omci_attr_def omciAttriDescriptListDot1agMaintenanceAssociation[] = {
	{  1,   2, 7, 1, 0, "MD pointer" },
	{  2,   1, 7, 6, 0, "short MA name format" },
	{  3,  25, 3, 5, 0, "short MA name 1" },
	{  4,  25, 3, 5, 0, "short MA name 2" },
	{  5,   1, 7, 6, 0, "CCM interval" },
	{  6,  24, 3, 5, 0, "associated vlans" },
	{  7,   1, 7, 6, 0, "MHF creation" },
	{  8,   1, 7, 6, 0, "sender id permission" },
};

static const struct omci_attr_def omciAttriDescriptListPPTPEthernetUNI[] = {
	{  1,   1, 3, 4, 0, "expected type" },
	{  2,   1, 1, 4, OMCI_ATTR_AVC, "sensed type" },
	{  3,   1, 3, 6, 0, "auto detection config" },
	{  4,   1, 3, 6, 0, "ethernet loopback config" },
	{  5,   1, 3, 6, 0, "admin state" },
	{  6,   1, 1, 6, OMCI_ATTR_AVC, "operational state" },
	{  7,   1, 1, 6, 0, "configuration ind" },
	{  8,   2, 3, 4, 0, "max frame size" },
	{  9,   1, 3, 6, 0, "dte or dce ind" },
	{ 10,   2, 3, 4, 0, "pause time" },
	{ 11,   1, 3, 6, 0, "Bridged or IP ind" },
	{ 12,   1, 3, 4, OMCI_ATTR_AVC, "ARC" },
	{ 13,   1, 3, 4, 0, "ARC interval" },
	{ 14,   1, 3, 6, 0, "pppoe filter" },
	{ 15,   1, 3, 6, 0, "power control" },
};

static const struct omci_attr_def omciAttriDescriptListEthernetPMHistoryData[] = {
	{  1,   1, 1, 4, 0, "interval end time" },
	{  2,   2, 7, 4, 0, "threshold data 1/2 id" },
	{  3,   4, 1, 4, 0, "fcs errors" },
	{  4,   4, 1, 4, 0, "excessive collision cnt" },
	{  5,   4, 1, 4, 0, "late collision cnt" },
	{  6,   4, 1, 4, 0, "frames too long" },
	{  7,   4, 1, 4, 0, "Buffer overflows on receive" },
	{  8,   4, 1, 4, 0, "Buffer overflows on transmit" },
	{  9,   4, 1, 4, 0, "single collision frame cnt" },
	{ 10,   4, 1, 4, 0, "multi collisions frame cnt" },
	{ 11,   4, 1, 4, 0, "sqe counter" },
	{ 12,   4, 1, 4, 0, "deferred transmission cnt" },
	{ 13,   4, 1, 4, 0, "internal mac trans error cnt" },
	{ 14,   4, 1, 4, 0, "carrier sense error cnt" },
	{ 15,   4, 1, 4, 0, "alignment error cnt" },
	{ 16,   4, 1, 4, 0, "internal mac recv error cnt" },
};

static const struct omci_attr_def omciAttriDescriptListEthernetPMHistoryData2[] = {
	{  1,   1, 1, 4, 0, "interval end time" },
	{  2,   2, 7, 4, 0, "threshold data 1/2 id" },
	{  3,   4, 1, 4, 0, "pppoe filtered frame cnt" },
};

static const struct omci_attr_def omciAttriDescriptListEthernetPMHistoryData3[] = {
	{  1,   1, 1, 4, 0, "interval end time" },
	{  2,   2, 7, 4, 0, "threshold data 1/2 id" },
	{  3,   4, 1, 4, 0, "drop events" },
	{  4,   4, 1, 4, 0, "octets" },
	{  5,   4, 1, 4, 0, "packets" },
	{  6,   4, 1, 4, 0, "broadcast packets" },
	{  7,   4, 1, 4, 0, "multicast packets" },
	{  8,   4, 1, 4, 0, "undersize packets" },
	{  9,   4, 1, 4, 0, "fragments" },
	{ 10,   4, 1, 4, 0, "jabbers" },
	{ 11,   4, 1, 4, 0, "packets 64 octets" },
	{ 12,   4, 1, 4, 0, "packets 65 to 127 octets" },
	{ 13,   4, 1, 4, 0, "packets 128 to 255 octets" },
	{ 14,   4, 1, 4, 0, "packets 256 to 511 octets" },
	{ 15,   4, 1, 4, 0, "packets 512 to 1023 octets" },
	{ 16,   4, 1, 4, 0, "packets 1024 to 1518 octets" },
};

static const struct omci_attr_def omciAttriDescriptListEthernetFrameExtendedPM64bit[] = {
	{  1,   1, 1, 4, 0, "interval end time" },
	{  2,  16, 7, 4, 0, "control block" },
	{  3,   8, 1, 4, 0, "drop events" },
	{  4,   8, 1, 4, 0, "octets" },
	{  5,   8, 1, 4, 0, "frames" },
	{  6,   8, 1, 4, 0, "broadcast frames" },
	{  7,   8, 1, 4, 0, "multicast frames" },
	{  8,   8, 1, 4, 0, "CRC errored frames" },
	{  9,   8, 1, 4, 0, "undersize frames" },
	{ 10,   8, 1, 4, 0, "oversize frames" },
	{ 11,   8, 1, 4, 0, "frames 64 octets" },
	{ 12,   8, 1, 4, 0, "frames 65 to 127 octets" },
	{ 13,   8, 1, 4, 0, "frames 128 to 255 octets" },
	{ 14,   8, 1, 4, 0, "frames 256 to 511 octets" },
	{ 15,   8, 1, 4, 0, "frames 512 to 1023 octets" },
	{ 16,   8, 1, 4, 0, "frames 1024 to 1518 octets" },
};

static const struct omci_attr_def omciAttriDescriptListVirtualEthernetInterfacePoint[] = {
	{  1,   1, 3, 6, 0, "admin state" },
	{  2,   1, 1, 6, OMCI_ATTR_AVC, "operational state" },
	{  3,  25, 3, 5, 0, "interdomain name" },
	{  4,   2, 3, 1, 0, "tcp/udp pointer" },
	{  5,   2, 1, 4, 0, "iana assigned port" },
};

static const struct omci_attr_def omciAttriDescriptListIPHostConfigData[] = {
	{  1,   1, 3, 2, 0, "ip options" },
	{  2,   6, 1, 5, 0, "mac addr" },
	{  3,  25, 3, 5, 0, "onu identifier" },
	{  4,   4, 3, 5, 0, "ip addr" },
	{  5,   4, 3, 5, 0, "mask" },
	{  6,   4, 3, 5, 0, "gateway" },
	{  7,   4, 3, 5, 0, "primary dns" },
	{  8,   4, 3, 5, 0, "secondary dns" },
	{  9,   4, 1, 5, OMCI_ATTR_AVC, "current addr" },
	{ 10,   4, 1, 5, OMCI_ATTR_AVC, "current mask" },
	{ 11,   4, 1, 5, OMCI_ATTR_AVC, "current gateway" },
	{ 12,   4, 1, 5, OMCI_ATTR_AVC, "current primary dns" },
	{ 13,   4, 1, 5, OMCI_ATTR_AVC, "current secondary dns" },
	{ 14,  25, 1, 5, OMCI_ATTR_AVC, "domain name" },
	{ 15,  25, 1, 5, OMCI_ATTR_AVC, "host name" },
};

static const struct omci_attr_def omciAttriDescriptListTcpUdpConfigData[] = {
	{  1,   2, 7, 4, 0, "port id" },
	{  2,   1, 7, 4, 0, "protocol" },
	{  3,   1, 7, 6, 0, "tos or diffserv field" },
	{  4,   2, 7, 1, 0, "ip host pointer" },
};

static const uint8_t meMapTableOnuG[] = {
	 0,	/* OMCI_ALARM_ID_EQUIPMENT */
	 6,	/* OMCI_ALARM_ID_ONU_SELFTEST_FAIL */
	 7,	/* OMCI_ALARM_ID_ONU_DYING_GASP */
	 8,	/* OMCI_ALARM_ID_ONU_TEMPER_YELLOW */
	 9,	/* OMCI_ALARM_ID_ONU_TEMPER_RED */
	10,	/* OMCI_ALARM_ID_ONU_VOLTAGE_YELLOW */
	11,	/* OMCI_ALARM_ID_ONU_VOLTAGE_RED */
	12,	/* OMCI_ALARM_ID_ONU_ONU_MANANA_POWER_OFF */
	13,	/* OMCI_ALARM_ID_ONU_INV_IMAGE */
};

static const uint8_t meMapTableAniG[] = {
	 0,	/* OMCI_ALARM_ID_LOW_RX_OPTICAL */
	 1,	/* OMCI_ALARM_ID_HIGH_RX_OPTICAL */
	 2,	/* OMCI_ALARM_ID_SF */
	 3,	/* OMCI_ALARM_ID_SD */
	 4,	/* OMCI_ALARM_ID_LOW_TX_OPTICAL */
	 5,	/* OMCI_ALARM_ID_HIGH_TX_OPTICAL */
	 6,	/* OMCI_ALARM_ID_LASER_BIAS_CURRENT */
};

static const uint8_t alarmIdTablePPTPEthernetUNI[] = {
	 0,	/* OMCI_ALARM_ID_PPTPEthernetUNI_LAN_LOS_ALARM */
};

static const uint8_t alarmIdTableVirtualEthernetInterfacePoint[] = {
	 0,	/* OMCI_ALARM_ID_VEIP_CONNECTING_FUNCTION_FAIL */
};

static const struct omci_attr_def omciAttriDescriptListMBPMHD[] = {
	{  1,   1, 1, 4, 0, "Interval end time" },
	{  2,   2, 7, 1, 0, "Threshold data 1/2 id" },
	{  3,   4, 1, 4, 0, "entry discard count" },
};

static const struct omci_attr_def omciAttriDescriptListVlanTagOperation[] = {
	{  1,   1, 7, 4, 0, "Upsream tag operation mode" },
	{  2,   2, 7, 4, 0, "Upstream TCI Value" },
	{  3,   1, 7, 4, 0, "Downstream tag operation mode" },
	{  4,   1, 7, 4, 0, "Association type" },
	{  5,   2, 7, 4, 0, "Associated ME pointer" },
};

static const struct omci_attr_def omciAttriDescriptListIPHostPM[] = {
	{  1,   1, 1, 4, 0, "interval end time" },
	{  2,   2, 7, 1, 0, "threshold data 1/2 id" },
	{  3,   4, 1, 4, 0, "icmp errors" },
	{  4,   4, 1, 4, 0, "dns errors" },
	{  5,   2, 1, 4, 0, "dhcp timeouts" },
	{  6,   2, 1, 4, 0, "ip addr conflict" },
	{  7,   2, 1, 4, 0, "out of memory" },
	{  8,   2, 1, 4, 0, "internal error" },
};

static const struct omci_attr_def omciAttriDescriptListProtectionProfile[] = {
	{  1,   1, 7, 4, 0, "Protect slot 1" },
	{  2,   1, 7, 4, 0, "Protect slot 2" },
	{  3,   1, 7, 4, 0, "Working slot 1" },
	{  4,   1, 7, 4, 0, "Working slot 2" },
	{  5,   1, 7, 4, 0, "Working slot 3" },
	{  6,   1, 7, 4, 0, "Working slot 4" },
	{  7,   1, 7, 4, 0, "Working slot 5" },
	{  8,   1, 7, 4, 0, "Working slot 6" },
	{  9,   1, 7, 4, 0, "Working slot 7" },
	{ 10,   1, 7, 4, 0, "Working slot 8" },
	{ 11,   1, 7, 4, 0, "Protect status 1" },
	{ 12,   1, 7, 4, 0, "Protect status 2" },
	{ 13,   1, 7, 4, 0, "Revertive ind" },
	{ 14,   1, 7, 4, 0, "Wait to restore time" },
};

static const struct omci_attr_def omciAttriDescriptListUniG[] = {
	{  1,   2, 3, 4, 0, "Deprecated" },
	{  2,   1, 3, 4, 0, "Administrative state" },
	{  3,   1, 1, 6, 0, "Management capability" },
	{  4,   2, 3, 1, 0, "Non-OMCI manageid" },
	{  5,   2, 3, 1, 0, "Relay Agent Options" },
};

static const struct omci_attr_def omciAttriDescriptListGemPortPm[] = {
	{  1,   1, 1, 4, 0, "Interval end time" },
	{  2,   2, 7, 4, 0, "Threshold data 1/2 id" },
	{  3,   4, 1, 4, 0, "Lost packets" },
	{  4,   4, 1, 4, 0, "Misinserted packets" },
	{  5,   5, 1, 4, 0, "Received packets" },
	{  6,   5, 1, 4, 0, "Received blocks" },
	{  7,   5, 1, 4, 0, "Transmitted blocks" },
	{  8,   4, 1, 4, 0, "Impaired blocks" },
};

static const struct omci_attr_def omciAttriDescriptListGALEthernetPM[] = {
	{  1,   1, 1, 4, 0, "interval end time" },
	{  2,   2, 7, 1, 0, "threshold data 1/2 id" },
	{  3,   4, 1, 4, 0, "discarded frames" },
};

static const struct omci_attr_def omciAttriDescriptListProtectionData[] = {
	{  1,   2, 1, 4, 0, "Working pointer" },
	{  2,   2, 1, 4, 0, "Protection pointer" },
	{  3,   1, 1, 4, 0, "Protection type" },
	{  4,   1, 1, 4, 0, "Revertive ind" },
	{  5,   2, 3, 4, 0, "Wait to restore time" },
	{  6,   2, 3, 4, 0, "Switching guard time" },
};

static const struct omci_attr_def omciAttriDescriptListTcpUdpPM[] = {
	{  1,   1, 1, 4, 0, "interval end time" },
	{  2,   2, 7, 1, 0, "threshold data 1/2 id" },
	{  3,   2, 1, 4, 0, "socket failed" },
	{  4,   2, 1, 4, 0, "listen failed" },
	{  5,   2, 1, 4, 0, "bind failed" },
	{  6,   2, 1, 4, 0, "accept failed" },
	{  7,   2, 1, 4, 0, "select failed" },
};

static const struct omci_attr_def omciAttriDescriptListEnergyConsumptionPm[] = {
	{  1,   1, 1, 4, 0, "Interval end time" },
	{  2,   2, 7, 4, 0, "Threshold data 1/2 id" },
	{  3,   4, 1, 4, 0, "Doze time" },
	{  4,   4, 1, 4, 0, "Cyclic sleep time" },
	{  5,   4, 1, 4, 0, "Energy consumed" },
};

static const struct omci_attr_def omciAttriDescriptListMBPortDesignData[] = {
	{  1,  24, 1, 4, 0, "bridge root cost port" },
	{  2,   1, 1, 4, 0, "Port state" },
};

static const struct omci_attr_def omciAttriDescriptListNetworkAddress[] = {
	{  1,   2, 7, 1, 0, "Security pointer" },
	{  2,   2, 7, 1, 0, "Address pointer" },
};

static const struct omci_attr_def omciAttriDescriptListAuthenticationMethod[] = {
	{  1,   1, 3, 4, 0, "Validation scheme" },
	{  2,  25, 3, 5, 0, "Username 1" },
	{  3,  25, 3, 5, 0, "Password" },
	{  4,  25, 3, 5, 0, "Realm" },
	{  5,  25, 3, 5, 0, "Username 2" },
};

static const struct omci_attr_def omciAttriDescriptListLargeString[] = {
	{  1,   1, 3, 4, 0, "Number of parts" },
	{  2,  25, 3, 5, 0, "Part 1" },
	{  3,  25, 3, 5, 0, "Part 2" },
	{  4,  25, 3, 5, 0, "Part 3" },
	{  5,  25, 3, 5, 0, "Part 4" },
	{  6,  25, 3, 5, 0, "Part 5" },
	{  7,  25, 3, 5, 0, "Part 6" },
	{  8,  25, 3, 5, 0, "Part 7" },
	{  9,  25, 3, 5, 0, "Part 8" },
	{ 10,  25, 3, 5, 0, "Part 9" },
	{ 11,  25, 3, 5, 0, "Part 10" },
	{ 12,  25, 3, 5, 0, "Part 11" },
	{ 13,  25, 3, 5, 0, "Part 12" },
	{ 14,  25, 3, 5, 0, "Part 13" },
	{ 15,  25, 3, 5, 0, "Part 14" },
	{ 16,  25, 3, 5, 0, "Part 15" },
};

static const struct omci_attr_def omciAttriDescriptListOunRemoteDebug[] = {
	{  1,   1, 1, 4, 0, "Command format" },
	{  2,  25, 2, 5, 0, "Command" },
	{  3,   0, 1, 7, 0, "Reply table" },
};

static const struct omci_attr_def omciAttriDescriptListManagedEntity[] = {
	{  1,  25, 1, 5, 0, "Name" },
	{  2,   2, 1, 7, 0, "Attributes table" },
	{  3,   1, 1, 6, 0, "Access" },
	{  4,   1, 1, 7, 0, "Alarms table" },
	{  5,   1, 1, 7, 0, "AVCs table" },
	{  6,   4, 1, 2, 0, "Actions" },
	{  7,   2, 1, 7, 0, "Instances table" },
	{  8,   1, 1, 6, 0, "Support" },
};

static const struct omci_attr_def omciAttriDescriptListAttribute[] = {
	{  1,  25, 1, 5, 0, "Name" },
	{  2,   4, 1, 4, 0, "Size" },
	{  3,   1, 1, 6, 0, "Access" },
	{  4,   1, 1, 6, 0, "Format" },
	{  5,   4, 1, 4, 0, "Lower limit" },
	{  6,   4, 1, 4, 0, "Upper limit" },
	{  7,   4, 1, 2, 0, "Bit field" },
	{  8,   2, 1, 7, 0, "Code points table" },
	{  9,   1, 1, 6, 0, "Support" },
};

static const struct omci_attr_def omciAttriDescriptListPortMapping[] = {
	{  1,   1, 1, 4, 0, "Max ports" },
	{  2,  16, 1, 5, 0, "Port list 1" },
	{  3,  16, 1, 5, 0, "Port list 2" },
	{  4,  16, 1, 5, 0, "Port list 3" },
	{  5,  16, 1, 5, 0, "Port list 4" },
	{  6,  16, 1, 5, 0, "Port list 5" },
	{  7,  16, 1, 5, 0, "Port list 6" },
	{  8,  16, 1, 5, 0, "Port list 7" },
	{  9,  16, 1, 5, 0, "Port list 8" },
	{ 10,   0, 1, 5, 0, "Combined port table" },
};

static const struct omci_attr_def omciAttriDescriptListTr069ManageServer[] = {
	{  1,   1, 3, 4, 0, "Administrative" },
	{  2,   2, 3, 1, 0, "ACS network address" },
	{  3,   2, 3, 4, 0, "Associated tag" },
};

const struct omci_me_def omci_me_defs[] = {
	{   2, "ONU data", 1, OMCI_ME_SUPPORTED,  1, omciAttriDescriptListOnuData, 0, NULL, 0, 0, 0 },
	{   5, "Cardholder", 1, OMCI_ME_SUPPORTED,  9, omciAttriDescriptListCardholder, 0, NULL, 8, 9, 0 },
	{   6, "Circuit pack", 1, OMCI_ME_SUPPORTED, 14, omciAttriDescriptListCircuitPack, 0, NULL, 0, 0, 6 },
	{   7, "Software image", 1, OMCI_ME_SUPPORTED,  6, omciAttriDescriptListSoftwareImage, 0, NULL, 0, 0, 0 },
	{  11, "PPTP Ethernet UNI", 1, OMCI_ME_SUPPORTED, 15, omciAttriDescriptListPPTPEthernetUNI, 1, alarmIdTablePPTPEthernetUNI, 12, 13, 5 },
	{  24, "Ethernet PM history data", 2, OMCI_ME_SUPPORTED, 16, omciAttriDescriptListEthernetPMHistoryData, 0, NULL, 0, 0, 0 },
	{  45, "MAC bridge service profile", 2, OMCI_ME_SUPPORTED, 10, omciAttriDescriptListMBServiceProfile, 0, NULL, 0, 0, 0 },
	{  46, "MAC bridge configuration data", 1, OMCI_ME_SUPPORTED,  8, omciAttriDescriptListMBConnData, 0, NULL, 0, 0, 0 },
	{  47, "MAC bridge port config data", 2, OMCI_ME_SUPPORTED, 13, omciAttriDescriptListMBPortConData, 0, NULL, 0, 0, 0 },
	{  48, "MAC bridge port designate data", 1, OMCI_ME_SUPPORTED,  2, omciAttriDescriptListMBPortDesignData, 0, NULL, 0, 0, 0 },
	{  50, "MAC bridge port table data", 1, OMCI_ME_SUPPORTED,  1, omciAttriDescriptListMBPortData, 0, NULL, 0, 0, 0 },
	{  51, "MAC bridge PMHD", 2, OMCI_ME_SUPPORTED,  3, omciAttriDescriptListMBPMHD, 0, NULL, 0, 0, 0 },
	{  52, "MAC bridge port PMHD", 2, OMCI_ME_SUPPORTED,  7, omciAttriDescriptListMBPortPMHD, 0, NULL, 0, 0, 0 },
	{  78, "vlan tag operation", 2, OMCI_ME_SUPPORTED,  5, omciAttriDescriptListVlanTagOperation, 0, NULL, 0, 0, 0 },
	{  84, "vlan tag filter data", 2, OMCI_ME_SUPPORTED,  3, omciAttriDescriptListVlanTagFilterData, 0, NULL, 0, 0, 0 },
	{  89, "Ethernet PM history data 2", 2, OMCI_ME_SUPPORTED,  3, omciAttriDescriptListEthernetPMHistoryData2, 0, NULL, 0, 0, 0 },
	{ 130, "802.1p mapper profile", 2, OMCI_ME_SUPPORTED, 13, omciAttriDescriptList8021pMapperProfile, 0, NULL, 0, 0, 0 },
	{ 131, "OLT-G", 1, OMCI_ME_SUPPORTED,  4, omciAttriDescriptListOltG, 0, NULL, 0, 0, 0 },
	{ 134, "IP host config data", 1, OMCI_ME_SUPPORTED, 15, omciAttriDescriptListIPHostConfigData, 0, NULL, 0, 0, 0 },
	{ 135, "IP host PM", 2, OMCI_ME_SUPPORTED,  8, omciAttriDescriptListIPHostPM, 0, NULL, 0, 0, 0 },
	{ 136, "tcp udp config data", 2, OMCI_ME_SUPPORTED,  4, omciAttriDescriptListTcpUdpConfigData, 0, NULL, 0, 0, 0 },
	{ 137, "Network address", 3, OMCI_ME_SUPPORTED,  2, omciAttriDescriptListNetworkAddress, 0, NULL, 0, 0, 0 },
	{ 148, "Authentication method", 3, OMCI_ME_SUPPORTED,  5, omciAttriDescriptListAuthenticationMethod, 0, NULL, 0, 0, 0 },
	{ 157, "Large string", 3, OMCI_ME_SUPPORTED, 16, omciAttriDescriptListLargeString, 0, NULL, 0, 0, 0 },
	{ 158, "ONU remote debug", 1, OMCI_ME_SUPPORTED | OMCI_ME_NO_MIB_UPLOAD,  3, omciAttriDescriptListOunRemoteDebug, 0, NULL, 0, 0, 0 },
	{ 159, "protection profile", 1, OMCI_ME_SUPPORTED, 14, omciAttriDescriptListProtectionProfile, 0, NULL, 0, 0, 0 },
	{ 160, "Extension package", 1, OMCI_ME_SUPPORTED,  2, omciAttriDescriptListExtensionPackage, 0, NULL, 0, 0, 0 },
	{ 171, "extend vlan tag operation", 2, OMCI_ME_SUPPORTED,  8, omciAttriDescriptListVlanTagExtendOperation, 0, NULL, 0, 0, 0 },
	{ 256, "ONU-G", 1, OMCI_ME_SUPPORTED, 13, omciAttriDescriptListOnuG, 9, meMapTableOnuG, 0, 0, 0 },
	{ 257, "ONU2-G", 1, OMCI_ME_SUPPORTED, 14, omciAttriDescriptListOnu2G, 0, NULL, 0, 0, 0 },
	{ 262, "T-CONT", 1, OMCI_ME_SUPPORTED,  3, omciAttriDescriptListTcont, 0, NULL, 0, 0, 0 },
	{ 263, "ANI-G", 1, OMCI_ME_SUPPORTED, 16, omciAttriDescriptListAniG, 7, meMapTableAniG, 8, 9, 0 },
	{ 264, "UNI-G", 1, OMCI_ME_SUPPORTED,  5, omciAttriDescriptListUniG, 0, NULL, 0, 0, 0 },
	{ 266, "GEM IWTP", 2, OMCI_ME_SUPPORTED,  8, omciAttriDescriptListGemIWTP, 0, NULL, 0, 0, 0 },
	{ 267, "GEM port PM", 2, OMCI_ME_SUPPORTED,  8, omciAttriDescriptListGemPortPm, 0, NULL, 0, 0, 0 },
	{ 268, "GEM port CTP", 2, OMCI_ME_SUPPORTED, 10, omciAttriDescriptListGemPortCtp, 0, NULL, 0, 0, 0 },
	{ 272, "GAL ethernet profile", 2, OMCI_ME_SUPPORTED,  1, omciAttriDescriptListGALEthernetProfile, 0, NULL, 0, 0, 0 },
	{ 273, "Threshold data 1", 2, OMCI_ME_SUPPORTED,  7, omciAttriDescriptListThresholdData1, 0, NULL, 0, 0, 0 },
	{ 274, "Threshold data 2", 2, OMCI_ME_SUPPORTED,  7, omciAttriDescriptListThresholdData2, 0, NULL, 0, 0, 0 },
	{ 276, "GAL ethernet PM", 2, OMCI_ME_SUPPORTED,  3, omciAttriDescriptListGALEthernetPM, 0, NULL, 0, 0, 0 },
	{ 277, "Priority queue", 1, OMCI_ME_SUPPORTED, 16, omciAttriDescriptListPriorityQueue, 0, NULL, 0, 0, 0 },
	{ 278, "Traffic scheduler", 1, OMCI_ME_SUPPORTED,  4, omciAttriDescriptListTrafficScheduler, 0, NULL, 0, 0, 0 },
	{ 279, "Protection data", 1, OMCI_ME_SUPPORTED,  6, omciAttriDescriptListProtectionData, 0, NULL, 0, 0, 0 },
	{ 280, "Traffic descriptor", 2, OMCI_ME_SUPPORTED,  8, omciAttriDescriptListTrafficDescriptor, 0, NULL, 0, 0, 0 },
	{ 281, "Multicast GEM IWTP", 2, OMCI_ME_SUPPORTED,  9, omciAttriDescriptListMulticastGemIWTP, 0, NULL, 0, 0, 0 },
	{ 287, "OMCI", 1, OMCI_ME_SUPPORTED | OMCI_ME_NO_MIB_UPLOAD,  2, omciAttriDescriptListOmci, 0, NULL, 0, 0, 0 },
	{ 288, "Managed Entity", 1, OMCI_ME_SUPPORTED | OMCI_ME_NO_MIB_UPLOAD,  8, omciAttriDescriptListManagedEntity, 0, NULL, 0, 0, 0 },
	{ 289, "Attribute", 1, OMCI_ME_SUPPORTED | OMCI_ME_NO_MIB_UPLOAD,  9, omciAttriDescriptListAttribute, 0, NULL, 0, 0, 0 },
	{ 296, "Ethernet PM history data 3", 2, OMCI_ME_SUPPORTED, 16, omciAttriDescriptListEthernetPMHistoryData3, 0, NULL, 0, 0, 0 },
	{ 297, "Port mapping", 1, OMCI_ME_SUPPORTED, 10, omciAttriDescriptListPortMapping, 0, NULL, 0, 0, 0 },
	{ 299, "Dot1ag maintenance domain", 2, OMCI_ME_SUPPORTED,  6, omciAttriDescriptListDot1agMaintenanceDomain, 0, NULL, 0, 0, 0 },
	{ 300, "Dot1ag maintenance association", 2, OMCI_ME_SUPPORTED,  8, omciAttriDescriptListDot1agMaintenanceAssociation, 0, NULL, 0, 0, 0 },
	{ 309, "multicast profile", 2, OMCI_ME_SUPPORTED, 16, omciAttriDescriptListMulticastProfile, 0, NULL, 0, 0, 0 },
	{ 310, "multicast subscriber cfg info", 2, OMCI_ME_SUPPORTED,  7, omciAttriDescriptListMulticasSubscribertCfgInfo, 0, NULL, 0, 0, 0 },
	{ 311, "multicast subscriber monitor", 2, OMCI_ME_SUPPORTED,  6, omciAttriDescriptListMulticastSubscriberMonitor, 0, NULL, 0, 0, 0 },
	{ 312, "FEC PM", 2, OMCI_ME_SUPPORTED,  7, omciAttriDescriptListFecPM, 0, NULL, 0, 0, 0 },
	{ 321, "ethernet frame down PMHD", 2, OMCI_ME_SUPPORTED, 16, omciAttriDescriptListEthFrameDownPMHD, 0, NULL, 0, 0, 0 },
	{ 322, "ethernet frame up PMHD", 2, OMCI_ME_SUPPORTED, 16, omciAttriDescriptList_EthFrameUpPMHD, 0, NULL, 0, 0, 0 },
	{ 329, "Virtual ethernet iface point", 1, OMCI_ME_SUPPORTED,  5, omciAttriDescriptListVirtualEthernetInterfacePoint, 1, alarmIdTableVirtualEthernetInterfacePoint, 0, 0, 1 },
	{ 334, "Ethernet frame extended PM", 2, OMCI_ME_SUPPORTED, 16, omciAttriDescriptListMBEthnetFrameExtendedPM, 0, NULL, 0, 0, 0 },
	{ 340, "TR-069 management server", 3, OMCI_ME_SUPPORTED,  3, omciAttriDescriptListTr069ManageServer, 0, NULL, 0, 0, 1 },
	{ 341, "GEM port network CTP PM", 2, OMCI_ME_SUPPORTED,  7, omciAttriDescriptListGemportNetworkCtpPm, 0, NULL, 0, 0, 0 },
	{ 342, "tcp udp PM", 2, OMCI_ME_SUPPORTED,  7, omciAttriDescriptListTcpUdpPM, 0, NULL, 0, 0, 0 },
	{ 343, "Energy consumption PM", 2, OMCI_ME_SUPPORTED,  5, omciAttriDescriptListEnergyConsumptionPm, 0, NULL, 0, 0, 0 },
	{ 425, "Ethernet frame extended pm 64_bit", 2, OMCI_ME_SUPPORTED, 16, omciAttriDescriptListEthernetFrameExtendedPM64bit, 0, NULL, 0, 0, 0 },
};

const unsigned int omci_me_defs_num = 65;
