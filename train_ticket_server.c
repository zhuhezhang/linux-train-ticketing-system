/*《Uni/Linux环境高级编程》课程作业
   1.实现火车售票系统，火车车次、价格、余票、中途站点、票价等信息以文件的形式存储
   2.服务端可以实现车次增加、价格调整
   3.客户端实现车次查询、余票数目查询及购买操作
   4.客户端与服务器端以多进程方式实现，即服务端应是一个进程多线程；客户端是多个进程；客户端程序可以执行多次从而产生多个进程；
   5.每来一个客户端请求，服务端应产生一个线程处理该请求
   6.对于购票操作应实现线程间的同步；	
   7.服务端应能以格式化文件与终端形式输出余票信息、客户端查询日志等信息；该功能以多进程的方式进行实现，而不应阻塞客户端的请求。
此程序是服务端*/

#include <sys/types.h>
#include <apue.h>

#define FIFO_WRITE "readfifo"
#define FIFO_READ "writefifo"
#define LEN 30

void Purchase_ticket();
void Finding_train_number();
int wfd, rfd;

struct train
{
   char train_number[10];    //火车车次
   char starting_station[10];//起始站
   char ending_station[10];  //终点站
   int price;                //票价
   int remaining_tickets;    //余票
}train[100];

int total_train_numbers;     //火车车次总数

//写入数据到文件
void Write_to_file()
{
   FILE *fp;
   int i = 0;
   if((fp = fopen("train_number_info.txt","w")) == NULL)//以写的方式打开文件
      err_sys("文件打开失败");
   setvbuf(fp,NULL,_IOFBF,BUFSIZ);                      //设置完全缓存模式
   fprintf(fp, "%d\n", total_train_numbers);
   while(i < total_train_numbers)
   {
      fprintf(fp,"%s\n%s\n%s\n%d\n%d\n\n",train[i].train_number,train[i].starting_station,train[i].ending_station,train[i].price,train[i].remaining_tickets);
      i++;
   }
   fclose(fp);
}

//从文件读出数据
void Read_from_file()
{
   FILE *fp;
   int i = 0;
   if((fp = fopen("train_number_info.txt","r")) == NULL)//以读的方式打开文件
      err_sys("文件打开失败");
   setvbuf(fp,NULL,_IOFBF,BUFSIZ);                      //设置完全缓存模式
   fscanf(fp,"%d\n",&total_train_numbers);
   while(i < total_train_numbers)
   {
      fscanf(fp,"%s\n%s\n%s\n%d\n%d\n\n",train[i].train_number,train[i].starting_station,train[i].ending_station,&train[i].price,&train[i].remaining_tickets);
      i++;
   }
   fclose(fp);
}

//车次增加
void Adding_train_number()
{
    int i;
    int flag;//标记火车车次是否已存在
    char c;  //指令Y?N
    Read_from_file();//输入数据前先将文件中原有的数据读入
    do
    {
        flag = 0;
        printf("\n\t请输入火车车次:");
        scanf("%s",train[total_train_numbers].train_number);
        for(i = 0; i < total_train_numbers; i++)
        {
            if(strcmp(train[i].train_number,train[total_train_numbers].train_number) == 0)
            {
                flag = 1;
                break;
            }
        }
        if(flag)
        {
            printf("\t该火车车次已存在！\n");
            printf("\t是否继续输入(y/n): ");
            scanf("%*c%c",&c);//%*c作用是读取输入流中数字后的一个字符，并丢弃，使得后面的输入函数不能读到那个字符跳过一个字符
            if(c == 'N' || c == 'n') 
                break;
            continue;      //继续进行条件判断
        }
        printf("\t请输入起始站:");
        scanf("%s", train[total_train_numbers].starting_station);
        printf("\t请输入终点站:");
        scanf("%s", train[total_train_numbers].ending_station);
        printf("\t请输入票价:");
        scanf("%d", &train[total_train_numbers].price);
        printf("\t请输入余票数:");
        scanf("%d", &train[total_train_numbers].remaining_tickets);
        printf("\t是否继续输入(y/n):");
        scanf("%*c%c",&c);//%*c作用是读取输入流中数字后的一个字符，并丢弃，使得后面的输入函数不能读到那个字符跳过一个字符
        total_train_numbers++;
    }while( c != 'N' && c != 'n');
    Write_to_file();             //调用该函数写入数据到文件中
    printf("\n\t数据已写入！\n\n");   
}

//价格调整
void Changing_price()
{
   char change_train_number[10], order;
   int position, change_price, flag;
   Read_from_file();
   while(1)
   {
      flag = 0;
      printf("\n\t请输入要调整价格的火车车次:");
      scanf("%s",change_train_number);
      for(position = 0;position < total_train_numbers;position++)
      {
         if(strcmp(train[position].train_number, change_train_number) == 0)
         {
            flag = 1;
            break;
         }
      }
      if(!flag)
      {
         printf("\t无此火车车次!");
         printf("\n\t是否继续修改(y/n)：");
         scanf("%*c%c",&order);
         if(order == 'N' || order == 'n')
            break;
         continue;
      }
      printf("\t请输入车次%s修改后的价格：", change_train_number);
      scanf("%d",&change_price);
      train[position].price = change_price;
      printf("\t价格修改成功，是否继续修改(y/n)：");
      scanf("%*c%c",&order);
      if(order == 'N' || order == 'n')
         break;
   }
   printf("\n");
   Write_to_file();
}

//显示车次信息
void Show_data()
{
   Read_from_file();
   printf("\n\t车次 起始站 终点站 票价 余票\n");
   if(total_train_numbers == 0)
      printf("              无\n");
   else
   {
      for(int i = 0;i < total_train_numbers;i++)
         printf("\t %s    %s     %s    %d  %d\n", train[i].train_number, train[i].starting_station, train[i].ending_station, train[i].price, train[i].remaining_tickets);
   }
   printf("\n");
}

//购买车票
void Purchase_ticket()
{
   int i, len;
   char writebuf[LEN], readbuf[LEN];
   do
   {
       Read_from_file();//输入数据前先将文件中原有的数据读入
       memset(readbuf,0,LEN);
       len = read(rfd, readbuf, LEN);
       readbuf[len] = '\0';
       printf("客户端请求购买的车次%s的车票\n",readbuf);
       memset(writebuf,0,LEN);
       strcpy(writebuf,"n");

       for(i = 0; i < total_train_numbers; i++)
       {
           if(strcmp(train[i].train_number,readbuf) == 0)
           {
               strcpy(writebuf,"y");
               break;
           }
       }

       write(wfd, writebuf, strlen(writebuf));

       if(strcmp(writebuf,"n") == 0)
       {
         memset(readbuf,0,LEN);
         len = read(rfd, readbuf, LEN);
         readbuf[len] = '\0';
	 if(strcmp(readbuf,"n") == 0) 
         {
            printf("客户端放弃购买\n");
            goto next;
         }
         printf("客户端请求重新购买\n");
         continue;      //继续进行条件判断
      }
      if(train[i].remaining_tickets <= 0)
      {
         write(wfd, "n", 1); 
         goto next;
      }
      else
      {
         write(wfd, "y", 1);
         train[i].remaining_tickets -= 1;
         Write_to_file();
      }

 next:
      memset(readbuf,0,LEN);
      len = read(rfd, readbuf, LEN);
      readbuf[len] = '\0';
      if(strcmp(readbuf,"1") == 0)
      {
         printf("客户端请求查询车次\n");
         Finding_train_number();
      }
      else if(strcmp(readbuf,"2") == 0)
      {
         printf("客户端请求继续购买\n");
         continue;
      }
      else if(strcmp(readbuf,"3") == 0)
      {
         printf("客户端请求退出\n");
         close(rfd);
         close(wfd);
         exit(0);
      }
      else
         goto next;
   }while(1);
}

//查询车次
void Finding_train_number()
{
    int i, len;
    char writebuf[LEN], readbuf[LEN];
    do
    {
        Read_from_file();//输入数据前先将文件中原有的数据读入
        memset(readbuf,0,LEN);
        len = read(rfd, readbuf, LEN);
        readbuf[len] = '\0';
        printf("客户端请求查询车次%s的信息\n",readbuf);

        memset(writebuf,0,LEN);
        strcpy(writebuf, "n");
        for(i = 0; i < total_train_numbers; i++)
        {
            if(strcmp(train[i].train_number,readbuf) == 0)
            {
                strcpy(writebuf,"y");
                break;
            }
        }
       
        writebuf[1] == '\0';
        write(wfd, writebuf, strlen(writebuf));

        if(strcmp(writebuf,"y") != 0)
        {
           memset(readbuf,0,LEN);
           len = read(rfd, readbuf, LEN);
           readbuf[len] = '\0';
	   if(strcmp(readbuf,"n") == 0) 
              break;
           printf("客户端请求重新查询\n");
           continue;//继续查询
        }

        memset(writebuf,0,LEN);//发送给客户端所查车次信息
        char price1[5], remaining_tickets1[5];
        sprintf(price1,"%d",train[i].price);
        sprintf(remaining_tickets1,"%d",train[i].remaining_tickets);
        strcpy(writebuf,train[i].train_number);
        strcat(writebuf,"    ");
        strcat(writebuf,train[i].starting_station);
        strcat(writebuf,"     ");
        strcat(writebuf,train[i].ending_station);
        strcat(writebuf,"    ");
        strcat(writebuf,price1);
        strcat(writebuf,"    ");
        strcat(writebuf,remaining_tickets1);
        writebuf[strlen(writebuf)] = '\0';
        write(wfd, writebuf, strlen(writebuf));

 upside:
        memset(readbuf,0,LEN);
        len = read(rfd, readbuf, LEN);
        readbuf[len] = '\0';
        if(strcmp(readbuf,"1") == 0)
        {
           printf("客户端请求继续查询车次\n");
           continue;
        }
        else if(strcmp(readbuf,"2") == 0)
        {
           printf("客户端请求购买车票\n");
           Purchase_ticket();
        }
        else if(strcmp(readbuf,"3") == 0)
        {
           printf("客户端请求退出\n");
           close(rfd);
           close(wfd);
           exit(0);
        }
        else
           goto upside;
           
    }while(1);
}

//开启服务
void Starting_service()
{
   int len;
   char readbuf[LEN];
   printf("\n正在接收来自客户端的信息...\n");
   mkfifo(FIFO_WRITE, 0666);
   rfd = open(FIFO_READ, O_RDONLY);
   wfd = open(FIFO_WRITE, O_WRONLY);

   while(1)
   {
      memset(readbuf,0,LEN);
      len = read(rfd, readbuf, LEN);
      readbuf[len] = '\0';
      if (len > 0)
      {
          if(strcmp(readbuf,"1") == 0)
          {
             printf("客户端请求查询车票\n");
             Finding_train_number();
          }
          else if(strcmp(readbuf,"2") == 0)
          {
             printf("客户端请求购买车票\n");
             Purchase_ticket();
          }
          else if(strcmp(readbuf,"3") == 0)
          {
             printf("客户端请求断开连接\n");
             close(rfd);
             close(wfd); 
             exit(0);
          }
          else
             continue;       
      }
   } 
}

//主函数部分
int main(void)
{
   char order[2];
   printf("欢迎进入朱和章的火车售票系统之服务端\n");
   while(1)
   {
      printf("1、增加车次\n2、价格修改\n3、显示信息\n4、开启服务\n5、退出程序\n输入您想进行的操作：");
      scanf("%s", order);
      if(strcmp(order,"1") == 0)
         Adding_train_number();
      else if(strcmp(order,"2") == 0)
         Changing_price();
      else if(strcmp(order,"3") == 0)
         Show_data();
      else if(strcmp(order,"4") == 0)
         Starting_service();
      else if(strcmp(order,"5") == 0)
         return 0;
      else
      {
         printf("请输入正确的指令！\n");
         continue;
      }
   }
}
