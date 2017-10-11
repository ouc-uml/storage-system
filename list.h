#include "queue.h"

struct db_list : public QUEUE{
    db_list(const char _name[],char k_ty){
        memcpy(name,_name,32);
        if (k_ty == 'd') k_type = 0;
        else k_type = 1;

        self.filenum = self.linenum = -1;
        block_size = node_block_size;
        head.set_null();
        tail.set_null();
        size = 0;
    }
};