#include "db_manage.h"

int main(){
    load_configuration();
    db_struct db(1);

   // db.create_list("shiyan",'d');
    db_list shiyan = db.get_list("shiyan");

    for (int i = 0;i<10;i++)
        shiyan.push_tail(i);

    shiyan.update(5,99);
    shiyan.show();
    return 0;
}