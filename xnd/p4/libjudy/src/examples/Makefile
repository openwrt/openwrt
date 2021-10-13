CCFLAGS += -ggdb
LIBS = -lJudy

all: Judy1DupCheck Judy1OpCheck

Judy1DupCheck: Judy1Dup.c Judy1DupCheck.c
	$(CC) $(CCFLAGS) -o $@ $^ $(LIBS)

Judy1OpCheck: Judy1Op.c Judy1OpCheck.c
	$(CC) $(CCFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -f Judy1DupCheck Judy1OpCheck *.o 
