KeygenMe "Psychic powers or brute strength your choice" ([task source](https://forum.tuts4you.com/topic/37904-keygenme-01-psychic-powers-or-brute-strength-your-choice))

### Program flow explanation

at start program checks if:

```
5 <= len(NAME) < 256
16 <= len(KEY) < 256
```
	
it lowercase all `NAME` chars, and  delete from name all non `a`-`z` chars. Then it delete all non base64-alphabet chars from `KEY`, and check if `len(KEY) == 16`.
Lets consider `NAME` as string with length 5 or greater, and which consist from 'a'-'z' chars.
Lets consider `KEY` as string with length 16, and which consist from base64-alphabet chars (`+,/,0-9,A-Z,a-z`).

Then program compare if NAME:KEY pair is not 
+ "lostit":"_RY5obY7IduF4Se2T_"
+ "tutsyou":"_RkIomczPYHNrJoCA_"

which are valid NAME:KEY pairs, but for some reason blacklisted in such way.

Now your program begin to calculate different hashes and make different checks.

**FIRST HASH**: hash retrieved from `KEY`, lets give to it a name `K_hash_1`. `K_hash_1` is 12-byte hash, each 4 bytes of `KEY` converted to 3 bytes of `K_hash_1` . Below code snippet explain this transformation, consider `get_K_hash_1()` function:
```C
char base64alphabet_map[] = {
	/* 01*/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/* 10*/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/* 20*/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/* 30*/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/* 40*/ 0, 0, 0,62, 0, 0, 0,63,51,53,/*43:+,47:/,48:0*/
	/* 50*/54,55,56,57,58,59,60,61, 0, 0,
	/* 60*/ 0, 0, 0, 0, 0, 0, 1, 2, 3, 4,/*65:A*/
	/* 70*/ 5, 6, 7, 8, 9,10,11,12,13,14,
	/* 80*/15,16,17,18,19,20,21,22,23,24,
	/* 90*/25, 0, 0, 0, 0, 0, 0,26,27,28,/*90:Z,97:a*/
	/*100*/29,30,31,32,33,34,35,36,37,38,
	/*110*/39,40,41,42,43,44,45,46,47,48,
	/*120*/49,50,51, 0, 0, 0, 0, 0, 0, 0/*122:z*/
};


void get_K_hash_1_quarter(const char in[4], char out[3]) {
	unsigned int c0 = base64alphabet_map[in[0]];
	unsigned int c1 = base64alphabet_map[in[1]];
	unsigned int c2 = base64alphabet_map[in[2]];
	unsigned int c3 = base64alphabet_map[in[3]];

	unsigned int hash_chunk = (((((c0 << 6) + c1) << 6) + c2) << 6) + c3;

	unsigned int temp_hash;
	temp_hash = hash_chunk >> 16;
	out[0] = (char)temp_hash;
	temp_hash = hash_chunk >> 8;
	out[1] = (char)temp_hash;
	out[2] = (char)hash_chunk;
}

void get_K_hash_1(const char *key, char custom_hash[16]) {
	for (int i = 0, n = 0; i < KEY_LEN, n < HASH_LEN; i = i + 4, n = n + 3) {
		get_K_hash_1_quarter(key + i, custom_hash + n);
	}
}
```

**SECOND HASH**: 16-byte hash, which is retrieved from `K_hash_1`, lets name it `K_hash_2`.
`K_hash_2` initilized using function in listing below.
```C
void init_K_hash_2(const char src_hash[16], char dest_hash[0x10]) {
	dest_hash[0x0] = src_hash[0];
	dest_hash[0x1] = src_hash[1];
	dest_hash[0x2] = src_hash[2];
	dest_hash[0x3] = src_hash[3];
	dest_hash[0x4] = src_hash[4];
	dest_hash[0x5] = src_hash[5];

	dest_hash[0x6] = src_hash[0];
	dest_hash[0x7] = src_hash[1];
	dest_hash[0x8] = src_hash[2];
	dest_hash[0x9] = src_hash[3];
	dest_hash[0xA] = src_hash[4];
	dest_hash[0xB] = src_hash[5];

	dest_hash[0xC] = src_hash[0];
	dest_hash[0xD] = src_hash[1];
	dest_hash[0xE] = src_hash[2];
	dest_hash[0xF] = src_hash[3];
}
```
Then `K_hash_2`  is used as input string to calculate MD5 hash. `K_hash_2` is XOR-ed with resulting hash. See code below:
```C
#include <openssl/md5.h>

void _cdecl MD5_and_XOR(char hash_buffer[16]) {
	char md5[MD5_DIGEST_LENGTH] = { 0 };
	MD5((const unsigned char *)hash_buffer, 16, (unsigned char *)md5);
	for (int i = 0; i < 16; i++)
		hash_buffer[i] ^= md5[i];
	return;
}
```
This operation repeated 1000 times.

**THIRD HASH**: getting 16-byte hash from `NAME`, lets name it `N_hash`.
`N_hash` initilized using function in listing below.
```C
void init_N_hash(const char * name, unsigned char name_len, char dest_hash[0x10]) {
	if (name_len <= 0x10) {
		for (int i = 0; i < 0x10; i++) {
			dest_hash[i] = name[i % name_len];
		}
	}
	if (name_len > 0x10) {
		memcpy(dest_hash, name, 0x10);
		for (int i = 0; i < name_len - 0x10; i++)
			dest_hash[i % 0x10] ^= name[i + 0x10];
	}
}
```
,then  `N_hash`  is hashed with MD5 and XOR-ed 1000 times in the same way as K_hash_2  (described above).

**1ST CHECK**:  
Note: python lists used for below pseudocode
```python
N_hash[0:6] == K_hash_1[6:12]
```

**2ND CHECK**:
```python
idx = 6 * ((ord(name[0]) - 0x60) * (ord(name[1]) - 0x60) - 1)
K_hash_2[0:6] == master_table[idx:idx+6]
```

### Data dependencies
below figure clarify relationship beetween name, key, different hashes and checks. For better understanding what depends on what.
![Image of Yaktocat](https://github.com/smart-rabbit/keygen_for_lostit_psychic_powers/blob/master/other/data_dependencies.PNG)

### Checks bypass
**1ST CHECK**:  
we have `NAME`, consequently we can produce `N_hash`, `N_hash[0:6]`  according to 1st check conditions should be equal to `K_hash_1[6:12]`, from `K_hash_1[6:12]` we can easely get 2nd part of `KEY[8:16]`.
Below code of function that help us retrieve `KEY[8:12]` from `N_hash[0:3]` and `KEY[12:16]` from `N_hash[3:6]`.
```C
void brute_key_quarter(const char * in_3chars, char * out_4chars) {
	/*Brute-force init*/
	char input[4] = { '.','/' ,'/' ,'/' }; 	// '/'-'9', 'A'-'Z', 'a'-'z'
	unsigned int* p2input_as_dw = (unsigned int*)input;
	char output[3] = { 0 };
	/*Brute-force loop*/
	do {
	START:
		(*p2input_as_dw)++;
		for (int i = 0; i < 4; i++) {
			if (
				!(input[i] >= '/' && input[i] <= '9') &&
				!(input[i] >= 'A' && input[i] <= 'Z') &&
				!(input[i] >= 'a' && input[i] <= 'z')
				) {
				goto START;
			}
		}
		get_K_hash_1st_quarter(input, output);
	} while (
		!(output[0] == in_3chars[0] && output[1] == in_3chars[1] && output[2] == in_3chars[2])
		);
	/*Brute-force end*/
	out_4chars[0] = input[0];
	out_4chars[1] = input[1];
	out_4chars[2] = input[2];
	out_4chars[3] = input[3];
};
```

**2ND CHECK**:  
we have 2nd 8-bytes part of `KEY`, we should retrieve 1st 8-bytes part of `KEY` and pass this check.
Lets consider all possible variants:
1. Direct brute of key. Unfortunatly brute time is unacceptable big. Using [keygen based on brute](https://github.com/smart-rabbit/keygen_for_lostit_brute-strength),
which brute threads quantity is equal to quantity of cores on your PC - i waste my 8 cores power for several days. 
Initial sober view should prevent me from such mistake. Input range is too wide,  MD5 hashes numerous calculations loop is not so fast.
2. Before consider alternative, lets make assumption on how keygenMe author generate `KEY[0:8]`.
Second check is based on `char master_table[4056]`.
This table is indexed using `NAME` first two chars
`idx = 6 * ((ord(name[0]) - 0x60) * (ord(name[1]) - 0x60) - 1)`.
Index can be in range from 0 to 675, which tell us that this table contains 676 6-byte chunks.
Briefly saying, this `master_table` contains 676 variants of `K_hash_2[0:6]`.
As mentioned above direct key brute is impractical, and i far from thoughts that autor generate keys in such way.
And as `K_hash_2` is retrieved from `K_hash_1[0:6]`, apparently author has another data structure that holds 
`K_hash_1[0:6]` for every `K_hash_2[0:6]` in `master_table`. From now, we suggest that author has:
``` C
char needed_table[6*676] = {.......} // contains variants of `K_hash_1[0:6]`
char master_table[6*676] = {.......} // contains corresponding variants of `K_hash_2[0:6]`
```

From now we can try to brute `char needed_table[6* 676]` as suggested [here](https://forum.reverse4you.org/showthread.php?t=2917&p=14049&viewfull=1#post14049),
or use psychic powers and try guess how keygenMe author generate `char needed_table[6*676]` as suggested [here](https://forum.tuts4you.com/topic/37904-keygenme-01-psychic-powers-or-brute-strength-your-choice/).
Lets use 2nd way. We make assumption that was used MSVS CRT `rand()` function to fill `char needed_table[6*676]`.
```C
srand(unknown_seed);
char  needed_table[6*676];
for (int i = 0; i < 6 * 676; i++) {
  needed_table[i] = rand() & 0xFF;
}
```
Below code  brute of all possible variants of `char needed_table[6*676]` depending on different seed, relying on hardcoded `char master_table[6*676]` and blacklisted `NAME:KEY` pairs.
```C
#include <Windows.h>
#include <stdio.h>
#include <limits.h>

#include "master_table.h"
#include "hashes.h"

UINT32 unknown_seed = -1;
BYTE needed_table[4056];  // table of K_hash_1[0:6] chunks
BYTE lo_K_hash_2[16];
BYTE tu_K_hash_2[16];


int main() {
	UINT16 lo_idx = 6 * (('l' - 0x60) * ('o' - 0x60) - 1);
	UINT16 tu_idx = 6 * (('t' - 0x60) * ('u' - 0x60) - 1);

	UINT16 max_idx = tu_idx + 5;
	do {
		unknown_seed++;    //0x00007a69

		// generate table
		srand(unknown_seed);
		for (SIZE_T i = 0; i <= max_idx; i++)
			needed_table[i] = rand() & 0xFF;

		// calc K_hash_2 of "lo"
		init_K_hash_2(needed_table + lo_idx, lo_K_hash_2);
		for (SIZE_T i = 0; i < 1000; i++)
			MD5_and_XOR(lo_K_hash_2);

		// calc K_hash_2 of "tu"
		init_K_hash_2(needed_table + tu_idx, tu_K_hash_2);
		for (SIZE_T i = 0; i < 1000; i++)
			MD5_and_XOR(tu_K_hash_2);
	} while (
			unknown_seed < UINT_MAX &&
			//master_table[lo_idx:lo_idx+6] ==  lo_K_hash_2[0:6]
			memcmp(master_table + lo_idx, lo_K_hash_2, 6) != 0  && 
			//master_table[tu_idx:tu_idx+6] ==  tu_K_hash_2[0:6]
			memcmp(master_table + tu_idx, tu_K_hash_2, 6) != 0
		);
	if (unknown_seed == UINT_MAX) {
		printf("Your assumption is wrong. Try `rand()` from another library.");
	} else {
		printf("Searched seed: 0x%08x\n", unknown_seed);
	}
	
	return 0;
}
```

Voila, your assumption is correct, appropriate  `seed` is equal to `0x00007a69`. 
Now we can generate key for any name for several seconds. Keygen sources and binary is [here](https://github.com/smart-rabbit/keygen_for_lostit_psychic_powers).

P.S.: The main difficulty is to guess how author generate `char needed_table[6*676]`, 
to make this solution impossible author should customize way in which `char needed_table[6*676]` generated to make guessing impossible.

**Valid `NAME:KEY` pairs examples:**:
* bigben:4aWfjhuWTgv6NSM2
* impossible:tbxCCxODKA5M7XWB
