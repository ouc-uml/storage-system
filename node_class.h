#ifndef _MEMORY_OPERATION_H_
#include "memory_operation.h"
#endif

#define _NODE_CLASS_H_ 0

struct memory_location{
    unsigned filenum,linenum;
    bool is_null(){
        return filenum==-1 && linenum==-1;
    }
    void set_null(){
        filenum = linenum = -1;
    }
/*
    memory_location& operator = (memory_location& x){
        this->filenum = x.filenum;
        this->linenum = x.linenum;
        return *this;
    }
*/
    memory_location& operator = (memory_location x){
        this->filenum = x.filenum;
        this->linenum = x.linenum;
        return *this;
    }
    bool operator == (memory_location & x){
        return x.filenum == this->filenum && x.linenum == this->linenum;
    }
    memory_location& operator = (unsigned char x[]){
        filenum = trans_block_to_int(x,0,4);
        linenum = trans_block_to_int(x,4,4);
        return *this;
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

struct data_type1{
    unsigned char key[q_len];
    bool k_type;
    data_type1(){
        k_type = 0;
    }
};

struct data_type2 {
    unsigned char key[k_len],value[v_len];
    bool k_type,v_type;//0 means unsigned,1 means unsigned char[]
    data_type2(){
        k_type = v_type = 0;
    }
    int cmpstr(unsigned char a[],unsigned char b[]){
        for (int i = 0;i<k_len;i++)
            if (a[i]<b[i]) return 1;
            else if (b[i]<a[i]) return -1;
        return 0;
    }
    bool operator < (data_type2 &x){
        if (k_type == 0){
            unsigned key1,key2;
            key1 = trans_block_to_int(key,0,4);
            key2 = trans_block_to_int(x.key,0,4);
            return key1<key2;
        }
        return cmpstr(this->key,x.key)==1;
    }
    bool operator == (data_type2 &x){
        if (k_type == 0){
            unsigned key1,key2;
            key1 = trans_block_to_int(key,0,4);
            key2 = trans_block_to_int(x.key,0,4);
            return key1 == key2;
        }
        return cmpstr(this->key, x.key) == 0;
    }
    data_type2& operator = (data_type2 &x){
        if (k_type == 0) memcpy(this->key,x.key,4);
        else memcpy(this->key,x.key,k_len);
        if (v_type == 0) memcpy(this->value,x.value,4);
        else memcpy(this->value,x.value,v_len);
        return *this;
    }
    data_type2& operator = (unsigned char x[]){
        if (k_type == 0) memcpy(this->key,x,4);
        else memcpy(this->key,x,k_len);
        return *this;
    }
};

struct queue_node : public node_class{
    memory_location prev,succ;
    data_type1 data;
    queue_node(){
        prev.set_null();
        succ.set_null();
    }

    void save(){
        if (self.is_null()) return ;
        unsigned char buffer[block_size];
        put_int_to_block(buffer,0,prev.filenum);
        put_int_to_block(buffer,4,prev.linenum);
        put_int_to_block(buffer,8,succ.filenum);
        put_int_to_block(buffer,12,succ.linenum);
        put_char_to_block(buffer,16,0,q_len,data.key);
        unsigned tmpmode = 0;
        if (data.k_type) tmpmode = 128;
        put_int_to_block(buffer,16+q_len,tmpmode);
        write_node_block(buffer,self.filenum,self.linenum);
    }

    void load(){
        if (self.is_null()) return ;
        unsigned char buffer[block_size];
        read_node_block(self.filenum,self.linenum,buffer);

        prev = buffer;
        succ = buffer+8;
        trans_block_to_char_array(buffer,16,q_len,data.key);
        data.k_type = trans_block_to_int(buffer,16+q_len,1) == 128;
    }
};

struct binary_tree_node : public node_class{
    memory_location left,right;
    int size,HASH,RANDOM;
    data_type2 data;
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
        put_char_to_block(buffer,28,0,k_len,data.key);
        put_char_to_block(buffer,k_len+28,0,v_len,data.value);
        unsigned tmpmode = 0;
        if (data.k_type) tmpmode |= 128;
        if (data.v_type) tmpmode |= 64;
        put_int_to_block(buffer,k_len+v_len+28,tmpmode);

        write_node_block(buffer,self.filenum,self.linenum);
    }

    void load(){
        if (self.is_null()){
            left.set_null();
            right.set_null();
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
        trans_block_to_char_array(buffer,28,k_len,data.key);
        trans_block_to_char_array(buffer,28+k_len,v_len,data.value);
        data.k_type = (trans_block_to_int(buffer,28+k_len+v_len,4) & 128) != 0;
        data.v_type = (trans_block_to_int(buffer,28+k_len+v_len,4) & 64) != 0;
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
