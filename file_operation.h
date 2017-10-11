#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#define _FILE_OPERATION_H_ 0

char message_queue[32];
unsigned node_block_size = 128;
unsigned file_block_size = 1024;
char folder_name[64] = "database";
unsigned max_node_block_number = 8192;

/*
加载文件系统的配置信息，在所有的操作进行之前进行
*/
void load_configuration(){
    char tmp[32];
    FILE *fp = fopen("global.conf","r");

    fscanf(fp,"%s",tmp);
    fscanf(fp,"%s",message_queue);

    fscanf(fp,"%s",tmp);
    fscanf(fp,"%u",&node_block_size);

    fscanf(fp,"%s",tmp);
    fscanf(fp,"%u",&file_block_size);

    fscanf(fp,"%s",tmp);
    fscanf(fp,"%s",folder_name);

    fscanf(fp,"%s",tmp);
    fscanf(fp,"%u",&max_node_block_number);
    chdir(folder_name);

    fclose(fp);
}

/*
将节点数据块写入指定的文件：行号位置
*/

void write_node_block(unsigned char data[],int file_num,int line_num){
    char filename[32];
    memset(filename,0,sizeof filename);
    int index = 0;
    if (file_num == 0) filename[index++] = '0';
    while (file_num){
        filename[index++] = file_num%10+'0';
        file_num/=10;
    }
    strcat(filename,".ndb");
    //printf("%s\n",filename);
    char tmp;
    for (int i = 0;i*2<index; i++){
        tmp = filename[i];
        filename[i] = filename[index-i-1];
        filename[index-i-1] = tmp;
    }

    int fd = open(filename,O_RDWR ,S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |S_IROTH);
    if (fd <0) fd = open(filename,O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |S_IROTH);
    if (fd <0) {
        printf("write_node_block error, cannot create block\n");
        return ;
    }

    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = line_num*node_block_size;
    lock.l_len = node_block_size;

    fcntl(fd,F_SETLKW,&lock);

    lseek(fd,line_num*node_block_size,SEEK_SET);
    write(fd,data,node_block_size);

    lock.l_type = F_UNLCK;
    fcntl(fd,F_SETLKW,&lock);
    close(fd);
}

/*
读取指定的文件：行号位置的节点数据块
*/

void read_node_block(int file_num,int line_num,unsigned char result[]){
    int filenum = file_num;
    char filename[32];
    memset(filename,0,sizeof filename);
    int index = 0;
    if (file_num == 0) filename[index++] = '0';
    while (file_num){
        filename[index++] = file_num%10+'0';
        file_num/=10;
    }
    strcat(filename,".ndb");
    char tmp;
    for (int i = 0;i*2<index; i++){
        tmp = filename[i];
        filename[i] = filename[index-i-1];
        filename[index-i-1] = tmp;
    }

    int fd = open(filename,O_RDWR ,S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |S_IROTH);
    if (fd <0) {
        printf("read_node_block error, file not exists\n");
        return ;
    }

    struct flock lock;
    lock.l_type = F_RDLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = line_num*node_block_size;
    lock.l_len = node_block_size;

    fcntl(fd,F_SETLKW,&lock);

    lseek(fd,line_num*node_block_size,SEEK_SET);
    read(fd,result,node_block_size);

    lock.l_type = F_UNLCK;
    fcntl(fd,F_SETLKW,&lock);
    close(fd);
}

/*
获取指定ndb文件的数据块数量
*/

int node_lines_num(int file_num){
    int filenum = file_num;

    char filename[32];
    memset(filename,0,sizeof filename);
    int index = 0;
    if (file_num == 0) filename[index++] = '0';
    while (file_num){
        filename[index++] = file_num%10+'0';
        file_num/=10;
    }
    strcat(filename,".ndb");
    char tmp;
    for (int i = 0;i*2<index; i++){
        tmp = filename[i];
        filename[i] = filename[index-i-1];
        filename[index-i-1] = tmp;
    }

    int fd = open(filename,O_RDWR ,S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |S_IROTH);
    if (fd <0) {
        printf("read_node_block error, file not exists\n");
        return -1;
    }

    struct flock lock;
    lock.l_type = F_RDLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;

    fcntl(fd,F_SETLKW,&lock);

    int sz = lseek(fd,0,SEEK_END);

    lock.l_type = F_UNLCK;
    fcntl(fd,F_SETLKW,&lock);
    close(fd);
    return sz/node_block_size;
}

/*
将指定记录数据块写入文件：行号指定位置
*/
void write_file_block(unsigned char data[],int file_num,int line_num){
    char filename[32];
    memset(filename,0,sizeof filename);
    int index = 0;
    if (file_num == 0) filename[index++] = '0';
    while (file_num){
        filename[index++] = file_num%10+'0';
        file_num/=10;
    }
    strcat(filename,".fdb");
    char tmp;
    for (int i = 0;i*2<index; i++){
        tmp = filename[i];
        filename[i] = filename[index-i-1];
        filename[index-i-1] = tmp;
    }

    int fd = open(filename,O_RDWR ,S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |S_IROTH);
    if (fd <0) fd = open(filename,O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |S_IROTH);
    if (fd <0) {
        printf("write_file_block error, cannot create block\n");
        return ;
    }

    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = line_num*file_block_size;
    lock.l_len = file_block_size;

    fcntl(fd,F_SETLKW,&lock);

    lseek(fd,line_num*file_block_size,SEEK_SET);
    write(fd,data,file_block_size);

    lock.l_type = F_UNLCK;
    fcntl(fd,F_SETLKW,&lock);
    close(fd);
}

/*
读取文件：行号指定的位置的记录数据块
*/
void read_file_block(int file_num,int line_num,unsigned char result[]){
    int filenum = file_num;
    char filename[32];
    memset(filename,0,sizeof filename);
    int index = 0;
    if (file_num == 0) filename[index++] = '0';
    while (file_num){
        filename[index++] = file_num%10+'0';
        file_num/=10;
    }
    strcat(filename,".fdb");
    char tmp;
    for (int i = 0;i*2<index; i++){
        tmp = filename[i];
        filename[i] = filename[index-i-1];
        filename[index-i-1] = tmp;
    }

    int fd = open(filename,O_RDWR ,S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |S_IROTH);
    if (fd <0) {
        printf("read_file_block error, file not exists\n");
        return ;
    }

    struct flock lock;
    lock.l_type = F_RDLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = line_num*file_block_size;
    lock.l_len = file_block_size;

    fcntl(fd,F_SETLKW,&lock);

    lseek(fd,line_num*file_block_size,SEEK_SET);
    read(fd,result,file_block_size);

    lock.l_type = F_UNLCK;
    fcntl(fd,F_SETLKW,&lock);
    close(fd);
}

/*
获取指定fdb文件的数据块数量
*/
int file_lines_num(int file_num){
    int filenum = file_num;

    char filename[32];
    memset(filename,0,sizeof filename);
    int index = 0;
    if (file_num == 0) filename[index++] = '0';
    while (file_num){
        filename[index++] = file_num%10+'0';
        file_num/=10;
    }
    strcat(filename,".fdb");
    char tmp;
    for (int i = 0;i*2<index; i++){
        tmp = filename[i];
        filename[i] = filename[index-i-1];
        filename[index-i-1] = tmp;
    }

    int fd = open(filename,O_RDWR ,S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |S_IROTH);
    if (fd <0) {
        printf("read_file_block error, file not exists\n");
        return -1;
    }

    struct flock lock;
    lock.l_type = F_RDLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;

    fcntl(fd,F_SETLKW,&lock);

    int sz = lseek(fd,0,SEEK_END);

    lock.l_type = F_UNLCK;
    fcntl(fd,F_SETLKW,&lock);
    close(fd);
    return sz/file_block_size;
}

/*
从数据块中的一部分转换为数字(不超过4字节)
*/
unsigned trans_block_to_int(unsigned char block[],int start,int len){
    unsigned ret = 0;
    for (int i = start;i<start+len;i++)
        ret = (ret<<8)+block[i];
    return ret;
}

/*
将数据块中的一部分信息提取为字符串
*/
void trans_block_to_char_array(unsigned char block[],int start,int len,unsigned char ret[]){
    memcpy(ret,block+start,len);
}

/*
将数字植入数据块中（必须是4字节）
*/
void put_int_to_block(unsigned char block[],int start,unsigned num){
    for (int i = start+3;i>=start;i--){
        block[i] = num & 255;
        num = num>>8;
    }
}

/*
将字符串植入数据块中
*/
void put_char_to_block(unsigned char block[],int block_start,int data_start,int len,unsigned char data[]){
    memcpy(block+block_start,data+data_start,len);
}
