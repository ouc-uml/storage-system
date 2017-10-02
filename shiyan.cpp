#include "treap.h"

int main(){
	load_configuration();

	unsigned char buffer[128];

	clear_all_nodes();
    for (int i = 0;i<100;i++){
        data_type tmp;
        tmp.key = i;
        tmp.value = i+1;
        a.Insert(tmp);
    }

    for (int i = 0;i<50;i++){
        data_type tmp;
        tmp.key = i;
        a.Remove(tmp);
    }
    return 0;
}
