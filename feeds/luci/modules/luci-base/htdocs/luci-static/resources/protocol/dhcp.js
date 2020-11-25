'use strict';
'require rpc';
'require form';
'require network';

var callFileRead = rpc.declare({
	object: 'file',
	method: 'read',
	params: [ 'path' ],
	expect: { data: '' },
	filter: function(value) { return value.trim() }
});

return network.registerProtocol('dhcp', {
	getI18n: function() {
		return _('DHCP client');
	},

	renderFormOptions: function(s) {
		var dev = this.getL2Device() || this.getDevice(), o;

		o = s.taboption('general', form.Value, 'hostname', _('Hostname to send when requesting DHCP'));
		o.datatype    = 'hostname';
		o.load = function(section_id) {
			return callFileRead('/proc/sys/kernel/hostname').then(L.bind(function(hostname) {
				this.placeholder = hostname;
				return form.Value.prototype.load.apply(this, [section_id]);
			}, this));
		};

		o = s.taboption('advanced', form.Flag, 'broadcast', _('Use broadcast flag'), _('Required for certain ISPs, e.g. Charter with DOCSIS 3'));
		o.default = o.disabled;

		o = s.taboption('advanced', form.Flag, 'defaultroute', _('Use default gateway'), _('If unchecked, no default route is configured'));
		o.default = o.enabled;

		o = s.taboption('advanced', form.Flag, 'peerdns', _('Use DNS servers advertised by peer'), _('If unchecked, the advertised DNS server addresses are ignored'));
		o.default = o.enabled;

		o = s.taboption('advanced', form.DynamicList, 'dns', _('Use custom DNS servers'));
		o.depends('peerdns', '0');
		o.datatype = 'ipaddr';
		o.cast     = 'string';

		o = s.taboption('advanced', form.Value, 'metric', _('Use gateway metric'));
		o.placeholder = '0';
		o.datatype    = 'uinteger';

		o = s.taboption('advanced', form.Value, 'clientid', _('Client ID to send when requesting DHCP'));
		o.datatype  = 'hexstring';

		s.taboption('advanced', form.Value, 'vendorid', _('Vendor Class to send when requesting DHCP'));

		o = s.taboption('advanced', form.Value, 'macaddr', _('Override MAC address'));
		o.datatype = 'macaddr';
		o.placeholder = dev ? (dev.getMAC() || '') : '';

		o = s.taboption('advanced', form.Value, 'mtu', _('Override MTU'));
		o.placeholder = dev ? (dev.getMTU() || '1500') : '1500';
		o.datatype    = 'max(9200)';
	}
});
