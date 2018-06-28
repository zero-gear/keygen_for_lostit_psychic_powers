#include <openssl/md5.h>

void MD5_and_XOR(char hash_buffer[16]) {
	char md5[MD5_DIGEST_LENGTH] = { 0 };
	MD5((const unsigned char *)hash_buffer, 16, (unsigned char *)md5);
	for (int i = 0; i < 16; i++)
		hash_buffer[i] ^= md5[i];
	return;
}