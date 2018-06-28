#include <stdio.h>
#include <Windows.h>
#include "tables.h"
#include "N_hash_init.h"
#include "md5.h"
#include "brute_K_1st_part.h"
#include "brute_K_2nd_part.h"


char * name;
char N_hash[16] = { 0 };

extern char needed_table[4056]; // table of K_hash_1[0:6] chunks
extern char master_table[4056];

char key[17] = { 0 };  // 16 base64alphabet chars


int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("[!] Usage: keygen.exe name\n");
		ExitProcess(1);
	}
	name = argv[1];
	if (strlen(name) <= 4) {
		printf("[!] Name is too short. It`s length should be minimum 5 chars.\n");
		ExitProcess(2);
	}
	if (strlen(name) > 255) {
		printf("[!] Name is too long. It`s length should be maximum 255 chars.\n");
		ExitProcess(2);
	}
	for (int i = 0; name[i]; i++) {
		if (name[i] < 'a' || name[i] > 'z') {
			printf("[!] Name should contains only lowercase letters in range 'a'-'z'.\n");
			ExitProcess(3);
		}
	}
	printf("[*] Provided name : %s\n", name);

	// calculate N hash
	init_N_hash(name, strlen(name), N_hash);
	for (int i = 0; i < 1000; i++)
		MD5_and_XOR(N_hash);

	/* Bypass 1st  C H E C K  ::
		N_hash[0:6] == K_hash_1st[6:12]
	and
		generate key[8:16]
		*/
	if (!brute_key_quarter(N_hash + 0, key + 8) || !brute_key_quarter(N_hash + 3, key + 12)) {
		puts("[!] Impossible to generate key for input name. Choose another name.");
		return 1;
	}

	/* Bypass 2nd  C H E C K  ::
		idx = 6 * ( (ord(name[0])-0x60) * (ord(name[1])-0x60) - 1 )
		K_hash_2nd[0:6] == master_table[idx:idx+6]
	and
		generate key[0:8]
		*/
	get_needed_table(master_table, needed_table);
	UINT16 idx = 6 * ((name[0] - 0x60) * (name[1] - 0x60) - 1);
	if (!brute_key_quarter(needed_table + (idx + 0), key + 0) || !brute_key_quarter(needed_table + (idx + 3), key + 4)) {
		puts("[!] Impossible to generate key for input name. Choose another name.");
		return 1;
	}

	printf("[*] Generated key : %s\n", key);

	return 0;
}


/*
Valid 'name:key' pairs :
	bigben:4aWfjhuWTgv6NSM2
	blackseaa:XxysWSdnEuJkAKbk
	abcde:SvArpaWGuMcYemf/
*/