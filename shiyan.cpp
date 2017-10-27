#include "db_manage.h"

int main(){
    load_configuration();
    db_struct db(1);

    db_map shiyan = db.get_map("shiyan");

 	printf("%d %d\n",shiyan.self.filenum,shiyan.self.linenum);
    return 0;
}