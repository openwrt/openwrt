'use strict';
'require network';

return network.registerProtocol('none', {
	getI18n: function() {
		return _('Unmanaged');
	}
});
