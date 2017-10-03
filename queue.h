#include "node_class.h"

struct QUEUE{
    memory_location self;
    memory_location head,tail;
    bool k_type;
    unsigned size,block_size;
    unsigned char name[32];

    QUEUE(const char x[]){
        memcpy(name,x,32);
        head.set_null();
        tail.set_null();
        block_size = node_block_size;
        size = 0;
        k_type = 0;
    }
    void push_tail(unsigned char x[]){
        size++;
        queue_node node_x;
        node_x.create();
        if (k_type == 0) memcpy(node_x.data.key,x,4);
        else memcpy(node_x.data.key,x,32);

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
    }
    void push_head(unsigned char x[]){
        size++;
        queue_node node_x;
        node_x.create();
        if (k_type == 0) memcpy(node_x.data.key,x,4);
        else memcpy(node_x.data.key,x,32);

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
    }
    void pop_tail(){
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
    }
    void pop_head(){
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
    }
    void show(){
        memory_location index = head;
        while (!index.is_null()){
            queue_node node;
            node.self = index;
            node.load();
            if (k_type) printf("%s\n",node.data.key);
            else {
                unsigned tmp = trans_block_to_int(node.data.key,0,4);
                printf("%u\n",tmp);
            }
            index = node.succ;
            node.self = index;
            node.load();
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
    }
    void release(){
        release_node();
        delete_node(self.filenum,self.linenum);
    }
    void save(){
        unsigned char buffer[block_size];
        put_int_to_block(buffer,0,head.filenum);
        put_int_to_block(buffer,4,head.linenum);
        put_int_to_block(buffer,8,tail.filenum);
        put_int_to_block(buffer,12,tail.linenum);
        put_int_to_block(buffer,16,size);
        put_char_to_block(buffer,20,0,32,name);
        put_int_to_block(buffer,52,k_type?128:0);
        write_node_block(buffer,self.filenum,self.linenum);
    }
    void load(){
        unsigned char buffer[block_size];
        read_node_block(self.filenum,self.linenum,buffer);

        head = buffer;
        tail = buffer+8;
        size = trans_block_to_int(buffer,16,4);
        trans_block_to_char_array(buffer,20,32,name);
        k_type = trans_block_to_int(buffer,52,4) == 128;
    }
};
