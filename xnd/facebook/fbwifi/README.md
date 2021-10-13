# Facebook Wi-Fi v2.0 Reference Implementation for OpenWrt 

## Getting started

Case studies for OEM customers are available at the official page of [Facebook Wi-Fi](https://www.facebook.com/facebook-wifi).

For OEM engineers, start by reading the init script in [files/etc/init.d/fbwifi](https://github.com/facebookincubator/fbc_owrt_feed/blob/master/fbwifi/files/etc/init.d/fbwifi)

To enable Facebook Wi-Fi, configure the gateway_token in `/etc/config/fbwifi`, and run `fbwifi enable`.
To disable Facebook Wi-Fi, run `fbwifi disable`.

## Contents

The 'files' subdirectory contains all the configuration, script and code 
that implements the Facebook Wi-Fi v2.0 standard for OpenWrt.

The folder structure follows *nix conventions :
- 'etc' is the boot time scripts and configuration
- 'usr' contains procedural scripts, lua common code module and GUI prototype for luci
- 'www' contains the HTTP endpoints as CGI handlers 

```
files/
├── etc
│   ├── config
│   │   └── fbwifi
│   ├── hotplug.d
│   │   └── iface
│   │       └── 50-fbwifi
│   ├── init.d
│   │   └── fbwifi
├── usr
│   ├── lib
│   │   └── lua
│   │       ├── fbwifi.lua
│   │       └── luci
│   │           ├── controller
│   │           │   └── fbwifi.lua
│   │           └── view
│   │               └── fbwifi.htm
│   ├── sbin
│   │   ├── fbwifi
│   │   ├── fbwifi_debug_dump
│   │   ├── fbwifi_gateway_info_update
│   │   ├── fbwifi_get_config
│   │   └── fbwifi_validate_token_db
│   └── share
│       └── fbwifi
│           ├── firewall.include
│           └── uhttpd.json
└── www
    └── cgi-bin
        └── fbwifi
            └── v2.0
                ├── auth
                ├── capport
                └── info
```
