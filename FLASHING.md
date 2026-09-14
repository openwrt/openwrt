# GL-MT3600BE 刷机与验证指引

## 0. 硬件确认
- 机型: GL.iNet Beryl 7 (GL-MT3600BE), BE3600 双频
- SoC: MediaTek MT7987A + MT7990
- 存储: 512MB NAND / 512MB DDR4
- 网口: 1x 2.5G (SoC) + 1x 2.5G (RTL8221B)
- 固件: 25.12.5 起原生支持 (commit c8aa9be)

## 1. 首次刷机: U-Boot Recovery (推荐)
1. 断电, 按住 Reset 孔 5 秒再上电
2. PC 网卡设静态 IP: `192.168.1.10/24`
3. 浏览器开 `http://192.168.1.1`
4. 上传 `openwrt-mediatek-filogic-glinet_gl-mt3600be-squashfs-sysupgrade.bin`
5. 等待 ~3 分钟, 自动重启

## 2. Web 升级 (从已有 OpenWrt)
- 系统 → 备份与升级 → 镜像 → **取消勾选 "保留配置"**
- 上传 sysupgrade.bin, 确认刷入

## 3. 刷前必做: 分区一致性核对
在官方/原固件 SSH 执行:
```
dmesg | grep -A12 "fixed-partitions\|nmbm_spim_nand"
```
确认分区表与下一致 (25.12.5 DTS `mt7987a-glinet-gl-mt3600be.dts`):
```
BL2        0x00000000 - 0x00100000  (1MB)
u-boot-env 0x00100000 - 0x00180000  (512KB)
Factory    0x00180000 - 0x00580000  (4MB)
FIP        0x00580000 - 0x00780000  (2MB)
log        0x00780000 - 0x007c0000  (256KB)
CFG        0x007c0000 - 0x00800000  (256KB)
ubi        0x00800000 - end
```
分区不一致 → 停止刷机, 避免砖机。

## 4. 首次启动验证清单
```bash
# SSH 登入 192.168.1.1
uname -a                    # 确认内核 6.12 + 25.12.5
cat /etc/openwrt_release
lsmod | grep -E "mt76|nf_|fullcone"   # 驱动/模块加载
dmesg | grep -iE "mt7987|mt7990|eth0|2.5G"  # 硬件识别
logread | grep -i "procd\|netifd"       # 服务正常
```

## 5. 基础功能回归
- [ ] LuCI 可访问 (https://192.168.1.1)
- [ ] 双频 WiFi7 (2.4G 688 + 5G 2882) 可搜到
- [ ] 双 2.5G 网口 link up
- [ ] PPPoE / DHCP 上网正常
- [ ] IPv6 (odhcpd) 获取前缀
- [ ] NAT 转发 (全锥需 `kmod-nft-fullcone` 加载)
- [ ] USB 存储挂载
- [ ] fwx Dashboard / AppFilter (若启用)

## 6. 回退
- Recovery 模式可随时刷回官方固件 (分区一致前提下)
- 建议首次刷前用官方工具**完整备份** Factory 分区 (含 MAC/校准)
```

## 7. 注意事项
- 25.12 默认包管理器为 **apk** (非 opkg)
- 全锥 NAT: 优先用 feed 的 `luci-app-fullconenat`, 不要手动覆盖内核
- OAF 特征文件: **个人免费, 商用禁止**; 分发固件须保留版权并注明仓库
- 构建失败优先看 `build.log` 末尾 50 行
