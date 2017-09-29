#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <unistd.h>

char message_queue[32];
int node_block_size;
int file_block_size;
char folder_name[64];

void load_configure(){
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
    fseek(fp,line_num*node_block_size,SEEK_SET);
    fwrite(data,sizeof(char), node_block_size,fp);
    fclose(fp);
}
void read_node_block(int file_num,int line_num,char result[]){
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
    fseek(fp,line_num*node_block_size,SEEK_SET);
    fread(result,sizeof(char), node_block_size,fp);
    fclose(fp);
}
int node_lines(int file_num){
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
    fseek(fp, 0L, SEEK_END);
    int sz = ftell(fp);
    fclose(fp);
    return sz/node_block_size;
}
int main(){
    load_configure();

    printf("%d\n",node_lines(109));
    return 0;
}
