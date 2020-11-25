'use strict';
'require rpc';

var callLuciRealtimeStats = rpc.declare({
	object: 'luci',
	method: 'getRealtimeStats',
	params: [ 'mode', 'device' ],
	expect: { result: [] }
});

var callLuciConntrackList = rpc.declare({
	object: 'luci',
	method: 'getConntrackList',
	expect: { result: [] }
});

var callNetworkRrdnsLookup = rpc.declare({
	object: 'network.rrdns',
	method: 'lookup',
	params: [ 'addrs', 'timeout', 'limit' ],
	expect: { '': {} }
});

var graphPolls = [],
    pollInterval = 3,
    dns_cache = {},
    enableLookups = false;

var recheck_lookup_queue = {};

Math.log2 = Math.log2 || function(x) { return Math.log(x) * Math.LOG2E; };

return L.view.extend({
	load: function() {
		return Promise.all([
			this.loadSVG(L.resource('connections.svg'))
		]);
	},

	updateGraph: function(svg, lines, cb) {
		var G = svg.firstElementChild;

		var view = document.querySelector('#view');

		var width  = view.offsetWidth - 2;
		var height = 300 - 2;
		var step   = 5;

		var data_wanted = Math.floor(width / step);

		var data_values = [],
		    line_elements = [];

		for (var i = 0; i < lines.length; i++)
			if (lines[i] != null)
				data_values.push([]);

		var info = {
			line_current: [],
			line_average: [],
			line_peak:    []
		};

		/* prefill datasets */
		for (var i = 0; i < data_values.length; i++)
			for (var j = 0; j < data_wanted; j++)
					data_values[i][j] = 0;

		/* plot horizontal time interval lines */
		for (var i = width % (step * 60); i < width; i += step * 60) {
			var line = document.createElementNS('http://www.w3.org/2000/svg', 'line');
				line.setAttribute('x1', i);
				line.setAttribute('y1', 0);
				line.setAttribute('x2', i);
				line.setAttribute('y2', '100%');
				line.setAttribute('style', 'stroke:black;stroke-width:0.1');

			var text = document.createElementNS('http://www.w3.org/2000/svg', 'text');
				text.setAttribute('x', i + 5);
				text.setAttribute('y', 15);
				text.setAttribute('style', 'fill:#eee; font-size:9pt; font-family:sans-serif; text-shadow:1px 1px 1px #000');
				text.appendChild(document.createTextNode(Math.round((width - i) / step / 60) + 'm'));

			G.appendChild(line);
			G.appendChild(text);
		}

		info.interval = pollInterval;
		info.timeframe = data_wanted / 60;

		graphPolls.push({
			svg:    svg,
			lines:  lines,
			cb:     cb,
			info:   info,
			width:  width,
			height: height,
			step:   step,
			values: data_values,
			timestamp: 0,
			fill: 1
		});
	},

	updateConntrack: function(conn) {
		var lookup_queue = [ ];
		var rows = [];

		conn.sort(function(a, b) {
			return b.bytes - a.bytes;
		});

		for (var i = 0; i < conn.length; i++)
		{
			var c  = conn[i];

			if ((c.src == '127.0.0.1' && c.dst == '127.0.0.1') ||
				(c.src == '::1'       && c.dst == '::1'))
				continue;

			if (!dns_cache[c.src] && lookup_queue.indexOf(c.src) == -1)
				lookup_queue.push(c.src);

			if (!dns_cache[c.dst] && lookup_queue.indexOf(c.dst) == -1)
				lookup_queue.push(c.dst);

			var src = dns_cache[c.src] || (c.layer3 == 'ipv6' ? '[' + c.src + ']' : c.src);
			var dst = dns_cache[c.dst] || (c.layer3 == 'ipv6' ? '[' + c.dst + ']' : c.dst);

			rows.push([
				c.layer3.toUpperCase(),
				c.layer4.toUpperCase(),
				c.hasOwnProperty('sport') ? (src + ':' + c.sport) : src,
				c.hasOwnProperty('dport') ? (dst + ':' + c.dport) : dst,
				'%1024.2mB (%d %s)'.format(c.bytes, c.packets, _('Pkts.'))
			]);
		}

		cbi_update_table('#connections', rows, E('em', _('No information available')));

		if (enableLookups && lookup_queue.length > 0) {
			var reduced_lookup_queue = lookup_queue;

			if (lookup_queue.length > 100)
				reduced_lookup_queue = lookup_queue.slice(0, 100);

			callNetworkRrdnsLookup(reduced_lookup_queue, 5000, 1000).then(function(replies) {
				for (var index in reduced_lookup_queue) {
					var address = reduced_lookup_queue[index];

					if (!address)
						continue;

					if (replies[address]) {
						dns_cache[address] = replies[address];
						lookup_queue.splice(reduced_lookup_queue.indexOf(address), 1);
						continue;
					}

					if (recheck_lookup_queue[address] > 2) {
						dns_cache[address] = (address.match(/:/)) ? '[' + address + ']' : address;
						lookup_queue.splice(index, 1);
					}
					else {
						recheck_lookup_queue[address] = (recheck_lookup_queue[address] || 0) + 1;
					}
				}

				var btn = document.querySelector('.btn.toggle-lookups');
				if (btn) {
					btn.firstChild.data = enableLookups ? _('Disable DNS lookups') : _('Enable DNS lookups');
					btn.classList.remove('spinning');
					btn.disabled = false;
				}
			});
		}
	},

	pollData: function() {
		L.Poll.add(L.bind(function() {
			var tasks = [
				L.resolveDefault(callLuciConntrackList(), [])
			];

			for (var i = 0; i < graphPolls.length; i++) {
				var ctx = graphPolls[i];
				tasks.push(L.resolveDefault(callLuciRealtimeStats('conntrack'), []));
			}

			return Promise.all(tasks).then(L.bind(function(datasets) {
				this.updateConntrack(datasets[0]);

				for (var gi = 0; gi < graphPolls.length; gi++) {
					var ctx = graphPolls[gi],
					    data = datasets[gi + 1],
					    values = ctx.values,
					    lines = ctx.lines,
					    info = ctx.info;

					var data_scale = 0;
					var data_wanted = Math.floor(ctx.width / ctx.step);
					var last_timestamp = NaN;

					for (var i = 0, di = 0; di < lines.length; di++) {
						if (lines[di] == null)
							continue;

						var multiply = (lines[di].multiply != null) ? lines[di].multiply : 1,
						    offset = (lines[di].offset != null) ? lines[di].offset : 0;

						for (var j = ctx.timestamp ? 0 : 1; j < data.length; j++) {
							/* skip overlapping entries */
							if (data[j][0] <= ctx.timestamp)
								continue;

							if (i == 0) {
								ctx.fill++;
								last_timestamp = data[j][0];
							}

							info.line_current[i] = data[j][di + 1] * multiply;
							info.line_current[i] -= Math.min(info.line_current[i], offset);
							values[i].push(info.line_current[i]);
						}

						i++;
					}

					/* cut off outdated entries */
					ctx.fill = Math.min(ctx.fill, data_wanted);

					for (var i = 0; i < values.length; i++) {
						var len = values[i].length;
						values[i] = values[i].slice(len - data_wanted, len);

						/* find peaks, averages */
						info.line_peak[i] = NaN;
						info.line_average[i] = 0;

						for (var j = 0; j < values[i].length; j++) {
							info.line_peak[i] = isNaN(info.line_peak[i]) ? values[i][j] : Math.max(info.line_peak[i], values[i][j]);
							info.line_average[i] += values[i][j];
						}

						info.line_average[i] = info.line_average[i] / ctx.fill;
					}

					info.peak = Math.max.apply(Math, info.line_peak);

					/* remember current timestamp, calculate horizontal scale */
					if (!isNaN(last_timestamp))
						ctx.timestamp = last_timestamp;

					var size = Math.floor(Math.log2(info.peak)),
					    div = Math.pow(2, size - (size % 10)),
					    mult = info.peak / div,
					    mult = (mult < 5) ? 2 : ((mult < 50) ? 10 : ((mult < 500) ? 100 : 1000));

					info.peak = info.peak + (mult * div) - (info.peak % (mult * div));

					data_scale = ctx.height / info.peak;

					/* plot data */
					for (var i = 0, di = 0; di < lines.length; di++) {
						if (lines[di] == null)
							continue;

						var el = ctx.svg.firstElementChild.getElementById(lines[di].line),
						    pt = '0,' + ctx.height,
						    y = 0;

						if (!el)
							continue;

						for (var j = 0; j < values[i].length; j++) {
							var x = j * ctx.step;

							y = ctx.height - Math.floor(values[i][j] * data_scale);
							//y -= Math.floor(y % (1 / data_scale));

							pt += ' ' + x + ',' + y;
						}

						pt += ' ' + ctx.width + ',' + y + ' ' + ctx.width + ',' + ctx.height;

						el.setAttribute('points', pt);

						i++;
					}

					info.label_25 = 0.25 * info.peak;
					info.label_50 = 0.50 * info.peak;
					info.label_75 = 0.75 * info.peak;

					if (typeof(ctx.cb) == 'function')
						ctx.cb(ctx.svg, info);
				}
			}, this));
		}, this), pollInterval);
	},

	loadSVG: function(src) {
		return L.Request.get(src).then(function(response) {
			if (!response.ok)
				throw new Error(response.statusText);

			return E('div', {
				'style': 'width:100%;height:300px;border:1px solid #000;background:#fff'
			}, E(response.text()));
		});
	},

	render: function(data) {
		var svg = data[0];

		var v = E([], [
			svg,
			E('div', { 'class': 'right' }, E('small', { 'id': 'scale' }, '-')),
			E('br'),

			E('div', { 'class': 'table', 'style': 'width:100%;table-layout:fixed' }, [
				E('div', { 'class': 'tr' }, [
					E('div', { 'class': 'td right top' }, E('strong', { 'style': 'border-bottom:2px solid blue' }, [ _('UDP:') ])),
					E('div', { 'class': 'td', 'id': 'lb_udp_cur' }, [ '0' ]),

					E('div', { 'class': 'td right top' }, E('strong', {}, [ _('Average:') ])),
					E('div', { 'class': 'td', 'id': 'lb_udp_avg' }, [ '0' ]),

					E('div', { 'class': 'td right top' }, E('strong', {}, [ _('Peak:') ])),
					E('div', { 'class': 'td', 'id': 'lb_udp_peak' }, [ '0' ])
				]),
				E('div', { 'class': 'tr' }, [
					E('div', { 'class': 'td right top' }, E('strong', { 'style': 'border-bottom:2px solid green' }, [ _('TCP:') ])),
					E('div', { 'class': 'td', 'id': 'lb_tcp_cur' }, [ '0' ]),

					E('div', { 'class': 'td right top' }, E('strong', {}, [ _('Average:') ])),
					E('div', { 'class': 'td', 'id': 'lb_tcp_avg' }, [ '0' ]),

					E('div', { 'class': 'td right top' }, E('strong', {}, [ _('Peak:') ])),
					E('div', { 'class': 'td', 'id': 'lb_tcp_peak' }, [ '0' ])
				]),
				E('div', { 'class': 'tr' }, [
					E('div', { 'class': 'td right top' }, E('strong', { 'style': 'border-bottom:2px solid red' }, [ _('Other:') ])),
					E('div', { 'class': 'td', 'id': 'lb_otr_cur' }, [ '0' ]),

					E('div', { 'class': 'td right top' }, E('strong', {}, [ _('Average:') ])),
					E('div', { 'class': 'td', 'id': 'lb_otr_avg' }, [ '0' ]),

					E('div', { 'class': 'td right top' }, E('strong', {}, [ _('Peak:') ])),
					E('div', { 'class': 'td', 'id': 'lb_otr_peak' }, [ '0' ])
				])
			]),

			E('div', { 'class': 'right' }, [
				E('button', {
					'class': 'btn toggle-lookups',
					'click': function(ev) {
						if (!enableLookups) {
							ev.currentTarget.classList.add('spinning');
							ev.currentTarget.disabled = true;
							enableLookups = true;
						}
						else {
							ev.currentTarget.firstChild.data = _('Enable DNS lookups');
							enableLookups = false;
						}

						this.blur();
					}
				}, [ enableLookups ? _('Disable DNS lookups') : _('Enable DNS lookups') ])
			]),

			E('br'),

			E('div', { 'class': 'cbi-section-node' }, [
				E('div', { 'class': 'table', 'id': 'connections' }, [
					E('div', { 'class': 'tr table-titles' }, [
						E('div', { 'class': 'th col-2 hide-xs' }, [ _('Network') ]),
						E('div', { 'class': 'th col-2' }, [ _('Protocol') ]),
						E('div', { 'class': 'th col-7' }, [ _('Source') ]),
						E('div', { 'class': 'th col-7' }, [ _('Destination') ]),
						E('div', { 'class': 'th col-4' }, [ _('Transfer') ])
					]),
					E('div', { 'class': 'tr placeholder' }, [
						E('div', { 'class': 'td' }, [
							E('em', {}, [ _('Collecting data...') ])
						])
					])
				])
			])
		]);

		this.updateGraph(svg, [ { line: 'udp' }, { line: 'tcp' }, { line: 'other' } ], function(svg, info) {
			var G = svg.firstElementChild, tab = svg.parentNode;

			G.getElementById('label_25').firstChild.data = '%d'.format(info.label_25);
			G.getElementById('label_50').firstChild.data = '%d'.format(info.label_50);
			G.getElementById('label_75').firstChild.data = '%d'.format(info.label_75);

			tab.querySelector('#scale').firstChild.data = _('(%d minute window, %d second interval)').format(info.timeframe, info.interval);

			tab.querySelector('#lb_udp_cur').firstChild.data = '%d'.format(info.line_current[0]);
			tab.querySelector('#lb_udp_avg').firstChild.data = '%d'.format(info.line_average[0]);
			tab.querySelector('#lb_udp_peak').firstChild.data = '%d'.format(info.line_peak[0]);

			tab.querySelector('#lb_tcp_cur').firstChild.data = '%d'.format(info.line_current[1]);
			tab.querySelector('#lb_tcp_avg').firstChild.data = '%d'.format(info.line_average[1]);
			tab.querySelector('#lb_tcp_peak').firstChild.data = '%d'.format(info.line_peak[1]);

			tab.querySelector('#lb_otr_cur').firstChild.data = '%d'.format(info.line_current[2]);
			tab.querySelector('#lb_otr_avg').firstChild.data = '%d'.format(info.line_average[2]);
			tab.querySelector('#lb_otr_peak').firstChild.data = '%d'.format(info.line_peak[2]);
		});

		this.pollData();

		return v;
	},

	handleSaveApply: null,
	handleSave: null,
	handleReset: null
});
