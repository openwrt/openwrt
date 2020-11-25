'use strict';
'require rpc';
'require form';
'require network';

var callFileList = rpc.declare({
	object: 'file',
	method: 'list',
	params: [ 'path' ],
	expect: { entries: [] },
	filter: function(list, params) {
		var rv = [];
		for (var i = 0; i < list.length; i++)
			if (list[i].name.match(/^ttyUSB/) || list[i].name.match(/^cdc-wdm/))
				rv.push(params.path + list[i].name);
		return rv.sort();
	}
});

network.registerPatternVirtual(/^ncm-.+$/);
network.registerErrorCode('CONFIGURE_FAILED',  _('Configuration failed'));
network.registerErrorCode('DISCONNECT_FAILED', _('Disconnection attempt failed'));
network.registerErrorCode('FINALIZE_FAILED',   _('Finalizing failed'));
network.registerErrorCode('GETINFO_FAILED',    _('Modem information query failed'));
network.registerErrorCode('INITIALIZE_FAILED', _('Initialization failure'));
network.registerErrorCode('SETMODE_FAILED',    _('Setting operation mode failed'));
network.registerErrorCode('UNSUPPORTED_MODEM', _('Unsupported modem'));

return network.registerProtocol('ncm', {
	getI18n: function() {
		return _('NCM');
	},

	getIfname: function() {
		return this._ubus('l3_device') || 'wan';
	},

	getOpkgPackage: function() {
		return 'comgt-ncm';
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

		o = s.taboption('general', form.Value, 'device', _('Modem device'));
		o.rmempty = false;
		o.load = function(section_id) {
			return callFileList('/dev/').then(L.bind(function(devices) {
				for (var i = 0; i < devices.length; i++)
					this.value(devices[i]);
				return form.Value.prototype.load.apply(this, [section_id]);
			}, this));
		};

		o = s.taboption('general', form.Value, 'service', _('Service Type'));
		o.value('', _('Modem default'));
		o.value('preferlte', _('Prefer LTE'));
		o.value('preferumts', _('Prefer UMTS'));
		o.value('lte', 'LTE');
		o.value('umts', 'UMTS/GPRS');
		o.value('gsm', _('GPRS only'));
		o.value('auto', _('auto'));

		o = s.taboption('general', form.ListValue, 'pdptype', _('IP Protocol'));
		o.default = 'IP';
		o.value('IP', _('IPv4'));
		o.value('IPV4V6', _('IPv4+IPv6'));
		o.value('IPV6', _('IPv6'));

		s.taboption('general', form.Value, 'apn', _('APN'));
		s.taboption('general', form.Value, 'pincode', _('PIN'));
		s.taboption('general', form.Value, 'username', _('PAP/CHAP username'));

		o = s.taboption('general', form.Value, 'password', _('PAP/CHAP password'));
		o.password = true;

		o = s.taboption('general', form.Value, 'dialnumber', _('Dial number'));
		o.placeholder = '*99***1#';

		if (L.hasSystemFeature('ipv6')) {
			o = s.taboption('advanced', form.ListValue, 'ipv6', _('Obtain IPv6-Address'));
			o.value('auto', _('Automatic'));
			o.value('0', _('Disabled'));
			o.value('1', _('Manual'));
			o.default = 'auto';
		}

		o = s.taboption('advanced', form.Value, 'delay', _('Modem init timeout'), _('Maximum amount of seconds to wait for the modem to become ready'));
		o.placeholder = '10';
		o.datatype    = 'min(1)';

		o = s.taboption('advanced', form.Flag, 'defaultroute', _('Default gateway'), _('If unchecked, no default route is configured'));
		o.default = o.enabled;

		o = s.taboption('advanced', form.Value, 'metric', _('Use gateway metric'));
		o.placeholder = '0';
		o.datatype    = 'uinteger';
		o.depends('defaultroute', '1');

		o = s.taboption('advanced', form.Flag, 'peerdns', _('Use DNS servers advertised by peer'), _('If unchecked, the advertised DNS server addresses are ignored'));
		o.default = o.enabled;

		o = s.taboption('advanced', form.DynamicList, 'dns', _('Use custom DNS servers'));
		o.depends('peerdns', '0');
		o.datatype = 'ipaddr';
	}
});
