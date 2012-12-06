#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/bn.h>
#include <openssl/bio.h>
#include "prime_generator.h"
#include "rsa.c"

char * read_word(FILE *f) {
	char word[10000];
	int i = 0, j;
	char c;
	do {
		c= fgetc(f);
	}while (c == ' ' || c == '\n' || c == '\r');

	do {
		word[i] = c;
		i++;
		c = fgetc(f);
	} while (c != EOF && c != ' ' && c != '\n' && c != '\r');
	char *res = (char *)malloc((i+1) * sizeof(char));
	for (j = 0; j < i; j++) {
		res[j] = word[j];
	}
	res[i] = 0;
	return res;
}

void get_rsa_serv_pub_key (BIGNUM** s_puk, BIGNUM** n){
	FILE *filepointer;
	filepointer = fopen("C_code/client_folder/server_rsa_public_key.txt","r");

	char* s_puk_str = read_word(filepointer);
	char* n_str =  read_word(filepointer);
	BN_dec2bn(s_puk, s_puk_str);
	BN_dec2bn(n, n_str);
}

void get_rsa_serv_priv_key (BIGNUM** s_prk, BIGNUM** n){
	FILE *filepointer;
	filepointer = fopen("C_code/client_folder/client_rsa_private_key.txt", "r");
	char* s_prk_str = read_word(filepointer);
	char* n_str = read_word(filepointer);
	BN_dec2bn(s_prk, s_prk_str);
	BN_dec2bn(n, n_str);
}

int main(int argc, char ** argv)
{
	int sc_fifo_fd, cs_fifo_fd;
	BIGNUM* s_puk = BN_new();
	BIGNUM* s_prk = BN_new();
	BIGNUM* n = BN_new();
	/* Mandatory arguments */
	if( !argv[1] || !argv[2] || !argv[3] || !argv[4] ) {
		fprintf(stderr,"client [server->client fifo] [client->server fifo] [password file] [username]\n");
		exit(1);
	}
	/* Create connection with the server */
	fprintf(stderr,"Create connection...\n");
	sc_fifo_fd = open_channel(argv[1]);
	cs_fifo_fd = open_channel(argv[2]);

	write_msg(cs_fifo_fd,(const u_int8_t *)CONNECTION_STRING, strlen(CONNECTION_STRING));

	/* Read OK */
	if( read_string(sc_fifo_fd,OK_STRING) < 0 ) {
		fprintf(stderr,"Communication error\n");
		goto next;
	}

	/* Server authentication */
//	write_msg(cs_fifo_fd,(const u_int8_t *)"A",1);

  // GET public rsa key of S, (s_puk,n), from "client_folder/server_rsa_public_key.txt"
	get_rsa_serv_pub_key (&n, &s_puk);
	printf("Server public key: (%s %s)\n", BN_bn2dec(s_puk), BN_bn2dec(n));
  /* ... */
  // CREATE a random number r
	primitive_p = initialize("1011011");
	initialize_rand(38362177);
	BIGNUM *rand = generate_random_no(5);
//	BIGNUM *rand = BN_new();
//	BN_add_word(rand, 5);
	printf("Generated random number: %s\n", BN_bn2dec(rand));

  // ENCRYPT r using (s_puk,n) -> c = r^s_puk mod n
	BIGNUM *enc_r = enc_dec(rand, s_puk, n);
	//TODO change here
	char *r_enc_str = BN_bn2dec(enc_r);
	printf("Encrypted random number: %s\n", r_enc_str);
  // WRITE c to S
	write_msg(cs_fifo_fd, r_enc_str, strlen(r_enc_str) + 1);

  // READ r' from C
	u_int8_t * buff;
	int len = read_msg(cs_fifo_fd, &buff);
	char *buf_str = (char *)malloc((len+1) * sizeof(char));
	int i;
	for (i = 0; i < len; i++) {
		buf_str[i] = (char)buff[i];
	}
	print_buff(buff, len);
	BIGNUM* dec_r = BN_new();
	BN_dec2bn(&dec_r, buf_str);
	printf("Received decrypted random number: %s, %d characters\n", BN_bn2dec(dec_r), len);
	if (strcmp(BN_bn2dec(dec_r), BN_bn2dec(rand)) == 0) {
		printf("SERVER AUTHENTICATION SUCCESFULL!!\n");
	} else {
		printf("Server auth. failed!\n");
		exit(1);
	}
  // CHECK if r = r'
  /* ... */

	/* Client authentication */
  // SEND client_name to S
	char* username = argv[4];
	printf("client is sending its username: %s\n", username);
	write_msg(cs_fifo_fd, username, strlen(username) + 1);

  // GET private rsa key of C, (s_prk,n) from "client_folder/client_rsa_private_key.txt"
	get_rsa_serv_priv_key(&n, &s_prk);
	printf("Server public key: (%s %s)\n", BN_bn2dec(s_prk), BN_bn2dec(n));
  // READ c from S
  /* ... */
  // DECRYPT c using (c_prk,n) -> r' = c^c_prk mod n
  /* ... */
  // WRITE r' to S
  /* ... */
  // GET private rsa key of C, c_prk from "client_folder/client_rsa_private_key.txt"
  /* ... */
  
  /* Negotiation of the cipher suite */
  /* ... */    

  /* Negotiation of the private key */  
  /* ... */    

  /* Encrypt communication */
  /* ... */    
  
  /* Disconnection */
  /* ... */    



  
next:
	/* Close connection with the server */
	fprintf(stderr,"Closing connection...\n");

	/* Expect BYE */
	if( read_string(sc_fifo_fd,CLOSE_CONNECTION_STRING) < 0 ) {
		fprintf(stderr,"Communication error\n");
		goto next;
	}

	close_channel(cs_fifo_fd);
	close_channel(sc_fifo_fd);
	exit(0);
}
