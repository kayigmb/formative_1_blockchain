#ifndef CRYPTO_H
#define CRYPTO_H

#include "blockchain.h"
#include <openssl/evp.h>

extern EVP_PKEY *pkey;

void initialize_keys(void);
void cleanup_keys(void);

void calculate_hash(Block *block, char output[HASH_SIZE]);

void sign_block(Block *block);

int verify_signature(Block *block);

#endif
