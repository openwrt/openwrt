{%
let devid;
let fd = fs.open("/etc/ucentral/dev-id", "r");
if (!fd) {
	warn("firstcontact: failed to find device id");
	exit(1);
}
devid = fd.read("all");
fd.close();

ret = system(sprintf('/usr/sbin/firstcontact -i %s', devid));

if (ret) {
	warn("firstcontact failed to contact redirector\n");
	exit(1);
}

let redirector = { };
let fd = fs.open("/etc/ucentral/redirector.json", "r");
if (fd) {
	let data = fd.read("all");
	fd.close();

	try {
		redirector = json(data);
	}
	catch (e) {
		warn("firstcontact: Unable to parse JSON data in %s: %s", path, e);

		exit(1);
	}
}
let config = {};

for (let r in redirector.fields)
	if (r.name && r.value)
		config[r.name] = r.value;
if (!config.Redirector) {
	warn("Reply is missing Redirector field\n");

	exit(1);
}

function store_config(path) {
	let cursor = uci.cursor(path);
	let redir = split(config.Redirector, ":");

	cursor.load("ucentral");
	cursor.set("ucentral", "config", "server", redir[0]);
	cursor.set("ucentral", "config", "port", redir[1] || 15002);
	cursor.commit();
}

store_config();
store_config("/etc/config-shadow/");

warn("firstcontact: managed to look up redirector\n");

system("/etc/init.d/ucentral enable");
system("/etc/init.d/firstcontact disable");

system("reload_config");
system("/etc/init.d/ucentral start");
system("/etc/init.d/firstcontact stop");
%}
