'use strict';
'require uci';
'require rpc';
'require validation';

var proto_errors = {
	CONNECT_FAILED:			_('Connection attempt failed'),
	INVALID_ADDRESS:		_('IP address in invalid'),
	INVALID_GATEWAY:		_('Gateway address is invalid'),
	INVALID_LOCAL_ADDRESS:	_('Local IP address is invalid'),
	MISSING_ADDRESS:		_('IP address is missing'),
	MISSING_PEER_ADDRESS:	_('Peer address is missing'),
	NO_DEVICE:				_('Network device is not present'),
	NO_IFACE:				_('Unable to determine device name'),
	NO_IFNAME:				_('Unable to determine device name'),
	NO_WAN_ADDRESS:			_('Unable to determine external IP address'),
	NO_WAN_LINK:			_('Unable to determine upstream interface'),
	PEER_RESOLVE_FAIL:		_('Unable to resolve peer host name'),
	PIN_FAILED:				_('PIN code rejected')
};

var iface_patterns_ignore = [
	/^wmaster\d+/,
	/^wifi\d+/,
	/^hwsim\d+/,
	/^imq\d+/,
	/^ifb\d+/,
	/^mon\.wlan\d+/,
	/^sit\d+/,
	/^gre\d+/,
	/^gretap\d+/,
	/^ip6gre\d+/,
	/^ip6tnl\d+/,
	/^tunl\d+/,
	/^lo$/
];

var iface_patterns_wireless = [
	/^wlan\d+/,
	/^wl\d+/,
	/^ath\d+/,
	/^\w+\.network\d+/
];

var iface_patterns_virtual = [ ];

var callLuciNetworkDevices = rpc.declare({
	object: 'luci-rpc',
	method: 'getNetworkDevices',
	expect: { '': {} }
});

var callLuciWirelessDevices = rpc.declare({
	object: 'luci-rpc',
	method: 'getWirelessDevices',
	expect: { '': {} }
});

var callLuciBoardJSON = rpc.declare({
	object: 'luci-rpc',
	method: 'getBoardJSON'
});

var callLuciHostHints = rpc.declare({
	object: 'luci-rpc',
	method: 'getHostHints',
	expect: { '': {} }
});

var callIwinfoAssoclist = rpc.declare({
	object: 'iwinfo',
	method: 'assoclist',
	params: [ 'device', 'mac' ],
	expect: { results: [] }
});

var callIwinfoScan = rpc.declare({
	object: 'iwinfo',
	method: 'scan',
	params: [ 'device' ],
	nobatch: true,
	expect: { results: [] }
});

var callNetworkInterfaceDump = rpc.declare({
	object: 'network.interface',
	method: 'dump',
	expect: { 'interface': [] }
});

var callNetworkProtoHandlers = rpc.declare({
	object: 'network',
	method: 'get_proto_handlers',
	expect: { '': {} }
});

var _init = null,
    _state = null,
    _protocols = {},
    _protospecs = {};

function getProtocolHandlers(cache) {
	return callNetworkProtoHandlers().then(function(protos) {
		/* Register "none" protocol */
		if (!protos.hasOwnProperty('none'))
			Object.assign(protos, { none: { no_device: false } });

		/* Hack: emulate relayd protocol */
		if (!protos.hasOwnProperty('relay'))
			Object.assign(protos, { relay: { no_device: true } });

		Object.assign(_protospecs, protos);

		return Promise.all(Object.keys(protos).map(function(p) {
			return Promise.resolve(L.require('protocol.%s'.format(p))).catch(function(err) {
				if (L.isObject(err) && err.name != 'NetworkError')
					L.error(err);
			});
		})).then(function() {
			return protos;
		});
	}).catch(function() {
		return {};
	});
}

function getWifiStateBySid(sid) {
	var s = uci.get('wireless', sid);

	if (s != null && s['.type'] == 'wifi-iface') {
		for (var radioname in _state.radios) {
			for (var i = 0; i < _state.radios[radioname].interfaces.length; i++) {
				var netstate = _state.radios[radioname].interfaces[i];

				if (typeof(netstate.section) != 'string')
					continue;

				var s2 = uci.get('wireless', netstate.section);

				if (s2 != null && s['.type'] == s2['.type'] && s['.name'] == s2['.name']) {
					if (s2['.anonymous'] == false && netstate.section.charAt(0) == '@')
						return null;

					return [ radioname, _state.radios[radioname], netstate ];
				}
			}
		}
	}

	return null;
}

function getWifiStateByIfname(ifname) {
	for (var radioname in _state.radios) {
		for (var i = 0; i < _state.radios[radioname].interfaces.length; i++) {
			var netstate = _state.radios[radioname].interfaces[i];

			if (typeof(netstate.ifname) != 'string')
				continue;

			if (netstate.ifname == ifname)
				return [ radioname, _state.radios[radioname], netstate ];
		}
	}

	return null;
}

function isWifiIfname(ifname) {
	for (var i = 0; i < iface_patterns_wireless.length; i++)
		if (iface_patterns_wireless[i].test(ifname))
			return true;

	return false;
}

function getWifiSidByNetid(netid) {
	var m = /^(\w+)\.network(\d+)$/.exec(netid);
	if (m) {
		var sections = uci.sections('wireless', 'wifi-iface');
		for (var i = 0, n = 0; i < sections.length; i++) {
			if (sections[i].device != m[1])
				continue;

			if (++n == +m[2])
				return sections[i]['.name'];
		}
	}

	return null;
}

function getWifiSidByIfname(ifname) {
	var sid = getWifiSidByNetid(ifname);

	if (sid != null)
		return sid;

	var res = getWifiStateByIfname(ifname);

	if (res != null && L.isObject(res[2]) && typeof(res[2].section) == 'string')
		return res[2].section;

	return null;
}

function getWifiNetidBySid(sid) {
	var s = uci.get('wireless', sid);
	if (s != null && s['.type'] == 'wifi-iface') {
		var radioname = s.device;
		if (typeof(s.device) == 'string') {
			var i = 0, netid = null, sections = uci.sections('wireless', 'wifi-iface');
			for (var i = 0, n = 0; i < sections.length; i++) {
				if (sections[i].device != s.device)
					continue;

				n++;

				if (sections[i]['.name'] != s['.name'])
					continue;

				return [ '%s.network%d'.format(s.device, n), s.device ];
			}

		}
	}

	return null;
}

function getWifiNetidByNetname(name) {
	var sections = uci.sections('wireless', 'wifi-iface');
	for (var i = 0; i < sections.length; i++) {
		if (typeof(sections[i].network) != 'string')
			continue;

		var nets = sections[i].network.split(/\s+/);
		for (var j = 0; j < nets.length; j++) {
			if (nets[j] != name)
				continue;

			return getWifiNetidBySid(sections[i]['.name']);
		}
	}

	return null;
}

function isVirtualIfname(ifname) {
	for (var i = 0; i < iface_patterns_virtual.length; i++)
		if (iface_patterns_virtual[i].test(ifname))
			return true;

	return false;
}

function isIgnoredIfname(ifname) {
	for (var i = 0; i < iface_patterns_ignore.length; i++)
		if (iface_patterns_ignore[i].test(ifname))
			return true;

	return false;
}

function appendValue(config, section, option, value) {
	var values = uci.get(config, section, option),
	    isArray = Array.isArray(values),
	    rv = false;

	if (isArray == false)
		values = L.toArray(values);

	if (values.indexOf(value) == -1) {
		values.push(value);
		rv = true;
	}

	uci.set(config, section, option, isArray ? values : values.join(' '));

	return rv;
}

function removeValue(config, section, option, value) {
	var values = uci.get(config, section, option),
	    isArray = Array.isArray(values),
	    rv = false;

	if (isArray == false)
		values = L.toArray(values);

	for (var i = values.length - 1; i >= 0; i--) {
		if (values[i] == value) {
			values.splice(i, 1);
			rv = true;
		}
	}

	if (values.length > 0)
		uci.set(config, section, option, isArray ? values : values.join(' '));
	else
		uci.unset(config, section, option);

	return rv;
}

function prefixToMask(bits, v6) {
	var w = v6 ? 128 : 32,
	    m = [];

	if (bits > w)
		return null;

	for (var i = 0; i < w / 16; i++) {
		var b = Math.min(16, bits);
		m.push((0xffff << (16 - b)) & 0xffff);
		bits -= b;
	}

	if (v6)
		return String.prototype.format.apply('%x:%x:%x:%x:%x:%x:%x:%x', m).replace(/:0(?::0)+$/, '::');
	else
		return '%d.%d.%d.%d'.format(m[0] >>> 8, m[0] & 0xff, m[1] >>> 8, m[1] & 0xff);
}

function maskToPrefix(mask, v6) {
	var m = v6 ? validation.parseIPv6(mask) : validation.parseIPv4(mask);

	if (!m)
		return null;

	var bits = 0;

	for (var i = 0, z = false; i < m.length; i++) {
		z = z || !m[i];

		while (!z && (m[i] & (v6 ? 0x8000 : 0x80))) {
			m[i] = (m[i] << 1) & (v6 ? 0xffff : 0xff);
			bits++;
		}

		if (m[i])
			return null;
	}

	return bits;
}

function initNetworkState(refresh) {
	if (_state == null || refresh) {
		_init = _init || Promise.all([
			L.resolveDefault(callNetworkInterfaceDump(), []),
			L.resolveDefault(callLuciBoardJSON(), {}),
			L.resolveDefault(callLuciNetworkDevices(), {}),
			L.resolveDefault(callLuciWirelessDevices(), {}),
			L.resolveDefault(callLuciHostHints(), {}),
			getProtocolHandlers(),
			uci.load(['network', 'wireless', 'luci'])
		]).then(function(data) {
			var netifd_ifaces = data[0],
			    board_json    = data[1],
			    luci_devs     = data[2];

			var s = {
				isTunnel: {}, isBridge: {}, isSwitch: {}, isWifi: {},
				ifaces: netifd_ifaces, radios: data[3], hosts: data[4],
				netdevs: {}, bridges: {}, switches: {}, hostapd: {}
			};

			for (var name in luci_devs) {
				var dev = luci_devs[name];

				if (isVirtualIfname(name))
					s.isTunnel[name] = true;

				if (!s.isTunnel[name] && isIgnoredIfname(name))
					continue;

				s.netdevs[name] = s.netdevs[name] || {
					idx:      dev.ifindex,
					name:     name,
					rawname:  name,
					flags:    dev.flags,
					stats:    dev.stats,
					macaddr:  dev.mac,
					type:     dev.type,
					mtu:      dev.mtu,
					qlen:     dev.qlen,
					wireless: dev.wireless,
					ipaddrs:  [],
					ip6addrs: []
				};

				if (Array.isArray(dev.ipaddrs))
					for (var i = 0; i < dev.ipaddrs.length; i++)
						s.netdevs[name].ipaddrs.push(dev.ipaddrs[i].address + '/' + dev.ipaddrs[i].netmask);

				if (Array.isArray(dev.ip6addrs))
					for (var i = 0; i < dev.ip6addrs.length; i++)
						s.netdevs[name].ip6addrs.push(dev.ip6addrs[i].address + '/' + dev.ip6addrs[i].netmask);
			}

			for (var name in luci_devs) {
				var dev = luci_devs[name];

				if (!dev.bridge)
					continue;

				var b = {
					name:    name,
					id:      dev.id,
					stp:     dev.stp,
					ifnames: []
				};

				for (var i = 0; dev.ports && i < dev.ports.length; i++) {
					var subdev = s.netdevs[dev.ports[i]];

					if (subdev == null)
						continue;

					b.ifnames.push(subdev);
					subdev.bridge = b;
				}

				s.bridges[name] = b;
				s.isBridge[name] = true;
			}

			if (L.isObject(board_json.switch)) {
				for (var switchname in board_json.switch) {
					var layout = board_json.switch[switchname],
					    netdevs = {},
					    nports = {},
					    ports = [],
					    pnum = null,
					    role = null;

					if (L.isObject(layout) && Array.isArray(layout.ports)) {
						for (var i = 0, port; (port = layout.ports[i]) != null; i++) {
							if (typeof(port) == 'object' && typeof(port.num) == 'number' &&
								(typeof(port.role) == 'string' || typeof(port.device) == 'string')) {
								var spec = {
									num:   port.num,
									role:  port.role || 'cpu',
									index: (port.index != null) ? port.index : port.num
								};

								if (port.device != null) {
									spec.device = port.device;
									spec.tagged = spec.need_tag;
									netdevs[port.num] = port.device;
								}

								ports.push(spec);

								if (port.role != null)
									nports[port.role] = (nports[port.role] || 0) + 1;
							}
						}

						ports.sort(function(a, b) {
							if (a.role != b.role)
								return (a.role < b.role) ? -1 : 1;

							return (a.index - b.index);
						});

						for (var i = 0, port; (port = ports[i]) != null; i++) {
							if (port.role != role) {
								role = port.role;
								pnum = 1;
							}

							if (role == 'cpu')
								port.label = 'CPU (%s)'.format(port.device);
							else if (nports[role] > 1)
								port.label = '%s %d'.format(role.toUpperCase(), pnum++);
							else
								port.label = role.toUpperCase();

							delete port.role;
							delete port.index;
						}

						s.switches[switchname] = {
							ports: ports,
							netdevs: netdevs
						};
					}
				}
			}

			if (L.isObject(board_json.dsl) && L.isObject(board_json.dsl.modem)) {
				s.hasDSLModem = board_json.dsl.modem;
			}

			_init = null;

			var objects = [];

			if (L.isObject(s.radios))
				for (var radio in s.radios)
					if (L.isObject(s.radios[radio]) && Array.isArray(s.radios[radio].interfaces))
						for (var i = 0; i < s.radios[radio].interfaces.length; i++)
							if (L.isObject(s.radios[radio].interfaces[i]) && s.radios[radio].interfaces[i].ifname)
								objects.push('hostapd.%s'.format(s.radios[radio].interfaces[i].ifname));

			return (objects.length ? L.resolveDefault(rpc.list.apply(rpc, objects), {}) : Promise.resolve({})).then(function(res) {
				for (var k in res) {
					var m = k.match(/^hostapd\.(.+)$/);
					if (m)
						s.hostapd[m[1]] = res[k];
				}

				return (_state = s);
			});
		});
	}

	return (_state != null ? Promise.resolve(_state) : _init);
}

function ifnameOf(obj) {
	if (obj instanceof Protocol)
		return obj.getIfname();
	else if (obj instanceof Device)
		return obj.getName();
	else if (obj instanceof WifiDevice)
		return obj.getName();
	else if (obj instanceof WifiNetwork)
		return obj.getIfname();
	else if (typeof(obj) == 'string')
		return obj.replace(/:.+$/, '');

	return null;
}

function networkSort(a, b) {
	return a.getName() > b.getName();
}

function deviceSort(a, b) {
	var typeWeigth = { wifi: 2, alias: 3 },
        weightA = typeWeigth[a.getType()] || 1,
        weightB = typeWeigth[b.getType()] || 1;

    if (weightA != weightB)
    	return weightA - weightB;

	return a.getName() > b.getName();
}

function formatWifiEncryption(enc) {
	if (!L.isObject(enc))
		return null;

	if (!enc.enabled)
		return 'None';

	var ciphers = Array.isArray(enc.ciphers)
		? enc.ciphers.map(function(c) { return c.toUpperCase() }) : [ 'NONE' ];

	if (Array.isArray(enc.wep)) {
		var has_open = false,
		    has_shared = false;

		for (var i = 0; i < enc.wep.length; i++)
			if (enc.wep[i] == 'open')
				has_open = true;
			else if (enc.wep[i] == 'shared')
				has_shared = true;

		if (has_open && has_shared)
			return 'WEP Open/Shared (%s)'.format(ciphers.join(', '));
		else if (has_open)
			return 'WEP Open System (%s)'.format(ciphers.join(', '));
		else if (has_shared)
			return 'WEP Shared Auth (%s)'.format(ciphers.join(', '));

		return 'WEP';
	}

	if (Array.isArray(enc.wpa)) {
		var versions = [],
		    suites = Array.isArray(enc.authentication)
			? enc.authentication.map(function(a) { return a.toUpperCase() }) : [ 'NONE' ];

		for (var i = 0; i < enc.wpa.length; i++)
			switch (enc.wpa[i]) {
			case 1:
				versions.push('WPA');
				break;

			default:
				versions.push('WPA%d'.format(enc.wpa[i]));
				break;
			}

		if (versions.length > 1)
			return 'mixed %s %s (%s)'.format(versions.join('/'), suites.join(', '), ciphers.join(', '));

		return '%s %s (%s)'.format(versions[0], suites.join(', '), ciphers.join(', '));
	}

	return 'Unknown';
}

function enumerateNetworks() {
	var uciInterfaces = uci.sections('network', 'interface'),
	    networks = {};

	for (var i = 0; i < uciInterfaces.length; i++)
		networks[uciInterfaces[i]['.name']] = this.instantiateNetwork(uciInterfaces[i]['.name']);

	for (var i = 0; i < _state.ifaces.length; i++)
		if (networks[_state.ifaces[i].interface] == null)
			networks[_state.ifaces[i].interface] =
				this.instantiateNetwork(_state.ifaces[i].interface, _state.ifaces[i].proto);

	var rv = [];

	for (var network in networks)
		if (networks.hasOwnProperty(network))
			rv.push(networks[network]);

	rv.sort(networkSort);

	return rv;
}


var Hosts, Network, Protocol, Device, WifiDevice, WifiNetwork;

/**
 * @class
 * @memberof LuCI
 * @hideconstructor
 * @classdesc
 *
 * The `LuCI.Network` class combines data from multiple `ubus` apis to
 * provide an abstraction of the current network configuration state.
 *
 * It provides methods to enumerate interfaces and devices, to query
 * current configuration details and to manipulate settings.
 */
Network = L.Class.extend(/** @lends LuCI.Network.prototype */ {
	/**
	 * Converts the given prefix size in bits to a netmask.
	 *
	 * @method
	 *
	 * @param {number} bits
	 * The prefix size in bits.
	 *
	 * @param {boolean} [v6=false]
	 * Whether to convert the bits value into an IPv4 netmask (`false`) or
	 * an IPv6 netmask (`true`).
	 *
	 * @returns {null|string}
	 * Returns a string containing the netmask corresponding to the bit count
	 * or `null` when the given amount of bits exceeds the maximum possible
	 * value of `32` for IPv4 or `128` for IPv6.
	 */
	prefixToMask: prefixToMask,

	/**
	 * Converts the given netmask to a prefix size in bits.
	 *
	 * @method
	 *
	 * @param {string} netmask
	 * The netmask to convert into a bit count.
	 *
	 * @param {boolean} [v6=false]
	 * Whether to parse the given netmask as IPv4 (`false`) or IPv6 (`true`)
	 * address.
	 *
	 * @returns {null|number}
	 * Returns the number of prefix bits contained in the netmask or `null`
	 * if the given netmask value was invalid.
	 */
	maskToPrefix: maskToPrefix,

	/**
	 * An encryption entry describes active wireless encryption settings
	 * such as the used key management protocols, active ciphers and
	 * protocol versions.
	 *
	 * @typedef {Object<string, boolean|Array<number|string>>} LuCI.Network.WifiEncryption
	 * @memberof LuCI.Network
	 *
	 * @property {boolean} enabled
	 * Specifies whether any kind of encryption, such as `WEP` or `WPA` is
	 * enabled. If set to `false`, then no encryption is active and the
	 * corresponding network is open.
	 *
	 * @property {string[]} [wep]
	 * When the `wep` property exists, the network uses WEP encryption.
	 * In this case, the property is set to an array of active WEP modes
	 * which might be either `open`, `shared` or both.
	 *
	 * @property {number[]} [wpa]
	 * When the `wpa` property exists, the network uses WPA security.
	 * In this case, the property is set to an array containing the WPA
	 * protocol versions used, e.g. `[ 1, 2 ]` for WPA/WPA2 mixed mode or
	 * `[ 3 ]` for WPA3-SAE.
	 *
	 * @property {string[]} [authentication]
	 * The `authentication` property only applies to WPA encryption and
	 * is defined when the `wpa` property is set as well. It points to
	 * an array of active authentication suites used by the network, e.g.
	 * `[ "psk" ]` for a WPA(2)-PSK network or `[ "psk", "sae" ]` for
	 * mixed WPA2-PSK/WPA3-SAE encryption.
	 *
	 * @property {string[]} [ciphers]
	 * If either WEP or WPA encryption is active, then the `ciphers`
	 * property will be set to an array describing the active encryption
	 * ciphers used by the network, e.g. `[ "tkip", "ccmp" ]` for a
	 * WPA/WPA2-PSK mixed network or `[ "wep-40", "wep-104" ]` for an
	 * WEP network.
	 */

	/**
	 * Converts a given {@link LuCI.Network.WifiEncryption encryption entry}
	 * into a human readable string such as `mixed WPA/WPA2 PSK (TKIP, CCMP)`
	 * or `WPA3 SAE (CCMP)`.
	 *
	 * @method
	 *
	 * @param {LuCI.Network.WifiEncryption} encryption
	 * The wireless encryption entry to convert.
	 *
	 * @returns {null|string}
	 * Returns the description string for the given encryption entry or
	 * `null` if the given entry was invalid.
	 */
	formatWifiEncryption: formatWifiEncryption,

	/**
	 * Flushes the local network state cache and fetches updated information
	 * from the remote `ubus` apis.
	 *
	 * @returns {Promise<Object>}
	 * Returns a promise resolving to the internal network state object.
	 */
	flushCache: function() {
		initNetworkState(true);
		return _init;
	},

	/**
	 * Instantiates the given {@link LuCI.Network.Protocol Protocol} backend,
	 * optionally using the given network name.
	 *
	 * @param {string} protoname
	 * The protocol backend to use, e.g. `static` or `dhcp`.
	 *
	 * @param {string} [netname=__dummy__]
	 * The network name to use for the instantiated protocol. This should be
	 * usually set to one of the interfaces described in /etc/config/network
	 * but it is allowed to omit it, e.g. to query protocol capabilities
	 * without the need for an existing interface.
	 *
	 * @returns {null|LuCI.Network.Protocol}
	 * Returns the instantiated protocol backend class or `null` if the given
	 * protocol isn't known.
	 */
	getProtocol: function(protoname, netname) {
		var v = _protocols[protoname];
		if (v != null)
			return new v(netname || '__dummy__');

		return null;
	},

	/**
	 * Obtains instances of all known {@link LuCI.Network.Protocol Protocol}
	 * backend classes.
	 *
	 * @returns {Array<LuCI.Network.Protocol>}
	 * Returns an array of protocol class instances.
	 */
	getProtocols: function() {
		var rv = [];

		for (var protoname in _protocols)
			rv.push(new _protocols[protoname]('__dummy__'));

		return rv;
	},

	/**
	 * Registers a new {@link LuCI.Network.Protocol Protocol} subclass
	 * with the given methods and returns the resulting subclass value.
	 *
	 * This functions internally calls
	 * {@link LuCI.Class.extend Class.extend()} on the `Network.Protocol`
	 * base class.
	 *
	 * @param {string} protoname
	 * The name of the new protocol to register.
	 *
	 * @param {Object<string, *>} methods
	 * The member methods and values of the new `Protocol` subclass to
	 * be passed to {@link LuCI.Class.extend Class.extend()}.
	 *
	 * @returns {LuCI.Network.Protocol}
	 * Returns the new `Protocol` subclass.
	 */
	registerProtocol: function(protoname, methods) {
		var spec = L.isObject(_protospecs) ? _protospecs[protoname] : null;
		var proto = Protocol.extend(Object.assign({
			getI18n: function() {
				return protoname;
			},

			isFloating: function() {
				return false;
			},

			isVirtual: function() {
				return (L.isObject(spec) && spec.no_device == true);
			},

			renderFormOptions: function(section) {

			}
		}, methods, {
			__init__: function(name) {
				this.sid = name;
			},

			getProtocol: function() {
				return protoname;
			}
		}));

		_protocols[protoname] = proto;

		return proto;
	},

	/**
	 * Registers a new regular expression pattern to recognize
	 * virtual interfaces.
	 *
	 * @param {RegExp} pat
	 * A `RegExp` instance to match a virtual interface name
	 * such as `6in4-wan` or `tun0`.
	 */
	registerPatternVirtual: function(pat) {
		iface_patterns_virtual.push(pat);
	},

	/**
	 * Registers a new human readable translation string for a `Protocol`
	 * error code.
	 *
	 * @param {string} code
	 * The `ubus` protocol error code to register a translation for, e.g.
	 * `NO_DEVICE`.
	 *
	 * @param {string} message
	 * The message to use as translation for the given protocol error code.
	 *
	 * @returns {boolean}
	 * Returns `true` if the error code description has been added or `false`
	 * if either the arguments were invalid or if there already was a
	 * description for the given code.
	 */
	registerErrorCode: function(code, message) {
		if (typeof(code) == 'string' &&
		    typeof(message) == 'string' &&
		    !proto_errors.hasOwnProperty(code)) {
			proto_errors[code] = message;
			return true;
		}

		return false;
	},

	/**
	 * Adds a new network of the given name and update it with the given
	 * uci option values.
	 *
	 * If a network with the given name already exist but is empty, then
	 * this function will update its option, otherwise it will do nothing.
	 *
	 * @param {string} name
	 * The name of the network to add. Must be in the format `[a-zA-Z0-9_]+`.
	 *
	 * @param {Object<string, string|string[]>} [options]
	 * An object of uci option values to set on the new network or to
	 * update in an existing, empty network.
	 *
	 * @returns {Promise<null|LuCI.Network.Protocol>}
	 * Returns a promise resolving to the `Protocol` subclass instance
	 * describing the added network or resolving to `null` if the name
	 * was invalid or if a non-empty network of the given name already
	 * existed.
	 */
	addNetwork: function(name, options) {
		return this.getNetwork(name).then(L.bind(function(existingNetwork) {
			if (name != null && /^[a-zA-Z0-9_]+$/.test(name) && existingNetwork == null) {
				var sid = uci.add('network', 'interface', name);

				if (sid != null) {
					if (L.isObject(options))
						for (var key in options)
							if (options.hasOwnProperty(key))
								uci.set('network', sid, key, options[key]);

					return this.instantiateNetwork(sid);
				}
			}
			else if (existingNetwork != null && existingNetwork.isEmpty()) {
				if (L.isObject(options))
					for (var key in options)
						if (options.hasOwnProperty(key))
							existingNetwork.set(key, options[key]);

				return existingNetwork;
			}
		}, this));
	},

	/**
	 * Get a {@link LuCI.Network.Protocol Protocol} instance describing
	 * the network with the given name.
	 *
	 * @param {string} name
	 * The logical interface name of the network get, e.g. `lan` or `wan`.
	 *
	 * @returns {Promise<null|LuCI.Network.Protocol>}
	 * Returns a promise resolving to a
	 * {@link LuCI.Network.Protocol Protocol} subclass instance describing
	 * the network or `null` if the network did not exist.
	 */
	getNetwork: function(name) {
		return initNetworkState().then(L.bind(function() {
			var section = (name != null) ? uci.get('network', name) : null;

			if (section != null && section['.type'] == 'interface') {
				return this.instantiateNetwork(name);
			}
			else if (name != null) {
				for (var i = 0; i < _state.ifaces.length; i++)
					if (_state.ifaces[i].interface == name)
						return this.instantiateNetwork(name, _state.ifaces[i].proto);
			}

			return null;
		}, this));
	},

	/**
	 * Gets an array containing all known networks.
	 *
	 * @returns {Promise<Array<LuCI.Network.Protocol>>}
	 * Returns a promise resolving to a name-sorted array of
	 * {@link LuCI.Network.Protocol Protocol} subclass instances
	 * describing all known networks.
	 */
	getNetworks: function() {
		return initNetworkState().then(L.bind(enumerateNetworks, this));
	},

	/**
	 * Deletes the given network and its references from the network and
	 * firewall configuration.
	 *
	 * @param {string} name
	 * The name of the network to delete.
	 *
	 * @returns {Promise<boolean>}
	 * Returns a promise resolving to either `true` if the network and
	 * references to it were successfully deleted from the configuration or
	 * `false` if the given network could not be found.
	 */
	deleteNetwork: function(name) {
		var requireFirewall = Promise.resolve(L.require('firewall')).catch(function() {});

		return Promise.all([ requireFirewall, initNetworkState() ]).then(function() {
			var uciInterface = uci.get('network', name);

			if (uciInterface != null && uciInterface['.type'] == 'interface') {
				uci.remove('network', name);

				uci.sections('luci', 'ifstate', function(s) {
					if (s.interface == name)
						uci.remove('luci', s['.name']);
				});

				uci.sections('network', 'alias', function(s) {
					if (s.interface == name)
						uci.remove('network', s['.name']);
				});

				uci.sections('network', 'route', function(s) {
					if (s.interface == name)
						uci.remove('network', s['.name']);
				});

				uci.sections('network', 'route6', function(s) {
					if (s.interface == name)
						uci.remove('network', s['.name']);
				});

				uci.sections('wireless', 'wifi-iface', function(s) {
					var networks = L.toArray(s.network).filter(function(network) { return network != name });

					if (networks.length > 0)
						uci.set('wireless', s['.name'], 'network', networks.join(' '));
					else
						uci.unset('wireless', s['.name'], 'network');
				});

				if (L.firewall)
					return L.firewall.deleteNetwork(name).then(function() { return true });

				return true;
			}

			return false;
		});
	},

	/**
	 * Rename the given network and its references to a new name.
	 *
	 * @param {string} oldName
	 * The current name of the network.
	 *
	 * @param {string} newName
	 * The name to rename the network to, must be in the format
	 * `[a-z-A-Z0-9_]+`.
	 *
	 * @returns {Promise<boolean>}
	 * Returns a promise resolving to either `true` if the network was
	 * successfully renamed or `false` if the new name was invalid, if
	 * a network with the new name already exists or if the network to
	 * rename could not be found.
	 */
	renameNetwork: function(oldName, newName) {
		return initNetworkState().then(function() {
			if (newName == null || !/^[a-zA-Z0-9_]+$/.test(newName) || uci.get('network', newName) != null)
				return false;

			var oldNetwork = uci.get('network', oldName);

			if (oldNetwork == null || oldNetwork['.type'] != 'interface')
				return false;

			var sid = uci.add('network', 'interface', newName);

			for (var key in oldNetwork)
				if (oldNetwork.hasOwnProperty(key) && key.charAt(0) != '.')
					uci.set('network', sid, key, oldNetwork[key]);

			uci.sections('luci', 'ifstate', function(s) {
				if (s.interface == oldName)
					uci.set('luci', s['.name'], 'interface', newName);
			});

			uci.sections('network', 'alias', function(s) {
				if (s.interface == oldName)
					uci.set('network', s['.name'], 'interface', newName);
			});

			uci.sections('network', 'route', function(s) {
				if (s.interface == oldName)
					uci.set('network', s['.name'], 'interface', newName);
			});

			uci.sections('network', 'route6', function(s) {
				if (s.interface == oldName)
					uci.set('network', s['.name'], 'interface', newName);
			});

			uci.sections('wireless', 'wifi-iface', function(s) {
				var networks = L.toArray(s.network).map(function(network) { return (network == oldName ? newName : network) });

				if (networks.length > 0)
					uci.set('wireless', s['.name'], 'network', networks.join(' '));
			});

			uci.remove('network', oldName);

			return true;
		});
	},

	/**
	 * Get a {@link LuCI.Network.Device Device} instance describing the
	 * given network device.
	 *
	 * @param {string} name
	 * The name of the network device to get, e.g. `eth0` or `br-lan`.
	 *
	 * @returns {Promise<null|LuCI.Network.Device>}
	 * Returns a promise resolving to the `Device` instance describing
	 * the network device or `null` if the given device name could not
	 * be found.
	 */
	getDevice: function(name) {
		return initNetworkState().then(L.bind(function() {
			if (name == null)
				return null;

			if (_state.netdevs.hasOwnProperty(name) || isWifiIfname(name))
				return this.instantiateDevice(name);

			var netid = getWifiNetidBySid(name);
			if (netid != null)
				return this.instantiateDevice(netid[0]);

			return null;
		}, this));
	},

	/**
	 * Get a sorted list of all found network devices.
	 *
	 * @returns {Promise<Array<LuCI.Network.Device>>}
	 * Returns a promise resolving to a sorted array of `Device` class
	 * instances describing the network devices found on the system.
	 */
	getDevices: function() {
		return initNetworkState().then(L.bind(function() {
			var devices = {};

			/* find simple devices */
			var uciInterfaces = uci.sections('network', 'interface');
			for (var i = 0; i < uciInterfaces.length; i++) {
				var ifnames = L.toArray(uciInterfaces[i].ifname);

				for (var j = 0; j < ifnames.length; j++) {
					if (ifnames[j].charAt(0) == '@')
						continue;

					if (isIgnoredIfname(ifnames[j]) || isVirtualIfname(ifnames[j]) || isWifiIfname(ifnames[j]))
						continue;

					devices[ifnames[j]] = this.instantiateDevice(ifnames[j]);
				}
			}

			for (var ifname in _state.netdevs) {
				if (devices.hasOwnProperty(ifname))
					continue;

				if (isIgnoredIfname(ifname) || isWifiIfname(ifname))
					continue;

				if (_state.netdevs[ifname].wireless)
					continue;

				devices[ifname] = this.instantiateDevice(ifname);
			}

			/* find VLAN devices */
			var uciSwitchVLANs = uci.sections('network', 'switch_vlan');
			for (var i = 0; i < uciSwitchVLANs.length; i++) {
				if (typeof(uciSwitchVLANs[i].ports) != 'string' ||
				    typeof(uciSwitchVLANs[i].device) != 'string' ||
				    !_state.switches.hasOwnProperty(uciSwitchVLANs[i].device))
					continue;

				var ports = uciSwitchVLANs[i].ports.split(/\s+/);
				for (var j = 0; j < ports.length; j++) {
					var m = ports[j].match(/^(\d+)([tu]?)$/);
					if (m == null)
						continue;

					var netdev = _state.switches[uciSwitchVLANs[i].device].netdevs[m[1]];
					if (netdev == null)
						continue;

					if (!devices.hasOwnProperty(netdev))
						devices[netdev] = this.instantiateDevice(netdev);

					_state.isSwitch[netdev] = true;

					if (m[2] != 't')
						continue;

					var vid = uciSwitchVLANs[i].vid || uciSwitchVLANs[i].vlan;
					    vid = (vid != null ? +vid : null);

					if (vid == null || vid < 0 || vid > 4095)
						continue;

					var vlandev = '%s.%d'.format(netdev, vid);

					if (!devices.hasOwnProperty(vlandev))
						devices[vlandev] = this.instantiateDevice(vlandev);

					_state.isSwitch[vlandev] = true;
				}
			}

			/* find wireless interfaces */
			var uciWifiIfaces = uci.sections('wireless', 'wifi-iface'),
			    networkCount = {};

			for (var i = 0; i < uciWifiIfaces.length; i++) {
				if (typeof(uciWifiIfaces[i].device) != 'string')
					continue;

				networkCount[uciWifiIfaces[i].device] = (networkCount[uciWifiIfaces[i].device] || 0) + 1;

				var netid = '%s.network%d'.format(uciWifiIfaces[i].device, networkCount[uciWifiIfaces[i].device]);

				devices[netid] = this.instantiateDevice(netid);
			}

			var rv = [];

			for (var netdev in devices)
				if (devices.hasOwnProperty(netdev))
					rv.push(devices[netdev]);

			rv.sort(deviceSort);

			return rv;
		}, this));
	},

	/**
	 * Test if a given network device name is in the list of patterns for
	 * device names to ignore.
	 *
	 * Ignored device names are usually Linux network devices which are
	 * spawned implicitly by kernel modules such as `tunl0` or `hwsim0`
	 * and which are unsuitable for use in network configuration.
	 *
	 * @param {string} name
	 * The device name to test.
	 *
	 * @returns {boolean}
	 * Returns `true` if the given name is in the ignore pattern list,
	 * else returns `false`.
	 */
	isIgnoredDevice: function(name) {
		return isIgnoredIfname(name);
	},

	/**
	 * Get a {@link LuCI.Network.WifiDevice WifiDevice} instance describing
	 * the given wireless radio.
	 *
	 * @param {string} devname
	 * The configuration name of the wireless radio to lookup, e.g. `radio0`
	 * for the first mac80211 phy on the system.
	 *
	 * @returns {Promise<null|LuCI.Network.WifiDevice>}
	 * Returns a promise resolving to the `WifiDevice` instance describing
	 * the underlying radio device or `null` if the wireless radio could not
	 * be found.
	 */
	getWifiDevice: function(devname) {
		return initNetworkState().then(L.bind(function() {
			var existingDevice = uci.get('wireless', devname);

			if (existingDevice == null || existingDevice['.type'] != 'wifi-device')
				return null;

			return this.instantiateWifiDevice(devname, _state.radios[devname] || {});
		}, this));
	},

	/**
	 * Obtain a list of all configured radio devices.
	 *
	 * @returns {Promise<Array<LuCI.Network.WifiDevice>>}
	 * Returns a promise resolving to an array of `WifiDevice` instances
	 * describing the wireless radios configured in the system.
	 * The order of the array corresponds to the order of the radios in
	 * the configuration.
	 */
	getWifiDevices: function() {
		return initNetworkState().then(L.bind(function() {
			var uciWifiDevices = uci.sections('wireless', 'wifi-device'),
			    rv = [];

			for (var i = 0; i < uciWifiDevices.length; i++) {
				var devname = uciWifiDevices[i]['.name'];
				rv.push(this.instantiateWifiDevice(devname, _state.radios[devname] || {}));
			}

			return rv;
		}, this));
	},

	/**
	 * Get a {@link LuCI.Network.WifiNetwork WifiNetwork} instance describing
	 * the given wireless network.
	 *
	 * @param {string} netname
	 * The name of the wireless network to lookup. This may be either an uci
	 * configuration section ID, a network ID in the form `radio#.network#`
	 * or a Linux network device name like `wlan0` which is resolved to the
	 * corresponding configuration section through `ubus` runtime information.
	 *
	 * @returns {Promise<null|LuCI.Network.WifiNetwork>}
	 * Returns a promise resolving to the `WifiNetwork` instance describing
	 * the wireless network or `null` if the corresponding network could not
	 * be found.
	 */
	getWifiNetwork: function(netname) {
		return initNetworkState()
			.then(L.bind(this.lookupWifiNetwork, this, netname));
	},

	/**
	 * Get an array of all {@link LuCI.Network.WifiNetwork WifiNetwork}
	 * instances describing the wireless networks present on the system.
	 *
	 * @returns {Promise<Array<LuCI.Network.WifiNetwork>>}
	 * Returns a promise resolving to an array of `WifiNetwork` instances
	 * describing the wireless networks. The array will be empty if no networks
	 * are found.
	 */
	getWifiNetworks: function() {
		return initNetworkState().then(L.bind(function() {
			var wifiIfaces = uci.sections('wireless', 'wifi-iface'),
			    rv = [];

			for (var i = 0; i < wifiIfaces.length; i++)
				rv.push(this.lookupWifiNetwork(wifiIfaces[i]['.name']));

			rv.sort(function(a, b) {
				return (a.getID() > b.getID());
			});

			return rv;
		}, this));
	},

	/**
	 * Adds a new wireless network to the configuration and sets its options
	 * to the provided values.
	 *
	 * @param {Object<string, string|string[]>} options
	 * The options to set for the newly added wireless network. This object
	 * must at least contain a `device` property which is set to the radio
	 * name the new network belongs to.
	 *
	 * @returns {Promise<null|LuCI.Network.WifiNetwork>}
	 * Returns a promise resolving to a `WifiNetwork` instance describing
	 * the newly added wireless network or `null` if the given options
	 * were invalid or if the associated radio device could not be found.
	 */
	addWifiNetwork: function(options) {
		return initNetworkState().then(L.bind(function() {
			if (options == null ||
			    typeof(options) != 'object' ||
			    typeof(options.device) != 'string')
			    return null;

			var existingDevice = uci.get('wireless', options.device);
			if (existingDevice == null || existingDevice['.type'] != 'wifi-device')
				return null;

			/* XXX: need to add a named section (wifinet#) here */
			var sid = uci.add('wireless', 'wifi-iface');
			for (var key in options)
				if (options.hasOwnProperty(key))
					uci.set('wireless', sid, key, options[key]);

			var radioname = existingDevice['.name'],
			    netid = getWifiNetidBySid(sid) || [];

			return this.instantiateWifiNetwork(sid, radioname, _state.radios[radioname], netid[0], null);
		}, this));
	},

	/**
	 * Deletes the given wireless network from the configuration.
	 *
	 * @param {string} netname
	 * The name of the network to remove. This may be either a
	 * network ID in the form `radio#.network#` or a Linux network device
	 * name like `wlan0` which is resolved to the corresponding configuration
	 * section through `ubus` runtime information.
	 *
	 * @returns {Promise<boolean>}
	 * Returns a promise resolving to `true` if the wireless network has been
	 * successfully deleted from the configuration or `false` if it could not
	 * be found.
	 */
	deleteWifiNetwork: function(netname) {
		return initNetworkState().then(L.bind(function() {
			var sid = getWifiSidByIfname(netname);

			if (sid == null)
				return false;

			uci.remove('wireless', sid);
			return true;
		}, this));
	},

	/* private */
	getStatusByRoute: function(addr, mask) {
		return initNetworkState().then(L.bind(function() {
			var rv = [];

			for (var i = 0; i < _state.ifaces.length; i++) {
				if (!Array.isArray(_state.ifaces[i].route))
					continue;

				for (var j = 0; j < _state.ifaces[i].route.length; j++) {
					if (typeof(_state.ifaces[i].route[j]) != 'object' ||
					    typeof(_state.ifaces[i].route[j].target) != 'string' ||
					    typeof(_state.ifaces[i].route[j].mask) != 'number')
					    continue;

					if (_state.ifaces[i].route[j].table)
						continue;

					if (_state.ifaces[i].route[j].target != addr ||
					    _state.ifaces[i].route[j].mask != mask)
					    continue;

					rv.push(_state.ifaces[i]);
				}
			}

			return rv;
		}, this));
	},

	/* private */
	getStatusByAddress: function(addr) {
		return initNetworkState().then(L.bind(function() {
			var rv = [];

			for (var i = 0; i < _state.ifaces.length; i++) {
				if (Array.isArray(_state.ifaces[i]['ipv4-address']))
					for (var j = 0; j < _state.ifaces[i]['ipv4-address'].length; j++)
						if (typeof(_state.ifaces[i]['ipv4-address'][j]) == 'object' &&
						    _state.ifaces[i]['ipv4-address'][j].address == addr)
							return _state.ifaces[i];

				if (Array.isArray(_state.ifaces[i]['ipv6-address']))
					for (var j = 0; j < _state.ifaces[i]['ipv6-address'].length; j++)
						if (typeof(_state.ifaces[i]['ipv6-address'][j]) == 'object' &&
						    _state.ifaces[i]['ipv6-address'][j].address == addr)
							return _state.ifaces[i];

				if (Array.isArray(_state.ifaces[i]['ipv6-prefix-assignment']))
					for (var j = 0; j < _state.ifaces[i]['ipv6-prefix-assignment'].length; j++)
						if (typeof(_state.ifaces[i]['ipv6-prefix-assignment'][j]) == 'object' &&
							typeof(_state.ifaces[i]['ipv6-prefix-assignment'][j]['local-address']) == 'object' &&
						    _state.ifaces[i]['ipv6-prefix-assignment'][j]['local-address'].address == addr)
							return _state.ifaces[i];
			}

			return null;
		}, this));
	},

	/**
	 * Get IPv4 wan networks.
	 *
	 * This function looks up all networks having a default `0.0.0.0/0` route
	 * and returns them as array.
	 *
	 * @returns {Promise<Array<LuCI.Network.Protocol>>}
	 * Returns a promise resolving to an array of `Protocol` subclass
	 * instances describing the found default route interfaces.
	 */
	getWANNetworks: function() {
		return this.getStatusByRoute('0.0.0.0', 0).then(L.bind(function(statuses) {
			var rv = [], seen = {};

			for (var i = 0; i < statuses.length; i++) {
				if (!seen.hasOwnProperty(statuses[i].interface)) {
					rv.push(this.instantiateNetwork(statuses[i].interface, statuses[i].proto));
					seen[statuses[i].interface] = true;
				}
			}

			return rv;
		}, this));
	},

	/**
	 * Get IPv6 wan networks.
	 *
	 * This function looks up all networks having a default `::/0` route
	 * and returns them as array.
	 *
	 * @returns {Promise<Array<LuCI.Network.Protocol>>}
	 * Returns a promise resolving to an array of `Protocol` subclass
	 * instances describing the found IPv6 default route interfaces.
	 */
	getWAN6Networks: function() {
		return this.getStatusByRoute('::', 0).then(L.bind(function(statuses) {
			var rv = [], seen = {};

			for (var i = 0; i < statuses.length; i++) {
				if (!seen.hasOwnProperty(statuses[i].interface)) {
					rv.push(this.instantiateNetwork(statuses[i].interface, statuses[i].proto));
					seen[statuses[i].interface] = true;
				}
			}

			return rv;
		}, this));
	},

	/**
	 * Describes an swconfig switch topology by specifying the CPU
	 * connections and external port labels of a switch.
	 *
	 * @typedef {Object<string, Object|Array>} SwitchTopology
	 * @memberof LuCI.Network
	 *
	 * @property {Object<number, string>} netdevs
	 * The `netdevs` property points to an object describing the CPU port
	 * connections of the switch. The numeric key of the enclosed object is
	 * the port number, the value contains the Linux network device name the
	 * port is hardwired to.
	 *
	 * @property {Array<Object<string, boolean|number|string>>} ports
	 * The `ports` property points to an array describing the populated
	 * ports of the switch in the external label order. Each array item is
	 * an object containg the following keys:
	 *  - `num` - the internal switch port number
	 *  - `label` - the label of the port, e.g. `LAN 1` or `CPU (eth0)`
	 *  - `device` - the connected Linux network device name (CPU ports only)
	 *  - `tagged` - a boolean indicating whether the port must be tagged to
	 *     function (CPU ports only)
	 */

	/**
	 * Returns the topologies of all swconfig switches found on the system.
	 *
	 * @returns {Promise<Object<string, LuCI.Network.SwitchTopology>>}
	 * Returns a promise resolving to an object containing the topologies
	 * of each switch. The object keys correspond to the name of the switches
	 * such as `switch0`, the values are
	 * {@link LuCI.Network.SwitchTopology SwitchTopology} objects describing
	 * the layout.
	 */
	getSwitchTopologies: function() {
		return initNetworkState().then(function() {
			return _state.switches;
		});
	},

	/* private */
	instantiateNetwork: function(name, proto) {
		if (name == null)
			return null;

		proto = (proto == null ? uci.get('network', name, 'proto') : proto);

		var protoClass = _protocols[proto] || Protocol;
		return new protoClass(name);
	},

	/* private */
	instantiateDevice: function(name, network, extend) {
		if (extend != null)
			return new (Device.extend(extend))(name, network);

		return new Device(name, network);
	},

	/* private */
	instantiateWifiDevice: function(radioname, radiostate) {
		return new WifiDevice(radioname, radiostate);
	},

	/* private */
	instantiateWifiNetwork: function(sid, radioname, radiostate, netid, netstate, hostapd) {
		return new WifiNetwork(sid, radioname, radiostate, netid, netstate, hostapd);
	},

	/* private */
	lookupWifiNetwork: function(netname) {
		var sid, res, netid, radioname, radiostate, netstate;

		sid = getWifiSidByNetid(netname);

		if (sid != null) {
			res        = getWifiStateBySid(sid);
			netid      = netname;
			radioname  = res ? res[0] : null;
			radiostate = res ? res[1] : null;
			netstate   = res ? res[2] : null;
		}
		else {
			res = getWifiStateByIfname(netname);

			if (res != null) {
				radioname  = res[0];
				radiostate = res[1];
				netstate   = res[2];
				sid        = netstate.section;
				netid      = L.toArray(getWifiNetidBySid(sid))[0];
			}
			else {
				res = getWifiStateBySid(netname);

				if (res != null) {
					radioname  = res[0];
					radiostate = res[1];
					netstate   = res[2];
					sid        = netname;
					netid      = L.toArray(getWifiNetidBySid(sid))[0];
				}
				else {
					res = getWifiNetidBySid(netname);

					if (res != null) {
						netid     = res[0];
						radioname = res[1];
						sid       = netname;
					}
				}
			}
		}

		return this.instantiateWifiNetwork(sid || netname, radioname,
			radiostate, netid, netstate,
			netstate ? _state.hostapd[netstate.ifname] : null);
	},

	/**
	 * Obtains the the network device name of the given object.
	 *
	 * @param {LuCI.Network.Protocol|LuCI.Network.Device|LuCI.Network.WifiDevice|LuCI.Network.WifiNetwork|string} obj
	 * The object to get the device name from.
	 *
	 * @returns {null|string}
	 * Returns a string containing the device name or `null` if the given
	 * object could not be converted to a name.
	 */
	getIfnameOf: function(obj) {
		return ifnameOf(obj);
	},

	/**
	 * Queries the internal DSL modem type from board information.
	 *
	 * @returns {Promise<null|string>}
	 * Returns a promise resolving to the type of the internal modem
	 * (e.g. `vdsl`) or to `null` if no internal modem is present.
	 */
	getDSLModemType: function() {
		return initNetworkState().then(function() {
			return _state.hasDSLModem ? _state.hasDSLModem.type : null;
		});
	},

	/**
	 * Queries aggregated information about known hosts.
	 *
	 * This function aggregates information from various sources such as
	 * DHCP lease databases, ARP and IPv6 neighbour entries, wireless
	 * association list etc. and returns a {@link LuCI.Network.Hosts Hosts}
	 * class instance describing the found hosts.
	 *
	 * @returns {Promise<LuCI.Network.Hosts>}
	 * Returns a `Hosts` instance describing host known on the system.
	 */
	getHostHints: function() {
		return initNetworkState().then(function() {
			return new Hosts(_state.hosts);
		});
	}
});

/**
 * @class
 * @memberof LuCI.Network
 * @hideconstructor
 * @classdesc
 *
 * The `LuCI.Network.Hosts` class encapsulates host information aggregated
 * from multiple sources and provides convenience functions to access the
 * host information by different criteria.
 */
Hosts = L.Class.extend(/** @lends LuCI.Network.Hosts.prototype */ {
	__init__: function(hosts) {
		this.hosts = hosts;
	},

	/**
	 * Lookup the hostname associated with the given MAC address.
	 *
	 * @param {string} mac
	 * The MAC address to lookup.
	 *
	 * @returns {null|string}
	 * Returns the hostname associated with the given MAC or `null` if
	 * no matching host could be found or if no hostname is known for
	 * the corresponding host.
	 */
	getHostnameByMACAddr: function(mac) {
		return this.hosts[mac] ? this.hosts[mac].name : null;
	},

	/**
	 * Lookup the IPv4 address associated with the given MAC address.
	 *
	 * @param {string} mac
	 * The MAC address to lookup.
	 *
	 * @returns {null|string}
	 * Returns the IPv4 address associated with the given MAC or `null` if
	 * no matching host could be found or if no IPv4 address is known for
	 * the corresponding host.
	 */
	getIPAddrByMACAddr: function(mac) {
		return this.hosts[mac] ? this.hosts[mac].ipv4 : null;
	},

	/**
	 * Lookup the IPv6 address associated with the given MAC address.
	 *
	 * @param {string} mac
	 * The MAC address to lookup.
	 *
	 * @returns {null|string}
	 * Returns the IPv6 address associated with the given MAC or `null` if
	 * no matching host could be found or if no IPv6 address is known for
	 * the corresponding host.
	 */
	getIP6AddrByMACAddr: function(mac) {
		return this.hosts[mac] ? this.hosts[mac].ipv6 : null;
	},

	/**
	 * Lookup the hostname associated with the given IPv4 address.
	 *
	 * @param {string} ipaddr
	 * The IPv4 address to lookup.
	 *
	 * @returns {null|string}
	 * Returns the hostname associated with the given IPv4 or `null` if
	 * no matching host could be found or if no hostname is known for
	 * the corresponding host.
	 */
	getHostnameByIPAddr: function(ipaddr) {
		for (var mac in this.hosts)
			if (this.hosts[mac].ipv4 == ipaddr && this.hosts[mac].name != null)
				return this.hosts[mac].name;
		return null;
	},

	/**
	 * Lookup the MAC address associated with the given IPv4 address.
	 *
	 * @param {string} ipaddr
	 * The IPv4 address to lookup.
	 *
	 * @returns {null|string}
	 * Returns the MAC address associated with the given IPv4 or `null` if
	 * no matching host could be found or if no MAC address is known for
	 * the corresponding host.
	 */
	getMACAddrByIPAddr: function(ipaddr) {
		for (var mac in this.hosts)
			if (this.hosts[mac].ipv4 == ipaddr)
				return mac;
		return null;
	},

	/**
	 * Lookup the hostname associated with the given IPv6 address.
	 *
	 * @param {string} ipaddr
	 * The IPv6 address to lookup.
	 *
	 * @returns {null|string}
	 * Returns the hostname associated with the given IPv6 or `null` if
	 * no matching host could be found or if no hostname is known for
	 * the corresponding host.
	 */
	getHostnameByIP6Addr: function(ip6addr) {
		for (var mac in this.hosts)
			if (this.hosts[mac].ipv6 == ip6addr && this.hosts[mac].name != null)
				return this.hosts[mac].name;
		return null;
	},

	/**
	 * Lookup the MAC address associated with the given IPv6 address.
	 *
	 * @param {string} ipaddr
	 * The IPv6 address to lookup.
	 *
	 * @returns {null|string}
	 * Returns the MAC address associated with the given IPv6 or `null` if
	 * no matching host could be found or if no MAC address is known for
	 * the corresponding host.
	 */
	getMACAddrByIP6Addr: function(ip6addr) {
		for (var mac in this.hosts)
			if (this.hosts[mac].ipv6 == ip6addr)
				return mac;
		return null;
	},

	/**
	 * Return an array of (MAC address, name hint) tuples sorted by
	 * MAC address.
	 *
	 * @param {boolean} [preferIp6=false]
	 * Whether to prefer IPv6 addresses (`true`) or IPv4 addresses (`false`)
	 * as name hint when no hostname is known for a specific MAC address.
	 *
	 * @returns {Array<Array<string>>}
	 * Returns an array of arrays containing a name hint for each found
	 * MAC address on the system. The array is sorted ascending by MAC.
	 *
	 * Each item of the resulting array is a two element array with the
	 * MAC being the first element and the name hint being the second
	 * element. The name hint is either the hostname, an IPv4 or an IPv6
	 * address related to the MAC address.
	 *
	 * If no hostname but both IPv4 and IPv6 addresses are known, the
	 * `preferIP6` flag specifies whether the IPv6 or the IPv4 address
	 * is used as hint.
	 */
	getMACHints: function(preferIp6) {
		var rv = [];
		for (var mac in this.hosts) {
			var hint = this.hosts[mac].name ||
				this.hosts[mac][preferIp6 ? 'ipv6' : 'ipv4'] ||
				this.hosts[mac][preferIp6 ? 'ipv4' : 'ipv6'];

			rv.push([mac, hint]);
		}
		return rv.sort(function(a, b) { return a[0] > b[0] });
	}
});

/**
 * @class
 * @memberof LuCI.Network
 * @hideconstructor
 * @classdesc
 *
 * The `Network.Protocol` class serves as base for protocol specific
 * subclasses which describe logical UCI networks defined by `config
 * interface` sections in `/etc/config/network`.
 */
Protocol = L.Class.extend(/** @lends LuCI.Network.Protocol.prototype */ {
	__init__: function(name) {
		this.sid = name;
	},

	_get: function(opt) {
		var val = uci.get('network', this.sid, opt);

		if (Array.isArray(val))
			return val.join(' ');

		return val || '';
	},

	_ubus: function(field) {
		for (var i = 0; i < _state.ifaces.length; i++) {
			if (_state.ifaces[i].interface != this.sid)
				continue;

			return (field != null ? _state.ifaces[i][field] : _state.ifaces[i]);
		}
	},

	/**
	 * Read the given UCI option value of this network.
	 *
	 * @param {string} opt
	 * The UCI option name to read.
	 *
	 * @returns {null|string|string[]}
	 * Returns the UCI option value or `null` if the requested option is
	 * not found.
	 */
	get: function(opt) {
		return uci.get('network', this.sid, opt);
	},

	/**
	 * Set the given UCI option of this network to the given value.
	 *
	 * @param {string} opt
	 * The name of the UCI option to set.
	 *
	 * @param {null|string|string[]} val
	 * The value to set or `null` to remove the given option from the
	 * configuration.
	 */
	set: function(opt, val) {
		return uci.set('network', this.sid, opt, val);
	},

	/**
	 * Get the associared Linux network device of this network.
	 *
	 * @returns {null|string}
	 * Returns the name of the associated network device or `null` if
	 * it could not be determined.
	 */
	getIfname: function() {
		var ifname;

		if (this.isFloating())
			ifname = this._ubus('l3_device');
		else
			ifname = this._ubus('device') || this._ubus('l3_device');

		if (ifname != null)
			return ifname;

		var res = getWifiNetidByNetname(this.sid);
		return (res != null ? res[0] : null);
	},

	/**
	 * Get the name of this network protocol class.
	 *
	 * This function will be overwritten by subclasses created by
	 * {@link LuCI.Network#registerProtocol Network.registerProtocol()}.
	 *
	 * @abstract
	 * @returns {string}
	 * Returns the name of the network protocol implementation, e.g.
	 * `static` or `dhcp`.
	 */
	getProtocol: function() {
		return null;
	},

	/**
	 * Return a human readable description for the protcol, such as
	 * `Static address` or `DHCP client`.
	 *
	 * This function should be overwritten by subclasses.
	 *
	 * @abstract
	 * @returns {string}
	 * Returns the description string.
	 */
	getI18n: function() {
		switch (this.getProtocol()) {
		case 'none':   return _('Unmanaged');
		case 'static': return _('Static address');
		case 'dhcp':   return _('DHCP client');
		default:       return _('Unknown');
		}
	},

	/**
	 * Get the type of the underlying interface.
	 *
	 * This function actually is a convenience wrapper around
	 * `proto.get("type")` and is mainly used by other `LuCI.Network` code
	 * to check whether the interface is declared as bridge in UCI.
	 *
	 * @returns {null|string}
	 * Returns the value of the `type` option of the associated logical
	 * interface or `null` if no `type` option is set.
	 */
	getType: function() {
		return this._get('type');
	},

	/**
	 * Get the name of the associated logical interface.
	 *
	 * @returns {string}
	 * Returns the logical interface name, such as `lan` or `wan`.
	 */
	getName: function() {
		return this.sid;
	},

	/**
	 * Get the uptime of the logical interface.
	 *
	 * @returns {number}
	 * Returns the uptime of the associated interface in seconds.
	 */
	getUptime: function() {
		return this._ubus('uptime') || 0;
	},

	/**
	 * Get the logical interface expiry time in seconds.
	 *
	 * For protocols that have a concept of a lease, such as DHCP or
	 * DHCPv6, this function returns the remaining time in seconds
	 * until the lease expires.
	 *
	 * @returns {number}
	 * Returns the amount of seconds until the lease expires or `-1`
	 * if it isn't applicable to the associated protocol.
	 */
	getExpiry: function() {
		var u = this._ubus('uptime'),
		    d = this._ubus('data');

		if (typeof(u) == 'number' && d != null &&
		    typeof(d) == 'object' && typeof(d.leasetime) == 'number') {
			var r = d.leasetime - (u % d.leasetime);
			return (r > 0 ? r : 0);
		}

		return -1;
	},

	/**
	 * Get the metric value of the logical interface.
	 *
	 * @returns {number}
	 * Returns the current metric value used for device and network
	 * routes spawned by the associated logical interface.
	 */
	getMetric: function() {
		return this._ubus('metric') || 0;
	},

	/**
	 * Get the requested firewall zone name of the logical interface.
	 *
	 * Some protocol implementations request a specific firewall zone
	 * to trigger inclusion of their resulting network devices into the
	 * firewall rule set.
	 *
	 * @returns {null|string}
	 * Returns the requested firewall zone name as published in the
	 * `ubus` runtime information or `null` if the remote protocol
	 * handler didn't request a zone.
	 */
	getZoneName: function() {
		var d = this._ubus('data');

		if (L.isObject(d) && typeof(d.zone) == 'string')
			return d.zone;

		return null;
	},

	/**
	 * Query the first (primary) IPv4 address of the logical interface.
	 *
	 * @returns {null|string}
	 * Returns the primary IPv4 address registered by the protocol handler
	 * or `null` if no IPv4 addresses were set.
	 */
	getIPAddr: function() {
		var addrs = this._ubus('ipv4-address');
		return ((Array.isArray(addrs) && addrs.length) ? addrs[0].address : null);
	},

	/**
	 * Query all IPv4 addresses of the logical interface.
	 *
	 * @returns {string[]}
	 * Returns an array of IPv4 addresses in CIDR notation which have been
	 * registered by the protocol handler. The order of the resulting array
	 * follows the order of the addresses in `ubus` runtime information.
	 */
	getIPAddrs: function() {
		var addrs = this._ubus('ipv4-address'),
		    rv = [];

		if (Array.isArray(addrs))
			for (var i = 0; i < addrs.length; i++)
				rv.push('%s/%d'.format(addrs[i].address, addrs[i].mask));

		return rv;
	},

	/**
	 * Query the first (primary) IPv4 netmask of the logical interface.
	 *
	 * @returns {null|string}
	 * Returns the netmask of the primary IPv4 address registered by the
	 * protocol handler or `null` if no IPv4 addresses were set.
	 */
	getNetmask: function() {
		var addrs = this._ubus('ipv4-address');
		if (Array.isArray(addrs) && addrs.length)
			return prefixToMask(addrs[0].mask, false);
	},

	/**
	 * Query the gateway (nexthop) of the default route associated with
	 * this logical interface.
	 *
	 * @returns {string}
	 * Returns a string containing the IPv4 nexthop address of the associated
	 * default route or `null` if no default route was found.
	 */
	getGatewayAddr: function() {
		var routes = this._ubus('route');

		if (Array.isArray(routes))
			for (var i = 0; i < routes.length; i++)
				if (typeof(routes[i]) == 'object' &&
				    routes[i].target == '0.0.0.0' &&
				    routes[i].mask == 0)
				    return routes[i].nexthop;

		return null;
	},

	/**
	 * Query the IPv4 DNS servers associated with the logical interface.
	 *
	 * @returns {string[]}
	 * Returns an array of IPv4 DNS servers registered by the remote
	 * protocol backend.
	 */
	getDNSAddrs: function() {
		var addrs = this._ubus('dns-server'),
		    rv = [];

		if (Array.isArray(addrs))
			for (var i = 0; i < addrs.length; i++)
				if (!/:/.test(addrs[i]))
					rv.push(addrs[i]);

		return rv;
	},

	/**
	 * Query the first (primary) IPv6 address of the logical interface.
	 *
	 * @returns {null|string}
	 * Returns the primary IPv6 address registered by the protocol handler
	 * in CIDR notation or `null` if no IPv6 addresses were set.
	 */
	getIP6Addr: function() {
		var addrs = this._ubus('ipv6-address');

		if (Array.isArray(addrs) && L.isObject(addrs[0]))
			return '%s/%d'.format(addrs[0].address, addrs[0].mask);

		addrs = this._ubus('ipv6-prefix-assignment');

		if (Array.isArray(addrs) && L.isObject(addrs[0]) && L.isObject(addrs[0]['local-address']))
			return '%s/%d'.format(addrs[0]['local-address'].address, addrs[0]['local-address'].mask);

		return null;
	},

	/**
	 * Query all IPv6 addresses of the logical interface.
	 *
	 * @returns {string[]}
	 * Returns an array of IPv6 addresses in CIDR notation which have been
	 * registered by the protocol handler. The order of the resulting array
	 * follows the order of the addresses in `ubus` runtime information.
	 */
	getIP6Addrs: function() {
		var addrs = this._ubus('ipv6-address'),
		    rv = [];

		if (Array.isArray(addrs))
			for (var i = 0; i < addrs.length; i++)
				if (L.isObject(addrs[i]))
					rv.push('%s/%d'.format(addrs[i].address, addrs[i].mask));

		addrs = this._ubus('ipv6-prefix-assignment');

		if (Array.isArray(addrs))
			for (var i = 0; i < addrs.length; i++)
				if (L.isObject(addrs[i]) && L.isObject(addrs[i]['local-address']))
					rv.push('%s/%d'.format(addrs[i]['local-address'].address, addrs[i]['local-address'].mask));

		return rv;
	},

	/**
	 * Query the gateway (nexthop) of the IPv6 default route associated with
	 * this logical interface.
	 *
	 * @returns {string}
	 * Returns a string containing the IPv6 nexthop address of the associated
	 * default route or `null` if no default route was found.
	 */
	getGateway6Addr: function() {
		var routes = this._ubus('route');

		if (Array.isArray(routes))
			for (var i = 0; i < routes.length; i++)
				if (typeof(routes[i]) == 'object' &&
				    routes[i].target == '::' &&
				    routes[i].mask == 0)
				    return routes[i].nexthop;

		return null;
	},

	/**
	 * Query the IPv6 DNS servers associated with the logical interface.
	 *
	 * @returns {string[]}
	 * Returns an array of IPv6 DNS servers registered by the remote
	 * protocol backend.
	 */
	getDNS6Addrs: function() {
		var addrs = this._ubus('dns-server'),
		    rv = [];

		if (Array.isArray(addrs))
			for (var i = 0; i < addrs.length; i++)
				if (/:/.test(addrs[i]))
					rv.push(addrs[i]);

		return rv;
	},

	/**
	 * Query the routed IPv6 prefix associated with the logical interface.
	 *
	 * @returns {null|string}
	 * Returns the routed IPv6 prefix registered by the remote protocol
	 * handler or `null` if no prefix is present.
	 */
	getIP6Prefix: function() {
		var prefixes = this._ubus('ipv6-prefix');

		if (Array.isArray(prefixes) && L.isObject(prefixes[0]))
			return '%s/%d'.format(prefixes[0].address, prefixes[0].mask);

		return null;
	},

	/**
	 * Query interface error messages published in `ubus` runtime state.
	 *
	 * Interface errors are emitted by remote protocol handlers if the setup
	 * of the underlying logical interface failed, e.g. due to bad
	 * configuration or network connectivity issues.
	 *
	 * This function will translate the found error codes to human readable
	 * messages using the descriptions registered by
	 * {@link LuCI.Network#registerErrorCode Network.registerErrorCode()}
	 * and fall back to `"Unknown error (%s)"` where `%s` is replaced by the
	 * error code in case no translation can be found.
	 *
	 * @returns {string[]}
	 * Returns an array of translated interface error messages.
	 */
	getErrors: function() {
		var errors = this._ubus('errors'),
		    rv = null;

		if (Array.isArray(errors)) {
			for (var i = 0; i < errors.length; i++) {
				if (!L.isObject(errors[i]) || typeof(errors[i].code) != 'string')
					continue;

				rv = rv || [];
				rv.push(proto_errors[errors[i].code] || _('Unknown error (%s)').format(errors[i].code));
			}
		}

		return rv;
	},

	/**
	 * Checks whether the underlying logical interface is declared as bridge.
	 *
	 * @returns {boolean}
	 * Returns `true` when the interface is declared with `option type bridge`
	 * and when the associated protocol implementation is not marked virtual
	 * or `false` when the logical interface is no bridge.
	 */
	isBridge: function() {
		return (!this.isVirtual() && this.getType() == 'bridge');
	},

	/**
	 * Get the name of the opkg package providing the protocol functionality.
	 *
	 * This function should be overwritten by protocol specific subclasses.
	 *
	 * @abstract
	 *
	 * @returns {string}
	 * Returns the name of the opkg package required for the protocol to
	 * function, e.g. `odhcp6c` for the `dhcpv6` prototocol.
	 */
	getOpkgPackage: function() {
		return null;
	},

	/**
	 * Checks whether the protocol functionality is installed.
	 *
	 * This function exists for compatibility with old code, it always
	 * returns `true`.
	 *
	 * @deprecated
	 * @abstract
	 *
	 * @returns {boolean}
	 * Returns `true` if the protocol support is installed, else `false`.
	 */
	isInstalled: function() {
		return true;
	},

	/**
	 * Checks whether this protocol is "virtual".
	 *
	 * A "virtual" protocol is a protocol which spawns its own interfaces
	 * on demand instead of using existing physical interfaces.
	 *
	 * Examples for virtual protocols are `6in4` which `gre` spawn tunnel
	 * network device on startup, examples for non-virtual protcols are
	 * `dhcp` or `static` which apply IP configuration to existing interfaces.
	 *
	 * This function should be overwritten by subclasses.
	 *
	 * @returns {boolean}
	 * Returns a boolean indicating whether the underlying protocol spawns
	 * dynamic interfaces (`true`) or not (`false`).
	 */
	isVirtual: function() {
		return false;
	},

	/**
	 * Checks whether this protocol is "floating".
	 *
	 * A "floating" protocol is a protocol which spawns its own interfaces
	 * on demand, like a virtual one but which relies on an existinf lower
	 * level interface to initiate the connection.
	 *
	 * An example for such a protocol is "pppoe".
	 *
	 * This function exists for backwards compatibility with older code
	 * but should not be used anymore.
	 *
	 * @deprecated
	 * @returns {boolean}
	 * Returns a boolean indicating whether this protocol is floating (`true`)
	 * or not (`false`).
	 */
	isFloating: function() {
		return false;
	},

	/**
	 * Checks whether this logical interface is dynamic.
	 *
	 * A dynamic interface is an interface which has been created at runtime,
	 * e.g. as sub-interface of another interface, but which is not backed by
	 * any user configuration. Such dynamic interfaces cannot be edited but
	 * only brought down or restarted.
	 *
	 * @returns {boolean}
	 * Returns a boolean indicating whether this interface is dynamic (`true`)
	 * or not (`false`).
	 */
	isDynamic: function() {
		return (this._ubus('dynamic') == true);
	},

	/**
	 * Checks whether this interface is an alias interface.
	 *
	 * Alias interfaces are interfaces layering on top of another interface
	 * and are denoted by a special `@interfacename` notation in the
	 * underlying `ifname` option.
	 *
	 * @returns {null|string}
	 * Returns the name of the parent interface if this logical interface
	 * is an alias or `null` if it is not an alias interface.
	 */
	isAlias: function() {
		var ifnames = L.toArray(uci.get('network', this.sid, 'ifname')),
		    parent = null;

		for (var i = 0; i < ifnames.length; i++)
			if (ifnames[i].charAt(0) == '@')
				parent = ifnames[i].substr(1);
			else if (parent != null)
				parent = null;

		return parent;
	},

	/**
	 * Checks whether this logical interface is "empty", meaning that ut
	 * has no network devices attached.
	 *
	 * @returns {boolean}
	 * Returns `true` if this logical interface is empty, else `false`.
	 */
	isEmpty: function() {
		if (this.isFloating())
			return false;

		var empty = true,
		    ifname = this._get('ifname');

		if (ifname != null && ifname.match(/\S+/))
			empty = false;

		if (empty == true && getWifiNetidBySid(this.sid) != null)
			empty = false;

		return empty;
	},

	/**
	 * Checks whether this logical interface is configured and running.
	 *
	 * @returns {boolean}
	 * Returns `true` when the interface is active or `false` when it is not.
	 */
	isUp: function() {
		return (this._ubus('up') == true);
	},

	/**
	 * Add the given network device to the logical interface.
	 *
	 * @param {LuCI.Network.Protocol|LuCI.Network.Device|LuCI.Network.WifiDevice|LuCI.Network.WifiNetwork|string} device
	 * The object or device name to add to the logical interface. In case the
	 * given argument is not a string, it is resolved though the
	 * {@link LuCI.Network#getIfnameOf Network.getIfnameOf()} function.
	 *
	 * @returns {boolean}
	 * Returns `true` if the device name has been added or `false` if any
	 * argument was invalid, if the device was already part of the logical
	 * interface or if the logical interface is virtual.
	 */
	addDevice: function(ifname) {
		ifname = ifnameOf(ifname);

		if (ifname == null || this.isFloating())
			return false;

		var wif = getWifiSidByIfname(ifname);

		if (wif != null)
			return appendValue('wireless', wif, 'network', this.sid);

		return appendValue('network', this.sid, 'ifname', ifname);
	},

	/**
	 * Remove the given network device from the logical interface.
	 *
	 * @param {LuCI.Network.Protocol|LuCI.Network.Device|LuCI.Network.WifiDevice|LuCI.Network.WifiNetwork|string} device
	 * The object or device name to remove from the logical interface. In case
	 * the given argument is not a string, it is resolved though the
	 * {@link LuCI.Network#getIfnameOf Network.getIfnameOf()} function.
	 *
	 * @returns {boolean}
	 * Returns `true` if the device name has been added or `false` if any
	 * argument was invalid, if the device was already part of the logical
	 * interface or if the logical interface is virtual.
	 */
	deleteDevice: function(ifname) {
		var rv = false;

		ifname = ifnameOf(ifname);

		if (ifname == null || this.isFloating())
			return false;

		var wif = getWifiSidByIfname(ifname);

		if (wif != null)
			rv = removeValue('wireless', wif, 'network', this.sid);

		if (removeValue('network', this.sid, 'ifname', ifname))
			rv = true;

		return rv;
	},

	/**
	 * Returns the Linux network device associated with this logical
	 * interface.
	 *
	 * @returns {LuCI.Network.Device}
	 * Returns a `Network.Device` class instance representing the
	 * expected Linux network device according to the configuration.
	 */
	getDevice: function() {
		if (this.isVirtual()) {
			var ifname = '%s-%s'.format(this.getProtocol(), this.sid);
			_state.isTunnel[this.getProtocol() + '-' + this.sid] = true;
			return L.network.instantiateDevice(ifname, this);
		}
		else if (this.isBridge()) {
			var ifname = 'br-%s'.format(this.sid);
			_state.isBridge[ifname] = true;
			return new Device(ifname, this);
		}
		else {
			var ifnames = L.toArray(uci.get('network', this.sid, 'ifname'));

			for (var i = 0; i < ifnames.length; i++) {
				var m = ifnames[i].match(/^([^:/]+)/);
				return ((m && m[1]) ? L.network.instantiateDevice(m[1], this) : null);
			}

			ifname = getWifiNetidByNetname(this.sid);

			return (ifname != null ? L.network.instantiateDevice(ifname[0], this) : null);
		}
	},

	/**
	 * Returns the layer 2 linux network device currently associated
	 * with this logical interface.
	 *
	 * @returns {LuCI.Network.Device}
	 * Returns a `Network.Device` class instance representing the Linux
	 * network device currently associated with the logical interface.
	 */
	getL2Device: function() {
		var ifname = this._ubus('device');
		return (ifname != null ? L.network.instantiateDevice(ifname, this) : null);
	},

	/**
	 * Returns the layer 3 linux network device currently associated
	 * with this logical interface.
	 *
	 * @returns {LuCI.Network.Device}
	 * Returns a `Network.Device` class instance representing the Linux
	 * network device currently associated with the logical interface.
	 */
	getL3Device: function() {
		var ifname = this._ubus('l3_device');
		return (ifname != null ? L.network.instantiateDevice(ifname, this) : null);
	},

	/**
	 * Returns a list of network sub-devices associated with this logical
	 * interface.
	 *
	 * @returns {null|Array<LuCI.Network.Device>}
	 * Returns an array of of `Network.Device` class instances representing
	 * the sub-devices attached to this logical interface or `null` if the
	 * logical interface does not support sub-devices, e.g. because it is
	 * virtual and not a bridge.
	 */
	getDevices: function() {
		var rv = [];

		if (!this.isBridge() && !(this.isVirtual() && !this.isFloating()))
			return null;

		var ifnames = L.toArray(uci.get('network', this.sid, 'ifname'));

		for (var i = 0; i < ifnames.length; i++) {
			if (ifnames[i].charAt(0) == '@')
				continue;

			var m = ifnames[i].match(/^([^:/]+)/);
			if (m != null)
				rv.push(L.network.instantiateDevice(m[1], this));
		}

		var uciWifiIfaces = uci.sections('wireless', 'wifi-iface');

		for (var i = 0; i < uciWifiIfaces.length; i++) {
			if (typeof(uciWifiIfaces[i].device) != 'string')
				continue;

			var networks = L.toArray(uciWifiIfaces[i].network);

			for (var j = 0; j < networks.length; j++) {
				if (networks[j] != this.sid)
					continue;

				var netid = getWifiNetidBySid(uciWifiIfaces[i]['.name']);

				if (netid != null)
					rv.push(L.network.instantiateDevice(netid[0], this));
			}
		}

		rv.sort(deviceSort);

		return rv;
	},

	/**
	 * Checks whether this logical interface contains the given device
	 * object.
	 *
	 * @param {LuCI.Network.Protocol|LuCI.Network.Device|LuCI.Network.WifiDevice|LuCI.Network.WifiNetwork|string} device
	 * The object or device name to check. In case the given argument is not
	 * a string, it is resolved though the
	 * {@link LuCI.Network#getIfnameOf Network.getIfnameOf()} function.
	 *
	 * @returns {boolean}
	 * Returns `true` when this logical interface contains the given network
	 * device or `false` if not.
	 */
	containsDevice: function(ifname) {
		ifname = ifnameOf(ifname);

		if (ifname == null)
			return false;
		else if (this.isVirtual() && '%s-%s'.format(this.getProtocol(), this.sid) == ifname)
			return true;
		else if (this.isBridge() && 'br-%s'.format(this.sid) == ifname)
			return true;

		var ifnames = L.toArray(uci.get('network', this.sid, 'ifname'));

		for (var i = 0; i < ifnames.length; i++) {
			var m = ifnames[i].match(/^([^:/]+)/);
			if (m != null && m[1] == ifname)
				return true;
		}

		var wif = getWifiSidByIfname(ifname);

		if (wif != null) {
			var networks = L.toArray(uci.get('wireless', wif, 'network'));

			for (var i = 0; i < networks.length; i++)
				if (networks[i] == this.sid)
					return true;
		}

		return false;
	}
});

/**
 * @class
 * @memberof LuCI.Network
 * @hideconstructor
 * @classdesc
 *
 * A `Network.Device` class instance represents an underlying Linux network
 * device and allows querying device details such as packet statistics or MTU.
 */
Device = L.Class.extend(/** @lends LuCI.Network.Device.prototype */ {
	__init__: function(ifname, network) {
		var wif = getWifiSidByIfname(ifname);

		if (wif != null) {
			var res = getWifiStateBySid(wif) || [],
			    netid = getWifiNetidBySid(wif) || [];

			this.wif    = new WifiNetwork(wif, res[0], res[1], netid[0], res[2], { ifname: ifname });
			this.ifname = this.wif.getIfname();
		}

		this.ifname  = this.ifname || ifname;
		this.dev     = _state.netdevs[this.ifname];
		this.network = network;
	},

	_devstate: function(/* ... */) {
		var rv = this.dev;

		for (var i = 0; i < arguments.length; i++)
			if (L.isObject(rv))
				rv = rv[arguments[i]];
			else
				return null;

		return rv;
	},

	/**
	 * Get the name of the network device.
	 *
	 * @returns {string}
	 * Returns the name of the device, e.g. `eth0` or `wlan0`.
	 */
	getName: function() {
		return (this.wif != null ? this.wif.getIfname() : this.ifname);
	},

	/**
	 * Get the MAC address of the device.
	 *
	 * @returns {null|string}
	 * Returns the MAC address of the device or `null` if not applicable,
	 * e.g. for non-ethernet tunnel devices.
	 */
	getMAC: function() {
		var mac = this._devstate('macaddr');
		return mac ? mac.toUpperCase() : null;
	},

	/**
	 * Get the MTU of the device.
	 *
	 * @returns {number}
	 * Returns the MTU of the device.
	 */
	getMTU: function() {
		return this._devstate('mtu');
	},

	/**
	 * Get the IPv4 addresses configured on the device.
	 *
	 * @returns {string[]}
	 * Returns an array of IPv4 address strings.
	 */
	getIPAddrs: function() {
		var addrs = this._devstate('ipaddrs');
		return (Array.isArray(addrs) ? addrs : []);
	},

	/**
	 * Get the IPv6 addresses configured on the device.
	 *
	 * @returns {string[]}
	 * Returns an array of IPv6 address strings.
	 */
	getIP6Addrs: function() {
		var addrs = this._devstate('ip6addrs');
		return (Array.isArray(addrs) ? addrs : []);
	},

	/**
	 * Get the type of the device..
	 *
	 * @returns {string}
	 * Returns a string describing the type of the network device:
	 *  - `alias` if it is an abstract alias device (`@` notation)
	 *  - `wifi` if it is a wireless interface (e.g. `wlan0`)
	 *  - `bridge` if it is a bridge device (e.g. `br-lan`)
	 *  - `tunnel` if it is a tun or tap device (e.g. `tun0`)
	 *  - `vlan` if it is a vlan device (e.g. `eth0.1`)
	 *  - `switch` if it is a switch device (e.g.`eth1` connected to switch0)
	 *  - `ethernet` for all other device types
	 */
	getType: function() {
		if (this.ifname != null && this.ifname.charAt(0) == '@')
			return 'alias';
		else if (this.wif != null || isWifiIfname(this.ifname))
			return 'wifi';
		else if (_state.isBridge[this.ifname])
			return 'bridge';
		else if (_state.isTunnel[this.ifname])
			return 'tunnel';
		else if (this.ifname.indexOf('.') > -1)
			return 'vlan';
		else if (_state.isSwitch[this.ifname])
			return 'switch';
		else
			return 'ethernet';
	},

	/**
	 * Get a short description string for the device.
	 *
	 * @returns {string}
	 * Returns the device name for non-wifi devices or a string containing
	 * the operation mode and SSID for wifi devices.
	 */
	getShortName: function() {
		if (this.wif != null)
			return this.wif.getShortName();

		return this.ifname;
	},

	/**
	 * Get a long description string for the device.
	 *
	 * @returns {string}
	 * Returns a string containing the type description and device name
	 * for non-wifi devices or operation mode and ssid for wifi ones.
	 */
	getI18n: function() {
		if (this.wif != null) {
			return '%s: %s "%s"'.format(
				_('Wireless Network'),
				this.wif.getActiveMode(),
				this.wif.getActiveSSID() || this.wif.getActiveBSSID() || this.wif.getID() || '?');
		}

		return '%s: "%s"'.format(this.getTypeI18n(), this.getName());
	},

	/**
	 * Get a string describing the device type.
	 *
	 * @returns {string}
	 * Returns a string describing the type, e.g. "Wireless Adapter" or
	 * "Bridge".
	 */
	getTypeI18n: function() {
		switch (this.getType()) {
		case 'alias':
			return _('Alias Interface');

		case 'wifi':
			return _('Wireless Adapter');

		case 'bridge':
			return _('Bridge');

		case 'switch':
			return _('Ethernet Switch');

		case 'vlan':
			return (_state.isSwitch[this.ifname] ? _('Switch VLAN') : _('Software VLAN'));

		case 'tunnel':
			return _('Tunnel Interface');

		default:
			return _('Ethernet Adapter');
		}
	},

	/**
	 * Get the associated bridge ports of the device.
	 *
	 * @returns {null|Array<LuCI.Network.Device>}
	 * Returns an array of `Network.Device` instances representing the ports
	 * (slave interfaces) of the bridge or `null` when this device isn't
	 * a Linux bridge.
	 */
	getPorts: function() {
		var br = _state.bridges[this.ifname],
		    rv = [];

		if (br == null || !Array.isArray(br.ifnames))
			return null;

		for (var i = 0; i < br.ifnames.length; i++)
			rv.push(L.network.instantiateDevice(br.ifnames[i].name));

		rv.sort(deviceSort);

		return rv;
	},

	/**
	 * Get the bridge ID
	 *
	 * @returns {null|string}
	 * Returns the ID of this network bridge or `null` if this network
	 * device is not a Linux bridge.
	 */
	getBridgeID: function() {
		var br = _state.bridges[this.ifname];
		return (br != null ? br.id : null);
	},

	/**
	 * Get the bridge STP setting
	 *
	 * @returns {boolean}
	 * Returns `true` when this device is a Linux bridge and has `stp`
	 * enabled, else `false`.
	 */
	getBridgeSTP: function() {
		var br = _state.bridges[this.ifname];
		return (br != null ? !!br.stp : false);
	},

	/**
	 * Checks whether this device is up.
	 *
	 * @returns {boolean}
	 * Returns `true` when the associated device is running pr `false`
	 * when it is down or absent.
	 */
	isUp: function() {
		var up = this._devstate('flags', 'up');

		if (up == null)
			up = (this.getType() == 'alias');

		return up;
	},

	/**
	 * Checks whether this device is a Linux bridge.
	 *
	 * @returns {boolean}
	 * Returns `true` when the network device is present and a Linux bridge,
	 * else `false`.
	 */
	isBridge: function() {
		return (this.getType() == 'bridge');
	},

	/**
	 * Checks whether this device is part of a Linux bridge.
	 *
	 * @returns {boolean}
	 * Returns `true` when this network device is part of a bridge,
	 * else `false`.
	 */
	isBridgePort: function() {
		return (this._devstate('bridge') != null);
	},

	/**
	 * Get the amount of transmitted bytes.
	 *
	 * @returns {number}
	 * Returns the amount of bytes transmitted by the network device.
	 */
	getTXBytes: function() {
		var stat = this._devstate('stats');
		return (stat != null ? stat.tx_bytes || 0 : 0);
	},

	/**
	 * Get the amount of received bytes.
	 *
	 * @returns {number}
	 * Returns the amount of bytes received by the network device.
	 */
	getRXBytes: function() {
		var stat = this._devstate('stats');
		return (stat != null ? stat.rx_bytes || 0 : 0);
	},

	/**
	 * Get the amount of transmitted packets.
	 *
	 * @returns {number}
	 * Returns the amount of packets transmitted by the network device.
	 */
	getTXPackets: function() {
		var stat = this._devstate('stats');
		return (stat != null ? stat.tx_packets || 0 : 0);
	},

	/**
	 * Get the amount of received packets.
	 *
	 * @returns {number}
	 * Returns the amount of packets received by the network device.
	 */
	getRXPackets: function() {
		var stat = this._devstate('stats');
		return (stat != null ? stat.rx_packets || 0 : 0);
	},

	/**
	 * Get the primary logical interface this device is assigned to.
	 *
	 * @returns {null|LuCI.Network.Protocol}
	 * Returns a `Network.Protocol` instance representing the logical
	 * interface this device is attached to or `null` if it is not
	 * assigned to any logical interface.
	 */
	getNetwork: function() {
		return this.getNetworks()[0];
	},

	/**
	 * Get the logical interfaces this device is assigned to.
	 *
	 * @returns {Array<LuCI.Network.Protocol>}
	 * Returns an array of `Network.Protocol` instances representing the
	 * logical interfaces this device is assigned to.
	 */
	getNetworks: function() {
		if (this.networks == null) {
			this.networks = [];

			var networks = enumerateNetworks.apply(L.network);

			for (var i = 0; i < networks.length; i++)
				if (networks[i].containsDevice(this.ifname) || networks[i].getIfname() == this.ifname)
					this.networks.push(networks[i]);

			this.networks.sort(networkSort);
		}

		return this.networks;
	},

	/**
	 * Get the related wireless network this device is related to.
	 *
	 * @returns {null|LuCI.Network.WifiNetwork}
	 * Returns a `Network.WifiNetwork` instance representing the wireless
	 * network corresponding to this network device or `null` if this device
	 * is not a wireless device.
	 */
	getWifiNetwork: function() {
		return (this.wif != null ? this.wif : null);
	}
});

/**
 * @class
 * @memberof LuCI.Network
 * @hideconstructor
 * @classdesc
 *
 * A `Network.WifiDevice` class instance represents a wireless radio device
 * present on the system and provides wireless capability information as
 * well as methods for enumerating related wireless networks.
 */
WifiDevice = L.Class.extend(/** @lends LuCI.Network.WifiDevice.prototype */ {
	__init__: function(name, radiostate) {
		var uciWifiDevice = uci.get('wireless', name);

		if (uciWifiDevice != null &&
		    uciWifiDevice['.type'] == 'wifi-device' &&
		    uciWifiDevice['.name'] != null) {
			this.sid    = uciWifiDevice['.name'];
		}

		this.sid    = this.sid || name;
		this._ubusdata = {
			radio: name,
			dev:   radiostate
		};
	},

	/* private */
	ubus: function(/* ... */) {
		var v = this._ubusdata;

		for (var i = 0; i < arguments.length; i++)
			if (L.isObject(v))
				v = v[arguments[i]];
			else
				return null;

		return v;
	},

	/**
	 * Read the given UCI option value of this wireless device.
	 *
	 * @param {string} opt
	 * The UCI option name to read.
	 *
	 * @returns {null|string|string[]}
	 * Returns the UCI option value or `null` if the requested option is
	 * not found.
	 */
	get: function(opt) {
		return uci.get('wireless', this.sid, opt);
	},

	/**
	 * Set the given UCI option of this network to the given value.
	 *
	 * @param {string} opt
	 * The name of the UCI option to set.
	 *
	 * @param {null|string|string[]} val
	 * The value to set or `null` to remove the given option from the
	 * configuration.
	 */
	set: function(opt, value) {
		return uci.set('wireless', this.sid, opt, value);
	},

	/**
	 * Checks whether this wireless radio is disabled.
	 *
	 * @returns {boolean}
	 * Returns `true` when the wireless radio is marked as disabled in `ubus`
	 * runtime state or when the `disabled` option is set in the corresponding
	 * UCI configuration.
	 */
	isDisabled: function() {
		return this.ubus('dev', 'disabled') || this.get('disabled') == '1';
	},

	/**
	 * Get the configuration name of this wireless radio.
	 *
	 * @returns {string}
	 * Returns the UCI section name (e.g. `radio0`) of the corresponding
	 * radio configuration which also serves as unique logical identifier
	 * for the wireless phy.
	 */
	getName: function() {
		return this.sid;
	},

	/**
	 * Gets a list of supported hwmodes.
	 *
	 * The hwmode values describe the frequency band and wireless standard
	 * versions supported by the wireless phy.
	 *
	 * @returns {string[]}
	 * Returns an array of valid hwmode values for this radio. Currently
	 * known mode values are:
	 *  - `a` - Legacy 802.11a mode, 5 GHz, up to 54 Mbit/s
	 *  - `b` - Legacy 802.11b mode, 2.4 GHz, up to 11 Mbit/s
	 *  - `g` - Legacy 802.11g mode, 2.4 GHz, up to 54 Mbit/s
	 *  - `n` - IEEE 802.11n mode, 2.4 or 5 GHz, up to 600 Mbit/s
	 *  - `ac` - IEEE 802.11ac mode, 5 GHz, up to 6770 Mbit/s
	 */
	getHWModes: function() {
		var hwmodes = this.ubus('dev', 'iwinfo', 'hwmodes');
		return Array.isArray(hwmodes) ? hwmodes : [ 'b', 'g' ];
	},

	/**
	 * Gets a list of supported htmodes.
	 *
	 * The htmode values describe the wide-frequency options supported by
	 * the wireless phy.
	 *
	 * @returns {string[]}
	 * Returns an array of valid htmode values for this radio. Currently
	 * known mode values are:
	 *  - `HT20` - applicable to IEEE 802.11n, 20 MHz wide channels
	 *  - `HT40` - applicable to IEEE 802.11n, 40 MHz wide channels
	 *  - `VHT20` - applicable to IEEE 802.11ac, 20 MHz wide channels
	 *  - `VHT40` - applicable to IEEE 802.11ac, 40 MHz wide channels
	 *  - `VHT80` - applicable to IEEE 802.11ac, 80 MHz wide channels
	 *  - `VHT160` - applicable to IEEE 802.11ac, 160 MHz wide channels
	 */
	getHTModes: function() {
		var htmodes = this.ubus('dev', 'iwinfo', 'htmodes');
		return (Array.isArray(htmodes) && htmodes.length) ? htmodes : null;
	},

	/**
	 * Get a string describing the wireless radio hardware.
	 *
	 * @returns {string}
	 * Returns the description string.
	 */
	getI18n: function() {
		var hw = this.ubus('dev', 'iwinfo', 'hardware'),
		    type = L.isObject(hw) ? hw.name : null;

		if (this.ubus('dev', 'iwinfo', 'type') == 'wl')
			type = 'Broadcom';

		var hwmodes = this.getHWModes(),
		    modestr = '';

		hwmodes.sort(function(a, b) {
			return (a.length != b.length ? a.length > b.length : a > b);
		});

		modestr = hwmodes.join('');

		return '%s 802.11%s Wireless Controller (%s)'.format(type || 'Generic', modestr, this.getName());
	},

	/**
	 * A wireless scan result object describes a neighbouring wireless
	 * network found in the vincinity.
	 *
	 * @typedef {Object<string, number|string|LuCI.Network.WifiEncryption>} WifiScanResult
	 * @memberof LuCI.Network
	 *
	 * @property {string} ssid
	 * The SSID / Mesh ID of the network.
	 *
	 * @property {string} bssid
	 * The BSSID if the network.
	 *
	 * @property {string} mode
	 * The operation mode of the network (`Master`, `Ad-Hoc`, `Mesh Point`).
	 *
	 * @property {number} channel
	 * The wireless channel of the network.
	 *
	 * @property {number} signal
	 * The received signal strength of the network in dBm.
	 *
	 * @property {number} quality
	 * The numeric quality level of the signal, can be used in conjunction
	 * with `quality_max` to calculate a quality percentage.
	 *
	 * @property {number} quality_max
	 * The maximum possible quality level of the signal, can be used in
	 * conjunction with `quality` to calculate a quality percentage.
	 *
	 * @property {LuCI.Network.WifiEncryption} encryption
	 * The encryption used by the wireless network.
	 */

	/**
	 * Trigger a wireless scan on this radio device and obtain a list of
	 * nearby networks.
	 *
	 * @returns {Promise<Array<LuCI.Network.WifiScanResult>>}
	 * Returns a promise resolving to an array of scan result objects
	 * describing the networks found in the vincinity.
	 */
	getScanList: function() {
		return callIwinfoScan(this.sid);
	},

	/**
	 * Check whether the wireless radio is marked as up in the `ubus`
	 * runtime state.
	 *
	 * @returns {boolean}
	 * Returns `true` when the radio device is up, else `false`.
	 */
	isUp: function() {
		if (L.isObject(_state.radios[this.sid]))
			return (_state.radios[this.sid].up == true);

		return false;
	},

	/**
	 * Get the wifi network of the given name belonging to this radio device
	 *
	 * @param {string} network
	 * The name of the wireless network to lookup. This may be either an uci
	 * configuration section ID, a network ID in the form `radio#.network#`
	 * or a Linux network device name like `wlan0` which is resolved to the
	 * corresponding configuration section through `ubus` runtime information.
	 *
	 * @returns {Promise<LuCI.Network.WifiNetwork>}
	 * Returns a promise resolving to a `Network.WifiNetwork` instance
	 * representing the wireless network and rejecting with `null` if
	 * the given network could not be found or is not associated with
	 * this radio device.
	 */
	getWifiNetwork: function(network) {
		return L.network.getWifiNetwork(network).then(L.bind(function(networkInstance) {
			var uciWifiIface = (networkInstance.sid ? uci.get('wireless', networkInstance.sid) : null);

			if (uciWifiIface == null || uciWifiIface['.type'] != 'wifi-iface' || uciWifiIface.device != this.sid)
				return Promise.reject();

			return networkInstance;
		}, this));
	},

	/**
	 * Get all wireless networks associated with this wireless radio device.
	 *
	 * @returns {Promise<Array<LuCI.Network.WifiNetwork>>}
	 * Returns a promise resolving to an array of `Network.WifiNetwork`
	 * instances respresenting the wireless networks associated with this
	 * radio device.
	 */
	getWifiNetworks: function() {
		return L.network.getWifiNetworks().then(L.bind(function(networks) {
			var rv = [];

			for (var i = 0; i < networks.length; i++)
				if (networks[i].getWifiDeviceName() == this.getName())
					rv.push(networks[i]);

			return rv;
		}, this));
	},

	/**
	 * Adds a new wireless network associated with this radio device to the
	 * configuration and sets its options to the provided values.
	 *
	 * @param {Object<string, string|string[]>} [options]
	 * The options to set for the newly added wireless network.
	 *
	 * @returns {Promise<null|LuCI.Network.WifiNetwork>}
	 * Returns a promise resolving to a `WifiNetwork` instance describing
	 * the newly added wireless network or `null` if the given options
	 * were invalid.
	 */
	addWifiNetwork: function(options) {
		if (!L.isObject(options))
			options = {};

		options.device = this.sid;

		return L.network.addWifiNetwork(options);
	},

	/**
	 * Deletes the wireless network with the given name associated with this
	 * radio device.
	 *
	 * @param {string} network
	 * The name of the wireless network to lookup. This may be either an uci
	 * configuration section ID, a network ID in the form `radio#.network#`
	 * or a Linux network device name like `wlan0` which is resolved to the
	 * corresponding configuration section through `ubus` runtime information.
	 *
	 * @returns {Promise<boolean>}
	 * Returns a promise resolving to `true` when the wireless network was
	 * successfully deleted from the configuration or `false` when the given
	 * network could not be found or if the found network was not associated
	 * with this wireless radio device.
	 */
	deleteWifiNetwork: function(network) {
		var sid = null;

		if (network instanceof WifiNetwork) {
			sid = network.sid;
		}
		else {
			var uciWifiIface = uci.get('wireless', network);

			if (uciWifiIface == null || uciWifiIface['.type'] != 'wifi-iface')
				sid = getWifiSidByIfname(network);
		}

		if (sid == null || uci.get('wireless', sid, 'device') != this.sid)
			return Promise.resolve(false);

		uci.delete('wireless', network);

		return Promise.resolve(true);
	}
});

/**
 * @class
 * @memberof LuCI.Network
 * @hideconstructor
 * @classdesc
 *
 * A `Network.WifiNetwork` instance represents a wireless network (vif)
 * configured on top of a radio device and provides functions for querying
 * the runtime state of the network. Most radio devices support multiple
 * such networks in parallel.
 */
WifiNetwork = L.Class.extend(/** @lends LuCI.Network.WifiNetwork.prototype */ {
	__init__: function(sid, radioname, radiostate, netid, netstate, hostapd) {
		this.sid    = sid;
		this.netid  = netid;
		this._ubusdata = {
			hostapd: hostapd,
			radio:   radioname,
			dev:     radiostate,
			net:     netstate
		};
	},

	ubus: function(/* ... */) {
		var v = this._ubusdata;

		for (var i = 0; i < arguments.length; i++)
			if (L.isObject(v))
				v = v[arguments[i]];
			else
				return null;

		return v;
	},

	/**
	 * Read the given UCI option value of this wireless network.
	 *
	 * @param {string} opt
	 * The UCI option name to read.
	 *
	 * @returns {null|string|string[]}
	 * Returns the UCI option value or `null` if the requested option is
	 * not found.
	 */
	get: function(opt) {
		return uci.get('wireless', this.sid, opt);
	},

	/**
	 * Set the given UCI option of this network to the given value.
	 *
	 * @param {string} opt
	 * The name of the UCI option to set.
	 *
	 * @param {null|string|string[]} val
	 * The value to set or `null` to remove the given option from the
	 * configuration.
	 */
	set: function(opt, value) {
		return uci.set('wireless', this.sid, opt, value);
	},

	/**
	 * Checks whether this wireless network is disabled.
	 *
	 * @returns {boolean}
	 * Returns `true` when the wireless radio is marked as disabled in `ubus`
	 * runtime state or when the `disabled` option is set in the corresponding
	 * UCI configuration.
	 */
	isDisabled: function() {
		return this.ubus('dev', 'disabled') || this.get('disabled') == '1';
	},

	/**
	 * Get the configured operation mode of the wireless network.
	 *
	 * @returns {string}
	 * Returns the configured operation mode. Possible values are:
	 *  - `ap` - Master (Access Point) mode
	 *  - `sta` - Station (client) mode
	 *  - `adhoc` - Ad-Hoc (IBSS) mode
	 *  - `mesh` - Mesh (IEEE 802.11s) mode
	 *  - `monitor` - Monitor mode
	 */
	getMode: function() {
		return this.ubus('net', 'config', 'mode') || this.get('mode') || 'ap';
	},

	/**
	 * Get the configured SSID of the wireless network.
	 *
	 * @returns {null|string}
	 * Returns the configured SSID value or `null` when this network is
	 * in mesh mode.
	 */
	getSSID: function() {
		if (this.getMode() == 'mesh')
			return null;

		return this.ubus('net', 'config', 'ssid') || this.get('ssid');
	},

	/**
	 * Get the configured Mesh ID of the wireless network.
	 *
	 * @returns {null|string}
	 * Returns the configured mesh ID value or `null` when this network
	 * is not in mesh mode.
	 */
	getMeshID: function() {
		if (this.getMode() != 'mesh')
			return null;

		return this.ubus('net', 'config', 'mesh_id') || this.get('mesh_id');
	},

	/**
	 * Get the configured BSSID of the wireless network.
	 *
	 * @returns {null|string}
	 * Returns the BSSID value or `null` if none has been specified.
	 */
	getBSSID: function() {
		return this.ubus('net', 'config', 'bssid') || this.get('bssid');
	},

	/**
	 * Get the names of the logical interfaces this wireless network is
	 * attached to.
	 *
	 * @returns {string[]}
	 * Returns an array of logical interface names.
	 */
	getNetworkNames: function() {
		return L.toArray(this.ubus('net', 'config', 'network') || this.get('network'));
	},

	/**
	 * Get the internal network ID of this wireless network.
	 *
	 * The network ID is a LuCI specific identifer in the form
	 * `radio#.network#` to identify wireless networks by their corresponding
	 * radio and network index numbers.
	 *
	 * @returns {string}
	 * Returns the LuCI specific network ID.
	 */
	getID: function() {
		return this.netid;
	},

	/**
	 * Get the configuration ID of this wireless network.
	 *
	 * @returns {string}
	 * Returns the corresponding UCI section ID of the network.
	 */
	getName: function() {
		return this.sid;
	},

	/**
	 * Get the Linux network device name.
	 *
	 * @returns {null|string}
	 * Returns the current Linux network device name as resolved from
	 * `ubus` runtime information or `null` if this network has no
	 * associated network device, e.g. when not configured or up.
	 */
	getIfname: function() {
		var ifname = this.ubus('net', 'ifname') || this.ubus('net', 'iwinfo', 'ifname');

		if (ifname == null || ifname.match(/^(wifi|radio)\d/))
			ifname = this.netid;

		return ifname;
	},

	/**
	 * Get the name of the corresponding wifi radio device.
	 *
	 * @returns {null|string}
	 * Returns the name of the radio device this network is configured on
	 * or `null` if it cannot be determined.
	 */
	getWifiDeviceName: function() {
		return this.ubus('radio') || this.get('device');
	},

	/**
	 * Get the corresponding wifi radio device.
	 *
	 * @returns {null|LuCI.Network.WifiDevice}
	 * Returns a `Network.WifiDevice` instance representing the corresponding
	 * wifi radio device or `null` if the related radio device could not be
	 * found.
	 */
	getWifiDevice: function() {
		var radioname = this.getWifiDeviceName();

		if (radioname == null)
			return Promise.reject();

		return L.network.getWifiDevice(radioname);
	},

	/**
	 * Check whether the radio network is up.
	 *
	 * This function actually queries the up state of the related radio
	 * device and assumes this network to be up as well when the parent
	 * radio is up. This is due to the fact that OpenWrt does not control
	 * virtual interfaces individually but within one common hostapd
	 * instance.
	 *
	 * @returns {boolean}
	 * Returns `true` when the network is up, else `false`.
	 */
	isUp: function() {
		var device = this.getDevice();

		if (device == null)
			return false;

		return device.isUp();
	},

	/**
	 * Query the current operation mode from runtime information.
	 *
	 * @returns {string}
	 * Returns the human readable mode name as reported by `ubus` runtime
	 * state. Possible returned values are:
	 *  - `Master`
	 *  - `Ad-Hoc`
	 *  - `Client`
	 *  - `Monitor`
	 *  - `Master (VLAN)`
	 *  - `WDS`
	 *  - `Mesh Point`
	 *  - `P2P Client`
	 *  - `P2P Go`
	 *  - `Unknown`
	 */
	getActiveMode: function() {
		var mode = this.ubus('net', 'iwinfo', 'mode') || this.ubus('net', 'config', 'mode') || this.get('mode') || 'ap';

		switch (mode) {
		case 'ap':      return 'Master';
		case 'sta':     return 'Client';
		case 'adhoc':   return 'Ad-Hoc';
		case 'mesh':    return 'Mesh';
		case 'monitor': return 'Monitor';
		default:        return mode;
		}
	},

	/**
	 * Query the current operation mode from runtime information as
	 * translated string.
	 *
	 * @returns {string}
	 * Returns the translated, human readable mode name as reported by
	 *`ubus` runtime state.
	 */
	getActiveModeI18n: function() {
		var mode = this.getActiveMode();

		switch (mode) {
		case 'Master':  return _('Master');
		case 'Client':  return _('Client');
		case 'Ad-Hoc':  return _('Ad-Hoc');
		case 'Mash':    return _('Mesh');
		case 'Monitor': return _('Monitor');
		default:        return mode;
		}
	},

	/**
	 * Query the current SSID from runtime information.
	 *
	 * @returns {string}
	 * Returns the current SSID or Mesh ID as reported by `ubus` runtime
	 * information.
	 */
	getActiveSSID: function() {
		return this.ubus('net', 'iwinfo', 'ssid') || this.ubus('net', 'config', 'ssid') || this.get('ssid');
	},

	/**
	 * Query the current BSSID from runtime information.
	 *
	 * @returns {string}
	 * Returns the current BSSID or Mesh ID as reported by `ubus` runtime
	 * information.
	 */
	getActiveBSSID: function() {
		return this.ubus('net', 'iwinfo', 'bssid') || this.ubus('net', 'config', 'bssid') || this.get('bssid');
	},

	/**
	 * Query the current encryption settings from runtime information.
	 *
	 * @returns {string}
	 * Returns a string describing the current encryption or `-` if the the
	 * encryption state could not be found in `ubus` runtime information.
	 */
	getActiveEncryption: function() {
		return formatWifiEncryption(this.ubus('net', 'iwinfo', 'encryption')) || '-';
	},

	/**
	 * A wireless peer entry describes the properties of a remote wireless
	 * peer associated with a local network.
	 *
	 * @typedef {Object<string, boolean|number|string|LuCI.Network.WifiRateEntry>} WifiPeerEntry
	 * @memberof LuCI.Network
	 *
	 * @property {string} mac
	 * The MAC address (BSSID).
	 *
	 * @property {number} signal
	 * The received signal strength.
	 *
	 * @property {number} [signal_avg]
	 * The average signal strength if supported by the driver.
	 *
	 * @property {number} [noise]
	 * The current noise floor of the radio. May be `0` or absent if not
	 * supported by the driver.
	 *
	 * @property {number} inactive
	 * The amount of milliseconds the peer has been inactive, e.g. due
	 * to powersave.
	 *
	 * @property {number} connected_time
	 * The amount of milliseconds the peer is associated to this network.
	 *
	 * @property {number} [thr]
	 * The estimated throughput of the peer, May be `0` or absent if not
	 * supported by the driver.
	 *
	 * @property {boolean} authorized
	 * Specifies whether the peer is authorized to associate to this network.
	 *
	 * @property {boolean} authenticated
	 * Specifies whether the peer completed authentication to this network.
	 *
	 * @property {string} preamble
	 * The preamble mode used by the peer. May be `long` or `short`.
	 *
	 * @property {boolean} wme
	 * Specifies whether the peer supports WME/WMM capabilities.
	 *
	 * @property {boolean} mfp
	 * Specifies whether management frame protection is active.
	 *
	 * @property {boolean} tdls
	 * Specifies whether TDLS is active.
	 *
	 * @property {number} [mesh llid]
	 * The mesh LLID, may be `0` or absent if not applicable or supported
	 * by the driver.
	 *
	 * @property {number} [mesh plid]
	 * The mesh PLID, may be `0` or absent if not applicable or supported
	 * by the driver.
	 *
	 * @property {string} [mesh plink]
	 * The mesh peer link state description, may be an empty string (`''`)
	 * or absent if not applicable or supported by the driver.
	 *
	 * The following states are known:
	 *  - `LISTEN`
	 *  - `OPN_SNT`
	 *  - `OPN_RCVD`
	 *  - `CNF_RCVD`
	 *  - `ESTAB`
	 *  - `HOLDING`
	 *  - `BLOCKED`
	 *  - `UNKNOWN`
	 *
	 * @property {number} [mesh local PS]
	 * The local powersafe mode for the peer link, may be an empty
	 * string (`''`) or absent if not applicable or supported by
	 * the driver.
	 *
	 * The following modes are known:
	 *  - `ACTIVE` (no power save)
	 *  - `LIGHT SLEEP`
	 *  - `DEEP SLEEP`
	 *  - `UNKNOWN`
	 *
	 * @property {number} [mesh peer PS]
	 * The remote powersafe mode for the peer link, may be an empty
	 * string (`''`) or absent if not applicable or supported by
	 * the driver.
	 *
	 * The following modes are known:
	 *  - `ACTIVE` (no power save)
	 *  - `LIGHT SLEEP`
	 *  - `DEEP SLEEP`
	 *  - `UNKNOWN`
	 *
	 * @property {number} [mesh non-peer PS]
	 * The powersafe mode for all non-peer neigbours, may be an empty
	 * string (`''`) or absent if not applicable or supported by the driver.
	 *
	 * The following modes are known:
	 *  - `ACTIVE` (no power save)
	 *  - `LIGHT SLEEP`
	 *  - `DEEP SLEEP`
	 *  - `UNKNOWN`
	 *
	 * @property {LuCI.Network.WifiRateEntry} rx
	 * Describes the receiving wireless rate from the peer.
	 *
	 * @property {LuCI.Network.WifiRateEntry} tx
	 * Describes the transmitting wireless rate to the peer.
	 */

	/**
	 * A wireless rate entry describes the properties of a wireless
	 * transmission rate to or from a peer.
	 *
	 * @typedef {Object<string, boolean|number>} WifiRateEntry
	 * @memberof LuCI.Network
	 *
	 * @property {number} [drop_misc]
	 * The amount of received misc. packages that have been dropped, e.g.
	 * due to corruption or missing authentication. Only applicable to
	 * receiving rates.
	 *
	 * @property {number} packets
	 * The amount of packets that have been received or sent.
	 *
	 * @property {number} bytes
	 * The amount of bytes that have been received or sent.
	 *
	 * @property {number} [failed]
	 * The amount of failed tranmission attempts. Only applicable to
	 * transmit rates.
	 *
	 * @property {number} [retries]
	 * The amount of retried transmissions. Only applicable to transmit
	 * rates.
	 *
	 * @property {boolean} is_ht
	 * Specifies whether this rate is an HT (IEEE 802.11n) rate.
	 *
	 * @property {boolean} is_vht
	 * Specifies whether this rate is an VHT (IEEE 802.11ac) rate.
	 *
	 * @property {number} mhz
	 * The channel width in MHz used for the transmission.
	 *
	 * @property {number} rate
	 * The bitrate in bit/s of the transmission.
	 *
	 * @property {number} [mcs]
	 * The MCS index of the used transmission rate. Only applicable to
	 * HT or VHT rates.
	 *
	 * @property {number} [40mhz]
	 * Specifies whether the tranmission rate used 40MHz wide channel.
	 * Only applicable to HT or VHT rates.
	 *
	 * Note: this option exists for backwards compatibility only and its
	 * use is discouraged. The `mhz` field should be used instead to
	 * determine the channel width.
	 *
	 * @property {boolean} [short_gi]
	 * Specifies whether a short guard interval is used for the transmission.
	 * Only applicable to HT or VHT rates.
	 *
	 * @property {number} [nss]
	 * Specifies the number of spatial streams used by the transmission.
	 * Only applicable to VHT rates.
	 */

	/**
	 * Fetch the list of associated peers.
	 *
	 * @returns {Promise<Array<LuCI.Network.WifiPeerEntry>>}
	 * Returns a promise resolving to an array of wireless peers associated
	 * with this network.
	 */
	getAssocList: function() {
		return callIwinfoAssoclist(this.getIfname());
	},

	/**
	 * Query the current operating frequency of the wireless network.
	 *
	 * @returns {null|string}
	 * Returns the current operating frequency of the network from `ubus`
	 * runtime information in GHz or `null` if the information is not
	 * available.
	 */
	getFrequency: function() {
		var freq = this.ubus('net', 'iwinfo', 'frequency');

		if (freq != null && freq > 0)
			return '%.03f'.format(freq / 1000);

		return null;
	},

	/**
	 * Query the current average bitrate of all peers associated to this
	 * wireless network.
	 *
	 * @returns {null|number}
	 * Returns the average bit rate among all peers associated to the network
	 * as reported by `ubus` runtime information or `null` if the information
	 * is not available.
	 */
	getBitRate: function() {
		var rate = this.ubus('net', 'iwinfo', 'bitrate');

		if (rate != null && rate > 0)
			return (rate / 1000);

		return null;
	},

	/**
	 * Query the current wireless channel.
	 *
	 * @returns {null|number}
	 * Returns the wireless channel as reported by `ubus` runtime information
	 * or `null` if it cannot be determined.
	 */
	getChannel: function() {
		return this.ubus('net', 'iwinfo', 'channel') || this.ubus('dev', 'config', 'channel') || this.get('channel');
	},

	/**
	 * Query the current wireless signal.
	 *
	 * @returns {null|number}
	 * Returns the wireless signal in dBm as reported by `ubus` runtime
	 * information or `null` if it cannot be determined.
	 */
	getSignal: function() {
		return this.ubus('net', 'iwinfo', 'signal') || 0;
	},

	/**
	 * Query the current radio noise floor.
	 *
	 * @returns {number}
	 * Returns the radio noise floor in dBm as reported by `ubus` runtime
	 * information or `0` if it cannot be determined.
	 */
	getNoise: function() {
		return this.ubus('net', 'iwinfo', 'noise') || 0;
	},

	/**
	 * Query the current country code.
	 *
	 * @returns {string}
	 * Returns the wireless country code as reported by `ubus` runtime
	 * information or `00` if it cannot be determined.
	 */
	getCountryCode: function() {
		return this.ubus('net', 'iwinfo', 'country') || this.ubus('dev', 'config', 'country') || '00';
	},

	/**
	 * Query the current radio TX power.
	 *
	 * @returns {null|number}
	 * Returns the wireless network transmit power in dBm as reported by
	 * `ubus` runtime information or `null` if it cannot be determined.
	 */
	getTXPower: function() {
		return this.ubus('net', 'iwinfo', 'txpower');
	},

	/**
	 * Query the radio TX power offset.
	 *
	 * Some wireless radios have a fixed power offset, e.g. due to the
	 * use of external amplifiers.
	 *
	 * @returns {number}
	 * Returns the wireless network transmit power offset in dBm as reported
	 * by `ubus` runtime information or `0` if there is no offset, or if it
	 * cannot be determined.
	 */
	getTXPowerOffset: function() {
		return this.ubus('net', 'iwinfo', 'txpower_offset') || 0;
	},

	/**
	 * Calculate the current signal.
	 *
	 * @deprecated
	 * @returns {number}
	 * Returns the calculated signal level, which is the difference between
	 * noise and signal (SNR), divided by 5.
	 */
	getSignalLevel: function(signal, noise) {
		if (this.getActiveBSSID() == '00:00:00:00:00:00')
			return -1;

		signal = signal || this.getSignal();
		noise  = noise  || this.getNoise();

		if (signal < 0 && noise < 0) {
			var snr = -1 * (noise - signal);
			return Math.floor(snr / 5);
		}

		return 0;
	},

	/**
	 * Calculate the current signal quality percentage.
	 *
	 * @returns {number}
	 * Returns the calculated signal quality in percent. The value is
	 * calculated from the `quality` and `quality_max` indicators reported
	 * by `ubus` runtime state.
	 */
	getSignalPercent: function() {
		var qc = this.ubus('net', 'iwinfo', 'quality') || 0,
		    qm = this.ubus('net', 'iwinfo', 'quality_max') || 0;

		if (qc > 0 && qm > 0)
			return Math.floor((100 / qm) * qc);

		return 0;
	},

	/**
	 * Get a short description string for this wireless network.
	 *
	 * @returns {string}
	 * Returns a string describing this network, consisting of the
	 * active operation mode, followed by either the SSID, BSSID or
	 * internal network ID, depending on which information is available.
	 */
	getShortName: function() {
		return '%s "%s"'.format(
			this.getActiveModeI18n(),
			this.getActiveSSID() || this.getActiveBSSID() || this.getID());
	},

	/**
	 * Get a description string for this wireless network.
	 *
	 * @returns {string}
	 * Returns a string describing this network, consisting of the
	 * term `Wireless Network`, followed by the active operation mode,
	 * the SSID, BSSID or internal network ID and the Linux network device
	 * name, depending on which information is available.
	 */
	getI18n: function() {
		return '%s: %s "%s" (%s)'.format(
			_('Wireless Network'),
			this.getActiveModeI18n(),
			this.getActiveSSID() || this.getActiveBSSID() || this.getID(),
			this.getIfname());
	},

	/**
	 * Get the primary logical interface this wireless network is attached to.
	 *
	 * @returns {null|LuCI.Network.Protocol}
	 * Returns a `Network.Protocol` instance representing the logical
	 * interface or `null` if this network is not attached to any logical
	 * interface.
	 */
	getNetwork: function() {
		return this.getNetworks()[0];
	},

	/**
	 * Get the logical interfaces this wireless network is attached to.
	 *
	 * @returns {Array<LuCI.Network.Protocol>}
	 * Returns an array of `Network.Protocol` instances representing the
	 * logical interfaces this wireless network is attached to.
	 */
	getNetworks: function() {
		var networkNames = this.getNetworkNames(),
		    networks = [];

		for (var i = 0; i < networkNames.length; i++) {
			var uciInterface = uci.get('network', networkNames[i]);

			if (uciInterface == null || uciInterface['.type'] != 'interface')
				continue;

			networks.push(L.network.instantiateNetwork(networkNames[i]));
		}

		networks.sort(networkSort);

		return networks;
	},

	/**
	 * Get the associated Linux network device.
	 *
	 * @returns {LuCI.Network.Device}
	 * Returns a `Network.Device` instance representing the Linux network
	 * device associted with this wireless network.
	 */
	getDevice: function() {
		return L.network.instantiateDevice(this.getIfname());
	},

	/**
	 * Check whether this wifi network supports deauthenticating clients.
	 *
	 * @returns {boolean}
	 * Returns `true` when this wifi network instance supports forcibly
	 * deauthenticating clients, otherwise `false`.
	 */
	isClientDisconnectSupported: function() {
		return L.isObject(this.ubus('hostapd', 'del_client'));
	},

	/**
	 * Forcibly disconnect the given client from the wireless network.
	 *
	 * @param {string} mac
	 * The MAC address of the client to disconnect.
	 *
	 * @param {boolean} [deauth=false]
	 * Specifies whether to deauthenticate (`true`) or disassociate (`false`)
	 * the client.
	 *
	 * @param {number} [reason=1]
	 * Specifies the IEEE 802.11 reason code to disassoc/deauth the client
	 * with. Default is `1` which corresponds to `Unspecified reason`.
	 *
	 * @param {number} [ban_time=0]
	 * Specifies the amount of milliseconds to ban the client from
	 * reconnecting. By default, no ban time is set which allows the client
	 * to reassociate / reauthenticate immediately.
	 *
	 * @returns {Promise<number>}
	 * Returns a promise resolving to the underlying ubus call result code
	 * which is typically `0`, even for not existing MAC addresses.
	 * The promise might reject with an error in case invalid arguments
	 * are passed.
	 */
	disconnectClient: function(mac, deauth, reason, ban_time) {
		if (reason == null || reason == 0)
			reason = 1;

		if (ban_time == 0)
			ban_time = null;

		return rpc.declare({
			object: 'hostapd.%s'.format(this.getIfname()),
			method: 'del_client',
			params: [ 'addr', 'deauth', 'reason', 'ban_time' ]
		})(mac, deauth, reason, ban_time);
	}
});

return Network;
