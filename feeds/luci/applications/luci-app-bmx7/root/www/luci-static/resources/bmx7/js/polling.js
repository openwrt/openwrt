/*
    Copyright © 2011 Pau Escrich <pau@dabax.net>
    Contributors Lluis Esquerda <eskerda@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

    The full GNU General Public License is included in this distribution in
    the file called "COPYING".
*/


/*
	Table pooler is a function to easy call XHR poller. 

	new TablePooler(5,"/cgi-bin/bmx7-info", {'status':''}, "status_table", function(st){
		var table = Array()
		table.push(st.first,st.second)
		return table
	}

	The parameters are: 
		polling_time: time between pollings
		json_url: the json url to fetch the data
		json_call: the json call
		output_table_id: the table where javascript will put the data
		callback_function: the function that will be executed each polling_time
	
	The callback_function must return an array of arrays (matrix).
	In the code st is the data obtained from the json call
*/

function TablePooler (time, jsonurl, getparams, div_id, callback) {
	this.div_id = div_id;
	this.div = document.getElementById(div_id);
	this.callback = callback;
	this.jsonurl = jsonurl;
	this.getparams = getparams;
	this.time = time;

	this.start = function(){
		XHR.poll(this.time, this.jsonurl, this.getparams, function(x, st){
			var data = this.callback(st);
			var content;
			for (var i = 0; i < data.length; i++){
				rowId = "trDiv_" + this.div_id + i;
				rowDiv = document.getElementById(rowId);
				if (rowDiv === null) {
					rowDiv = document.createElement("div");
					rowDiv.id = rowId;
					rowDiv.className = "tr";
					this.div.appendChild(rowDiv);
				}
				for (var j = 0; j < data[i].length; j++){
					cellId = "tdDiv_" + this.div_id + i + j;
					cellDiv = document.getElementById(cellId);
					if (cellDiv === null) {
						cellDiv = document.createElement("div");
						cellDiv.id = cellId;
						cellDiv.className = "td";
						rowDiv.appendChild(cellDiv);
					}
					if (typeof data[i][j] !== 'undefined' && data[i][j].length == 2) {
						content = data[i][j][0] + "/" + data[i][j][1];
					}
					else content = data[i][j];
					cellDiv.innerHTML = content;
				}
			}
		}.bind(this));
	}


	this.start();
}
