'use strict';
'require form';
'require fs';
'require poll';
'require rpc';
'require view';

var callGetConfig, callGetStatus;

callGetConfig = rpc.declare({
	object: 'luci.cpu',
	method: 'get_config',
	expect: {  }
});

callGetStatus = rpc.declare({
	object: 'luci.cpu',
	method: 'get_status',
	expect: {  }
});

return view.extend({
	load: function() {
		return Promise.all([
			callGetConfig(),
			callGetStatus()
		]);
	},

	poll_status: function(nodes, data) {
		var stats = data[0].stats;

		let cpuStatArray = [];

		for(let str of stats) {
			let arr = str.split(/\s+/).slice(0, 8);
			arr[0] = (arr[0] === 'cpu') ? Infinity : arr[0].replace('cpu', '');
			cpuStatArray.push(arr.map(e => Number(e)));
		}

		let rows=new Array();
		cpuStatArray.forEach((c, i) => {
			let user = c[1] - window.cpuStatusStatArray[i][1];
			let nice = c[2] - window.cpuStatusStatArray[i][2];
			let sys = c[3] - window.cpuStatusStatArray[i][3];
			let idle = c[4] - window.cpuStatusStatArray[i][4];
			let io = c[5] - window.cpuStatusStatArray[i][5];
			let irq = c[6] - window.cpuStatusStatArray[i][6];
			let sirq = c[7] - window.cpuStatusStatArray[i][7];
			let sum = user + nice + sys + idle + io + irq + sirq;
			let loadUser = Number((100 * user / sum).toFixed(1));
			let loadNice = Number((100 * nice / sum).toFixed(1));
			let loadSys = Number((100 * sys / sum).toFixed(1));
			let loadIdle = Number((100 * idle / sum).toFixed(1));
			let loadIo = Number((100 * io / sum).toFixed(1));
			let loadIrq = Number((100 * irq / sum).toFixed(1));
			let loadSirq = Number((100 * sirq / sum).toFixed(1));
			let loadAvg = Math.round(100 * (user + nice + sys + io + irq + sirq) / sum);

			let row = [cpuStatArray[i][0], data[0].frequency[i], loadAvg, loadUser, loadNice, loadSys, loadIdle, loadIo, loadIrq, loadSirq];
			rows.push(row.map(e => String(e)));
		});

		window.cpuStatusStatArray = cpuStatArray;

		cbi_update_table(nodes.querySelector('#cpu_status_table'), rows, E('em', _('There are no CPUs.')));

		return;
	},

	render: function(results) {
		var governors = Array.isArray(results[0].governors) ? results[0].governors : [];
		var frequencies = Array.isArray(results[0].frequencies) ? results[0].frequencies : [];
		var stats = Array.isArray(results[1].stats) ? results[1].stats : [];
		
		var m, s, o;
		m = new form.Map('cpufreq', _('Kernel Manager'), _('Kernel manager is an application which manages kernel parameters.'));

		s = m.section(form.GridSection, '_cpu_status');

		s.render = L.bind(function(view, section_id) {
			var table = E('table', { 'class': 'table cbi-section-table', 'id': 'cpu_status_table' }, [
				E('tr', { 'class': 'tr table-titles' }, [
					E('th', { 'class': 'th' }, _('Core')),
					E('th', { 'class': 'th' }, _('Current Frequency')),
					E('th', { 'class': 'th' }, _('Load %')),
					E('th', { 'class': 'th' }, 'user %'),
					E('th', { 'class': 'th' }, 'nice %'),
					E('th', { 'class': 'th' }, 'system %'),
					E('th', { 'class': 'th' }, 'idle %'),
					E('th', { 'class': 'th' }, 'iowait %'),
					E('th', { 'class': 'th' }, 'irq %'),
					E('th', { 'class': 'th' }, 'softirq %')
				])
			]);

			let cpuStatArray = [];

			for(let str of stats) {
				let arr = str.split(/\s+/).slice(0, 8);
				arr[0] = (arr[0] === 'cpu') ? Infinity : arr[0].replace('cpu', '');
				cpuStatArray.push(arr.map(e => Number(e)));
			}
			
			let rows=new Array();
			cpuStatArray.forEach((c, i) => {
				let row = [cpuStatArray[i][0], results[1].frequency[i], 0, 0, 0, 0, 0, 0, 0, 0];
				rows.push(row.map(e => String(e)));
			});

			window.cpuStatusStatArray = cpuStatArray;

			cbi_update_table(table, rows, E('em', _('There are no CPUs.')));

			return E('div', { 'class': 'cbi-section cbi-tblsection' }, [
					E('h3', _('CPU Status')), table ]);
		}, o, this);

		s = m.section(form.TypedSection, 'cpufreq', _('Kernel Settings'));
		s.anonymous = true;

		o = s.option(form.ListValue, 'governor', _('CPU Governor'), _('The CPU governor determines how the CPU behaves in workload.'));
		governors.forEach(function(value){ o.value(value); });
		o.cfgvalue = function() { return results[1].governor; };

		o = s.option(form.Value, 'upthreshold', _('CPU Switching Threshold'), _('Set the threshold for stepping up to another frequency.'));
		o.datatype = 'range(1,99)';
		o.depends('governor', 'ondemand');
		o.cfgvalue = function() { return results[1].upthreshold; };

		o = s.option(form.Value, 'factor', _('CPU Sampling rate'), _('Set the frequency(us) which governor checks to tune the CPU'));
		o.datatype = 'range(1,100000)';
		o.depends('governor', 'ondemand');
		o.cfgvalue = function() { return results[1].samplingrate; };

		o = s.option(form.ListValue, 'minfreq', _('CPU Minimum Frequency'), _('Set the minimum frequency the CPU scales down to.'));
		frequencies.forEach(function(value){ o.value(value); });
		o.cfgvalue = function() { return results[1].minfreq; };

		o = s.option(form.ListValue, 'maxfreq', _('CPU Maximum Frequency'), _('Set the maximum frequency the CPU scales up to.'));
		frequencies.forEach(function(value){ o.value(value); });
		o.cfgvalue = function() { return results[1].maxfreq; };

		return m.render().then(L.bind(function(m, nodes) {
			poll.add(L.bind(function() {
				return Promise.all([
					callGetStatus()
				]).then(L.bind(this.poll_status, this, nodes));
			}, this), 5);
			return nodes;
		}, this, m));
	}
});
