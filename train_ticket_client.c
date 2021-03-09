/*《Uni/Linux环境高级编程》课程作业
   1.实现火车售票系统，火车车次、价格、余票、中途站点、票价等信息以文件的形式存储
   2.服务端可以实现车次增加、价格调整
   3.客户端实现车次查询、余票数目查询及购买操作
   4.客户端与服务器端以多进程方式实现，即服务端应是一个进程多线程；客户端是多个进程；客户端程序可以执行多次从而产生多个进程；
   5.每来一个客户端请求，服务端应产生一个线程处理该请求
   6.对于购票操作应实现线程间的同步；	
   7.服务端应能以格式化文件与终端形式输出余票信息、客户端查询日志等信息；该功能以多进程的方式进行实现，而不应阻塞客户端的请求。
此程序是客户端*/

#include <sys/types.h>
#include <apue.h>

#define FIFO_READ "readfifo"
#define FIFO_WRITE "writefifo"
#define LEN 30

void Purchase_ticket();
void Finding_train_number();
int wfd, rfd;

//购买车票
void Purchase_ticket()
{
   int len;
   char writebuf[LEN], readbuf[LEN];
   do
   {
       printf("\n\t请输入您想购买的火车车次:");
       memset(writebuf,0,LEN);
       fgets(writebuf, LEN, stdin);
       writebuf[strlen(writebuf)-1] = '\0';
       write(wfd, writebuf, strlen(writebuf));

       memset(readbuf,0,LEN);
       read(rfd, readbuf, LEN);
       readbuf[1] = '\0';
       if(strcmp(readbuf,"n") == 0)
       {
          printf("\t该火车车次不存在！\n");
          printf("\t是否继续购买(y/n): ");
          memset(writebuf,0,LEN);
          fgets(writebuf, LEN, stdin);
          writebuf[strlen(writebuf)-1] = '\0';
          write(wfd, writebuf, strlen(writebuf));
	  if(strcmp(writebuf,"n") == 0) 
          {
             goto next;
             printf("\n\n");
          }
          continue;      //继续进行条件判断
       }

       memset(readbuf,0,LEN);
       len = read(rfd, readbuf, LEN);
       readbuf[len] = '\0';
       if(strcmp(readbuf,"n") == 0)
       {
          printf("\t该车次余票不足！\n\n");   
          goto next;
       }
       else
       {
          printf("\t购买成功！\n\n");
       }

  next:
       printf("1、查询车次\n2、购买车票\n3、退出程序\n请输入您想进行的操作：");
       memset(writebuf,0,LEN);
       fgets(writebuf, LEN, stdin);
       writebuf[strlen(writebuf)-1] = '\0';
       write(wfd, writebuf, strlen(writebuf));

       if(strcmp(writebuf,"1") == 0)
          Finding_train_number();
       else if(strcmp(writebuf,"2") == 0)
          continue;
       else if(strcmp(writebuf,"3") == 0)
       {
          close(wfd);
          close(rfd);
          exit(0);
       }
       else
       {
          printf("请输入正确指令！\n\n");
          goto next;
       }
   }while(1);
}

//查询车次
void Finding_train_number()
{
    int len;
    char writebuf[LEN], readbuf[LEN];
    do
    {
        printf("\n\t请输入您想查询的火车车次:");
        memset(writebuf,0,LEN);
        fgets(writebuf, LEN, stdin);
        writebuf[strlen(writebuf)-1] = '\0';
        write(wfd, writebuf, strlen(writebuf));

        memset(readbuf,0,LEN);
        read(rfd, readbuf, LEN);
        readbuf[1] = '\0';

        if(strcmp(readbuf,"y") != 0)
        {
           printf("\t该火车车次不存在！\n");
	   printf("\t是否继续查询(y/n): ");
           memset(writebuf,0,LEN);
           fgets(writebuf, LEN, stdin);
           writebuf[strlen(writebuf)-1] = '\0';
           write(wfd, writebuf, strlen(writebuf));
	   if(strcmp(writebuf,"n") == 0)
           {
              printf("\n");
              break;
           }
           continue;      //继续进行条件判断
        }
        printf("\t车次 起始站 终点站 票价 余票\n");
        memset(readbuf,0,LEN);
        len = read(rfd, readbuf, LEN);
        readbuf[len] = '\0';
        printf("\t %s\n\n",readbuf);

 upside:
        printf("1、继续查询\n2、购买车票\n3、退出程序\n请输入您想进行的操作：");
        memset(writebuf,0,LEN);
        fgets(writebuf, LEN, stdin);
        writebuf[strlen(writebuf)-1] = '\0';
        write(wfd, writebuf, strlen(writebuf));
        if(strcmp(writebuf,"1") == 0)
           continue;
        else if(strcmp(writebuf,"2") == 0)
           Purchase_ticket();
        else if(strcmp(writebuf,"3") == 0)
        {
           close(wfd);
           close(rfd);
           exit(0);
        }
        else
        {
           printf("请输入正确的指令！\n\n");
           goto upside;
        }
    }while(1);
}

//主函数部分
int main(void)
{
   printf("欢迎进入朱和章的火车售票系统之客户端\n");
   mkfifo(FIFO_WRITE, 0666);
   wfd = open(FIFO_WRITE, O_WRONLY);
   rfd = open(FIFO_READ, O_RDONLY);
   char writebuf[LEN];

   while(1)
   {
       printf("1、查询车次\n2、购买车票\n3、退出程序\n请输入您想进行的操作：");
       memset(writebuf,0,LEN);
       fgets(writebuf, LEN, stdin);
       writebuf[strlen(writebuf)-1] = '\0';
       write(wfd, writebuf, strlen(writebuf));
       if(strcmp(writebuf,"1") == 0)
          Finding_train_number();
       else if(strcmp(writebuf,"2") == 0)
          Purchase_ticket();
       else if(strcmp(writebuf,"3") == 0)
       {
          close(wfd);
          close(rfd);
          return 0;
       }
       else
       {
          printf("\n请输入正确的指令！\n");
          continue;
       }
   }
}
