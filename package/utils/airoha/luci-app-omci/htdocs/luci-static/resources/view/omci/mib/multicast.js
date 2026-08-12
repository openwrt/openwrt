'use strict';
'require view';
'require omci.mib as mib';

return view.extend({
	load: mib.load,
	render: function(data) {
		return mib.render('multicast', data, _('Multicast'), _('Multicast and IGMP-related managed entities.'));
	},
	handleSaveApply: null,
	handleSave: null,
	handleReset: null
});
