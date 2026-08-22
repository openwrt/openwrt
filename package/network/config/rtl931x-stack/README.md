# RTL931x switch stacking

This package contains the userspace manager and diagnostic client for the
experimental RTL931x two-member switch stack. The kernel driver supplies the
stack transport, peer-port representors and a private CPU Ethernet endpoint.

This is proof-of-concept code. It currently assumes two RTL931x members, one
physical fabric link and fixed leader/follower roles. Do not treat it as a
general high-availability stacking implementation.

## Boot sequence

`rtl931x-stackd` and netifd start independently. The package installs a netifd
configuration selector which reads the configured stack role before netifd
parses `/etc/config/network`. It does not wait for the fabric or peer.

The leader loads its complete network configuration immediately. References to
peer representors remain pending until stackd creates those netdevs. A follower
loads a restricted configuration profile immediately, before its front-panel
ports can be configured. The loopback interface and global network settings are
retained automatically. Other network UCI sections required on the follower
must be assigned to the selected profile. The `rtl931x.stack.ready` ubus object
reports stack health and does not gate network startup.

The default follower profile is `rtl931x-stack`. It may be changed with the
`netifd_profile` option in `/etc/config/rtl931x-stack`.

## Management network

Each configured member exposes `stack0`. It is a virtual Linux Ethernet device
representing that member CPU on the real stack fabric. Ethernet frames are
carried unchanged, so normal 802.1Q subinterfaces can provide a private
stack-wide management network.

For example, the leader may add `stack0` as a tagged member of management VLAN
100 on its normal bridge:

```uci
config bridge-vlan 'management_vlan'
	option device 'switch'
	option vlan '100'
	list ports 'lan48:u*'
	list ports 'stack0:t'
```

The follower can assign its own address from the same management subnet to a
tagged stack device. Every section required on the restricted follower must
carry the profile option:

```uci
config device 'stack_management_vlan'
	option name 'stack0.100'
	option type '8021q'
	option ifname 'stack0'
	option vid '100'
	option profile 'rtl931x-stack'

config interface 'stack_management'
	option device 'stack0.100'
	option proto 'static'
	option ipaddr '10.50.16.128'
	option netmask '255.255.255.0'
	option profile 'rtl931x-stack'
```

Routes, rules, aliases or additional devices needed by the follower must also
be tagged with the same profile. Wireless and dynamically injected procd
network configuration are deliberately not loaded in restricted mode.

There is no uci-defaults script for this setup: the bridge name, VLAN ID,
management subnet and per-member address are deployment policy and cannot be
chosen safely by the package.

## Basic configuration

The stack configuration names the fabric DSA port and assigns the two device
IDs. Exactly one enabled `stack` section is supported:

```uci
config stack 'main'
	option enabled '1'
	option interface 'lan49'
	option member_id '1'
	option peer_id '0'
	option master_id '0'
	option generation '1'
	option flags '0'
	option boot_policy 'wait'
	option ready_timeout '60'
	option netifd_profile 'rtl931x-stack'
```

The peer uses the inverse member and peer IDs with the same master, generation
and flags. The physical fabric interface must be administratively usable and
must not belong to a bridge or LAG before stacking starts.

The `wait` policy retries stack convergence indefinitely. `fallback` restores
standalone switch state after `ready_timeout`, while `fail` stops automatic
retries. These policies do not broaden the network configuration selected at
startup. A configured follower remains fail-closed if stacking fails or falls
back; omitted standalone configuration is never activated automatically. To
return a follower to standalone networking, disable stacking and reload or
restart netifd explicitly.

## Diagnostic client

The package installs `rtl931x-stack` for manual inspection and recovery. Run it
without arguments to list the available commands. The normal boot path is
owned by `rtl931x-stackd`; manual commands should be reserved for development
and fault diagnosis.
