#include <stdbool.h>

static char base64alphabet_map[] = {
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


void get_K_hash_1st_quarter(const char in[4], char out[3]) {
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


bool brute_key_quarter(const char * in_3chars, char * out_4chars) {
	/*Brute-force init*/
	char input[4] = { '.','/' ,'/' ,'/' }; 	// '/'-'9', 'A'-'Z', 'a'-'z'
	unsigned int* p2input_as_dw = (unsigned int*)input;
	char output[3] = { 0 };
	/*Brute-force loop*/
	do {
		if (*p2input_as_dw == 0x7a7a7a7a)
			return false;
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
	return true;
};