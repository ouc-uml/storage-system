#include "db_manage.h"

int main(){
	load_configuration();
	clear_all_nodes();
	db_struct a;
	a.install();
	return 0;
}