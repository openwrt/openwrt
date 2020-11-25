'use strict';
'require form';
'require network';

network.registerPatternVirtual(/^6to4-.+$/);

return network.registerProtocol('6to4', {
	getI18n: function() {
		return _('IPv6-over-IPv4 (6to4)');
	},

	getIfname: function() {
		return this._ubus('l3_device') || '6to4-%s'.format(this.sid);
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
