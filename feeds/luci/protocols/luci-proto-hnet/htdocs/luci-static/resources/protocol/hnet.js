'use strict';
'require form';
'require network';

return network.registerProtocol('hnet', {
	getI18n: function() {
		return _('Automatic Homenet (HNCP)');
	},

	getOpkgPackage: function() {
		return 'hnet-full';
	},

	renderFormOptions: function(s) {
		var dev = this.getL2Device() || this.getDevice(), o;

		o = s.taboption('general', form.ListValue, 'mode', _('Category'));
		o.value('auto', _('Automatic'));
		o.value('external', _('External'));
		o.value('internal', _('Internal'));
		o.value('leaf', _('Leaf'));
		o.value('guest', _('Guest'));
		o.value('adhoc', _('Ad-Hoc'));
		o.value('hybrid', _('Hybrid'));
		o.default = 'auto';

		o = s.taboption('advanced', form.Value, 'ip6assign', _('IPv6 assignment length'), _('Assign a part of given length of every public IPv6-prefix to this interface'));
		o.datatype = 'max(128)';
		o.default = '64';

		s.taboption('advanced', form.Value, 'link_id', _('IPv6 assignment hint'), _('Assign prefix parts using this hexadecimal subprefix ID for this interface.'));

		o = s.taboption('advanced', form.Value, 'ip4assign', _('IPv4 assignment length'));
		o.datatype = 'max(32)';
		o.default = '24';

		o = s.taboption('advanced', form.Value, 'dnsname', _('DNS-Label / FQDN'));
		o.default = s.section;

		o = s.taboption('advanced', form.Value, 'macaddr', _('Override MAC address'));
		o.datatype = 'macaddr';
		o.placeholder = dev ? (dev.getMAC() || '') : '';

		o = s.taboption('advanced', form.Value, 'mtu', _('Override MTU'));
		o.datatype = 'max(9200)';
		o.placeholder = dev ? (dev.getMTU() || '1500') : '1500';
	}
});
