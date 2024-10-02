# ufp - OpenWrt device fingerprinting

The daemon uses plugins to gather information about connected devices. Each fingerprint type has a weight. Based on the aggregated weight of all identified data points, the fingerprint is matched against the data base.

The following plugins currently exist
* DHCP 
* WIFI Taxonomy
* MDNS

## uBus API
* ubus call fingerprint fingerprint - This is the main user facing method, listing the identified devices.
* ubus call fingerprint get_data - This lists the raw data used for internal matching,


## Supported data points
* wifi4 - the HT taxonomy
* wifi6 - the VHT taxonomy
* wifi-vendor-oui-XXYYZZ - identified vendor IEs
* dhcp_req - the DHCP discover / request signature
* dhcp_vendorid - the vendor ID value inside DHCP discover / request  frames
* mac-oui-XXYYZZ - OUI of the primary MAC

All MDNS based fingerprinting is done inside the plugin.

## Setting up the code
The daemon does not need to be configured, it will try to identify any device it sees on wired and wireless interfaces. The latest version of umdnsd and udhcpsnoop are required for the plugins to work.
