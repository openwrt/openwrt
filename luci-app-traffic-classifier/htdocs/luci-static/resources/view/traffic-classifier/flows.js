'use strict';
'require view';
'require rpc';
'require dom';
'require poll';

var callGetFlows = rpc.declare({
	object: 'traffic-classifier',
	method: 'get_flows',
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

function protoName(proto) {
	if (proto === 6) return 'TCP';
	if (proto === 17) return 'UDP';
	return String(proto);
}

function renderClassBadge(cls, confidence) {
	var color = classColors[cls] || '#757575';
	return E('span', {
		'style': 'display:inline-block;padding:2px 8px;border-radius:10px;' +
			'background:' + color + ';color:#fff;font-size:12px;font-weight:bold;' +
			'text-transform:capitalize;'
	}, cls + ' ' + (confidence || 0) + '%');
}

function renderFlowsTable(data) {
	var flows = (data && data.flows) ? data.flows : [];
	var totalFlows = data ? (data.total_flows || flows.length) : 0;

	if (flows.length === 0) {
		return E('div', { 'class': 'cbi-section' }, [
			E('h3', {}, 'Active Flows'),
			E('em', {}, 'No active flows. Traffic will appear here once clients are active.')
		]);
	}

	flows.sort(function(a, b) {
		return ((b.bytes_fwd || 0) + (b.bytes_bwd || 0)) -
		       ((a.bytes_fwd || 0) + (a.bytes_bwd || 0));
	});

	var headerRow = E('tr', { 'class': 'tr table-titles' }, [
		E('th', { 'class': 'th' }, 'Class'),
		E('th', { 'class': 'th' }, 'Source'),
		E('th', { 'class': 'th' }, 'Destination'),
		E('th', { 'class': 'th' }, 'Proto'),
		E('th', { 'class': 'th' }, 'Upload'),
		E('th', { 'class': 'th' }, 'Download'),
		E('th', { 'class': 'th' }, 'Pkts'),
		E('th', { 'class': 'th' }, 'Client')
	]);

	var rows = [headerRow];
	var limit = Math.min(flows.length, 100);

	for (var i = 0; i < limit; i++) {
		var f = flows[i];
		var src = (f.src_ip || '?') + ':' + (f.src_port || 0);
		var dst = (f.dst_ip || '?') + ':' + (f.dst_port || 0);
		var totalPkts = (f.pkts_fwd || 0) + (f.pkts_bwd || 0);
		var clientInfo = f.src_mac || '-';
		if (f.ssid)
			clientInfo += ' (' + f.ssid + ')';

		rows.push(E('tr', { 'class': 'tr' }, [
			E('td', { 'class': 'td' }, renderClassBadge(f.class || 'unknown', f.confidence)),
			E('td', { 'class': 'td', 'style': 'font-family:monospace;font-size:12px' }, src),
			E('td', { 'class': 'td', 'style': 'font-family:monospace;font-size:12px' }, dst),
			E('td', { 'class': 'td' }, protoName(f.proto)),
			E('td', { 'class': 'td' }, formatBytes(f.bytes_fwd || 0)),
			E('td', { 'class': 'td' }, formatBytes(f.bytes_bwd || 0)),
			E('td', { 'class': 'td' }, String(totalPkts)),
			E('td', { 'class': 'td', 'style': 'font-size:12px' }, clientInfo)
		]));
	}

	return E('div', { 'class': 'cbi-section' }, [
		E('h3', {}, 'Active Flows (' + totalFlows + ')' +
			(flows.length > limit ? ' — showing top ' + limit : '')),
		E('div', { 'style': 'overflow-x:auto' }, [
			E('table', { 'class': 'table cbi-section-table' }, rows)
		])
	]);
}

return view.extend({
	load: function() {
		return callGetFlows();
	},

	render: function(data) {
		var content = E('div', {}, [
			E('h2', {}, 'Flow Table'),
			renderFlowsTable(data)
		]);

		poll.add(L.bind(function() {
			return callGetFlows().then(L.bind(function(res) {
				var root = document.querySelector('[data-page="traffic-classifier-flows"]');
				if (!root) return;

				dom.content(root, [
					E('h2', {}, 'Flow Table'),
					renderFlowsTable(res)
				]);
			}, this));
		}, this), 5);

		return E('div', { 'data-page': 'traffic-classifier-flows' }, [content]);
	},

	handleSave: null,
	handleSaveApply: null,
	handleReset: null
});
