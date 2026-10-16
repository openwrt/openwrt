'use strict';
'require view';
'require omci.mib as mib';

return view.extend({
	load: mib.load,
	render: function(data) {
		return mib.render('gpon', data, _('GPON'), _('ANI and GPON-facing managed entities.'));
	},
	handleSaveApply: null,
	handleSave: null,
	handleReset: null
});
