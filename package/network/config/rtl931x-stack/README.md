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

## Bridge offload

Peer ports can join the same VLAN-aware bridge as local DSA ports. The driver
programs VLAN membership, CIST state and device-aware isolation matrices on
both switches. It also enables stack source learning while the distributed
bridge exists. Local-to-remote, remote-to-local and remote-to-remote traffic
can therefore remain in the RTL931x forwarding engines instead of traversing
the leader CPU.

This proof of concept deliberately supports a narrow bridge configuration:

- one distributed bridge;
- 802.1Q VLAN filtering enabled;
- CIST only, with MST disabled;
- multicast snooping disabled; and
- default learning, flood and port-isolation flags.

Set `option igmp_snooping '0'` explicitly on the bridge while testing. MDB
offload, user-installed static FDB entries, non-default bridge-port flags,
multiple distributed bridges and remote LAGs are not implemented yet. Dynamic
unicast learning and unknown unicast, multicast and broadcast forwarding are
offloaded.

Follower front-panel link changes are carried to the leader as session-fenced,
sequenced Device Talk events. The leader also reconciles the complete carrier
bitmap periodically, so a lost event or a transiently busy receiver does not
leave representor carrier state stale.

The administrative state of each leader representor controls the corresponding
follower MAC and phylink. Absolute full-bitmap mutations and inventory readback
keep that state convergent after a lost reply or stack-link recovery.

These features update the private Device Talk protocol. Both members must run
an image built from the same revision before enabling the stack.

## Peer representor contract

The leader-side `sw1pN` devices are stack-specific remote bridge
representors. They are not full DSA user ports. Their current contract
includes the Ethernet data path, remote administrative state, live carrier
state, fixed inventory MAC address and MTU, and the bridge offload subset
described above.

The following DSA-port features are not currently provided:

- live PHY configuration and most ethtool operations;
- pause, EEE, WOL and hardware statistics;
- MTU and MAC-address changes;
- TC and switchdev offloads outside the supported bridge subset;
- MST and VLAN-unaware bridges; and
- remote LAG, mirror and traffic-control configuration.

Unsupported netdevice operations return `EOPNOTSUPP`. The physical port name
uses `d<device>p<port>`, and the representor iflink identifies its stack
fabric port. The Device Talk protocol version and the capability bitmap shown
by `rtl931x-stack peer-switch` negotiate the peer operations required before
representors are activated. Those capabilities do not claim general DSA-port
parity.

If these representors remain a public interface, they will need a versioned
per-netdevice feature ABI before their contract grows. A future cascaded-DSA
implementation should instead expose remote ports through normal DSA APIs;
duplicating the complete DSA ethtool, PHY and offload surface in this
experimental representor layer is deliberately out of scope.

## Diagnostic client

The package installs `rtl931x-stack` for manual inspection and recovery. Run it
without arguments to list the available commands. The normal boot path is
owned by `rtl931x-stackd`; manual commands should be reserved for development
and fault diagnosis.
