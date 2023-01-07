#!/bin/sh 

# turn power led green
set_gpio 13 1
set_gpio 15 0

# signal that we are waiting for internets
set_gpio 14 0