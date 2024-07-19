# learn-network-program
env: wsl Ubuntu-Preview

gcc: 13.2.0

CXX: g++-13.2.0 and clang++-17

## 12/15/23  
锁在read(sock, buffer, size)似乎不起作用，具体表现就是之前在要接收文件的时候在发消息的线程临时调用一下read然后通过buffer对文件进行写入时，同时告诉我recive message:......  最后让服务端发消息过去然后在接收消息线程写文件，这样也可以先确认服务端已经收到file消息。  另外编译器一直提醒在quit前对stop进行写操作存在data race，不是很理解， 另外一个线程晚点退出就晚点吧， 也许除了线程外公共变量还有别的更安全的线程间通信方式？

## 1/16/24
主要写了个client， 把原来一团乱七八糟塞在main函数里面的东西拉出来做了几个类，方便以后功能扩展。 client.hpp里面可以不断添加任务， 然后需要执行这个任务的时候就submit到taskpool或者threadpool， 如果是比较重要的任务就让主线程池执行， 别的要么等到callback的时候再说，要么全部塞给一个线程慢慢解决。为了区分自己发送的消息和serve传来的消息找了个简单的ui库， 其实应该换成支持类似于lable元素的， 现在这个很别扭。 loger.hpp是写着好玩的给自己加了个namespace， 以后换成spdlog学习学习。

/5里面的内容才刚起步， 没什么好说的
