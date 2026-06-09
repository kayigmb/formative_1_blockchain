#include "crypto.h"
#include <openssl/ec.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/obj_mac.h>
#include <openssl/sha.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SIGNATURE_LENGTH 100

EVP_PKEY *pkey = NULL;

void initialize_keys(void) {
  EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_EC, NULL);
  if (!ctx) {
    fprintf(stderr, "Failed to create PKEY context.\n");
    exit(1);
  }

  if (EVP_PKEY_keygen_init(ctx) <= 0) {
    fprintf(stderr, "Failed to init keygen.\n");
    EVP_PKEY_CTX_free(ctx);
    exit(1);
  }

  if (EVP_PKEY_CTX_set_ec_paramgen_curve_nid(ctx, NID_secp256k1) <= 0) {
    fprintf(stderr, "Failed to set curve.\n");
    EVP_PKEY_CTX_free(ctx);
    exit(1);
  }

  if (EVP_PKEY_keygen(ctx, &pkey) <= 0) {
    fprintf(stderr, "Failed to generate key.\n");
    EVP_PKEY_CTX_free(ctx);
    exit(1);
  }

  EVP_PKEY_CTX_free(ctx);
}

void cleanup_keys(void) {
  if (pkey) {
    EVP_PKEY_free(pkey);
    pkey = NULL;
  }
}

void sha256_string(const char *input, char output[HASH_SIZE]) {
  unsigned char hash[SHA256_DIGEST_LENGTH];

  SHA256((unsigned char *)input, strlen(input), hash);

  for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
    sprintf(output + (i * 2), "%02x", hash[i]);
  }

  output[64] = '\0';
}

void calculate_hash(Block *block, char output[HASH_SIZE]) {
  char data[1024];

  snprintf(data, sizeof(data), "%d%ld%s%s%s%s%d%s%lu%s", block->index,
           block->timestamp, block->student_id, block->full_name,
           block->course_code, block->status, block->token_reward,
           block->transaction_id, block->nonce, block->previous_hash);

  sha256_string(data, output);
}

void sign_block(Block *block) {
  EVP_MD_CTX *ctx = EVP_MD_CTX_new();
  if (!ctx) {
    fprintf(stderr, "Failed to create digest context.\n");
    exit(1);
  }

  if (EVP_DigestSignInit(ctx, NULL, EVP_sha256(), NULL, pkey) <= 0) {
    fprintf(stderr, "Failed to init signing.\n");
    EVP_MD_CTX_free(ctx);
    exit(1);
  }

  if (EVP_DigestSignUpdate(ctx, (unsigned char *)block->hash,
                           strlen(block->hash)) <= 0) {
    fprintf(stderr, "Failed to update signing.\n");
    EVP_MD_CTX_free(ctx);
    exit(1);
  }

  size_t sig_len = 0;
  if (EVP_DigestSignFinal(ctx, NULL, &sig_len) <= 0) {
    fprintf(stderr, "Failed to get signature length.\n");
    EVP_MD_CTX_free(ctx);
    exit(1);
  }

  if (sig_len > MAX_SIGNATURE_LENGTH) {
    fprintf(stderr, "Signature too large for buffer.\n");
    EVP_MD_CTX_free(ctx);
    exit(1);
  }

  if (EVP_DigestSignFinal(ctx, block->signature, &sig_len) <= 0) {
    fprintf(stderr, "Failed to finalize signing.\n");
    EVP_MD_CTX_free(ctx);
    exit(1);
  }

  block->signature_length = (unsigned int)sig_len;
  EVP_MD_CTX_free(ctx);
}

int verify_signature(Block *block) {
  EVP_MD_CTX *ctx = EVP_MD_CTX_new();
  if (!ctx) {
    fprintf(stderr, "Failed to create digest context.\n");
    return 0;
  }

  if (EVP_DigestVerifyInit(ctx, NULL, EVP_sha256(), NULL, pkey) <= 0) {
    fprintf(stderr, "Failed to init verification.\n");
    EVP_MD_CTX_free(ctx);
    return 0;
  }

  if (EVP_DigestVerifyUpdate(ctx, (unsigned char *)block->hash,
                             strlen(block->hash)) <= 0) {
    fprintf(stderr, "Failed to update verification.\n");
    EVP_MD_CTX_free(ctx);
    return 0;
  }

  int ret =
      EVP_DigestVerifyFinal(ctx, block->signature, block->signature_length);

  EVP_MD_CTX_free(ctx);
  return (ret == 1);
}
