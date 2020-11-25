'use strict';
'require uci';
'require form';
'require network';

network.registerPatternVirtual(/^6in4-.+$/);

return network.registerProtocol('6in4', {
	getI18n: function() {
		return _('IPv6-in-IPv4 (RFC4213)');
	},

	getIfname: function() {
		return this._ubus('l3_device') || '6in4-%s'.format(this.sid);
	},

	getOpkgPackage: function() {
		return '6in4';
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

		o = s.taboption('general', form.Value, 'peeraddr', _('Remote IPv4 address'), _('This is usually the address of the nearest PoP operated by the tunnel broker'));
		o.rmempty  = false;
		o.datatype = 'ip4addr("nomask")';

		o = s.taboption('general', form.Value, 'ip6addr', _('Local IPv6 address'), _('This is the local endpoint address assigned by the tunnel broker, it usually ends with <code>...:2/64</code>'));
		o.datatype = 'cidr6';

		o = s.taboption('general', form.DynamicList, 'ip6prefix', _('IPv6 routed prefix'), _('This is the prefix routed to you by the tunnel broker for use by clients'));
		o.datatype = 'cidr6';

		o = s.taboption('general', form.Flag, '_update', _('Dynamic tunnel'), _('Enable HE.net dynamic endpoint update'));
		o.enabled  = '1';
		o.disabled = '0';
		o.write = function() {};
		o.remove = function() {};
		o.cfgvalue = function(section_id) {
			return !isNaN(+uci.get('network', section_id, 'tunnelid')) ? this.enabled : this.disabled;
		};

		o = s.taboption('general', form.Value, 'tunnelid', _('Tunnel ID'));
		o.datatype = 'uinteger';
		o.depends('_update', '1');

		o = s.taboption('general', form.Value, 'username', _('HE.net username'), _('This is the plain username for logging into the account'));
		o.depends('_update', '1');
		o.validate = function(section_id, value) {
			if (/^[a-fA-F0-9]{32}$/.test(value))
				return _('The HE.net endpoint update configuration changed, you must now use the plain username instead of the user ID!');
			return true;
		};

		o = s.taboption('general', form.Value, 'password', _('HE.net password'), _('This is either the "Update Key" configured for the tunnel or the account password if no update key has been configured'));
		o.password = true;
		o.depends('_update', '1');

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
