'use strict';
'require form';
'require network';

network.registerPatternVirtual(/^6rd-.+$/);

return network.registerProtocol('6rd', {
	getI18n: function() {
		return _('IPv6-over-IPv4 (6rd)');
	},

	getIfname: function() {
		return this._ubus('l3_device') || '6rd-%s'.format(this.sid);
	},

	getOpkgPackage: function() {
		return '6rd';
	},

	isFloating: function() {
		return true;
	},

	isVirtual: function() {
		return true;
	},

	getDevices: function() {
		return null;
	},

	containsDevice: function(ifname) {
		return (network.getIfnameOf(ifname) == this.getIfname());
	},

	renderFormOptions: function(s) {
		var o;

		o = s.taboption('general', form.Value, 'ipaddr', _('Local IPv4 address'), _('Leave empty to use the current WAN address'));
		o.datatype = 'ip4addr("nomask")';
		o.load = function(section_id) {
			return network.getWANNetworks().then(L.bind(function(nets) {
				if (nets.length)
					this.placeholder = nets[0].getIPAddr();
				return form.Value.prototype.load.apply(this, [section_id]);
			}, this));
		};

		o = s.taboption('general', form.Value, 'peeraddr', _('Remote IPv4 address'), _('This IPv4 address of the relay'));
		o.rmempty  = false;
		o.datatype = 'ip4addr("nomask")';

		o = s.taboption('general', form.Value, 'ip6prefix', _('IPv6 prefix'), _('The IPv6 prefix assigned to the provider, usually ends with <code>::</code>'));
		o.rmempty  = false;
		o.datatype = 'ip6addr';

		o = s.taboption('general', form.Value, 'ip6prefixlen', _('IPv6 prefix length'), _('The length of the IPv6 prefix in bits'));
		o.placeholder = '16';
		o.datatype    = 'range(0,128)';

		o = s.taboption('general', form.Value, 'ip4prefixlen', _('IPv4 prefix length'), _('The length of the IPv4 prefix in bits, the remainder is used in the IPv6 addresses.'));
		o.placeholder = '0';
		o.datatype    = 'range(0,32)';

		o = s.taboption('advanced', form.Flag, 'defaultroute', _('Default gateway'), _('If unchecked, no default route is configured'));
		o.default = o.enabled;

		o = s.taboption('advanced', form.Value, 'metric', _('Use gateway metric'));
		o.placeholder = '0';
		o.datatype    = 'uinteger';
		o.depends('defaultroute', '1');

		o = s.taboption('advanced', form.Value, 'ttl', _('Use TTL on tunnel interface'));
		o.placeholder = '64';
		o.datatype    = 'range(1,255)';

		o = s.taboption('advanced', form.Value, 'mtu', _('Use MTU on tunnel interface'));
		o.placeholder = '1280';
		o.datatype    = 'max(9200)';
	}
});
