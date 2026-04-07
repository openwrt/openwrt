'use strict';
'require view';
'require rpc';
'require dom';
'require poll';

var callStatus = rpc.declare({
	object: 'traffic-classifier',
	method: 'status',
	expect: {}
});

var callGetStats = rpc.declare({
	object: 'traffic-classifier',
	method: 'get_stats',
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

function renderStatusCard(status) {
	var table = E('table', { 'class': 'table' }, [
		E('tr', { 'class': 'tr' }, [
			E('td', { 'class': 'td', 'style': 'width:50%' }, E('strong', {}, 'Version')),
			E('td', { 'class': 'td' }, status.version || '-')
		]),
		E('tr', { 'class': 'tr' }, [
			E('td', { 'class': 'td' }, E('strong', {}, 'Status')),
			E('td', { 'class': 'td' }, [
				E('span', {
					'style': 'color:' + (status.status === 'running' ? '#43a047' : '#e53935') +
						';font-weight:bold'
				}, (status.status || 'unknown').toUpperCase())
			])
		]),
		E('tr', { 'class': 'tr' }, [
			E('td', { 'class': 'td' }, E('strong', {}, 'Active Flows')),
			E('td', { 'class': 'td' }, String(status.active_flows || 0))
		]),
		E('tr', { 'class': 'tr' }, [
			E('td', { 'class': 'td' }, E('strong', {}, 'Max Flows')),
			E('td', { 'class': 'td' }, String(status.max_flows || 0))
		]),
		E('tr', { 'class': 'tr' }, [
			E('td', { 'class': 'td' }, E('strong', {}, 'Tracked Stations')),
			E('td', { 'class': 'td' }, String(status.tracked_stations || 0))
		])
	]);

	return E('div', { 'class': 'cbi-section' }, [
		E('h3', {}, 'Daemon Status'),
		table
	]);
}

function renderClassificationBars(stats) {
	var classification = stats.classification || {};
	var totalFlows = 0;
	var entries = [];

	for (var cls in classification) {
		if (classification.hasOwnProperty(cls) && classification[cls] > 0) {
			entries.push({ name: cls, count: classification[cls] });
			totalFlows += classification[cls];
		}
	}

	entries.sort(function(a, b) { return b.count - a.count; });

	var rows = [];
	for (var i = 0; i < entries.length; i++) {
		var e = entries[i];
		var pct = totalFlows > 0 ? (e.count / totalFlows * 100) : 0;
		var color = classColors[e.name] || '#757575';

		rows.push(E('div', { 'style': 'margin-bottom:12px' }, [
			E('div', { 'style': 'display:flex;justify-content:space-between;margin-bottom:4px' }, [
				E('span', { 'style': 'font-weight:bold;text-transform:capitalize' }, e.name),
				E('span', {}, e.count + ' flows (' + pct.toFixed(1) + '%)')
			]),
			E('div', {
				'style': 'background:#e0e0e0;border-radius:4px;height:24px;overflow:hidden'
			}, [
				E('div', {
					'style': 'background:' + color +
						';height:100%;width:' + pct + '%;border-radius:4px;' +
						'transition:width 0.5s ease'
				})
			])
		]));
	}

	return E('div', { 'class': 'cbi-section' }, [
		E('h3', {}, 'Traffic Classification'),
		E('div', { 'style': 'padding:8px' }, [
			E('div', { 'style': 'display:flex;justify-content:space-between;margin-bottom:16px' }, [
				E('span', {}, [
					E('strong', {}, 'Total Flows: '),
					String(totalFlows)
				]),
				E('span', {}, [
					E('strong', {}, 'Total Bytes: '),
					formatBytes(stats.total_bytes || 0)
				]),
				E('span', {}, [
					E('strong', {}, 'Stations: '),
					String(stats.tracked_stations || 0)
				])
			]),
			E('div', {}, rows)
		])
	]);
}

function renderDonutChart(stats) {
	var classification = stats.classification || {};
	var entries = [];
	var total = 0;

	for (var cls in classification) {
		if (classification.hasOwnProperty(cls) && classification[cls] > 0) {
			entries.push({ name: cls, count: classification[cls] });
			total += classification[cls];
		}
	}

	if (total === 0) {
		return E('div', { 'class': 'cbi-section' }, [
			E('h3', {}, 'Distribution'),
			E('em', {}, 'No classified flows yet.')
		]);
	}

	entries.sort(function(a, b) { return b.count - a.count; });

	var size = 200;
	var cx = size / 2, cy = size / 2, r = 80, innerR = 50;
	var paths = [];
	var angle = -Math.PI / 2;

	for (var i = 0; i < entries.length; i++) {
		var e = entries[i];
		var sliceAngle = (e.count / total) * 2 * Math.PI;
		var endAngle = angle + sliceAngle;
		var largeArc = sliceAngle > Math.PI ? 1 : 0;
		var color = classColors[e.name] || '#757575';

		var x1 = cx + r * Math.cos(angle);
		var y1 = cy + r * Math.sin(angle);
		var x2 = cx + r * Math.cos(endAngle);
		var y2 = cy + r * Math.sin(endAngle);
		var ix1 = cx + innerR * Math.cos(endAngle);
		var iy1 = cy + innerR * Math.sin(endAngle);
		var ix2 = cx + innerR * Math.cos(angle);
		var iy2 = cy + innerR * Math.sin(angle);

		var d = 'M ' + x1 + ' ' + y1 +
			' A ' + r + ' ' + r + ' 0 ' + largeArc + ' 1 ' + x2 + ' ' + y2 +
			' L ' + ix1 + ' ' + iy1 +
			' A ' + innerR + ' ' + innerR + ' 0 ' + largeArc + ' 0 ' + ix2 + ' ' + iy2 +
			' Z';

		paths.push('<path d="' + d + '" fill="' + color + '"/>');
		angle = endAngle;
	}

	var svg = '<svg width="' + size + '" height="' + size + '" viewBox="0 0 ' + size + ' ' + size + '">' +
		paths.join('') +
		'<text x="' + cx + '" y="' + (cy - 5) + '" text-anchor="middle" font-size="20" font-weight="bold">' + total + '</text>' +
		'<text x="' + cx + '" y="' + (cy + 15) + '" text-anchor="middle" font-size="12" fill="#666">flows</text>' +
		'</svg>';

	var legendItems = [];
	for (var j = 0; j < entries.length; j++) {
		var le = entries[j];
		var pct = (le.count / total * 100).toFixed(1);
		legendItems.push(
			'<div style="display:flex;align-items:center;margin-bottom:4px">' +
			'<span style="width:12px;height:12px;border-radius:2px;background:' +
			(classColors[le.name] || '#757575') + ';margin-right:8px;display:inline-block"></span>' +
			'<span style="text-transform:capitalize">' + le.name + ' — ' + le.count + ' (' + pct + '%)</span>' +
			'</div>'
		);
	}

	var container = E('div', { 'class': 'cbi-section' }, [
		E('h3', {}, 'Distribution')
	]);

	var inner = E('div', {
		'style': 'display:flex;align-items:center;gap:32px;flex-wrap:wrap;padding:8px'
	});
	inner.innerHTML = svg + '<div>' + legendItems.join('') + '</div>';
	container.appendChild(inner);

	return container;
}

return view.extend({
	load: function() {
		return Promise.all([
			callStatus(),
			callGetStats()
		]);
	},

	render: function(data) {
		var status = data[0] || {};
		var stats = data[1] || {};

		var view = E('div', {}, [
			E('h2', {}, 'Traffic Classifier'),
			E('div', { 'style': 'display:flex;gap:16px;flex-wrap:wrap' }, [
				E('div', { 'style': 'flex:1;min-width:300px' }, [
					renderStatusCard(status)
				]),
				E('div', { 'style': 'flex:1;min-width:300px' }, [
					renderDonutChart(stats)
				])
			]),
			renderClassificationBars(stats)
		]);

		poll.add(L.bind(function() {
			return Promise.all([callStatus(), callGetStats()]).then(L.bind(function(res) {
				var s = res[0] || {};
				var st = res[1] || {};
				var root = document.querySelector('[data-page="traffic-classifier-overview"]');
				if (!root) return;

				dom.content(root, [
					E('h2', {}, 'Traffic Classifier'),
					E('div', { 'style': 'display:flex;gap:16px;flex-wrap:wrap' }, [
						E('div', { 'style': 'flex:1;min-width:300px' }, [
							renderStatusCard(s)
						]),
						E('div', { 'style': 'flex:1;min-width:300px' }, [
							renderDonutChart(st)
						])
					]),
					renderClassificationBars(st)
				]);
			}, this));
		}, this), 5);

		return E('div', { 'data-page': 'traffic-classifier-overview' }, [view]);
	},

	handleSave: null,
	handleSaveApply: null,
	handleReset: null
});
