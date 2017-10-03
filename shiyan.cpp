#include "treap.h"

int main(){

	load_configuration();
    clear_all_nodes();

	TREAP a("shiyan");
	a.create();
	a.k_type = 0;
	a.v_type = 0;
	unsigned char buffer[32];
	memcpy(buffer,"shiyan_",7);

	for (int i = 0;i<100000;i++){
        data_type tmp;
        put_int_to_block(tmp.key,0,i);
        put_int_to_block(tmp.value,0,i);

        a.Insert(tmp);
	}

	a.save();
	a.load();
	a.show(a.root);

    return 0;
}
