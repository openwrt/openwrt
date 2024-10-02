let ubus, sub, listener, global;

const dhcp_parser_proto = {
	reset: function() {
		this.offset = 0;
	},

	parseAt: function(offset) {
		let id = hex(substr(this.buffer, offset, 2));
		let len = hex(substr(this.buffer, offset + 2, 2)) * 2;
		if (type(id) != "int" || type(len) != "int")
			return null;

		let data = substr(this.buffer, offset + 4, len);
		if (length(data) != len)
			return null;

		return [ id, data, len + 4 ];
	},

	next: function() {
		let data = this.parseAt(this.offset);
		if (!data)
			return null;

		this.offset += data[2];
		return data;
	},

	foreach: function(cb) {
		let offset = 0;
		let data;

		while ((data = this.parseAt(offset)) != null) {
			offset += data[2];
			let ret = cb(data);
			if (type(ret) == "boolean" && !ret)
				break;
		}
	},
};

function dhcp_opt_parser(data) {
	let parser = {
		offset: 0,
		buffer: data,
	};

	proto(parser, dhcp_parser_proto);

	return parser;
}

function parse_array(data)
{
	return map(match(data, /../g), (val) => val[0]);
}

function parse_string(data)
{
	return join("", map(parse_array(data[1]), (val) => chr(hex(val))));
}

function parse_macaddr(addr)
{
	return join(":", parse_array(addr));
}

function dhcp_cb(msg) {
	if (msg.type != "discover" && msg.type != "request")
		return;

	let packet = msg.data.packet;
	if (!packet)
		return;

	let opts = substr(packet, 240 * 2);
	if (length(opts) < 16)
		return;

	let macaddr = parse_macaddr(substr(packet, 28 * 2, 12));

	opts = dhcp_opt_parser(opts);
	opts.foreach((data) => {
		let id = data[0];
		switch (id) {
		case 12:
			typestr = "%device_name|dhcp_device_name";
			data = parse_string(data);
			break;
		case 55:
			typestr = "dhcp_req";
			data = join(",", map(parse_array(data[1]), (val) => hex(val)));
			break;
		case 60:
			typestr = "dhcp_vendorid";
			data = parse_string(data);
			break;
		default:
			return;
		}
		global.device_add_data(macaddr, `${typestr}|${data}`);
	});
}

function init(gl) {
	global = gl;
	ubus = gl.ubus;

	gl.weight.dhcp_req = 1.1;
	gl.weight.dhcp_device_name = 5.0;
	sub = ubus.subscriber(dhcp_cb);
	listener = ubus.listener("ubus.object.add", (event, msg) => {
		if (msg.path == "dhcpsnoop")
			sub.subscribe(msg.path);
	});
	sub.subscribe("dhcpsnoop");
}

return { init };
