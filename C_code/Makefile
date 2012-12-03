SERVER_OBJS=Debug/objs/server.o Debug/objs/common.o
CLIENT_OBJS=Debug/objs/field.o Debug/objs/lsfr.o Debug/objs/bunny.o Debug/objs/all5.o Debug/objs/a51.o  Debug/objs/sponge.o Debug/objs/prime_generator.o  Debug/objs/client.o Debug/objs/common.o
RSA_OBJS=Debug/objs/field.o Debug/objs/lsfr.o Debug/objs/bunny.o Debug/objs/all5.o Debug/objs/a51.o  Debug/objs/sponge.o Debug/objs/prime_generator.o  Debug/objs/rsa_key_generation.o
TESTS_OBJS=Debug/objs/field.o Debug/objs/lsfr.o Debug/objs/bunny.o Debug/objs/all5.o Debug/objs/a51.o  Debug/objs/sponge.o Debug/objs/prime_generator.o Debug/objs/tests.o   
CC=gcc
CFLAGS=--std=c99 -g -pedantic -Wall -lm
OBJDIR=Debug/objs
SDIR=C_code
        
all: server client rsa tests

$(OBJDIR)/%.o: $(SDIR)/%.c 
	$(CC) -c $(INC) -o $@ $< $(CFLAGS) -lcrypto

client: $(CLIENT_OBJS)
	$(CC) $(CLIENT_OBJS) -o Debug/binaries/$@ -lcrypto

server: $(SERVER_OBJS)
	$(CC) $(SERVER_OBJS) -o Debug/binaries/$@ -lcrypto

rsa: $(RSA_OBJS)
	$(CC) $(RSA_OBJS) -o Debug/binaries/$@ -lcrypto

#inverse: $(INVERSE_OBJS)
#	$(CC) $(INVERSE_OBJS) -o Debug/binaries/$@

tests: $(TESTS_OBJS)
	$(CC) $(TESTS_OBJS) -o Debug/binaries/$@ -lcrypto


clean:
	rm -f $(CLIENT_OBJS) $(SERVER_OBJS) ${BUNNY_OBJS} ${TESTS_OBJS} ${A51_OBJS}
	rm -f Debug/binaries/*
