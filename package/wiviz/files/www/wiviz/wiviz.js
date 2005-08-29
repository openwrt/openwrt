/*
This file is part of Wi-viz (http://wiviz.natetrue.com).

Wi-viz is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License v2 as published by
the Free Software Foundation.

Wi-viz is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wi-viz; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
var mv = 353;
var stupid = eval('window.attachEvent') ? 1 : 0;
var hosts;
var idle_timeout = 20;
var erase_timeout = 35;
var skew_x = 0; skew_y = 0;
var listening = 1;
var wiviz_cgi_url = "/cgi-bin/wiviz/get.cgi";

//What? You mean the POSIX thread API hasn't been ported to Javascript?  Bugger.
function scan_thread() {
	var loc = document.getElementById('wivizGetFrame').contentWindow.location;
	if (!listening) return;
	if (loc.href != wiviz_cgi_url) {
		loc.replace(wiviz_cgi_url);
	}
	else {
		loc.reload(true);
	}
	setTimeout("scan_thread()", 5000);
}

function toggleListen() {
	statusel = document.getElementById('status');
	statusbutton = document.getElementById('togglelisten');
	listening = 1 - listening;
	if (listening) {
	    statusel.innerHTML = "Monitoring";
	    statusbutton.value = "Stop monitoring";
	    document.getElementById('content').innerHTML = '';
	    scan_thread();
	}
	else {
	    statusel.innerHTML = "Stopped";
	    statusbutton.value = "Start monitoring";
	}
}

function channelSet() {
	channelset = document.getElementById('channelsel').value;
	if (channelset == 'hop') {
	    document.getElementById('hopoptions').style.display = 'inline';
	}
	else {
	    document.getElementById('hopoptions').style.display = 'none';
	    if (channelset != 'nochange') document.forms[0].submit();
	}
}

function mousenter(e) {
	if (stupid) e = event;
	el = stupid ? e.srcElement : e.currentTarget;
	el.parentNode.parentNode.className = 'hostdiv_hov';
	el.nextSibling.nextSibling.nextSibling.style.visibility = 'visible';
}

function mouseout(e) {
	if (stupid) e = event;
	el = stupid ? e.srcElement : e.currentTarget;
	el.parentNode.parentNode.className = 'hostdiv';
	el.nextSibling.nextSibling.nextSibling.style.visibility = 'hidden';
}

function generate_mnemonic(hash) {
	c = new Array('b','c','d','f','g','h','j','k','l','m','n','p','qu','r','s',
		't','v','w','y','z','th','ch','sh','cc','rr');
	v = new Array('a','e','i','o','u','ae','ai','ao','au','eo','ei','eu','iu','oa','oe');
	var i, a;
	var p = hash & 1;
	var n = '';
	for (i = 0; i < 4; i++) {
		a = p ? c : v;
		n += a[hash % a.length];
		hash += a.length << 3 + a.length / 2;
		hash *= hash;
	  p = 1 - p;
	}
	return n;
}

function mkhash(mac) {
	var	macarr = mac.split(/:/);
  var hash = 0;
		for (j = 0; j < 6; j++) {
			hash += parseInt(macarr[j]) * j << j;
			hash += 11;
		}
	if (hash < 0) hash = -hash;
	return hash;
}

function wiviz_callback(mhosts, cfgstring) {
	var nh = '';
	hosts = mhosts;
	for (i = 0; i < hosts.length; i++) {
	  hs = hosts[i];
	  if (hs.length == 0) break;
    hs.mac = hs[0];
    hs.rssi = hs[1];
    hs.desc = hs[2];
    hs.descarr = hs.desc.split(/-/)
    hs.age = hs[3];
		hs.hash = mkhash(hs.mac);
    hs.mnem = generate_mnemonic(hs.hash)
    hs.name = hs.mnem;
    el = document.getElementById(hs.mnem);
    if (el) {
      if (hs.age > erase_timeout) {
        el.parentNode.removeChild(el);
        continue;
      }
	el.innerHTML = genHTML(hs);
    }
    else {
      if (hs.age > erase_timeout) continue;
			hs.x = Math.sin(hs.hash / mv) * hs.rssi * 2 - 67;
			hs.y = Math.cos(hs.hash / mv) * hs.rssi * 2;
			nh += "<div class='hostdiv' id='" + hs.mnem + "' style='top: ";
			nh += parseInt(hs.y) + "px; left: " + parseInt(hs.x) + "px'>";
		  nh += genHTML(hs) + "</div>";
    }
	}
	document.getElementById('content').innerHTML += nh;
	
	cfgarr = cfgstring.split(/-/);
	if (cfgarr[1]) {
	    if (cfgarr[1] == 'hopping') cfgarr[1] = 'hop';
	    document.getElementById('channelsel').value = cfgarr[1];
	    if (cfgarr[1] == 'hop') channelSet();
	}
	
	//repip();
	setTimeout("declump(); repip();", 250);
}

function repip() {
  var nh = "";
  if (!hosts) return;
	for (i = 0; i < hosts.length; i++) {
	  hs = hosts[i];
	  if (hs.length == 0) break;
    mac = hs[0];
    rssi = hs[1];
    desc = hs[2].split(/-/);
		if (desc[0] == 'sta' && desc[1] == 'assoc') {
			bss = desc[2];
			hs.apmnem = generate_mnemonic(mkhash(bss));
			ap = document.getElementById(hs.apmnem);
			sta = document.getElementById(hs.mnem);
			if (ap && sta) {
			  x = parseInt(sta.style.left);
			  y = parseInt(sta.style.top);
			  dx = parseInt(ap.style.left) - x;
			  dy = parseInt(ap.style.top) - y;
			  x += 67;
			  y += 10;
			  d = Math.sqrt(dx*dx+dy*dy);
			  for (j = 0; j < d; j += 15) {
			    nh += "<img src='"
					  + ((hs.age < idle_timeout) ? "pip" : "pip-idle")
					  + (stupid ? ".gif" : ".png")
						+ "' class='pip' style='top:"
			      + parseInt(y+dy * j / d) + "; left:"
			      + parseInt(x+dx * j / d) + "'>";
			  }
			}
		}
	}
	document.getElementById('pips').innerHTML = nh;
}

function declump() {
	var c = 0;
	var top = 30000,left = 30000,right = -30000,bottom = -30000;
	for (i = 0; i < hosts.length; i++) {
 	  for (j = 0; j < hosts.length; j++) {
	    if (i == j) continue;
			e1 = document.getElementById(hosts[i].mnem);
			e2 = document.getElementById(hosts[j].mnem);
			if (!e1 || !e2) continue;
			x1 = parseInt(e1.style.left);
			x2 = parseInt(e2.style.left);
			y1 = parseInt(e1.style.top);
			y2 = parseInt(e2.style.top);
			if (x1 < left) left = x1;
			if (y1 < top) top = y1;
			if (x1 > right) right = x1;
			if (y1 > bottom) bottom = y1;
			ox = x2;
			oy = y2;
			dist = Math.sqrt(Math.pow((x1-x2), 2) + Math.pow((y1-y2), 2));
			if (dist == 0) {
			    x2 += Math.random() * 5;
				y2 += Math.random() * 5;
				dist = 10;
			}
			if (dist < 100) {
				cx = (x1-x2) * 5 / (dist / 3);
				cy = (y1-y2) * 5 / (dist / 3);
				x2 -= cx;
				y2 -= cy;
				}
			if (hosts[j].apmnem == hosts[i].mnem
					|| hosts[i].apmnem == hosts[j].mnem) {
			  cx = (x1-x2) * 5 / (dist / 3);
			  cy = (y1-y2) * 5 / (dist / 3);
				if (dist > 150) {
				  x2 += cx;
				  y2 += cy;
				}
			}
			if (Math.abs(ox-x2) > 2 || Math.abs(oy-y2) > 2) {
				e2.style.left = parseInt(x2);
				e2.style.top = parseInt(y2);
				c++;
			}
	  }
	}
	if (top < bottom && left < right) {
	  document.getElementById('debug').innerHTML = left + "," + right + "," + top + "," +bottom;
	  document.getElementById('content').style.left =
		document.getElementById('pips').style.left =
			 -(right - left) / 2 - left - 67;
	  document.getElementById('content').style.top =
	  document.getElementById('pips').style.top =
			 -(bottom - top) / 2 - top - 25;
	}
	repip();
	if (c) setTimeout("declump()", 100);
}

function genHTML(hs) {
	var nh = '';
  nh += "<center><img class='icon' src='"
	a = hs.descarr;
	if (a[0] == 'ap' || a[0] == 'adhoc') {
	  if (a[0] == 'ap') {
			nh += "ap";
			if (a[5] == 'enc') nh += "-wep";
	  }
	  else {
	    nh += "adhoc";
	  }
		hs.channel = a[2];
		hs.name = a[4];
	}
	else if (a[0] == 'sta') {
		nh += "station";
		hs.channel = 0;
	}
	nh += (hs.age < idle_timeout) ? "": "-idle";
	nh += stupid ? ".gif" : ".png";
	nh += "' onmouseover='mousenter(event)' onmouseout='mouseout(event)'"
		+ "><br><span class='hostdesc'>" + hs.mac + "<br><i>'" + hs.name;
	nh += "'</i>";
	if (hs.channel) {
	  nh += " ch" + hs.channel;
	}
	nh += "</span><span class='extrafo'><br>";
	if (a[0] == 'ap') nh += "Access point";
	if (a[0] == 'sta') nh += "Station";
	if (a[0] == 'adhoc') nh += "Logical ad-hoc entity";
	if (a[0] == 'ap' || a[0] == 'adhoc') {
		nh += "<br>";
		if (a[5] == '?enc') nh += "Encryption unknown";
		if (a[5] == 'enc') nh += "Encrypted";
		if (a[5] == 'unenc') nh += "Unencrypted";
		if (a[6] == 'wep') nh += "-WEP";
		if (a[6] == 'wpa') nh += "-WPA";
	}
	nh += "<br>RSSI: " + hs.rssi + " dBm<br>"
		+ "Seen " + hs.age + " seconds ago<br>";
 	nh += "</span></center>";
	return nh;
}
