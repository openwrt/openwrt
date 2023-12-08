# Device tree bindings
These are set in the device tree under the `min-rpm` and `max-rpm` properties.

When min-rpm and max-rpm are set, the driver enable RPM/speed based control in
the EMC2301 and register a cooling device with the kernel (if CONFIG_THERMAL set).

     fanctrl:  emc2301@2f {
         reg = <0x2f>;
         compatible = "microchip,emc2301";
         #cooling-cells = <0x02>;

         fan@0 {
           min-rpm = /bits/ 16 <3500>;
           max-rpm = /bits/ 16 <5500>;
         };
     };

The CPU device (or whatever is the thermal subsystem device) needs to be setup to use the fan. This is how we did it for the LS1088:
```
thermal_zone0 {
  trips {
    fanmid0: fanmid0 {
      temperature = <60000>;
      hysteresis = <2000>;
      type = "active";
    };
    fanmax0: fanmax0 {
      temperature = <70000>;
      hysteresis = <2000>;
      type = "active";
    };
  };
  cooling-maps {
    map2 {
      trip = <&fanmid0>;
      cooling-device = <&fanctrl 2 6>;
    };
    map3 {
      trip = <&fanmax0>;
      cooling-device = <&fanctrl 7 THERMAL_NO_LIMIT>;
    };
  };
};
```

The driver will present eight cooling 'steps' to the cooling subsystem between the min-rpm and max-rpm.

# Reading current RPM
You can read the current RPM from the `fan1_input` file:

     $ cat /sys/class/hwmon/hwmon2/fan1_input
     3398

You can also read the current target RPM:

     $ cat /sys/class/hwmon/hwmon2/fan1_target
     3300

As well as forcing a new target. This will remain in effect until the cooling subsystem wants to change, see below for information on disabling automatic control.

     $ echo 9000 > /sys/class/hwmon/hwmon2/fan1_target

# Disabling cooling device support

If you don't want to use the automatic cooling control in Linux, you can disable registration of a cooling device by supplying the parameter 'register_cdev'

    rmmod emc2301
    modprobe emc2301 register_cdev=0

The EMC2301 does not have any hardware inputs - so the fan speed is entirely controlled by software. Be careful not to overheat your CPU. The thermal subsystem may use other measures, such as cpufreq, if the temperature gets too high.
