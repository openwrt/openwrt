'use strict';
'require form';
'require network';

return network.registerProtocol('dhcpv6', {
	getI18n: function() {
		return _('DHCPv6 client');
	},

	getOpkgPackage: function() {
		return 'odhcp6c';
	},

	renderFormOptions: function(s) {
		var dev = this.getL2Device() || this.getDevice(), o;

		o = s.taboption('general', form.ListValue, 'reqaddress', _('Request IPv6-address'));
		o.value('try');
		o.value('force');
		o.value('none', 'disabled');
		o.default = 'try';

		o = s.taboption('general', form.Value, 'reqprefix', _('Request IPv6-prefix of length'));
		o.value('auto', _('Automatic'));
		o.value('no', _('disabled'));
		o.value('48');
		o.value('52');
		o.value('56');
		o.value('60');
		o.value('64');
		o.default = 'auto';

		o = s.taboption('advanced', form.Flag, 'defaultroute', _('Use default gateway'), _('If unchecked, no default route is configured'));
		o.default = o.enabled;

		o = s.taboption('advanced', form.DynamicList, 'ip6prefix', _('Custom delegated IPv6-prefix'));
		o.datatype = 'cidr6';

		o = s.taboption('advanced', form.Flag, 'peerdns', _('Use DNS servers advertised by peer'), _('If unchecked, the advertised DNS server addresses are ignored'));
		o.default = o.enabled;

		o = s.taboption('advanced', form.DynamicList, 'dns', _('Use custom DNS servers'));
		o.depends('peerdns', '0');
		o.datatype = 'ipaddr';
		o.cast     = 'string';

		o = s.taboption('advanced', form.Value, 'clientid', _('Client ID to send when requesting DHCP'));
		o.datatype  = 'hexstring';

		o = s.taboption('advanced', form.Value, 'macaddr', _('Override MAC address'));
		o.datatype = 'macaddr';
		o.placeholder = dev ? (dev.getMAC() || '') : '';

		o = s.taboption('advanced', form.Value, 'mtu', _('Override MTU'));
		o.placeholder = dev ? (dev.getMTU() || '1500') : '1500';
		o.datatype    = 'max(9200)';
	}
});
