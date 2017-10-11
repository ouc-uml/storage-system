#include "db_map.h"
#include "useful_tools.h"

int main(){
	load_configuration();

    db_map a("shiyan",'s','d');
    a.create();
    printf("%d %d\n",a.self.filenum,a.self.linenum);

    unsigned char key[32];
    for (int i = 0;i<100;i++){
        uprint(key,32,"sus","my name is ",i,".");
        a.add(key,i);
    }
    unsigned char x[200][32];
    unsigned num = a.get_all_key(x);
    for (int i = 0;i<num;i++)
        printf("%s\n",x[i]);

    a.release();

    return 0;
}
