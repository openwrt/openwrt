# luci-app-frpc

Luci support for Frpc

[![Release Version](https://img.shields.io/github/release/kuoruan/luci-app-frpc.svg)](https://github.com/kuoruan/luci-app-frpc/releases/latest) [![Latest Release Download](https://img.shields.io/github/downloads/kuoruan/luci-app-frpc/latest/total.svg)](https://github.com/kuoruan/luci-app-frpc/releases/latest)

## Install

1. Download ipk files from [release page](https://github.com/kuoruan/luci-app-frpc/releases)

2. Upload files to your router.

3. Install package with opkg:

```sh
opkg install luci-app-frpc_*.ipk
```

For Simplified Chinese, please install ```luci-i18n-zh-cn```.

## Configure

1. Download client file from Frp release [link](https://github.com/fatedier/frp/releases) or Frp ipk release [link](https://github.com/kuoruan/openwrt-frp/releases).

2. Upload the client file to your router, or install the ipk file.

3. Config client file path in luci.

4. Add your server and proxy rules.

5. Enable the client.
