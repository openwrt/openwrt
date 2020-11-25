'use strict';
'require fs';
'require ui';
'require rpc';

var callLuciProcessList = rpc.declare({
	object: 'luci',
	method: 'getProcessList',
	expect: { result: [] }
});

return L.view.extend({
	load: function() {
		return callLuciProcessList();
	},

	handleSignal: function(signum, pid, ev) {
		return fs.exec('/bin/kill', ['-%d'.format(signum), '%s'.format(pid)]).then(L.bind(function() {
			return callLuciProcessList().then(L.bind(function(processes) {
				this.updateTable('.table', processes);
			}, this));
		}, this)).catch(function(e) { ui.addNotification(null, E('p', e.message)) });
	},

	updateTable: function(table, processes) {
		var rows = [];

		processes.sort(function(a, b) {
			return (a.PID - b.PID);
		});

		for (var i = 0; i < processes.length; i++) {
			var proc = processes[i];

			rows.push([
				proc.PID,
				proc.USER,
				proc.COMMAND,
				proc['%CPU'],
				proc['%MEM'],
				E('div', { 'class': 'nowrap' }, [
					E('button', {
						'class': 'btn cbi-button-action',
						'click': ui.createHandlerFn(this, 'handleSignal', 1, proc.PID)
					}, _('Hang Up')), ' ',
					E('button', {
						'class': 'btn cbi-button-negative',
						'click': ui.createHandlerFn(this, 'handleSignal', 15, proc.PID)
					}, _('Terminate')), ' ',
					E('button', {
						'class': 'btn cbi-button-negative',
						'click': ui.createHandlerFn(this, 'handleSignal', 9, proc.PID)
					}, _('Kill'))
				])
			]);
		}

		cbi_update_table(table, rows, E('em', _('No information available')));
	},

	render: function(processes) {
		var v = E([], [
			E('h2', _('Processes')),
			E('div', { 'class': 'cbi-map-descr' }, _('This list gives an overview over currently running system processes and their status.')),

			E('div', { 'class': 'table' }, [
				E('div', { 'class': 'tr table-titles' }, [
					E('div', { 'class': 'th' }, _('PID')),
					E('div', { 'class': 'th' }, _('Owner')),
					E('div', { 'class': 'th' }, _('Command')),
					E('div', { 'class': 'th' }, _('CPU usage (%)')),
					E('div', { 'class': 'th' }, _('Memory usage (%)')),
					E('div', { 'class': 'th center' }, _('Actions'))
				])
			])
		]);

		this.updateTable(v.lastElementChild, processes);

		return v;
	},

	handleSaveApply: null,
	handleSave: null,
	handleReset: null
});
