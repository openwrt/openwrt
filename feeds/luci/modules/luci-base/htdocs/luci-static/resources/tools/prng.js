'use strict';

var s = [0x0000, 0x0000, 0x0000, 0x0000];

function mul(a, b) {
	var r = [0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000];

	for (var j = 0; j < 4; j++) {
		var k = 0;
		for (var i = 0; i < 4; i++) {
			var t = a[i] * b[j] + r[i+j] + k;
			r[i+j] = t & 0xffff;
			k = t >>> 16;
		}
		r[j+4] = k;
	}

	r.length = 4;

	return r;
}

function add(a, n) {
	var r = [0x0000, 0x0000, 0x0000, 0x0000],
	    k = n;

	for (var i = 0; i < 4; i++) {
		var t = a[i] + k;
		r[i] = t & 0xffff;
		k = t >>> 16;
	}

	return r;
}

function shr(a, n) {
	var r = [a[0], a[1], a[2], a[3], 0x0000],
	    i = 4,
	    k = 0;

	for (; n > 16; n -= 16, i--)
		for (var j = 0; j < 4; j++)
			r[j] = r[j+1];

	for (; i > 0; i--) {
		var s = r[i-1];
		r[i-1] = (s >>> n) | k;
		k = ((s & ((1 << n) - 1)) << (16 - n));
	}

	r.length = 4;

	return r;
}

return L.Class.extend({
	seed: function(n) {
		n = (n - 1)|0;
		s[0] = n & 0xffff;
		s[1] = n >>> 16;
		s[2] = 0;
		s[3] = 0;
	},

	int: function() {
		s = mul(s, [0x7f2d, 0x4c95, 0xf42d, 0x5851]);
		s = add(s, 1);

		var r = shr(s, 33);
		return (r[1] << 16) | r[0];
	},

	get: function() {
		var r = (this.int() % 0x7fffffff) / 0x7fffffff, l, u;

		switch (arguments.length) {
		case 0:
			return r;

		case 1:
			l = 1;
			u = arguments[0]|0;
			break;

		case 2:
			l = arguments[0]|0;
			u = arguments[1]|0;
			break;
		}

		return Math.floor(r * (u - l + 1)) + l;
	}
});
