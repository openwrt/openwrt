'use strict';
'require form';
'require network';
'require tools.widgets as widgets';

network.registerPatternVirtual(/^ipip-.+$/);

return network.registerProtocol('ipip', {
	getI18n: function() {
		return _('IPv4-in-IPv4 (RFC2003)');
	},

	getIfname: function() {
		return this._ubus('l3_device') || 'ipip-%s'.format(this.sid);
	},

	getOpkgPackage: function() {
		return 'ipip';
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

		o = s.taboption('general', form.Value, 'peeraddr', _('Remote IPv4 address or FQDN'), _('The IPv4 address or the fully-qualified domain name of the remote tunnel end.'));
		o.optional = false;
		o.datatype = 'or(hostname,ip4addr("nomask"))';

		o = s.taboption('general', form.Value, 'ipaddr', _('Local IPv4 address'), _('The local IPv4 address over which the tunnel is created (optional).'));
		o.optional = true;
		o.datatype = 'ip4addr("nomask")';

		o = s.taboption('general', widgets.NetworkSelect, 'tunlink', _('Bind interface'), _('Bind the tunnel to this interface (optional).'));
		o.exclude = s.section;
		o.nocreate = true;
		o.optional = true;

		o = s.taboption('advanced', form.Value, 'mtu', _('Override MTU'), _('Specify an MTU (Maximum Transmission Unit) other than the default (1280 bytes).'));
		o.optional = true;
		o.placeholder = 1280;
		o.datatype = 'range(68, 9200)';

		o = s.taboption('advanced', form.Value, 'ttl', _('Override TTL'), _('Specify a TTL (Time to Live) for the encapsulating packet other than the default (64).'));
		o.optional = true;
		o.placeholder = 64;
		o.datatype = 'min(1)';

		o = s.taboption('advanced', form.Value, 'tos', _('Override TOS'), _('Specify a TOS (Type of Service).'));
		o.optional = true;
		o.datatype = 'range(0, 255)';

		s.taboption('advanced', form.Flag, 'df', _("Don't Fragment"), _("Enable the DF (Don't Fragment) flag of the encapsulating packets."));
	}
});
