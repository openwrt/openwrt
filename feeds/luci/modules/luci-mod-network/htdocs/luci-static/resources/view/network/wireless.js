'use strict';
'require fs';
'require ui';
'require rpc';
'require uci';
'require form';
'require network';
'require firewall';
'require tools.widgets as widgets';

function count_changes(section_id) {
	var changes = ui.changes.changes, n = 0;

	if (!L.isObject(changes))
		return n;

	if (Array.isArray(changes.wireless))
		for (var i = 0; i < changes.wireless.length; i++)
			n += (changes.wireless[i][1] == section_id);

	return n;
}

function render_radio_badge(radioDev) {
	return E('span', { 'class': 'ifacebadge' }, [
		E('img', { 'src': L.resource('icons/wifi%s.png').format(radioDev.isUp() ? '' : '_disabled') }),
		' ',
		radioDev.getName()
	]);
}

function render_signal_badge(signalPercent, signalValue, noiseValue, wrap) {
	var icon, title, value;

	if (signalPercent < 0)
		icon = L.resource('icons/signal-none.png');
	else if (signalPercent == 0)
		icon = L.resource('icons/signal-0.png');
	else if (signalPercent < 25)
		icon = L.resource('icons/signal-0-25.png');
	else if (signalPercent < 50)
		icon = L.resource('icons/signal-25-50.png');
	else if (signalPercent < 75)
		icon = L.resource('icons/signal-50-75.png');
	else
		icon = L.resource('icons/signal-75-100.png');

	if (signalValue != null && signalValue != 0 && noiseValue != null && noiseValue != 0) {
		value = '%d / %d %s'.format(signalValue, noiseValue, _('dBm'));
		title = '%s: %d %s / %s: %d %s / %s %d'.format(
			_('Signal'), signalValue, _('dBm'),
			_('Noise'), noiseValue, _('dBm'),
			_('SNR'), signalValue - noiseValue);
	}
	else if (signalValue != null && signalValue != 0) {
		value = '%d %s'.format(signalValue, _('dBm'));
		title = '%s: %d %s'.format(_('Signal'), signalValue, _('dBm'));
	}
	else {
		value = E('em', {}, E('small', {}, [ _('disabled') ]));
		title = _('No signal');
	}

	return E('div', { 'class': wrap ? 'center' : 'ifacebadge', 'title': title },
		[ E('img', { 'src': icon }), wrap ? E('br') : ' ', value ]);
}

function render_network_badge(radioNet) {
	return render_signal_badge(radioNet.isUp() ? radioNet.getSignalPercent() : -1,  radioNet.getSignal(), radioNet.getNoise());
}

function render_radio_status(radioDev, wifiNets) {
	var name = radioDev.getI18n().replace(/ Wireless Controller .+$/, ''),
	    node = E('div', [ E('big', {}, E('strong', {}, name)), E('div') ]),
	    channel, frequency, bitrate;

	for (var i = 0; i < wifiNets.length; i++) {
		channel   = channel   || wifiNets[i].getChannel();
		frequency = frequency || wifiNets[i].getFrequency();
		bitrate   = bitrate   || wifiNets[i].getBitRate();
	}

	if (radioDev.isUp())
		L.itemlist(node.lastElementChild, [
			_('Channel'), '%s (%s %s)'.format(channel || '?', frequency || '?', _('GHz')),
			_('Bitrate'), '%s %s'.format(bitrate || '?', _('Mbit/s'))
		], ' | ');
	else
		node.lastElementChild.appendChild(E('em', _('Device is not active')));

	return node;
}

function render_network_status(radioNet) {
	var mode = radioNet.getActiveMode(),
	    bssid = radioNet.getActiveBSSID(),
	    channel = radioNet.getChannel(),
	    disabled = (radioNet.get('disabled') == '1' || uci.get('wireless', radioNet.getWifiDeviceName(), 'disabled') == '1'),
	    is_assoc = (bssid && bssid != '00:00:00:00:00:00' && channel && mode != 'Unknown' && !disabled),
	    is_mesh = (radioNet.getMode() == 'mesh'),
	    changecount = count_changes(radioNet.getName()),
	    status_text = null;

	if (changecount)
		status_text = E('a', {
			href: '#',
			click: L.bind(ui.changes.displayChanges, ui.changes)
		}, _('Interface has %d pending changes').format(changecount));
	else if (!is_assoc)
		status_text = E('em', disabled ? _('Wireless is disabled') : _('Wireless is not associated'));

	return L.itemlist(E('div'), [
		is_mesh ? _('Mesh ID') : _('SSID'), (is_mesh ? radioNet.getMeshID() : radioNet.getSSID()) || '?',
		_('Mode'),       mode,
		_('BSSID'),      (!changecount && is_assoc) ? bssid : null,
		_('Encryption'), (!changecount && is_assoc) ? radioNet.getActiveEncryption() || _('None') : null,
		null,            status_text
	], [ ' | ', E('br') ]);
}

function render_modal_status(node, radioNet) {
	var mode = radioNet.getActiveMode(),
	    noise = radioNet.getNoise(),
	    bssid = radioNet.getActiveBSSID(),
	    channel = radioNet.getChannel(),
	    disabled = (radioNet.get('disabled') == '1'),
	    is_assoc = (bssid && bssid != '00:00:00:00:00:00' && channel && mode != 'Unknown' && !disabled);

	if (node == null)
		node = E('span', { 'class': 'ifacebadge large', 'data-network': radioNet.getName() }, [ E('small'), E('span') ]);

	L.dom.content(node.firstElementChild, render_signal_badge(disabled ? -1 : radioNet.getSignalPercent(), radioNet.getSignal(), noise, true));

	L.itemlist(node.lastElementChild, [
		_('Mode'),       mode,
		_('SSID'),       radioNet.getSSID() || '?',
		_('BSSID'),      is_assoc ? bssid : null,
		_('Encryption'), is_assoc ? radioNet.getActiveEncryption() || _('None') : null,
		_('Channel'),    is_assoc ? '%d (%.3f %s)'.format(radioNet.getChannel(), radioNet.getFrequency() || 0, _('GHz')) : null,
		_('Tx-Power'),   is_assoc ? '%d %s'.format(radioNet.getTXPower(), _('dBm')) : null,
		_('Signal'),     is_assoc ? '%d %s'.format(radioNet.getSignal(), _('dBm')) : null,
		_('Noise'),      (is_assoc && noise != null) ? '%d %s'.format(noise, _('dBm')) : null,
		_('Bitrate'),    is_assoc ? '%.1f %s'.format(radioNet.getBitRate() || 0, _('Mbit/s')) : null,
		_('Country'),    is_assoc ? radioNet.getCountryCode() : null
	], [ ' | ', E('br'), E('br'), E('br'), E('br'), E('br'), ' | ', E('br'), ' | ' ]);

	if (!is_assoc)
		L.dom.append(node.lastElementChild, E('em', disabled ? _('Wireless is disabled') : _('Wireless is not associated')));

	return node;
}

function format_wifirate(rate) {
	var s = '%.1f Mbit/s, %dMHz'.format(rate.rate / 1000, rate.mhz);

	if (rate.ht || rate.vht) {
		if (rate.vht)      s += ', VHT-MCS %d'.format(rate.mcs);
		if (rate.nss)      s += ', VHT-NSS %d'.format(rate.nss);
		if (rate.ht)       s += ', MCS %s'.format(rate.mcs);
		if (rate.short_gi) s += ', Short GI';
	}

	return s;
}

function radio_restart(id, ev) {
	var row = document.querySelector('.cbi-section-table-row[data-sid="%s"]'.format(id)),
	    dsc = row.querySelector('[data-name="_stat"] > div'),
	    btn = row.querySelector('.cbi-section-actions button');

	btn.blur();
	btn.classList.add('spinning');
	btn.disabled = true;

	dsc.setAttribute('restart', '');
	L.dom.content(dsc, E('em', _('Device is restarting…')));
}

function network_updown(id, map, ev) {
	var radio = uci.get('wireless', id, 'device'),
	    disabled = (uci.get('wireless', id, 'disabled') == '1') ||
	               (uci.get('wireless', radio, 'disabled') == '1');

	if (disabled) {
		uci.unset('wireless', id, 'disabled');
		uci.unset('wireless', radio, 'disabled');
	}
	else {
		uci.set('wireless', id, 'disabled', '1');

		var all_networks_disabled = true,
		    wifi_ifaces = uci.sections('wireless', 'wifi-iface');

		for (var i = 0; i < wifi_ifaces.length; i++) {
			if (wifi_ifaces[i].device == radio && wifi_ifaces[i].disabled != '1') {
				all_networks_disabled = false;
				break;
			}
		}

		if (all_networks_disabled)
			uci.set('wireless', radio, 'disabled', '1');
	}

	return map.save().then(function() {
		ui.changes.apply()
	});
}

function next_free_sid(offset) {
	var sid = 'wifinet' + offset;

	while (uci.get('wireless', sid))
		sid = 'wifinet' + (++offset);

	return sid;
}

var CBIWifiFrequencyValue = form.Value.extend({
	callFrequencyList: rpc.declare({
		object: 'iwinfo',
		method: 'freqlist',
		params: [ 'device' ],
		expect: { results: [] }
	}),

	load: function(section_id) {
		return Promise.all([
			network.getWifiDevice(section_id),
			this.callFrequencyList(section_id)
		]).then(L.bind(function(data) {
			this.channels = {
				'11g': L.hasSystemFeature('hostapd', 'acs') ? [ 'auto', 'auto', true ] : [],
				'11a': L.hasSystemFeature('hostapd', 'acs') ? [ 'auto', 'auto', true ] : []
			};

			for (var i = 0; i < data[1].length; i++)
				this.channels[(data[1][i].mhz > 2484) ? '11a' : '11g'].push(
					data[1][i].channel,
					'%d (%d Mhz)'.format(data[1][i].channel, data[1][i].mhz),
					!data[1][i].restricted
				);

			var hwmodelist = L.toArray(data[0] ? data[0].getHWModes() : null)
				.reduce(function(o, v) { o[v] = true; return o }, {});

			this.modes = [
				'', 'Legacy', true,
				'n', 'N', hwmodelist.n,
				'ac', 'AC', hwmodelist.ac
			];

			var htmodelist = L.toArray(data[0] ? data[0].getHTModes() : null)
				.reduce(function(o, v) { o[v] = true; return o }, {});

			this.htmodes = {
				'': [ '', '-', true ],
				'n': [
					'HT20', '20 MHz', htmodelist.HT20,
					'HT40', '40 MHz', htmodelist.HT40
				],
				'ac': [
					'VHT20', '20 MHz', htmodelist.VHT20,
					'VHT40', '40 MHz', htmodelist.VHT40,
					'VHT80', '80 MHz', htmodelist.VHT80,
					'VHT160', '160 MHz', htmodelist.VHT160
				]
			};

			this.bands = {
				'': [
					'11g', '2.4 GHz', this.channels['11g'].length > 3,
					'11a', '5 GHz', this.channels['11a'].length > 3
				],
				'n': [
					'11g', '2.4 GHz', this.channels['11g'].length > 3,
					'11a', '5 GHz', this.channels['11a'].length > 3
				],
				'ac': [
					'11a', '5 GHz', true
				]
			};
		}, this));
	},

	setValues: function(sel, vals) {
		if (sel.vals)
			sel.vals.selected = sel.selectedIndex;

		while (sel.options[0])
			sel.remove(0);

		for (var i = 0; vals && i < vals.length; i += 3)
			if (vals[i+2])
				sel.add(E('option', { value: vals[i+0] }, [ vals[i+1] ]));

		if (vals && !isNaN(vals.selected))
			sel.selectedIndex = vals.selected;

		sel.parentNode.style.display = (sel.options.length <= 1) ? 'none' : '';
		sel.vals = vals;
	},

	toggleWifiMode: function(elem) {
		this.toggleWifiHTMode(elem);
		this.toggleWifiBand(elem);
	},

	toggleWifiHTMode: function(elem) {
		var mode = elem.querySelector('.mode');
		var bwdt = elem.querySelector('.htmode');

		this.setValues(bwdt, this.htmodes[mode.value]);
	},

	toggleWifiBand: function(elem) {
		var mode = elem.querySelector('.mode');
		var band = elem.querySelector('.band');

		this.setValues(band, this.bands[mode.value]);
		this.toggleWifiChannel(elem);
	},

	toggleWifiChannel: function(elem) {
		var band = elem.querySelector('.band');
		var chan = elem.querySelector('.channel');

		this.setValues(chan, this.channels[band.value]);
	},

	setInitialValues: function(section_id, elem) {
		var mode = elem.querySelector('.mode'),
		    band = elem.querySelector('.band'),
		    chan = elem.querySelector('.channel'),
		    bwdt = elem.querySelector('.htmode'),
		    htval = uci.get('wireless', section_id, 'htmode'),
		    hwval = uci.get('wireless', section_id, 'hwmode'),
		    chval = uci.get('wireless', section_id, 'channel');

		this.setValues(mode, this.modes);

		if (/VHT20|VHT40|VHT80|VHT160/.test(htval))
			mode.value = 'ac';
		else if (/HT20|HT40/.test(htval))
			mode.value = 'n';
		else
			mode.value = '';

		this.toggleWifiMode(elem);

		if (/a/.test(hwval))
			band.value = '11a';
		else
			band.value = '11g';

		this.toggleWifiBand(elem);

		bwdt.value = htval;
		chan.value = chval;

		return elem;
	},

	renderWidget: function(section_id, option_index, cfgvalue) {
		var elem = E('div');

		L.dom.content(elem, [
			E('label', { 'style': 'float:left; margin-right:3px' }, [
				_('Mode'), E('br'),
				E('select', {
					'class': 'mode',
					'style': 'width:auto',
					'change': L.bind(this.toggleWifiMode, this, elem)
				})
			]),
			E('label', { 'style': 'float:left; margin-right:3px' }, [
				_('Band'), E('br'),
				E('select', {
					'class': 'band',
					'style': 'width:auto',
					'change': L.bind(this.toggleWifiBand, this, elem)
				})
			]),
			E('label', { 'style': 'float:left; margin-right:3px' }, [
				_('Channel'), E('br'),
				E('select', {
					'class': 'channel',
					'style': 'width:auto'
				})
			]),
			E('label', { 'style': 'float:left; margin-right:3px' }, [
				_('Width'), E('br'),
				E('select', {
					'class': 'htmode',
					'style': 'width:auto'
				})
			]),
			E('br', { 'style': 'clear:left' })
		]);

		return this.setInitialValues(section_id, elem);
	},

	cfgvalue: function(section_id) {
		return [
		    uci.get('wireless', section_id, 'htmode'),
		    uci.get('wireless', section_id, 'hwmode'),
		    uci.get('wireless', section_id, 'channel')
		];
	},

	formvalue: function(section_id) {
		var node = this.map.findElement('data-field', this.cbid(section_id));

		return [
		    node.querySelector('.htmode').value,
		    node.querySelector('.band').value,
 			node.querySelector('.channel').value
		];
	},

	write: function(section_id, value) {
		uci.set('wireless', section_id, 'htmode', value[0] || null);
		uci.set('wireless', section_id, 'hwmode', value[1]);
		uci.set('wireless', section_id, 'channel', value[2]);
	}
});

var CBIWifiTxPowerValue = form.ListValue.extend({
	callTxPowerList: rpc.declare({
		object: 'iwinfo',
		method: 'txpowerlist',
		params: [ 'device' ],
		expect: { results: [] }
	}),

	load: function(section_id) {
		return this.callTxPowerList(section_id).then(L.bind(function(pwrlist) {
			this.powerval = this.wifiNetwork ? this.wifiNetwork.getTXPower() : null;
			this.poweroff = this.wifiNetwork ? this.wifiNetwork.getTXPowerOffset() : null;

			this.value('', _('driver default'));

			for (var i = 0; i < pwrlist.length; i++)
				this.value(pwrlist[i].dbm, '%d dBm (%d mW)'.format(pwrlist[i].dbm, pwrlist[i].mw));

			return form.ListValue.prototype.load.apply(this, [section_id]);
		}, this));
	},

	renderWidget: function(section_id, option_index, cfgvalue) {
		var widget = form.ListValue.prototype.renderWidget.apply(this, [section_id, option_index, cfgvalue]);
		    widget.firstElementChild.style.width = 'auto';

		L.dom.append(widget, E('span', [
			' - ', _('Current power'), ': ',
			E('span', [ this.powerval != null ? '%d dBm'.format(this.powerval) : E('em', _('unknown')) ]),
			this.poweroff ? ' + %d dB offset = %s dBm'.format(this.poweroff, this.powerval != null ? this.powerval + this.poweroff : '?') : ''
		]));

		return widget;
	}
});

var CBIWifiCountryValue = form.Value.extend({
	callCountryList: rpc.declare({
		object: 'iwinfo',
		method: 'countrylist',
		params: [ 'device' ],
		expect: { results: [] }
	}),

	load: function(section_id) {
		return this.callCountryList(section_id).then(L.bind(function(countrylist) {
			if (Array.isArray(countrylist) && countrylist.length > 0) {
				this.value('', _('driver default'));

				for (var i = 0; i < countrylist.length; i++)
					this.value(countrylist[i].iso3166, '%s - %s'.format(countrylist[i].iso3166, countrylist[i].country));
			}

			return form.Value.prototype.load.apply(this, [section_id]);
		}, this));
	},

	validate: function(section_id, formvalue) {
		if (formvalue != null && formvalue != '' && !/^[A-Z0-9][A-Z0-9]$/.test(formvalue))
			return _('Use ISO/IEC 3166 alpha2 country codes.');

		return true;
	},

	renderWidget: function(section_id, option_index, cfgvalue) {
		var typeClass = (this.keylist && this.keylist.length) ? form.ListValue : form.Value;
		return typeClass.prototype.renderWidget.apply(this, [section_id, option_index, cfgvalue]);
	}
});

return L.view.extend({
	poll_status: function(map, data) {
		var rows = map.querySelectorAll('.cbi-section-table-row[data-sid]');

		for (var i = 0; i < rows.length; i++) {
			var section_id = rows[i].getAttribute('data-sid'),
			    radioDev = data[1].filter(function(d) { return d.getName() == section_id })[0],
			    radioNet = data[2].filter(function(n) { return n.getName() == section_id })[0],
			    badge = rows[i].querySelector('[data-name="_badge"] > div'),
			    stat = rows[i].querySelector('[data-name="_stat"]'),
			    btns = rows[i].querySelectorAll('.cbi-section-actions button'),
			    busy = btns[0].classList.contains('spinning') || btns[1].classList.contains('spinning') || btns[2].classList.contains('spinning');

			if (radioDev) {
				L.dom.content(badge, render_radio_badge(radioDev));
				L.dom.content(stat, render_radio_status(radioDev, data[2].filter(function(n) { return n.getWifiDeviceName() == radioDev.getName() })));
			}
			else {
				L.dom.content(badge, render_network_badge(radioNet));
				L.dom.content(stat, render_network_status(radioNet));
			}

			if (stat.hasAttribute('restart'))
				L.dom.content(stat, E('em', _('Device is restarting…')));

			btns[0].disabled = busy;
			btns[1].disabled = busy;
			btns[2].disabled = busy;
		}

		var table = document.querySelector('#wifi_assoclist_table'),
		    hosts = data[0],
		    trows = [];

		for (var i = 0; i < data[3].length; i++) {
			var bss = data[3][i],
			    name = hosts.getHostnameByMACAddr(bss.mac),
			    ipv4 = hosts.getIPAddrByMACAddr(bss.mac),
			    ipv6 = hosts.getIP6AddrByMACAddr(bss.mac);

			var hint;

			if (name && ipv4 && ipv6)
				hint = '%s (%s, %s)'.format(name, ipv4, ipv6);
			else if (name && (ipv4 || ipv6))
				hint = '%s (%s)'.format(name, ipv4 || ipv6);
			else
				hint = name || ipv4 || ipv6 || '?';

			var row = [
				E('span', { 'class': 'ifacebadge' }, [
					E('img', {
						'src': L.resource('icons/wifi%s.png').format(bss.network.isUp() ? '' : '_disabled'),
						'title': bss.radio.getI18n()
					}),
					' %s '.format(bss.network.getShortName()),
					E('small', '(%s)'.format(bss.network.getIfname()))
				]),
				bss.mac,
				hint,
				render_signal_badge(Math.min((bss.signal + 110) / 70 * 100, 100), bss.signal, bss.noise),
				E('span', {}, [
					E('span', format_wifirate(bss.rx)),
					E('br'),
					E('span', format_wifirate(bss.tx))
				])
			];

			if (bss.network.isClientDisconnectSupported()) {
				if (table.firstElementChild.childNodes.length < 6)
					table.firstElementChild.appendChild(E('div', { 'class': 'th nowrap right'}, [ _('Disconnect') ]));

				row.push(E('button', {
					'class': 'cbi-button cbi-button-remove',
					'click': L.bind(function(net, mac, ev) {
						L.dom.parent(ev.currentTarget, '.tr').style.opacity = 0.5;
						ev.currentTarget.classList.add('spinning');
						ev.currentTarget.disabled = true;
						ev.currentTarget.blur();

						net.disconnectClient(mac, true, 5, 60000);
					}, this, bss.network, bss.mac)
				}, [ _('Disconnect') ]));
			}
			else {
				row.push('-');
			}

			trows.push(row);
		}

		cbi_update_table(table, trows, E('em', _('No information available')));

		var stat = document.querySelector('.cbi-modal [data-name="_wifistat_modal"] .ifacebadge.large');

		if (stat)
			render_modal_status(stat, data[2].filter(function(n) { return n.getName() == stat.getAttribute('data-network') })[0]);

		return network.flushCache();
	},

	load: function() {
		return Promise.all([
			uci.changes(),
			uci.load('wireless')
		]);
	},

	checkAnonymousSections: function() {
		var wifiIfaces = uci.sections('wireless', 'wifi-iface');

		for (var i = 0; i < wifiIfaces.length; i++)
			if (wifiIfaces[i]['.anonymous'])
				return true;

		return false;
	},

	callUciRename: rpc.declare({
		object: 'uci',
		method: 'rename',
		params: [ 'config', 'section', 'name' ]
	}),

	render: function() {
		if (this.checkAnonymousSections())
			return this.renderMigration();
		else
			return this.renderOverview();
	},

	handleMigration: function(ev) {
		var wifiIfaces = uci.sections('wireless', 'wifi-iface'),
		    id_offset = 0,
		    tasks = [];

		for (var i = 0; i < wifiIfaces.length; i++) {
			if (!wifiIfaces[i]['.anonymous'])
				continue;

			var new_name = next_free_sid(id_offset);

			tasks.push(this.callUciRename('wireless', wifiIfaces[i]['.name'], new_name));
			id_offset = +new_name.substring(7) + 1;
		}

		return Promise.all(tasks)
			.then(L.bind(ui.changes.init, ui.changes))
			.then(L.bind(ui.changes.apply, ui.changes));
	},

	renderMigration: function() {
		ui.showModal(_('Wireless configuration migration'), [
			E('p', _('The existing wireless configuration needs to be changed for LuCI to function properly.')),
			E('p', _('Upon pressing "Continue", anonymous "wifi-iface" sections will be assigned with a name in the form <em>wifinet#</em> and the network will be restarted to apply the updated configuration.')),
			E('div', { 'class': 'right' },
				E('button', {
					'class': 'btn cbi-button-action important',
					'click': ui.createHandlerFn(this, 'handleMigration')
				}, _('Continue')))
		]);
	},

	renderOverview: function() {
		var m, s, o;

		m = new form.Map('wireless');
		m.chain('network');
		m.chain('firewall');

		s = m.section(form.GridSection, 'wifi-device', _('Wireless Overview'));
		s.anonymous = true;
		s.addremove = false;

		s.load = function() {
			return network.getWifiDevices().then(L.bind(function(radios) {
				this.radios = radios.sort(function(a, b) {
					return a.getName() > b.getName();
				});

				var tasks = [];

				for (var i = 0; i < radios.length; i++)
					tasks.push(radios[i].getWifiNetworks());

				return Promise.all(tasks);
			}, this)).then(L.bind(function(data) {
				this.wifis = [];

				for (var i = 0; i < data.length; i++)
					this.wifis.push.apply(this.wifis, data[i]);
			}, this));
		};

		s.cfgsections = function() {
			var rv = [];

			for (var i = 0; i < this.radios.length; i++) {
				rv.push(this.radios[i].getName());

				for (var j = 0; j < this.wifis.length; j++)
					if (this.wifis[j].getWifiDeviceName() == this.radios[i].getName())
						rv.push(this.wifis[j].getName());
			}

			return rv;
		};

		s.modaltitle = function(section_id) {
			var radioNet = this.wifis.filter(function(w) { return w.getName() == section_id})[0];
			return radioNet ? radioNet.getI18n() : _('Edit wireless network');
		};

		s.lookupRadioOrNetwork = function(section_id) {
			var radioDev = this.radios.filter(function(r) { return r.getName() == section_id })[0];
			if (radioDev)
				return radioDev;

			var radioNet = this.wifis.filter(function(w) { return w.getName() == section_id })[0];
			if (radioNet)
				return radioNet;

			return null;
		};

		s.renderRowActions = function(section_id) {
			var inst = this.lookupRadioOrNetwork(section_id), btns;

			if (inst.getWifiNetworks) {
				btns = [
					E('button', {
						'class': 'cbi-button cbi-button-neutral',
						'title': _('Restart radio interface'),
						'click': ui.createHandlerFn(this, radio_restart, section_id)
					}, _('Restart')),
					E('button', {
						'class': 'cbi-button cbi-button-action important',
						'title': _('Find and join network'),
						'click': ui.createHandlerFn(this, 'handleScan', inst)
					}, _('Scan')),
					E('button', {
						'class': 'cbi-button cbi-button-add',
						'title': _('Provide new network'),
						'click': ui.createHandlerFn(this, 'handleAdd', inst)
					}, _('Add'))
				];
			}
			else {
				var isDisabled = (inst.get('disabled') == '1' ||
					uci.get('wireless', inst.getWifiDeviceName(), 'disabled') == '1');

				btns = [
					E('button', {
						'class': 'cbi-button cbi-button-neutral enable-disable',
						'title': isDisabled ? _('Enable this network') : _('Disable this network'),
						'click': ui.createHandlerFn(this, network_updown, section_id, this.map)
					}, isDisabled ? _('Enable') : _('Disable')),
					E('button', {
						'class': 'cbi-button cbi-button-action important',
						'title': _('Edit this network'),
						'click': ui.createHandlerFn(this, 'renderMoreOptionsModal', section_id)
					}, _('Edit')),
					E('button', {
						'class': 'cbi-button cbi-button-negative remove',
						'title': _('Delete this network'),
						'click': ui.createHandlerFn(this, 'handleRemove', section_id)
					}, _('Remove'))
				];
			}

			return E('div', { 'class': 'td middle cbi-section-actions' }, E('div', btns));
		};

		s.addModalOptions = function(s) {
			return network.getWifiNetwork(s.section).then(function(radioNet) {
				var hwtype = uci.get('wireless', radioNet.getWifiDeviceName(), 'type');
				var o, ss;

				o = s.option(form.SectionValue, '_device', form.NamedSection, radioNet.getWifiDeviceName(), 'wifi-device', _('Device Configuration'));
				o.modalonly = true;

				ss = o.subsection;
				ss.tab('general', _('General Setup'));
				ss.tab('advanced', _('Advanced Settings'));

				var isDisabled = (radioNet.get('disabled') == '1' ||
					uci.get('wireless', radioNet.getWifiDeviceName(), 'disabled') == 1);

				o = ss.taboption('general', form.DummyValue, '_wifistat_modal', _('Status'));
				o.cfgvalue = L.bind(function(radioNet) {
					return render_modal_status(null, radioNet);
				}, this, radioNet);
				o.write = function() {};

				o = ss.taboption('general', form.Button, '_toggle', isDisabled ? _('Wireless network is disabled') : _('Wireless network is enabled'));
				o.inputstyle = isDisabled ? 'apply' : 'reset';
				o.inputtitle = isDisabled ? _('Enable') : _('Disable');
				o.onclick = ui.createHandlerFn(s, network_updown, s.section, s.map);

				o = ss.taboption('general', CBIWifiFrequencyValue, '_freq', '<br />' + _('Operating frequency'));
				o.ucisection = s.section;

				if (hwtype == 'mac80211') {
					o = ss.taboption('general', CBIWifiTxPowerValue, 'txpower', _('Maximum transmit power'), _('Specifies the maximum transmit power the wireless radio may use. Depending on regulatory requirements and wireless usage, the actual transmit power may be reduced by the driver.'));
					o.wifiNetwork = radioNet;

					o = ss.taboption('advanced', CBIWifiCountryValue, 'country', _('Country Code'));
					o.wifiNetwork = radioNet;

					o = ss.taboption('advanced', form.Flag, 'legacy_rates', _('Allow legacy 802.11b rates'));
					o.default = o.enabled;

					o = ss.taboption('advanced', form.Value, 'distance', _('Distance Optimization'), _('Distance to farthest network member in meters.'));
					o.datatype = 'range(0,114750)';
					o.placeholder = 'auto';

					o = ss.taboption('advanced', form.Value, 'frag', _('Fragmentation Threshold'));
					o.datatype = 'min(256)';
					o.placeholder = _('off');

					o = ss.taboption('advanced', form.Value, 'rts', _('RTS/CTS Threshold'));
					o.datatype = 'uinteger';
					o.placeholder = _('off');

					o = ss.taboption('advanced', form.Flag, 'noscan', _('Force 40MHz mode'), _('Always use 40MHz channels even if the secondary channel overlaps. Using this option does not comply with IEEE 802.11n-2009!'));
					o.rmempty = true;

					o = ss.taboption('advanced', form.Value, 'beacon_int', _('Beacon Interval'));
					o.datatype = 'range(15,65535)';
					o.placeholder = 100;
					o.rmempty = true;
				}


				o = s.option(form.SectionValue, '_device', form.NamedSection, radioNet.getName(), 'wifi-iface', _('Interface Configuration'));
				o.modalonly = true;

				ss = o.subsection;
				ss.tab('general', _('General Setup'));
				ss.tab('encryption', _('Wireless Security'));
				ss.tab('macfilter', _('MAC-Filter'));
				ss.tab('advanced', _('Advanced Settings'));

				o = ss.taboption('general', form.ListValue, 'mode', _('Mode'));
				o.value('ap', _('Access Point'));
				o.value('sta', _('Client'));
				o.value('adhoc', _('Ad-Hoc'));

				o = ss.taboption('general', form.Value, 'mesh_id', _('Mesh Id'));
				o.depends('mode', 'mesh');

				o = ss.taboption('advanced', form.Flag, 'mesh_fwding', _('Forward mesh peer traffic'));
				o.rmempty = false;
				o.default = '1';
				o.depends('mode', 'mesh');

				o = ss.taboption('advanced', form.Value, 'mesh_rssi_threshold', _('RSSI threshold for joining'), _('0 = not using RSSI threshold, 1 = do not change driver default'));
				o.rmempty = false;
				o.default = '0';
				o.datatype = 'range(-255,1)';
				o.depends('mode', 'mesh');

				o = ss.taboption('general', form.Value, 'ssid', _('<abbr title="Extended Service Set Identifier">ESSID</abbr>'));
				o.datatype = 'maxlength(32)';
				o.depends('mode', 'ap');
				o.depends('mode', 'sta');
				o.depends('mode', 'adhoc');
				o.depends('mode', 'ahdemo');
				o.depends('mode', 'monitor');
				o.depends('mode', 'ap-wds');
				o.depends('mode', 'sta-wds');
				o.depends('mode', 'wds');

				o = ss.taboption('general', form.Value, 'bssid', _('<abbr title="Basic Service Set Identifier">BSSID</abbr>'));
				o.datatype = 'macaddr';

				o = ss.taboption('general', widgets.NetworkSelect, 'network', _('Network'), _('Choose the network(s) you want to attach to this wireless interface or fill out the <em>create</em> field to define a new network.'));
				o.rmempty = true;
				o.multiple = true;
				o.novirtual = true;
				o.write = function(section_id, value) {
					return network.getDevice(section_id).then(L.bind(function(dev) {
						var old_networks = dev.getNetworks().reduce(function(o, v) { o[v.getName()] = v; return o }, {}),
						    new_networks = {},
						    values = L.toArray(value),
						    tasks = [];

						for (var i = 0; i < values.length; i++) {
							new_networks[values[i]] = true;

							if (old_networks[values[i]])
								continue;

							tasks.push(network.getNetwork(values[i]).then(L.bind(function(name, net) {
								return net || network.addNetwork(name, { proto: 'none' });
							}, this, values[i])).then(L.bind(function(dev, net) {
								if (net) {
									if (!net.isEmpty())
										net.set('type', 'bridge');
									net.addDevice(dev);
								}
							}, this, dev)));
						}

						for (var name in old_networks)
							if (!new_networks[name])
								tasks.push(network.getNetwork(name).then(L.bind(function(dev, net) {
									if (net)
										net.deleteDevice(dev);
								}, this, dev)));

						return Promise.all(tasks);
					}, this));
				};

				if (hwtype == 'mac80211') {
					var mode = ss.children[0],
					    bssid = ss.children[5],
					    encr;

					mode.value('mesh', '802.11s');
					mode.value('ahdemo', _('Pseudo Ad-Hoc (ahdemo)'));
					mode.value('monitor', _('Monitor'));

					bssid.depends('mode', 'adhoc');
					bssid.depends('mode', 'sta');
					bssid.depends('mode', 'sta-wds');

					o = ss.taboption('macfilter', form.ListValue, 'macfilter', _('MAC-Address Filter'));
					o.depends('mode', 'ap');
					o.depends('mode', 'ap-wds');
					o.value('', _('disable'));
					o.value('allow', _('Allow listed only'));
					o.value('deny', _('Allow all except listed'));

					o = ss.taboption('macfilter', form.DynamicList, 'maclist', _('MAC-List'));
					o.datatype = 'macaddr';
					o.depends('macfilter', 'allow');
					o.depends('macfilter', 'deny');
					o.load = function(section_id) {
						return network.getHostHints().then(L.bind(function(hints) {
							hints.getMACHints().map(L.bind(function(hint) {
								this.value(hint[0], hint[1] ? '%s (%s)'.format(hint[0], hint[1]) : hint[0]);
							}, this));

							return form.DynamicList.prototype.load.apply(this, [section_id]);
						}, this));
					};

					mode.value('ap-wds', '%s (%s)'.format(_('Access Point'), _('WDS')));
					mode.value('sta-wds', '%s (%s)'.format(_('Client'), _('WDS')));

					mode.write = function(section_id, value) {
						switch (value) {
						case 'ap-wds':
							uci.set('wireless', section_id, 'mode', 'ap');
							uci.set('wireless', section_id, 'wds', '1');
							break;

						case 'sta-wds':
							uci.set('wireless', section_id, 'mode', 'sta');
							uci.set('wireless', section_id, 'wds', '1');
							break;

						default:
							uci.set('wireless', section_id, 'mode', value);
							uci.unset('wireless', section_id, 'wds');
							break;
						}
					};

					mode.cfgvalue = function(section_id) {
						var mode = uci.get('wireless', section_id, 'mode'),
						    wds = uci.get('wireless', section_id, 'wds');

						if (mode == 'ap' && wds)
							return 'ap-wds';
						else if (mode == 'sta' && wds)
							return 'sta-wds';

						return mode;
					};

					o = ss.taboption('general', form.Flag, 'hidden', _('Hide <abbr title="Extended Service Set Identifier">ESSID</abbr>'));
					o.depends('mode', 'ap');
					o.depends('mode', 'ap-wds');

					o = ss.taboption('general', form.Flag, 'wmm', _('WMM Mode'));
					o.depends('mode', 'ap');
					o.depends('mode', 'ap-wds');
					o.default = o.enabled;

					o = ss.taboption('advanced', form.Flag, 'isolate', _('Isolate Clients'), _('Prevents client-to-client communication'));
					o.depends('mode', 'ap');
					o.depends('mode', 'ap-wds');

					o = ss.taboption('advanced', form.Value, 'ifname', _('Interface name'), _('Override default interface name'));
					o.optional = true;
					o.placeholder = radioNet.getIfname();
					if (/^radio\d+\.network/.test(o.placeholder))
						o.placeholder = '';

					o = ss.taboption('advanced', form.Flag, 'short_preamble', _('Short Preamble'));
					o.default = o.enabled;

					o = ss.taboption('advanced', form.Value, 'dtim_period', _('DTIM Interval'), _('Delivery Traffic Indication Message Interval'));
					o.optional = true;
					o.placeholder = 2;
					o.datatype = 'range(1,255)';

					o = ss.taboption('advanced', form.Value, 'wpa_group_rekey', _('Time interval for rekeying GTK'), _('sec'));
					o.optional    = true;
					o.placeholder = 600;
					o.datatype    = 'uinteger';

					o = ss.taboption('advanced', form.Flag , 'skip_inactivity_poll', _('Disable Inactivity Polling'));
					o.optional    = true;
					o.datatype    = 'uinteger';

					o = ss.taboption('advanced', form.Value, 'max_inactivity', _('Station inactivity limit'), _('sec'));
					o.optional    = true;
					o.placeholder = 300;
					o.datatype    = 'uinteger';

					o = ss.taboption('advanced', form.Value, 'max_listen_interval', _('Maximum allowed Listen Interval'));
					o.optional    = true;
					o.placeholder = 65535;
					o.datatype    = 'uinteger';

					o = ss.taboption('advanced', form.Flag, 'disassoc_low_ack', _('Disassociate On Low Acknowledgement'), _('Allow AP mode to disconnect STAs based on low ACK condition'));
					o.default = o.enabled;
				}


				encr = o = ss.taboption('encryption', form.ListValue, 'encryption', _('Encryption'));
				o.depends('mode', 'ap');
				o.depends('mode', 'sta');
				o.depends('mode', 'adhoc');
				o.depends('mode', 'ahdemo');
				o.depends('mode', 'ap-wds');
				o.depends('mode', 'sta-wds');
				o.depends('mode', 'mesh');

				o.cfgvalue = function(section_id) {
					var v = String(uci.get('wireless', section_id, 'encryption'));
					if (v == 'wep')
						return 'wep-open';
					else if (v.match(/\+/))
						return v.replace(/\+.+$/, '');
					return v;
				};

				o.write = function(section_id, value) {
					var e = this.section.children.filter(function(o) { return o.option == 'encryption' })[0].formvalue(section_id),
					    co = this.section.children.filter(function(o) { return o.option == 'cipher' })[0], c = co.formvalue(section_id);

					if (value == 'wpa' || value == 'wpa2')
						uci.unset('wireless', section_id, 'key');

					if (co.isActive(section_id) && e && (c == 'tkip' || c == 'ccmp' || c == 'tkip+ccmp'))
						e += '+' + c;

					uci.set('wireless', section_id, 'encryption', e);
				};

				o = ss.taboption('encryption', form.ListValue, 'cipher', _('Cipher'));
				o.depends('encryption', 'wpa');
				o.depends('encryption', 'wpa2');
				o.depends('encryption', 'psk');
				o.depends('encryption', 'psk2');
				o.depends('encryption', 'wpa-mixed');
				o.depends('encryption', 'psk-mixed');
				o.value('auto', _('auto'));
				o.value('ccmp', _('Force CCMP (AES)'));
				o.value('tkip', _('Force TKIP'));
				o.value('tkip+ccmp', _('Force TKIP and CCMP (AES)'));
				o.write = ss.children.filter(function(o) { return o.option == 'encryption' })[0].write;

				o.cfgvalue = function(section_id) {
					var v = String(uci.get('wireless', section_id, 'encryption'));
					if (v.match(/\+/)) {
						v = v.replace(/^[^+]+\+/, '');
						if (v == 'aes')
							v = 'ccmp';
						else if (v == 'tkip+aes' || v == 'aes+tkip' || v == 'ccmp+tkip')
							v = 'tkip+ccmp';
					}
					return v;
				};


				var crypto_modes = [];

				if (hwtype == 'mac80211') {
					var has_supplicant = L.hasSystemFeature('wpasupplicant'),
					    has_hostapd = L.hasSystemFeature('hostapd');

					// Probe EAP support
					var has_ap_eap = L.hasSystemFeature('hostapd', 'eap'),
					    has_sta_eap = L.hasSystemFeature('wpasupplicant', 'eap');

					// Probe SAE support
					var has_ap_sae = L.hasSystemFeature('hostapd', 'sae'),
					    has_sta_sae = L.hasSystemFeature('wpasupplicant', 'sae');

					// Probe OWE support
					var has_ap_owe = L.hasSystemFeature('hostapd', 'owe'),
					    has_sta_owe = L.hasSystemFeature('wpasupplicant', 'owe');


					if (has_hostapd || has_supplicant) {
						crypto_modes.push(['psk2',      'WPA2-PSK',                    33]);
						crypto_modes.push(['psk-mixed', 'WPA-PSK/WPA2-PSK Mixed Mode', 22]);
						crypto_modes.push(['psk',       'WPA-PSK',                     21]);
					}
					else {
						encr.description = _('WPA-Encryption requires wpa_supplicant (for client mode) or hostapd (for AP and ad-hoc mode) to be installed.');
					}

					if (has_ap_sae || has_sta_sae) {
						crypto_modes.push(['sae',       'WPA3-SAE',                     31]);
						crypto_modes.push(['sae-mixed', 'WPA2-PSK/WPA3-SAE Mixed Mode', 30]);
					}

					if (has_ap_eap || has_sta_eap) {
						crypto_modes.push(['wpa2', 'WPA2-EAP', 32]);
						crypto_modes.push(['wpa',  'WPA-EAP',  20]);
					}

					if (has_ap_owe || has_sta_owe) {
						crypto_modes.push(['owe', 'OWE', 1]);
					}

					encr.crypto_support = {
						'ap': {
							'wep-open': true,
							'wep-shared': true,
							'psk': has_hostapd || _('Requires hostapd'),
							'psk2': has_hostapd || _('Requires hostapd'),
							'psk-mixed': has_hostapd || _('Requires hostapd'),
							'sae': has_ap_sae || _('Requires hostapd with SAE support'),
							'sae-mixed': has_ap_sae || _('Requires hostapd with SAE support'),
							'wpa': has_ap_eap || _('Requires hostapd with EAP support'),
							'wpa2': has_ap_eap || _('Requires hostapd with EAP support'),
							'owe': has_ap_owe || _('Requires hostapd with OWE support')
						},
						'sta': {
							'wep-open': true,
							'wep-shared': true,
							'psk': has_supplicant || _('Requires wpa-supplicant'),
							'psk2': has_supplicant || _('Requires wpa-supplicant'),
							'psk-mixed': has_supplicant || _('Requires wpa-supplicant'),
							'sae': has_sta_sae || _('Requires wpa-supplicant with SAE support'),
							'sae-mixed': has_sta_sae || _('Requires wpa-supplicant with SAE support'),
							'wpa': has_sta_eap || _('Requires wpa-supplicant with EAP support'),
							'wpa2': has_sta_eap || _('Requires wpa-supplicant with EAP support'),
							'owe': has_sta_owe || _('Requires wpa-supplicant with OWE support')
						},
						'adhoc': {
							'wep-open': true,
							'wep-shared': true,
							'psk': has_supplicant || _('Requires wpa-supplicant'),
							'psk2': has_supplicant || _('Requires wpa-supplicant'),
							'psk-mixed': has_supplicant || _('Requires wpa-supplicant'),
						},
						'mesh': {
							'sae': has_sta_sae || _('Requires wpa-supplicant with SAE support')
						},
						'ahdemo': {
							'wep-open': true,
							'wep-shared': true
						},
						'wds': {
							'wep-open': true,
							'wep-shared': true
						}
					};

					encr.crypto_support['ap-wds'] = encr.crypto_support['ap'];
					encr.crypto_support['sta-wds'] = encr.crypto_support['sta'];

					encr.validate = function(section_id, value) {
						var modeopt = this.section.children.filter(function(o) { return o.option == 'mode' })[0],
						    modeval = modeopt.formvalue(section_id),
						    modetitle = modeopt.vallist[modeopt.keylist.indexOf(modeval)],
						    enctitle = this.vallist[this.keylist.indexOf(value)];

						if (value == 'none')
							return true;

						if (!L.isObject(this.crypto_support[modeval]) || !this.crypto_support[modeval].hasOwnProperty(value))
							return _('The selected %s mode is incompatible with %s encryption').format(modetitle, enctitle);

						return this.crypto_support[modeval][value];
					};
				}
				else if (hwtype == 'broadcom') {
					crypto_modes.push(['psk2',     'WPA2-PSK',                    33]);
					crypto_modes.push(['psk+psk2', 'WPA-PSK/WPA2-PSK Mixed Mode', 22]);
					crypto_modes.push(['psk',      'WPA-PSK',                     21]);
				}

				crypto_modes.push(['wep-open',   _('WEP Open System'), 11]);
				crypto_modes.push(['wep-shared', _('WEP Shared Key'),  10]);
				crypto_modes.push(['none',       _('No Encryption'),   0]);

				crypto_modes.sort(function(a, b) { return b[2] - a[2] });

				for (var i = 0; i < crypto_modes.length; i++) {
					var security_level = (crypto_modes[i][2] >= 30) ? _('strong security')
						: (crypto_modes[i][2] >= 20) ? _('medium security')
							: (crypto_modes[i][2] >= 10) ? _('weak security') : _('open network');

					encr.value(crypto_modes[i][0], '%s (%s)'.format(crypto_modes[i][1], security_level));
				}


				o = ss.taboption('encryption', form.Value, 'auth_server', _('Radius-Authentication-Server'));
				o.depends({ mode: 'ap', encryption: 'wpa' });
				o.depends({ mode: 'ap', encryption: 'wpa2' });
				o.depends({ mode: 'ap-wds', encryption: 'wpa' });
				o.depends({ mode: 'ap-wds', encryption: 'wpa2' });
				o.rmempty = true;
				o.datatype = 'host(0)';

				o = ss.taboption('encryption', form.Value, 'auth_port', _('Radius-Authentication-Port'), _('Default %d').format(1812));
				o.depends({ mode: 'ap', encryption: 'wpa' });
				o.depends({ mode: 'ap', encryption: 'wpa2' });
				o.depends({ mode: 'ap-wds', encryption: 'wpa' });
				o.depends({ mode: 'ap-wds', encryption: 'wpa2' });
				o.rmempty = true;
				o.datatype = 'port';

				o = ss.taboption('encryption', form.Value, 'auth_secret', _('Radius-Authentication-Secret'));
				o.depends({ mode: 'ap', encryption: 'wpa' });
				o.depends({ mode: 'ap', encryption: 'wpa2' });
				o.depends({ mode: 'ap-wds', encryption: 'wpa' });
				o.depends({ mode: 'ap-wds', encryption: 'wpa2' });
				o.rmempty = true;
				o.password = true;

				o = ss.taboption('encryption', form.Value, 'acct_server', _('Radius-Accounting-Server'));
				o.depends({ mode: 'ap', encryption: 'wpa' });
				o.depends({ mode: 'ap', encryption: 'wpa2' });
				o.depends({ mode: 'ap-wds', encryption: 'wpa' });
				o.depends({ mode: 'ap-wds', encryption: 'wpa2' });
				o.rmempty = true;
				o.datatype = 'host(0)';

				o = ss.taboption('encryption', form.Value, 'acct_port', _('Radius-Accounting-Port'), _('Default %d').format(1813));
				o.depends({ mode: 'ap', encryption: 'wpa' });
				o.depends({ mode: 'ap', encryption: 'wpa2' });
				o.depends({ mode: 'ap-wds', encryption: 'wpa' });
				o.depends({ mode: 'ap-wds', encryption: 'wpa2' });
				o.rmempty = true;
				o.datatype = 'port';

				o = ss.taboption('encryption', form.Value, 'acct_secret', _('Radius-Accounting-Secret'));
				o.depends({ mode: 'ap', encryption: 'wpa' });
				o.depends({ mode: 'ap', encryption: 'wpa2' });
				o.depends({ mode: 'ap-wds', encryption: 'wpa' });
				o.depends({ mode: 'ap-wds', encryption: 'wpa2' });
				o.rmempty = true;
				o.password = true;

				o = ss.taboption('encryption', form.Value, 'dae_client', _('DAE-Client'));
				o.depends({ mode: 'ap', encryption: 'wpa' });
				o.depends({ mode: 'ap', encryption: 'wpa2' });
				o.depends({ mode: 'ap-wds', encryption: 'wpa' });
				o.depends({ mode: 'ap-wds', encryption: 'wpa2' });
				o.rmempty = true;
				o.datatype = 'host(0)';

				o = ss.taboption('encryption', form.Value, 'dae_port', _('DAE-Port'), _('Default %d').format(3799));
				o.depends({ mode: 'ap', encryption: 'wpa' });
				o.depends({ mode: 'ap', encryption: 'wpa2' });
				o.depends({ mode: 'ap-wds', encryption: 'wpa' });
				o.depends({ mode: 'ap-wds', encryption: 'wpa2' });
				o.rmempty = true;
				o.datatype = 'port';

				o = ss.taboption('encryption', form.Value, 'dae_secret', _('DAE-Secret'));
				o.depends({ mode: 'ap', encryption: 'wpa' });
				o.depends({ mode: 'ap', encryption: 'wpa2' });
				o.depends({ mode: 'ap-wds', encryption: 'wpa' });
				o.depends({ mode: 'ap-wds', encryption: 'wpa2' });
				o.rmempty = true;
				o.password = true;


				o = ss.taboption('encryption', form.Value, '_wpa_key', _('Key'));
				o.depends('encryption', 'psk');
				o.depends('encryption', 'psk2');
				o.depends('encryption', 'psk+psk2');
				o.depends('encryption', 'psk-mixed');
				o.depends('encryption', 'sae');
				o.depends('encryption', 'sae-mixed');
				o.datatype = 'wpakey';
				o.rmempty = true;
				o.password = true;

				o.cfgvalue = function(section_id) {
					var key = uci.get('wireless', section_id, 'key');
					return /^[1234]$/.test(key) ? null : key;
				};

				o.write = function(section_id, value) {
					uci.set('wireless', section_id, 'key', value);
					uci.unset('wireless', section_id, 'key1');
					uci.unset('wireless', section_id, 'key2');
					uci.unset('wireless', section_id, 'key3');
					uci.unset('wireless', section_id, 'key4');
				};


				o = ss.taboption('encryption', form.ListValue, '_wep_key', _('Used Key Slot'));
				o.depends('encryption', 'wep-open');
				o.depends('encryption', 'wep-shared');
				o.value('1', _('Key #%d').format(1));
				o.value('2', _('Key #%d').format(2));
				o.value('3', _('Key #%d').format(3));
				o.value('4', _('Key #%d').format(4));

				o.cfgvalue = function(section_id) {
					var slot = +uci.get('wireless', section_id, 'key');
					return (slot >= 1 && slot <= 4) ? String(slot) : '';
				};

				o.write = function(section_id, value) {
					uci.set('wireless', section_id, 'key', value);
				};

				for (var slot = 1; slot <= 4; slot++) {
					o = ss.taboption('encryption', form.Value, 'key%d'.format(slot), _('Key #%d').format(slot));
					o.depends('encryption', 'wep-open');
					o.depends('encryption', 'wep-shared');
					o.datatype = 'wepkey';
					o.rmempty = true;
					o.password = true;

					o.write = function(section_id, value) {
						if (value != null && (value.length == 5 || value.length == 13))
							value = 's:%s'.format(value);
						uci.set('wireless', section_id, this.option, value);
					};
				}


				if (hwtype == 'mac80211') {
					// Probe 802.11r support (and EAP support as a proxy for Openwrt)
					var has_80211r = L.hasSystemFeature('hostapd', '11r') || L.hasSystemFeature('hostapd', 'eap');

					o = ss.taboption('encryption', form.Flag, 'ieee80211r', _('802.11r Fast Transition'), _('Enables fast roaming among access points that belong to the same Mobility Domain'));
					o.depends({ mode: 'ap', encryption: 'wpa' });
					o.depends({ mode: 'ap', encryption: 'wpa2' });
					o.depends({ mode: 'ap-wds', encryption: 'wpa' });
					o.depends({ mode: 'ap-wds', encryption: 'wpa2' });
					if (has_80211r) {
						o.depends({ mode: 'ap', encryption: 'psk' });
						o.depends({ mode: 'ap', encryption: 'psk2' });
						o.depends({ mode: 'ap', encryption: 'psk-mixed' });
						o.depends({ mode: 'ap', encryption: 'sae' });
						o.depends({ mode: 'ap', encryption: 'sae-mixed' });
						o.depends({ mode: 'ap-wds', encryption: 'psk' });
						o.depends({ mode: 'ap-wds', encryption: 'psk2' });
						o.depends({ mode: 'ap-wds', encryption: 'psk-mixed' });
						o.depends({ mode: 'ap-wds', encryption: 'sae' });
						o.depends({ mode: 'ap-wds', encryption: 'sae-mixed' });
					}
					o.rmempty = true;

					o = ss.taboption('encryption', form.Value, 'nasid', _('NAS ID'), _('Used for two different purposes: RADIUS NAS ID and 802.11r R0KH-ID. Not needed with normal WPA(2)-PSK.'));
					o.depends({ mode: 'ap', encryption: 'wpa' });
					o.depends({ mode: 'ap', encryption: 'wpa2' });
					o.depends({ mode: 'ap-wds', encryption: 'wpa' });
					o.depends({ mode: 'ap-wds', encryption: 'wpa2' });
					o.depends({ ieee80211r: '1' });
					o.rmempty = true;

					o = ss.taboption('encryption', form.Value, 'mobility_domain', _('Mobility Domain'), _('4-character hexadecimal ID'));
					o.depends({ ieee80211r: '1' });
					o.placeholder = '4f57';
					o.datatype = 'and(hexstring,length(4))';
					o.rmempty = true;

					o = ss.taboption('encryption', form.Value, 'reassociation_deadline', _('Reassociation Deadline'), _('time units (TUs / 1.024 ms) [1000-65535]'));
					o.depends({ ieee80211r: '1' });
					o.placeholder = '1000';
					o.datatype = 'range(1000,65535)';
					o.rmempty = true;

					o = ss.taboption('encryption', form.ListValue, 'ft_over_ds', _('FT protocol'));
					o.depends({ ieee80211r: '1' });
					o.value('1', _('FT over DS'));
					o.value('0', _('FT over the Air'));
					o.rmempty = true;

					o = ss.taboption('encryption', form.Flag, 'ft_psk_generate_local', _('Generate PMK locally'), _('When using a PSK, the PMK can be automatically generated. When enabled, the R0/R1 key options below are not applied. Disable this to use the R0 and R1 key options.'));
					o.depends({ ieee80211r: '1' });
					o.default = o.enabled;
					o.rmempty = false;

					o = ss.taboption('encryption', form.Value, 'r0_key_lifetime', _('R0 Key Lifetime'), _('minutes'));
					o.depends({ ieee80211r: '1' });
					o.placeholder = '10000';
					o.datatype = 'uinteger';
					o.rmempty = true;

					o = ss.taboption('encryption', form.Value, 'r1_key_holder', _('R1 Key Holder'), _('6-octet identifier as a hex string - no colons'));
					o.depends({ ieee80211r: '1' });
					o.placeholder = '00004f577274';
					o.datatype = 'and(hexstring,length(12))';
					o.rmempty = true;

					o = ss.taboption('encryption', form.Flag, 'pmk_r1_push', _('PMK R1 Push'));
					o.depends({ ieee80211r: '1' });
					o.placeholder = '0';
					o.rmempty = true;

					o = ss.taboption('encryption', form.DynamicList, 'r0kh', _('External R0 Key Holder List'), _('List of R0KHs in the same Mobility Domain. <br />Format: MAC-address,NAS-Identifier,128-bit key as hex string. <br />This list is used to map R0KH-ID (NAS Identifier) to a destination MAC address when requesting PMK-R1 key from the R0KH that the STA used during the Initial Mobility Domain Association.'));
					o.depends({ ieee80211r: '1' });
					o.rmempty = true;

					o = ss.taboption('encryption', form.DynamicList, 'r1kh', _('External R1 Key Holder List'), _ ('List of R1KHs in the same Mobility Domain. <br />Format: MAC-address,R1KH-ID as 6 octets with colons,128-bit key as hex string. <br />This list is used to map R1KH-ID to a destination MAC address when sending PMK-R1 key from the R0KH. This is also the list of authorized R1KHs in the MD that can request PMK-R1 keys.'));
					o.depends({ ieee80211r: '1' });
					o.rmempty = true;
					// End of 802.11r options

					o = ss.taboption('encryption', form.ListValue, 'eap_type', _('EAP-Method'));
					o.value('tls',  'TLS');
					o.value('ttls', 'TTLS');
					o.value('peap', 'PEAP');
					o.value('fast', 'FAST');
					o.depends({ mode: 'sta', encryption: 'wpa' });
					o.depends({ mode: 'sta', encryption: 'wpa2' });
					o.depends({ mode: 'sta-wds', encryption: 'wpa' });
					o.depends({ mode: 'sta-wds', encryption: 'wpa2' });

					o = ss.taboption('encryption', form.FileUpload, 'ca_cert', _('Path to CA-Certificate'));
					o.depends({ mode: 'sta', encryption: 'wpa' });
					o.depends({ mode: 'sta', encryption: 'wpa2' });
					o.depends({ mode: 'sta-wds', encryption: 'wpa' });
					o.depends({ mode: 'sta-wds', encryption: 'wpa2' });

					o = ss.taboption('encryption', form.FileUpload, 'client_cert', _('Path to Client-Certificate'));
					o.depends({ mode: 'sta', eap_type: 'tls', encryption: 'wpa' });
					o.depends({ mode: 'sta', eap_type: 'tls', encryption: 'wpa2' });
					o.depends({ mode: 'sta-wds', eap_type: 'tls', encryption: 'wpa' });
					o.depends({ mode: 'sta-wds', eap_type: 'tls', encryption: 'wpa2' });

					o = ss.taboption('encryption', form.FileUpload, 'priv_key', _('Path to Private Key'));
					o.depends({ mode: 'sta', eap_type: 'tls', encryption: 'wpa2' });
					o.depends({ mode: 'sta', eap_type: 'tls', encryption: 'wpa' });
					o.depends({ mode: 'sta-wds', eap_type: 'tls', encryption: 'wpa2' });
					o.depends({ mode: 'sta-wds', eap_type: 'tls', encryption: 'wpa' });

					o = ss.taboption('encryption', form.Value, 'priv_key_pwd', _('Password of Private Key'));
					o.depends({ mode: 'sta', eap_type: 'tls', encryption: 'wpa2' });
					o.depends({ mode: 'sta', eap_type: 'tls', encryption: 'wpa' });
					o.depends({ mode: 'sta-wds', eap_type: 'tls', encryption: 'wpa2' });
					o.depends({ mode: 'sta-wds', eap_type: 'tls', encryption: 'wpa' });
					o.password = true;

					o = ss.taboption('encryption', form.ListValue, 'auth', _('Authentication'));
					o.value('PAP', 'PAP');
					o.value('CHAP', 'CHAP');
					o.value('MSCHAP', 'MSCHAP');
					o.value('MSCHAPV2', 'MSCHAPv2');
					o.value('EAP-GTC');
					o.value('EAP-MD5');
					o.value('EAP-MSCHAPV2');
					o.value('EAP-TLS');
					o.depends({ mode: 'sta', eap_type: 'fast', encryption: 'wpa2' });
					o.depends({ mode: 'sta', eap_type: 'fast', encryption: 'wpa' });
					o.depends({ mode: 'sta', eap_type: 'peap', encryption: 'wpa2' });
					o.depends({ mode: 'sta', eap_type: 'peap', encryption: 'wpa' });
					o.depends({ mode: 'sta', eap_type: 'ttls', encryption: 'wpa2' });
					o.depends({ mode: 'sta', eap_type: 'ttls', encryption: 'wpa' });
					o.depends({ mode: 'sta-wds', eap_type: 'fast', encryption: 'wpa2' });
					o.depends({ mode: 'sta-wds', eap_type: 'fast', encryption: 'wpa' });
					o.depends({ mode: 'sta-wds', eap_type: 'peap', encryption: 'wpa2' });
					o.depends({ mode: 'sta-wds', eap_type: 'peap', encryption: 'wpa' });
					o.depends({ mode: 'sta-wds', eap_type: 'ttls', encryption: 'wpa2' });
					o.depends({ mode: 'sta-wds', eap_type: 'ttls', encryption: 'wpa' });

					o.validate = function(section_id, value) {
						var eo = this.section.children.filter(function(o) { return o.option == 'eap_type' })[0],
						    ev = eo.formvalue(section_id);

						if (ev != 'ttls' && (value == 'PAP' || value == 'CHAP' || value == 'MSCHAP' || value == 'MSCHAPV2'))
							return _('This authentication type is not applicable to the selected EAP method.');

						return true;
					};

					o = ss.taboption('encryption', form.FileUpload, 'ca_cert2', _('Path to inner CA-Certificate'));
					o.depends({ mode: 'sta', auth: 'EAP-TLS', encryption: 'wpa' });
					o.depends({ mode: 'sta', auth: 'EAP-TLS', encryption: 'wpa2' });
					o.depends({ mode: 'sta-wds', auth: 'EAP-TLS', encryption: 'wpa' });
					o.depends({ mode: 'sta-wds', auth: 'EAP-TLS', encryption: 'wpa2' });

					o = ss.taboption('encryption', form.FileUpload, 'client_cert2', _('Path to inner Client-Certificate'));
					o.depends({ mode: 'sta', auth: 'EAP-TLS', encryption: 'wpa' });
					o.depends({ mode: 'sta', auth: 'EAP-TLS', encryption: 'wpa2' });
					o.depends({ mode: 'sta-wds', auth: 'EAP-TLS', encryption: 'wpa' });
					o.depends({ mode: 'sta-wds', auth: 'EAP-TLS', encryption: 'wpa2' });

					o = ss.taboption('encryption', form.FileUpload, 'priv_key2', _('Path to inner Private Key'));
					o.depends({ mode: 'sta', auth: 'EAP-TLS', encryption: 'wpa' });
					o.depends({ mode: 'sta', auth: 'EAP-TLS', encryption: 'wpa2' });
					o.depends({ mode: 'sta-wds', auth: 'EAP-TLS', encryption: 'wpa' });
					o.depends({ mode: 'sta-wds', auth: 'EAP-TLS', encryption: 'wpa2' });

					o = ss.taboption('encryption', form.Value, 'priv_key2_pwd', _('Password of inner Private Key'));
					o.depends({ mode: 'sta', auth: 'EAP-TLS', encryption: 'wpa' });
					o.depends({ mode: 'sta', auth: 'EAP-TLS', encryption: 'wpa2' });
					o.depends({ mode: 'sta-wds', auth: 'EAP-TLS', encryption: 'wpa' });
					o.depends({ mode: 'sta-wds', auth: 'EAP-TLS', encryption: 'wpa2' });
					o.password = true;

					o = ss.taboption('encryption', form.Value, 'identity', _('Identity'));
					o.depends({ mode: 'sta', eap_type: 'fast', encryption: 'wpa2' });
					o.depends({ mode: 'sta', eap_type: 'fast', encryption: 'wpa' });
					o.depends({ mode: 'sta', eap_type: 'peap', encryption: 'wpa2' });
					o.depends({ mode: 'sta', eap_type: 'peap', encryption: 'wpa' });
					o.depends({ mode: 'sta', eap_type: 'ttls', encryption: 'wpa2' });
					o.depends({ mode: 'sta', eap_type: 'ttls', encryption: 'wpa' });
					o.depends({ mode: 'sta-wds', eap_type: 'fast', encryption: 'wpa2' });
					o.depends({ mode: 'sta-wds', eap_type: 'fast', encryption: 'wpa' });
					o.depends({ mode: 'sta-wds', eap_type: 'peap', encryption: 'wpa2' });
					o.depends({ mode: 'sta-wds', eap_type: 'peap', encryption: 'wpa' });
					o.depends({ mode: 'sta-wds', eap_type: 'ttls', encryption: 'wpa2' });
					o.depends({ mode: 'sta-wds', eap_type: 'ttls', encryption: 'wpa' });
					o.depends({ mode: 'sta', eap_type: 'tls', encryption: 'wpa2' });
					o.depends({ mode: 'sta', eap_type: 'tls', encryption: 'wpa' });
					o.depends({ mode: 'sta-wds', eap_type: 'tls', encryption: 'wpa2' });
					o.depends({ mode: 'sta-wds', eap_type: 'tls', encryption: 'wpa' });

					o = ss.taboption('encryption', form.Value, 'anonymous_identity', _('Anonymous Identity'));
					o.depends({ mode: 'sta', eap_type: 'fast', encryption: 'wpa2' });
					o.depends({ mode: 'sta', eap_type: 'fast', encryption: 'wpa' });
					o.depends({ mode: 'sta', eap_type: 'peap', encryption: 'wpa2' });
					o.depends({ mode: 'sta', eap_type: 'peap', encryption: 'wpa' });
					o.depends({ mode: 'sta', eap_type: 'ttls', encryption: 'wpa2' });
					o.depends({ mode: 'sta', eap_type: 'ttls', encryption: 'wpa' });
					o.depends({ mode: 'sta-wds', eap_type: 'fast', encryption: 'wpa2' });
					o.depends({ mode: 'sta-wds', eap_type: 'fast', encryption: 'wpa' });
					o.depends({ mode: 'sta-wds', eap_type: 'peap', encryption: 'wpa2' });
					o.depends({ mode: 'sta-wds', eap_type: 'peap', encryption: 'wpa' });
					o.depends({ mode: 'sta-wds', eap_type: 'ttls', encryption: 'wpa2' });
					o.depends({ mode: 'sta-wds', eap_type: 'ttls', encryption: 'wpa' });
					o.depends({ mode: 'sta', eap_type: 'tls', encryption: 'wpa2' });
					o.depends({ mode: 'sta', eap_type: 'tls', encryption: 'wpa' });
					o.depends({ mode: 'sta-wds', eap_type: 'tls', encryption: 'wpa2' });
					o.depends({ mode: 'sta-wds', eap_type: 'tls', encryption: 'wpa' });

					o = ss.taboption('encryption', form.Value, 'password', _('Password'));
					o.depends({ mode: 'sta', eap_type: 'fast', encryption: 'wpa2' });
					o.depends({ mode: 'sta', eap_type: 'fast', encryption: 'wpa' });
					o.depends({ mode: 'sta', eap_type: 'peap', encryption: 'wpa2' });
					o.depends({ mode: 'sta', eap_type: 'peap', encryption: 'wpa' });
					o.depends({ mode: 'sta', eap_type: 'ttls', encryption: 'wpa2' });
					o.depends({ mode: 'sta', eap_type: 'ttls', encryption: 'wpa' });
					o.depends({ mode: 'sta-wds', eap_type: 'fast', encryption: 'wpa2' });
					o.depends({ mode: 'sta-wds', eap_type: 'fast', encryption: 'wpa' });
					o.depends({ mode: 'sta-wds', eap_type: 'peap', encryption: 'wpa2' });
					o.depends({ mode: 'sta-wds', eap_type: 'peap', encryption: 'wpa' });
					o.depends({ mode: 'sta-wds', eap_type: 'ttls', encryption: 'wpa2' });
					o.depends({ mode: 'sta-wds', eap_type: 'ttls', encryption: 'wpa' });
					o.password = true;


					if (hwtype == 'mac80211') {
						// ieee802.11w options
						if (L.hasSystemFeature('hostapd', '11w')) {
							o = ss.taboption('encryption', form.ListValue, 'ieee80211w', _('802.11w Management Frame Protection'), _("Requires the 'full' version of wpad/hostapd and support from the wifi driver <br />(as of Jan 2019: ath9k, ath10k, mwlwifi and mt76)"));
							o.value('', _('Disabled'));
							o.value('1', _('Optional'));
							o.value('2', _('Required'));
							o.depends({ mode: 'ap', encryption: 'wpa2' });
							o.depends({ mode: 'ap-wds', encryption: 'wpa2' });
							o.depends({ mode: 'ap', encryption: 'psk2' });
							o.depends({ mode: 'ap', encryption: 'psk-mixed' });
							o.depends({ mode: 'ap', encryption: 'sae' });
							o.depends({ mode: 'ap', encryption: 'sae-mixed' });
							o.depends({ mode: 'ap', encryption: 'owe' });
							o.depends({ mode: 'ap-wds', encryption: 'psk2' });
							o.depends({ mode: 'ap-wds', encryption: 'psk-mixed' });
							o.depends({ mode: 'ap-wds', encryption: 'sae' });
							o.depends({ mode: 'ap-wds', encryption: 'sae-mixed' });
							o.depends({ mode: 'ap-wds', encryption: 'owe' });
							o.depends({ mode: 'sta', encryption: 'wpa2' });
							o.depends({ mode: 'sta-wds', encryption: 'wpa2' });
							o.depends({ mode: 'sta', encryption: 'psk2' });
							o.depends({ mode: 'sta', encryption: 'psk-mixed' });
							o.depends({ mode: 'sta', encryption: 'sae' });
							o.depends({ mode: 'sta', encryption: 'sae-mixed' });
							o.depends({ mode: 'sta', encryption: 'owe' });
							o.depends({ mode: 'sta-wds', encryption: 'psk2' });
							o.depends({ mode: 'sta-wds', encryption: 'psk-mixed' });
							o.depends({ mode: 'sta-wds', encryption: 'sae' });
							o.depends({ mode: 'sta-wds', encryption: 'sae-mixed' });
							o.depends({ mode: 'sta-wds', encryption: 'owe' });
							o.defaults = {
								'2': [{ encryption: 'sae' }, { encryption: 'owe' }],
								'1': [{ encryption: 'sae-mixed'}],
								'':  []
							};

							o = ss.taboption('encryption', form.Value, 'ieee80211w_max_timeout', _('802.11w maximum timeout'), _('802.11w Association SA Query maximum timeout'));
							o.depends('ieee80211w', '1');
							o.depends('ieee80211w', '2');
							o.datatype = 'uinteger';
							o.placeholder = '1000';
							o.rmempty = true;

							o = ss.taboption('encryption', form.Value, 'ieee80211w_retry_timeout', _('802.11w retry timeout'), _('802.11w Association SA Query retry timeout'));
							o.depends('ieee80211w', '1');
							o.depends('ieee80211w', '2');
							o.datatype = 'uinteger';
							o.placeholder = '201';
							o.rmempty = true;
						};

						o = ss.taboption('encryption', form.Flag, 'wpa_disable_eapol_key_retries', _('Enable key reinstallation (KRACK) countermeasures'), _('Complicates key reinstallation attacks on the client side by disabling retransmission of EAPOL-Key frames that are used to install keys. This workaround might cause interoperability issues and reduced robustness of key negotiation especially in environments with heavy traffic load.'));
						o.depends({ mode: 'ap', encryption: 'wpa2' });
						o.depends({ mode: 'ap', encryption: 'psk2' });
						o.depends({ mode: 'ap', encryption: 'psk-mixed' });
						o.depends({ mode: 'ap', encryption: 'sae' });
						o.depends({ mode: 'ap', encryption: 'sae-mixed' });
						o.depends({ mode: 'ap-wds', encryption: 'wpa2' });
						o.depends({ mode: 'ap-wds', encryption: 'psk2' });
						o.depends({ mode: 'ap-wds', encryption: 'psk-mixed' });
						o.depends({ mode: 'ap-wds', encryption: 'sae' });
						o.depends({ mode: 'ap-wds', encryption: 'sae-mixed' });

						if (L.hasSystemFeature('hostapd', 'cli') && L.hasSystemFeature('wpasupplicant')) {
							o = ss.taboption('encryption', form.Flag, 'wps_pushbutton', _('Enable WPS pushbutton, requires WPA(2)-PSK/WPA3-SAE'))
							o.enabled = '1';
							o.disabled = '0';
							o.default = o.disabled;
							o.depends('encryption', 'psk');
							o.depends('encryption', 'psk2');
							o.depends('encryption', 'psk-mixed');
							o.depends('encryption', 'sae');
							o.depends('encryption', 'sae-mixed');
						}
					}
				}
			});
		};

		s.handleRemove = function(section_id, ev) {
			document.querySelector('.cbi-section-table-row[data-sid="%s"]'.format(section_id)).style.opacity = 0.5;
			return form.TypedSection.prototype.handleRemove.apply(this, [section_id, ev]);
		};

		s.handleScan = function(radioDev, ev) {
			var table = E('div', { 'class': 'table' }, [
				E('div', { 'class': 'tr table-titles' }, [
					E('div', { 'class': 'th col-2 middle center' }, _('Signal')),
					E('div', { 'class': 'th col-4 middle left' }, _('SSID')),
					E('div', { 'class': 'th col-2 middle center hide-xs' }, _('Channel')),
					E('div', { 'class': 'th col-2 middle left hide-xs' }, _('Mode')),
					E('div', { 'class': 'th col-3 middle left hide-xs' }, _('BSSID')),
					E('div', { 'class': 'th col-3 middle left' }, _('Encryption')),
					E('div', { 'class': 'th cbi-section-actions right' }, ' '),
				])
			]);

			var stop = E('button', {
				'class': 'btn',
				'click': L.bind(this.handleScanStartStop, this),
				'style': 'display:none',
				'data-state': 'stop'
			}, _('Stop refresh'));

			cbi_update_table(table, [], E('em', { class: 'spinning' }, _('Starting wireless scan...')));

			var md = ui.showModal(_('Join Network: Wireless Scan'), [
				table,
				E('div', { 'class': 'right' }, [
					stop,
					' ',
					E('button', {
						'class': 'btn',
						'click': L.bind(this.handleScanAbort, this)
					}, _('Dismiss'))
				])
			]);

			md.style.maxWidth = '90%';
			md.style.maxHeight = 'none';

			this.pollFn = L.bind(this.handleScanRefresh, this, radioDev, {}, table, stop);

			L.Poll.add(this.pollFn);
			L.Poll.start();
		};

		s.handleScanRefresh = function(radioDev, scanCache, table, stop) {
			return radioDev.getScanList().then(L.bind(function(results) {
				var rows = [];

				for (var i = 0; i < results.length; i++)
					scanCache[results[i].bssid] = results[i];

				for (var k in scanCache)
					if (scanCache[k].stale)
						results.push(scanCache[k]);

				results.sort(function(a, b) {
					var diff = (b.quality - a.quality) || (a.channel - b.channel);

					if (diff)
						return diff;

					if (a.ssid < b.ssid)
						return -1;
					else if (a.ssid > b.ssid)
						return 1;

					if (a.bssid < b.bssid)
						return -1;
					else if (a.bssid > b.bssid)
						return 1;
				});

				for (var i = 0; i < results.length; i++) {
					var res = results[i],
					    qv = res.quality || 0,
					    qm = res.quality_max || 0,
					    q = (qv > 0 && qm > 0) ? Math.floor((100 / qm) * qv) : 0,
					    s = res.stale ? 'opacity:0.5' : '';

					rows.push([
						E('span', { 'style': s }, render_signal_badge(q, res.signal, res.noise)),
						E('span', { 'style': s }, (res.ssid != null) ? '%h'.format(res.ssid) : E('em', _('hidden'))),
						E('span', { 'style': s }, '%d'.format(res.channel)),
						E('span', { 'style': s }, '%h'.format(res.mode)),
						E('span', { 'style': s }, '%h'.format(res.bssid)),
						E('span', { 'style': s }, '%h'.format(network.formatWifiEncryption(res.encryption))),
						E('div', { 'class': 'right' }, E('button', {
							'class': 'cbi-button cbi-button-action important',
							'click': L.bind(this.handleJoin, this, radioDev, res)
						}, _('Join Network')))
					]);

					res.stale = true;
				}

				cbi_update_table(table, rows);

				stop.disabled = false;
				stop.style.display = '';
				stop.classList.remove('spinning');
			}, this));
		};

		s.handleScanStartStop = function(ev) {
			var btn = ev.currentTarget;

			if (btn.getAttribute('data-state') == 'stop') {
				L.Poll.remove(this.pollFn);
				btn.firstChild.data = _('Start refresh');
				btn.setAttribute('data-state', 'start');
			}
			else {
				L.Poll.add(this.pollFn);
				btn.firstChild.data = _('Stop refresh');
				btn.setAttribute('data-state', 'stop');
				btn.classList.add('spinning');
				btn.disabled = true;
			}
		};

		s.handleScanAbort = function(ev) {
			var md = L.dom.parent(ev.target, 'div[aria-modal="true"]');
			if (md) {
				md.style.maxWidth = '';
				md.style.maxHeight = '';
			}

			ui.hideModal();
			L.Poll.remove(this.pollFn);

			this.pollFn = null;
		};

		s.handleJoinConfirm = function(radioDev, bss, form, ev) {
			var nameopt = L.toArray(form.lookupOption('name', '_new_'))[0],
			    passopt = L.toArray(form.lookupOption('password', '_new_'))[0],
			    bssidopt = L.toArray(form.lookupOption('bssid', '_new_'))[0],
			    zoneopt = L.toArray(form.lookupOption('zone', '_new_'))[0],
			    replopt = L.toArray(form.lookupOption('replace', '_new_'))[0],
			    nameval = (nameopt && nameopt.isValid('_new_')) ? nameopt.formvalue('_new_') : null,
			    passval = (passopt && passopt.isValid('_new_')) ? passopt.formvalue('_new_') : null,
			    bssidval = (bssidopt && bssidopt.isValid('_new_')) ? bssidopt.formvalue('_new_') : null,
			    zoneval = zoneopt ? zoneopt.formvalue('_new_') : null,
			    enc = L.isObject(bss.encryption) ? bss.encryption : null,
			    is_wep = (enc && Array.isArray(enc.wep)),
			    is_psk = (enc && Array.isArray(enc.wpa) && L.toArray(enc.authentication).filter(function(a) { return a == 'psk' })),
			    is_sae = (enc && Array.isArray(enc.wpa) && L.toArray(enc.authentication).filter(function(a) { return a == 'sae' }));

			if (nameval == null || (passopt && passval == null))
				return;

			var section_id = null;

			return this.map.save(function() {
				var wifi_sections = uci.sections('wireless', 'wifi-iface');

				if (replopt.formvalue('_new_') == '1') {
					for (var i = 0; i < wifi_sections.length; i++)
						if (wifi_sections[i].device == radioDev.getName())
							uci.remove('wireless', wifi_sections[i]['.name']);
				}

				if (uci.get('wireless', radioDev.getName(), 'disabled') == '1') {
					for (var i = 0; i < wifi_sections.length; i++)
						if (wifi_sections[i].device == radioDev.getName())
							uci.set('wireless', wifi_sections[i]['.name'], 'disabled', '1');

					uci.unset('wireless', radioDev.getName(), 'disabled');
				}

				section_id = next_free_sid(wifi_sections.length);

				uci.add('wireless', 'wifi-iface', section_id);
				uci.set('wireless', section_id, 'device', radioDev.getName());
				uci.set('wireless', section_id, 'mode', (bss.mode == 'Ad-Hoc') ? 'adhoc' : 'sta');
				uci.set('wireless', section_id, 'network', nameval);

				if (bss.ssid != null) {
					uci.set('wireless', section_id, 'ssid', bss.ssid);

					if (bssidval == '1')
						uci.set('wireless', section_id, 'bssid', bss.bssid);
				}
				else if (bss.bssid != null) {
					uci.set('wireless', section_id, 'bssid', bss.bssid);
				}

				if (is_sae) {
					uci.set('wireless', section_id, 'encryption', 'sae');
					uci.set('wireless', section_id, 'key', passval);
				}
				else if (is_psk) {
					for (var i = enc.wpa.length - 1; i >= 0; i--) {
						if (enc.wpa[i] == 2) {
							uci.set('wireless', section_id, 'encryption', 'psk2');
							break;
						}
						else if (enc.wpa[i] == 1) {
							uci.set('wireless', section_id, 'encryption', 'psk');
							break;
						}
					}

					uci.set('wireless', section_id, 'key', passval);
				}
				else if (is_wep) {
					uci.set('wireless', section_id, 'encryption', 'wep-open');
					uci.set('wireless', section_id, 'key', '1');
					uci.set('wireless', section_id, 'key1', passval);
				}
				else {
					uci.set('wireless', section_id, 'encryption', 'none');
				}

				return network.addNetwork(nameval, { proto: 'dhcp' }).then(function(net) {
					firewall.deleteNetwork(net.getName());

					var zonePromise = zoneval
						? firewall.getZone(zoneval).then(function(zone) { return zone || firewall.addZone(zoneval) })
						: Promise.resolve();

					return zonePromise.then(function(zone) {
						if (zone)
							zone.addNetwork(net.getName());
					});
				});
			}).then(L.bind(function() {
				return this.renderMoreOptionsModal(section_id);
			}, this));
		};

		s.handleJoin = function(radioDev, bss, ev) {
			this.handleScanAbort(ev);

			var m2 = new form.Map('wireless'),
			    s2 = m2.section(form.NamedSection, '_new_'),
			    enc = L.isObject(bss.encryption) ? bss.encryption : null,
			    is_wep = (enc && Array.isArray(enc.wep)),
			    is_psk = (enc && Array.isArray(enc.wpa) && L.toArray(enc.authentication).filter(function(a) { return a == 'psk' || a == 'sae' })),
			    replace, passphrase, name, bssid, zone;

			var nameUsed = function(name) {
				var s = uci.get('network', name);
				if (s != null && s['.type'] != 'interface')
					return true;

				var net = (s != null) ? network.instantiateNetwork(name) : null;
				return (net != null && !net.isEmpty());
			};

			s2.render = function() {
				return Promise.all([
					{},
					this.renderUCISection('_new_')
				]).then(this.renderContents.bind(this));
			};

			replace = s2.option(form.Flag, 'replace', _('Replace wireless configuration'), _('Check this option to delete the existing networks from this radio.'));

			name = s2.option(form.Value, 'name', _('Name of the new network'), _('The allowed characters are: <code>A-Z</code>, <code>a-z</code>, <code>0-9</code> and <code>_</code>'));
			name.datatype = 'uciname';
			name.default = 'wwan';
			name.rmempty = false;
			name.validate = function(section_id, value) {
				if (nameUsed(value))
					return _('The network name is already used');

				return true;
			};

			for (var i = 2; nameUsed(name.default); i++)
				name.default = 'wwan%d'.format(i);

			if (is_wep || is_psk) {
				passphrase = s2.option(form.Value, 'password', is_wep ? _('WEP passphrase') : _('WPA passphrase'), _('Specify the secret encryption key here.'));
				passphrase.datatype = is_wep ? 'wepkey' : 'wpakey';
				passphrase.password = true;
				passphrase.rmempty = false;
			}

			if (bss.ssid != null) {
				bssid = s2.option(form.Flag, 'bssid', _('Lock to BSSID'), _('Instead of joining any network with a matching SSID, only connect to the BSSID <code>%h</code>.').format(bss.bssid));
				bssid.default = '0';
			}

			zone = s2.option(widgets.ZoneSelect, 'zone', _('Create / Assign firewall-zone'), _('Choose the firewall zone you want to assign to this interface. Select <em>unspecified</em> to remove the interface from the associated zone or fill out the <em>create</em> field to define a new zone and attach the interface to it.'));
			zone.default = 'wan';

			return m2.render().then(L.bind(function(nodes) {
				ui.showModal(_('Joining Network: %q').replace(/%q/, '"%h"'.format(bss.ssid)), [
					nodes,
					E('div', { 'class': 'right' }, [
						E('button', {
							'class': 'btn',
							'click': ui.hideModal
						}, _('Cancel')), ' ',
						E('button', {
							'class': 'cbi-button cbi-button-positive important',
							'click': ui.createHandlerFn(this, 'handleJoinConfirm', radioDev, bss, m2)
						}, _('Submit'))
					])
				], 'cbi-modal').querySelector('[id="%s"] input[class][type]'.format((passphrase || name).cbid('_new_'))).focus();
			}, this));
		};

		s.handleAdd = function(radioDev, ev) {
			var section_id = next_free_sid(uci.sections('wireless', 'wifi-iface').length);

			uci.unset('wireless', radioDev.getName(), 'disabled');

			uci.add('wireless', 'wifi-iface', section_id);
			uci.set('wireless', section_id, 'device', radioDev.getName());
			uci.set('wireless', section_id, 'mode', 'ap');
			uci.set('wireless', section_id, 'ssid', 'OpenWrt');
			uci.set('wireless', section_id, 'encryption', 'none');

			this.addedSection = section_id;
			return this.renderMoreOptionsModal(section_id);
		};

		o = s.option(form.DummyValue, '_badge');
		o.modalonly = false;
		o.textvalue = function(section_id) {
			var inst = this.section.lookupRadioOrNetwork(section_id),
			    node = E('div', { 'class': 'center' });

			if (inst.getWifiNetworks)
				node.appendChild(render_radio_badge(inst));
			else
				node.appendChild(render_network_badge(inst));

			return node;
		};

		o = s.option(form.DummyValue, '_stat');
		o.modalonly = false;
		o.textvalue = function(section_id) {
			var inst = this.section.lookupRadioOrNetwork(section_id);

			if (inst.getWifiNetworks)
				return render_radio_status(inst, this.section.wifis.filter(function(e) {
					return (e.getWifiDeviceName() == inst.getName());
				}));
			else
				return render_network_status(inst);
		};

		return m.render().then(L.bind(function(m, nodes) {
			L.Poll.add(L.bind(function() {
				var section_ids = m.children[0].cfgsections(),
				    tasks = [ network.getHostHints(), network.getWifiDevices() ];

				for (var i = 0; i < section_ids.length; i++) {
					var row = nodes.querySelector('.cbi-section-table-row[data-sid="%s"]'.format(section_ids[i])),
					    dsc = row.querySelector('[data-name="_stat"] > div'),
					    btns = row.querySelectorAll('.cbi-section-actions button');

					if (dsc.getAttribute('restart') == '') {
						dsc.setAttribute('restart', '1');
						tasks.push(fs.exec('/sbin/wifi', ['up', section_ids[i]]).catch(function(e) {
							ui.addNotification(null, E('p', e.message));
						}));
					}
					else if (dsc.getAttribute('restart') == '1') {
						dsc.removeAttribute('restart');
						btns[0].classList.remove('spinning');
						btns[0].disabled = false;
					}
				}

				return Promise.all(tasks)
					.then(L.bind(function(hosts_radios) {
						var tasks = [];

						for (var i = 0; i < hosts_radios[1].length; i++)
							tasks.push(hosts_radios[1][i].getWifiNetworks());

						return Promise.all(tasks).then(function(data) {
							hosts_radios[2] = [];

							for (var i = 0; i < data.length; i++)
								hosts_radios[2].push.apply(hosts_radios[2], data[i]);

							return hosts_radios;
						});
					}, network))
					.then(L.bind(function(hosts_radios_wifis) {
						var tasks = [];

						for (var i = 0; i < hosts_radios_wifis[2].length; i++)
							tasks.push(hosts_radios_wifis[2][i].getAssocList());

						return Promise.all(tasks).then(function(data) {
							hosts_radios_wifis[3] = [];

							for (var i = 0; i < data.length; i++) {
								var wifiNetwork = hosts_radios_wifis[2][i],
								    radioDev = hosts_radios_wifis[1].filter(function(d) { return d.getName() == wifiNetwork.getWifiDeviceName() })[0];

								for (var j = 0; j < data[i].length; j++)
									hosts_radios_wifis[3].push(Object.assign({ radio: radioDev, network: wifiNetwork }, data[i][j]));
							}

							return hosts_radios_wifis;
						});
					}, network))
					.then(L.bind(this.poll_status, this, nodes));
			}, this), 5);

			var table = E('div', { 'class': 'table', 'id': 'wifi_assoclist_table' }, [
				E('div', { 'class': 'tr table-titles' }, [
					E('div', { 'class': 'th nowrap' }, _('Network')),
					E('div', { 'class': 'th hide-xs' }, _('MAC-Address')),
					E('div', { 'class': 'th' }, _('Host')),
					E('div', { 'class': 'th nowrap' }, _('Signal / Noise')),
					E('div', { 'class': 'th nowrap' }, _('RX Rate / TX Rate'))
				])
			]);

			cbi_update_table(table, [], E('em', { 'class': 'spinning' }, _('Collecting data...')))

			return E([ nodes, E('h3', _('Associated Stations')), table ]);
		}, this, m));
	}
});
