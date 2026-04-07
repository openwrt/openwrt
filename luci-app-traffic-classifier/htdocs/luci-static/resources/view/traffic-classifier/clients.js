'use strict';
'require view';
'require rpc';
'require dom';
'require poll';

var callGetClients = rpc.declare({
	object: 'traffic-classifier',
	method: 'get_clients',
	expect: {}
});

var classColors = {
	'unknown':  '#9e9e9e',
	'video':    '#e53935',
	'gaming':   '#8e24aa',
	'social':   '#1e88e5',
	'browsing': '#43a047',
	'download': '#fb8c00',
	'voip':     '#00acc1',
	'other':    '#6d4c41'
};

function formatBytes(bytes) {
	if (bytes === 0) return '0 B';
	var k = 1024;
	var sizes = ['B', 'KB', 'MB', 'GB', 'TB'];
	var i = Math.floor(Math.log(bytes) / Math.log(k));
	return parseFloat((bytes / Math.pow(k, i)).toFixed(2)) + ' ' + sizes[i];
}

function renderAppBar(appUsage) {
	var total = 0;
	var entries = [];

	for (var cls in appUsage) {
		if (appUsage.hasOwnProperty(cls)) {
			entries.push({ name: cls, count: appUsage[cls] });
			total += appUsage[cls];
		}
	}

	if (total === 0)
		return E('em', {}, 'No activity');

	entries.sort(function(a, b) { return b.count - a.count; });

	var segments = [];
	for (var i = 0; i < entries.length; i++) {
		var e = entries[i];
		var pct = (e.count / total * 100);
		var color = classColors[e.name] || '#757575';
		segments.push(E('div', {
			'style': 'background:' + color + ';height:100%;width:' + pct + '%;' +
				'display:inline-block;',
			'title': e.name + ': ' + e.count + ' flows (' + pct.toFixed(1) + '%)'
		}));
	}

	return E('div', {}, [
		E('div', {
			'style': 'display:flex;height:16px;border-radius:3px;overflow:hidden;' +
				'background:#e0e0e0;margin-bottom:4px'
		}, segments),
		E('div', { 'style': 'font-size:11px;color:#666' },
			entries.map(function(e) {
				return e.name + ':' + e.count;
			}).join('  ')
		)
	]);
}

function renderClientsTable(data) {
	var clients = (data && data.clients) ? data.clients : [];

	if (clients.length === 0) {
		return E('div', { 'class': 'cbi-section' }, [
			E('h3', {}, 'Connected Clients'),
			E('em', {}, 'No clients detected yet. The daemon needs a few seconds to start classifying.')
		]);
	}

	clients.sort(function(a, b) {
		return (b.total_bytes || 0) - (a.total_bytes || 0);
	});

	var headerRow = E('tr', { 'class': 'tr table-titles' }, [
		E('th', { 'class': 'th' }, 'MAC Address'),
		E('th', { 'class': 'th' }, 'SSID'),
		E('th', { 'class': 'th' }, 'Total Bytes'),
		E('th', { 'class': 'th' }, 'Flows'),
		E('th', { 'class': 'th', 'style': 'min-width:250px' }, 'Application Usage')
	]);

	var rows = [headerRow];

	for (var i = 0; i < clients.length; i++) {
		var c = clients[i];
		rows.push(E('tr', { 'class': 'tr' }, [
			E('td', { 'class': 'td', 'style': 'font-family:monospace' }, c.mac || '-'),
			E('td', { 'class': 'td' }, c.ssid || '-'),
			E('td', { 'class': 'td' }, formatBytes(c.total_bytes || 0)),
			E('td', { 'class': 'td' }, String(c.total_flows || 0)),
			E('td', { 'class': 'td' }, renderAppBar(c.app_usage || {}))
		]));
	}

	return E('div', { 'class': 'cbi-section' }, [
		E('h3', {}, 'Connected Clients (' + clients.length + ')'),
		E('table', { 'class': 'table cbi-section-table' }, rows)
	]);
}

function renderLegend() {
	var items = [];
	for (var cls in classColors) {
		if (classColors.hasOwnProperty(cls)) {
			items.push(E('span', {
				'style': 'display:inline-flex;align-items:center;margin-right:16px;margin-bottom:4px'
			}, [
				E('span', {
					'style': 'width:12px;height:12px;border-radius:2px;background:' +
						classColors[cls] + ';margin-right:4px;display:inline-block'
				}),
				E('span', { 'style': 'text-transform:capitalize;font-size:13px' }, cls)
			]));
		}
	}

	return E('div', {
		'style': 'display:flex;flex-wrap:wrap;padding:8px 0;margin-bottom:8px'
	}, items);
}

return view.extend({
	load: function() {
		return callGetClients();
	},

	render: function(data) {
		var content = E('div', {}, [
			E('h2', {}, 'Client Traffic'),
			renderLegend(),
			renderClientsTable(data)
		]);

		poll.add(L.bind(function() {
			return callGetClients().then(L.bind(function(res) {
				var root = document.querySelector('[data-page="traffic-classifier-clients"]');
				if (!root) return;

				dom.content(root, [
					E('h2', {}, 'Client Traffic'),
					renderLegend(),
					renderClientsTable(res)
				]);
			}, this));
		}, this), 5);

		return E('div', { 'data-page': 'traffic-classifier-clients' }, [content]);
	},

	handleSave: null,
	handleSaveApply: null,
	handleReset: null
});
