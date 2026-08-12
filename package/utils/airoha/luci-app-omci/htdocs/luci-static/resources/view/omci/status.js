'use strict';
'require view';
'require poll';
'require dom';
'require omci.rpc as omci';

function text(value, fallback) {
	return value == null || value === '' ? (fallback || '-') : String(value);
}

function yesno(value) {
	return value ? _('Yes') : _('No');
}

function hex(value, width) {
	if (value == null)
		return '-';
	return '0x' + Number(value).toString(16).padStart(width || 2, '0');
}

function state(value) {
	return value == null ? '-' : 'O' + value;
}

function power(value) {
	var n = Number(value);
	if (!Number.isFinite(n) || n <= 0)
		return '-';
	return '%s nW (%s dBm)'.format(n, (10 * Math.log10(n / 1000000)).toFixed(2));
}

function temperature(value) {
	var n = Number(value);
	return Number.isFinite(n) ? (n / 1000).toFixed(3) + ' °C' : '-';
}

function row(label, value, id) {
	return E('tr', {}, [
		E('td', { 'class': 'td left', 'width': '42%' }, label),
		E('td', { 'class': 'td left', 'id': 'omci-status-' + id }, text(value))
	]);
}

function section(title, rows) {
	return E('div', { 'class': 'cbi-section' }, [
		E('h3', {}, title),
		E('table', { 'class': 'table' }, rows)
	]);
}

function summary(status) {
	var cards = [
		[ _('GPON state'), state(status.state) ],
		[ _('OMCI agent'), status.agent_operational ? _('Operational') : _('Not operational') ],
		[ _('OLT profile'), text(status.olt_profile_effective_name) ],
		[ _('MIB objects'), text(status.mib_objects, '0') ]
	];

	return E('div', { 'class': 'omci-summary' }, cards.map(function(card) {
		return E('div', { 'class': 'omci-card' }, [
			E('div', { 'class': 'omci-card-label' }, card[0]),
			E('div', { 'class': 'omci-card-value' }, card[1])
		]);
	}));
}

function injectStyle() {
	if (document.getElementById('omci-style'))
		return;

	document.head.appendChild(E('style', { 'id': 'omci-style' }, [
		'.omci-summary{display:grid;grid-template-columns:repeat(auto-fit,minmax(180px,1fr));gap:10px;margin:0 0 18px}',
		'.omci-card{border:1px solid var(--border-color-medium,#ccc);border-radius:6px;padding:12px 14px;background:var(--background-color-high,#fff)}',
		'.omci-card-label{font-size:12px;opacity:.72;margin-bottom:5px}',
		'.omci-card-value{font-size:18px;font-weight:600;overflow-wrap:anywhere}',
		'.omci-quirks{font-family:monospace;font-size:12px;overflow-wrap:anywhere}'
	].join('')));
}

function values(status) {
	var identity = status.identity || {};
	var olt = status.olt || {};
	return {
		state: state(status.state),
		ifindex: text(status.ifindex),
		onu_id: text(status.onu_id),
		gem_port: text(status.gem_port),
		channel_up: yesno(status.channel_up),
		agent_enabled: yesno(status.agent_enabled),
		agent_operational: yesno(status.agent_operational),
		permissive: yesno(status.permissive),
		fake_omci: yesno(status.fake_omci),
		dying_gasp: yesno(status.dying_gasp),
		uapi: '%s / client %s (%s)'.format(text(status.uapi_version), text(status.client_uapi_version), status.uapi_compatible ? _('compatible') : _('mismatch')),
		capabilities: hex(status.capabilities, 8),
		profile_configured: text(status.olt_profile_configured_name),
		profile_effective: text(status.olt_profile_effective_name),
		profile_forced: text(status.olt_profile_forced_name),
		quirks: (status.olt_profile_quirk_names || []).join(', ') || _('None'),
		mib_sync: text(status.mib_sync),
		mib_objects: text(status.mib_objects),
		rx_packets: text(status.rx_packets), rx_bytes: text(status.rx_bytes), rx_dropped: text(status.rx_dropped),
		tx_packets: text(status.tx_packets), tx_bytes: text(status.tx_bytes), tx_errors: text(status.tx_errors),
		responses: text(status.responses), duplicates: text(status.duplicates), unsupported: text(status.unsupported),
		fake_responses: text(status.fake_responses),
		serial: text(identity.serial), vendor_id: text(identity['vendor-id']),
		hardware_version: text(identity['hardware-version']), software_version_0: text(identity['software-version-0']),
		software_version_1: text(identity['software-version-1']), equipment_id: text(identity['equipment-id']),
		omcc_version: identity['omcc-version'] == null ? '-' : '%s (%s)'.format(hex(identity['omcc-version'], 2), identity['omcc-version']),
		olt_vendor: text(olt.vendor_id || olt.vendor), olt_equipment: text(olt.equipment_id || olt.model),
		olt_version: text(olt.version),
		fec_downstream: text(status.fec_downstream), fec_upstream: text(status.fec_upstream),
		bosa_temperature: temperature(status.bosa_temperature_mc), bosa_voltage: status.bosa_voltage_uv == null ? '-' : status.bosa_voltage_uv + ' µV',
		bosa_bias: status.bosa_bias_ua == null ? '-' : status.bosa_bias_ua + ' µA',
		bosa_tx_power: power(status.bosa_tx_power_nw), bosa_rx_power: power(status.bosa_rx_power_nw),
		bosa_alarms: hex(status.bosa_alarms, 8)
	};
}

function update(status) {
	var v = values(status);
	Object.keys(v).forEach(function(key) {
		var node = document.getElementById('omci-status-' + key);
		if (node)
			dom.content(node, v[key]);
	});

	var summaryNode = document.getElementById('omci-summary-root');
	if (summaryNode)
		dom.content(summaryNode, summary(status));
}

return view.extend({
	load: function() {
		return omci.status(0);
	},

	render: function(status) {
		injectStyle();
		status = status || {};
		var v = values(status);

		poll.add(function() {
			return omci.status(0).then(function(next) {
				if (next)
					update(next);
			});
		}, 2);

		return E([], [
			E('h2', {}, _('OMCI status')),
			E('p', {}, _('Runtime state reported directly by the Linux OMCI Generic Netlink API through the native ucode binding.')),
			E('div', { 'id': 'omci-summary-root' }, summary(status)),
			section(_('Channel and agent'), [
				row(_('GPON state'), v.state, 'state'), row(_('Interface index'), v.ifindex, 'ifindex'),
				row(_('ONU ID'), v.onu_id, 'onu_id'), row(_('Default GEM port'), v.gem_port, 'gem_port'),
				row(_('OMCI channel up'), v.channel_up, 'channel_up'), row(_('Agent enabled'), v.agent_enabled, 'agent_enabled'),
				row(_('Agent operational'), v.agent_operational, 'agent_operational'), row(_('Permissive mode'), v.permissive, 'permissive'),
				row(_('Fake OMCI'), v.fake_omci, 'fake_omci'), row(_('Dying gasp'), v.dying_gasp, 'dying_gasp'),
				row(_('UAPI'), v.uapi, 'uapi'), row(_('Capabilities'), v.capabilities, 'capabilities')
			]),
			section(_('ONU identity'), [
				row(_('Serial number'), v.serial, 'serial'), row(_('Vendor ID'), v.vendor_id, 'vendor_id'),
				row(_('Hardware version'), v.hardware_version, 'hardware_version'),
				row(_('Software image 0 version'), v.software_version_0, 'software_version_0'),
				row(_('Software image 1 version'), v.software_version_1, 'software_version_1'),
				row(_('Equipment ID'), v.equipment_id, 'equipment_id'), row(_('OMCC version'), v.omcc_version, 'omcc_version')
			]),
			section(_('OLT interoperability'), [
				row(_('Configured profile'), v.profile_configured, 'profile_configured'),
				row(_('Effective profile'), v.profile_effective, 'profile_effective'),
				row(_('Forced profile'), v.profile_forced, 'profile_forced'),
				row(_('Active quirks'), v.quirks, 'quirks'),
				row(_('OLT vendor ID'), v.olt_vendor, 'olt_vendor'), row(_('OLT equipment ID'), v.olt_equipment, 'olt_equipment'),
				row(_('OLT version'), v.olt_version, 'olt_version')
			]),
			section(_('MIB and OMCI traffic'), [
				row(_('MIB data sync'), v.mib_sync, 'mib_sync'), row(_('MIB objects'), v.mib_objects, 'mib_objects'),
				row(_('RX packets'), v.rx_packets, 'rx_packets'), row(_('RX bytes'), v.rx_bytes, 'rx_bytes'),
				row(_('RX dropped'), v.rx_dropped, 'rx_dropped'), row(_('TX packets'), v.tx_packets, 'tx_packets'),
				row(_('TX bytes'), v.tx_bytes, 'tx_bytes'), row(_('TX errors'), v.tx_errors, 'tx_errors'),
				row(_('Agent responses'), v.responses, 'responses'), row(_('Duplicates'), v.duplicates, 'duplicates'),
				row(_('Unsupported requests'), v.unsupported, 'unsupported'), row(_('Fake responses'), v.fake_responses, 'fake_responses')
			]),
			section(_('Optical telemetry'), [
				row(_('Downstream FEC'), v.fec_downstream, 'fec_downstream'), row(_('Upstream FEC'), v.fec_upstream, 'fec_upstream'),
				row(_('BOSA temperature'), v.bosa_temperature, 'bosa_temperature'), row(_('BOSA voltage'), v.bosa_voltage, 'bosa_voltage'),
				row(_('BOSA bias current'), v.bosa_bias, 'bosa_bias'), row(_('TX optical power'), v.bosa_tx_power, 'bosa_tx_power'),
				row(_('RX optical power'), v.bosa_rx_power, 'bosa_rx_power'), row(_('BOSA alarms'), v.bosa_alarms, 'bosa_alarms')
			])
		]);
	},

	handleSaveApply: null,
	handleSave: null,
	handleReset: null
});
