#include "../kernel/types.h"
#include "user.h"
#include "stddef.h"

// 父进程写入管道数据，子进程读出管道数据，输出第一个数，
// 对数据是不是素数进行判断

void child(int fl[]);

int main(){
  int fd[2],i;
  pipe(fd);
  int pid = fork();
  if(pid == 0){
    child(fd);
  }else{
    close(fd[0]);
    for(i=2;i<36;i++){
      write(fd[1], &i, sizeof(int));
    }
    close(fd[1]); 
    wait(NULL);
  }
  exit(0);
}

void child(int fl[]){
  int fr[2], n;
  close(fl[1]);
  int num = read(fl[0], &n, sizeof(int));
  if(num == 0)
    exit(0);
  printf("prime %d\n", n);
  pipe(fr);
  if(fork() == 0){
    child(fr);
  }else{
    close(fr[0]);
    int prime = n;
    while(read(fl[0], &n, sizeof(int))!=0){
      if(n%prime!=0){
        write(fr[1], &n, sizeof(int));
      }
    }
    close(fr[1]);
    wait(NULL);
    exit(0);
  }
}




