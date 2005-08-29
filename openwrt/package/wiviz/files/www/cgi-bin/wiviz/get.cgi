#!/bin/sh

WIVIZ_PATH=wiviz

echo Content-type: text/html
echo 
killall -USR1 wiviz >/dev/null 2>&1
if [ 0 -ne $? ]
 then #### Wi-Viz daemon not running, start it
  $WIVIZ_PATH >/dev/null </dev/null 2>&1 &
  killall -USR1 wiviz > /dev/null
 fi
echo "<html><head><script language='JavaScript1.2'>"
cat /tmp/wiviz-pipe
echo "</script></head><body></body></html>"
