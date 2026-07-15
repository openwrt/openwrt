'use strict';
'require omci.mib as mib';

return mib.createListView({
	title: _('All operational MIB objects'),
	description: _('Complete runtime MIB owned by the kernel OMCI agent.'),
	category: 'all',
	reset: true
});
