#include "db_manage.h"

int main(){
    load_configuration();
    db_struct db(1);

    db_list shiyan = db.get_list("shiyan");

    printf("%d\n", shiyan.size);
    return 0;
}