%.o : %.c
	gcc -mno-cygwin -c -g -Wall -I ../ -I ../../cbl/src/ -I ../../wpdpack/Include -o $@ $<

etrax100boot : common.o network.o boot_images.o
	gcc -mno-cygwin -g -Wall -L ../../wpdpack/Lib -o $@ $^ -lwpcap -lpacket -lws2_32

clean :
	rm -f *.o etrax100boot
