var alpmp3_playtime = 0;
var alpmp3_state = 0;
var alpmp3_ip = "";
var obj_volume; 
var obj_bass; 
var obj_playtime; 
var obj_sate; 
var obj_filename; 
var MP3Object;
var is_setup = 0;

function alpmp3_ajax_new_data(){
	obj_bass.firstChild.nodeValue = MP3Object.alpmp3.bass;
	obj_volume.firstChild.nodeValue = MP3Object.alpmp3.volume;
	obj_state.firstChild.nodeValue = MP3Object.alpmp3.type;
	obj_filename.firstChild.nodeValue = MP3Object.alpmp3.filename;
	alpmp3_state = MP3Object.alpmp3.state;
	if(Math.abs(alpmp3_playtime - MP3Object.alpmp3.playtime) > 1){
		alpmp3_playtime = MP3Object.alpmp3.playtime;
	}
}

function alpmp3_update_data(url){
	var data;
	var http_request = new XMLHttpRequest();
	http_request.open("GET", url, true);
	http_request.onreadystatechange = function () {
		if (http_request.readyState == 4) {
			if (http_request.status == 200) {
				MP3Object = eval("(" + http_request.responseText + ")");
				alpmp3_ajax_new_data();
			} else {	
				alert("There was a problem with the URL.");
			}
			http_request = null;
		}
	}
	http_request.send(null);
	self.setTimeout("alpmp3_update_data('mp3_json.cgi');", 4000);
}

function alpmp3_remote(cmd){
	var doit = "";	
	switch(cmd){
	case 'volup':
		if(MP3Object.alpmp3.volume < 30){
			MP3Object.alpmp3.volume++;
		}
		doit = "?vol=" + MP3Object.alpmp3.volume;
		break;
	case 'voldown':
		if(MP3Object.alpmp3.volume > 0){
			MP3Object.alpmp3.volume--;
		}
		doit = "?vol=" + MP3Object.alpmp3.volume;
		break;
	case 'bassup':
		if(MP3Object.alpmp3.bass < 30){
			MP3Object.alpmp3.bass++;
		}
		doit = "?bass=" + MP3Object.alpmp3.bass;
		break;
	case 'bassdown':
		if(MP3Object.alpmp3.volume < 30){
			MP3Object.alpmp3.bass--;
		}
		doit = "?bass=" + MP3Object.alpmp3.bass;
		break;
	case 'stop':
		doit = "?stop=1";
		break;
	case 'start':
		doit = "?start=1";
		break;
	case 'next':
		doit = "?next=1";
		break;
	case 'back':
		doit = "?back=1";
		break;
	}
	if(doit != ""){
		var http_request2 = new XMLHttpRequest();
		http_request2.open("GET", 'mp3_cmd.cgi'+doit, true);
		http_request2.onreadystatechange = function () {
			if (http_request2.readyState == 4) {
				if (http_request2.status == 200) {
					alpmp3_ajax_new_data();
				} else {	
					alert("There was a problem with the URL.");
				}
				http_request2 = null;
			}
		}
		http_request2.send(null);
	}

}

function alpmp3_timeout(){
	alpmp3_state = 0;
	alert(alpmp3_playtime);
}

function alpmp3_playtime_update(){
	self.setTimeout("alpmp3_playtime_update()", 1000);
	if(alpmp3_state > 0){
		alpmp3_playtime ++;
	} else {
		alpmp3_playtime = 0;
	}
	var s = alpmp3_playtime;
	var h = 0;
	var m = 0;
	while(s > 3599){
		h++;
		s -= 3600;
	}
	while(s > 59){
		m++;
		s -= 60;
	}
	ptime = ((m < 10) ? "0" : "") + m + ":" + ((s < 10) ? "0" : "") + s;
	if(h > 0){
		ptime = ((h < 10) ? "0" : "") + h + ":" + ptime;
	}
	obj_playtime.firstChild.nodeValue = ptime;
}

function alpmp3_setup($ip){
	if(is_setup == 0){
		obj_volume = document.getElementById("alpmp3_volume");
		obj_bass = document.getElementById("alpmp3_bass");
		obj_state = document.getElementById("alpmp3_state");
		obj_filename = document.getElementById("alpmp3_filename");
		obj_playtime = document.getElementById("alpmp3_playtime");
		is_setup = 1;
	}
	self.setTimeout("alpmp3_update_data('mp3_json.cgi');", 4000);
	self.setTimeout("alpmp3_playtime_update()", 1000);
}
