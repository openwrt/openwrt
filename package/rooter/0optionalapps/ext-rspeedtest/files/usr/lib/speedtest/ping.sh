#!/bin/sh

rm -f /tmp/pinfo

echo "0" > /tmp/getspeed
echo "0" >> /tmp/getspeed
echo "0" >> /tmp/getspeed
echo "0" >> /tmp/getspeed
echo "1" > /tmp/spworking
while IFS= read -r line
do
	read -r line
	read -r line
	read -r line
	read -r line
	read -r line
	break
done < /tmp/sinfo
	
hp=$(httping -c 3 $line)
pingg=$(echo $hp" " | grep -o "round-trip .\+ ms " | tr " " "," | tr "/" ",")
latency=$(echo $pingg | cut -d, -f7)

echo "$latency" > /tmp/pinfo	
echo "2" > /tmp/spworking

