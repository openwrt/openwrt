# luci-app-fanctrol for EdgePi E87N

版本：`3.2.14-7`

该包保留最终版风扇控制中心的界面和交互功能，底层硬件访问已全部改为
EdgePi E87N 实现，不包含其他机型的 PWM、无线或模组温度定义。

## E87N 硬件规则

- PWM 使用 E87N DTS 的正向 `pwm-fan` 冷却档位：`state 0` 停转，
  `max_state` 全速。
- 温度来源固定为处理器、网口 PHY、NVMe 1、NVMe 2。
- CPU 读取 `cpu-thermal`/SoC thermal zone。
- 网口 PHY 读取 `mdio_bus:*` hwmon。
- 两块 NVMe 均读取各自的 `temp1_input` 复合温度，并按 hwmon 数字编号排序；
  该顺序与屏幕上的 N1/N2 一致。
- 未检测到的 PHY 或 NVMe 来源会自动隐藏，设备出现后自动显示。

## 功能

- 静音、均衡、性能和自定义模式。
- 手动百分比控制和可拖动温度-转速曲线。
- 模式、开关、滑块、温度来源和曲线在操作完成后自动提交 UCI 并立即生效。
- 保存后的模式、曲线、开关和温度来源在重启或断电后保留。
- 单温度来源控制；当前来源失效时进入 100% 全速保护。
- 新安装默认使用处理器温度，默认模式为均衡。
- 页面独占控制，避免多个浏览器同时覆盖曲线。
- 主包内置简体中文，不需要独立的风扇中文包。
- 升级时保留已有 `/etc/config/fancontrol`，sysupgrade 同样保留配置。

## 自测

```sh
python3 tools/test_release_3_2_14.py
python3 tools/test_fan_power_states.py
python3 tools/test_temperature_sources.py
python3 tools/test_upgrade_paths.py
```

其中温度来源测试会构造乱序的 `hwmon2`/`hwmon10`，确认下拉菜单的
NVMe 1/NVMe 2 与屏幕 N1/N2 使用相同的数字排序规则。

## 生成一体 IPK

```sh
python3 tools/build_ipk.py
```

辅助脚本会先运行全部测试，再生成主包 IPK 和源码 ZIP。主包提供
`luci-i18n-fanctrol-zh-cn` 能力，内置翻译文件为
`fancontrol-bundled.zh-cn.lmo`。
