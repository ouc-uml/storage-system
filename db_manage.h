#ifndef _DB_MAP_H_
#include "db_map.h"
#endif

#ifndef _DB_LIST_H_
#include "db_list.h"
#endif

#ifndef _USEFUL_TOOLS_H_
#include "useful_tools.h"
#endif
#define _DB_MANAGE_H_ 0

struct db_struct{
	db_map map1,map2;

	void save(){
	    int fd = open("db_root.dat",O_RDWR ,S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |S_IROTH);
	    if (fd <0) fd = open("db_root.dat",O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |S_IROTH);
	    if (fd <0) {
	        printf("write_node_block error, cannot create block\n");
	        return ;
	    }

	    struct flock lock;
	    lock.l_type = F_WRLCK;
	    lock.l_whence = SEEK_SET;
	    int line_num = 0;
	    lock.l_start = line_num*node_block_size;
	    lock.l_len = node_block_size;

	    fcntl(fd,F_SETLKW,&lock);

	    lseek(fd,line_num*node_block_size,SEEK_SET);
	    unsigned char data[node_block_size];
	    put_int_to_block(data,0,map1.self.filenum);
	    put_int_to_block(data,4,map1.self.linenum);
	    put_int_to_block(data,8,map2.self.filenum);
	    put_int_to_block(data,12,map2.self.linenum);
	    write(fd,data,node_block_size);

	    lock.l_type = F_UNLCK;
	    fcntl(fd,F_SETLKW,&lock);
	    close(fd);
	}

	db_struct(int any_num){
	    int fd = open("db_root.dat",O_RDWR ,S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |S_IROTH);
	    if (fd <0) {
	        printf("read_node_block error, file not exists\n");
	        return ;
	    }

	    unsigned char result[node_block_size];
	    int linenum = 0;

	    struct flock lock;
	    lock.l_type = F_RDLCK;
	    lock.l_whence = SEEK_SET;
	    int line_num = 0;
	    lock.l_start = line_num*node_block_size;
	    lock.l_len = node_block_size;

	    fcntl(fd,F_SETLKW,&lock);

	    read(fd,result,node_block_size);
	    map1.self.filenum = trans_block_to_int(result,0,4);
	    map1.self.linenum = trans_block_to_int(result,4,4);
	    map2.self.filenum = trans_block_to_int(result,8,4);
	    map2.self.linenum = trans_block_to_int(result,12,4);

	    lock.l_type = F_UNLCK;
	    fcntl(fd,F_SETLKW,&lock);
	    close(fd);
	    map1.load();
	    map2.load();
	}
	db_struct(){}
	/*
	危险函数，调用之后会丢失之前的管理库并创建新的管理库
	*/
	void install(){
		map1.create();
		map1.k_type = map1.v_type = 1;
		map1.save();
		map2.create();
		map2.k_type = map2.v_type = 1;
		map2.save();
		save();
	}

	bool exists_map(const char _name[]){
        unsigned char name[k_len];
        uprint(name,k_len,"s",_name);
        return map1.exists(name);
	}
	bool exists_list(const char _name[]){
        unsigned char name[k_len];
        uprint(name,k_len,"s",_name);
        return map2.exists(name);
	}
	db_map get_map(const char name[]){
		unsigned char key[k_len],value[v_len];
		uprint(key,k_len,"s",name);
		int ifexists = map1.get_by_key(key,value);

		db_map ret;
		if (ifexists == -1) ret.self.set_null();
		else {
			ret.self.filenum = trans_block_to_int(value,0,4);
			ret.self.linenum = trans_block_to_int(value,4,4);
			ret.load();
		}
		return ret;
	}
	db_map create_map(const char name[],char k_ty,char v_ty){
		if (exists_map(name)) return get_map(name);
		db_map ret(name,k_ty,v_ty);
		ret.create();
		ret.save();

		unsigned char key[k_len],value[v_len];
		uprint(key,k_len,"s",name);
		put_int_to_block(value,0,ret.self.filenum);
		put_int_to_block(value,4,ret.self.linenum);
		map1.add(key,value);
		save();
		return ret;
	}
	void delete_map(const char name[]){
		unsigned char key[k_len];
		uprint(key,k_len,"s",name);
		db_map tmp = get_map(name);
		tmp.release();
		map1.drop(key);
		save();
	}
	void show_all_map(){
		int map_num = map1.get_size();
		unsigned char ret[map_num+1][k_len];
		map1.get_all_key(ret);
		for (int i = 0;i<map_num;i++)
			printf("%s\n", ret[i]);
	}

	db_list get_list(const char name[]){
		unsigned char key[k_len],value[v_len];
		uprint(key,k_len,"s",name);
		int ifexists = map2.get_by_key(key,value);

		db_list ret;
		if (ifexists == -1) ret.self.set_null();
		else {
			ret.self.filenum = trans_block_to_int(value,0,4);
			ret.self.linenum = trans_block_to_int(value,4,4);
			ret.load();
		}
		return ret;
	}
	db_list create_list(const char name[],char k_ty){
		if (exists_list(name)) return get_list(name);
		db_list ret(name,k_ty);
		ret.create();
		ret.save();

		unsigned char key[k_len],value[v_len];
		uprint(key,k_len,"s",name);
		put_int_to_block(value,0,ret.self.filenum);
		put_int_to_block(value,4,ret.self.linenum);
		map2.add(key,value);
		save();
		return ret;
	}
	void delete_list(const char name[]){
		unsigned char key[k_len];
		uprint(key,k_len,"s",name);
		db_list tmp = get_list(name);
		tmp.release();
		map2.drop(key);
		save();
	}
	void show_all_list(){
		int map_num = map2.get_size();
		unsigned char ret[map_num+1][k_len];
		map2.get_all_key(ret);
		for (int i = 0;i<map_num;i++)
			printf("%s\n", ret[i]);
	}
};
