'use strict';
'require uci';

return L.view.extend({
	load: function() {
		return uci.load('ttyd');
	},
	render: function() {
		var port = uci.get_first('ttyd', 'ttyd', 'port') || '7681';
		if (port === '0')
			return E('div', { class: 'alert-message warning' },
					_('Random ttyd port (port=0) is not supported.<br>Change to a fixed port and try again.'));
		return E('iframe', {
			src: window.location.protocol + '//' + window.location.hostname + ':' + port,
			style: 'width: 100%; min-height: 500px; border: none; border-radius: 3px; resize: vertical;'
		});
	},
	handleSaveApply: null,
	handleSave: null,
	handleReset: null
});
