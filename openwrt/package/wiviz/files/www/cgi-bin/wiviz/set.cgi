#!/bin/sh
httpd -d $QUERY_STRING > /tmp/wiviz-cfg
killall -USR2 wiviz
