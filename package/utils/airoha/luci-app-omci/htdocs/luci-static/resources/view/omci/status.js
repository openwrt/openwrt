'use strict';
'require view';
'require poll';
'require rpc';
'require uci';

var callStatus = rpc.declare({
	object: 'omci.agent',
	method: 'status',
	params: [ 'device' ],
	expect: { status: {} },
	reject: true
});

var callConfigList = rpc.declare({
	object: 'omci.agent',
	method: 'config_list',
	params: [ 'device' ],
	expect: { config: {} },
	reject: true
});

var callMibGet = rpc.declare({
	object: 'omci.agent',
	method: 'mib_get',
	params: [ 'device', 'class_id', 'entity_id' ],
	expect: { object: {} },
	reject: true
});

function row(label, value, id) {
	var attrs = { 'class': 'td left' };

	if (id != null)
		attrs.id = 'omci-status-' + id;

	return E('tr', {}, [ E('td', { 'class': 'td left' }, label),
		E('td', attrs, String(value)) ]);
}

function formatFec(value) {
	if (value === 2)
		return _('Up');
	if (value === 1)
		return _('Down');
	return _('Unavailable');
}

function formatTemperature(value) {
	return value == null ? '-' : (value / 1000).toFixed(3) + ' °C';
}

function formatVoltage(value) {
	return value == null ? '-' : (value / 1000000).toFixed(3) + ' V';
}

function formatBias(value) {
	return value == null ? '-' : (value / 1000).toFixed(3) + ' mA';
}

function formatPower(value) {
	var dbm;

	if (value == null)
		return '-';
	if (value <= 0)
		return '0.000 µW';

	dbm = 10 * Math.log10(value / 1000000);
	return dbm.toFixed(2) + ' dBm (' + (value / 1000).toFixed(3) + ' µW)';
}

function formatCounter(value) {
	var number = Number(value);

	if (!Number.isFinite(number))
		return '-';

	if (number < 1024)
		return number + ' B';
	if (number < 1024 * 1024)
		return (number / 1024).toFixed(2) + ' KiB';
	if (number < 1024 * 1024 * 1024)
		return (number / (1024 * 1024)).toFixed(2) + ' MiB';

	return (number / (1024 * 1024 * 1024)).toFixed(2) + ' GiB';
}

function formatAlarms(value) {
	if (value == null)
		return '-';
	return value === 0 ? _('None') : '0x' + value.toString(16).padStart(8, '0');
}

function formatByte(value) {
	var number;

	if (value == null || value === '')
		return '-';
	number = Number(value);
	if (!Number.isFinite(number))
		return '-';
	return '0x' + number.toString(16).padStart(2, '0') + ' (' + number + ')';
}

function formatCapabilities(value) {
	var flags = Number(value) || 0;
	var names = [
		[ 1 << 0, _('Hardware MIC') ],
		[ 1 << 1, _('In-kernel baseline agent') ],
		[ 1 << 2, _('Optical/FEC telemetry') ]
	].filter(function(entry) {
		return (flags & entry[0]) !== 0;
	}).map(function(entry) {
		return entry[1];
	});

	return '0x' + flags.toString(16).padStart(8, '0') +
		(names.length ? ' — ' + names.join(', ') : ' — ' + _('None'));
}

function profileName(value, forced) {
	var names = {
		0: forced ? _('None') : _('Unspecified'),
		1: _('Generic'),
		2: _('Automatic detection'),
		3: _('Nokia / Alcatel-Lucent'),
		4: _('DASAN'),
		5: _('Huawei'),
		6: _('FiberHome'),
		7: _('ZTE')
	};

	return names[value] || _('Unknown') + ' (' + value + ')';
}

function formatQuirks(value) {
	var quirks = [
		[ 1 << 0, _('Allow Set as Create') ],
		[ 1 << 1, _('Fake unsupported success') ],
		[ 1 << 2, _('Ignore unsupported UNI') ],
		[ 1 << 3, _('Full UNI entity ID') ],
		[ 1 << 4, _('Sanitize OLT version') ],
		[ 1 << 5, _('ZTE VLAN tag mode') ],
		[ 1 << 6, _('Vendor-specific MEs') ],
		[ 1 << 7, _('DASAN multicast ANI') ]
	];
	var active = quirks.filter(function(entry) {
		return (value & entry[0]) !== 0;
	}).map(function(entry) {
		return entry[1];
	});

	return active.length ? active.join(', ') : _('None');
}

function cleanMibString(value) {
	return String(value || '').replace(/\0.*$/, '').trim();
}

function oltIdentity(object) {
	var olt = object && object.olt_g || {};
	var vendor = cleanMibString(olt.vendor_id);
	var equipment = cleanMibString(olt.equipment_id);
	var version = cleanMibString(olt.version);
	var model = equipment;

	if (!model)
		model = [ vendor, version ].filter(function(value) {
			return value !== '';
		}).join(' ');

	return {
		vendor: vendor || '-',
		model: model || '-',
		version: version || '-'
	};
}

function identityFromConfig(config) {
	config = config || {};

	return {
		serial: config.serial,
		vendor_id: config['vendor-id'],
		hardware_version: config['hardware-version'],
		software_version_0: config['software-version-0'],
		software_version_1: config['software-version-1'],
		equipment_id: config['equipment-id'],
		omcc_version: config['omcc-version']
	};
}

function statusValues(status, identity, oltObject) {
	var olt = oltIdentity(oltObject);
	return {
		state: 'O' + status.state,
		uapi_version: status.uapi_version == null ? '-' : status.uapi_version,
		client_uapi_version: status.client_uapi_version == null ? '-' : status.client_uapi_version,
		uapi_compatible: status.uapi_compatible ? _('Compatible') : _('Mismatch'),
		capabilities: formatCapabilities(status.capabilities),
		channel: status.channel_up ? _('Up') : _('Down'),
		fec_downstream: formatFec(status.fec_downstream),
		fec_upstream: formatFec(status.fec_upstream),
		bosa_temperature: formatTemperature(status.bosa_temperature_mc),
		bosa_voltage: formatVoltage(status.bosa_voltage_uv),
		bosa_bias: formatBias(status.bosa_bias_ua),
		bosa_tx_power: formatPower(status.bosa_tx_power_nw),
		bosa_rx_power: formatPower(status.bosa_rx_power_nw),
		bosa_alarms: formatAlarms(status.bosa_alarms),
		onu_id: status.onu_id,
		gem_port: status.gem_port,
		agent_enabled: status.agent_enabled ? _('Enabled') : _('Disabled'),
		agent_operational: status.agent_operational ? _('Ready') : _('Not ready'),
		policy: status.permissive ? _('Permissive') : _('Strict'),
		fake_omci: status.fake_omci ? _('Enabled') : _('Disabled'),
		dying_gasp: status.dying_gasp ? _('Enabled') : _('Disabled'),
		profile_configured: profileName(status.olt_profile_configured, false),
		profile_effective: profileName(status.olt_profile_effective, false),
		profile_forced: profileName(status.olt_profile_forced, true),
		profile_quirks: formatQuirks(status.olt_profile_quirks || 0),
		mib_sync: status.mib_sync,
		mib_objects: status.mib_objects,
		rx_packets: status.rx_packets,
		rx_bytes: formatCounter(status.rx_bytes),
		rx_dropped: status.rx_dropped,
		tx_packets: status.tx_packets,
		tx_bytes: formatCounter(status.tx_bytes),
		tx_errors: status.tx_errors,
		responses: status.responses,
		duplicates: status.duplicates,
		unsupported: status.unsupported,
		fake_responses: status.fake_responses,
		serial: identity.serial || '-',
		vendor_id: identity.vendor_id || '-',
		hardware_version: identity.hardware_version || '-',
		software_version_0: identity.software_version_0 || '-',
		software_version_1: identity.software_version_1 || '-',
		equipment_id: identity.equipment_id || '-',
		omcc_version: formatByte(identity.omcc_version),
		olt_vendor: olt.vendor,
		olt_model: olt.model,
		olt_version: olt.version
	};
}

function renderStatusTable(status, identity, oltObject) {
	var values = statusValues(status, identity, oltObject);

	return E('table', { 'class': 'table', 'id': 'omci-status-table' }, [
		row(_('GPON state'), values.state, 'state'),
		row(_('Kernel OMCI UAPI version'), values.uapi_version, 'uapi_version'),
		row(_('Package OMCI UAPI version'), values.client_uapi_version, 'client_uapi_version'),
		row(_('UAPI compatibility'), values.uapi_compatible, 'uapi_compatible'),
		row(_('Driver capabilities'), values.capabilities, 'capabilities'),
		row(_('Channel'), values.channel, 'channel'),
		row(_('Downstream FEC'), values.fec_downstream, 'fec_downstream'),
		row(_('Upstream FEC'), values.fec_upstream, 'fec_upstream'),
		row(_('BOSA temperature'), values.bosa_temperature, 'bosa_temperature'),
		row(_('BOSA supply voltage'), values.bosa_voltage, 'bosa_voltage'),
		row(_('BOSA TX bias'), values.bosa_bias, 'bosa_bias'),
		row(_('BOSA TX power'), values.bosa_tx_power, 'bosa_tx_power'),
		row(_('BOSA RX power'), values.bosa_rx_power, 'bosa_rx_power'),
		row(_('BOSA alarms'), values.bosa_alarms, 'bosa_alarms'),
		row(_('ONU-ID'), values.onu_id, 'onu_id'),
		row(_('OMCC GEM port'), values.gem_port, 'gem_port'),
		row(_('Agent'), values.agent_enabled, 'agent_enabled'),
		row(_('Operational readiness'), values.agent_operational, 'agent_operational'),
		row(_('Policy'), values.policy, 'policy'),
		row(_('FAKE OMCI'), values.fake_omci, 'fake_omci'),
		row(_('Dying-gasp alarm'), values.dying_gasp, 'dying_gasp'),
		row(_('Configured OLT profile'), values.profile_configured, 'profile_configured'),
		row(_('Effective OLT profile'), values.profile_effective, 'profile_effective'),
		row(_('Forced OLT profile'), values.profile_forced, 'profile_forced'),
		row(_('Active interoperability quirks'), values.profile_quirks, 'profile_quirks'),
		row(_('MIB sync counter'), values.mib_sync, 'mib_sync'),
		row(_('MIB objects'), values.mib_objects, 'mib_objects'),
		row(_('RX packets'), values.rx_packets, 'rx_packets'),
		row(_('RX bytes'), values.rx_bytes, 'rx_bytes'),
		row(_('RX dropped'), values.rx_dropped, 'rx_dropped'),
		row(_('TX packets'), values.tx_packets, 'tx_packets'),
		row(_('TX bytes'), values.tx_bytes, 'tx_bytes'),
		row(_('TX errors'), values.tx_errors, 'tx_errors'),
		row(_('Agent responses'), values.responses, 'responses'),
		row(_('Duplicate requests'), values.duplicates, 'duplicates'),
		row(_('Unsupported requests'), values.unsupported, 'unsupported'),
		row(_('FAKE OMCI responses'), values.fake_responses, 'fake_responses'),
		row(_('Serial number'), values.serial, 'serial'),
		row(_('Vendor ID'), values.vendor_id, 'vendor_id'),
		row(_('Hardware version'), values.hardware_version, 'hardware_version'),
		row(_('Software image 0 version'), values.software_version_0, 'software_version_0'),
		row(_('Software image 1 version'), values.software_version_1, 'software_version_1'),
		row(_('Equipment ID'), values.equipment_id, 'equipment_id'),
		row(_('OMCC version'), values.omcc_version, 'omcc_version'),
		row(_('OLT vendor ID'), values.olt_vendor, 'olt_vendor'),
		row(_('OLT model'), values.olt_model, 'olt_model'),
		row(_('OLT version'), values.olt_version, 'olt_version')
	]);
}

function updateStatusTable(status, identity, oltObject) {
	var values = statusValues(status, identity, oltObject);

	for (var id in values) {
		var elem = document.getElementById('omci-status-' + id);

		if (elem != null)
			elem.textContent = String(values[id]);
	}
}

return view.extend({
	load: function() {
		return uci.load('omci').then(function() {
			var device = Number(uci.get('omci', 'main', 'device')) || 0;

			return Promise.all([
				callStatus(device),
				callConfigList(device),
				L.resolveDefault(callMibGet(device, 131, 0), null)
			]).then(function(data) {
				return {
					device: device,
					status: data[0],
					config: data[1],
					olt: data[2]
				};
			});
		});
	},

	render: function(data) {
		var status = data.status;
		var identity = identityFromConfig(data.config);
		var table = renderStatusTable(status, identity, data.olt);

		poll.add(function() {
			return Promise.all([
				L.resolveDefault(callStatus(data.device), null),
				L.resolveDefault(callConfigList(data.device), null),
				L.resolveDefault(callMibGet(data.device, 131, 0), null)
			]).then(function(result) {
				if (result[0] == null)
					return;

				if (result[1] != null)
					identity = identityFromConfig(result[1]);

				updateStatusTable(result[0], identity, result[2]);
			});
		}, 2);

		return E([], [
			E('h2', {}, _('OMCI Agent Status')),
			E('div', { 'class': 'cbi-map-descr' },
				_('The kernel agent handles normal OLT requests; status is read through the OMCI RPC service.')),
			table
		]);
	},

	handleSaveApply: null,
	handleSave: null,
	handleReset: null
});
