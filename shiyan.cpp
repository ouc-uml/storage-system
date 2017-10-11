#include "db_manage.h"

int main(){
/*
	load_configuration();
    clear_all_nodes();
    db_struct db(1);
    char name[32];
    db.create_list("shiyan",'d');
    db_list shiyan = db.get_list("shiyan");

    int len = shiyan.size;
    unsigned x[len];
    shiyan.get_all_value(x);
    for (int i = 0;i<len;i++)
        printf("%u\n", x[i]);
*/
    unsigned char key[32];
    uprint(key,32,"sxs","shiyan",45,"ooo");
    char value1[32],value2[32];
    unsigned value;
    uscan(key,"sxs",value1,6,&value,value2,3);
    printf("%s\n",value1);
    printf("%u\n",value);
    printf("%s\n",value2);
    return 0;
}
