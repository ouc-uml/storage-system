#include "file_operation.h"
#include <sys/file.h>

/*
清空节点数据库内所有存储
*/
void clear_all_nodes(){
    char buffer[20];
    memset(buffer,0,sizeof buffer);
    FILE *fp = fopen("node_pointer.dat","w");
    flock(fileno(fp),LOCK_EX);
    fwrite(buffer,sizeof(char),12,fp);
    flock(fileno(fp),LOCK_UN);
    fclose(fp);

    fp = fopen("node_trash.dat","w");
    fclose(fp);
}

/*
清空记录数据库内的所有存储
*/
void clear_all_records(){
    char buffer[20];
    memset(buffer,0,sizeof buffer);
    FILE *fp = fopen("record_pointer.dat","w");
    flock(fileno(fp),LOCK_EX);
    fwrite(buffer,sizeof(char),12,fp);
    flock(fileno(fp),LOCK_UN);
    fclose(fp);

    fp = fopen("record_trash.dat","w");
    fclose(fp);
}

/*
分配一个新的节点数据块的位置
*/
void new_node_position(unsigned *filenum,unsigned *linenum){
    int fdd = open("node_pointer.dat",O_RDWR ,S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |S_IROTH);
    if (fdd <0) {
        printf("node_pointer.dat not exists\n");
        return ;
    }

    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;

    fcntl(fdd,F_SETLKW,&lock);

    unsigned char buffer[20];
    read(fdd,buffer,12);
    *filenum = trans_block_to_int(buffer,0,4);
    *linenum = trans_block_to_int(buffer,4,4);

    unsigned trash_size = trans_block_to_int(buffer,8,4);

    if (trash_size == 0){
        unsigned new_filenum = *filenum;
        unsigned new_linenum = *linenum+1;

        if (new_linenum >= max_node_block_number){
            new_linenum = 0;
            new_filenum++;
        }

        put_int_to_block(buffer,0,new_filenum);
        put_int_to_block(buffer,4,new_linenum);

        lseek(fdd,0,SEEK_SET);
        write(fdd,buffer,12);

        lock.l_type = F_ULOCK;
        fcntl(fdd,F_SETLKW,&lock);
        close(fdd);
        return ;
    }

    int fd = open("node_trash.dat",O_RDWR ,S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |S_IROTH);
    if (fd <0) {
        printf("node_trash.dat not exists\n");
        return ;
    }

    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    fcntl(fd,F_SETLKW,&lock);

    lseek(fd,8*(trash_size-1),SEEK_SET);

    unsigned char trash_buffer[12];
    read(fd,trash_buffer,8);

    *filenum = trans_block_to_int(trash_buffer,0,4);
    *linenum = trans_block_to_int(trash_buffer,4,4);

    lock.l_type = F_UNLCK;
    fcntl(fd,F_SETLKW,&lock);
    close(fd);

    trash_size--;
    put_int_to_block(buffer,8,trash_size);

    lseek(fdd,0,SEEK_SET);
    write(fdd,buffer,12);

    lock.l_type = F_UNLCK;
    fcntl(fdd,F_SETLKW,&lock);
    close(fdd);
}

/*
删除一个节点数据块的位置
*/
void delete_node(int filenum,int linenum){
    int fdd = open("node_pointer.dat",O_RDWR ,S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |S_IROTH);
    if (fdd <0) {
        printf("node_pointer.dat not exists\n");
        return ;
    }

    struct flock lock,lock1;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;

    fcntl(fdd,F_SETLKW,&lock);

    unsigned char buffer[20];
    read(fdd,buffer,12);
    unsigned trash_size = trans_block_to_int(buffer,8,4);
    put_int_to_block(buffer,8,trash_size+1);

    lseek(fdd,0,SEEK_SET);
    write(fdd,buffer,12);

    int fd = open("node_trash.dat",O_RDWR ,S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |S_IROTH);
    if (fd <0) {
        printf("node_trash.dat not exists\n");
        return ;
    }

    put_int_to_block(buffer,0,filenum);
    put_int_to_block(buffer,4,linenum);

    lock1.l_type = F_WRLCK;
    lock1.l_whence = SEEK_SET;
    lock1.l_start = 0;
    lock1.l_len = 0;

    fcntl(fd,F_SETLKW,&lock1);
    lseek(fd,8*trash_size,SEEK_SET);
    write(fd,buffer,8);

    lock1.l_type = F_UNLCK;
    fcntl(fd,F_SETLKW,&lock1);
    close(fd);
    lock.l_type = F_UNLCK;
    fcntl(fdd,F_SETLKW,&lock);
    close(fdd);
}

/*
分配新的记录数据块位置
*/
void new_record_position(unsigned *filenum,unsigned *linenum){
    int fdd = open("record_pointer.dat",O_RDWR ,S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |S_IROTH);
    if (fdd <0) {
        printf("record_pointer.dat not exists\n");
        return ;
    }

    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;

    fcntl(fdd,F_SETLKW,&lock);

    unsigned char buffer[20];
    read(fdd,buffer,12);
    *filenum = trans_block_to_int(buffer,0,4);
    *linenum = trans_block_to_int(buffer,4,4);

    unsigned trash_size = trans_block_to_int(buffer,8,4);

    if (trash_size == 0){
        unsigned new_filenum = *filenum;
        unsigned new_linenum = *linenum+1;

        if (new_linenum >= max_node_block_number){
            new_linenum = 0;
            new_filenum++;
        }

        put_int_to_block(buffer,0,new_filenum);
        put_int_to_block(buffer,4,new_linenum);

        lseek(fdd,0,SEEK_SET);
        write(fdd,buffer,12);

        lock.l_type = F_ULOCK;
        fcntl(fdd,F_SETLKW,&lock);
        close(fdd);
        return ;
    }

    int fd = open("record_trash.dat",O_RDWR ,S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |S_IROTH);
    if (fd <0) {
        printf("record_trash.dat not exists\n");
        return ;
    }

    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    fcntl(fd,F_SETLKW,&lock);

    lseek(fd,8*(trash_size-1),SEEK_SET);

    unsigned char trash_buffer[12];
    read(fd,trash_buffer,8);

    *filenum = trans_block_to_int(trash_buffer,0,4);
    *linenum = trans_block_to_int(trash_buffer,4,4);

    lock.l_type = F_UNLCK;
    fcntl(fd,F_SETLKW,&lock);
    close(fd);

    trash_size--;
    put_int_to_block(buffer,8,trash_size);

    lseek(fdd,0,SEEK_SET);
    write(fdd,buffer,12);

    lock.l_type = F_UNLCK;
    fcntl(fdd,F_SETLKW,&lock);
    close(fdd);
}

/*
删除一条给定位置的记录数据块
*/
void delete_record(int filenum,int linenum){
    int fdd = open("record_pointer.dat",O_RDWR ,S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |S_IROTH);
    if (fdd <0) {
        printf("record_pointer.dat not exists\n");
        return ;
    }

    struct flock lock,lock1;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;

    fcntl(fdd,F_SETLKW,&lock);

    unsigned char buffer[20];
    read(fdd,buffer,12);
    unsigned trash_size = trans_block_to_int(buffer,8,4);
    put_int_to_block(buffer,8,trash_size+1);

    lseek(fdd,0,SEEK_SET);
    write(fdd,buffer,12);

    int fd = open("record_trash.dat",O_RDWR ,S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |S_IROTH);
    if (fd <0) {
        printf("record_trash.dat not exists\n");
        return ;
    }

    put_int_to_block(buffer,0,filenum);
    put_int_to_block(buffer,4,linenum);

    lock1.l_type = F_WRLCK;
    lock1.l_whence = SEEK_SET;
    lock1.l_start = 0;
    lock1.l_len = 0;

    fcntl(fd,F_SETLKW,&lock1);
    lseek(fd,8*trash_size,SEEK_SET);
    write(fd,buffer,8);

    lock1.l_type = F_UNLCK;
    fcntl(fd,F_SETLKW,&lock1);
    close(fd);
    lock.l_type = F_UNLCK;
    fcntl(fdd,F_SETLKW,&lock);
    close(fdd);
}
