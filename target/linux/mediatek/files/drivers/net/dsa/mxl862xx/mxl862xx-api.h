/**
 * struct mdio_relay_data - relayed access to the switch internal MDIO bus
 * @data: data to be read or written
 * @phy: PHY index
 * @mmd: MMD device
 * @reg: register rndex
 */
struct mdio_relay_data {
	__le16 data;
	u8 phy;
	u8 mmd;
	__le16 reg;
} __packed;

/* Register access parameter to directly modify internal registers */
struct mxl862xx_register_mod {
	__le16 addr;
	__le16 data;
	__le16 mask;
} __packed;

/**
 * enum mxl862xx_mac_clear_type - MAC table clear type
 * @MXL862XX_MAC_CLEAR_PHY_PORT: clear dynamic entries based on port_id
 * @MXL862XX_MAC_CLEAR_DYNAMIC: clear all dynamic entries
 */
enum mxl862xx_mac_clear_type {
	MXL862XX_MAC_CLEAR_PHY_PORT = 0,
	MXL862XX_MAC_CLEAR_DYNAMIC,
};

/**
 * struct mxl862xx_mac_table_clear - MAC table clear
 * @type: see &enum mxl862xx_mac_clear_type
 * @port_id: physical port id
 */
struct mxl862xx_mac_table_clear {
	u8 type;
	u8 port_id;
} __packed;

/**
 * enum mxl862xx_pmapper_mapping_mode - P-mapper Mapping Mode
 * @MXL862XX_PMAPPER_MAPPING_PCP = 0, Use PCP for VLAN tagged packets to derive sub interface ID group
 * @MXL862XX_PMAPPER_MAPPING_LAG = 1, Use LAG Index for Pmapper access (regardless of IP and VLAN packet)
 * @MXL862XX_PMAPPER_MAPPING_DSCP = 2, Use DSCP for VLAN tagged IP packets to derive sub interface ID group
 */
enum mxl862xx_pmapper_mapping_mode {
	MXL862XX_PMAPPER_MAPPING_PCP = 0,
	MXL862XX_PMAPPER_MAPPING_LAG,
	MXL862XX_PMAPPER_MAPPING_DSCP,
};

/**
 * struct mxl862xx_monitor_port_cfg - Port monitor configuration
 * @port_id: port number
 * @sub_if_id: monitoring sub-interface id
 * @monitor_port: reserved
 */
struct mxl862xx_monitor_port_cfg {;
	u8 port_id;
	__le16 sub_if_id;
	u8 monitor_port;
} __packed;

/**
 * enum mxl862xx_ctp_port_config_mask - CTP Port configuration mask.
 * @MXL862XX_CTP_PORT_CONFIG_MASK_BRIDGE_PORT_ID: Mask for bridge_port_id
 * @MXL862XX_CTP_PORT_CONFIG_MASK_FORCE_TRAFFIC_CLASS: Mask for forced_traffic_class and default_traffic_class
 * @MXL862XX_CTP_PORT_CONFIG_MASK_INGRESS_VLAN: Mask for ingress_extended_vlan_enable and ingress_extended_vlan_block_id
 * @MXL862XX_CTP_PORT_CONFIG_MASK_INGRESS_VLAN_IGMP: Mask for ingress_extended_vlan_igmp_enable and ingress_extended_vlan_block_id_igmp
 * @MXL862XX_CTP_PORT_CONFIG_MASK_EGRESS_VLAN: Mask for egress_extended_vlan_enable and egress_extended_vlan_block_id
 * @MXL862XX_CTP_PORT_CONFIG_MASK_EGRESS_VLAN_IGMP: Mask for egress_extended_vlan_igmp_enable and egress_extended_vlan_block_id_igmp
 * @MXL862XX_CTP_PORT_CONFIG_MASK_INRESS_NTO1_VLAN: Mask for ingress_nto1Vlan_enable
 * @MXL862XX_CTP_PORT_CONFIG_MASK_EGRESS_NTO1_VLAN: Mask for egress_nto1Vlan_enable
 * @MXL862XX_CTP_PORT_CONFIG_INGRESS_MARKING: Mask for ingress_marking_mode
 * @MXL862XX_CTP_PORT_CONFIG_EGRESS_MARKING: Mask for egress_marking_mode
 * @MXL862XX_CTP_PORT_CONFIG_EGRESS_MARKING_OVERRIDE: Mask for egress_marking_override_enable and egress_marking_mode_override
 * @MXL862XX_CTP_PORT_CONFIG_EGRESS_REMARKING: Mask for egress_remarking_mode
 * @MXL862XX_CTP_PORT_CONFIG_INGRESS_METER: Mask for ingress_metering_enable and ingress_traffic_meter_id
 * @MXL862XX_CTP_PORT_CONFIG_EGRESS_METER: Mask for egress_metering_enable and egress_traffic_meter_id
 * @MXL862XX_CTP_PORT_CONFIG_BRIDGING_BYPASS: Mask for bridging_bypass
 * @MXL862XX_CTP_PORT_CONFIG_FLOW_ENTRY: Mask for first_flow_entry_index and number_of_flow_entries
 * @MXL862XX_CTP_PORT_CONFIG_LOOPBACK_AND_MIRROR: Mask for ingress_loopback_enable
 * @MXL862XX_CTP_PORT_CONFIG_MASK_ALL: Enable all fields
 * @MXL862XX_CTP_PORT_CONFIG_MASK_FORCE: Bypass any check for debug purpose
 */
enum mxl862xx_ctp_port_config_mask {
	MXL862XX_CTP_PORT_CONFIG_MASK_BRIDGE_PORT_ID = BIT(0),
	MXL862XX_CTP_PORT_CONFIG_MASK_FORCE_TRAFFIC_CLASS = BIT(1),
	MXL862XX_CTP_PORT_CONFIG_MASK_INGRESS_VLAN = BIT(2),
	MXL862XX_CTP_PORT_CONFIG_MASK_INGRESS_VLAN_IGMP = BIT(3),
	MXL862XX_CTP_PORT_CONFIG_MASK_EGRESS_VLAN = BIT(4),
	MXL862XX_CTP_PORT_CONFIG_MASK_EGRESS_VLAN_IGMP = BIT(5),
	MXL862XX_CTP_PORT_CONFIG_MASK_INRESS_NTO1_VLAN = BIT(6),
	MXL862XX_CTP_PORT_CONFIG_MASK_EGRESS_NTO1_VLAN = BIT(7),
	MXL862XX_CTP_PORT_CONFIG_INGRESS_MARKING = BIT(8),
	MXL862XX_CTP_PORT_CONFIG_EGRESS_MARKING = BIT(9),
	MXL862XX_CTP_PORT_CONFIG_EGRESS_MARKING_OVERRIDE = BIT(10),
	MXL862XX_CTP_PORT_CONFIG_EGRESS_REMARKING = BIT(11),
	MXL862XX_CTP_PORT_CONFIG_INGRESS_METER = BIT(12),
	MXL862XX_CTP_PORT_CONFIG_EGRESS_METER = BIT(13),
	MXL862XX_CTP_PORT_CONFIG_BRIDGING_BYPASS = BIT(14),
	MXL862XX_CTP_PORT_CONFIG_LOOPBACK_AND_MIRROR = BIT(16),
	MXL862XX_CTP_PORT_CONFIG_MASK_ALL = 0x7FFFFFFF,
	MXL862XX_CTP_PORT_CONFIG_MASK_FORCE = 0x8000000,
};

/** enum mxl862xx_color_marking_mode - Color Marking Mode
 * @MXL862XX_MARKING_ALL_GREEN: mark packets (except critical) to green
 * @MXL862XX_MARKING_INTERNAL_MARKING: do not change color and priority
 * @MXL862XX_MARKING_DEI: DEI mark mode
 * @MXL862XX_MARKING_PCP_8P0D: PCP 8P0D mark mode
 * @MXL862XX_MARKING_PCP_7P1D: PCP 7P1D mark mode
 * @MXL862XX_MARKING_PCP_6P2D: PCP 6P2D mark mode
 * @MXL862XX_MARKING_PCP_5P3D: PCP 5P3D mark mode
 * @MXL862XX_MARKING_DSCP_AF: DSCP AF class
 */
enum mxl862xx_color_marking_mode {
	MXL862XX_MARKING_ALL_GREEN = 0,
	MXL862XX_MARKING_INTERNAL_MARKING,
	MXL862XX_MARKING_DEI,
	MXL862XX_MARKING_PCP_8P0D,
	MXL862XX_MARKING_PCP_7P1D,
	MXL862XX_MARKING_PCP_6P2D,
	MXL862XX_MARKING_PCP_5P3D,
	MXL862XX_MARKING_DSCP_AF,
};

/**
 * enum mxl862xx_color_remarking_mode - \brief Color Remarking Mode
 * @MXL862XX_REMARKING_NONE: values from last process stage
 * @MXL862XX_REMARKING_DEI: DEI mark mode
 * @MXL862XX_REMARKING_PCP_8P0D: PCP 8P0D mark mode
 * @MXL862XX_REMARKING_PCP_7P1D: PCP 7P1D mark mode
 * @MXL862XX_REMARKING_PCP_6P2D: PCP 6P2D mark mode
 * @MXL862XX_REMARKING_PCP_5P3D: PCP 5P3D mark mode
 * @MXL862XX_REMARKING_DSCP_AF: DSCP AF class
 */
enum mxl862xx_color_remarking_mode {
	MXL862XX_REMARKING_NONE = 0,
	MXL862XX_REMARKING_DEI = 2,
	MXL862XX_REMARKING_PCP_8P0D,
	MXL862XX_REMARKING_PCP_7P1D,
	MXL862XX_REMARKING_PCP_6P2D,
	MXL862XX_REMARKING_PCP_5P3D,
	MXL862XX_REMARKING_DSCP_AF,
};

/**
 * struct mxl862xx_pmapper - P-mapper Configuration
 * @pmapper_id: Index of P-mapper <0-31>
 * @dest_sub_if_id_group: Sub interface ID group
 */
struct mxl862xx_pmapper {
	__le16 pmapper_id;
	u8 dest_sub_if_id_group[73];
} __packed;

/**
 * struct mxl862xx_ctp_port_config -  CTP Port Configuration
 * @logical_port_id: the ID of the port
 * @n_sub_if_id_group: sub interface ID group. The valid range is hardware/protocol dependent
 * @mask: See &enum mxl862xx_ctp_port_config_mask
 * @bridge_port_id: bridge port ID association to ingress CTP
 * @forced_traffic_class: default traffic class can not be overridden by other rules
 * @default_traffic_class: default traffic class associated with all ingress traffic
 * @ingress_extended_vlan_enable: Enable Extended VLAN processing for ingress non-IGMP traffic.
 * @ingress_extended_vlan_block_id: Extended VLAN block allocated for ingress non-IGMP traffic
 * @ingress_extended_vlan_block_size: Extended VLAN block size for ingress non-IGMP traffic
 * @ingress_extended_vlan_igmp_enable: Enable extended VLAN processing for ingress IGMP traffic
 * @ingress_extended_vlan_block_id_igmp: Extended VLAN block allocated for ingress IGMP traffic
 * @ingress_extended_vlan_block_size_igmp: Extended VLAN block size for ingress IGMP traffic
 * @egress_extended_vlan_enable: Enable extended VLAN processing for egress non-IGMP traffic
 * @egress_extended_vlan_block_id: Extended VLAN block allocated for egress non-IGMP traffic
 * @egress_extended_vlan_block_size: Extended VLAN block size for egress non-IGMP traffic
 * @egress_extended_vlan_igmp_enable: Enable extended VLAN processing for egress IGMP traffic
 * @egress_extended_vlan_block_id_igmp: Extended VLAN block allocated for egress IGMP traffic
 * @egress_extended_vlan_block_size_igmp: Extended VLAN block size for egress IGMP traffic
 * @ingress_marking_mode: See &enum mxl862xx_color_marking_mode
 * @egress_marking_mode: See &enum mxl862xx_color_marking_mode
 * @egress_marking_override_enable: Egress color marking mode override
 * @egress_marking_mode_override: See &enum mxl862xx_color_marking_mode
 * @ingress_metering_enable: Traffic metering on ingress traffic applies
 * @ingress_traffic_meter_id: Meter for ingress CTP process
 * @egress_metering_enable: Traffic metering on egress traffic applies.
 * @egress_traffic_meter_id: Meter ID for egress CTP process
 * @bridging_bypass: Ingress traffic bypass bridging/multicast processing
 * @dest_logical_port_id: logical destination port port when bridging_bypass is set
 * @pmapper_enable: Enable port mappin
 * @dest_sub_if_id_group: defines destination sub interface ID group
 * pmapper_mapping_mode: See &enum mxl862xx_pmapper_mapping_mode
 * @pmapper: See &enum mxl862xx_pmapper
 * @first_flow_entry_index: First traffic flow table entry is associated to this CTP port
 * @number_of_flow_entries: Number of traffic flow table entries that are associated to this CTP port
 * @ingress_loopback_enable: Ingress traffic from this CTP port will be redirected to the logical port
 * @ingress_da_sa_swap_enable: Destination/Source MAC address of ingress traffic is swapped before transmission
 * @egress_loopback_enable: Egress traffic to this CTP port will be redirected to ingress logical port
 * @egress_da_sa_swap_enable: Destination/Source MAC address of egress traffic is swapped before transmission
 * @ingress_mirror_enable: Ingress traffic is mirrored to the monitoring port
 * @egress_mirror_enable: Egress traffic is mirrored to the monitoring port
 */
struct mxl862xx_ctp_port_config
{
	u8 logical_port_id;
	__le16 n_sub_if_id_group;
	enum mxl862xx_ctp_port_config_mask mask;
	__le16 bridge_port_id;
	u8 forced_traffic_class;
	u8 default_traffic_class;
	u8 ingress_extended_vlan_enable;
	__le16 ingress_extended_vlan_block_id;
	__le16 ingress_extended_vlan_block_size;
	u8 ingress_extended_vlan_igmp_enable;
	__le16 ingress_extended_vlan_block_id_igmp;
	__le16 ingress_extended_vlan_block_size_igmp;
	u8 egress_extended_vlan_enable;
	__le16 egress_extended_vlan_block_id;
	__le16 egress_extended_vlan_block_size;
	u8 egress_extended_vlan_igmp_enable;
	__le16 egress_extended_vlan_block_id_igmp;
	__le16 egress_extended_vlan_block_size_igmp;
	u8 ingress_nto1vlan_enable;
	u8 egress_nto1vlan_enable;
	enum mxl862xx_color_marking_mode ingress_marking_mode;
	enum mxl862xx_color_marking_mode egress_marking_mode;
	u8 egress_marking_override_enable;
	enum mxl862xx_color_marking_mode egress_marking_mode_override;
	enum mxl862xx_color_remarking_mode egress_remarking_mode;
	u8 ingress_metering_enable;
	__le16 ingress_traffic_meter_id;
	u8 egress_metering_enable;
	__le16 egress_traffic_meter_id;
	u8 bridging_bypass;
	u8 dest_logical_port_id;
	u8 pmapper_enable;
	__le16 dest_sub_if_id_group;
	enum mxl862xx_pmapper_mapping_mode pmapper_mapping_mode;
	u8 pmapper_id_valid;
	struct mxl862xx_pmapper pmapper;
	__le16 first_flow_entry_index;
	__le16 number_of_flow_entries;
	u8 ingress_loopback_enable;
	u8 ingress_da_sa_swap_enable;
	u8 egress_loopback_enable;
	u8 egress_da_sa_swap_enable;
	u8 ingress_mirror_enable;
	u8 egress_mirror_enable;
} __packed;

/**
 * enum mxl862xx_age_timer - Aging Timer Value.
 * @MXL862XX_AGETIMER_1_SEC: 1 second aging time
 * @MXL862XX_AGETIMER_10_SEC: 10 seconds aging time
 * @MXL862XX_AGETIMER_300_SEC: 300 seconds aging time
 * @MXL862XX_AGETIMER_1_HOUR: 1 hour aging time
 * @MXL862XX_AGETIMER_1_DAY: 24 hours aging time
 * @MXL862XX_AGETIMER_CUSTOM: Custom aging time in seconds
 */
enum mxl862xx_age_timer {
	MXL862XX_AGETIMER_1_SEC = 1,
	MXL862XX_AGETIMER_10_SEC,
	MXL862XX_AGETIMER_300_SEC,
	MXL862XX_AGETIMER_1_HOUR,
	MXL862XX_AGETIMER_1_DAY,
	MXL862XX_AGETIMER_CUSTOM,
};

/**
 * struct mxl862xx_cfg -  Global Switch configuration Attributes
 * @mac_table_age_timer: See &enum mxl862xx_age_timer
 * @age_timer: Custom MAC table aging timer in seconds
 * @max_packet_len: Maximum Ethernet packet length.
 * @learning_limit_action: Automatic MAC address table learning limitation consecutive action
 * @mac_locking_action: Accept or discard MAC port locking violation packets
 * @mac_spoofing_action: Accept or discard MAC spoofing and port MAC locking violation packets
 * @pause_mac_mode_src: Pause frame MAC source address mode
 * @pause_mac_src: Pause frame MAC source address
 */
struct mxl862xx_cfg {
	enum mxl862xx_age_timer mac_table_age_timer;
	__le32 age_timer;
	__le16 max_packet_len;
	u8 learning_limit_action;
	u8 mac_locking_action;
	u8 mac_spoofing_action;
	u8 pause_mac_mode_src;
	u8 pause_mac_src[ETH_ALEN];
} __packed;

/**
 * struct mxl862xx_ss_sp_tag
 * @pid: port ID (1~16)
 * @mask: bit value 1 to indicate valid field
 *	0 - rx
 *	1 - tx
 *	2 - rx_pen
 *	3 - tx_pen
 * @rx: RX special tag mode
 *	0 - packet does NOT have special tag and special tag is NOT inserted
 *	1 - packet does NOT have special tag and special tag is inserted
 *	2 - packet has special tag and special tag is NOT inserted
 * @tx: TX special tag mode
 *	0 - packet does NOT have special tag and special tag is NOT removed
 *	1 - packet has special tag and special tag is replaced
 *	2 - packet has special tag and special tag is NOT removed
 *	3 - packet has special tag and special tag is removed
 * @rx_pen: RX special tag info over preamble
 *	0 - special tag info inserted from byte 2 to 7 are all 0
 *	1 - special tag byte 5 is 16, other bytes from 2 to 7 are 0
 *	2 - special tag byte 5 is from preamble field, others are 0
 *	3 - special tag byte 2 to 7 are from preabmle field
 * @tx_pen: TX special tag info over preamble
 *	0 - disabled
 *	1 - enabled
*/
struct mxl862xx_ss_sp_tag {
	u8 pid;
	u8 mask;
	u8 rx;
	u8 tx;
	u8 rx_pen;
	u8 tx_pen;
} __packed;

/**
 * enum mxl862xx_logical_port_mode - Logical port mode
 * @MXL862XX_LOGICAL_PORT_8BIT_WLAN: WLAN with 8-bit station ID
 * @MXL862XX_LOGICAL_PORT_9BIT_WLAN: WLAN with 9-bit station ID
 * @MXL862XX_LOGICAL_PORT_GPON: GPON OMCI context
 * @MXL862XX_LOGICAL_PORT_EPON: EPON context
 * @MXL862XX_LOGICAL_PORT_GINT: G.INT context
 * @MXL862XX_LOGICAL_PORT_OTHER: Others
 */
enum mxl862xx_logical_port_mode {
	MXL862XX_LOGICAL_PORT_8BIT_WLAN = 0,
	MXL862XX_LOGICAL_PORT_9BIT_WLAN,
	MXL862XX_LOGICAL_PORT_GPON,
	MXL862XX_LOGICAL_PORT_EPON,
	MXL862XX_LOGICAL_PORT_GINT,
	MXL862XX_LOGICAL_PORT_OTHER = 0xFF,
};

/**
 * struct mxl862xx_ctp_port_assignment - CTP Port Assignment/association with logical port
 * @logical_port_id: Logical Port Id. The valid range is hardware dependent
 * @first_ctp_port_id: First CTP Port ID mapped to above logical port ID
 * @number_of_ctp_port: Total number of CTP Ports mapped above logical port ID
 * @mode: See &enum mxl862xx_logical_port_mode
 * @bridge_port_id: Bridge ID (FID)
 */
struct mxl862xx_ctp_port_assignment {
	u8 logical_port_id;
	__le16 first_ctp_port_id;
	__le16 number_of_ctp_port;
	enum mxl862xx_logical_port_mode mode;
	__le16 bridge_port_id;
} __packed;

/**
 * struct mxl862xx_mac_table_add - MAC Table Entry to be added
 * @fid: Filtering Identifier (FID) (not supported by all switches)
 * @port_id: Ethernet Port number
 * @port_map: Bridge Port Map
 * @sub_if_id: Sub-Interface Identifier Destination
 * @age_timer: Aging Time in seconds
 * @vlan_id: STAG VLAN Id
 * @static_entry: Static Entry (value will be aged out if the entry is not set to static)
 * @traffic_class: Egress queue traffic class
 * @mac: MAC Address to add to the table
 * @filter_flag: Source/Destination MAC address filtering flag
 *	0 - not filter
 *	1 - source address filter
 *	2 - destination address filter
 *	3 - both source and destination filter
 * @igmp_controlled: Packet is marked as IGMP controlled if destination MAC address matche MAC in this entry
 * @associated_mac: Associated Mac address
 * @tci: TCI for B-Step
 *	Bit [0:11] - VLAN ID
 *	Bit [12] - VLAN CFI/DEI
 *	Bit [13:15] - VLAN PRI
 */
struct mxl862xx_mac_table_add {
	__le16 fid;
	__le32 port_id;
	__le16 port_map[8];
	__le16 sub_if_id;
	int age_timer;
	__le16 vlan_id;
	u8 static_entry;
	u8 traffic_class;
	u8 mac[ETH_ALEN];
	u8 filter_flag;
	u8 igmp_controlled;
	u8 associated_mac[ETH_ALEN];
	__le16 tci;
} __packed;

/**
 * struct mxl862xx_mac_table_remove - MAC Table Entry to be removed
 * @fid: Filtering Identifier (FID)
 * @mac: MAC Address to be removed from the table.
 * @filter_flag: Source/Destination MAC address filtering
 *	0 - not filter
 *	1 - source address filter
 *	2 - destination address filter
 *	3 - both source and destination filter
 * @tci: TCI for B-Step
 *	Bit [0:11] - VLAN ID
 *	Bit [12] - VLAN CFI/DEI
 *	Bit [13:15] - VLAN PRI
 */
struct mxl862xx_mac_table_remove {
	__le16 fid;
	u8 mac[ETH_ALEN];
	u8 filter_flag;
	__le16 tci;
} __packed;

/**
 * struct mxl862xx_mac_table_read - MAC Table Entry to be read
 * @initial: Restart the get operation from the beginning of the table
 * @last: Indicates that the read operation returned last entry
 * @fid: Get the MAC table entry belonging to the given Filtering Identifier
 * @port_id: The Bridge Port ID
 * @port_map: Bridge Port Map
 * @age_timer: Aging Time
 * @vlan_id: STAG VLAN Id
 * @static_entry: Indicates if this is a Static Entry
 * @sub_if_id: Sub-Interface Identifier Destination
 * @mac: MAC Address. Filled out by the switch API implementation.
 * @filter_flag: Source/Destination MAC address filtering
 *	0 - not filter
 *	1 - source address filter
 *	2 - destination address filter
 *	3 - both source and destination filter
 * @igmp_controlled: Packet is marked as IGMP controlled if destination MAC address matches the MAC in this entry
 * @entry_changed: Indicate if the Entry has Changed
 * @associated_mac: Associated MAC address
 * @hit_status: MAC Table Hit Status Update
 * @tci: TCI for  B-Step
 *	Bit [0:11] - VLAN ID
 *	Bit [12] - VLAN CFI/DEI
 *	Bit [13:15] - VLAN PRI
 */
struct mxl862xx_mac_table_read {
	u8 initial;
	u8 last;
	__le16 fid;
	__le32 port_id;
	__le16 port_map[8];
	int age_timer;
	__le16 vlan_id;
	u8 static_entry;
	__le16 sub_if_id;
	u8 mac[ETH_ALEN];
	u8 filter_flag;
	u8 igmp_controlled;
	u8 entry_changed;
	u8 associated_mac[ETH_ALEN];
	u8 hit_status;
	__le16 tci;
	__le16 first_bridge_port_id;
} __packed;

/**
 * enum mxl862xx_stp_port_state
 * @MXL862XX_STP_PORT_STATE_FORWARD: Forwarding state
 * @MXL862XX_STP_PORT_STATE_DISABLE: Disabled/Discarding state
 * @MXL862XX_STP_PORT_STATE_LEARNING: Learning state
 * @MXL862XX_STP_PORT_STATE_BLOCKING: Blocking/Listening
 */
enum mxl862xx_stp_port_state {
	MXL862XX_STP_PORT_STATE_FORWARD = 0,
	MXL862XX_STP_PORT_STATE_DISABLE,
	MXL862XX_STP_PORT_STATE_LEARNING,
	MXL862XX_STP_PORT_STATE_BLOCKING,
};

/**
 * struct mxl862xx_stp_port_cfg - Configures the Spanning Tree Protocol state of an Ethernet port
 * @port_id: Port number
 * @fid: Filtering Identifier (FID)
 * @port_state: See &enum mxl862xx_stp_port_state
 */
struct mxl862xx_stp_port_cfg {
	__le16 port_id;
	__le16 fid;
	enum mxl862xx_stp_port_state port_state;
} __packed;

/**
 * struct mxl862xx_bridge_port_alloc - Bridge Port Allocation.
 * @bridge_port_id: If the bridge port allocation is successful, a valid ID will be
 *		  returned in this field. Otherwise, INVALID_HANDLE is returned.
 *		  For bridge port free, this field should contain a valid ID
 *		  returned by the bridge port allocation. ID 0 is special for
 *		  the CPU port in PRX300, mapping to CTP 0 (Logical Port 0 with
 *		  Sub-interface ID 0), and is pre-allocated during initialization.
 *
 * Used by MXL862XX_BRIDGE_PORT_ALLOC and MXL862XX_BRIDGE_PORT_FREE.
 */
struct mxl862xx_bridge_port_alloc {
	__le16 bridge_port_id;
};

/**
 * enum mxl862xx_port_duplex - Ethernet port duplex status
 * @MXL862XX_DUPLEX_FULL: Port operates in full-duplex mode
 * @MXL862XX_DUPLEX_HALF: Port operates in half-duplex mode
 * @MXL862XX_DUPLEX_AUTO: Port operates in Auto mode
 */
enum mxl862xx_port_duplex {
	MXL862XX_DUPLEX_FULL = 0,
	MXL862XX_DUPLEX_HALF,
	MXL862XX_DUPLEX_AUTO,
};

/**
 * enum mxl862xx_port_speed -  Ethernet port speed mode
 * @MXL862XX_PORT_SPEED_10: 10 Mbit/s
 * @MXL862XX_PORT_SPEED_100: 100 Mbit/s
 * @MXL862XX_PORT_SPEED_200: 200 Mbit/s
 * @MXL862XX_PORT_SPEED_1000: 1000 Mbit/s
 * @MXL862XX_PORT_SPEED_2500: 2.5 Gbit/s
 * @MXL862XX_PORT_SPEED_5000: 5 Gbit/s
 * @MXL862XX_PORT_SPEED_10000: 10 Gbit/s
 * @MXL862XX_PORT_SPEED_AUTO: Auto speed for XGMAC
 */
enum mxl862xx_port_speed {
	MXL862XX_PORT_SPEED_10 = 0,
	MXL862XX_PORT_SPEED_100,
	MXL862XX_PORT_SPEED_200,
	MXL862XX_PORT_SPEED_1000,
	MXL862XX_PORT_SPEED_2500,
	MXL862XX_PORT_SPEED_5000,
	MXL862XX_PORT_SPEED_10000,
	MXL862XX_PORT_SPEED_AUTO,
};

/**
 * enum mxl862xx_port_link - Force the MAC and PHY link modus
 * @MXL862XX_PORT_LINK_UP: Link up
 * @MXL862XX_PORT_LINK_DOWN: Link down
 * @MXL862XX_PORT_LINK_AUTO: Link Auto
 */
enum mxl862xx_port_link {
	MXL862XX_PORT_LINK_UP = 0,
	MXL862XX_PORT_LINK_DOWN,
	MXL862XX_PORT_LINK_AUTO,
};

/**
 * enum mxl862xx_mii_mode - Ethernet port interface mode
 * @MXL862XX_PORT_HW_MII: Normal PHY interface
 * @MXL862XX_PORT_HW_RMII: Reduced MII interface in normal mode
 * @MXL862XX_PORT_HW_GMII: GMII or MII, depending upon the speed
 * @MXL862XX_PORT_HW_RGMII: RGMII mode
 * @MXL862XX_PORT_HW_XGMII: XGMII mode
 */
enum mxl862xx_mii_mode {
	MXL862XX_PORT_HW_MII = 0,
	MXL862XX_PORT_HW_RMII,
	MXL862XX_PORT_HW_GMII,
	MXL862XX_PORT_HW_RGMII,
	MXL862XX_PORT_HW_XGMII,
};

/**
 * enum mxl862xx_mii_type - Ethernet port configuration for PHY or MAC mode
 * @MXL862XX_PORT_MAC: The Ethernet port is configured to work in MAC mode
 * @MXL862XX_PORT_PHY: The Ethernet port is configured to work in PHY mode
 */
enum mxl862xx_mii_type {
	MXL862XX_PORT_MAC = 0,
	MXL862XX_PORT_PHY,
};

/**
 * enum mxl862xx_clk_mode - Ethernet port clock source configuration
 * @MXL862XX_PORT_CLK_NA: Clock Mode not applicable
 * @MXL862XX_PORT_CLK_MASTER: Clock Master Mode. The port is configured to provide the clock as output signal
 * @MXL862XX_PORT_CLK_SLAVE: Clock Slave Mode. The port is configured to use the input clock signal
 */
enum mxl862xx_clk_mode {
	MXL862XX_PORT_CLK_NA = 0,
	MXL862XX_PORT_CLK_MASTER,
	MXL862XX_PORT_CLK_SLAVE,
};

/**
 * struct mxl862xx_port_link_cfg - Ethernet port link, speed status and flow control status
 * @port_id: Ethernet Port number
 * @duplex_force: Force Port Duplex Mode
 * @duplex: See &enum mxl862xx_port_duplex
 * @speed_force: Force Link Speed
 * @speed: See &enum mxl862xx_port_speed
 * @link_force: Force Link
 * @link: See &enum mxl862xx_port_link
 * @mii_mode: See &enum mxl862xx_mii_mode
 * @mii_type: See &enum mxl862xx_mii_type
 * @clk_mode: See &enum mxl862xx_clk_mode
 * @lpi: 'Low Power Idle' Support for 'Energy Efficient Ethernet'
 */
struct mxl862xx_port_link_cfg {
	__le16 port_id;
	u8 duplex_force;
	enum mxl862xx_port_duplex duplex;
	u8 speed_force;
	enum mxl862xx_port_speed speed;
	u8 link_force;
	enum mxl862xx_port_link link;
	enum mxl862xx_mii_mode mii_mode;
	enum mxl862xx_mii_type mii_type;
	enum mxl862xx_clk_mode clk_mode;
	u8 lpi;
} __packed;

/**
 * enum mxl862xx_port_type - Port Type
 * @MXL862XX_LOGICAL_PORT: Logical Port
 * @MXL862XX_PHYSICAL_PORT: Physical Port
 * @MXL862XX_CTP_PORT: Connectivity Termination Port (CTP)
 * @MXL862XX_BRIDGE_PORT: Bridge Port
 */
enum mxl862xx_port_type {
	MXL862XX_LOGICAL_PORT = 0,
	MXL862XX_PHYSICAL_PORT,
	MXL862XX_CTP_PORT,
	MXL862XX_BRIDGE_PORT,
};

/**
 * enum mxl862xx_port_enable - port enable type selection.
 * @MXL862XX_PORT_DISABLE: the port is disabled in both directions
 * @MXL862XX_PORT_ENABLE_RXTX: the port is enabled in both directions
 * @MXL862XX_PORT_ENABLE_RX: the port is enabled in the receive direction
 * @MXL862XX_PORT_ENABLE_TX: the port is enabled in the transmit direction
 */
enum mxl862xx_port_enable{
	MXL862XX_PORT_DISABLE = 0,
	MXL862XX_PORT_ENABLE_RXTX,
	MXL862XX_PORT_ENABLE_RX,
	MXL862XX_PORT_ENABLE_TX,
};

/**
 * enum mxl862xx_port_flow - ethernet flow control status
 * @MXL862XX_FLOW_AUTO: automatic flow control
 * @MXL862XX_FLOW_RX: receive flow control only
 * @MXL862XX_FLOW_TX: transmit flow control only
 * @MXL862XX_FLOW_RXTX: receive and transmit flow control
 * @MXL862XX_FLOW_OFF: no flow control
 */
enum mxl862xx_port_flow {
	MXL862XX_FLOW_AUTO = 0,
	MXL862XX_FLOW_RX,
	MXL862XX_FLOW_TX,
	MXL862XX_FLOW_RXTX,
	MXL862XX_FLOW_OFF,
};

/**
 * enum mxl862xx_port_monitor - port mirror options
 * @MXL862XX_PORT_MONITOR_NONE: mirroring is disabled
 * @MXL862XX_PORT_MONITOR_RX: ingress packets are mirrored
 * @MXL862XX_PORT_MONITOR_TX: egress packets are mirrored
 * @MXL862XX_PORT_MONITOR_RXTX: ingress and egress packets are mirrored
 * @MXL862XX_PORT_MONITOR_VLAN_UNKNOWN: mirroring of 'unknown VLAN violation' frames
 * @MXL862XX_PORT_MONITOR_VLAN_MEMBERSHIP: mirroring of 'VLAN ingress or egress membership
	violation' frames
 * @MXL862XX_PORT_MONITOR_PORT_STATE: mirroring of 'port state violation' frames
 * @MXL862XX_PORT_MONITOR_LEARNING_LIMIT: mirroring of 'MAC learning limit violation' frames
 * @MXL862XX_PORT_MONITOR_PORT_LOCK: mirroring of 'port lock violation' frames
 */
enum mxl862xx_port_monitor {
	MXL862XX_PORT_MONITOR_NONE = 0,
	MXL862XX_PORT_MONITOR_RX,
	MXL862XX_PORT_MONITOR_TX,
	MXL862XX_PORT_MONITOR_RXTX,
	MXL862XX_PORT_MONITOR_VLAN_UNKNOWN,
	MXL862XX_PORT_MONITOR_VLAN_MEMBERSHIP = 16,
	MXL862XX_PORT_MONITOR_PORT_STATE = 32,
	MXL862XX_PORT_MONITOR_LEARNING_LIMIT = 64,
	MXL862XX_PORT_MONITOR_PORT_LOCK = 128,
};

/**
 * enum mxl862xx_if_rmon_mode - interface RMON counter mode
 * @MXL862XX_IF_RMON_FID: FID based RMON counters
 * @MXL862XX_IF_RMON_SUBID: sub-interface ID based
 * @MXL862XX_IF_RMON_FLOWID_LSB: flow ID based (bits 3:0)
 * @MXL862XX_IF_RMON_FLOWID_MSB: flow ID based (bits 7:4)
 */
enum mxl862xx_if_rmon_mode {
	MXL862XX_IF_RMON_FID = 0,
	MXL862XX_IF_RMON_SUBID,
	MXL862XX_IF_RMON_FLOWID_LSB,
	MXL862XX_IF_RMON_FLOWID_MSB,
};

/**
 * struct mxl862xx_port_cfg - Port Configuration Parameters
 * @port_type: See &enum mxl862xx_port_type
 * @port_id: Ethernet Port number (zero-based counting)
 * @enable: See &enum mxl862xx_port_enable
 * @unicast_unknown_drop: Drop unknown unicast packets
 * @multicast_unknown_drop: Drop unknown multicast packets
 * @reserved_packet_drop: Drop reserved packet types
 * @broadcast_drop: Drop broadcast packets
 * @aging: Enables MAC address table aging.
 * @learning: MAC address table learning
 * @learning_mac_port_lock: Automatic MAC address table learning locking on the port
 * @learning_limit: Automatic MAC address table learning limitation on this port
 * @mac_spoofing_detection: MAC spoofing detection. Identifies ingress packets that carry
 *      a MAC source address which was previously learned on a different ingress port
 * @flow_ctrl: See &enum mxl862xx_port_flow
 * @port_monitor: See &enum mxl862xx_port_monitor
 * @if_counters: Assign Interface RMON Counters for this Port
 * @if_count_start_idx: Interface RMON Counters Start Index
 * @if_rmonmode: See &enum mxl862xx_if_rmon_mode
 */
struct mxl862xx_port_cfg {
	enum mxl862xx_port_type port_type;
	__le16 port_id;
	enum mxl862xx_port_enable enable;
	u8 unicast_unknown_drop;
	u8 multicast_unknown_drop;
	u8 reserved_packet_drop;
	u8 broadcast_drop;
	u8 aging;
	u8 learning;
	u8 learning_mac_port_lock;
	__le16 learning_limit;
	u8 mac_spoofing_detection;
	enum mxl862xx_port_flow flow_ctrl;
	enum mxl862xx_port_monitor port_monitor;
	u8 if_counters;
	int if_count_start_idx;
	enum mxl862xx_if_rmon_mode if_rmonmode;
} __packed;

/**
 * struct mxl862xx_bridge_alloc - Bridge Allocation
 * @bridge_id: the ID assigned to the new bridge
 */
struct mxl862xx_bridge_alloc {
	__le16 bridge_id;
} __packed;

/**
 * enum mxl862xx_rmon_port_type - RMON counter table structure
 */
enum mxl862xx_rmon_port_type {
	MXL862XX_RMON_CTP_PORT_RX = 0,
	MXL862XX_RMON_CTP_PORT_TX,
	MXL862XX_RMON_BRIDGE_PORT_RX,
	MXL862XX_RMON_BRIDGE_PORT_TX,
	MXL862XX_RMON_CTP_PORT_PCE_BYPASS,
	MXL862XX_RMON_TFLOW_RX,
	MXL862XX_RMON_TFLOW_TX,
	MXL862XX_RMON_QMAP = 0x0E,
	MXL862XX_RMON_METER = 0x19,
	MXL862XX_RMON_PMAC = 0x1C,
};

/**
 * struct mxl862xx_rmon_port_cnt - RMON Counters for physical port
 * @port_type: Port Type. This gives information which type of port to get RMON.
 *             port_id should be based on this field.
 *             This is new in GSWIP-3.1. For GSWIP-2.1/2.2/3.0, this field is always
 *             ZERO (GSW_LOGICAL_PORT).
 * @port_id: Ethernet Port number (zero-based counting). The valid range is hardware
 *           dependent. An error code is delivered if the selected port is not
 *           available. This parameter specifies for which MAC port the RMON
 *           counter is read. It has to be set by the application before
 *           calling GSW_RMON_Port_Get.
 * @sub_if_id_group: Sub interface ID group. The valid range is hardware/protocol dependent.
 *                   This field is valid when ePortType is GSW_CTP_PORT.
 *                   Sub interface ID group is defined for each of GSW_LogicalPortMode_t.
 *                   For both GSW_LOGICAL_PORT_8BIT_WLAN and GSW_LOGICAL_PORT_9BIT_WLAN,
 *                   this field is VAP. For GSW_LOGICAL_PORT_GPON, this field is GEM index.
 *                   For GSW_LOGICAL_PORT_EPON, this field is stream index.
 *                   For GSW_LOGICAL_PORT_GINT, this field is LLID.
 *                   For others, this field is 0.
 * @pce_bypass: Separate set of CTP Tx counters when PCE is bypassed. GSWIP-3.1 only.
 * @rx_extended_vlan_discard_pkts: Discarded at Extended VLAN Operation Packet Count. GSWIP-3.1 only.
 * @mtu_exceed_discard_pkts: Discarded MTU Exceeded Packet Count. GSWIP-3.1 only.
 * @tx_under_size_good_pkts: Tx Undersize (<64) Packet Count. GSWIP-3.1 only.
 * @tx_oversize_good_pkts: Tx Oversize (>1518) Packet Count. GSWIP-3.1 only.
 * @rx_good_pkts: Receive Packet Count (only packets that are accepted and not discarded).
 * @rx_unicast_pkts: Receive Unicast Packet Count.
 * @rx_broadcast_pkts: Receive Broadcast Packet Count.
 * @rx_multicast_pkts: Receive Multicast Packet Count.
 * @rx_fcserror_pkts: Receive FCS Error Packet Count.
 * @rx_under_size_good_pkts: Receive Undersize Good Packet Count.
 * @rx_oversize_good_pkts: Receive Oversize Good Packet Count.
 * @rx_under_size_error_pkts: Receive Undersize Error Packet Count.
 * @rx_good_pause_pkts: Receive Good Pause Packet Count.
 * @rx_oversize_error_pkts: Receive Oversize Error Packet Count.
 * @rx_align_error_pkts: Receive Align Error Packet Count.
 * @rx_filtered_pkts: Filtered Packet Count.
 * @rx64byte_pkts: Receive Size 64 Bytes Packet Count.
 * @rx127byte_pkts: Receive Size 65-127 Bytes Packet Count.
 * @rx255byte_pkts: Receive Size 128-255 Bytes Packet Count.
 * @rx511byte_pkts: Receive Size 256-511 Bytes Packet Count.
 * @rx1023byte_pkts: Receive Size 512-1023 Bytes Packet Count.
 * @rx_max_byte_pkts: Receive Size 1024-1522 Bytes (or more, if configured) Packet Count.
 * @tx_good_pkts: Overall Transmit Good Packets Count.
 * @tx_unicast_pkts: Transmit Unicast Packet Count.
 * @tx_broadcast_pkts: Transmit Broadcast Packet Count.
 * @tx_multicast_pkts: Transmit Multicast Packet Count.
 * @tx_single_coll_count: Transmit Single Collision Count.
 * @tx_mult_coll_count: Transmit Multiple Collision Count.
 * @tx_late_coll_count: Transmit Late Collision Count.
 * @tx_excess_coll_count: Transmit Excessive Collision Count.
 * @tx_coll_count: Transmit Collision Count.
 * @tx_pause_count: Transmit Pause Packet Count.
 * @tx64byte_pkts: Transmit Size 64 Bytes Packet Count.
 * @tx127byte_pkts: Transmit Size 65-127 Bytes Packet Count.
 * @tx255byte_pkts: Transmit Size 128-255 Bytes Packet Count.
 * @tx511byte_pkts: Transmit Size 256-511 Bytes Packet Count.
 * @tx1023byte_pkts: Transmit Size 512-1023 Bytes Packet Count.
 * @tx_max_byte_pkts: Transmit Size 1024-1522 Bytes (or more, if configured) Packet Count.
 * @tx_dropped_pkts: Transmit Drop Packet Count.
 * @tx_acm_dropped_pkts: Transmit Dropped Packet Count, based on Congestion Management.
 * @rx_dropped_pkts: Receive Dropped Packet Count.
 * @rx_good_bytes: Receive Good Byte Count (64 bit).
 * @rx_bad_bytes: Receive Bad Byte Count (64 bit).
 * @tx_good_bytes: Transmit Good Byte Count (64 bit).
 */
struct mxl862xx_rmon_port_cnt {
	enum mxl862xx_port_type port_type;
	__le16 port_id;
	__le16 sub_if_id_group;
	u8 pce_bypass;
	__le32 rx_extended_vlan_discard_pkts;
	__le32 mtu_exceed_discard_pkts;
	__le32 tx_under_size_good_pkts;
	__le32 tx_oversize_good_pkts;
	__le32 rx_good_pkts;
	__le32 rx_unicast_pkts;
	__le32 rx_broadcast_pkts;
	__le32 rx_multicast_pkts;
	__le32 rx_fcserror_pkts;
	__le32 rx_under_size_good_pkts;
	__le32 rx_oversize_good_pkts;
	__le32 rx_under_size_error_pkts;
	__le32 rx_good_pause_pkts;
	__le32 rx_oversize_error_pkts;
	__le32 rx_align_error_pkts;
	__le32 rx_filtered_pkts;
	__le32 rx64byte_pkts;
	__le32 rx127byte_pkts;
	__le32 rx255byte_pkts;
	__le32 rx511byte_pkts;
	__le32 rx1023byte_pkts;
	__le32 rx_max_byte_pkts;
	__le32 tx_good_pkts;
	__le32 tx_unicast_pkts;
	__le32 tx_broadcast_pkts;
	__le32 tx_multicast_pkts;
	__le32 tx_single_coll_count;
	__le32 tx_mult_coll_count;
	__le32 tx_late_coll_count;
	__le32 tx_excess_coll_count;
	__le32 tx_coll_count;
	__le32 tx_pause_count;
	__le32 tx64byte_pkts;
	__le32 tx127byte_pkts;
	__le32 tx255byte_pkts;
	__le32 tx511byte_pkts;
	__le32 tx1023byte_pkts;
	__le32 tx_max_byte_pkts;
	__le32 tx_dropped_pkts;
	__le32 tx_acm_dropped_pkts;
	__le32 rx_dropped_pkts;
	__le64 rx_good_bytes;
	__le64 rx_bad_bytes;
	__le64 tx_good_bytes;
} __packed;

/**
 * enum mxl862xx_bridge_port_config_mask - Bridge Port configuration mask
 */
enum mxl862xx_bridge_port_config_mask {
	MXL862XX_BRIDGE_PORT_CONFIG_MASK_BRIDGE_ID = BIT(0),
	MXL862XX_BRIDGE_PORT_CONFIG_MASK_INGRESS_VLAN = BIT(1),
	MXL862XX_BRIDGE_PORT_CONFIG_MASK_EGRESS_VLAN = BIT(2),
	MXL862XX_BRIDGE_PORT_CONFIG_MASK_INGRESS_MARKING = BIT(3),
	MXL862XX_BRIDGE_PORT_CONFIG_MASK_EGRESS_REMARKING = BIT(4),
	MXL862XX_BRIDGE_PORT_CONFIG_MASK_INGRESS_METER = BIT(5),
	MXL862XX_BRIDGE_PORT_CONFIG_MASK_EGRESS_SUB_METER = BIT(6),
	MXL862XX_BRIDGE_PORT_CONFIG_MASK_EGRESS_CTP_MAPPING = BIT(7),
	MXL862XX_BRIDGE_PORT_CONFIG_MASK_BRIDGE_PORT_MAP = BIT(8),
	MXL862XX_BRIDGE_PORT_CONFIG_MASK_MC_DEST_IP_LOOKUP = BIT(9),
	MXL862XX_BRIDGE_PORT_CONFIG_MASK_MC_SRC_IP_LOOKUP = BIT(10),
	MXL862XX_BRIDGE_PORT_CONFIG_MASK_MC_DEST_MAC_LOOKUP = BIT(11),
	MXL862XX_BRIDGE_PORT_CONFIG_MASK_MC_SRC_MAC_LEARNING = BIT(12),
	MXL862XX_BRIDGE_PORT_CONFIG_MASK_MAC_SPOOFING = BIT(13),
	MXL862XX_BRIDGE_PORT_CONFIG_MASK_PORT_LOCK = BIT(14),
	MXL862XX_BRIDGE_PORT_CONFIG_MASK_MAC_LEARNING_LIMIT = BIT(15),
	MXL862XX_BRIDGE_PORT_CONFIG_MASK_MAC_LEARNED_COUNT = BIT(16),
	MXL862XX_BRIDGE_PORT_CONFIG_MASK_INGRESS_VLAN_FILTER = BIT(17),
	MXL862XX_BRIDGE_PORT_CONFIG_MASK_EGRESS_VLAN_FILTER1 = BIT(18),
	MXL862XX_BRIDGE_PORT_CONFIG_MASK_EGRESS_VLAN_FILTER2 = BIT(19),
	MXL862XX_BRIDGE_PORT_CONFIG_MASK_VLAN_BASED_MAC_LEARNING = BIT(20),
	MXL862XX_BRIDGE_PORT_CONFIG_MASK_VLAN_BASED_MULTICAST_LOOKUP = BIT(21),
	MXL862XX_BRIDGE_PORT_CONFIG_MASK_LOOP_VIOLATION_COUNTER = BIT(22),
	MXL862XX_BRIDGE_PORT_CONFIG_MASK_ALL = 0x7FFFFFFF,
	MXL862XX_BRIDGE_PORT_CONFIG_MASK_FORCE = BIT(31)
};

/**
 * enum mxl862xx_bridge_port_egress_meter - Meters for various egress traffic type
 * @MXL862XX_BRIDGE_PORT_EGRESS_METER_BROADCAST: Index of broadcast traffic meter
 * @MXL862XX_BRIDGE_PORT_EGRESS_METER_MULTICAST: Index of known multicast traffic meter
 * @MXL862XX_BRIDGE_PORT_EGRESS_METER_UNKNOWN_MC_IP: Index of unknown multicast IP traffic meter
 * @MXL862XX_BRIDGE_PORT_EGRESS_METER_UNKNOWN_MC_NON_IP: Index of unknown multicast non-IP traffic meter
 * @MXL862XX_BRIDGE_PORT_EGRESS_METER_UNKNOWN_UC: Index of unknown unicast traffic meter
 * @MXL862XX_BRIDGE_PORT_EGRESS_METER_OTHERS: Index of traffic meter for other types
 * @MXL862XX_BRIDGE_PORT_EGRESS_METER_MAX: Number of index
 */
enum mxl862xx_bridge_port_egress_meter {
	MXL862XX_BRIDGE_PORT_EGRESS_METER_BROADCAST = 0,
	MXL862XX_BRIDGE_PORT_EGRESS_METER_MULTICAST,
	MXL862XX_BRIDGE_PORT_EGRESS_METER_UNKNOWN_MC_IP,
	MXL862XX_BRIDGE_PORT_EGRESS_METER_UNKNOWN_MC_NON_IP,
	MXL862XX_BRIDGE_PORT_EGRESS_METER_UNKNOWN_UC,
	MXL862XX_BRIDGE_PORT_EGRESS_METER_OTHERS,
	MXL862XX_BRIDGE_PORT_EGRESS_METER_MAX,
};

struct mxl862xx_bridge_port_config {
	__le16 bridge_port_id;
	enum mxl862xx_bridge_port_config_mask mask;
	__le16 bridge_id;
	u8 ingress_extended_vlan_enable;
	__le16 ingress_extended_vlan_block_id;
	__le16 ingress_extended_vlan_block_size;
	u8 egress_extended_vlan_enable;
	__le16 egress_extended_vlan_block_id;
	__le16 egress_extended_vlan_block_size;
	enum mxl862xx_color_marking_mode ingress_marking_mode;
	enum mxl862xx_color_remarking_mode egress_remarking_mode;
	u8 ingress_metering_enable;
	__le16 ingress_traffic_meter_id;
	u8 egress_sub_metering_enable[MXL862XX_BRIDGE_PORT_EGRESS_METER_MAX];
	__le16 egress_traffic_sub_meter_id[MXL862XX_BRIDGE_PORT_EGRESS_METER_MAX];
	u8 dest_logical_port_id;
	u8 pmapper_enable;
	__le16 dest_sub_if_id_group;
	enum mxl862xx_pmapper_mapping_mode pmapper_mapping_mode;
	u8 pmapper_id_valid;
	struct mxl862xx_pmapper pmapper;
	__le16 bridge_port_map[8];
	u8 mc_dest_ip_lookup_disable;
	u8 mc_src_ip_lookup_enable;
	u8 dest_mac_lookup_disable;
	u8 src_mac_learning_disable;
	u8 mac_spoofing_detect_enable;
	u8 port_lock_enable;
	u8 mac_learning_limit_enable;
	__le16 mac_learning_limit;
	__le16 loop_violation_count;
	__le16 mac_learning_count;
	u8 ingress_vlan_filter_enable;
	__le16 ingress_vlan_filter_block_id;
	__le16 ingress_vlan_filter_block_size;
	u8 bypass_egress_vlan_filter1;
	u8 egress_vlan_filter1enable;
	__le16 egress_vlan_filter1block_id;
	__le16 egress_vlan_filter1block_size;
	u8 egress_vlan_filter2enable;
	__le16 egress_vlan_filter2block_id;
	__le16 egress_vlan_filter2block_size;
	u8 vlan_tag_selection;
	u8 vlan_src_mac_priority_enable;
	u8 vlan_src_mac_dei_enable;
	u8 vlan_src_mac_vid_enable;
	u8 vlan_dst_mac_priority_enable;
	u8 vlan_dst_mac_dei_enable;
	u8 vlan_dst_mac_vid_enable;
	u8 vlan_multicast_priority_enable;
	u8 vlan_multicast_dei_enable;
	u8 vlan_multicast_vid_enable;
} __packed;

/**
 * enum mxl862xx_bridge_config_mask - Bridge configuration mask
 * @MXL862XX_BRIDGE_CONFIG_MASK_MAC_LEARNING_LIMIT: Mask for mac_learning_limit_enable and mac_learning_limit.
 * @MXL862XX_BRIDGE_CONFIG_MASK_MAC_LEARNED_COUNT: Mask for mac_learning_count
 * @MXL862XX_BRIDGE_CONFIG_MASK_MAC_DISCARD_COUNT: Mask for learning_discard_event
 * @MXL862XX_BRIDGE_CONFIG_MASK_SUB_METER: Mask for sub_metering_enable and traffic_sub_meter_id
 * @MXL862XX_BRIDGE_CONFIG_MASK_FORWARDING_MODE: Mask for forward_broadcast, forward_unknown_multicast_ip, forward_unknown_multicast_non_ip and forward_unknown_unicast.
 * @MXL862XX_BRIDGE_CONFIG_MASK_ALL: Enable all
 * @MXL862XX_BRIDGE_CONFIG_MASK_FORCE: Bypass any check for debug purpose
 */
enum mxl862xx_bridge_config_mask {
	MXL862XX_BRIDGE_CONFIG_MASK_MAC_LEARNING_LIMIT = BIT(0),
	MXL862XX_BRIDGE_CONFIG_MASK_MAC_LEARNED_COUNT = BIT(1),
	MXL862XX_BRIDGE_CONFIG_MASK_MAC_DISCARD_COUNT = BIT(2),
	MXL862XX_BRIDGE_CONFIG_MASK_SUB_METER = BIT(3),
	MXL862XX_BRIDGE_CONFIG_MASK_FORWARDING_MODE = BIT(4),
	MXL862XX_BRIDGE_CONFIG_MASK_ALL = 0x7FFFFFFF,
	MXL862XX_BRIDGE_CONFIG_MASK_FORCE = BIT(31)
};

/**
 * enum mxl862xx_bridge_forward_mode - Bridge forwarding type of packet
 * @MXL862XX_BRIDGE_FORWARD_FLOOD: Packet is flooded to port members of ingress bridge port
 * @MXL862XX_BRIDGE_FORWARD_DISCARD: Packet is dscarded
 * @MXL862XX_BRIDGE_FORWARD_CPU: Packet is forwarded to logical port 0 CTP port 0 bridge port 0
 */
enum mxl862xx_bridge_forward_mode {
	MXL862XX_BRIDGE_FORWARD_FLOOD = 0,
	MXL862XX_BRIDGE_FORWARD_DISCARD,
	MXL862XX_BRIDGE_FORWARD_CPU,
};

/**
 * struct mxl862xx_bridge_config - Bridge Configuration
 * @bridge_id: Bridge ID (FID)
 * @mask: See &enum mxl862xx_bridge_config_mask
 * @mac_learning_limit_enable: Enable MAC learning limitation.
 * @mac_learning_limit: Max number of MAC can be learned in this bridge (all bridge ports).
 * @mac_learning_count: Get number of MAC address learned from this bridge port
 * @learning_discard_event: Number of learning discard event due to hardware resource not available
 * @sub_metering_enable: Traffic metering on type of traffic
 * @traffic_sub_meter_id: Meter for bridge process with specific type
 * @forward_broadcast: See &enum mxl862xx_bridge_forward_mode
 * @forward_unknown_multicast_ip: See &enum mxl862xx_bridge_forward_mode
 * @forward_unknown_multicast_non_ip: See &enum mxl862xx_bridge_forward_mode
 * @forward_unknown_unicast: See &enum mxl862xx_bridge_forward_mode
 */
struct mxl862xx_bridge_config {
	__le16 bridge_id;
	enum mxl862xx_bridge_config_mask mask;
	u8 mac_learning_limit_enable;
	__le16 mac_learning_limit;
	__le16 mac_learning_count;
	__le32 learning_discard_event;
	u8 sub_metering_enable[MXL862XX_BRIDGE_PORT_EGRESS_METER_MAX];
	__le16 traffic_sub_meter_id[MXL862XX_BRIDGE_PORT_EGRESS_METER_MAX];
	enum mxl862xx_bridge_forward_mode forward_broadcast;
	enum mxl862xx_bridge_forward_mode forward_unknown_multicast_ip;
	enum mxl862xx_bridge_forward_mode forward_unknown_multicast_non_ip;
	enum mxl862xx_bridge_forward_mode forward_unknown_unicast;
} __packed;

/**
 * enum mxl862xx_vlan_filter_tci_mask - VLAN Filter TCI mask
 * @MXL862XX_VLAN_FILTER_TCI_MASK_VID: TCI mask for VLAN ID
 * @MXL862XX_VLAN_FILTER_TCI_MASK_PCP: TCI mask for VLAN PCP
 * @MXL862XX_VLAN_FILTER_TCI_MASK_TCI: TCI mask for VLAN TCI
 */
enum mxl862xx_vlan_filter_tci_mask {
	MXL862XX_VLAN_FILTER_TCI_MASK_VID = 0,
	MXL862XX_VLAN_FILTER_TCI_MASK_PCP = 1,
	MXL862XX_VLAN_FILTER_TCI_MASK_TCI = 2
};

/**
 * enum mxl862xx_extended_vlan_4_tpid_mode - Extended VLAN 4 TPID mode
 * @MXL862XX_EXTENDEDVLAN_TPID_VTETYPE_1: Use global configured VTE type 1
 * @MXL862XX_EXTENDEDVLAN_TPID_VTETYPE_2: Use global configured VTE type 2
 * @MXL862XX_EXTENDEDVLAN_TPID_VTETYPE_3: Use global configured VTE type 3
 * @MXL862XX_EXTENDEDVLAN_TPID_VTETYPE_4: Use global configured VTE type 4
 */
enum mxl862xx_extended_vlan_4_tpid_mode {
	MXL862XX_EXTENDEDVLAN_TPID_VTETYPE_1 = 0,
	MXL862XX_EXTENDEDVLAN_TPID_VTETYPE_2 = 1,
	MXL862XX_EXTENDEDVLAN_TPID_VTETYPE_3 = 2,
	MXL862XX_EXTENDEDVLAN_TPID_VTETYPE_4 = 3
};

/**
 * enum mxl862xx_extended_vlan_filter_tpid - Extended VLAN filter TPID mode
 * @MXL862XX_EXTENDEDVLAN_FILTER_TPID_NO_FILTER: No filter
 * @MXL862XX_EXTENDEDVLAN_FILTER_TPID_8021Q: TPID is 0x8100
 * @MXL862XX_EXTENDEDVLAN_FILTER_TPID_VTETYPE: TPID is global configured value
 */
enum mxl862xx_extended_vlan_filter_tpid {
	MXL862XX_EXTENDEDVLAN_FILTER_TPID_NO_FILTER = 0,
	MXL862XX_EXTENDEDVLAN_FILTER_TPID_8021Q = 1,
	MXL862XX_EXTENDEDVLAN_FILTER_TPID_VTETYPE = 2
};

/**
 * enum mxl862xx_extended_vlan_treatment_tpid - Extended VLAN treatment TPID
 * @MXL862XX_EXTENDEDVLAN_TREATMENT_INNER_TPID: Copy from inner VLAN tag
 * @MXL862XX_EXTENDEDVLAN_TREATMENT_OUTER_TPID: Copy from outer VLAN tag
 * @MXL862XX_EXTENDEDVLAN_TREATMENT_VTETYPE: Use global configured value
 * @MXL862XX_EXTENDEDVLAN_TREATMENT_8021Q: TPID is 0x8100
 */
enum mxl862xx_extended_vlan_treatment_tpid {
	MXL862XX_EXTENDEDVLAN_TREATMENT_INNER_TPID = 0,
	MXL862XX_EXTENDEDVLAN_TREATMENT_OUTER_TPID = 1,
	MXL862XX_EXTENDEDVLAN_TREATMENT_VTETYPE = 2,
	MXL862XX_EXTENDEDVLAN_TREATMENT_8021Q = 3
};

/**
 * enum mxl862xx_extended_vlan_filter_dei - Extended VLAN filter DEI
 * @MXL862XX_EXTENDEDVLAN_FILTER_DEI_NO_FILTER: No filter
 * @MXL862XX_EXTENDEDVLAN_FILTER_DEI_0: DEI must be 0
 * @MXL862XX_EXTENDEDVLAN_FILTER_DEI_1: DEI must be 1
 */
enum mxl862xx_extended_vlan_filter_dei {
	MXL862XX_EXTENDEDVLAN_FILTER_DEI_NO_FILTER = 0,
	MXL862XX_EXTENDEDVLAN_FILTER_DEI_0 = 1,
	MXL862XX_EXTENDEDVLAN_FILTER_DEI_1 = 2
};

/**
 * enum mxl862xx_extended_vlan_treatment_dei - Extended VLAN treatment DEI
 * @MXL862XX_EXTENDEDVLAN_TREATMENT_INNER_DEI: Copy from inner VLAN tag
 * @MXL862XX_EXTENDEDVLAN_TREATMENT_OUTER_DEI: Copy from outer VLAN tag
 * @MXL862XX_EXTENDEDVLAN_TREATMENT_DEI_0: Force DEI to 0
 * @MXL862XX_EXTENDEDVLAN_TREATMENT_DEI_1: Force DEI to 1
 */
enum mxl862xx_extended_vlan_treatment_dei {
	MXL862XX_EXTENDEDVLAN_TREATMENT_INNER_DEI = 0,
	MXL862XX_EXTENDEDVLAN_TREATMENT_OUTER_DEI = 1,
	MXL862XX_EXTENDEDVLAN_TREATMENT_DEI_0 = 2,
	MXL862XX_EXTENDEDVLAN_TREATMENT_DEI_1 = 3
};

/**
 * enum mxl862xx_extended_vlan_filter_type - Extended VLAN filter type
 * @MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NORMAL: Tagged packet with matching criteria
 * @MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NO_FILTER: Tagged packet with no criteria
 * @MXL862XX_EXTENDEDVLAN_FILTER_TYPE_DEFAULT: Default entry if no rule applies
 * @MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NO_TAG: No VLAN tag
 * @MXL862XX_EXTENDEDVLAN_BLOCK_INVALID: Invalid block
 */
enum mxl862xx_extended_vlan_filter_type {
	MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NORMAL = 0,
	MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NO_FILTER = 1,
	MXL862XX_EXTENDEDVLAN_FILTER_TYPE_DEFAULT = 2,
	MXL862XX_EXTENDEDVLAN_FILTER_TYPE_NO_TAG = 3,
	MXL862XX_EXTENDEDVLAN_BLOCK_INVALID = 4
};

/**
 * enum mxl862xx_extended_vlan_filter_ethertype - Extended VLAN filter ethertype
 * @MXL862XX_EXTENDEDVLAN_FILTER_ETHERTYPE_NO_FILTER: No filter
 * @MXL862XX_EXTENDEDVLAN_FILTER_ETHERTYPE_IPOE: IPoE frame (0x0800)
 * @MXL862XX_EXTENDEDVLAN_FILTER_ETHERTYPE_PPPOE: PPPoE frame (0x8863/0x8864)
 * @MXL862XX_EXTENDEDVLAN_FILTER_ETHERTYPE_ARP: ARP frame (0x0806)
 * @MXL862XX_EXTENDEDVLAN_FILTER_ETHERTYPE_IPV6IPOE: IPv6 IPoE (0x86DD)
 * @MXL862XX_EXTENDEDVLAN_FILTER_ETHERTYPE_EAPOL: EAPOL (0x888E)
 * @MXL862XX_EXTENDEDVLAN_FILTER_ETHERTYPE_DHCPV4: DHCPv4 (UDP port 67/68)
 * @MXL862XX_EXTENDEDVLAN_FILTER_ETHERTYPE_DHCPV6: DHCPv6 (UDP port 546/547)
 */
enum mxl862xx_extended_vlan_filter_ethertype {
	MXL862XX_EXTENDEDVLAN_FILTER_ETHERTYPE_NO_FILTER = 0,
	MXL862XX_EXTENDEDVLAN_FILTER_ETHERTYPE_IPOE = 1,
	MXL862XX_EXTENDEDVLAN_FILTER_ETHERTYPE_PPPOE = 2,
	MXL862XX_EXTENDEDVLAN_FILTER_ETHERTYPE_ARP = 3,
	MXL862XX_EXTENDEDVLAN_FILTER_ETHERTYPE_IPV6IPOE = 4,
	MXL862XX_EXTENDEDVLAN_FILTER_ETHERTYPE_EAPOL = 5,
	MXL862XX_EXTENDEDVLAN_FILTER_ETHERTYPE_DHCPV4 = 6,
	MXL862XX_EXTENDEDVLAN_FILTER_ETHERTYPE_DHCPV6 = 7
};

/**
 * enum mxl862xx_extended_vlan_treatment_priority - Extended VLAN treatment priority
 * @MXL862XX_EXTENDEDVLAN_TREATMENT_PRIORITY_VAL: Use a fixed priority
 * @MXL862XX_EXTENDEDVLAN_TREATMENT_INNER_PRORITY: Copy priority from inner VLAN
 * @MXL862XX_EXTENDEDVLAN_TREATMENT_OUTER_PRORITY: Copy priority from outer VLAN
 * @MXL862XX_EXTENDEDVLAN_TREATMENT_DSCP: Derive priority from DSCP
 */
enum mxl862xx_extended_vlan_treatment_priority {
	MXL862XX_EXTENDEDVLAN_TREATMENT_PRIORITY_VAL = 0,
	MXL862XX_EXTENDEDVLAN_TREATMENT_INNER_PRORITY = 1,
	MXL862XX_EXTENDEDVLAN_TREATMENT_OUTER_PRORITY = 2,
	MXL862XX_EXTENDEDVLAN_TREATMENT_DSCP = 3
};

/**
 * enum mxl862xx_extended_vlan_treatment_vid - Extended VLAN treatment VID
 * @MXL862XX_EXTENDEDVLAN_TREATMENT_VID_VAL: Use a fixed VID
 * @MXL862XX_EXTENDEDVLAN_TREATMENT_INNER_VID: Copy from inner VLAN
 * @MXL862XX_EXTENDEDVLAN_TREATMENT_OUTER_VID: Copy from outer VLAN
 */
enum mxl862xx_extended_vlan_treatment_vid {
	MXL862XX_EXTENDEDVLAN_TREATMENT_VID_VAL = 0,
	MXL862XX_EXTENDEDVLAN_TREATMENT_INNER_VID = 1,
	MXL862XX_EXTENDEDVLAN_TREATMENT_OUTER_VID = 2,
};

/**
 * enum mxl862xx_extended_vlan_treatment_remove_tag - Extended VLAN remove tag action
 * @MXL862XX_EXTENDEDVLAN_TREATMENT_NOT_REMOVE_TAG: Do not remove VLAN tag
 * @MXL862XX_EXTENDEDVLAN_TREATMENT_REMOVE_1_TAG: Remove one VLAN tag
 * @MXL862XX_EXTENDEDVLAN_TREATMENT_REMOVE_2_TAG: Remove two VLAN tags
 * @MXL862XX_EXTENDEDVLAN_TREATMENT_DISCARD_UPSTREAM: Discard upstream traffic
 */
enum mxl862xx_extended_vlan_treatment_remove_tag {
	MXL862XX_EXTENDEDVLAN_TREATMENT_NOT_REMOVE_TAG = 0,
	MXL862XX_EXTENDEDVLAN_TREATMENT_REMOVE_1_TAG = 1,
	MXL862XX_EXTENDEDVLAN_TREATMENT_REMOVE_2_TAG = 2,
	MXL862XX_EXTENDEDVLAN_TREATMENT_DISCARD_UPSTREAM = 3,
};

/**
 * struct mxl862xx_extendedvlan_filter_vlan - Extended VLAN filter VLAN tag
 * @type: Filter type
 * @priority_enable: Enable priority filtering
 * @priority_val: Priority value to match
 * @vid_enable: Enable VID filtering
 * @vid_val: VID value to match
 * @tpid: VLAN filter TPID
 * @dei: VLAN filter DEI
 */
struct mxl862xx_extendedvlan_filter_vlan {
	enum mxl862xx_extended_vlan_filter_type type;
	bool priority_enable;
	__le32 priority_val;
	bool vid_enable;
	__le32 vid_val;
	enum mxl862xx_extended_vlan_filter_tpid tpid;
	enum mxl862xx_extended_vlan_filter_dei dei;
} __packed;

/**
 * struct mxl862xx_extendedvlan_treatment_vlan - Extended VLAN treatment VLAN tag
 * @priority_mode: Source of VLAN tag priority
 * @priority_val: Priority value if using a fixed priority
 * @vid_mode: Source of VLAN tag VID
 * @vid_val: VID if using a fixed VID
 * @tpid: Source of TPID field
 * @dei: Source of DEI field
 */
struct mxl862xx_extendedvlan_treatment_vlan {
	enum mxl862xx_extended_vlan_treatment_priority priority_mode;
	__le32 priority_val;
	enum mxl862xx_extended_vlan_treatment_vid vid_mode;
	__le32 vid_val;
	enum mxl862xx_extended_vlan_treatment_tpid tpid;
	enum mxl862xx_extended_vlan_treatment_dei dei;
} __packed;

/**
 * struct mxl862xx_extendedvlan_filter - Extended VLAN filter configuration
 * @original_packet_filter_mode: True if filtering is on original packet
 * @filter_4_tpid_mode: 4 TPID mode
 * @outer_vlan: Filter for outer VLAN
 * @inner_vlan: Filter for inner VLAN
 * @ether_type: Filter by ethertype
 */
struct mxl862xx_extendedvlan_filter {
	bool original_packet_filter_mode;
	enum mxl862xx_extended_vlan_4_tpid_mode filter_4_tpid_mode;
	struct mxl862xx_extendedvlan_filter_vlan outer_vlan;
	struct mxl862xx_extendedvlan_filter_vlan inner_vlan;
	enum mxl862xx_extended_vlan_filter_ethertype ether_type;
} __packed;

/**
 * struct mxl862xx_extendedvlan_alloc - Extended VLAN allocation
 * @number_of_entries: Number of entries requested
 * @extended_vlan_block_id: Returned block ID
 */
struct mxl862xx_extendedvlan_alloc {
	__le16 number_of_entries;
	__le16 extended_vlan_block_id;
} __packed;

/**
 * struct mxl862xx_extendedvlan_treatment - Extended VLAN treatment
 * @remove_tag: Tag removal method
 * @treatment_4_tpid_mode: 4 TPID treatment mode
 * @add_outer_vlan: If true, apply changes to outer VLAN
 * @outer_vlan: Outer VLAN changes
 * @add_inner_vlan: If true, apply changes to inner VLAN
 * @inner_vlan: Inner VLAN changes
 * @reassign_bridge_port: If true, reassign to a new bridge port
 * @new_bridge_port_id: Bridge port ID if reassigned
 * @new_dscp_enable: If true, override DSCP
 * @new_dscp: DSCP to set
 * @new_traffic_class_enable: If true, override traffic class
 * @new_traffic_class: Traffic class to set
 * @new_meter_enable: If true, use new meter
 * @s_new_traffic_meter_id: Meter ID if new meter is used
 * @mirror_enable: Enable mirroring
 */
struct mxl862xx_extendedvlan_treatment {
	enum mxl862xx_extended_vlan_treatment_remove_tag remove_tag;
	enum mxl862xx_extended_vlan_4_tpid_mode treatment_4_tpid_mode;
	bool add_outer_vlan;
	struct mxl862xx_extendedvlan_treatment_vlan outer_vlan;
	bool add_inner_vlan;
	struct mxl862xx_extendedvlan_treatment_vlan inner_vlan;
	bool reassign_bridge_port;
	__le16 new_bridge_port_id;
	bool new_dscp_enable;
	__le16 new_dscp;
	bool new_traffic_class_enable;
	u8 new_traffic_class;
	bool new_meter_enable;
	__le16 s_new_traffic_meter_id;
	u8 dscp2pcp_map[64];
	bool loopback_enable;
	bool da_sa_swap_enable;
	bool mirror_enable;
} __packed;

/**
 * struct mxl862xx_extendedvlan_config - Extended VLAN configuration
 * @extended_vlan_block_id: Block ID
 * @entry_index: Entry index
 * @filter: Filter settings
 * @treatment: Treatment settings
 */
struct mxl862xx_extendedvlan_config {
	__le16 extended_vlan_block_id;
	__le16 entry_index;
	struct mxl862xx_extendedvlan_filter filter;
	struct mxl862xx_extendedvlan_treatment treatment;
} __packed;

/**
 * struct mxl862xx_vlanfilter_alloc - VLAN Filter Allocation
 * @number_of_entries: Number of entries requested
 * @vlan_filter_block_id: Returned block ID
 * @discard_untagged: Discard untagged packets
 * @discard_unmatched_tagged: Discard unmatched tagged packets
 * @use_default_port_vid: Use default port VLAN ID for filtering
 */
struct mxl862xx_vlanfilter_alloc {
	__le16 number_of_entries;
	__le16 vlan_filter_block_id;
	bool discard_untagged;
	bool discard_unmatched_tagged;
	bool use_default_port_vid;
} __packed;

/**
 * struct mxl862xx_vlanfilter_config - VLAN Filter
 * @vlan_filter_block_id: Block ID
 * @entry_index: Entry index
 * @vlan_filter_mask: VLAN TCI filter mask mode
 * @val: Value for VLAN filtering
 * @discard_matched: Discard matched packets
 */
struct mxl862xx_vlanfilter_config {
	__le16 vlan_filter_block_id;
	__le16 entry_index;
	enum mxl862xx_vlan_filter_tci_mask vlan_filter_mask;
	__le32 val;
	bool discard_matched;
} __packed;

/**
 * enum mxl862xx_vlan_rmon_type - VLAN RMON counter type
 * @MXL862XX_VLAN_RMON_RX: RX RMON counter
 * @MXL862XX_VLAN_RMON_TX: TX RMON counter
 * @MXL862XX_VLAN_RMON__PCE_BYPASS: PCE bypass RMON counter
 */
enum mxl862xx_vlan_rmon_type {
	MXL862XX_VLAN_RMON_RX = 0,
	MXL862XX_VLAN_RMON_TX = 1,
	MXL862XX_VLAN_RMON__PCE_BYPASS = 2,
};

/**
 * struct mxl862xx_vlan_rmon_cnt - VLAN RMON counters
 * @vlan_counter_index: Counter index
 * @vlan_rmon_type: RMON counter type
 * @byte_count_high: High 32 bits of byte count
 * @byte_count_low: Low 32 bits of byte count
 * @total_pkt_count: Total packet count
 * @multicast_pkt_count: Multicast packet count
 * @drop_pkt_count: Drop packet count
 * @clear_all: Clear all counters
 */
struct mxl862xx_vlan_rmon_cnt {
	__le16 vlan_counter_index;
	enum mxl862xx_vlan_rmon_type vlan_rmon_type;
	__le32 byte_count_high;
	__le32 byte_count_low;
	__le32 total_pkt_count;
	__le32 multicast_pkt_count;
	__le32 drop_pkt_count;
	__le32 clear_all;
} __packed;

/**
 * struct mxl862xx_vlan_rmon_control - VLAN RMON control
 * @vlan_rmon_enable: Enable VLAN RMON
 * @include_broad_cast_pkt_counting: Include broadcast packet counting
 * @vlan_last_entry: Last entry index
 */
struct mxl862xx_vlan_rmon_control {
	bool vlan_rmon_enable;
	bool include_broad_cast_pkt_counting;
	__le32 vlan_last_entry;
} __packed;

/**
 * enum mxl862xx_vlan_counter_mapping_type - VLAN counter mapping type
 * @MXL862XX_VLAN_MAPPING_INGRESS: Ingress mapping
 * @MXL862XX_VLAN_MAPPING_EGRESS: Egress mapping
 * @MXL862XX_VLAN_MAPPING_INGRESS_AND_EGRESS: Ingress and egress mapping
 */
enum mxl862xx_vlan_counter_mapping_type {
	MXL862XX_VLAN_MAPPING_INGRESS = 0,
	MXL862XX_VLAN_MAPPING_EGRESS = 1,
	MXL862XX_VLAN_MAPPING_INGRESS_AND_EGRESS = 2
};

/**
 * enum mxl862xx_vlan_counter_map_filter_type - VLAN counter mapping filter type
 * @MXL862XX_VLANCOUNTERMAP_FILTER_TYPE_NORMAL: Tagged packet with matching criteria
 * @MXL862XX_VLANCOUNTERMAP_FILTER_TYPE_NO_FILTER: Tagged packet with no criteria
 * @MXL862XX_VLANCOUNTERMAP_FILTER_TYPE_DEFAULT: Default entry if no rule applies
 * @MXL862XX_VLANCOUNTERMAP_FILTER_TYPE_NO_TAG: No VLAN tag
 * @MXL862XX_VLANCOUNTERMAP_FILTER_INVALID: Invalid filter
 */
enum mxl862xx_vlan_counter_map_filter_type {
	MXL862XX_VLANCOUNTERMAP_FILTER_TYPE_NORMAL = 0,
	MXL862XX_VLANCOUNTERMAP_FILTER_TYPE_NO_FILTER = 1,
	MXL862XX_VLANCOUNTERMAP_FILTER_TYPE_DEFAULT = 2,
	MXL862XX_VLANCOUNTERMAP_FILTER_TYPE_NO_TAG = 3,
	MXL862XX_VLANCOUNTERMAP_FILTER_INVALID = 4,
};

/**
 * struct mxl862xx_vlan_counter_mapping_config - VLAN counter mapping configuration
 * @counter_index: Counter index
 * @ctp_port_id: CTP port ID
 * @priority_enable: Enable priority filtering
 * @priority_val: Priority value to match
 * @vid_enable: Enable VID filtering
 * @vid_val: VID value to match
 * @vlan_tag_selection_enable: Enable VLAN tag selection
 * @vlan_counter_mapping_type: Mapping type
 * @vlan_counter_mapping_filter_type: Filter type
 */
struct mxl862xx_vlan_counter_mapping_config {
	u8 counter_index;
	__le16 ctp_port_id;
	bool priority_enable;
	__le32 priority_val;
	bool vid_enable;
	__le32 vid_val;
	bool vlan_tag_selection_enable;
	enum mxl862xx_vlan_counter_mapping_type vlan_counter_mapping_type;
	enum mxl862xx_vlan_counter_map_filter_type vlan_counter_mapping_filter_type;
} __packed;

/**
 * struct mxl862xx_sys_fw_image_version - VLAN counter mapping configuration
 * @iv_major: firmware major version
 * @iv_minor: firmware minor version
 * @iv_revision: firmware revision
 * @iv_build_num: firmware build number
 */
struct mxl862xx_sys_fw_image_version {
	u8 iv_major;
	u8 iv_minor;
	__le16 iv_revision;
	__le32 iv_build_num;
} __packed;

/**
 * struct mxl862xx_sys_sfp_cfg - Config SFP/Serdes ports
 * @port_id: port id (0 or 1)
 * @option: config options (0 - SFP mode/speed/link-status, 1 - flow control)
 * @mode: SFP mode (0 - auto, 1 - fix, 2 - disable)
 * @speed: select speed when mode is 1
 * @link: get link state
 * @fc_en: flow control (0 - disable, 1 - enable)
 */
struct mxl862xx_sys_sfp_cfg {
	u8 port_id: 4;
	u8 option: 4;
	union {
		struct {
			u8 mode;
			/** select speed when mode is 1
			 *	0 - 10G Quad USXGMII
			 *	1 - 1000BaseX ANeg
			 *	2 - 10G	XFI
			 *	3 - 10G Single USXGMII
			 *	4 - 2.5G SGMII
			 *	5 - 2500 Single USXGMI
			 *	6 - 2500BaseX NonANeg
			 *	7 - 1000BaseX NonANeg
			 *	8 - 1G SGMI
			 */
			u8 speed;
			u8 link;
		};
		u8 fc_en;
	};
};
