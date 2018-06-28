void get_needed_table(char master_table[4056], char needed_table[4056]) {
	srand(0x00007a69);
	for (int i = 0; i < 4056; i++)
		needed_table[i] = rand() & 0xFF;
}