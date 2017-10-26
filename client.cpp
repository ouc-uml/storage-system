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
		
		db_struct db(1);
		if (args[0] == "show"){
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

				tmp_map.show(tmp_map.root);
			}
			else if (args[1] == "list"){
				db_list tmp_list = db.get_list(args[2].c_str());

				int start = atoi(args[3].c_str()),end = atoi(args[4].c_str());
				tmp_list.show(start,end);
			}
		}
	}
	return 0;
}