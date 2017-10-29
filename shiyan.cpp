#include "db_manage.h"

int main(){
    load_configuration();
    db_struct db(1);

    db_map shiyan = db.get_map("shiyan");

 	unsigned char value[32];
 	uprint(value,32,"s","刘咏彬");
 	for (int i = 0;i<32;i++)
 		printf("%d ", value[i]);
    return 0;
}