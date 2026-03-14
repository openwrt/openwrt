'use strict';
'require view';
'require rpc';
'require poll';
'require dom';

var callGetStatus = rpc.declare({
	object: 'luci.photonicat',
	method: 'get_status'
});

var callSetFanMode = rpc.declare({
	object: 'luci.photonicat',
	method: 'set_fan_mode',
	params: ['mode']
});

var callSetFanLevel = rpc.declare({
	object: 'luci.photonicat',
	method: 'set_fan_level',
	params: ['level']
});

var callSetFanCurve = rpc.declare({
	object: 'luci.photonicat',
	method: 'set_fan_curve',
	params: ['min_temp', 'max_temp', 'hysteresis']
});

var callSetCPUGovernor = rpc.declare({
	object: 'luci.photonicat',
	method: 'set_cpu_governor',
	params: ['governor']
});

/* ── helpers ─────────────────────────────────────────────── */

function tempColor(temp) {
	if (temp < 40) return '#4CAF50';
	if (temp < 55) return '#8BC34A';
	if (temp < 65) return '#FFC107';
	if (temp < 75) return '#FF9800';
	if (temp < 85) return '#FF5722';
	return '#F44336';
}

function bar(pct, color) {
	return E('div', {
		'style': 'background:#e0e0e0; border-radius:4px; overflow:hidden; height:18px;'
	}, [
		E('div', {
			'style': 'background:' + color + '; width:' + Math.max(0, Math.min(100, pct)) +
				'%; height:100%; border-radius:4px; transition:width 0.6s ease;'
		})
	]);
}

function tempBar(temp) {
	return bar(Math.min(100, temp), tempColor(temp));
}

function row(label, value) {
	return E('div', { 'class': 'tr' }, [
		E('div', { 'class': 'td', 'style': 'width:160px; font-weight:bold; padding:6px 8px;' }, label),
		E('div', { 'class': 'td', 'style': 'padding:6px 8px;' }, value)
	]);
}

function section(title, content) {
	return E('div', { 'class': 'cbi-section' }, [
		E('h3', {}, title),
		E('div', { 'class': 'cbi-section-node' }, [
			E('div', { 'class': 'table' }, content)
		])
	]);
}

/* ── view ────────────────────────────────────────────────── */

return view.extend({
	_status: {},
	_fanModeButtons: {},
	_fanLevelSlider: null,
	_fanLevelLabel: null,
	_govSelect: null,

	load: function() {
		return Promise.all([callGetStatus()]);
	},

	render: function(data) {
		this._status = data[0] || {};

		var statusDiv = E('div', { 'id': 'pcat-status' });
		var controlsDiv = E('div', { 'id': 'pcat-controls' });

		this.updateStatus(statusDiv);
		this.renderControls(controlsDiv);

		poll.add(L.bind(function() {
			return callGetStatus().then(L.bind(function(s) {
				this._status = s;
				var el = document.getElementById('pcat-status');
				if (el) this.updateStatus(el);
				this.syncControlState();
			}, this));
		}, this), 5);

		return E('div', { 'class': 'cbi-map' }, [
			E('h2', {}, _('Photonicat 2')),
			E('div', { 'class': 'cbi-map-descr' },
				_('System dashboard and hardware control for the Photonicat 2 SBC.')),
			statusDiv,
			controlsDiv
		]);
	},

	/* ── status display (rebuilt every poll) ──────────────── */

	updateStatus: function(container) {
		if (!container) return;
		var s = this._status;
		dom.content(container, [
			this.renderPower(s),
			this.renderTemps(s),
			this.renderFanStatus(s),
			this.renderCPUStatus(s)
		]);
	},

	renderPower: function(s) {
		var bat = s.battery || {};
		var chg = s.charger || {};

		var pct = (bat.capacity != null) ? bat.capacity : null;
		var batStatus = bat.status || 'Unknown';
		var volts = (bat.voltage != null) ? bat.voltage.toFixed(2) + ' V' : '--';
		var amps  = (bat.current != null) ? Math.abs(bat.current).toFixed(2) + ' A' : '--';
		var batColor = (pct > 50) ? '#4CAF50' : (pct > 20) ? '#FFC107' : '#F44336';
		var chgText = chg.online ? _('Connected') : _('Disconnected');

		var rows = [];

		if (pct != null) {
			rows.push(row(_('Battery'), [
				E('span', { 'style': 'font-size:1.2em; font-weight:bold;' }, pct + '%'),
				E('span', { 'style': 'margin-left:12px; color:#666;' }, batStatus)
			]));
			rows.push(E('div', { 'class': 'tr' }, [
				E('div', { 'class': 'td' }, ''),
				E('div', { 'class': 'td', 'style': 'padding:2px 8px 6px;' }, [bar(pct, batColor)])
			]));
			rows.push(row('', [
				E('span', {}, volts),
				E('span', { 'style': 'margin-left:20px;' }, amps)
			]));
		}

		rows.push(row(_('Charger'), chgText));

		return section(_('Power'), rows);
	},

	renderTemps: function(s) {
		var zones = s.thermal_zones || [];
		var board = s.board_temp;
		var rows = [];

		if (board != null) {
			rows.push(E('div', { 'class': 'tr' }, [
				E('div', { 'class': 'td', 'style': 'width:160px; padding:6px 8px;' }, _('Board (MCU)')),
				E('div', { 'class': 'td', 'style': 'width:70px; text-align:right; font-weight:bold; color:' +
					tempColor(board) + '; padding:6px 4px;' }, board.toFixed(0) + '\u00b0C'),
				E('div', { 'class': 'td', 'style': 'padding:6px 8px;' }, [tempBar(board)])
			]));
		}

		for (var i = 0; i < zones.length; i++) {
			var z = zones[i];
			rows.push(E('div', { 'class': 'tr' }, [
				E('div', { 'class': 'td', 'style': 'width:160px; padding:6px 8px;' }, z.type || ('Zone ' + i)),
				E('div', { 'class': 'td', 'style': 'width:70px; text-align:right; font-weight:bold; color:' +
					tempColor(z.temp) + '; padding:6px 4px;' }, z.temp.toFixed(0) + '\u00b0C'),
				E('div', { 'class': 'td', 'style': 'padding:6px 8px;' }, [tempBar(z.temp)])
			]));
		}

		return section(_('Temperatures'), rows);
	},

	renderFanStatus: function(s) {
		var rpm   = (s.fan_rpm != null) ? s.fan_rpm + ' RPM' : '--';
		var level = (s.fan_level != null) ? s.fan_level : '--';
		var max   = s.fan_max_level || 9;
		var cfg   = s.fan_config || {};

		return section(_('Fan'), [
			row(_('Speed'), rpm),
			row(_('Level'), level + ' / ' + max +
				'  (' + (cfg.mode || 'auto') + ' mode)')
		]);
	},

	renderCPUStatus: function(s) {
		var cpu = s.cpu || {};
		var labels = { 'policy0': 'Cortex-A55 (Little)', 'policy4': 'Cortex-A76 (Big)' };
		var rows = [];

		for (var policy in cpu) {
			var p = cpu[policy];
			rows.push(row(labels[policy] || policy, [
				E('span', { 'style': 'font-weight:bold;' },
					(p.cur_freq || '--') + ' MHz'),
				E('span', { 'style': 'margin-left:12px; color:#666;' },
					p.governor || '--')
			]));
		}

		return section(_('CPU'), rows);
	},

	/* ── controls (built once, kept persistent) ──────────── */

	renderControls: function(container) {
		var s = this._status;
		var cfg = s.fan_config || {};
		var cpu = s.cpu || {};

		/* Fan mode buttons */
		var modes = ['auto', 'manual', 'off'];
		var modeButtons = [];
		for (var i = 0; i < modes.length; i++) {
			var m = modes[i];
			var btn = E('button', {
				'class': 'cbi-button' + (cfg.mode === m ? ' cbi-button-positive' : ''),
				'data-mode': m,
				'click': L.bind(this.handleFanMode, this, m),
				'style': 'margin-right:6px;'
			}, m.charAt(0).toUpperCase() + m.slice(1));
			this._fanModeButtons[m] = btn;
			modeButtons.push(btn);
		}

		/* Fan level slider (manual mode) */
		this._fanLevelLabel = E('span', { 'style': 'font-weight:bold; min-width:2em; display:inline-block;' },
			String(cfg.manual_level || 3));

		this._fanLevelSlider = E('input', {
			'type': 'range', 'min': '0', 'max': '9',
			'value': String(cfg.manual_level || 3),
			'style': 'width:200px; vertical-align:middle;',
			'input': L.bind(function(ev) {
				this._fanLevelLabel.textContent = ev.target.value;
			}, this),
			'change': L.bind(this.handleFanLevel, this)
		});

		var levelRow = E('div', {
			'class': 'cbi-value',
			'id': 'fan-level-row',
			'style': cfg.mode === 'manual' ? '' : 'display:none;'
		}, [
			E('label', { 'class': 'cbi-value-title' }, _('Fan Level')),
			E('div', { 'class': 'cbi-value-field' }, [
				this._fanLevelSlider,
				E('span', { 'style': 'margin-left:10px;' }, [
					this._fanLevelLabel, E('span', {}, ' / 9')
				])
			])
		]);

		/* Fan curve inputs (auto mode) */
		var minInput  = E('input', { 'type': 'number', 'class': 'cbi-input-text',
			'id': 'fc-min', 'value': String(cfg.min_temp || 45),
			'style': 'width:60px;', 'min': '20', 'max': '80' });
		var maxInput  = E('input', { 'type': 'number', 'class': 'cbi-input-text',
			'id': 'fc-max', 'value': String(cfg.max_temp || 85),
			'style': 'width:60px;', 'min': '50', 'max': '110' });
		var hystInput = E('input', { 'type': 'number', 'class': 'cbi-input-text',
			'id': 'fc-hyst', 'value': String(cfg.hysteresis || 3),
			'style': 'width:60px;', 'min': '1', 'max': '15' });

		var curveRow = E('div', {
			'class': 'cbi-value',
			'id': 'fan-curve-row',
			'style': cfg.mode === 'auto' ? '' : 'display:none;'
		}, [
			E('label', { 'class': 'cbi-value-title' }, _('Fan Curve')),
			E('div', { 'class': 'cbi-value-field', 'style': 'display:flex; align-items:center; flex-wrap:wrap; gap:6px;' }, [
				E('span', {}, _('Start')), minInput, E('span', {}, '\u00b0C'),
				E('span', { 'style': 'margin-left:8px;' }, _('Max')), maxInput, E('span', {}, '\u00b0C'),
				E('span', { 'style': 'margin-left:8px;' }, _('Hyst')), hystInput, E('span', {}, '\u00b0C'),
				E('button', {
					'class': 'cbi-button cbi-button-apply',
					'style': 'margin-left:12px;',
					'click': L.bind(this.handleFanCurve, this)
				}, _('Apply'))
			])
		]);

		/* CPU governor selector */
		var govs = [];
		var currentGov = '';
		for (var p in cpu) {
			if (cpu[p].governors && cpu[p].governors.length)
				govs = cpu[p].governors;
			if (cpu[p].governor)
				currentGov = cpu[p].governor;
		}

		this._govSelect = E('select', {
			'class': 'cbi-input-select',
			'change': L.bind(this.handleGovernor, this)
		});
		for (var j = 0; j < govs.length; j++) {
			var opt = E('option', { 'value': govs[j] }, govs[j]);
			if (govs[j] === currentGov) opt.selected = true;
			this._govSelect.appendChild(opt);
		}

		dom.content(container, [
			E('div', { 'class': 'cbi-section' }, [
				E('h3', {}, _('Fan Control')),
				E('div', { 'class': 'cbi-section-node' }, [
					E('div', { 'class': 'cbi-value' }, [
						E('label', { 'class': 'cbi-value-title' }, _('Mode')),
						E('div', { 'class': 'cbi-value-field' }, modeButtons)
					]),
					levelRow,
					curveRow
				])
			]),
			E('div', { 'class': 'cbi-section' }, [
				E('h3', {}, _('CPU Governor')),
				E('div', { 'class': 'cbi-section-node' }, [
					E('div', { 'class': 'cbi-value' }, [
						E('label', { 'class': 'cbi-value-title' }, _('Governor')),
						E('div', { 'class': 'cbi-value-field' }, [
							this._govSelect,
							E('span', { 'style': 'margin-left:12px; color:#666;' },
								_('Applies to all CPU clusters'))
						])
					])
				])
			])
		]);
	},

	/* ── sync control visual state after poll ────────────── */

	syncControlState: function() {
		var mode = (this._status.fan_config || {}).mode || 'auto';

		for (var m in this._fanModeButtons) {
			this._fanModeButtons[m].className =
				'cbi-button' + (m === mode ? ' cbi-button-positive' : '');
		}

		var lr = document.getElementById('fan-level-row');
		if (lr) lr.style.display = (mode === 'manual') ? '' : 'none';

		var cr = document.getElementById('fan-curve-row');
		if (cr) cr.style.display = (mode === 'auto') ? '' : 'none';
	},

	/* ── event handlers ──────────────────────────────────── */

	handleFanMode: function(mode) {
		callSetFanMode(mode).then(L.bind(function() {
			this._status.fan_config = this._status.fan_config || {};
			this._status.fan_config.mode = mode;
			this.syncControlState();
		}, this));
	},

	handleFanLevel: function(ev) {
		var level = parseInt(ev.target.value);
		callSetFanLevel(level);
	},

	handleFanCurve: function() {
		var min  = parseInt(document.getElementById('fc-min').value)  || 45;
		var max  = parseInt(document.getElementById('fc-max').value)  || 85;
		var hyst = parseInt(document.getElementById('fc-hyst').value) || 3;
		callSetFanCurve(min, max, hyst);
	},

	handleGovernor: function(ev) {
		callSetCPUGovernor(ev.target.value);
	},

	handleSave: null,
	handleSaveApply: null,
	handleReset: null
});
