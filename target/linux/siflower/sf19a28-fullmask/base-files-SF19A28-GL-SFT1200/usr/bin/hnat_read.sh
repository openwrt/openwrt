#!/bin/sh

devmem 0x10804008 32 0x11
devmem 0x10804008 32 0x01

index=0
addr=0x10804100

echo "===== start dump hnat count ====="
while(true)
do
        printf "base:0x%x " $addr
        aa=`devmem $addr`
        printf "value:%d \n" $aa
        addr=$(($addr + 4))
        index=$(($index + 1))
        if [ $index -eq 60 ]; then
                break;
        fi
done
echo "===== end dump hnat count ====="
