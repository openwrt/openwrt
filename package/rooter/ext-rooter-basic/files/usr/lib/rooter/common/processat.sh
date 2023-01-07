#!/bin/sh

OX=$1

M6=$(echo "$OX" | sed -e "s/      / /g")
M5=$(echo "$M6" | sed -e "s/     / /g")
M4=$(echo "$M5" | sed -e "s/    / /g")
M3=$(echo "$M4" | sed -e "s/   / /g")
M2=$(echo "$M3" | sed -e "s/  / /g")
M2=$(echo "$M2" | sed -e "s/TAC:/   /;s/Tx Power:/   /;s/SINR/   /;s!SYSINFOEX:!SYSINFOEX: !;s!CNTI:!CNTI: !;s!SELRAT:!SELRAT: !;s!ZSNT:!ZSNT: !")
M1=$(echo "$M2" | sed -e "s!Car0 Tot Ec/Io!+ECIOx!;s!Car1 Tot Ec/Io!+ECIO1x!;s!Car0 RSCP!+RSCPx!;s!+CGMM: !!")
M2=$(echo "$M1" | sed -e "s!Car1 RSCP!+RSCP1x!;s!CSNR:!CSNR: !;s!RSSI (dBm):!RSSI4: !;s!AirCard !AirCard!;s!USB !USB!")
M3=$(echo "$M2" | sed -e "s!RSRP (dBm):!RSRP4: !;s!RSRQ (dB):!RSRQ4: !;s!LTERSRP:!LTERSRP: !;s!Model:!+MODEL: !;s!SYSCFGEX:!SYSCFGEX: !")
M7=$(echo "$M3" | sed -e "s!RX level Carrier 0 (dBm):!RSSI3: !;s!RX level Carrier 1 (dBm):!RSSI13: !;s!SYSCFG:!SYSCFG: !;s!  ! !g")
OX=$(echo "$M7" | sed -e "s!WCDMA channel:!UMTS:!;s!SYSINFO:!SYSINFO: !;s!+PSRAT:!+PSRAT: !;s!+MODODR:!+MODODR: !")
echo "$OX"