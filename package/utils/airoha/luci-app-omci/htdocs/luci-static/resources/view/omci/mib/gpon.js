'use strict';
'require omci.mib as mib';

return mib.createListView({
	title: _('GPON traffic MIBs'),
	description: _('ANI-G, T-CONT, GEM port, GEM interworking, scheduler and queue objects.'),
	category: 'gpon'
});
