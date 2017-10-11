#include "db_list.h"
#include "useful_tools.h"

int main(){
	load_configuration();
    clear_all_nodes();

    db_list a("shiyan",'d');
    a.create();
    printf("%d %d\n",a.self.filenum,a.self.linenum);

    for (int i = 0;i<100;i++){
        a.push_head(i);
    }
    
    unsigned ret[110];
    int len = a.get_all_value(ret);
    for (int i = 0;i<len;i++){
        printf("%u\n",ret[i]);
    }

    return 0;
}
