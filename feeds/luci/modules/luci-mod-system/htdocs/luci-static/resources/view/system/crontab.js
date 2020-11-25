'use strict';
'require fs';
'require ui';

return L.view.extend({
	load: function() {
		return L.resolveDefault(fs.read('/etc/crontabs/root'), '');
	},

	handleSave: function(ev) {
		var value = (document.querySelector('textarea').value || '').trim().replace(/\r\n/g, '\n') + '\n';

		return fs.write('/etc/crontabs/root', value).then(function(rc) {
			document.querySelector('textarea').value = value;
			ui.addNotification(null, E('p', _('Contents have been saved.')), 'info');
		}).catch(function(e) {
			ui.addNotification(null, E('p', _('Unable to save contents: %s').format(e.message)));
		});
	},

	render: function(crontab) {
		return E([
			E('h2', _('Scheduled Tasks')),
			E('p', {},
				_('This is the system crontab in which scheduled tasks can be defined.') +
				_('<br/>Note: you need to manually restart the cron service if the crontab file was empty before editing.')),
			E('p', {}, E('textarea', { 'style': 'width:100%', 'rows': 10 }, [ crontab != null ? crontab : '' ]))
		]);
	},

	handleSaveApply: null,
	handleReset: null
});
