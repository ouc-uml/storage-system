void parse_block_as_list_node(char node_block[],unsigned* prev_file,unsigned* prev_line,unsigned *next_file,unsigned *next_line,char data_block[]){
	*prev_line = *prev_file = *next_file = *next_line = 0;
	for (int i = 0;i<4;i++)
		*prev_file = (*prev_file<<8)+node_block[i];
	for (int i = 0;i<4;i++)
		*prev_line = (*prev_line<<8)+node_block[i+4];
	for (int i = 0;i<4;i++)
		*next_file = (*next_file<<8)+node_block[i+8];
	for (int i = 0;i<4;i++)
		*next_file = (*next_file<<8)+node_block[i+12];
	strncpy(data_block,node_block+16,node_block_size-16);
}