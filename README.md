﻿# 1.作业题目
1、实现火车售票系统，火车车次、价格、余票、中途站点、票价等信息以文件的形式存储；
    2、服务端可以实现车次增加、价格调整；
    3、客户端实现车次查询、余票数目查询及购买操作；
    4、客户端与服务器端以多进程方式实现，即服务端应是一个进程多线程；客户端是多个进程；客户端程序可以执行多次从而产生多个进程；
    5、每来一个客户端请求，服务端应产生一个线程处理该请求；
    6、对于购票操作应实现线程间的同步；	
    7、服务端应能以格式化文件与终端形式输出余票信息、客户端查询日志等信息；该功能以多进程的方式进行实现，而不应阻塞客户端的请求。
# 2.总体设计
利用结构体数组临时存储火车信息并保存到文件上，其中结构体包含的信息有火车车次、起点站、终点站、票价以及余票数目。对存储火车车次信息的文件放置在服务端，客户端只能通过两者之间的有名管道对文件间接读写操作，这其中利用两条有名管道进行全双工操作。服务端对文件的读写首先会将文件信息读到结构体上，并且服务端每次对文件信息的修改会实时更新保存至文件。而客户端只能通过管道向服务端仅仅读取所要查询的信息，管道传回的也只有这些信息。
客户端的执行逻辑是和服务端相对应的部分执行逻辑是一样的，只不过是读写操作相反过来罢了，也只有这样才可以实现两的操作同步。服务端首页是可进行火车信息的修改、车次增加以及开启服务，也即是开启接受来自客户端的信息，当然也可以退出，服务端可以记录显示出来客户端的操作。客户端打开后通过有名管道和服务端互相通信，进行查询车票以及购买的操作。
# 3.详细设计
首先说一下服务端：声明相关的头文件，由于买车票的函数Purchase_ticket()和查找车次的函数Finding_train_number();互相调用，所以为了避免出现未定义，提前声明它们。接着定义存储结构体数组，存储火车车次信息，包括火车车次、起始站、终点站、票价以及余票，还有一个保存车次总数的变量。从主函数开始说起，进去便是主函数菜单，有增加车次、价格修改、显示余票信息、以及开始服务接受来自客户端的请求的功能，当然还有退出程序，输入相应的选项进入相应额界面。
若输入1进入增加车次Adding_train_number()：首先从文件读入信息，然后将代码放置在一个do-while循环里面，输入要增加的火车车次，然后会利用for循环以及strcmp逐个查找是否有相同的车次，若有则提示用户并询问是否继续输入若否则break出循环否则continue回到循环开始；若车次还未存在，则依次输入车次信息然后调用写函数写入文件即可，最后再返回主函数的菜单。
若输入2则进入价格调整Changing_price()：同样先读入数据并将程序放在一个do-while循环里。输入要调整价格的车次，若没有则提示并且询问是否继续输入，否则输入修改后的价格并且写入文件，逻辑结构和上一个函数基本一致。
若输入3则显示车次信息Show_data()：首先读入信息，再利用for循环逐个打印出来车次信息即可。
若输入4则进入开启服务：由于这一部分是服务端和客户端所共有的代码，所以采取两者结合的方式来说明。开启服务后服务端创建写的管道，相当于客户端的读管道。利用open分别打开两个管道，对应读和写。当然服务端会阻塞在第一个open直到客户端以写的方式打开。接下来是while循环，首先会接收来自客户端的指令。
若客户端选择1则进入查询车票Finding_train_number()：由于进入车票后至少购买一次，所以利用do-while循环，每次循环开始为了数据实时更新都会重新调用函数从文件读入信息，客户端输入要查询的车次，服务端开始逐个查询，若不存在则返回标志给客户端，否则利用strcat函数将所有信息放入writebuf通过管道传回客户端显示出来。接下来询问1继续查询、2购买车次、3退出程序，否则goto重新输入指令。
若客户端选择2则进入购买车票：逻辑结构同上基本一样，这里就不再赘述。
若客户端选择3则退出服务，否则重新输入正确的指令。
最后再说明一下写数据到文件的函数Write_to_file()：首先定义文件句柄，以写的方式打开文件并设置完全缓存模式利用fprintf分别将车次总数以及各个车次信息写入文件，最后再关闭文件即可。还有一个从文件读出数据函数Read_from_file()，类似于写入文件，这里就不再说明。
# 4.问题分析
由于个人能力，没法实现服务端的多线程，但是客户端的多进程是可以的。但是也尝试过，想法大概就是所有客户端进程都知道服务端的的一个通用管道。通过这个管道，由于进程号在一个主机上是唯一的，所以所有客户端可以传达自己的进程号给服务端用来创建两个只属于它们的之间管道。至于服务端，可以在一开始就创建一个线程运行，用来实时监测通用管道是否有信息，有的话就读出并创建打开两个管道，没有的话则一直阻塞。
想法虽好，但是这样的话它们之间的管道文件描述符应该如何保存，若定义全局变量则它们之间会产生冲突，若定义局部变量，由于程序是有多个函数的，所以局部变量的文件描述符又无法被识别，所以这样也不行，除非改变现有的逻辑结构。

