'use strict';
'require form';
'require network';
'require tools.widgets as widgets';

network.registerPatternVirtual(/^map-.+$/);
network.registerErrorCode('INVALID_MAP_RULE', _('MAP rule is invalid'));
network.registerErrorCode('NO_MATCHING_PD',   _('No matching prefix delegation'));
network.registerErrorCode('UNSUPPORTED_TYPE', _('Unsupported MAP type'));

return network.registerProtocol('map', {
	getI18n: function() {
		return _('MAP / LW4over6');
	},

	getIfname: function() {
		return this._ubus('l3_device') || 'map-%s'.format(this.sid);
	},

	getOpkgPackage: function() {
		return 'map-t';
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

		o = s.taboption('general', form.ListValue, 'type', _('Type'));
		o.value('map-e', 'MAP-E');
		o.value('map-t', 'MAP-T');
		o.value('lw4o6', 'LW4over6');

		o = s.taboption('general', form.Value, 'peeraddr', _('BR / DMR / AFTR'));
		o.rmempty  = false;
		o.datatype = 'ip6addr';

		o = s.taboption('general', form.Value, 'ipaddr', _('IPv4 prefix'));
		o.datatype = 'ip4addr';

		o = s.taboption('general', form.Value, 'ip4prefixlen', _('IPv4 prefix length'), _('The length of the IPv4 prefix in bits, the remainder is used in the IPv6 addresses.'));
		o.placeholder = '32';
		o.datatype    = 'range(0,32)';

		o = s.taboption('general', form.Value, 'ip6prefix', _('IPv6 prefix'), _('The IPv6 prefix assigned to the provider, usually ends with <code>::</code>'));
		o.rmempty  = false;
		o.datatype = 'ip6addr';

		o = s.taboption('general', form.Value, 'ip6prefixlen', _('IPv6 prefix length'), _('The length of the IPv6 prefix in bits'));
		o.placeholder = '16';
		o.datatype    = 'range(0,64)';

		o = s.taboption('general', form.Value, 'ealen', _('EA-bits length'));
		o.datatype = 'range(0,48)';

		o = s.taboption('general', form.Value, 'psidlen', _('PSID-bits length'));
		o.datatype = 'range(0,16)';

		o = s.taboption('general', form.Value, 'offset', _('PSID offset'));
		o.datatype = 'range(0,16)';

		o = s.taboption('advanced', widgets.NetworkSelect, 'tunlink', _('Tunnel Link'));
		o.nocreate = true;
		o.exclude  = s.section;

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
