#include "field.h"

/* The primitive polynom */
polynom primitive_p;

polynom BunnyTn(polynom m, polynom k);
polynom DecBunnyTn(polynom c, polynom k);
polynom concat(polynom *pieces, char no);
polynom INV_Sbox1(polynom x);
polynom INV_Sbox2(polynom x);
polynom INV_Sbox3(polynom x);
polynom INV_Sbox4(polynom x);
polynom cipher_block_chaining_enc(char *message, char *iv, char *key);
polynom cipher_block_chaining_dec(char *ciphertext, char *iv, char *key);
polynom* compute_keys(polynom key);
polynom* MixingLayer(polynom *x);
polynom* InvMixingLayer(polynom *x);
polynom* split(polynom p, char pieces);
