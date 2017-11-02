#include "db_manage.h"
#include <vector>
#include <sstream>
#include <regex>
#include <unistd.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#define PORT 12345
#define MAXDATASIZE 512
#define BACKLOG 1

vector<string> get_arg(string order){
	vector<std::string> result;
	istringstream iss(order);
	for(std::string s; iss >> s; )
    result.push_back(s);
	
	return result;
}
void send_file(const char filename[],int connectfd,int iret){
	int fd = open(filename,O_RDWR ,S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |S_IROTH);
    if (fd <0) {
        printf("send error, file not exists\n");
        return ;
    }

	int sz = lseek(fd, 0L, SEEK_END);
	lseek(fd, 0L, SEEK_SET);

    unsigned char buf[MAXDATASIZE];
	for (int i = 0;i<sz;i+=MAXDATASIZE-4){
		unsigned nn = read(fd,buf+4,MAXDATASIZE-4);

		if (i+MAXDATASIZE-4<sz) put_int_to_block(buf,0,nn+MAXDATASIZE);
		else put_int_to_block(buf,0,nn);

		if (nn == 0) break;

		write(connectfd,buf,nn+4);
	}
	close(fd);
}

int main(){
	load_configuration();

/*
network connection start
*/
    char recebuf[MAXDATASIZE];
    int listenfd, connectfd;

    struct sockaddr_in server; //server's address information
    struct sockaddr_in client;  // client's address information

    socklen_t addrlen;

    /*create tcp socket*/
    if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        printf("create socket failed\n");
        exit(1);
    }

    //set socket options
    int opt = SO_REUSEADDR;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    //bind socket
    if(bind(listenfd, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        printf("bind error\n");
        exit(1);
    }

    //listen scoket
    if(listen(listenfd, BACKLOG) == -1)
    {
        printf("listen error\n");
        exit(1);
    }

    addrlen = sizeof(client);
/*
network connection end
*/

	while (1){
		if((connectfd = accept(listenfd, (struct sockaddr *)&client, &addrlen)) == -1)
        {
            printf("accept error\n");
            exit(1);
        }

        struct timeval tv;
        gettimeofday(&tv, NULL);
        printf("you got a connnetion from client's ip %s, port %d at time %ld.%ld\n", inet_ntoa(client.sin_addr), htons(client.sin_port),
            tv.tv_sec, tv.tv_usec);

        int iret = -1;

        FILE *fp = fopen("tmp_out.txt","w");
        fprintf(fp, ">>");
        fclose(fp);
        send_file("tmp_out.txt",connectfd,iret);

        while (1){
			iret = recv(connectfd, recebuf, MAXDATASIZE, 0);
        	if (iret<=0)
            {
                close(connectfd);
                break;
            }

			string order = recebuf;

			vector<string> args = get_arg(order);
			for (;args.size()<10;args.push_back(""));
			
			FILE *fp = fopen("tmp_out.txt","w");
			if (args[0] == "show"){
				db_struct db(1);
				if (args[1] == "db"){
					if (args[2] == "map"){
						int size = db.map1.get_size();
						unsigned char map_name[size][k_len];
						db.map1.get_all_key(map_name);

						regex e(args[3]);
						for (int i = 0;i<size;i++){
							if (regex_match((char *)map_name[i],e))
								fprintf(fp,"%s\n", map_name[i]);
						}
					}
					if (args[2] == "list"){
						int size = db.map2.get_size();
						unsigned char list_name[size][k_len];
						db.map2.get_all_key(list_name);

						regex e(args[3]);
						for (int i = 0;i<size;i++){
							if (regex_match((char *)list_name[i],e))
								fprintf(fp,"%s\n", list_name[i]);
						}
					}
				}
				else if (args[1] == "map"){
					db_map tmp_map = db.get_map(args[2].c_str());
					if (tmp_map.self.is_null()) {
						fprintf(fp,"map not exist\n");
						fprintf(fp, ">>");
						fclose(fp);
						send_file("tmp_out.txt",connectfd,iret);
						continue;
					}

					tmp_map.show(fp,tmp_map.root,0);
				}
				else if (args[1] == "list"){
					db_list tmp_list = db.get_list(args[2].c_str());
					if (tmp_list.self.is_null()) {
						fprintf(fp,"list not exist\n");
						fprintf(fp, ">>");
						fclose(fp);
						send_file("tmp_out.txt",connectfd,iret);
						continue;
					}

					int start = atoi(args[3].c_str()),end = atoi(args[4].c_str());
					tmp_list.show(fp,start,end);
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
					if (tmp.self.is_null()) {
						fprintf(fp,"map not exist\n");
						fprintf(fp, ">>");
						fclose(fp);
						send_file("tmp_out.txt",connectfd,iret);
						continue;
					}
					if (tmp.k_type&&tmp.v_type) {
						unsigned char key[k_len],value[v_len];
						uprint(key,k_len,"s",args[3].c_str());
						uprint(value,v_len,"s",args[4].c_str());
						tmp.add(key,value);
					}
					else if (tmp.k_type&&!tmp.v_type) {
						unsigned tmp_value = atoi(args[4].c_str());
						unsigned char key[k_len];
						uprint(key,k_len,"s",args[3].c_str());
						tmp.add(key,tmp_value);
					}
					else if (!tmp.k_type&&tmp.v_type) {
						unsigned tmp_key = atoi(args[3].c_str());
						unsigned char value[v_len];
						uprint(value,v_len,"s",args[4].c_str());
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
					if (tmp.self.is_null()) {
						fprintf(fp,"list not exist\n");
						fprintf(fp, ">>");
						fclose(fp);
						send_file("tmp_out.txt",connectfd,iret);
						continue;
					}
					if (tmp.k_type) {
						unsigned char key[q_len];
						uprint(key,q_len,"s",args[3].c_str());
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
				if (tmp.self.is_null()) {
					fprintf(fp,"map not exist\n");
					fprintf(fp, ">>");
					fclose(fp);
					send_file("tmp_out.txt",connectfd,iret);
					continue;
				}

				if (tmp.k_type){
					unsigned char key[k_len];
					uprint(key,k_len,"s",args[2].c_str());
					tmp.drop(key);
				}
				else {
					unsigned key = atoi(args[2].c_str());
					tmp.drop(key);
				}
			}
			else if (args[0] == "pop"){
				db_struct db(1);
				db_list tmp = db.get_list(args[1].c_str());
				if (tmp.self.is_null()) {
					fprintf(fp,"list not exist\n");
					fprintf(fp, ">>");
					fclose(fp);
					send_file("tmp_out.txt",connectfd,iret);
					continue;
				}
				tmp.pop_tail();
			}
			else if (args[0] == "update"){
				db_struct db(1);
				if (args[1] == "map"){
					db_map tmp = db.get_map(args[2].c_str());
					if (tmp.self.is_null()) {
						fprintf(fp,"map not exist\n");
						fprintf(fp, ">>");
						fclose(fp);
						send_file("tmp_out.txt",connectfd,iret);
						continue;
					}
					unsigned char key[k_len],value[v_len];
					if (tmp.k_type && tmp.v_type){
						uprint(key,k_len,"s",args[3].c_str());
						uprint(value,v_len,"s",args[4].c_str());
						tmp.update(key,value);
					}
					else if (tmp.k_type && !tmp.v_type){
						uprint(key,k_len,"s",args[3].c_str());
						unsigned value = atoi(args[4].c_str());
						tmp.update(key,value);
					}
					else if (!tmp.k_type && tmp.v_type){
						uprint(value,v_len,"s",args[4].c_str());
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
					if (tmp.self.is_null()) {
						fprintf(fp,"list not exist\n");
						fprintf(fp, ">>");
						fclose(fp);
						send_file("tmp_out.txt",connectfd,iret);
						continue;
					}

					int index = atoi(args[3].c_str());
					unsigned char value[q_len];
					if (tmp.k_type){
						uprint(value,q_len,"s",args[4].c_str());
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
					fprintf(fp,"%llu\n",used_memory());
				}
				else if (args[1] == "map"){
					db_map tmp = db.get_map(args[2].c_str());
					if (tmp.self.is_null()) {
						fprintf(fp,"map not exist\n");
						fprintf(fp, ">>");
						fclose(fp);
						send_file("tmp_out.txt",connectfd,iret);
						continue;
					}
					fprintf(fp,"%u\n", tmp.get_size());
				}
				else if (args[1] == "list"){
					db_list tmp_list = db.get_list(args[2].c_str());
					if (tmp_list.self.is_null()) {
						fprintf(fp,"list not exist\n");
						fprintf(fp, ">>");
						fclose(fp);
						send_file("tmp_out.txt",connectfd,iret);
						continue;
					}
					fprintf(fp,"%u\n", tmp_list.size);
				}
			}
			else if (args[0] == "get"){
				db_struct db(1);
				db_map tmp = db.get_map(args[1].c_str());
				if (tmp.self.is_null()) {
					fprintf(fp,"map not exist\n");
					fprintf(fp, ">>");
					fclose(fp);
					send_file("tmp_out.txt",connectfd,iret);
					continue;
				}
				unsigned char key[k_len],value[v_len];
				if (tmp.k_type && tmp.v_type){
					uprint(key,k_len,"s",args[2].c_str());
					tmp.get_by_key(key,value);
					fprintf(fp,"%s\n", value);
				}
				else if (tmp.k_type && !tmp.v_type){
					uprint(key,k_len,"s",args[2].c_str());
					unsigned value;
					tmp.get_by_key(key,&value);
					fprintf(fp,"%u\n", value);
				}
				else if (!tmp.k_type && tmp.v_type){
					unsigned key = atoi(args[2].c_str());
					tmp.get_by_key(key,value);
					fprintf(fp,"%s\n", value);
				}
				else if (!tmp.k_type && !tmp.v_type){
					unsigned key = atoi(args[2].c_str());
					unsigned value;
					tmp.get_by_key(key,&value);
					fprintf(fp,"%u\n", value);
				}
			}
			else if (args[0] == "reset"){
				fprintf(fp,"Do you want to reset the database?[y/n]\n");
				fclose(fp);
				send_file("tmp_out.txt",connectfd,iret);
				fp = fopen("tmp_out.txt","w");
				string confirm;
				
				iret = recv(connectfd, recebuf, MAXDATASIZE, 0);
	        	if (iret<=0)
	            {
	                close(connectfd);
	                break;
	            }
	            confirm = recebuf;

				if (confirm[0] == 'y' && confirm.length() == 1){
					clear_all_nodes();
					db_struct a;
					a.install();
				}
			}
			else if (args[0] == "exit"){
				fprintf(fp,"Good Bye~\n");
				fclose(fp);
				send_file("tmp_out.txt",connectfd,iret);
				break;
			}
			fprintf(fp, ">>");
			fclose(fp);
			send_file("tmp_out.txt",connectfd,iret);
		}
	}

//close the network connection
    close(listenfd);

	return 0;
}
