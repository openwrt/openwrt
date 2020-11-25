'use strict';
'require rpc';
'require form';

return L.view.extend({
	callHostHints: rpc.declare({
		object: 'luci-rpc',
		method: 'getHostHints',
		expect: { '': {} }
	}),

	load: function() {
		return this.callHostHints();
	},

	render: function(hosts) {
		var m, s, o;

		m = new form.Map('dhcp', _('Hostnames'));

		s = m.section(form.GridSection, 'domain', _('Host entries'));
		s.addremove = true;
		s.anonymous = true;
		s.sortable  = true;

		o = s.option(form.Value, 'name', _('Hostname'));
		o.datatype = 'hostname';
		o.rmempty = true;

		o = s.option(form.Value, 'ip', _('IP address'));
		o.datatype = 'ipaddr';
		o.rmempty = true;
		L.sortedKeys(hosts, 'ipv4', 'addr').forEach(function(mac) {
			o.value(hosts[mac].ipv4, '%s (%s)'.format(
				hosts[mac].ipv4,
				hosts[mac].name || mac
			));
		});

		return m.render();
	}
});
