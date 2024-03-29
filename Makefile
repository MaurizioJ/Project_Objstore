
DIR_PATH = ./data/*
SOCKET_PATH = ./objstore.sock
FILE_LOG = ./testout.log

FILE_SCRIPT = ./testsum.sh

CC = gcc
AR = ar
CFLAGS += -std=c99 -std=gnu99 -Wall -pedantic -g
ARFLAGS = rvs
INCLUDES = -I.
LDFLAGS  = -L.
LIBS = -pthread -lm

#targets
TARGETS		= libostore.a \
			objstore \
		   client


OBJECTS		= lib.o


.PHONY: all clean cleanall real_test test
.SUFFIXES: .c .h


%: %.c
	$(CC) $(CFLAGS) $(INCLUDES) $(OPTFLAGS) -o $@ $< $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) $(OPTFLAGS) -c -o $@ $<

all: $(TARGETS)


objstore: objstore.o 
	$(CC) $(CFLAGS) $(INCLUDES) $(OPTFLAGS) -o $@ $^ $(LIBS)

client: client.o libostore.a 
	$(CC) $(CFLAGS) $(INCLUDES) $(OPTFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS) -lostore


libostore.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $^



clean:
	rm -f $(TARGETS)
	rm -f $(OBJECTS)
	rm -f client.o
	rm -f objstore.o
	rm -f libostore.a

cleanall: clean
	rm -f *.o libostore.a $(SOCKET_PATH)
	rm -f $(FILE_LOG)
	rm -fr  $(DIR_PATH)



real_test:
	make cleanall
	make all
	@chmod +x $(FILE_SCRIPT)
	@./objstore &
	@$(FILE_SCRIPT)

# lancio il test in silent mode
test:
	make -s real_test



