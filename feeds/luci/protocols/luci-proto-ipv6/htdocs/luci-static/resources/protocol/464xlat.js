'use strict';
'require form';
'require network';
'require tools.widgets as widgets';

network.registerPatternVirtual(/^464-.+$/);
network.registerErrorCode('CLAT_CONFIG_FAILED', _('CLAT configuration failed'));

return network.registerProtocol('464xlat', {
	getI18n: function() {
		return _('464XLAT (CLAT)');
	},

	getIfname: function() {
		return this._ubus('l3_device') || '464-%s'.format(this.sid);
	},

	getOpkgPackage: function() {
		return '464xlat';
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

		o = s.taboption('general', form.Value, 'ip6prefix', _('NAT64 Prefix'), _('Leave empty to autodetect'));
		o.datatype = 'cidr6';

		o = s.taboption('advanced', widgets.NetworkSelect, 'tunlink', _('Tunnel Link'));
		o.nocreate = true;
		o.exclude  = s.section;

		o = s.taboption('advanced', form.Flag, 'defaultroute', _('Default gateway'), _('If unchecked, no default route is configured'));
		o.default = o.enabled;

		o = s.taboption('advanced', form.Value, 'metric', _('Use gateway metric'));
		o.placeholder = '0';
		o.datatype    = 'uinteger';
		o.depends('defaultroute', '1');

		o = s.taboption('advanced', form.Value, 'mtu', _('Use MTU on tunnel interface'));
		o.placeholder = '1280';
		o.datatype    = 'max(9200)';
	}
});
