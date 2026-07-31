'use strict';
'require omci.mib as mib';

return mib.createListView({
	title: _('OMCI management and capability MIBs'),
	description: _('OMCI, Managed Entity and Attribute capability-table objects exposed by the in-kernel agent.'),
	category: 'management'
});
