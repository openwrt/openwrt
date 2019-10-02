# openwrt-frp

Frpc/Frps for OpenWrt

OpenWrt/LEDE 上可用的 Frp 客户端/服务端

[![Release Version](https://img.shields.io/github/release/kuoruan/openwrt-frp.svg)](https://github.com/kuoruan/openwrt-frp/releases/latest) [![Latest Release Download](https://img.shields.io/github/downloads/kuoruan/openwrt-frp/total.svg)](https://github.com/kuoruan/openwrt-frp/releases/latest)

## 安装说明

1. 到 [release](https://github.com/kuoruan/openwrt-frp/releases) 页面下载最新版的```frpc``` 或 ```frps```（注：请根据你的路由器架构下载对应版本）

2. 将文件上传到你的路由器上，进行安装

```sh
opkg install frpc_*.ipk
opkg install frps_*.ipk
```

安装完毕，你可以在 ```/usr/bin``` 目录下找到对应的二进制文件。

获取 Luci 请访问：[kuoruan/luci-app-frpc](https://github.com/kuoruan/luci-app-frpc)

## 编译说明

请使用最新版的 OpenWrt SDK 或 master 版源代码。

进入 SDK 根目录或源码根目录，执行命令下载 Makefile：

```sh
git clone https://github.com/kuoruan/openwrt-frp.git package/frp
```

编译流程：

```sh
./scripts/feeds update -a
./scripts/feeds install -a

make menuconfig

Languages  ---> Go  ---> <M> golang-github-fatedier-frp-dev # 源码包，通常并不需要
Network  ---> Web Servers/Proxies  ---> <*> frpc
Network  ---> Web Servers/Proxies  ---> <*> frps

make package/frp/{clean,compile} V=s
```

## 卸载说明

```sh
opkg remove frpc
opkg remove frps
```
