#include "db_map.h"
#include "useful_tools.h"

int main(){
	load_configuration();

    db_map a("shiyan",'s','d');
    a.create();

    unsigned char key[32];
    unsigned value;

    for (unsigned i = 0;i<100;i++){
        memset(key,0,32);
        uprint(key,"su","shiyan_",i);
        a.add(key,i);
    }

    for (unsigned i = 0;i<20;i++){
        memset(key,0,32);
        uprint(key,"su","shiyan_",i);
        a.drop(key);
    }

    for (unsigned i = 0;i<100;i++){
        memset(key,0,32);
        uprint(key,"su","shiyan_",i);
        printf("%d %d\n",i,a.exists(key));
    }

    a.release();

    return 0;
}
