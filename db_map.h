#include "treap.h"

struct db_map : public TREAP{
    db_map(const char _name[],char k_ty,char v_ty){
        memset(name,0,sizeof name);
        for (int i = 0;i<32;i++)
            if (!_name[i]) break;
            else name[i] = _name[i];
        if (k_ty == 'd') k_type = 0;
        else k_type = 1;
        if (v_ty == 'd') v_type = 0;
        else v_type = 1;

        self.filenum = self.linenum = -1;
        block_size = node_block_size;
        root.set_null();
        srand(time(NULL));
        hash_top = 0;
    }

    void add(unsigned char _key[],unsigned char _value[]){
        data_type tmp;
        memcpy(tmp.key,_key,32);
        memcpy(tmp.value,_value,32);
        Insert(tmp);
        save();
    }
    void add(unsigned _key,unsigned char _value[]){
        data_type tmp;
        put_int_to_block(tmp.key,0,_key);
        put_char_to_block(tmp.value,0,0,32,_value);
        Insert(tmp);
        save();
    }
    void add(unsigned char _key[],unsigned _value){
        data_type tmp;
        put_char_to_block(tmp.key,0,0,32,_key);
        put_int_to_block(tmp.value,0,_value);
        Insert(tmp);
        save();
    }
    void add(unsigned _key,unsigned _value){
        data_type tmp;
        put_int_to_block(tmp.key,0,_key);
        put_int_to_block(tmp.value,0,_value);
        Insert(tmp);
        save();
    }

    void drop(unsigned char _key[]){
        data_type tmp;
        put_char_to_block(tmp.key,0,0,32,_key);
        Remove(tmp);
        save();
    }
    void drop(unsigned _key){
        data_type tmp;
        put_int_to_block(tmp.key,0,_key);
        Remove(tmp);
        save();
    }

    int get_by_key(unsigned char key[],unsigned char value[]){
        memory_location ret;
        data_type tmp;
        put_char_to_block(tmp.key,0,0,32,key);
        ret = Find(tmp);

        if (ret.is_null()) return -1;
        binary_tree_node ret_node;
        ret_node.self = ret;
        ret_node.load();
        memcpy(value,ret_node.data.value,32);
        return 0;
    }
    int get_by_key(unsigned char key[],unsigned *value){
        memory_location ret;
        data_type tmp;
        put_char_to_block(tmp.key,0,0,32,key);
        ret = Find(tmp);

        if (ret.is_null()) return -1;
        binary_tree_node ret_node;
        ret_node.self = ret;
        ret_node.load();
        *value = trans_block_to_int(ret_node.data.value,0,4);
        return 0;
    }
    int get_by_key(unsigned key,unsigned char value[]){
        memory_location ret;
        data_type tmp;
        put_int_to_block(tmp.key,0,key);
        ret = Find(tmp);

        if (ret.is_null()) return -1;
        binary_tree_node ret_node;
        ret_node.self = ret;
        ret_node.load();
        memcpy(value,ret_node.data.value,32);
        return 0;
    }
    int get_by_key(unsigned key,unsigned *value){
        memory_location ret;
        data_type tmp;
        put_int_to_block(tmp.key,0,key);
        ret = Find(tmp);

        if (ret.is_null()) return -1;
        binary_tree_node ret_node;
        ret_node.self = ret;
        ret_node.load();
        *value = trans_block_to_int(ret_node.data.value,0,4);
        return 0;
    }

    int update(unsigned char key[],unsigned char value[]){
        memory_location ret;
        data_type tmp;
        put_char_to_block(tmp.key,0,0,32,key);
        ret = Find(tmp);

        if (ret.is_null()) return -1;
        binary_tree_node ret_node;
        ret_node.self = ret;
        ret_node.load();
        memcpy(ret_node.data.value,value,32);
        ret_node.save();
        save();
        return 0;
    }
    int update(unsigned char key[],unsigned value){
        memory_location ret;
        data_type tmp;
        put_char_to_block(tmp.key,0,0,32,key);
        ret = Find(tmp);

        if (ret.is_null()) return -1;
        binary_tree_node ret_node;
        ret_node.self = ret;
        ret_node.load();
        put_int_to_block(ret_node.data.value,0,value);
        ret_node.save();
        save();
        return 0;
    }
    int update(unsigned key,unsigned char value[]){
        memory_location ret;
        data_type tmp;
        put_int_to_block(tmp.key,0,key);
        ret = Find(tmp);

        if (ret.is_null()) return -1;
        binary_tree_node ret_node;
        ret_node.self = ret;
        ret_node.load();
        memcpy(ret_node.data.value,value,32);
        ret_node.save();
        save();
        return 0;
    }
    int update(unsigned key,unsigned value){
        memory_location ret;
        data_type tmp;
        put_int_to_block(tmp.key,0,key);
        ret = Find(tmp);

        if (ret.is_null()) return -1;
        binary_tree_node ret_node;
        ret_node.self = ret;
        ret_node.load();
        put_int_to_block(ret_node.data.value,0,value);
        ret_node.save();
        save();
        return 0;
    }

    bool exists(unsigned char key[]){
        memory_location ret;
        data_type tmp;
        put_char_to_block(tmp.key,0,0,32,key);
        ret = Find(tmp);

        return !ret.is_null();
    }
    bool exists(unsigned key){
        memory_location ret;
        data_type tmp;
        put_int_to_block(tmp.key,0,key);
        ret = Find(tmp);

        return !ret.is_null();
    }

    unsigned get_key(memory_location root,unsigned x[],unsigned num){
        if (root.is_null()) return 0;
        binary_tree_node node;
        node.self = root;
        node.load();
        x[num++]=trans_block_to_int(node.data.key,0,4);
        num+=get_key(node.left,x,num);
        num+=get_key(node.right,x,num);
        return node.size;
    }
    unsigned get_key(memory_location root,unsigned char x[][32],unsigned num){
        if (root.is_null()) return 0;
        binary_tree_node node;
        node.self = root;
        node.load();
        memcpy(x[num++],node.data.key,32);
        num+=get_key(node.left,x,num);
        num+=get_key(node.right,x,num);
        return node.size;
    }
    unsigned get_all_key(unsigned x[]){
        return get_key(root,x,0);
    }
    unsigned get_all_key(unsigned char x[][32]){
        return get_key(root,x,0);
    }
};
