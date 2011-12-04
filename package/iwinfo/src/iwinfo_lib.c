/*
 * iwinfo - Wireless Information Library - Lua Bindings
 *
 *   Copyright (C) 2009 Jo-Philipp Wich <xm@subsignal.org>
 *
 * The iwinfo library is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 *
 * The iwinfo library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with the iwinfo library. If not, see http://www.gnu.org/licenses/.
 */

#include "iwinfo.h"


/*
 * name constants
 */
const char *IWINFO_CIPHER_NAMES[] = {
	"NONE",
	"WEP40",
	"TKIP",
	"WRAP",
	"CCMP",
	"WEP104",
	"AES-OCB",
	"CKIP",
};

const char *IWINFO_KMGMT_NAMES[] = {
	"NONE",
	"802.1X",
	"PSK",
};

const char *IWINFO_AUTH_NAMES[] = {
	"OPEN",
	"SHARED",
};


/*
 * ISO3166 country labels
 */

const struct iwinfo_iso3166_label IWINFO_ISO3166_NAMES[] = {
	{ 0x3030 /* 00 */, "World" },
	{ 0x4144 /* AD */, "Andorra" },
	{ 0x4145 /* AE */, "United Arab Emirates" },
	{ 0x4146 /* AF */, "Afghanistan" },
	{ 0x4147 /* AG */, "Antigua and Barbuda" },
	{ 0x4149 /* AI */, "Anguilla" },
	{ 0x414C /* AL */, "Albania" },
	{ 0x414D /* AM */, "Armenia" },
	{ 0x414E /* AN */, "Netherlands Antilles" },
	{ 0x414F /* AO */, "Angola" },
	{ 0x4151 /* AQ */, "Antarctica" },
	{ 0x4152 /* AR */, "Argentina" },
	{ 0x4153 /* AS */, "American Samoa" },
	{ 0x4154 /* AT */, "Austria" },
	{ 0x4155 /* AU */, "Australia" },
	{ 0x4157 /* AW */, "Aruba" },
	{ 0x4158 /* AX */, "Aland Islands" },
	{ 0x415A /* AZ */, "Azerbaijan" },
	{ 0x4241 /* BA */, "Bosnia and Herzegovina" },
	{ 0x4242 /* BB */, "Barbados" },
	{ 0x4244 /* BD */, "Bangladesh" },
	{ 0x4245 /* BE */, "Belgium" },
	{ 0x4246 /* BF */, "Burkina Faso" },
	{ 0x4247 /* BG */, "Bulgaria" },
	{ 0x4248 /* BH */, "Bahrain" },
	{ 0x4249 /* BI */, "Burundi" },
	{ 0x424A /* BJ */, "Benin" },
	{ 0x424C /* BL */, "Saint Barthelemy" },
	{ 0x424D /* BM */, "Bermuda" },
	{ 0x424E /* BN */, "Brunei Darussalam" },
	{ 0x424F /* BO */, "Bolivia" },
	{ 0x4252 /* BR */, "Brazil" },
	{ 0x4253 /* BS */, "Bahamas" },
	{ 0x4254 /* BT */, "Bhutan" },
	{ 0x4256 /* BV */, "Bouvet Island" },
	{ 0x4257 /* BW */, "Botswana" },
	{ 0x4259 /* BY */, "Belarus" },
	{ 0x425A /* BZ */, "Belize" },
	{ 0x4341 /* CA */, "Canada" },
	{ 0x4343 /* CC */, "Cocos (Keeling) Islands" },
	{ 0x4344 /* CD */, "Congo" },
	{ 0x4346 /* CF */, "Central African Republic" },
	{ 0x4347 /* CG */, "Congo" },
	{ 0x4348 /* CH */, "Switzerland" },
	{ 0x4349 /* CI */, "Cote d'Ivoire" },
	{ 0x434B /* CK */, "Cook Islands" },
	{ 0x434C /* CL */, "Chile" },
	{ 0x434D /* CM */, "Cameroon" },
	{ 0x434E /* CN */, "China" },
	{ 0x434F /* CO */, "Colombia" },
	{ 0x4352 /* CR */, "Costa Rica" },
	{ 0x4355 /* CU */, "Cuba" },
	{ 0x4356 /* CV */, "Cape Verde" },
	{ 0x4358 /* CX */, "Christmas Island" },
	{ 0x4359 /* CY */, "Cyprus" },
	{ 0x435A /* CZ */, "Czech Republic" },
	{ 0x4445 /* DE */, "Germany" },
	{ 0x444A /* DJ */, "Djibouti" },
	{ 0x444B /* DK */, "Denmark" },
	{ 0x444D /* DM */, "Dominica" },
	{ 0x444F /* DO */, "Dominican Republic" },
	{ 0x445A /* DZ */, "Algeria" },
	{ 0x4543 /* EC */, "Ecuador" },
	{ 0x4545 /* EE */, "Estonia" },
	{ 0x4547 /* EG */, "Egypt" },
	{ 0x4548 /* EH */, "Western Sahara" },
	{ 0x4552 /* ER */, "Eritrea" },
	{ 0x4553 /* ES */, "Spain" },
	{ 0x4554 /* ET */, "Ethiopia" },
	{ 0x4649 /* FI */, "Finland" },
	{ 0x464A /* FJ */, "Fiji" },
	{ 0x464B /* FK */, "Falkland Islands" },
	{ 0x464D /* FM */, "Micronesia" },
	{ 0x464F /* FO */, "Faroe Islands" },
	{ 0x4652 /* FR */, "France" },
	{ 0x4741 /* GA */, "Gabon" },
	{ 0x4742 /* GB */, "United Kingdom" },
	{ 0x4744 /* GD */, "Grenada" },
	{ 0x4745 /* GE */, "Georgia" },
	{ 0x4746 /* GF */, "French Guiana" },
	{ 0x4747 /* GG */, "Guernsey" },
	{ 0x4748 /* GH */, "Ghana" },
	{ 0x4749 /* GI */, "Gibraltar" },
	{ 0x474C /* GL */, "Greenland" },
	{ 0x474D /* GM */, "Gambia" },
	{ 0x474E /* GN */, "Guinea" },
	{ 0x4750 /* GP */, "Guadeloupe" },
	{ 0x4751 /* GQ */, "Equatorial Guinea" },
	{ 0x4752 /* GR */, "Greece" },
	{ 0x4753 /* GS */, "South Georgia" },
	{ 0x4754 /* GT */, "Guatemala" },
	{ 0x4755 /* GU */, "Guam" },
	{ 0x4757 /* GW */, "Guinea-Bissau" },
	{ 0x4759 /* GY */, "Guyana" },
	{ 0x484B /* HK */, "Hong Kong" },
	{ 0x484D /* HM */, "Heard and McDonald Islands" },
	{ 0x484E /* HN */, "Honduras" },
	{ 0x4852 /* HR */, "Croatia" },
	{ 0x4854 /* HT */, "Haiti" },
	{ 0x4855 /* HU */, "Hungary" },
	{ 0x4944 /* ID */, "Indonesia" },
	{ 0x4945 /* IE */, "Ireland" },
	{ 0x494C /* IL */, "Israel" },
	{ 0x494D /* IM */, "Isle of Man" },
	{ 0x494E /* IN */, "India" },
	{ 0x494F /* IO */, "Chagos Islands" },
	{ 0x4951 /* IQ */, "Iraq" },
	{ 0x4952 /* IR */, "Iran" },
	{ 0x4953 /* IS */, "Iceland" },
	{ 0x4954 /* IT */, "Italy" },
	{ 0x4A45 /* JE */, "Jersey" },
	{ 0x4A4D /* JM */, "Jamaica" },
	{ 0x4A4F /* JO */, "Jordan" },
	{ 0x4A50 /* JP */, "Japan" },
	{ 0x4B45 /* KE */, "Kenya" },
	{ 0x4B47 /* KG */, "Kyrgyzstan" },
	{ 0x4B48 /* KH */, "Cambodia" },
	{ 0x4B49 /* KI */, "Kiribati" },
	{ 0x4B4D /* KM */, "Comoros" },
	{ 0x4B4E /* KN */, "Saint Kitts and Nevis" },
	{ 0x4B50 /* KP */, "North Korea" },
	{ 0x4B52 /* KR */, "South Korea" },
	{ 0x4B57 /* KW */, "Kuwait" },
	{ 0x4B59 /* KY */, "Cayman Islands" },
	{ 0x4B5A /* KZ */, "Kazakhstan" },
	{ 0x4C41 /* LA */, "Laos" },
	{ 0x4C42 /* LB */, "Lebanon" },
	{ 0x4C43 /* LC */, "Saint Lucia" },
	{ 0x4C49 /* LI */, "Liechtenstein" },
	{ 0x4C4B /* LK */, "Sri Lanka" },
	{ 0x4C52 /* LR */, "Liberia" },
	{ 0x4C53 /* LS */, "Lesotho" },
	{ 0x4C54 /* LT */, "Lithuania" },
	{ 0x4C55 /* LU */, "Luxembourg" },
	{ 0x4C56 /* LV */, "Latvia" },
	{ 0x4C59 /* LY */, "Libyan Arab Jamahiriya" },
	{ 0x4D41 /* MA */, "Morocco" },
	{ 0x4D43 /* MC */, "Monaco" },
	{ 0x4D44 /* MD */, "Moldova" },
	{ 0x4D45 /* ME */, "Montenegro" },
	{ 0x4D46 /* MF */, "Saint Martin (French part)" },
	{ 0x4D47 /* MG */, "Madagascar" },
	{ 0x4D48 /* MH */, "Marshall Islands" },
	{ 0x4D4B /* MK */, "Macedonia" },
	{ 0x4D4C /* ML */, "Mali" },
	{ 0x4D4D /* MM */, "Myanmar" },
	{ 0x4D4E /* MN */, "Mongolia" },
	{ 0x4D4F /* MO */, "Macao" },
	{ 0x4D50 /* MP */, "Northern Mariana Islands" },
	{ 0x4D51 /* MQ */, "Martinique" },
	{ 0x4D52 /* MR */, "Mauritania" },
	{ 0x4D53 /* MS */, "Montserrat" },
	{ 0x4D54 /* MT */, "Malta" },
	{ 0x4D55 /* MU */, "Mauritius" },
	{ 0x4D56 /* MV */, "Maldives" },
	{ 0x4D57 /* MW */, "Malawi" },
	{ 0x4D58 /* MX */, "Mexico" },
	{ 0x4D59 /* MY */, "Malaysia" },
	{ 0x4D5A /* MZ */, "Mozambique" },
	{ 0x4E41 /* NA */, "Namibia" },
	{ 0x4E43 /* NC */, "New Caledonia" },
	{ 0x4E45 /* NE */, "Niger" },
	{ 0x4E46 /* NF */, "Norfolk Island" },
	{ 0x4E47 /* NG */, "Nigeria" },
	{ 0x4E49 /* NI */, "Nicaragua" },
	{ 0x4E4C /* NL */, "Netherlands" },
	{ 0x4E4F /* NO */, "Norway" },
	{ 0x4E50 /* NP */, "Nepal" },
	{ 0x4E52 /* NR */, "Nauru" },
	{ 0x4E55 /* NU */, "Niue" },
	{ 0x4E5A /* NZ */, "New Zealand" },
	{ 0x4F4D /* OM */, "Oman" },
	{ 0x5041 /* PA */, "Panama" },
	{ 0x5045 /* PE */, "Peru" },
	{ 0x5046 /* PF */, "French Polynesia" },
	{ 0x5047 /* PG */, "Papua New Guinea" },
	{ 0x5048 /* PH */, "Philippines" },
	{ 0x504B /* PK */, "Pakistan" },
	{ 0x504C /* PL */, "Poland" },
	{ 0x504D /* PM */, "Saint Pierre and Miquelon" },
	{ 0x504E /* PN */, "Pitcairn" },
	{ 0x5052 /* PR */, "Puerto Rico" },
	{ 0x5053 /* PS */, "Palestinian Territory" },
	{ 0x5054 /* PT */, "Portugal" },
	{ 0x5057 /* PW */, "Palau" },
	{ 0x5059 /* PY */, "Paraguay" },
	{ 0x5141 /* QA */, "Qatar" },
	{ 0x5245 /* RE */, "Reunion" },
	{ 0x524F /* RO */, "Romania" },
	{ 0x5253 /* RS */, "Serbia" },
	{ 0x5255 /* RU */, "Russian Federation" },
	{ 0x5257 /* RW */, "Rwanda" },
	{ 0x5341 /* SA */, "Saudi Arabia" },
	{ 0x5342 /* SB */, "Solomon Islands" },
	{ 0x5343 /* SC */, "Seychelles" },
	{ 0x5344 /* SD */, "Sudan" },
	{ 0x5345 /* SE */, "Sweden" },
	{ 0x5347 /* SG */, "Singapore" },
	{ 0x5348 /* SH */, "St. Helena and Dependencies" },
	{ 0x5349 /* SI */, "Slovenia" },
	{ 0x534A /* SJ */, "Svalbard and Jan Mayen" },
	{ 0x534B /* SK */, "Slovakia" },
	{ 0x534C /* SL */, "Sierra Leone" },
	{ 0x534D /* SM */, "San Marino" },
	{ 0x534E /* SN */, "Senegal" },
	{ 0x534F /* SO */, "Somalia" },
	{ 0x5352 /* SR */, "Suriname" },
	{ 0x5354 /* ST */, "Sao Tome and Principe" },
	{ 0x5356 /* SV */, "El Salvador" },
	{ 0x5359 /* SY */, "Syrian Arab Republic" },
	{ 0x535A /* SZ */, "Swaziland" },
	{ 0x5443 /* TC */, "Turks and Caicos Islands" },
	{ 0x5444 /* TD */, "Chad" },
	{ 0x5446 /* TF */, "French Southern Territories" },
	{ 0x5447 /* TG */, "Togo" },
	{ 0x5448 /* TH */, "Thailand" },
	{ 0x544A /* TJ */, "Tajikistan" },
	{ 0x544B /* TK */, "Tokelau" },
	{ 0x544C /* TL */, "Timor-Leste" },
	{ 0x544D /* TM */, "Turkmenistan" },
	{ 0x544E /* TN */, "Tunisia" },
	{ 0x544F /* TO */, "Tonga" },
	{ 0x5452 /* TR */, "Turkey" },
	{ 0x5454 /* TT */, "Trinidad and Tobago" },
	{ 0x5456 /* TV */, "Tuvalu" },
	{ 0x5457 /* TW */, "Taiwan" },
	{ 0x545A /* TZ */, "Tanzania" },
	{ 0x5541 /* UA */, "Ukraine" },
	{ 0x5547 /* UG */, "Uganda" },
	{ 0x554D /* UM */, "U.S. Minor Outlying Islands" },
	{ 0x5553 /* US */, "United States" },
	{ 0x5559 /* UY */, "Uruguay" },
	{ 0x555A /* UZ */, "Uzbekistan" },
	{ 0x5641 /* VA */, "Vatican City State" },
	{ 0x5643 /* VC */, "St. Vincent and Grenadines" },
	{ 0x5645 /* VE */, "Venezuela" },
	{ 0x5647 /* VG */, "Virgin Islands, British" },
	{ 0x5649 /* VI */, "Virgin Islands, U.S." },
	{ 0x564E /* VN */, "Viet Nam" },
	{ 0x5655 /* VU */, "Vanuatu" },
	{ 0x5746 /* WF */, "Wallis and Futuna" },
	{ 0x5753 /* WS */, "Samoa" },
	{ 0x5945 /* YE */, "Yemen" },
	{ 0x5954 /* YT */, "Mayotte" },
	{ 0x5A41 /* ZA */, "South Africa" },
	{ 0x5A4D /* ZM */, "Zambia" },
	{ 0x5A57 /* ZW */, "Zimbabwe" },
	{ 0,               "" }
};

/*
 * hardware database
 */

const char VENDOR_UBNT[] = "Ubiquiti";
const char VENDOR_ATH[]  = "Atheros";

const struct iwinfo_hardware_entry IWINFO_HARDWARE_ENTRIES[] = {
/*	{ vendor, model, vendorid, deviceid, subsys vendorid, subsys deviceid, poweroff, freqoff } */
#if defined(USE_MADWIFI) || defined(USE_NL80211)
	{ VENDOR_UBNT, "PowerStation2 (18V)", 	0xffff, 0xffff, 0xffff, 0xb102,  0,     0 },
	{ VENDOR_UBNT, "PowerStation2 (16D)", 	0xffff, 0xffff, 0xffff, 0xb202,  0,     0 },
	{ VENDOR_UBNT, "PowerStation2 (EXT)", 	0xffff, 0xffff, 0xffff, 0xb302,  0,     0 },
	{ VENDOR_UBNT, "PowerStation5 (22V)", 	0xffff, 0xffff, 0xffff, 0xb105,  0,     0 },
	{ VENDOR_UBNT, "PowerStation5 (EXT)", 	0xffff, 0xffff, 0xffff, 0xb305,  0,     0 },
	{ VENDOR_UBNT, "WispStation5",        	0xffff, 0xffff, 0xffff, 0xa105,  0,     0 },
	{ VENDOR_UBNT, "LiteStation2",        	0xffff, 0xffff, 0xffff, 0xa002, 10,     0 },
	{ VENDOR_UBNT, "LiteStation5",        	0xffff, 0xffff, 0xffff, 0xa005,  5,     0 },
	{ VENDOR_UBNT, "NanoStation2",        	0xffff, 0xffff, 0xffff, 0xc002, 10,     0 },
	{ VENDOR_UBNT, "NanoStation5",        	0xffff, 0xffff, 0xffff, 0xc005,  5,     0 },
	{ VENDOR_UBNT, "NanoStation Loco2",   	0xffff, 0xffff, 0xffff, 0xc102, 10,     0 },
	{ VENDOR_UBNT, "NanoStation Loco5",   	0xffff, 0xffff, 0xffff, 0xc105,  5,     0 },
	{ VENDOR_UBNT, "Bullet2",             	0xffff, 0xffff, 0xffff, 0xc202, 10,     0 },
	{ VENDOR_UBNT, "Bullet5",             	0xffff, 0xffff, 0xffff, 0xc205,  5,     0 },
	{ VENDOR_UBNT, "XR2",                   0x168c, 0x001b, 0x0777, 0x3002, 10,     0 },
	{ VENDOR_UBNT, "XR2",                   0x168c, 0x001b, 0x7777, 0x3002, 10,     0 },
	{ VENDOR_UBNT, "XR2.3",                 0x168c, 0x001b, 0x0777, 0x3b02, 10,     0 },
	{ VENDOR_UBNT, "XR2.6",                 0x168c, 0x001b, 0x0777, 0x3c02, 10,     0 },
	{ VENDOR_UBNT, "XR3-2.8",               0x168c, 0x001b, 0x0777, 0x3b03, 10,     0 },
	{ VENDOR_UBNT, "XR3-3.6",               0x168c, 0x001b, 0x0777, 0x3c03, 10,     0 },
	{ VENDOR_UBNT, "XR3",                   0x168c, 0x001b, 0x0777, 0x3003, 10,     0 },
	{ VENDOR_UBNT, "XR4",                   0x168c, 0x001b, 0x0777, 0x3004, 10,     0 },
	{ VENDOR_UBNT, "XR5",                   0x168c, 0x001b, 0x0777, 0x3005, 10,     0 },
	{ VENDOR_UBNT, "XR5",                   0x168c, 0x001b, 0x7777, 0x3005, 10,     0 },
	{ VENDOR_UBNT, "XR7",                   0x168c, 0x001b, 0x0777, 0x3007, 10,     0 },
	{ VENDOR_UBNT, "XR9",                   0x168c, 0x001b, 0x0777, 0x3009, 10, -1520 },
	{ VENDOR_UBNT, "SRC",                   0x168c, 0x0013, 0x168c, 0x1042,  1,     0 },
	{ VENDOR_UBNT, "SR2",                   0x168c, 0x0013, 0x0777, 0x2041, 10,     0 },
	{ VENDOR_UBNT, "SR4",                   0x168c, 0x0013, 0x0777, 0x2004,  6,     0 },
	{ VENDOR_UBNT, "SR4",                   0x168c, 0x0013, 0x7777, 0x2004,  6,     0 },
	{ VENDOR_UBNT, "SR4C",                  0x168c, 0x0013, 0x0777, 0x1004,  6,     0 },
	{ VENDOR_UBNT, "SR4C",                  0x168c, 0x0013, 0x7777, 0x1004,  6,     0 },
	{ VENDOR_UBNT, "SR5",                   0x168c, 0x0013, 0x168c, 0x2042,  7,     0 },
	{ VENDOR_UBNT, "SR9",                   0x168c, 0x0013, 0x7777, 0x2009, 12, -1500 },
	{ VENDOR_UBNT, "SR71A",                 0x168c, 0x0027, 0x168c, 0x2082, 10,     0 },
	{ VENDOR_UBNT, "SR71",                  0x168c, 0x0027, 0x0777, 0x4082, 10,     0 },
#endif
#ifdef NL80211
	{ VENDOR_UBNT, "NanoStation M2",        0x168c, 0x002a, 0x0777, 0xe012, 10,     0 }, /* ToDo: confirm offset */
	{ VENDOR_UBNT, "NanoStation M5",        0x168c, 0x002a, 0x0777, 0xe005,  5,     0 }, /* ToDo: confirm offset */
	{ VENDOR_UBNT, "Bullet M2",             0x168c, 0x002a, 0x0777, 0xe202, 12,     0 },
	{ VENDOR_UBNT, "Bullet M5",             0x168c, 0x002a, 0x0777, 0xe205,  5,     0 },

	{ VENDOR_ATH,  "AR9220",                0x168c, 0x0029, 0x168c, 0xa094,  0,     0 },
	{ VENDOR_ATH,  "AR9223",                0x168c, 0x0029, 0x168c, 0xa095,  0,     0 },
#endif
	{ NULL }
};


const char * iwinfo_type(const char *ifname)
{
#ifdef USE_NL80211
	if (nl80211_probe(ifname))
		return "nl80211";
	else
#endif

#ifdef USE_MADWIFI
	if (madwifi_probe(ifname))
		return "madwifi";
	else
#endif

#ifdef USE_WL
	if (wl_probe(ifname))
		return "wl";
	else
#endif

	if (wext_probe(ifname))
		return "wext";

	return NULL;
}

const struct iwinfo_ops * iwinfo_backend(const char *ifname)
{
	const char *type;
	struct iwinfo_ops *ops;

	type = iwinfo_type(ifname);
	if (!type)
		return NULL;

#ifdef USE_NL80211
	if (!strcmp(type, "nl80211"))
		return &nl80211_ops;
	else
#endif

#ifdef USE_MADWIFI
	if (!strcmp(type, "madwifi"))
		return &madwifi_ops;
	else
#endif

#ifdef USE_WL
	if (!strcmp(type, "wl"))
		return &wl_ops;
	else
#endif

	if (!strcmp(type, "wext"))
		return &wext_ops;

	return NULL;
}

void iwinfo_finish(void)
{
#ifdef USE_WL
	wl_close();
#endif
#ifdef USE_MADWIFI
	madwifi_close();
#endif
#ifdef USE_NL80211
	nl80211_close();
#endif
	wext_close();
	iwinfo_close();
}
