'use strict';
'require view';
'require rpc';
'require poll';
'require dom';

var callStatus = rpc.declare({ object: 'econet-xpon', method: 'status', expect: { } });

function na() { return E('span', { 'style': 'color:#888' }, _('Unavailable')); }
function dash() { return E('span', { 'style': 'color:#888' }, '—'); }

/* SFF-8472 DDM raw -> human units (raw values from /proc/econet_xpon_ddmi) */
function ddmTemp(raw) { return (raw / 256).toFixed(3) + ' °C'; }          /* 1/256 C, signed */
function ddmVolt(raw) { return (raw / 10000).toFixed(3) + ' V'; }              /* 100 uV/LSB */
function ddmBias(raw) { return (raw * 2 / 1000).toFixed(3) + ' mA'; }          /* 2 uA/LSB */
function ddmPower(raw) {                                                        /* 0.1 uW/LSB */
	var uW = raw / 10;
	if (raw <= 0) return '< -40.00 dBm (0 µW)';
	var dbm = 10 * Math.log10(raw) - 40;   /* 10*log10(uW/1000) */
	return dbm.toFixed(2) + ' dBm (' + uW.toFixed(3) + ' µW)';
}

function colored(text, kind) {
	var c = ({ ok: '#2e7d32', warn: '#b26a00', bad: '#c62828' })[kind];
	return c ? E('span', { 'style': 'color:' + c + ';font-weight:600' }, text) : text;
}

function stateCell(s) {
	if (!s || s === 'unknown') return colored(_('unknown'), 'warn');
	if (s === 'Error') return colored('Error', 'warn');
	if (/^O5/.test(s)) return colored(s, 'ok');
	return colored(s, 'warn');
}

function rows(d) {
	d = d || {};
	var agent = (d.driver_loaded && d.omci_daemon) ? colored(_('Enabled'), 'ok') : colored(_('Disabled'), 'bad');

	return [
		[ _('GPON state'),            stateCell(d.gpon_state) ],
		[ _('Channel'),               d.channel === 'Up' ? colored('Up', 'ok') : colored(d.channel || 'Down', 'warn') ],
		[ _('Downstream FEC'),        na() ],
		[ _('Upstream FEC'),          na() ],
		[ _('BOSA temperature'),      d.ddm_present ? ddmTemp(d.ddm_temp_8472)  : na() ],
		[ _('BOSA supply voltage'),   d.ddm_present ? ddmVolt(d.ddm_vcc_8472)   : na() ],
		[ _('BOSA TX bias'),          d.ddm_present ? ddmBias(d.ddm_bias_8472)  : na() ],
		[ _('BOSA TX power'),         d.ddm_present ? ddmPower(d.ddm_txpwr_8472) : na() ],
		[ _('BOSA RX power'),         d.ddm_present ? ddmPower(d.ddm_rxpwr_8472) : na() ],
		[ _('Optical RX link'),       d.opt_rx ? (/DOWN|loss/i.test(d.opt_rx) ? colored(d.opt_rx, 'bad') : colored(d.opt_rx, 'ok')) : na() ],
		[ _('APD bias'),              d.opt_apd ? E('code', {}, d.opt_apd) : dash() ],
		[ _('Loss samples'),          d.opt_loss || dash() ],
		[ _('BOSA alarms'),           E('code', {}, d.bosa_alarms || '0x00000000') ],
		[ _('ONU-ID'),                String(d.onu_id != null ? d.onu_id : '-') ],
		[ _('OMCC GEM port'),         String(d.omcc_gem != null ? d.omcc_gem : '-') ],
		[ _('Agent'),                 agent ],
		[ _('Policy'),                dash() ],
		[ _('FAKE OMCI'),             _('Disabled') ],
		[ _('MIB sync counter'),      na() ],
		[ _('MIB objects'),           na() ],
		[ _('RX packets'),            String(d.rx_packets != null ? d.rx_packets : '-') ],
		[ _('RX dropped'),            String(d.rx_dropped != null ? d.rx_dropped : '-') ],
		[ _('TX packets'),            String(d.tx_packets != null ? d.tx_packets : '-') ],
		[ _('TX errors'),             String(d.tx_errors != null ? d.tx_errors : '-') ],
		[ _('OMCI/OAM monitor'),      String(d.omci_oam_monitor != null ? d.omci_oam_monitor : '-') ],
		[ _('O1 ranging timeouts'),   String(d.to1_timeout_cnt != null ? d.to1_timeout_cnt : '-') ],
		[ _('Loss of Signal'),        d.los ? colored(_('LOS'), 'bad') : colored(_('signal OK'), 'ok') ],
		[ _('WAN interface (ponwan0)'), d.ponwan_state ? (d.ponwan_state === 'up' ? colored('up', 'ok') : d.ponwan_state) : na() ],
		[ _('Agent responses'),       na() ],
		[ _('Duplicate requests'),    na() ],
		[ _('Unsupported requests'),  na() ],
		[ _('FAKE OMCI responses'),   na() ],
		[ _('Serial number'),         E('code', {}, d.serial || '-') ],
		[ _('Vendor ID'),             d.vendor_id || '-' ],
		[ _('Equipment ID'),          d.equipment_id || '-' ],
		[ _('OLT vendor ID'),         dash() ],
		[ _('OLT model'),             dash() ],
		[ _('OLT version'),           dash() ]
	];
}

function renderTable(d) {
	if (!d || !('driver_loaded' in d))
		return E('em', {}, _('Status unavailable — the econet-xpon driver is not loaded or the rpcd backend is missing.'));

	var tbody = rows(d).map(function(r) {
		return E('tr', { 'class': 'tr' }, [
			E('td', { 'class': 'td left', 'width': '33%' }, r[0]),
			E('td', { 'class': 'td left' }, r[1])
		]);
	});
	return E('table', { 'class': 'table' }, tbody);
}

return view.extend({
	load: function() {
		return callStatus().catch(function() { return {}; });
	},

	render: function(data) {
		var container = E('div', { 'class': 'cbi-map' }, [
			E('h2', {}, _('OMCI Agent Status')),
			E('div', { 'class': 'cbi-map-descr' },
				_('The in-kernel econet-xpon agent handles OLT/OMCI requests; status is read from the driver procfs and refreshed every 3 seconds. Fields a full userspace OMCI agent would expose (calibrated BOSA DDM, MIB and agent counters) are shown as Unavailable on this target.')),
			E('div', { 'id': 'omci-status' }, renderTable(data))
		]);

		poll.add(function() {
			return callStatus().then(function(d) {
				var node = document.getElementById('omci-status');
				if (node) dom.content(node, renderTable(d));
			}).catch(function() {});
		}, 3);

		return container;
	},

	handleSave: null,
	handleSaveApply: null,
	handleReset: null
});
