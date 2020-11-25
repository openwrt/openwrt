'use strict';
'require rpc';

var callLuciDSLStatus = rpc.declare({
	object: 'luci-rpc',
	method: 'getDSLStatus',
	expect: { '': {} }
});

function renderbox(dsl) {
	return E('div', { class: 'ifacebox' }, [
		E('div', { class: 'ifacebox-head center ' + ((dsl.line_state === 'UP') ? 'active' : '') },
			E('strong', _('DSL Status'))),
		E('div', { class: 'ifacebox-body left' }, [
			L.itemlist(E('span'), [
				_('Line State'), '%s [0x%x]'.format(dsl.line_state, dsl.line_state_detail),
				_('Line Mode'), dsl.line_mode_s || '-',
				_('Line Uptime'), dsl.line_uptime_s || '-',
				_('Annex'), dsl.annex_s || '-',
				_('Profile'), dsl.profile_s || '-',
				_('Data Rate'), '%s/s / %s/s'.format(dsl.data_rate_down_s, dsl.data_rate_up_s),
				_('Max. Attainable Data Rate (ATTNDR)'), '%s/s / %s/s'.format(dsl.max_data_rate_down_s, dsl.max_data_rate_up_s),
				_('Latency'), '%s / %s'.format(dsl.latency_num_down, dsl.latency_num_up),
				_('Line Attenuation (LATN)'), '%.1f dB / %.1f dB'.format(dsl.line_attenuation_down, dsl.line_attenuation_up),
				_('Signal Attenuation (SATN)'), '%.1f dB / %.1f dB'.format(dsl.signal_attenuation_down, dsl.signal_attenuation_up),
				_('Noise Margin (SNR)'), '%.1f dB / %.1f dB'.format(dsl.noise_margin_down, dsl.noise_margin_up),
				_('Aggregate Transmit Power(ACTATP)'), '%.1f dB / %.1f dB'.format(dsl.actatp_down, dsl.actatp_up),
				_('Forward Error Correction Seconds (FECS)'), '%d / %d'.format(dsl.errors_fecs_near, dsl.errors_fecs_far),
				_('Errored seconds (ES)'), '%d / %d'.format(dsl.errors_es_near, dsl.errors_es_far),
				_('Severely Errored Seconds (SES)'), '%d / %d'.format(dsl.errors_ses_near, dsl.errors_ses_far),
				_('Loss of Signal Seconds (LOSS)'), '%d / %d'.format(dsl.errors_loss_near, dsl.errors_loss_far),
				_('Unavailable Seconds (UAS)'), '%d / %d'.format(dsl.errors_uas_near, dsl.errors_uas_far),
				_('Header Error Code Errors (HEC)'), '%d / %d'.format(dsl.errors_hec_near, dsl.errors_hec_far),
				_('Non Pre-emtive CRC errors (CRC_P)'), '%d / %d'.format(dsl.errors_crc_p_near, dsl.errors_crc_p_far),
				_('Pre-emtive CRC errors (CRCP_P)'), '%d / %d'.format(dsl.errors_crcp_p_near, dsl.errors_crcp_p_far),
				_('ATU-C System Vendor ID'), dsl.atuc_vendor_id,
				_('Power Management Mode'), dsl.power_mode_s
			])
		])
	]);
}

return L.Class.extend({
	title: _('DSL'),

	load: function() {
		return L.resolveDefault(callLuciDSLStatus(), {});
	},

	render: function(dsl) {
		if (!dsl.line_state)
			return null;

		return E('div', { 'id': 'dsl_status_table', 'class': 'network-status-table' }, renderbox(dsl));
	}
});
