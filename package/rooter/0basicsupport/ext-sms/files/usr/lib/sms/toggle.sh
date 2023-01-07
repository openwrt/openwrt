#!/bin/sh

SET=$1

uci set modem.sms.menable=$SET
uci commit modem

