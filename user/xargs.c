#include "../kernel/types.h"
#include "user.h"
#include "../kernel/param.h"
#include "../kernel/stat.h"

#define MAXN 100

int main(int argc, char *argv[]){
  // 如果参数个数小于 2
  if (argc < 2) {
      // 打印参数错误提示
      fprintf(2, "usage: xargs command\n");
      // 异常退出
      exit(1);
  }
  char* command = argv[1]; // 需要执行的命令
  char* paramers[MAXARG]; // 命令所带的参数
  int index = 0;
  for(int i=1; i<argc; i++){
    paramers[index++] = argv[i];
  }
  int n;
  char buf[MAXN]; // 存放管道中的数据
  while((n=read(0, buf, MAXN))>0){
    char temp[MAXN]; // 用来存放一个字符串
    paramers[index] = temp; // 追加该字符串到最终参数中
    for(unsigned int i=0; i<strlen(buf);i++){
      if(buf[i]=='\n'){
        if(fork()==0){
          exec(command, paramers);
          exit(0);
        }
        else
          wait(0);
      }
      else{
        temp[i] = buf[i];
      }
    }
  }
  exit(0);
}
