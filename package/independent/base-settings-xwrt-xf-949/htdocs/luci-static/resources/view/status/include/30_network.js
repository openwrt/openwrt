'use strict';
'require baseclass';
'require fs';
'require network';

function progressbar(value, max, byte) {
	var vn = parseInt(value) || 0,
	    mn = parseInt(max) || 100,
	    fv = byte ? String.format('%1024.2mB', value) : value,
	    fm = byte ? String.format('%1024.2mB', max) : max,
	    pc = Math.floor((100 / mn) * vn);

	return E('div', {
		'class': 'cbi-progressbar',
		'title': '%s / %s (%d%%)'.format(fv, fm, pc)
	}, E('div', { 'style': 'width:%.2f%%'.format(pc) }));
}

function renderbox(ifc, ipv6) {
	var dev = ifc.getL3Device(),
	    active = (dev && ifc.getProtocol() != 'none'),
	    addrs = (ipv6 ? ifc.getIP6Addrs() : ifc.getIPAddrs()) || [],
	    dnssrv = (ipv6 ? ifc.getDNS6Addrs() : ifc.getDNSAddrs()) || [],
	    expires = (ipv6 ? null : ifc.getExpiry()),
	    uptime = ifc.getUptime();

	return E('div', { class: 'ifacebox' }, [
		E('div', { class: 'ifacebox-head center ' + (active ? 'active' : '') },
			E('strong', ifc.getName())),
		E('div', { class: 'ifacebox-body left' }, [
			L.itemlist(E('span'), [
				_('Protocol'), ifc.getI18n() || E('em', _('Not connected')),
				_('Prefix Delegated'), ipv6 ? ifc.getIP6Prefix() : null,
				_('Address'), addrs[0],
				_('Address'), addrs[1],
				_('Address'), addrs[2],
				_('Address'), addrs[3],
				_('Address'), addrs[4],
				_('Address'), addrs[5],
				_('Address'), addrs[6],
				_('Address'), addrs[7],
				_('Address'), addrs[8],
				_('Address'), addrs[9],
				_('Gateway'), ipv6 ? (ifc.getGateway6Addr() || '::') : (ifc.getGatewayAddr() || '0.0.0.0'),
				_('DNS') + ' 1', dnssrv[0],
				_('DNS') + ' 2', dnssrv[1],
				_('DNS') + ' 3', dnssrv[2],
				_('DNS') + ' 4', dnssrv[3],
				_('DNS') + ' 5', dnssrv[4],
				_('Expires'), (expires != null && expires > -1) ? '%t'.format(expires) : null,
				_('Connected'), (uptime > 0) ? '%t'.format(uptime) : null
			]),
			E('div', {}, renderBadge(
				L.resource('icons/%s.png').format(dev ? dev.getType() : 'ethernet_disabled'), null,
				_('Device'), dev ? dev.getI18n() : '-',
				_('MAC-Address'), dev.getMAC())
			)
		])
	]);
}

return baseclass.extend({
	title: _('Network'),

	load: function() {
		return Promise.all([
			network.getWANNetworks()
		]);
	},

	render: function(data) {
		var wan_nets  = data[0];

		var netstatus = E('div', { 'class': 'network-status-table' });

		for (var i = 0; i < wan_nets.length; i++)
			netstatus.appendChild(renderbox(wan_nets[i], false));

		return E([
			netstatus
		]);
	}
});
