/*
* FOXMP3 
* Copyright (c) 2007 acmesystems.it - john@acmesystems.it
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA02111-1307USA
*
* Feedback, Bugs...  info@acmesystems.it 
*
*/ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <netdb.h>

#include <json.h>

#define SOCKET_PATH "/tmp/foxmp3"

void print_http_header(){
	printf("Content-type: text/html\n\n");
}

int read_parameter(char *name, char *value, int maxlen){
	char *pos1, *pos2;
	char *query_string = getenv("QUERY_STRING");
	int success = 0;
  
	if(query_string){
		pos1 = strstr(query_string, name);
		if(pos1){
			pos1 += strlen(name) + 1;
			pos2 = strstr(pos1, "&");
			if(pos2){
				*pos2 = '\0';
			}
			if(strlen(pos1) >= maxlen){
				pos1[maxlen] = '\0';
			}
			strcpy(value, pos1);
			success = 1;
		}    
	}
	return success;
}


int issue_command(unsigned char *str){
	int s, t, len;
	struct sockaddr_un remote;

	if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
		exit(1);
	}
	remote.sun_family = AF_UNIX;
	strcpy(remote.sun_path, SOCKET_PATH);
	len = strlen(remote.sun_path) + sizeof(remote.sun_family);
	if (connect(s, (struct sockaddr *)&remote, len) == -1) {
		return 1;
	}
	if (send(s, str, strlen(str), 0) == -1) {
		return 1;
	}
	unsigned char loop = 1;
	while(loop){
		if ((t=recv(s, str, 2048, 0)) > 0) {
			str[t] = '\0';
			if((strstr(str, "OK\n")) || (strstr(str, "ERROR\n"))){
				loop = 0;
			}
		} else {
			return 1;
		}
	}
	close(s);
	return 0;
}

void handle_cmd(){
	unsigned char param[128];
	unsigned char cmd[256];
	int i;
	*cmd = '\0';
	if(read_parameter("vol", param, 128)){
		i = atoi(param);
		i = 120 - (i * 4);
		sprintf(cmd, "VOLUME %d", i);
	}
	if(read_parameter("bass", param, 128)){
		i = atoi(param);
		sprintf(cmd, "BASS %d", i);
	}
	if(read_parameter("start", param, 128)){
		sprintf(cmd, "START");
	}
	if(read_parameter("stop", param, 128)){
		sprintf(cmd, "STOP");
	}
	if(read_parameter("next", param, 128)){
		sprintf(cmd, "NEXT");
	}
	if(read_parameter("back", param, 128)){
		sprintf(cmd, "BACK");
	}	
	if(*cmd){
		issue_command(cmd);
	}
}

void print_html_remote(){
	char name[128];
	gethostname(name, 128);
	printf("<html><head><title>foxmp3 - %s - remote</title><link rel=\"stylesheet\" href=\"../local/stylesheet.css\" type=\"text/css\" /><script src=\"../local/json.js\" type=\"text/javascript\"></script></head><body onload=\"alpmp3_setup();\">");
	printf("<center><p id=alpmp3_remote>");
	printf("<table width=50%%>");
	printf("<tr class=cellhead><th colspan=3 id=cell align=center>FOXMP3 - %s </td></tr>", name);
	printf("<tr class=cellone><td>Action</td><td colspan=2 id=alpmp3_state>0</td></tr>");
	printf("<tr class=celltwo><td>Filename</td><td colspan=2 id=alpmp3_filename>0</td></tr>");
	printf("<tr class=cellone><td>Playtime</td><td colspan=2 id=alpmp3_playtime>0</td></tr>");
	printf("<tr class=celltwo><td>Volume</td><td id=alpmp3_volume>0</td><td><a href=\"#\" onclick=\"javascript:alpmp3_remote('volup');\">Up</a>&nbsp;&nbsp;<a href=\"#\" onclick=\"javascript:alpmp3_remote('voldown')\">Down</a></td></tr>");
	printf("<tr class=cellone><td width=20%%>Bass</td><td id=alpmp3_bass>0</td><td><a href=\"#\" onclick=\"javascript:alpmp3_remote('bassup');\" class=browse>Up</a>&nbsp;&nbsp;<a href=\"#\" onclick=\"javascript:alpmp3_remote('bassdown')\" class=browse>Down</a></td></tr>");
	printf("<tr class=cellhead><th colspan=3 id=cell align=center><a href=\"#\" onclick=\"javascript:alpmp3_remote('start');\">Start</a>&nbsp;<a href=\"#\" onclick=\"javascript:alpmp3_remote('stop');\">Stop</a>&nbsp;<a href=\"#\" onclick=\"javascript:alpmp3_remote('back');\">Back</a>&nbsp;<a href=\"#\" onclick=\"javascript:alpmp3_remote('next');\">Next</a>&nbsp;</td></tr>");
	printf("</table>");
	printf("</p></center>");
	printf("</body></html>");
}



void print_json_info(){
	unsigned char str[2048];
	sprintf(str, "STATE");
	if(issue_command(str) == 0){
		int state = 0;
		int volume = 0;
		int bass = 0;
		int playtime = 0;
		unsigned char filename[1024];
		unsigned char *p1, *p2;
		
		memset(filename, 0, 1024);
		p1 = str;
		while(p1){
			p2 = strstr(p1, "\n");
			if(p2){
				*p2 = '\0';
				p2 ++;
				// printf("parsing %s\n", p1);
				if(strncmp(p1, "VOLUME", strlen("VOLUME")) == 0){
					volume = atoi(&p1[strlen("VOLUME") + 1]);
					if(volume > 120)
						volume = 120;
					volume = 120 - volume;
					volume /= 4;
					//printf("vol = %d\n", volume);
				} else if(strncmp(p1, "BASS", strlen("BASS")) == 0){
					bass = atoi(&p1[strlen("BASS") + 1]);
					//printf("bass = %d\n", bass);
				} else if(strncmp(p1, "PLAYTIME", strlen("PLAYTIME")) == 0){
					playtime = atoi(&p1[strlen("PLAYTIME") + 1]);
					//printf("playtime = %d\n", playtime);
				} else if(strncmp(p1, "STATE", strlen("STATE")) == 0){
					if(strstr(p1, "MP3_STATE_IDLE")){
						state = 0;
					} else if(strstr(p1, "MP3_STATE_FILE")){
						state = 1;
					} else if(strstr(p1, "MP3_STATE_STREAM")){
						state = 2;
					}
					//printf("state = %d\n", state);
				} else if(strncmp(p1, "STREAM", strlen("STREAM")) == 0){
					strcpy(filename, &p1[strlen("STREAM") + 1]);
					//printf("filename = %s\n", filename);
				} else if(strncmp(p1, "FILE", strlen("FILE")) == 0){
					strcpy(filename, &p1[strlen("FILE") + 1]);
					//printf("filename = %s\n", filename);
				}  
				
				p1 = p2;
			} else {
				p1 = 0;
			}
		}

		struct json_object *alpmp3 = json_object_new_object();
		json_object_object_add(alpmp3, "state", json_object_new_int(state));
		switch(state){
		case 1:
			json_object_object_add(alpmp3, "type", json_object_new_string("file"));
			break;
		case 2:
			json_object_object_add(alpmp3, "type", json_object_new_string("stream"));
			break;
		default:
			json_object_object_add(alpmp3, "type", json_object_new_string("idle"));
			break;
		}
		json_object_object_add(alpmp3, "volume", json_object_new_int(volume));
		json_object_object_add(alpmp3, "bass", json_object_new_int(bass));
		json_object_object_add(alpmp3, "playtime", json_object_new_int(playtime));
		json_object_object_add(alpmp3, "filename", json_object_new_string(filename));	
		struct json_object *jo = json_object_new_object();
		json_object_object_add(jo, "alpmp3", alpmp3);
		printf("\n%s\n", json_object_to_json_string(jo));
	}
}

int main(int argc, char **argv){
	print_http_header();
	
	if(strstr(argv[0], "mp3_remote.cgi")){
		print_html_remote();
	} else if(strstr(argv[0], "mp3_json.cgi")){
 		print_json_info();
	} else if(strstr(argv[0], "mp3_cmd.cgi")){
 		handle_cmd();
	} else {
		printf("Unknown command");
	}
	return 0;
}
