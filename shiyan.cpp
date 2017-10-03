#include "treap.h"

int main(){
	load_configuration();
    clear_all_nodes();

	TREAP a("shiyan");
	a.create();
	a.k_type = 0;
	a.v_type = 0;

	for (unsigned char i = 'a';i<='z';i++){
        data_type tmp;
        put_int_to_block(tmp.key,0,i-'a');
        put_int_to_block(tmp.value,0,i-'a'+1);
        a.Insert(tmp);
	}

	a.show(a.root);
	a.save();
    return 0;
}
