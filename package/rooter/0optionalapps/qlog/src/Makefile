SOURCES =  main.c asr.c mdm.c tty2tcp.c sahara.c

linux: clean
	${CC} $(SOURCES) $(CFLAGS) -Wall -o QLog -lpthread -ldl

clean:
	rm -rf QLog *.exe *.dSYM *.obj *.exp .*o *.lib *~ libs out

