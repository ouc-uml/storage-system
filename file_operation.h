#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <unistd.h>

char message_queue[32];
int node_block_size;
int file_block_size;
char folder_name[64];

/*
加载文件系统的配置信息，在所有的操作进行之前进行
*/
void load_configuration(){
    char tmp[32];
    FILE *fp = fopen("global.conf","r");

    fscanf(fp,"%s",tmp);
    fscanf(fp,"%s",message_queue);

    fscanf(fp,"%s",tmp);
    fscanf(fp,"%d",&node_block_size);

    fscanf(fp,"%s",tmp);
    fscanf(fp,"%d",&file_block_size);

    fscanf(fp,"%s",tmp);
    fscanf(fp,"%s",folder_name);
    chdir(folder_name);

    fclose(fp);
}

/*
将节点数据块写入指定的文件：行号位置
*/

void write_node_block(char data[],int file_num,int line_num){
    char filename[32];
    memset(filename,0,sizeof filename);
    int index = 0;
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

    FILE *fp = fopen(filename, "r+");
    if (fp == NULL) fp = fopen(filename,"w+");
    fseek(fp,line_num*node_block_size,SEEK_SET);
    fwrite(data,sizeof(char), node_block_size,fp);
    fclose(fp);
}

/*
读取指定的文件：行号位置的节点数据块
*/

void read_node_block(int file_num,int line_num,char result[]){
    int filenum = file_num;
    char filename[32];
    memset(filename,0,sizeof filename);
    int index = 0;
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

    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        printf("read_node_block error, %d.ndb not exists\n",filenum);
        return;
    }
    fseek(fp,line_num*node_block_size,SEEK_SET);
    fread(result,sizeof(char), node_block_size,fp);
    fclose(fp);
}

/*
获取指定ndb文件的数据块数量
*/

int node_lines_num(int file_num){
    int filenum = file_num;

    char filename[32];
    memset(filename,0,sizeof filename);
    int index = 0;
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

    FILE *fp = fopen(filename,"r");
    if (fp == NULL) {
        printf("node_lines_num error, %d.ndb not exists\n",filenum);
        return -1;
    }
    fseek(fp, 0L, SEEK_END);
    int sz = ftell(fp);
    fclose(fp);
    return sz/node_block_size;
}

/*
将指定记录数据块写入文件：行号指定位置
*/
void write_file_block(char data[],int file_num,int line_num){
    char filename[32];
    memset(filename,0,sizeof filename);
    int index = 0;
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

    FILE *fp = fopen(filename, "r+");
    if (fp == NULL) {
        fp = fopen(filename,"w+");
    }

    fseek(fp,line_num*file_block_size,SEEK_SET);
    fwrite(data,sizeof(char), file_block_size,fp);
    fclose(fp);
}

/*
读取文件：行号指定的位置的记录数据块
*/
void read_file_block(int file_num,int line_num,char result[]){
    int filenum = file_num;
    char filename[32];
    memset(filename,0,sizeof filename);
    int index = 0;
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

    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        printf("read_file_block error, %d not exists\n",filenum);
        return;
    }
    fseek(fp,line_num*file_block_size,SEEK_SET);
    fread(result,sizeof(char), file_block_size,fp);
    fclose(fp);
}

/*
获取指定fdb文件的数据块数量
*/
int file_lines_num(int file_num){
    int filenum = file_num;

    char filename[32];
    memset(filename,0,sizeof filename);
    int index = 0;
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

    FILE *fp = fopen(filename,"r");
    if (fp == NULL) {
        printf("file_lines_num error, %d not exists\n",filenum);
        return -1;
    }
    fseek(fp, 0L, SEEK_END);
    int sz = ftell(fp);
    fclose(fp);
    return sz/file_block_size;
}

/*
从数据块中的一部分转换为数字(不超过4字节)
*/
unsigned trans_block_to_int(char block[],int start,int len){
    unsigned ret = 0;
    for (int i = start;i<start+len;i++)
        ret = (ret<<8)+block[i];
    return ret;
}

/*
将数据块中的一部分信息提取为字符串
*/
void trans_block_to_char_array(char block[],int start,int len,char ret[]){
    strncpy(ret,block+start,len);
}

/*
将数字植入数据块中（必须是4字节）
*/
void put_int_to_block(char block[],int start,unsigned num){
    for (int i = start+3;i>=start;i--){
        block[i] = num & 255;
        num = num>>8;
    }
}