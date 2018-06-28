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