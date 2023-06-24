#include <stdio.h>
#include "openssl/crypto.h"
#include "openssl/core.h"

int main() {
  // Initialize OpenSSL
  OPENSSL_init_crypto(0, NULL);

  // Get the OpenSSL version number
  unsigned long version_num = SSLeay();
  int major = (int)(version_num >> 28) & 0xFF;
  int minor = (int)(version_num >> 20) & 0xFF;
  int patch = (int)(version_num >> 12) & 0xFF;
  int status = (int)(version_num) & 0xFFF;

  // Print the OpenSSL version number
  printf("OpenSSL version: %d.%d.%d%s\n", major, minor, patch, status ? " (beta)" : "");

  return 0;
}