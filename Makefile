all: server multiserver client

# define the commands we'll use for compilation and library building
OBJS = libSystem.a

# define useful flags to cc/ld/etc.
CFLAGS = -g -Wall -I. -Iheaders -L. -lSystem -lpthread

server: QueryServer.c $(OBJS)
	gcc  -o queryserver QueryServer.c $(OBJS) $(CFLAGS)

multiserver: MultiServer.c $(OBJS)
	gcc -o multiserver MultiServer.c $(OBJS) $(CFLAGS)

runserver:
	./queryserver -f data_small/ -p 1500

runmultiserver:
	./multiserver -f data_small/ -p 1500

client: QueryClient.c $(OBJS)
	gcc -o queryclient QueryClient.c $(OBJS) $(CFLAGS)

runclient:
	./queryclient 127.0.0.1 1500

rungoldserver: 
	binaries/queryserver data_small/ 1500

rungoldmultiserver: 
	binaries/multiserver data_small/ 1500

rungoldclient: 
	binaries/queryclient 127.0.0.1 1500

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean: FORCE
	/bin/rm -f *.o *~ multiserver queryserver queryclient

FORCE:
