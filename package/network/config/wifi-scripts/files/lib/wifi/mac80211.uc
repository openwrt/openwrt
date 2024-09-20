#!/usr/bin/env ucode
import { readfile } from "fs";
import * as uci from 'uci';

const bands_order = [ "6G", "5G", "2G" ];
const htmode_order = [ "EHT", "HE", "VHT", "HT" ];

let board = json(readfile("/etc/board.json"));
if (!board.wlan)
        exit(0);

let idx = 0;
let commit;

let random_mac_bytes = getenv("MT76_ENV_RANDOM_MAC_BYTES");
let config = uci.cursor().get_all("wireless") ?? {};

function radio_exists(path, macaddr, phy) {
        for (let name, s in config) {
                if (s[".type"] != "wifi-device")
                        continue;
                if (s.macaddr & lc(s.macaddr) == lc(macaddr))
                        return true;
                if (s.phy == phy)
                        return true;
                if (!s.path || !path)
                        continue;
                if (substr(s.path, -length(path)) == path)
                        return true;
        }
}

for (let phy_name, phy in board.wlan) {
        let info = phy.info;
        if (!info || !length(info.bands))
                continue;

        while (config[`radio${idx}`])
                idx++;
        let name = "radio" + idx++;

        let s = "wireless." + name;
        let si = "wireless.default_" + name;

        let band_name = filter(bands_order, (b) => info.bands[b])[0];
        if (!band_name)
                continue;

        let band = info.bands[band_name];
                let channel = band.default_channel ?? "auto";
                if (band_name == "6G")
                        channel = 37;

        let width = band.max_width;

        let htmode = filter(htmode_order, (m) => band[lc(m)])[0];
        if (htmode)
                htmode += width;
        else
                htmode = "NOHT";

        if (!phy.path)
                continue;

        let macaddr = trim(readfile(`/sys/class/ieee80211/${phy_name}/macaddress`));
        if (radio_exists(phy.path, macaddr, phy_name))
                continue;

        let id = `phy='${phy_name}'`;
        if (match(phy_name, /^phy[0-9]/))
                id = `path='${phy.path}'`;

        let disabled = getenv("MT76_ENV_WM_TM") ? 1 : 0;
        let noscan = 0;
        let mbssid = 0;
        let rnr = 0;
        let background_radar = 0;
        let encryption = "none";
        let mbo = 0;
        let band_idx = 0;
        let ssid = "";

        if (band_name == "6G") {
                encryption = "sae";
                mbo = 1;
                band_idx = 2;
                ssid = "OpenWrt-6g";
        } else if (band_name == "5G") {
                noscan = 1;
                rnr = 1;
                background_radar = 1;
                band_idx = 1;
                ssid = "OpenWrt-5g";
        } else {
                noscan = 1;
                rnr = 1;
                ssid = "OpenWrt-2g";
        }

        print(`set ${s}=wifi-device
set ${s}.type='mac80211'
set ${s}.${id}
set ${s}.band='${lc(band_name)}'
set ${s}.channel='${channel}'
set ${s}.htmode='${htmode}'
set ${s}.country='US'
set ${s}.noscan=${noscan}
set ${s}.disabled=${disabled}

set ${si}=wifi-iface
set ${si}.device='${name}'
set ${si}.network='lan'
set ${si}.mode='ap'
set ${si}.ssid=${ssid}
set ${si}.encryption=${encryption}
set ${si}.mbo=${mbo}

`);

                if (mbssid)
                        print(`set ${s}.mbssid=${mbssid}
`);
                if (rnr)
                        print(`set ${s}.rnr=${rnr}
`);
                if (background_radar)
                        print(`set ${s}.background_radar=${background_radar}
`);
                if (getenv("MT76_ENV_TX_BURST_ZERO"))
                        print(`set ${s}.tx_burst=0.0
`);
                if (encryption == "sae")
                        print(`set ${si}.key=12345678
set ${si}.sae_pwe=2
set ${si}.ieee80211w=2
`);
                if (random_mac_bytes) {
                        print(`set ${si}.macaddr=00:0${idx - 1}:55:66${random_mac_bytes}
`);
                        if (has_mlo)
                                print(`set ${si_mld}.macaddr=00:1${idx - 1}:55:66${random_mac_bytes}
`);
                }
        commit = true;
}

if (commit)
        print("commit wireless\n");
