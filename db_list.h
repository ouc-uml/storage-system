#ifndef _NODE_CLASS_H_
#include "node_class.h"
#endif
#define _DB_LIST_H_ 0

struct db_list{
    memory_location self;
    memory_location head,tail;
    bool k_type;
    unsigned size,block_size;
    unsigned char name[k_len];

    db_list(const char x[],char k_ty){
        memcpy(name,x,k_len);
        head.set_null();
        tail.set_null();
        block_size = node_block_size;
        size = 0;
        if (k_ty == 'd') k_type = 0;
        else k_type = 1;
    }
    db_list(){
        block_size = node_block_size;
        size = 0;
    }
    void save(){
        unsigned char buffer[block_size];
        put_int_to_block(buffer,0,head.filenum);
        put_int_to_block(buffer,4,head.linenum);
        put_int_to_block(buffer,8,tail.filenum);
        put_int_to_block(buffer,12,tail.linenum);
        put_int_to_block(buffer,16,size);
        put_char_to_block(buffer,20,0,k_len,name);
        put_int_to_block(buffer,20+k_len,k_type?128:0);
        write_node_block(buffer,self.filenum,self.linenum);
    }
    void load(){
        unsigned char buffer[block_size];
        read_node_block(self.filenum,self.linenum,buffer);

        head = buffer;
        tail = buffer+8;
        size = trans_block_to_int(buffer,16,4);
        trans_block_to_char_array(buffer,20,k_len,name);
        k_type = trans_block_to_int(buffer,20+k_len,4) == 128;
    }
    void push_tail(unsigned char x[]){
        load();
        size++;
        queue_node node_x;
        node_x.create();
        memcpy(node_x.data.key,x,q_len);

        if (head.is_null() && tail.is_null()){
            tail = head = node_x.self;
        }
        else {
            queue_node node_tail;
            node_tail.self = tail;
            node_tail.load();
            node_tail.succ = node_x.self;
            node_tail.save();
            node_x.prev = tail;
            tail = node_x.self;
        }
        node_x.save();
        save();
    }
    void push_tail(unsigned x){
        load();
        size++;
        queue_node node_x;
        node_x.create();
        put_int_to_block(node_x.data.key,0,x);

        if (head.is_null() && tail.is_null()){
            tail = head = node_x.self;
        }
        else {
            queue_node node_tail;
            node_tail.self = tail;
            node_tail.load();
            node_tail.succ = node_x.self;
            node_tail.save();
            node_x.prev = tail;
            tail = node_x.self;
        }
        node_x.save();
        save();
    }
    void push_head(unsigned char x[]){
        load();
        size++;
        queue_node node_x;
        node_x.create();
        memcpy(node_x.data.key,x,q_len);

        if (head.is_null() && tail.is_null()){
            tail = head = node_x.self;
        }
        else {
            queue_node node_head;
            node_head.self = head;
            node_head.load();
            node_head.prev = node_x.self;
            node_head.save();
            node_x.succ = head;
            head = node_x.self;
        }
        node_x.save();
        save();
    }
    void push_head(unsigned x){
        load();
        size++;
        queue_node node_x;
        node_x.create();
        put_int_to_block(node_x.data.key,0,x);

        if (head.is_null() && tail.is_null()){
            tail = head = node_x.self;
        }
        else {
            queue_node node_head;
            node_head.self = head;
            node_head.load();
            node_head.prev = node_x.self;
            node_head.save();
            node_x.succ = head;
            head = node_x.self;
        }
        node_x.save();
        save();
    }
    void pop_tail(){
        load();
        if (size == 0) return ;
        size--;
        queue_node node_tail;
        node_tail.self = tail;
        node_tail.load();
        memory_location tmp = tail;
        tail = node_tail.prev;
        if (!tail.is_null()){
            node_tail.self = tail;
            node_tail.load();
            node_tail.succ.set_null();
            node_tail.save();
        }
        else head.set_null();
        delete_node(tmp.filenum,tmp.linenum);
        save();
    }
    void pop_head(){
        load();
        if (size == 0) return ;
        size--;
        queue_node node_head;
        node_head.self = head;
        node_head.load();
        memory_location tmp = head;
        head = node_head.succ;
        if (!head.is_null()){
            node_head.self = head;
            node_head.load();
            node_head.prev.set_null();
            node_head.save();
        }
        else tail.set_null();
        delete_node(tmp.filenum,tmp.linenum);
        save();
    }
    unsigned get_all_value(unsigned char x[][q_len]){
        load();
        memory_location index = head;
        unsigned num = 0;
        while (!index.is_null()){
            queue_node node;
            node.self = index;
            node.load();
            memcpy(x[num++],node.data.key,q_len);
            index = node.succ;
        }
        return num;
    }
    unsigned get_all_value(unsigned x[]){
        load();
        memory_location index = head;
        unsigned num = 0;
        while (!index.is_null()){
            queue_node node;
            node.self = index;
            node.load();
            x[num++] = trans_block_to_int(node.data.key,0,4);
            index = node.succ;
        }
        return num;
    }
    int update(unsigned index,unsigned x){
        load();
        if (index>=size) return -1;

        memory_location tmp = head;
        queue_node node;
        node.self = tmp;
        node.load();

        for (int i = 0;i<index;i++){
            tmp = node.succ;
            node.self = tmp;
            node.load();
        }

        put_int_to_block(node.data.key,0,x);
        node.save();
        save();
        return 0;
    }
    int update(unsigned index,unsigned char x[]){
        load();
        if (index>=size) return -1;

        memory_location tmp = head;
        queue_node node;
        node.self = tmp;
        node.load();

        for (int i = 0;i<index;i++){
            tmp = node.succ;
            node.self = tmp;
            node.load();
        }

        put_char_to_block(node.data.key,0,0,q_len,x);
        node.save();
        save();
        return 0;
    }
    int get_by_index(int index,unsigned *value){        
        load();
        if (index>=size) return -1;

        memory_location tmp = head;
        queue_node node;
        node.self = tmp;
        node.load();

        for (int i = 0;i<index;i++){
            tmp = node.succ;
            node.self = tmp;
            node.load();
        }

        *value = trans_block_to_int(node.data.key,0,4);
        return 0;
    }
    int get_by_index(int index,unsigned char value[]){        
        load();
        if (index>=size) return -1;

        memory_location tmp = head;
        queue_node node;
        node.self = tmp;
        node.load();

        for (int i = 0;i<index;i++){
            tmp = node.succ;
            node.self = tmp;
            node.load();
        }

        memcpy(value,node.data.key,q_len);
        return 0;
    }
    void show(FILE* fp){
        memory_location index = head;
        while (!index.is_null()){
            queue_node node;
            node.self = index;
            node.load();
            if (k_type) fprintf(fp,"%s\n",node.data.key);
            else {
                unsigned tmp = trans_block_to_int(node.data.key,0,4);
                fprintf(fp,"%u\n",tmp);
            }
            index = node.succ;
        }
    }
    void show(FILE *fp,int start,int end){
        int nn = 0;
        memory_location index = head;
        while (!index.is_null()){
            queue_node node;
            node.self = index;
            node.load();
            if (nn>=start && nn<=end){
                fprintf(fp,"[%d]\t:\t", nn);
            if (k_type) fprintf(fp,"\"%s\"\n",node.data.key);
            else {
                unsigned tmp = trans_block_to_int(node.data.key,0,4);
                fprintf(fp,"%u\n",tmp);
            }
            }
            index = node.succ;
            nn++;
        }
    }
    void create(){
        new_node_position(&self.filenum,&self.linenum);
    }
    void release_node(){
        memory_location index = head;
        while (!index.is_null()){
            queue_node node;
            node.self = index;
            node.load();
            index = node.succ;
            node.release();
        }
        head.set_null();
        tail.set_null();
        size = 0;
    }
    void release(){
        load();
        if (self.is_null()) return;
        release_node();
        delete_node(self.filenum,self.linenum);
    }
    void clear(){
        load();
        if (self.is_null()) return;
        release_node();
    }
};
