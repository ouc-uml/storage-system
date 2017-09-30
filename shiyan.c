#include "file_operation.h"

int main(){
	load_configuration();

	char buffer[node_block_size];
	put_int_to_block(buffer,0,2567);
	write_node_block(buffer,109,3);

	read_node_block(109,3,buffer);
	int x = trans_block_to_int(buffer,0,4);
	printf("%d\n", x);
	return 0;
}