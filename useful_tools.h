#include <stdarg.h>
#include <stdio.h>
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
    }
    ret[start_point] = 0;
    va_end(ap);
}
