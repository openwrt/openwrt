<img src="https://openwrt.org/_media/logo.png" alt="logo" width="274" height="84" align="right">

# OpenWrt项目

这个项目fork自[OpenWrt](https://github.com/openwrt/openwrt)

默认登录地址: http://10.0.0.1, 用户名: __root__, 密码: __password__.

## 如何编译自己需要的 OpenWrt 固件
### 快速开始
1. 执行命令 `git clone -b <branch> --single-branch https://github.com/YukPingFong/openwrt.git` 下载源码.
2. 执行命令 `cd openwrt` 进入源码目录.
3. 执行命令 `./scripts/feeds update -a` 获取feeds.conf / feeds.conf.default中定义的最新的包.
4. 执行命令 `./scripts/feeds install -a` to install symlinks for all obtained packages into package/feeds/
5. 执行命令 `make menuconfig` 选择你需要的toolchain, target system和firmware包生成配置文件.
6. 执行命令 `make download -j8 V=s` 下载源码、交叉编译工具链、Linux内核等(需要能访问国际互联网, 此过程可能会有部分文件下载失败, 为保守起见最好执行2遍).
7. 执行命令 `make -j8 V=s` 编译固件.

## 许可
OpenWrt is licensed under [GPL-3.0-only](https://spdx.org/licenses/GPL-3.0-only.html).
