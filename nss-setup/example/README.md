---
# **Configuring VLANs with OpenWRT on NSS-Enabled Devices**
## **Important Note:** No VLAN Filtering for NSS Devices

For **NSS setups**, you **cannot** enable VLAN filtering directly because **NSS handles VLAN internally** through the module `kmod-qca-nss-drv-vlan-mgr`.
Enabling VLAN filtering via the standard method will interfere with the NSS. To check whether it's enabled, follow the steps below.
---

### 1. **Check if VLAN Filtering is Enabled**
Run the following command to check if VLAN filtering is active:
```vim
uci show network | grep vlan_filtering
```
If you see a line like this:
```vim
network.@device[0].vlan_filtering='1'
```
VLAN filtering is enabled and you need to **disable it**. Follow the next steps.

### 2. **Disable VLAN Filtering**
## **VLAN Tagging Restrictions**
To disable VLAN filtering, run:
```vim
uci del 'network.@device[0].vlan_filtering'
uci commit network
service network restart
```
Alternatively, you can reboot your device to apply the changes:
```vim
reboot
```
---
## **VLAN Tagging Restrictions**

NSS does not support VLAN tagging in the usual way. This means **you can't use tags like `u*` (untagged) or `t` (tagged)** in your configuration.

**Example of what to avoid**:
```bash
list ports 'lan1:u*'
list ports 'lan2:t'
list ports 'lan3:t'
```
---

## **How to Set Up VLANs on NSS-Enabled Devices**

Instead of tagging, you'll need to follow a different approach. Here's a step-by-step process:

1. **Set up VLANs on specific ports.**
2. **Bridge these VLANs into interfaces** (you can leave them **unmanaged** if needed).
3. **Create firewall rules** to manage traffic between VLANs.

---

## **Example VLAN Setup**

Below is an example of how you can configure your VLANs on an OpenWRT router with **NSS support**. This setup includes:

- A **Primary Network** on VLAN 10 (untagged).
- A **Guest Network** on VLAN 30.
- An **IoT Network** on VLAN 40.

### **Network Configuration**
This is an example of `/etc/config/network`:
```vim
config interface 'loopback'
    option device 'lo'
    option proto 'static'
    option ipaddr '127.0.0.1'
    option netmask '255.0.0.0'

config globals 'globals'
    option ula_prefix 'fd32:aa0c:9a35::/48'

config device
    option name 'br-lan'
    option type 'bridge'
    list ports 'lan1'
    list ports 'lan2'
    list ports 'lan3'
    list ports 'lan4'
    list ports 'wan'
    option igmp_snooping '1'

config device
    option name 'lan1'
    option macaddr 'AA:BB:CC:DD:EE:FF'

config device
    option name 'lan2'
    option macaddr 'AA:BB:CC:DD:EE:FF'

config device
    option name 'lan3'
    option macaddr 'AA:BB:CC:DD:EE:FF'

config device
    option name 'lan4'
    option macaddr 'AA:BB:CC:DD:EE:FF'

config interface 'lan'
    option device 'br-lan'
    option proto 'static'
    option ip6assign '60'
    list ipaddr '192.168.1.1/24'
    option force_link '0'

config device
    option type '8021q'
    option ifname 'wan'
    option vid '30'
    option name 'wan.30'

config device
    option type '8021q'
    option ifname 'wan'
    option vid '40'
    option name 'wan.40'

config device
    option type 'bridge'
    option name 'br-iot'
    list ports 'wan.40'
    option igmp_snooping '1'

config device
    option type 'bridge'
    option name 'br-guest'
    list ports 'wan.30'
    option igmp_snooping '1'

config interface 'guest'
    option proto 'none'
    option device 'br-guest'

config interface 'iot'
    option proto 'none'
    option device 'br-iot'
```
### **Explanation**

- **Primary Network (VLAN 10)**: Connected through the LAN ports, untagged.
- **Guest Network (VLAN 30)**: Runs on a bridge called `br-guest` and spans across the **WAN** port tagged with VLAN ID 30.
- **IoT Network (VLAN 40)**: Runs on a bridge called `br-iot` and uses VLAN ID 40 on the **WAN** port.
---

## **WiFi Configuration**

Here is an example of how to configure the WiFi interfaces for different networks in `/etc/config/wireless`:
```vim
config wifi-iface 'default_radio0'
    option device 'radio0'
    option mode 'ap'
    option network 'lan'
    option ssid 'OpenWrt'
    option encryption 'psk2'
    option key '********'
    option ocv '0'
    option bss_transition '1'
    option dtim_period '3'

config wifi-iface 'guest'
    option device 'radio0'
    option mode 'ap'
    option network 'br-guest'
    option ssid 'OpenWrt-Guest'
    option encryption 'psk2'
    option key '********'
    option ocv '0'
    option bss_transition '1'
    option dtim_period '3'

config wifi-iface 'iot'
    option device 'radio0'
    option mode 'ap'
    option network 'br-iot'
    option ssid 'OpenWrt-IoT'
    option encryption 'psk2'
    option key '********'
    option ocv '0'
    option bss_transition '1'
    option dtim_period '3'
```
### **Explanation**

- **LAN WiFi**: The `default_radio0` interface is associated with the **Primary Network** on **VLAN 10**.
- **Guest WiFi**: The `guest` interface is associated with the **Guest Network** on **VLAN 30**.
- **IoT WiFi**: The `iot` interface is associated with the **IoT Network** on **VLAN 40**.
---

## **Advanced VLAN Setup for DMZ**
To create a DMZ using **VLAN 30** on **LAN ports 3 and 4**, you can modify your configuration like this:
```vim
config device
    option name 'br-lan'
    option type 'bridge'
    list ports 'lan1'
    list ports 'lan2'
    list ports 'wan'

config device
    option type '8021q'
    option ifname 'wan'
    option vid '30'
    option name 'dmz.30'

config device
    option type 'bridge'
    option name 'br-dmz'
    list ports 'dmz.30'
    list ports 'lan3'
    list ports 'lan4'

config interface 'dmz'
    option proto 'none'
    option device 'br-dmz'
```
