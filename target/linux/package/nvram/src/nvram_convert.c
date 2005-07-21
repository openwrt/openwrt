
#define WL(a)	"wl_"a	
#define WL0(a)	"wl0_"a	
#define D11G(a)	"d11g_"a

#define PPP(a)		"ppp_"a
#define PPPOE(a)	"pppoe_"a

struct nvram_convert {
        char *name;		// for WEB
	char *wl0_name;		// for driver
        char *d11g_name;	// for old nv name
};

struct nvram_convert nvram_converts[] = {
	// Bellow change from 3.11.48.7
	{ WL("ssid"),	 	WL0("ssid"),	""},
	{ WL("radio"), 		WL0("mode"),	""},
	{ WL("mode"), 		WL0("mode"),	""},
	{ WL("wds"), 		WL0("wds"),	""},
	{ WL("auth"), 		WL0("auth"),	""},
	{ WL("key"), 		WL0("key"), 	""},
	{ WL("key1"), 		WL0("key1"), 	""},
	{ WL("key2"), 		WL0("key2"),	""},
	{ WL("key3"), 		WL0("key3"),	""},
	{ WL("key4"), 		WL0("key4"),	""},
	{ WL("maclist"), 	WL0("maclist"),	""},
	{ WL("channel"), 	WL0("channel"), D11G("channel")},
	{ WL("rateset"), 	WL0("rateset"), D11G("rateset")},
	{ WL("rts"), 		WL0("rts"), 	D11G("rts")},
	{ WL("bcn"), 		WL0("bcn"),	D11G("bcn")},
	{ WL("gmode"), 		WL0("gmode"), 	"d11g_mode"},
	{ WL("unit"), 		WL0("unit"), 	""},
	{ WL("ifname"), 	WL0("ifname"),	""},
	{ WL("phytype"), 	WL0("phytype"),	""},
	{ WL("country"), 	WL0("country"),	""},
	{ WL("closed"), 	WL0("closed"),	""},
	{ WL("lazywds"), 	WL0("lazywds"),	""},
	{ WL("wep"), 		WL0("wep"),	""},
	{ WL("macmode"), 	WL0("macmode"),	""},
	{ WL("rate"), 		WL0("rate"), 	D11G("rate")},
	{ WL("frag"), 		WL0("frag"),	D11G("frag")},
	{ WL("dtim"), 		WL0("dtim"), 	D11G("dtim")},
	{ WL("plcphdr"), 	WL0("plcphdr"),	""},
	{ WL("gmode_protection"), 	WL0("gmode_protection"),	""},
	{ WL("radio"), 		WL0("radio"),	""},
	// Bellow change from 3.21.9.0
	{ WL("auth_mode"), 	WL0("auth_mode"),	""},
	{ WL("radius_ipaddr"), 	WL0("radius_ipaddr"),	""},
	{ WL("radius_port"), 	WL0("radius_port"),	""},
	{ WL("radius_key"), 	WL0("radius_key"),	""},
	{ WL("wpa_psk"), 	WL0("wpa_psk"),		""},
	{ WL("wpa_gtk_rekey"), 	WL0("wpa_gtk_rekey"),	""},
	{ WL("frameburst"), 	WL0("frameburst"),	""},
	{ WL("crypto"), 	WL0("crypto"),		""},
	{ WL("ap_isolate"), 	WL0("ap_isolate"),	""},
	{ WL("afterburner"), 	WL0("afterburner"),	""},
	// for PPPoE
	{ PPP("username"), 	PPPOE("username"),	""},
	{ PPP("passwd"), 	PPPOE("passwd"),	""},
	{ PPP("idletime"), 	PPPOE("idletime"),	""},
	{ PPP("keepalive"), 	PPPOE("keepalive"),	""},
	{ PPP("demand"), 	PPPOE("demand"),	""},
	{ PPP("service"), 	PPPOE("service"),	""},
	{ PPP("ac"), 		PPPOE("ac"),		""},
	{ PPP("static"),	PPPOE("static"),	""},
	{ PPP("static_ip"), 	PPPOE("static_ip"),	""},
	{ PPP("username_1"), 	PPPOE("username_1"),	""},
	{ PPP("passwd_1"), 	PPPOE("passwd_1"),	""},
	{ PPP("idletime_1"), 	PPPOE("idletime_1"),	""},
	{ PPP("keepalive_1"), 	PPPOE("keepalive_1"),	""},
	{ PPP("demand_1"), 	PPPOE("demand_1"),	""},
	{ PPP("service_1"), 	PPPOE("service_1"),	""},
	{ PPP("ac_1"), 		PPPOE("ac_1"),		""},
	
	{ 0, 0, 0},
};
