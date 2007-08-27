/*
* FOXMP3 
* Copyright (c) 2006 acmesystems.it - john@acmesystems.it
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

#include "mp3_file.h"
#include "mp3_stream.h"
#include "mp3_nix_socket.h"
#include "mp3_tcp_socket.h"
#include "mp3_common.h"
#include "mp3_statemachine.h"
#include "mp3_socket_parser.h"
#include "mp3_misc.h"
#include "mp3_playtime.h"
#include "mp3_states.h"

void state_event(int event, EVENT_PARAM *param);
EVENT_PARAM* state_new_event(unsigned char *text, int numeric);
