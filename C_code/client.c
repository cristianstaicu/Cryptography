#include "common.h"
#include <stdio.h>

void get_rsa_serv_pub_key (long* s_puk, long* n){
	FILE *filepointer;
	long array_data[2];
	int i = 0;
	char line[80];
	filepointer = fopen("C_code/client_folder/server_rsa_public_key.txt","r");

	while (fgets(line, 80, filepointer)){
		sscanf(line, "%ld", &array_data[i]);
		printf("%ld\n", array_data[i]);
		i++;
	}
	fclose(filepointer);
	s_puk = array_data[0];
	n = array_data[1];
	printf("Server public key is equal to: %ld\n", s_puk);
	printf("Value n is equal to: %ld\n", n);
}

int main(int argc, char ** argv)
{
	int sc_fifo_fd, cs_fifo_fd;
	long s_puk;
	long n;
	/* Mandatory arguments */
	if( !argv[1] || !argv[2] || !argv[3] || !argv[4] ) {
		fprintf(stderr,"client [server->client fifo] [client->server fifo] [password file] [username]\n");
		exit(1);
	}
	/* Create connection with the server */
	fprintf(stderr,"Create connection...\n");
	sc_fifo_fd = open_channel(argv[1]);
	cs_fifo_fd = open_channel(argv[2]);

	write_msg(cs_fifo_fd,(const u_int8_t *)CONNECTION_STRING,strlen(CONNECTION_STRING));

	/* Read OK */
	if( read_string(sc_fifo_fd,OK_STRING) < 0 ) {
		fprintf(stderr,"Communication error\n");
		goto next;
	}

	/* Server authentication */
	write_msg(cs_fifo_fd,(const u_int8_t *)"A",1);

  // GET public rsa key of S, (s_puk,n), from "client_folder/server_rsa_public_key.txt"
	get_rsa_serv_pub_key (s_puk, n);
	printf("s_puk  = %ld\n", s_puk);

  /* ... */
  // CREATE a random number r
  /* ... */
  // ENCRYPT r using (s_puk,n) -> c = r^s_puk mod n
  /* ... */
  // WRITE c to S
  /* ... */
  // READ r' from C
  /* ... */
  // CHECK if r = r'
  /* ... */

	/* Client authentication */
  // SEND client_name to S
  /* ... */
  // GET private rsa key of C, (s_prk,n) from "client_folder/client_rsa_private_key.txt"
  /* ... */
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
