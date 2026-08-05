'use strict';
'require view';
'require rpc';
'require poll';
'require dom';

var callRaw = rpc.declare({ object: 'econet-xpon', method: 'raw', expect: { } });

function block(title, text) {
	return E('div', {}, [
		E('h3', { 'style': 'margin-top:1em' }, title),
		E('pre', { 'style': 'white-space:pre-wrap;max-height:22em;overflow:auto' }, text || _('(empty)'))
	]);
}

function render(d) {
	d = d || {};
	return E('div', {}, [
		block(_('/proc/xpon/ponInfo'), d.ponInfo),
		block(_('/proc/econet_xpon_optical'), d.optical),
		block(_('/proc/gpon/stage_chk_cnt'), d.stage_chk),
		block(_('/proc/gpon/power_management'), d.power_mgmt)
	]);
}

return view.extend({
	load: function() {
		return callRaw().catch(function() { return {}; });
	},

	render: function(data) {
		var container = E('div', { 'class': 'cbi-map' }, [
			E('h2', {}, _('MIB data')),
			E('div', { 'class': 'cbi-map-descr' },
				_('Raw diagnostic state from the in-kernel econet-xpon driver. This target has no userspace OMCI MIB database, so the OMCI managed-entity dump is not available; the raw PON/optical driver procfs is shown instead.')),
			E('div', { 'id': 'omci-raw' }, render(data))
		]);

		poll.add(function() {
			return callRaw().then(function(d) {
				var node = document.getElementById('omci-raw');
				if (node) dom.content(node, render(d));
			}).catch(function() {});
		}, 5);

		return container;
	},

	handleSave: null,
	handleSaveApply: null,
	handleReset: null
});
