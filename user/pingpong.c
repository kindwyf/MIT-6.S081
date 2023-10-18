#include "../kernel/types.h"
#include "user.h"
#include "stddef.h"

int main(){
  int ping_fd[2]; // 父写子读
  int pong_fd[2]; // 父读子写
  // 上述为文件描述符，下面创建管道
  pipe(ping_fd);
  pipe(pong_fd);
  // [0]指管道的读取端，[1]指管道的写入端
  char buf[8]; // 设置缓冲区，读完放这
  int pid = fork();

  // 如果不用wait，那么就需要关闭管道某端
  if(pid == 0){
    // close(ping_fd[1]);
    // close(pong_fd[0]);
    read(ping_fd[0], buf, 4);
    printf("%d: recevied %s\n",getpid(), buf);
    write(pong_fd[1], "pong", strlen("pong"));
    // close(ping_fd[0]);
    // close(pong_fd[1]);
  }else{
    // close(ping_fd[0]);
    // close(pong_fd[1]);
    write(ping_fd[1], "ping", strlen("ping"));
    wait(NULL);
    read(pong_fd[0], buf, 4);
    printf("%d: recevied %s\n",getpid(), buf);
    // close(ping_fd[1]);
    // close(pong_fd[0]); 
  }
  exit(0);
}