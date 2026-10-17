#!/usr/bin/ucode
// MAVLink RADIO_STATUS sender for QCA9533 Smart Radio.
// Subscribes to ubus "linkstate" events, extracts RSSI, sends MAVLink
// RADIO_STATUS (msg_id=109) via UDP to the configured GCS port.
let ubus = require("ubus").connect();
let uci = require("uci").cursor();
let fs = require("fs");
let socket = require("socket");

let gcs_ip = uci.get("mavlink", "settings", "gcs_ip") ?? "255.255.255.255";
let udp_port = +(uci.get("mavlink", "settings", "udp_port") ?? 14550);
let sysid = +(uci.get("mavlink", "settings", "sysid") ?? 51);
let compid = +(uci.get("mavlink", "settings", "compid") ?? 68);

let seq = 0;
let sock = socket.open(socket.AF_INET, socket.SOCK_DGRAM);

// MAVLink v1 RADIO_STATUS (msg_id=109, payload 7 bytes)
function send_radio_status(rssi_dbm, noise_dbm) {
	// Map dBm to 0-254 scale: -120 dBm=0, 0 dBm=254
	let rssi_u8 = rssi_dbm ? max(0, min(254, rssi_dbm + 120) * 254 / 120) : 0;
	rssi_u8 = int(rssi_u8);
	let noise_u8 = noise_dbm ? max(0, min(254, noise_dbm + 120) * 254 / 120) : 0;
	noise_u8 = int(noise_u8);

	// MAVLink v1 header: STX(0xFE), len, seq, sysid, compid, msgid
	let pkt = chr(0xFE, 7, seq & 0xFF, sysid, compid, 109);
	// RADIO_STATUS payload: rssi, remrssi, txbuf, noise, remnoise, rxerrors(u16), fixed(u16)
	pkt += chr(rssi_u8, 0, 100, noise_u8, 0);
	pkt += chr(0, 0);  // rxerrors u16 LE
	pkt += chr(0, 0);  // fixed u16 LE

	// CRC (X.25 with RADIO_STATUS seed byte 0xBC)
	let crc = 0xFFFF;
	for (let i = 1; i < length(pkt); i++) {
		let b = ord(pkt, i);
		let tmp = b ^ (crc & 0xFF);
		tmp = (tmp ^ (tmp << 4)) & 0xFF;
		crc = (crc >> 8) ^ (tmp << 8) ^ (tmp << 3) ^ (tmp >> 4);
		crc &= 0xFFFF;
	}
	// Extra CRC byte for msg_id 109 = 0xBC
	let extra = 0xBC;
	let tmp = extra ^ (crc & 0xFF);
	tmp = (tmp ^ (tmp << 4)) & 0xFF;
	crc = (crc >> 8) ^ (tmp << 8) ^ (tmp << 3) ^ (tmp >> 4);
	crc &= 0xFFFF;

	pkt += chr(crc & 0xFF, (crc >> 8) & 0xFF);
	seq++;

	sock.send(pkt, 0, { family: socket.AF_INET, address: gcs_ip, port: udp_port });
}

// Subscribe to linkstate ubus events
ubus.subscribe("linkstate", function(type, data) {
	if (type != "linkstate") return;
	let rssi = null;
	let noise = data?.noise;
	if (data?.sta_stats && length(data.sta_stats) > 0) {
		rssi = data.sta_stats[0].rssi;
	}
	send_radio_status(rssi, noise);
});

// uloop keeps process alive waiting for events
require("uloop").run();
