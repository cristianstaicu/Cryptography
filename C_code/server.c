#include "common.h"
#include "stdio.h"
#include <openssl/bn.h>
#include <openssl/bio.h>
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

void get_rsa_serv_private_key(BIGNUM** s_puk, BIGNUM** n){
	FILE *filepointer;
	filepointer = fopen("C_code/server_folder/server_rsa_private_key.txt","r");

	char* s_puk_str = read_word(filepointer);
	char* n_str =  read_word(filepointer);
	BN_dec2bn(s_puk, s_puk_str);
	BN_dec2bn(n, n_str);
}


int open_fifo(const char * pathname)
{
	/* Recreat xe the FIFO in pathname */
	unlink(pathname);
	if( mkfifo(pathname,0600) < 0 ) {
		perror("mkfifo()");
		exit(1);
	}

	/* Open the channel  */
	return (open_channel(pathname));
}

int wait_connection(int channel_fd)
{
	ssize_t msg_size;
	u_int8_t * buff;

	/* Wait a connection from the client */
	if( 	((msg_size = read_msg(channel_fd,&buff)) < 0) ||
		(msg_size != strlen(CONNECTION_STRING) ||
		strncasecmp((const char *)CONNECTION_STRING,(const char *)buff,msg_size)) ) {
		/* TODO Protocol error - if it doesn't read 'Hello' return -1*/
		return (-1);
	}

	return (0);
}

int main(int argc, char ** argv)
{
	int sc_fifo_fd, cs_fifo_fd;
	u_int8_t * buff;
 
	/* Mandatory arguments */
	if( !argv[1] || !argv[2] || !argv[3] ) {
		fprintf(stderr,"server [server->client fifo] [client->server fifo] [password file]\n");
		exit(1);
	}

	/* Main loop */
	do {
		/* Open the server->client fifo */
		fprintf(stderr,"Opening server->client fifo...\n");
		sc_fifo_fd = open_fifo(argv[1]);

		/* Open the client->server fifo */
		fprintf(stderr,"Opening client->server fifo...\n");
		cs_fifo_fd = open_fifo(argv[2]);

		/* Wait for a connection */
		fprintf(stderr,"Waiting connection...\n");
		if( wait_connection(cs_fifo_fd) < 0 ) {
			fprintf(stderr,"Communication error...\n");
			goto next;
		}
		
		/* Write OK */
		write_OK(sc_fifo_fd);
     
	  	/* Server authentication */
    // GET private rsa key of S, (s_prk,n) from "server_folder/server_rsa_private_key.txt"
		FILE* file;
		BIGNUM* priv_key = BN_new();
		BIGNUM* n = BN_new();
		get_rsa_serv_private_key(&n, &priv_key);

    // READ c from S
		int len = read_msg(cs_fifo_fd, &buff);
		char *buf_str = (char *)malloc((len+1) * sizeof(char));
		int i;
		for (i = 0; i < len; i++) {
			buf_str[i] = (char)buff[i];
		}
		print_buff(buff, len);
		BIGNUM *enc_r = BN_new();
		BN_dec2bn(&enc_r, buf_str);
		printf("Received encrypted random number: %s, %d characters\n", BN_bn2dec(enc_r), len);

    // DECRYPT c using (s_prk,n) -> r' = c^s_prk mod n
		BIGNUM *r = BN_new();
		r = enc_dec(enc_r, priv_key, n);
		char *r_enc_str = BN_bn2dec(r);
		printf("Decrypted random number, r'=%s\n", r_enc_str);
    // SEND r' to C
		write_msg(cs_fifo_fd, r_enc_str, strlen(r_enc_str) + 1);

	  /* Client authentication */
    // READ client name nm of C
		u_int8_t * buff;
		len = read_msg(cs_fifo_fd, &buff);
		char *buf_username = (char *)malloc((len+1) * sizeof(char));
		for (i = 0; i < len; i++) {
			buf_username[i] = (char)buff[i];
		}
		printf("Server reads: %s", buf_username);
		print_buff(buff, len);
    // GET the public rsa keys of the possible clients associated to each name, (names[],c_puk[],n[]) from "client_folder/clients_rsa_public_keys.txt"
    /* ... */
    // EXTRACT from (names[],c_puk[],n[]) the pair (c_puk[i],n[i]) where names[i] = nm
    /* ... */
    // CREATE a pseudo-random message r
    /* ... */
    // ENCRYPT r using s_puk[i] -> c = r^s_puk[i] mod n[i]
    /* ... */
    // WRITE c to C
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
		/* Close current connection */
		fprintf(stderr,"Closing connection...\n");
		write_BYE(sc_fifo_fd);
		close_channel(sc_fifo_fd);
		close_channel(cs_fifo_fd);
	} while(1);

	close_channel(sc_fifo_fd);
	close_channel(cs_fifo_fd);

	exit(0);
}
