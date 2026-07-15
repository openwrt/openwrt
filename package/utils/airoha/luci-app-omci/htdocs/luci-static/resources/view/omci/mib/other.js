'use strict';
'require omci.mib as mib';

return mib.createListView({
	title: _('Other and vendor MIBs'),
	description: _('Objects not assigned to another OMCI MIB category.'),
	category: 'other'
});
