#include "memory_operation.h"

struct memory_location{
    unsigned filenum,linenum;
    bool is_null(){
        return filenum==-1 && linenum==-1;
    }
    bool set_null(){
        filenum = linenum = -1;
    }
    memory_location& operator = (memory_location& x){
        this->filenum = x.filenum;
        this->linenum = x.linenum;
        return *this;
    }
    bool operator == (memory_location & x){
        return x.filenum == this->filenum && x.linenum == this->linenum;
    }
};

struct node_class{
    memory_location self;
    unsigned block_size;
    node_class(unsigned x = node_block_size){
        block_size = node_block_size;
    }
    virtual void save() = 0;
    virtual void load() = 0;
    void create(){
        new_node_position(&self.filenum,&self.linenum);
    }
    void release(){
        delete_node(self.filenum,self.linenum);
    }
};

struct data_type {
    unsigned key,value;
    bool operator < (data_type &x){
        return this->key<x.key;
    }
    bool operator == (data_type &x){
        return this->key == x.key;
    }
    data_type& operator = (data_type &x){
        this->key = x.key;
        this->value = x.value;
    }
};

struct binary_tree_node : public node_class{
    memory_location left,right;
    int size,HASH,RANDOM;
    data_type data;
    binary_tree_node(){
        left.set_null();
        right.set_null();
    }

    void save(){
        if (self.is_null()) return ;

        unsigned char buffer[block_size];
        put_int_to_block(buffer,0,left.filenum);
        put_int_to_block(buffer,4,left.linenum);
        put_int_to_block(buffer,8,right.filenum);
        put_int_to_block(buffer,12,right.linenum);
        put_int_to_block(buffer,16,size);
        put_int_to_block(buffer,20,HASH);
        put_int_to_block(buffer,24,RANDOM);
        put_int_to_block(buffer,28,data.key);
        put_int_to_block(buffer,32,data.value);

        write_node_block(buffer,self.filenum,self.linenum);
    }

    void load(){
        if (self.is_null()){
            left.filenum = left.linenum = -1;
            right.filenum = right.linenum = -1;
            size = HASH = RANDOM = 0;
            return ;
        }

        unsigned char buffer[block_size];
        read_node_block(self.filenum,self.linenum,buffer);

        left.filenum = trans_block_to_int(buffer,0,4);
        left.linenum = trans_block_to_int(buffer,4,4);
        right.filenum = trans_block_to_int(buffer,8,4);
        right.linenum = trans_block_to_int(buffer,12,4);
        size = trans_block_to_int(buffer,16,4);
        HASH = trans_block_to_int(buffer,20,4);
        RANDOM = trans_block_to_int(buffer,24,4);
        data.key = trans_block_to_int(buffer,28,4);
        data.value = trans_block_to_int(buffer,32,4);
    }

    binary_tree_node& operator = (binary_tree_node& x){
        this->left = x.left;
        this->right = x.right;
        this->size = x.size;
        this->HASH = x.HASH;
        this->RANDOM = x.RANDOM;
        this->data = x.data;

        return *this;
    }
};
