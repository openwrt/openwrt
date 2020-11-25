'use strict';
'require form';
'require network';

network.registerPatternVirtual(/^vpn-.+$/);

return network.registerProtocol('vpnc', {
	getI18n: function() {
		return _('VPNC (CISCO 3000 (and others) VPN)');
	},

	getIfname: function() {
		return this._ubus('l3_device') || 'vpn-%s'.format(this.sid);
	},

	getOpkgPackage: function() {
		return 'vpnc';
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

		o = s.taboption('general', form.Value, 'server', _('VPN Server'));
		o.datatype = 'host(0)';

		o = s.taboption('general', form.Value, 'local_addr', _('VPN Local address'));
		o.placeholder = '0.0.0.0';
		o.datatype    = 'ipaddr';

		o = s.taboption('general', form.Value, 'local_port', _('VPN Local port'));
		o.placeholder = '500';
		o.datatype    = 'port';

		o = s.taboption('general', form.Value, 'interface', _('Output Interface'));
		o.template = 'cbi/network_netlist';

		o = s.taboption('general', form.Value, 'mtu', _('MTU'));
		o.datatype = 'uinteger';

		s.taboption('general', form.Value, 'username', _('Username'));

		o = s.taboption('general', form.Value, 'password', _('Password'));
		o.password = true;

		o = s.taboption('general', form.Value, 'hexpassword', _('Obfuscated Password'));
		o.password = true;

		s.taboption('general', form.Value, 'authgroup', _('Auth Group'));

		o = s.taboption('general', form.Value, 'passgroup', _('Group Password'));
		o.password = true;

		o = s.taboption('general', form.Value, 'hexpassgroup', _('Obfuscated Group Password'));
		o.password= true;

		s.taboption('general', form.Value, 'domain', _('NT Domain'));
		s.taboption('general', form.Value, 'vendor', _('Vendor'));

		o = s.taboption('general', form.ListValue, 'dh_group', _('IKE DH Group'));
		o.value('dh2');
		o.value('dh1');
		o.value('dh5');

		o = s.taboption('general', form.ListValue, 'pfs', _('Perfect Forward Secrecy'));
		o.value('server');
		o.value('nopfs');
		o.value('dh1');
		o.value('dh2');
		o.value('dh5');

		o = s.taboption('general', form.ListValue, 'natt_mode', _('NAT-T Mode'));
		o.value('natt', _('RFC3947 NAT-T mode'));
		o.value('none', _('No NAT-T'));
		o.value('force-natt', _('Force use of NAT-T'));
		o.value('cisco-udp', _('Cisco UDP encapsulation'));

		o = s.taboption('general', form.Flag, 'enable_no_enc', _('Disable Encryption'), _('If checked, encryption is disabled'));
		o.default = o.disabled;

		o = s.taboption('general', form.Flag, 'enable_single_des', _('Enable Single DES'), _('If checked, 1DES is enabled'));
		o.default = o.disabled;

		o = s.taboption('general', form.Value, 'dpd_idle', _('DPD Idle Timeout'));
		o.datatype = 'uinteger';
		o.placeholder = '600';

		o = s.taboption('general', form.Value, 'target_network', _('Target network'));
		o.placeholder = '0.0.0.0/0';
		o.datatype    = 'network';

		o = s.taboption('general', form.ListValue, 'defaultroute', _('Default Route'), _('Set VPN as Default Route'));
		o.value('0', _('No'));
		o.value('1', _('Yes'));
	}
});
