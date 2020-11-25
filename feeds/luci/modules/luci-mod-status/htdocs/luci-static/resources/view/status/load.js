'use strict';
'require rpc';

var callLuciRealtimeStats = rpc.declare({
	object: 'luci',
	method: 'getRealtimeStats',
	params: [ 'mode', 'device' ],
	expect: { result: [] }
});

var graphPolls = [],
    pollInterval = 3;

Math.log2 = Math.log2 || function(x) { return Math.log(x) * Math.LOG2E; };

return L.view.extend({
	load: function() {
		return Promise.all([
			this.loadSVG(L.resource('load.svg'))
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

	pollData: function() {
		L.Poll.add(L.bind(function() {
			var tasks = [];

			for (var i = 0; i < graphPolls.length; i++) {
				var ctx = graphPolls[i];
				tasks.push(L.resolveDefault(callLuciRealtimeStats('load'), []));
			}

			return Promise.all(tasks).then(L.bind(function(datasets) {
				for (var gi = 0; gi < graphPolls.length; gi++) {
					var ctx = graphPolls[gi],
					    data = datasets[gi],
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
					E('div', { 'class': 'td right top' }, E('strong', { 'style': 'border-bottom:2px solid #f00' }, [ _('1 Minute Load:') ])),
					E('div', { 'class': 'td', 'id': 'lb_load01_cur' }, [ '0.00' ]),

					E('div', { 'class': 'td right top' }, E('strong', {}, [ _('Average:') ])),
					E('div', { 'class': 'td', 'id': 'lb_load01_avg' }, [ '0.00' ]),

					E('div', { 'class': 'td right top' }, E('strong', {}, [ _('Peak:') ])),
					E('div', { 'class': 'td', 'id': 'lb_load01_peak' }, [ '0.00' ])
				]),
				E('div', { 'class': 'tr' }, [
					E('div', { 'class': 'td right top' }, E('strong', { 'style': 'border-bottom:2px solid #f60' }, [ _('5 Minute Load:') ])),
					E('div', { 'class': 'td', 'id': 'lb_load05_cur' }, [ '0.00' ]),

					E('div', { 'class': 'td right top' }, E('strong', {}, [ _('Average:') ])),
					E('div', { 'class': 'td', 'id': 'lb_load05_avg' }, [ '0.00' ]),

					E('div', { 'class': 'td right top' }, E('strong', {}, [ _('Peak:') ])),
					E('div', { 'class': 'td', 'id': 'lb_load05_peak' }, [ '0.00' ])
				]),
				E('div', { 'class': 'tr' }, [
					E('div', { 'class': 'td right top' }, E('strong', { 'style': 'border-bottom:2px solid #fa0' }, [ _('15 Minute Load:') ])),
					E('div', { 'class': 'td', 'id': 'lb_load15_cur' }, [ '0.00' ]),

					E('div', { 'class': 'td right top' }, E('strong', {}, [ _('Average:') ])),
					E('div', { 'class': 'td', 'id': 'lb_load15_avg' }, [ '0.00' ]),

					E('div', { 'class': 'td right top' }, E('strong', {}, [ _('Peak:') ])),
					E('div', { 'class': 'td', 'id': 'lb_load15_peak' }, [ '0.00' ])
				])
			])
		]);

		this.updateGraph(svg, [ { line: 'load01' }, { line: 'load05' }, { line: 'load15' } ], function(svg, info) {
			var G = svg.firstElementChild, tab = svg.parentNode;

			G.getElementById('label_25').firstChild.data = '%.2f'.format(info.label_25 / 100);
			G.getElementById('label_50').firstChild.data = '%.2f'.format(info.label_50 / 100);
			G.getElementById('label_75').firstChild.data = '%.2f'.format(info.label_75 / 100);

			tab.querySelector('#scale').firstChild.data = _('(%d minute window, %d second interval)').format(info.timeframe, info.interval);

			tab.querySelector('#lb_load01_cur').firstChild.data = '%.2f'.format(info.line_current[0] / 100);
			tab.querySelector('#lb_load01_avg').firstChild.data = '%.2f'.format(info.line_average[0] / 100);
			tab.querySelector('#lb_load01_peak').firstChild.data = '%.2f'.format(info.line_peak[0] / 100);

			tab.querySelector('#lb_load05_cur').firstChild.data = '%.2f'.format(info.line_current[1] / 100);
			tab.querySelector('#lb_load05_avg').firstChild.data = '%.2f'.format(info.line_average[1] / 100);
			tab.querySelector('#lb_load05_peak').firstChild.data = '%.2f'.format(info.line_peak[1] / 100);

			tab.querySelector('#lb_load15_cur').firstChild.data = '%.2f'.format(info.line_current[2] / 100);
			tab.querySelector('#lb_load15_avg').firstChild.data = '%.2f'.format(info.line_average[2] / 100);
			tab.querySelector('#lb_load15_peak').firstChild.data = '%.2f'.format(info.line_peak[2] / 100);
		});

		this.pollData();

		return v;
	},

	handleSaveApply: null,
	handleSave: null,
	handleReset: null
});
