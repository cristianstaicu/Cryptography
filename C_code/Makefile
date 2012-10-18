SERVER_OBJS=Debug/objs/server.o Debug/objs/common.o
CLIENT_OBJS=Debug/objs//client.o Debug/objs/common.o
BUNNY_OBJS=Debug/objs/bunny.o Debug/objs/field.o
TESTS_OBJS=Debug/objs/tests.o Debug/objs/field.o
CC=gcc
CFLAGS=-Wall
OBJDIR=Debug/objs
SDIR=C_code
        
all: server client tests

$(OBJDIR)/%.o: $(SDIR)/%.c 
	$(CC) -c $(INC) -o $@ $< $(CFLAGS) 

client: $(CLIENT_OBJS)
	$(CC) $(CLIENT_OBJS) -o Debug/binaries/$@

server: $(SERVER_OBJS)
	$(CC) $(SERVER_OBJS) -o Debug/binaries/$@

#bunny: $(BUNNY_OBJS)
#	$(CC) $(BUNNY_OBJS) -o Debug/binaries/$@

tests: $(TESTS_OBJS)
	$(CC) $(TESTS_OBJS) -o Debug/binaries/$@

clean:
	rm -f $(CLIENT_OBJS) $(SERVER_OBJS) ${BUNNY_OBJS} ${TESTS_OBJS}
	rm -f Debug/binaries/*
