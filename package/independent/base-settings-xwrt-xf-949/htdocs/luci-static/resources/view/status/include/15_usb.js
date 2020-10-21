'use strict';
'require baseclass';
'require fs';
'require rpc';

var callUsbInfo = rpc.declare({
	object: 'luci.usb',
	method: 'info'
});

var callWwanStatus = rpc.declare({
	object: 'network.interface.wwan',
	method: 'status',
	expect: { }
});

var callWanStatus = rpc.declare({
	object: 'network.interface.wan',
	method: 'status',
	expect: { }
});

var callLanStatus = rpc.declare({
	object: 'network.interface.lan',
	method: 'status',
	expect: { }
});

var callLanapStatus = rpc.declare({
	object: 'network.interface.lanap',
	method: 'status',
	expect: { }
});

return baseclass.extend({
	title: _('Printer Info'),

	load: function() {
		return Promise.all([
			L.resolveDefault(callUsbInfo(), {}),
			L.resolveDefault(callWwanStatus(), {}),
			L.resolveDefault(callWanStatus(), {}),
			L.resolveDefault(callLanStatus(), {}),
			L.resolveDefault(callLanapStatus(), {})
		]);
	},

	render: function(data) {
		var usbinfo   = data[0];
		var wwan = data[1];
		var wan = data[2];
		var lan = data[3];
		var lanap = data[4];

		if (wwan['ipv4-address'] && wwan['ipv4-address'][0] && wwan['ipv4-address'][0]['address'])
			wwan = wwan['ipv4-address'][0]['address'];
		else
			wwan = "-";

		if (wan['ipv4-address'] && wan['ipv4-address'][0] && wan['ipv4-address'][0]['address'])
			wan = wan['ipv4-address'][0]['address'];
		else
			wan = "-";

		if (lan['ipv4-address'] && lan['ipv4-address'][0] && lan['ipv4-address'][0]['address'])
			lan = lan['ipv4-address'][0]['address'];
		else
			lan = "-";

		if (lanap['ipv4-address'] && lanap['ipv4-address'][0] && lanap['ipv4-address'][0]['address'])
			lanap = lanap['ipv4-address'][0]['address'];
		else
			lanap = "-";

		var fields = [
			_('USB Info'),         usbinfo.info ? usbinfo.info : "-",
			_('WiFi STA'),             wwan,
			_('LAN Port') + "(" + _('auto') + ")",              wan,
			_('LAN Port'),              lan,
			_('WiFi AP'),            lanap
		];

		var table = E('table', { 'class': 'table' });

		for (var i = 0; i < fields.length; i += 2) {
			table.appendChild(E('tr', { 'class': 'tr' }, [
				E('td', { 'class': 'td left', 'width': '33%' }, [ fields[i] ]),
				E('td', { 'class': 'td left' }, [ (fields[i + 1] != null) ? fields[i + 1] : '?' ])
			]));
		}

		return table;
	}
});
