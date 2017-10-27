#ifndef _NODE_CLASS_H_
#include "node_class.h"
#endif

#ifndef _USEFUL_TOOLS_H
#include "useful_tools.h"
#endif 

#include <iostream>
#include <time.h>
#define _TREAP_H_ 0
using namespace std;

struct TREAP{
    memory_location self;
    memory_location root;
    bool k_type,v_type;
    unsigned hash_top;
    unsigned char name[32];
    unsigned block_size;
    TREAP(){}
    TREAP(const char xname[]){
        memset(name,0,sizeof name);
        for (int i = 0;i<32;i++){
            if (!xname[i]) break;
            else name[i] = xname[i];
        }
        self.filenum = self.linenum = -1;
        block_size = node_block_size;
        root.set_null();
        srand(time(NULL));
        hash_top = 0;
    }
    int get_size(){
        binary_tree_node tmp;
        tmp.self = root;
        tmp.load();
        return tmp.size;
	}
    void adapt(memory_location x){
        binary_tree_node tmp,tmp_left,tmp_right;
        tmp.self = x;
        tmp.load();
        tmp_left.self = tmp.left;
        tmp_left.load();
        tmp_right.self = tmp.right;
        tmp_right.load();
		tmp.size=tmp_left.size+tmp_right.size+1;
		tmp.save();
	}
    void left_rot(memory_location fa,memory_location x){
        memory_location y;
        binary_tree_node node_x,node_y,node_fa;
        node_fa.self = fa;
        node_fa.load();
        node_x.self = x;
        node_x.load();

		y=node_x.right;
		node_y.self = y;
		node_y.load();

		node_x.right=node_y.left;
		node_y.left=x;
		node_y.size=node_x.size;
        node_x.save();
		node_y.save();

		if (!fa.is_null()){
            if (node_fa.left==x) node_fa.left=y;
            else node_fa.right=y;
            node_fa.save();
		}

		adapt(x);
		if (root == x) root = y;
	}
	void right_rot(memory_location fa,memory_location x){
        memory_location y;
        binary_tree_node node_x,node_y,node_fa;
        node_fa.self = fa;
        node_fa.load();
        node_x.self = x;
        node_x.load();

		y=node_x.left;
		node_y.self = y;
		node_y.load();

		node_x.left=node_y.right;
		node_y.right=x;
		node_y.size=node_x.size;

        node_x.save();
		node_y.save();
        if (!fa.is_null()){
            if (node_fa.left==x) node_fa.left=y;
            else node_fa.right=y;
            node_fa.save();
		}
		adapt(x);
		if (root == x) root = y;
	}
    memory_location insert(memory_location fa,memory_location x,data_type data){
		if (x.is_null()){
			binary_tree_node node_x;
			node_x.create();
            node_x.data.k_type = k_type;
			node_x.data.v_type = v_type;

			if (root.is_null()) root=node_x.self;
			node_x.size=1;
			node_x.left.set_null();
			node_x.right.set_null();
			node_x.data=data;
			node_x.HASH=++hash_top;
			node_x.RANDOM=rand();
			node_x.save();
			return node_x.self;
		}
		else{
            binary_tree_node node_x;
            node_x.self = x;
            node_x.load();

			node_x.size++;
			node_x.save();
			data.k_type = k_type;
			if (data<node_x.data||(data==node_x.data)&&(node_x.HASH>(hash_top+1))) {
                memory_location tmp = insert(x,node_x.left,data);
                if (node_x.left.is_null()) {
                    node_x.left=tmp;
                    node_x.save();
                }

                binary_tree_node node_left;
                node_left.self = node_x.left;
                node_left.load();
                if (node_x.RANDOM>node_left.RANDOM) right_rot(fa,x);
                return tmp;
            }
			else {
                memory_location tmp = insert(x,node_x.right,data);
                if (node_x.right.is_null()) {
                    node_x.right=tmp;
                    node_x.save();
                }

                binary_tree_node node_right;
                node_right.self = node_x.right;
                node_right.load();
                if (node_x.RANDOM>node_right.RANDOM) left_rot(fa,x);
                return tmp;
            }
		}
	}
    memory_location remove(memory_location fa,memory_location x,data_type data){
        memory_location null;
        null.set_null();
		if (x.is_null()) return null;

		binary_tree_node node_fa,node_x;
		node_x.self = x;
		node_x.load();
		node_fa.self = fa;
		node_fa.load();

		data.k_type = k_type;
		if (node_x.data<data){
			memory_location kk=remove(x,node_x.right,data);
			if (!kk.is_null()) {node_x.load();node_x.size--;node_x.save();}
			return kk;
        }
		else
		if (data<node_x.data){
			memory_location kk=remove(x,node_x.left,data);
			if (!kk.is_null()) {node_x.load();node_x.size--;node_x.save();}
			return kk;
        }
		else{
			if (!node_x.left.is_null()&&node_x.right.is_null()){
                binary_tree_node node_left;
                node_left.self = node_x.left;
                node_left.load();

				node_x=node_left;
				node_x.save();
				node_left.release();
				return x;
			}
			else
			if (!node_x.right.is_null()&&node_x.left.is_null()){
                binary_tree_node node_right;
                node_right.self = node_x.right;
                node_right.load();

				node_x = node_right;
				node_x.save();
				node_right.release();
				return x;
			}
			else
			if (node_x.left.is_null()&&node_x.right.is_null()){
				if (root==x) root.set_null();
				if (!fa.is_null()){
					if (node_fa.left==x) node_fa.left.set_null();
					else node_fa.right.set_null();
					node_fa.save();
                }
                node_x.release();
                return x;
			}
			else{
				memory_location ret=node_x.right,faa=x;
				node_x.size--;
				node_x.save();

				binary_tree_node node_ret,node_faa;
				node_ret.self = ret;
				node_ret.load();
				while (!node_ret.left.is_null()){
                    node_ret.size--;
                    node_ret.save();
                    faa=node_ret.self;
                    node_ret.self = node_ret.left;;
                    node_ret.load();
				}

				node_faa.self = faa;
				node_faa.load();

				if (faa==x) node_x.right=node_ret.right;
				else {
                    node_faa.left=node_ret.right;
                    node_faa.save();
                }

                node_x.data=node_ret.data;
                node_x.save();
                node_ret.release();
                return x;
			}
		}
	}
	void Insert(data_type data){
        memory_location null;
        null.set_null();
        insert(null,root,data);
	}
	memory_location Remove(data_type data){
        return remove(root,root,data);
	}

    memory_location Find(data_type data){
        data.k_type = k_type;
		if (root.is_null()) {
            memory_location null;
            null.set_null();
            return null;
        }
		else {
			memory_location x=root;
			binary_tree_node node_x;
			node_x.self = x;
			node_x.load();

			while (!(node_x.data==data)&&!x.is_null()){
				if (node_x.data<data) x=node_x.right;
				else x=node_x.left;
				node_x.self = x;
				node_x.load();
			}
			if (x.is_null()) {
                memory_location null;
                null.set_null();
                return null;
			}
			else if (node_x.data==data) return x;
		}
	}

	int show(memory_location x,int index){
        if (x.is_null()) return index;
        binary_tree_node node_x,left_node,right_node;
        node_x.self = x;
        node_x.load();
        left_node.self = node_x.left;
        if (left_node.self.is_null()) left_node.data.key[0] = 0;
        else left_node.load();

        right_node.self = node_x.right;
        if (right_node.self.is_null()) right_node.data.key[0] = 0;
        else  right_node.load();

        if (!node_x.left.is_null()) index+=show(node_x.left,index);
        if (k_type) {
            printf("[%d]\t\"",index++);
            uout(node_x.data.key,32);
            printf("\"\t:\t");
        }
        else {
            unsigned tmp = trans_block_to_int(node_x.data.key,0,4);
            printf("[%d]\t%d\t:\t",index++,tmp);
        }
        if (v_type) {
            printf("\"");
            uout(node_x.data.value,32);
            printf("\"");
            printf("\n");
        }
        else {
            unsigned tmp = trans_block_to_int(node_x.data.value,0,4);
            printf("%d\n",tmp);
        }
        if (!node_x.right.is_null()) index+=show(node_x.right,index);

        return index;
	}

	void save(){
        unsigned char buffer[block_size];
        put_int_to_block(buffer,0,root.filenum);
        put_int_to_block(buffer,4,root.linenum);
        put_int_to_block(buffer,8,hash_top);
        put_char_to_block(buffer,12,0,32,name);
        unsigned tmpmode = 0;
        if (k_type) tmpmode |= 128;
        if (v_type) tmpmode |= 64;
        put_int_to_block(buffer,44,tmpmode);
        write_node_block(buffer,self.filenum,self.linenum);
	}
	void load(){
        unsigned char buffer[block_size];
        read_node_block(self.filenum,self.linenum,buffer);
        root.filenum = trans_block_to_int(buffer,0,4);
        root.linenum = trans_block_to_int(buffer,4,4);
        hash_top = trans_block_to_int(buffer,8,4);
        trans_block_to_char_array(buffer,12,32,name);
        k_type = (trans_block_to_int(buffer,44,4) & 128) != 0;
        v_type = (trans_block_to_int(buffer,44,4) & 64) != 0;
	}
	void create(){
        new_node_position(&self.filenum,&self.linenum);
	}
    void release_node(memory_location x){
        binary_tree_node node_x;
        node_x.self = x;
        node_x.load();
        if (!node_x.left.is_null()) release_node(node_x.left);
        if (!node_x.right.is_null()) release_node(node_x.right);
        delete_node(x.filenum,x.linenum);
    }
    void clear(){
        release_node(root);
        root.set_null();
        save();
    }
    void release(){
        release_node(root);
        delete_node(self.filenum,self.linenum);
    }
};
