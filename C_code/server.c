#include "common.h"
#include "stdio.h"
#include <openssl/bn.h>
#include <openssl/bio.h>
#include "rsa.c"
#include "prime_generator.h"
#include "all5.h"
#include "maj5.h"

char * read_word(FILE *f) {
	char word[10000];
	int i = 0, j;
	char c;
	do {
		c = fgetc(f);
	} while (c == ' ' || c == '\n' || c == '\r');

	do {
		word[i] = c;
		i++;
		c = fgetc(f);
	} while (c != EOF && c != ' ' && c != '\n' && c != '\r');
	char *res = (char *) malloc((i + 1) * sizeof(char));
	for (j = 0; j < i; j++) {
		res[j] = word[j];
	}
	res[i] = 0;
	return res;
}

void get_rsa_serv_private_key(BIGNUM** s_puk, BIGNUM** n) {
	FILE *filepointer;
	filepointer = fopen("C_code/server_folder/server_rsa_private_key.txt", "r");

	char* s_puk_str = read_word(filepointer);
	char* n_str = read_word(filepointer);
	BN_dec2bn(s_puk, s_puk_str);
	BN_dec2bn(n, n_str);
}

void get_client_public_key(char *name_client, BIGNUM** s_puk, BIGNUM** n) {
	FILE *filepointer;
	filepointer = fopen("C_code/server_folder/clients_rsa_public_keys.txt",
			"r");

	char *name;
	char *s_puk_str;
	char* n_str;
	do {
		name = read_word(filepointer);
		s_puk_str = read_word(filepointer);
		n_str = read_word(filepointer);
	} while (strcmp(name, name_client));
	printf("%s %s\n", s_puk_str, n_str);
	BN_dec2bn(s_puk, s_puk_str);
	BN_dec2bn(n, n_str);
}

char * convert_to_chars(char *x) {
	char *res = (char*) malloc(10000 * sizeof(char));
	int i,c,power;
	int j = 0;
	for (i = 0; x[i] != '\0'; i+=8) {
		res[j++] = (x[i] - '0') * 128 + (x[i+1] - '0') * 64 + (x[i+2] - '0') * 32 + (x[i+3] - '0') * 16 + (x[i+4] - '0') * 8
				+ (x[i+5] - '0') * 4 + (x[i+6] - '0') * 2 + (x[i+7] - '0');
	}
	res[j] = 0;
	return res;
}

int open_fifo(const char * pathname) {
	/* Recreat xe the FIFO in pathname */
	unlink(pathname);
	if (mkfifo(pathname, 0600) < 0) {
		perror("mkfifo()");
		exit(1);
	}

	/* Open the channel  */
	return (open_channel(pathname));
}

int wait_connection(int channel_fd) {
	ssize_t msg_size;
	u_int8_t * buff;

	/* Wait a connection from the client */
	if (((msg_size = read_msg(channel_fd, &buff)) < 0)
			|| (msg_size != strlen(CONNECTION_STRING)
					|| strncasecmp((const char *) CONNECTION_STRING,
							(const char *) buff, msg_size))) {
		/* TODO Protocol error - if it doesn't read 'Hello' return -1*/
		return (-1);
	}

	return (0);
}

int check_availability_of_suite(char suite) {
	FILE *filepointer;
		filepointer = fopen("C_code/server_folder/server_cipher_suite_list.txt",
				"r");
	int n = 0;
	do {
		char c;
		n = fscanf(filepointer, "%c\n", &c);
		if (c == suite) {
			return 1;
		}
	} while (n != 0 && n != -1);
	return 0;
}

int main(int argc, char ** argv) {
	int sc_fifo_fd, cs_fifo_fd;
	u_int8_t * buff;

	/* Mandatory arguments */
	if (!argv[1] || !argv[2] || !argv[3]) {
		fprintf(stderr,
				"server [server->client fifo] [client->server fifo] [password file]\n");
		exit(1);
	}

	/* Main loop */
	do {
		/* Open the server->client fifo */
		fprintf(stderr, "Opening server->client fifo...\n");
		sc_fifo_fd = open_fifo(argv[1]);

		/* Open the client->server fifo */
		fprintf(stderr, "Opening client->server fifo...\n");
		cs_fifo_fd = open_fifo(argv[2]);

		/* Wait for a connection */
		fprintf(stderr, "Waiting connection...\n");
		if (wait_connection(cs_fifo_fd) < 0) {
			fprintf(stderr, "Communication error...\n");
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
		char *buf_str = (char *) malloc((len + 1) * sizeof(char));
		int i;
		for (i = 0; i < len; i++) {
			buf_str[i] = (char) buff[i];
		}
		print_buff(buff, len);
		BIGNUM *enc_r = BN_new();
		BN_hex2bn(&enc_r, buf_str);
		printf("Received encrypted random number: %s, %d characters\n",
				BN_bn2hex(enc_r), len);

		// DECRYPT c using (s_prk,n) -> r' = c^s_prk mod n
		BIGNUM *r = BN_new();
		r = enc_dec(enc_r, priv_key, n);
		char *r_enc_str = BN_bn2hex(r);
		printf("Decrypted random number, r'=%s\n", r_enc_str);
		// SEND r' to C
		write_msg(sc_fifo_fd, r_enc_str, strlen(r_enc_str) + 1);

		/* Client authentication */
		// READ client name nm of C
		u_int8_t * buff;
		len = read_msg(cs_fifo_fd, &buff);
		char *buf_username = (char *) malloc((len + 1) * sizeof(char));
		for (i = 0; i < len; i++) {
			buf_username[i] = (char) buff[i];
		}
		printf("Server reads client username: %s\n", buf_username);
		print_buff(buff, len);
		// GET the public rsa keys of the possible clients associated to each name, (names[],c_puk[],n[]) from "client_folder/clients_rsa_public_keys.txt"
		// EXTRACT from (names[],c_puk[],n[]) the pair (c_puk[i],n[i]) where names[i] = nm
		BIGNUM* client_public_key = BN_new();
		BIGNUM* client_n = BN_new();
		get_client_public_key(buf_username, &client_n, &client_public_key);
		printf("Client public key for %s is (%s %s)\n", buf_username,
				BN_bn2hex(client_n), BN_bn2hex(client_public_key));
		// CREATE a pseudo-random message r
		primitive_p = initialize("1011011");
		initialize_rand(38362175);
		BIGNUM *rand;
		rand = generate_random_no(5);
		printf("Generated random number: %s\n", BN_bn2hex(rand));
		// ENCRYPT r using s_puk[i] -> c = r^s_puk[i] mod n[i]
		BIGNUM *enc_rs = enc_dec(rand, client_public_key, client_n);
		char *rs_enc_str = BN_bn2hex(enc_rs);
		printf("Encrypted random number: %s\n", rs_enc_str);
		// WRITE c to C
		/* ... */
		write_msg(sc_fifo_fd, rs_enc_str, strlen(rs_enc_str) + 1);
		// READ r' from C
		len = read_msg(cs_fifo_fd, &buff);
		buf_str = (char *) malloc((len + 1) * sizeof(char));
		for (i = 0; i < len; i++) {
			buf_str[i] = (char) buff[i];
		}
		print_buff(buff, len);
		BIGNUM *dec_r = BN_new();
		BN_hex2bn(&dec_r, buf_str);
		printf("Received encrypted r' from client: %s, %d characters\n",
				BN_bn2hex(dec_r), len);
		if (strcmp(BN_bn2hex(dec_r), BN_bn2hex(rand)) == 0) {
			printf("CLIENT AUTHENTICATION SUCCESFULL!!\n");
		} else {
			printf("Client auth. failed!\n");
			goto next;
		}

		/* Negotiation of the cipher suite */
		len = read_msg(cs_fifo_fd, &buff);
		char cipher_suite = buff[0];
		printf("Received cipher suite from the client: %c %d\n", cipher_suite, len);
		if (!check_availability_of_suite(cipher_suite)) {
			printf("Unknown suite used by client!\n");
			goto next;
		}
		/* Negotiation of the private key */
		BIGNUM *key;
		if (cipher_suite == 'A' || cipher_suite == 'B') {
			/* Generate 24 bits key*/
			key = generate_random_no(24);
		} else {
			/* Generate 64 bits key*/
			key = generate_random_no(64);
		}
		char *key_string = BN_bn2hex(key);
		printf("The key generated by server is : %s\n", key_string);
		enc_rs = enc_dec(key, client_public_key, client_n);
		rs_enc_str = BN_bn2hex(enc_rs);
		printf("Encrypted key: %s\n", rs_enc_str);
		// WRITE c to C
		write_msg(sc_fifo_fd, rs_enc_str, strlen(rs_enc_str) + 1);

		/* Encrypt communication */
		/* Read encrypted message*/
		len = read_msg(cs_fifo_fd, &buff);
		printf("Encrypted message + hashed msg received from client: %s\n", buff);
		buff = hex_to_binary(buff);
		len = strlen(buff);
		printf("Encrypted message + hashed msg received binary: %s\n", buff);
		char *msg_ciphered = (char *)malloc(10000 * sizeof(char));
		char *hashed_message = (char *)malloc(10000 * sizeof(char));
		for (i = 0; i < len - 160; i++) {
			msg_ciphered[i] = buff[i];
		}
		msg_ciphered[i] = 0;
		printf("Encrypted message msg received from client: %s\n", msg_ciphered);
		for (i = len - 160; i < len; i++) {
			hashed_message[i - len + 160] = buff[i];
		}
		hashed_message[160] = 0;
		printf("Hashed message from client: %s\n", hashed_message);
		char decrypted_message[1000];
//		if (cipher_suite == 'A') {
//			/* BUNNY */
//			polynom c = initialize(msg_ciphered);
//			polynom k = initialize(hex_to_binary(key_string));
//			polynom m = DecBunnyTn(c, k);
//			for (i = 0; i < m.size; i++) {
//				decrypted_message[i] = m.p[m.size - i - 1] + '0';
//			}
//			decrypted_message[m.size] = 0;
		if (cipher_suite == 'A' || cipher_suite == 'B') {
			/* BUNNYCBC */
			polynom c = cipher_block_chaining_dec(msg_ciphered, hex_to_binary("000000"), hex_to_binary(key_string));
			for (i = 0; i < c.size; i++) {
				decrypted_message[i] = c.p[c.size - i - 1] + '0';
			}
			decrypted_message[c.size] = 0;
		} if (cipher_suite == 'C' || cipher_suite =='D') {
			/* ALL5 */
			char *res = ALL5DEC(hex_to_binary(key_string), msg_ciphered);
			for (i = 0; i <= strlen(res); i++) {
				decrypted_message[i] = res[i];
			}
			decrypted_message[strlen(res) + 1] = 0;
		} if (cipher_suite == 'E'  || cipher_suite =='F') {
			/* MAJ5 */
			printf("KEY=%s\n",hex_to_binary(key_string));
			printf("MSG=%s\n",msg_ciphered);
			char *res = MAJ5DEC(hex_to_binary(key_string), msg_ciphered);
			for (i = 0; i <= strlen(res); i++) {
				decrypted_message[i] = res[i];
			}
			decrypted_message[strlen(res)] = 0;
		}
//		} else if (cipher_suite == 'E') {
//			/* A51 */
//			printf("KEY=%s\n",hex_to_binary(key_string));
//			printf("MSG=%s\n",msg_ciphered);
//			char *res = A51DEC(hex_to_binary(key_string), msg_ciphered);
//			for (i = 0; i <= strlen(res); i++) {
//				decrypted_message[i] = res[i];
//			}
//			decrypted_message[strlen(res)] = 0;
//		}
		printf("Decrypted message: %s\n", convert_to_chars(decrypted_message));
//		len = read_msg(cs_fifo_fd, &buff);
		char *computed_hash = SPONGEBUNNY(decrypted_message);
		printf("Computed Hashed message: %s\n", computed_hash);
		if (strcmp(hashed_message, computed_hash)) {
			printf("CORRUPTED MESSAGE RECEIVED\n");
		} else {
			FILE *filepointer;
			filepointer = fopen("C_code/server_folder/received_messages.txt",
					"a+");
			fprintf(filepointer, "%s\n", convert_to_chars(decrypted_message));
			fclose(filepointer);
		}
		/* Disconnection */

next:
		/* Close current connection */
		fprintf(stderr, "Closing connection...\n");
		write_BYE(sc_fifo_fd);
		close_channel(sc_fifo_fd);
		close_channel(cs_fifo_fd);
	} while (1);

	close_channel(sc_fifo_fd);
	close_channel(cs_fifo_fd);

	exit(0);
}
