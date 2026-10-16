'use strict';
'require view';
'require omci.mib as mib';

return view.extend({
	load: mib.load,
	render: function(data) {
		return mib.render('inventory', data, _('Inventory'), _('ONU inventory, identity and management managed entities.'));
	},
	handleSaveApply: null,
	handleSave: null,
	handleReset: null
});
