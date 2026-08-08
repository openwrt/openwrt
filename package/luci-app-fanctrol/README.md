# luci-app-fanctrol

适配 BPI-R3-MINI 风扇，兼容使用相反 cooling-state 顺序的 5.4 与 6.6 设备树。

使用设备的 `pwm-fan` thermal cooling device 作为控制后端，界面和功能基于
`luci-app-fancontrol`，适配 Argon 主题。

Included functions:

- Quiet, balanced and performance modes
- Enable/disable control
- Manual percentage control
- Preset curves
- Editable temperature/fan curve
- CPU temperature and logical fan output status
- Automatic normal/inverted PWM cooling-state detection
- Runtime write/readback check with original-state restoration
- UCI hot reload without repeatedly restarting the service

The package does not query modem temperature.

## PWM 检查

刷机后在路由器上运行：

```sh
/usr/bin/fancontrol --check
```

`result=ok` 与 `write_restore_check=ok` 表示内核的 `pwm-fan` cooling device
可写、写入值可读回，并且探测结束后已恢复原状态。`direction` 会显示
`normal` 或 `inverted`。这只能验证内核控制链路；若硬件没有转速反馈线，仍需
现场观察风扇是否实际启停或测量 PWM 引脚，不能仅凭该命令宣称实体风扇已转动。

默认 `option direction 'auto'` 需要对应 hwmon 的 `pwm1` 可读。若某个内核没有
暴露 `pwm1`，必须在 `/etc/config/fancontrol` 显式设置 `normal` 或 `inverted`；
插件不会在 5.4/6.6 方向不明时冒险猜测。

## OpenWrt 编译

将整个 `luci-app-fanctrol` 文件夹放入 OpenWrt 源码的 `package/` 目录：

```sh
make package/luci-app-fanctrol/compile V=s
```

生成的 IPK 位于 `bin/packages/` 对应架构目录。
