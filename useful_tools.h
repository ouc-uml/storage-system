#include <stdarg.h>
#include <stdio.h>
#define _USEFUL_TOOLS_H_ 0

void uprint(unsigned char ret[],int maxlen, const char control[], ...){
    memset(ret,0,maxlen);
    va_list ap;
    va_start(ap,control);

    char buffer[32];
    int len = strlen(control),start_point = 0;
    for (int i = 0;i<len;i++){
        if (control[i] == 'd') {
            int tmp = va_arg(ap, int);
            sprintf(buffer,"%d",tmp);
            int len1 = strlen(buffer);
            memcpy(ret+start_point,buffer,len1);
            start_point += len1;
        }
        else if (control[i] == 'u'){
            int tmp = va_arg(ap, unsigned);
            sprintf(buffer,"%u",tmp);
            int len1 = strlen(buffer);
            memcpy(ret+start_point,buffer,len1);
            start_point += len1;
        }
        else if (control[i] == 's'){
            const char* tmp = va_arg(ap, const char*);
            int len1 = strlen(tmp);
            memcpy(ret+start_point,tmp,len1);
            start_point += len1;
        }
        else if (control[i] == 'a'){
            const unsigned char* tmp = va_arg(ap, const unsigned char*);
            int len1 = va_arg(ap,unsigned);
            memcpy(ret+start_point,tmp,len1);
            start_point += len1;
        }
        else if (control[i] == 'c'){
            char tmp = va_arg(ap, int);
            ret[start_point] = tmp;
            start_point ++;
        }
        else if (control[i] == 'x'){
            unsigned tmp = va_arg(ap,unsigned);
            put_int_to_block(ret,start_point,tmp);
            start_point += 4;
        }
    }
    ret[start_point] = 0;
    va_end(ap);
}

void uscan(unsigned char data[],const char control[], ...){
    va_list ap;
    va_start(ap,control);
    int len = strlen(control);
    unsigned start_point = 0;
    for (int i = 0;i<len;i++){
        if (control[i] == 'x'){
            unsigned *tmp = va_arg(ap,unsigned *);
            *tmp = trans_block_to_int(data,start_point,4);
            start_point+=4;
        }
        else if (control[i] == 's'){
            char * tmp = va_arg(ap,char*);
            unsigned len = va_arg(ap,unsigned);
            memcpy(tmp,data+start_point,len);
            tmp[len] = 0;
            start_point+=len;
        }
        else if (control[i] == 'a'){
            unsigned char * tmp = va_arg(ap,unsigned char *);
            unsigned len = va_arg(ap,unsigned);
            memcpy(tmp,data+start_point,len);
            start_point+=len;
        }
    }
    va_end(ap);
}
