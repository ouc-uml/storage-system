#include "db_manage.h"
#include <vector>
#include <sstream>
#include <regex>

vector<string> get_arg(string order){
	vector<std::string> result;
	istringstream iss(order);
	for(std::string s; iss >> s; )
    result.push_back(s);
	
	return result;
}

int main(){
	load_configuration();

	while (1){
		cout<<">>";
		string order;
		getline(cin,order);

		vector<string> args = get_arg(order);
		for (;args.size()<10;args.push_back(""));
		
		if (args[0] == "show"){
			db_struct db(1);
			if (args[1] == "db"){
				if (args[2] == "map"){
					int size = db.map1.get_size();
					unsigned char map_name[size][32];
					db.map1.get_all_key(map_name);

					regex e(args[3]);
					for (int i = 0;i<size;i++){
						if (regex_match((char *)map_name[i],e))
							printf("%s\n", map_name[i]);
					}
				}
				if (args[2] == "list"){
					int size = db.map2.get_size();
					unsigned char list_name[size][32];
					db.map2.get_all_key(list_name);

					regex e(args[3]);
					for (int i = 0;i<size;i++){
						if (regex_match((char *)list_name[i],e))
							printf("%s\n", list_name[i]);
					}
				}
			}
			else if (args[1] == "map"){
				db_map tmp_map = db.get_map(args[2].c_str());
				if (tmp_map.self.is_null()) {printf("map not exist\n");continue;}

				tmp_map.show(tmp_map.root,0);
			}
			else if (args[1] == "list"){
				db_list tmp_list = db.get_list(args[2].c_str());
				if (tmp_list.self.is_null()) {printf("list not exist\n");continue;}

				int start = atoi(args[3].c_str()),end = atoi(args[4].c_str());
				tmp_list.show(start,end);
			}
		}
		else if (args[0] == "create"){
			db_struct db(1);
			if (args[1] == "map"){
				db.create_map(args[2].c_str(),args[3][0],args[4][0]);
			}
			else if (args[1] == "list"){
				db.create_list(args[2].c_str(),args[3][0]);
			}
		}
		else if (args[0] == "add"){
			db_struct db(1);
			if (args[1] == "map"){
				db_map tmp = db.get_map(args[2].c_str());
				if (tmp.self.is_null()) {printf("map not exist\n");continue;}
				if (tmp.k_type&&tmp.v_type) {
					unsigned char key[32],value[32];
					uprint(key,32,"s",args[3].c_str());
					uprint(value,32,"s",args[4].c_str());
					tmp.add(key,value);
				}
				else if (tmp.k_type&&!tmp.v_type) {
					unsigned tmp_value = atoi(args[4].c_str());
					unsigned char key[32];
					uprint(key,32,"s",args[3].c_str());
					tmp.add(key,tmp_value);
				}
				else if (!tmp.k_type&&tmp.v_type) {
					unsigned tmp_key = atoi(args[3].c_str());
					unsigned char value[32];
					uprint(value,32,"s",args[4].c_str());
					tmp.add(tmp_key,value);
				}
				else {
					unsigned tmp_key = atoi(args[3].c_str());
					unsigned tmp_value = atoi(args[4].c_str());
					tmp.add(tmp_key,tmp_value);
				}
			}
			else if (args[1] == "list"){
				db_list tmp = db.get_list(args[2].c_str());
				if (tmp.self.is_null()) {printf("list not exist\n");continue;}
				if (tmp.k_type) {
					unsigned char key[32];
					uprint(key,32,"s",args[3].c_str());
					tmp.push_tail(key);
				}
				else {
					unsigned tmp_value = atoi(args[3].c_str());
					tmp.push_tail(tmp_value);
				}
			}
		}
		else if (args[0] == "drop"){
			db_struct db(1);
			if (args[1] == "map"){
				db.delete_map(args[2].c_str());
			}
			else if (args[1] == "list"){
				db.delete_list(args[2].c_str());
			}
		}
		else if (args[0] == "delete"){
			db_struct db(1);
			db_map tmp = db.get_map(args[1].c_str());
			if (tmp.self.is_null()) {printf("map not exist\n");continue;}
			unsigned char key[32];
			uprint(key,32,"s",args[2].c_str());
			tmp.drop(key);
		}
		else if (args[0] == "pop"){
			db_struct db(1);
			db_list tmp = db.get_list(args[1].c_str());
			if (tmp.self.is_null()) {printf("list not exist\n");continue;}
			tmp.pop_tail();
		}
		else if (args[0] == "update"){
			db_struct db(1);
			if (args[1] == "map"){
				db_map tmp = db.get_map(args[2].c_str());
				if (tmp.self.is_null()) {printf("map not exist\n");continue;}
				unsigned char key[32],value[32];
				if (tmp.k_type && tmp.v_type){
					uprint(key,32,"s",args[3].c_str());
					uprint(value,32,"s",args[4].c_str());
					tmp.update(key,value);
				}
				else if (tmp.k_type && !tmp.v_type){
					uprint(key,32,"s",args[3].c_str());
					unsigned value = atoi(args[4].c_str());
					tmp.update(key,value);
				}
				else if (!tmp.k_type && tmp.v_type){
					uprint(value,32,"s",args[4].c_str());
					unsigned key = atoi(args[3].c_str());
					tmp.update(key,value);
				}
				else if (!tmp.k_type && !tmp.v_type){
					unsigned key = atoi(args[3].c_str());
					unsigned value = atoi(args[4].c_str());
					tmp.update(key,value);
				}
			}
			else if (args[1] == "list"){
				db_list tmp = db.get_list(args[2].c_str());
				if (tmp.self.is_null()) {printf("list not exist\n");continue;}

				int index = atoi(args[3].c_str());
				unsigned char value[32];
				if (tmp.k_type){
					uprint(value,32,"s",args[4].c_str());
					tmp.update(index,value);
				}
				else {
					unsigned value = atoi(args[4].c_str());
					tmp.update(index,value);
				}
			}
		}
		else if (args[0] == "size"){
			db_struct db(1);
			if (args[1] == "db"){
				printf("%llu\n",used_memory());
			}
			else if (args[1] == "map"){
				db_map tmp = db.get_map(args[2].c_str());
				if (tmp.self.is_null()) {printf("map not exist\n");continue;}
				printf("%u\n", tmp.get_size());
			}
			else if (args[1] == "list"){
				db_list tmp_list = db.get_list(args[2].c_str());
				if (tmp_list.self.is_null()) {printf("list not exist\n");continue;}
				printf("%u\n", tmp_list.size);
			}
		}
		else if (args[0] == "get"){
			db_struct db(1);
			db_map tmp = db.get_map(args[1].c_str());
			if (tmp.self.is_null()) {printf("map not exist\n");continue;}
			unsigned char key[32],value[32];
			if (tmp.k_type && tmp.v_type){
				uprint(key,32,"s",args[2].c_str());
				tmp.get_by_key(key,value);
				printf("%s\n", value);
			}
			else if (tmp.k_type && !tmp.v_type){
				uprint(key,32,"s",args[2].c_str());
				unsigned value;
				tmp.get_by_key(key,&value);
				printf("%u\n", value);
			}
			else if (!tmp.k_type && tmp.v_type){
				unsigned key = atoi(args[3].c_str());
				tmp.get_by_key(key,value);
				printf("%s\n", value);
			}
			else if (!tmp.k_type && !tmp.v_type){
				unsigned key = atoi(args[3].c_str());
				unsigned value;
				tmp.get_by_key(key,&value);
				printf("%u\n", value);
			}
		}
		else if (args[0] == "reset"){
			printf("Do you want to reset the database?[y/n]\n");
			string confirm;
			getline(cin,confirm);
			if (confirm[0] == 'y' && confirm.length() == 1){
				clear_all_nodes();
				db_struct a;
				a.install();
			}
		}
		else if (args[0] == "exit"){
			printf("Good Bye~\n");
			break;
		}
	}
	return 0;
}