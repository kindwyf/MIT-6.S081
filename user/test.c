#include "../kernel/types.h"
#include "user.h"
#include "../kernel/param.h"

#define MAXN 2

int main(int argc, char *argv[]){
  int n;
  char buf[MAXARG]; // 存放管道中的数据
  while((n=read(0, buf, MAXN))>0){
    
    for(unsigned int i=0; i<strlen(buf);i++){
      printf("%c", buf[i]);
    }
    printf("\n");
  }
  exit(0);
}