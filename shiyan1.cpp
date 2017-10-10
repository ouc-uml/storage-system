#include "db_map.h"
#include "useful_tools.h"

int main(){
	load_configuration();
    //clear_all_nodes();

    db_map a("shiyan",'s','d');
    a.create();

    unsigned char key[32];
    unsigned char value[32];

    for (int i = 0 ;i<11;i++){
        memset(key,0,32);
        uprint(key,"su","shiyan_",i);

        a.add(key,i);

        if (i == 10){
            a.show(a.root);
            printf("\n");
        }
    }

    a.release();
    return 0;
}

