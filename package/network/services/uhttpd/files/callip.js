'use strict';
'require uci';
'require form';
'require network';
'require tools.widgets as widgets';

network.registerPatternVirtual(/^relay-.+$/);

var RelayDevicePrototype = {
	__init__: function(ifname, network) {
		this.ifname  = ifname;
		this.network = network;
	},

	_aggregateDevices: function(fn, first) {
		var devices = this.network ? this.network.getDevices() : [],
		    rv = 0;

		for (var i = 0; i < devices.length; i++) {
			var v = devices[i][fn].apply(devices[i]);

			if (v != null) {
				if (first)
					return v;

				rv += v;
			}
		}

		return first ? null : [ rv, devices.length ];
	},

	getPorts: function() { return this.network ? this.network.getDevices() : [] },

	getType: function() { return 'tunnel' },
	getTypeI18n: function() { return _('Relay Bridge') },

	getShortName: function() {
		return '%s "%h"'.format(_('Relay'), this.ifname);
	},

	isUp: function() {
		var res = this._aggregateDevices('isUp');
		return (res[1] > 0 && res[0] == res[1]);
	},

	getTXBytes: function() { return this._aggregateDevices('getTXBytes')[0] },
	getRXBytes: function() { return this._aggregateDevices('getRXBytes')[0] },
	getTXPackets: function() { return this._aggregateDevices('getTXPackets')[0] },
	getRXPackets: function() { return this._aggregateDevices('getRXPackets')[0] },

	getMAC: function() { return this._aggregateDevices('getMAC', true) },

	getIPAddrs: function() {
		var ipaddr = this.network ? L.toArray(uci.get('network', this.network.getName(), 'ipaddr'))[0] : null;
		return (ipaddr != null ? [ ipaddr ] : []);
	},

	getIP6Addrs: function() { return [] }
};

return network.registerProtocol('relay', {
	getI18n: function() {
		return _('Relay bridge');
	},

	getIfname: function() {
		return 'relay-%s'.format(this.sid);
	},

	getOpkgPackage: function() {
		return 'relayd';
	},

	isFloating: function() {
		return true;
	},

	isVirtual: function() {
		return true;
	},

	containsDevice: function(ifname) {
		return (network.getIfnameOf(ifname) == this.getIfname());
	},

	isUp: function() {
		var dev = this.getDevice();
		return (dev ? dev.isUp() : false);
	},

	getDevice: function() {
		return network.instantiateDevice(this.sid, this, RelayDevicePrototype);
	},

	getDevices: function() {
		if (this.devices)
			return this.devices;

		var networkNames = L.toArray(uci.get('network', this.sid, 'network')),
		    deviceNames = L.toArray(uci.get('network', this.sid, 'ifname')),
		    devices = {},
		    rv = [];

		for (var i = 0; i < networkNames.length; i++) {
			var net = network.instantiateNetwork(networkNames[i]),
			    dev = net ? net.getDevice() : null;

			if (dev)
				devices[dev.getName()] = dev;
		}

		for (var i = 0; i < deviceNames.length; i++) {
			var dev = network.getDevice(deviceNames[i]);

			if (dev)
				devices[dev.getName()] = dev;
		}

		deviceNames = Object.keys(devices);
		deviceNames.sort();

		for (var i = 0; i < deviceNames.length; i++)
			rv.push(devices[deviceNames[i]]);

		this.devices = rv;

		return rv;
	},

	getUptime: function() {
		var networkNames = L.toArray(uci.get('network', this.sid, 'network')),
		    uptime = 0;

		for (var i = 0; i < networkNames.length; i++) {
			var net = network.instantiateNetwork(networkNames[i]);
			if (net)
				uptime = Math.max(uptime, net.getUptime());
		}

		return uptime;
	},

	getErrors: function() {
		return null;
	},

	renderFormOptions: function(s) {
		var o;

		o = s.taboption('general', form.Value, 'ipaddr', _('Local IPv4 address'), _('Address to access local relay bridge'));
		o.datatype = 'ip4addr("nomask")';

		o = s.taboption('general', widgets.NetworkSelect, 'network', _('Relay between networks'));
		o.exclude = s.section;
		o.multiple = true;
		o.nocreate = true;
		o.nobridges = true;
		o.novirtual = true;

		o = s.taboption('advanced', form.Flag, 'forward_bcast', _('Forward broadcast traffic'));
		o.default = o.enabled;

		o = s.taboption('advanced', form.Flag, 'forward_dhcp', _('Forward DHCP traffic'));
		o.default = o.enabled;

		o = s.taboption('advanced', form.Value, 'gateway', _('Use DHCP gateway'), _('Override the gateway in DHCP responses'));
		o.datatype = 'ip4addr("nomask")';
		o.depends('forward_dhcp', '1');

		o = s.taboption('advanced', form.Value, 'expiry', _('Host expiry timeout'), _('Specifies the maximum amount of seconds after which hosts are presumed to be dead'));
		o.placeholder = '30';
		o.datatype    = 'min(1)';

		o = s.taboption('advanced', form.Value, 'retry', _('ARP retry threshold'), _('Specifies the maximum amount of failed ARP requests until hosts are presumed to be dead'));
		o.placeholder = '5';
		o.datatype    = 'min(1)';

		o = s.taboption('advanced', form.Value, 'table', _('Use routing table'), _('Override the table used for internal routes'));
		o.placeholder = '16800';
		o.datatype    = 'range(0,65535)';
	}
});
