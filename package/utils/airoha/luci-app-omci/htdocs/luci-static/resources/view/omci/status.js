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

function formatAlarms(value) {
	if (value == null)
		return '-';
	return value === 0 ? _('None') : '0x' + value.toString(16).padStart(8, '0');
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

function statusValues(status, serial, vendorId, equipmentId, oltObject) {
	var olt = oltIdentity(oltObject);
	return {
		state: 'O' + status.state,
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
		policy: status.permissive ? _('Permissive') : _('Strict'),
		fake_omci: status.fake_omci ? _('Enabled') : _('Disabled'),
		mib_sync: status.mib_sync,
		mib_objects: status.mib_objects,
		rx_packets: status.rx_packets,
		rx_dropped: status.rx_dropped,
		tx_packets: status.tx_packets,
		tx_errors: status.tx_errors,
		responses: status.responses,
		duplicates: status.duplicates,
		unsupported: status.unsupported,
		fake_responses: status.fake_responses,
		serial: serial || '-',
		vendor_id: vendorId || '-',
		equipment_id: equipmentId || '-',
		olt_vendor: olt.vendor,
		olt_model: olt.model,
		olt_version: olt.version
	};
}

function renderStatusTable(status, serial, vendorId, equipmentId, oltObject) {
	var values = statusValues(status, serial, vendorId, equipmentId, oltObject);

	return E('table', { 'class': 'table', 'id': 'omci-status-table' }, [
		row(_('GPON state'), values.state, 'state'),
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
		row(_('Policy'), values.policy, 'policy'),
		row(_('FAKE OMCI'), values.fake_omci, 'fake_omci'),
		row(_('MIB sync counter'), values.mib_sync, 'mib_sync'),
		row(_('MIB objects'), values.mib_objects, 'mib_objects'),
		row(_('RX packets'), values.rx_packets, 'rx_packets'),
		row(_('RX dropped'), values.rx_dropped, 'rx_dropped'),
		row(_('TX packets'), values.tx_packets, 'tx_packets'),
		row(_('TX errors'), values.tx_errors, 'tx_errors'),
		row(_('Agent responses'), values.responses, 'responses'),
		row(_('Duplicate requests'), values.duplicates, 'duplicates'),
		row(_('Unsupported requests'), values.unsupported, 'unsupported'),
		row(_('FAKE OMCI responses'), values.fake_responses, 'fake_responses'),
		row(_('Serial number'), values.serial, 'serial'),
		row(_('Vendor ID'), values.vendor_id, 'vendor_id'),
		row(_('Equipment ID'), values.equipment_id, 'equipment_id'),
		row(_('OLT vendor ID'), values.olt_vendor, 'olt_vendor'),
		row(_('OLT model'), values.olt_model, 'olt_model'),
		row(_('OLT version'), values.olt_version, 'olt_version')
	]);
}

function updateStatusTable(status, serial, vendorId, equipmentId, oltObject) {
	var values = statusValues(status, serial, vendorId, equipmentId, oltObject);

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
		var serial = data.config.serial;
		var vendorId = data.config['vendor-id'];
		var equipmentId = data.config['equipment-id'];
		var table = renderStatusTable(status, serial, vendorId, equipmentId,
			data.olt);

		poll.add(function() {
			return Promise.all([
				L.resolveDefault(callStatus(data.device), null),
				L.resolveDefault(callMibGet(data.device, 131, 0), null)
			]).then(function(result) {
				if (result[0] == null)
					return;

				updateStatusTable(result[0], serial, vendorId, equipmentId,
					result[1]);
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
