<img src="image/ninja.jpeg" alt="TurtleChat Logo" height="250">

-----------------
[![Build & Test](https://github.com/YukunJ/Turtle/actions/workflows/build_actions.yml/badge.svg?branch=main)](https://github.com/YukunJ/Turtle/actions/workflows/build_actions.yml)
<a href="https://github.com/YukunJ/Turtle/blob/main/LICENSE"><img src="https://badgen.net/github/license/YukunJ/Turtle?color=orange" alt="license"></a>
<a href="https://github.com/YukunJ/Turtle"><img src="https://img.shields.io/badge/Language-C++-red.svg"></a>
<a href="https://github.com/YukunJ/Turtle"><img src="https://badgen.net/badge/OS Support/Linux/cyan?list=1" alt="os"></a>
<a href="https://github.com/YukunJ/Turtle"><img src="https://badgen.net/badge/Database/MySQL/white?list=1" alt="os"></a>
<a href="https://github.com/YukunJ/Turtle/stargazers"><img src="https://badgen.net/github/stars/YukunJ/Turtle?color=yellow" alt="stars"></a>
<a href="https://github.com/YukunJ/Turtle/network/members"><img src="https://badgen.net/github/forks/YukunJ/Turtle?color=black" alt="forks"></a>
## TURTLE

[**English Version** 英文文档](./README.md)

**Turtle**是一个基于Linux的C++17轻量级网络库. 它将繁琐的套接字操作抽象为优雅可复用的类. 它支持自定义服务器的快速设立: 以回调函数的形式为每一个用户的TCP连接加入业务逻辑. 它同时支持HTTP协议的GET/HEAD请求与回复.

如果有任何问题, 欢迎提出issue, 提交pull request或者给我们发一封[邮件](mailto:yukunj.cs@gmail.com).

### 亮点概述

+ 采用非阻塞套接字和边缘触发(ET)的事件处理模式来容纳高并发.
+ 采纳[陈硕](https://github.com/chenshuo)先生的'one reactor per thread'的设计思路, 并以线程池进行管理.
+ 低耦合和可单独扩展使用的库设计.
+ 支持HTTP协议的GET/HEAD请求与回复.
+ 支持动态CGI协议请求与回复.
+ 支持可调参的缓存层来加快响应与减低服务器负载.
+ 支持与MySQL数据库交互.
+ 支持定时器功能来删除不活跃的用户连接.
+ 支持异步消费者-生产者模式的日志记录
+ 使用[Catch2](https://github.com/catchorg/Catch2)测试框架进行单元测试覆盖


### 系统架构

<img src="image/system_architecture_cn.png" alt="System Architecture New" height="450">

上图是**Turtle**系统架构的一个简单概括性图示.

1. 最基本的单位是一条**Connection**, 它内含了一个**Socket**和一个**Buffer**来分别处理套接字文件描述符与字节级数据的进与出. 用户可以为每一条**Connection**注册回调函数.
2. 系统由一个**Acceptor**开始, 它本质上是一个专门处理接受新用户连接请求的**Connection**. 它为每个用户建立**Connection**连接, 然后将其随机分流给某一个**Looper**去负责.
3. 每个**Looper**都拥有一个**Poller**. **Poller**只负责在它所监听的那些连接上做epoll, 然后将有事件触发的连接反馈给**Looper**.
4. **Looper**是该系统的核心组件, 每个都单独运行在一个线程中. 它从**Poller**中接收到有事件触发的用户连接后, 会获取并执行它们的回调函数.
5. **ThreadPool**线程池管理着系统中有多少个**Looper**在运行来防止注册过多线程使得性能下降.
6. 额外的, 可调参数的**Cache**组件以LRU的策略来缓存最近访问的资源.

**Turtle**核心网络库部分约**1000**行代码, HTTP+CGI模块约**700**行代码.

### Docker

如果您不在Linux系统上, 但仍希望能在Linux环境下尝试**Turtle**库, 我们提供Vagrant文件来部署一个Linux Docker. 现在**Turtle**兼容在Linux和MacOS上的构建.

1. 安装 [**Vagrant**](https://www.vagrantup.com/downloads)和[**Docker**](https://docs.docker.com/desktop/). 对MacOS用户, 您可以使用homebrew来安装Vagrant, 不过请不要使用homebrew来安装Docker. 请使用上面的链接下载Mac Docker桌面版.

2. 开启运行Docker程序.

3. 把`Vagrantfile`拖出来并和本`Turtle`项目文件夹并行摆放. 例如, 参考如下的文件结构:

```text
/Turtle_Wrapper
    - /Turtle
    - /Vagrantfile
```

4. `cd`到`Turtle_Wrapper`文件夹中并运行指令`vagrant up --provider=docker`. 这一步会花费几分钟的时间去部署环境和安装Build所需要的依赖包.

5. 通过执行`vagrant ssh developer`进入到Docker环境中.

6. `cd`到`/vagrant/Turtle`文件夹. 这个文件夹是和原文件夹`./Turtle`自动同步的. 你可以在原文件中修改代码, 而改动会被自动同步到docker里的文件夹.

7. 按照下个章节的步骤构建项目.

### 构建

您可以使用**CMake**构建本项目.

当您在项目的根路径时, 可以执行以下指令:

```console
// 环境设立 (Linux)
$ sh setup/setup.sh
$ sudo systemctl start mysql
$ sudo mysql < setup/setup.sql  // 设立测试用的默认MySQL

// Build, 多种选项
$ mkdir build
$ cd build
$ cmake .. // 默认为有日志, 不启用定时器
$ cmake -DLOG_LEVEL=NOLOG .. // 无日志
$ cmake -DTIMER=3000 .. // 开启定时器 3000毫秒定时
$ make

// 格式化 & 风格检验 & 代码行数统计
$ make format
$ make cpplint
$ make linecount
```

### 性能基准

为了测试**Turtle**服务器在高并发情况下的性能, 我们采用[Webbench](http://cs.uccs.edu/~cs526/webbench/webbench.htm)作为压测的工具.

性能测试的过程是全自动化的. 您可以执行以下指令来一键压测:

```console
$ make benchmark

# 以上指令在Linux环境中将会
# 1. 构建Webbench压测工具
# 2. 在背景模式中运行http服务器, 默认监听20080端口来传递一个很小的dummy index文件
# 3. 使用Webbench压测工具来创建10500个并发用户请求, 运行5秒时间
# 4. 将性能统计数据汇报到终端
# 5. 关闭背景模式运行中的http服务器并退出
```

我们在Amazon EC2云服务器上进行了性能测试. 具体如下:

+ **硬件配置**: **m5.2xlarge**云服务器, 使用**Ubuntu 20.04 LTS**操作系统, 拥有**8**vCPUs, **32**GiB内存, **50**GiB根硬盘存储. (请注意vCPU不是真正的CPU核, 试验了一下`std::hardware_concurrency() = 2`在这种机型上)
+ **QPS**:
  - **62.3**k (无日志, 无计时器)
  - **52.5**k (有日志, 无计时器)
  - **36.5**k (无日志, 有计时器)
  - **29.8**k (有日志, 有计时器)

我们可以发现, 异步日志只会给系统带来轻微的运行时性能损失, 而计时器功能会带来一定的性能损失, 因为它需要同步操作.

为了更好的了解相对性能, 我们性能测试了一些其他的主流C++网络Web Server库, 并为了公平起见, 尽力采用它们最佳的设定.

重申一下, 我们绝不应该仅仅根据范围有限的基准测试和像我们这样不熟悉的人可能使用的错误配置下的实验结果来判断不同的库的好坏. 此处仅仅是为了得到一个正确的性能数量级.

下列所有的性能测试比较都是在**相同的服务器**上进行, 使用webbench工具在**10500**个并发clients的情况下传输相同的小dummy index文件.

1. [TinyWebServer](https://github.com/qinguoyi/TinyWebServer.git): 最佳QPS = **38.5**k
```console
# 我们使用下列设定来运行TinyWebServer:
# 1. 监听新链接模式 和 客户链接模式: -m 1 LT + ET | -m 3 ET + ET
# 2. 8线程在 8-vCPUs 服务器上
# 3. 关闭日志
# 4. 反应堆模型: -a 0 Proactor | -a 1 Reactor
# 5. 编译器优化级别设置为-O3

# Proactor LT + ET
$ ./server -m 1 -t 8 -c 1 -a 0
$ QPS is 38.5k

# Proactor ET + ET
$ ./server -m 3 -t 8 -c 1 -a 0
$ QPS is 38.2k

# Reactor LT + ET
$ ./server -m 1 -t 8 -c 1 -a 1
$ QPS is 26.7k

# Reactor ET + ET
$ ./server -m 3 -t 8 -c 1 -a 1
$ QPS is 25.6k
```

2. [Muduo](https://github.com/chenshuo/muduo): 最佳QPS = **48.3**k

```console
# 我们使用 'muduo/net/http/tests/HttpServer_test.cc' 作为基准测试程序
# 设定它在基准模式下跑线程池大小=8, 并屏蔽绝大部分日志
```

3. [libevent](https://github.com/libevent/libevent): 单线程 最佳QPS = **29.0**k

我们使用这个示例[脚本](https://github.com/denischatelain/libevent-http-server-get-example)来快速测试

```console
# 注意这个测试是在单线程的设定下跑libevent的http server 使用I/O多路复用
# 它的性能已经非常亮眼了, 并且还没有使用全8核的硬件条件
# 我们在未来可能使用pthread和work queue来在多线程的设定下更充分的测试libevent
# 不过对现在性能基准测试来说, 这是太多的工作量了
```

### API风格
**Turtle**库中的类在设计时将**解耦**这个重点牢牢挂在心中. 大部分的组件都可以单独或某几个拿出来独立使用, 尤其是**网络核心库**中的部分.

下面我们就从最基本的**Socket**类举一个例子, 假设您只是想借用**Turtle**库来避免套接字建立的繁琐步骤. 首先我们来看一下**Socket**类的主要接口:

```CPP
/**
 * 这个Socket类抽象了一个套接字文件符的操作
 * 它可以用来建立客户端或服务端的连接
 * 同时支持Ipv4和Ipv6两种协议
 * */
class Socket {
 public:
  Socket() noexcept;
  auto GetFd() const noexcept -> int;

  /* 客户端: 只需要一步, 直接connect */
  void Connect(NetAddress &server_address);

  /* 服务端: 需要三步, bind + listen + 不断accept */
  void Bind(NetAddress &server_address, bool set_reusable = true);

  /* 进入监听模式 */
  void Listen();

  /* 阻塞模式接收一个新客户连接, 并记录下来它的地址信息 */
  auto Accept(NetAddress &client_address) -> int;

 private:
  int fd_{-1}; 
};
```

在有了这样的接口后, 我们可以非常便捷快速地在几行内搭建客户端和服务端的套接字:

```CPP
#include "core/net_address.h"
#include "core/socket.h"

// 本地Ipv4地址在8080端口
NetAddress local_address("127.0.0.1", 8080, Protocol::Ipv4);

// 搭建一个客户端
Socket client_sock;
client_sock.Connect(local_address);

// 搭建一个服务端
Socket server_sock;
server_sock.Bind(local_address);
server_sock.Listen();

// 接收1个新客户连接请求
// client_address里将会被填充进新客户的ip地址信息
NetAddress client_address;
int client_fd = server_sock.Accept(client_address);
```

**Turtle**中还有许多类似的便于**解耦**单独使用的组件, 可以查看源码根据需求来使用.

### 用例展示

#### 通用

为设立一个自定义化的通用服务器, 用户需要构建一个**TurtleServer**的实例, 然后只需要提供2个回调函数:
1. **OnAccept(Connection \*)**: 在接收新用户连接请求时执行额外的业务逻辑.
2. **OnHandle(Connetion \*)**: 在用户发来请求时的业务逻辑.

请注意大部分的接收新用户的基本功能已经在基本版本[**Acceptor::BaseAcceptCallback**](./src/include/acceptor.h)中实现了, 包括套接字建立, 设立**Connection**并放入**Poller**的监控.

用户所提供的**OnAccept(Connection \*)**函数将会被合并到基本版本中一起执行. 对于另一个函数**OnHandle(Connection \*)**, 并没有基础版本的存在. 用户必须提供给服务器一个版本.

下面让我们一起来看看如何在20行内搭建一个简单的传统echo服务器:

```CPP
#include "core/turtle_server.h"

int main() {
  TURTLE_SERVER::NetAddress local_address("0.0.0.0", 20080);
  TURTLE_SERVER::TurtleServer echo_server(local_address);
  echo_server
      .OnHandle([&](TURTLE_SERVER::Connection* client_conn) {
        int from_fd = client_conn->GetFd();
        auto [read, exit] = client_conn->Recv();
        if (exit) {
          client_conn->GetLooper()->DeleteConnection(from_fd);
          // client_conn 指针在这之后即失效了, 不再触碰
          return;
        }
        if (read) {
	  // 将读buffer复制到写buffer
          client_conn->WriteToWriteBuffer(client_conn->ReadAsString());
	  // 原封不动echo回去
          client_conn->Send();
          client_conn->ClearReadBuffer();
        }
      })
      .Begin();
  return 0;
}
```

这个echo[服务端](./demo/echo_server.cpp)和[用户端](./demo/echo_client.cpp)的demo在`./demo`文件夹中供您参考. 在`build`文件夹路径中, 可以执行以下指令来运行它:

```console
$ make echo_server
$ make echo_client

// 在一个终端窗口
$ ./echo_server

// 在另一个终端窗口
$ ./echo_client
```

#### HTTP

HTTP协议服务端的[demo](./src/http/http_server.cpp)在`./src/http`文件夹中供参考. 它支持**GET**和**HEAD**方法. 一个简单的HTTP协议服务器可以在**Turtle**核心库和HTTP模块扩展库的帮助下在50行内被搭建起来.

#### CGI

CGI模块构建在HTTP服务器上, 并以传统的父子跨进程方式执行. 在解析完参数后, [**Cgier**](./src/include/http/cgier.h)会`fork`一个子进程来执行cgi程序, 并通过临时共享文件的方式将结果返回给父进程.

它假设cgi程序位于`/cgi-bin`文件夹下, 参数以`&`分隔. 例如, 如果有一个远程CGI程序`int add(int a, int b)`将两个整数相加. 要请求计算`1+2=3`, HTTP请求行应该是

```console
GET /cgi-bin/add&1&2 HTTP/1.1
```

#### 数据库
由于数据库是许多web应用程序不可缺少的一部分, **Turtle**也支持与数据库的基本交互, 特别是与**MySQL**. 我们将官方MySQL C++ Connector包装成可以执行查询并返回结果的简单连接. **Turtle**的用户在实现自定义服务回调函数时可以考虑插入此组件使用.

相关的源代码在[db](./src/db)文件夹下. 用户可以参考[setup.sql](./setup/setup.sql)和[mysqler_test](./test/db/mysqler_test.cpp)进行初始设置和简单使用样例参考.

举一个简单的使用样例, 假设在`localhost`的`3306`端口上, 有一个密码为`root`的`root`用户, 且在数据库`test_db`上有一个包含`firstname`和`lastname`两个字段的`user`表. 我们可以进行如下更新和查询:

```CPP
#include <string>
#include "db/mysqler.h"

/* 方便起见 */
using TURTLE_SERVER::DB::MySqler;

int main(int argc, char* argv[]) {
    // 初始化一条数据库连接
    MySqler mysqler = MySqler("127.0.0.1", 3306, "root", "root", "test_db");
    // 同步插入一个新用户 Barack Obama
    std::string command_insert = "INSERT INTO user (firstname, lastname) VALUES ('Barack', 'Obama');";
    mysqler.ExecuteBlocking(command_insert);
    // std::async异步查询所有firstname是Barack的用户
    std::string command_query = "SELECT firstname, lastname FROM user WHERE firstname = 'Barack';"
    auto fut = mysqler.ExecuteQueryNonBlocking(command_query);      
    sql::ResultSet result_set = fut.get();  // 执行
    // 迭代器遍历, 或许很多人的firstname都是Barack
    size_t return_size = result_set->rowsCount();
    while (result_set->next()) {
        // 获取匹配的用户的lastname
        std::string lastname = result_set->getString("lastname");
    }
    return 0;
}
```

#### 日志
使用单例模式以异步消费者-生产者模式来支持日志记录. 调用者以非阻塞的方式生产日志, 后台工作线程会定期以FIFO方式处理自从上次唤醒以来生成的所有日志. "处理"日志的确切方法可以取决于自定义的策略插件. 默认是写入磁盘上的日志文件. 您还可以调整日志刷新间隔长度或刷新日志记数.

宏定义有四个级别的日志记录:
+ `LOG_INFO`
+ `LOG_WARNING`
+ `LOG_ERROR`
+ `LOG_FATAL`

您可以通过在CMake构建中传递标志`-DLOG_LEVEL=NOLOG`来禁用任何日志记录.

### 未来计划

本项目正处于积极的维护和更新中. 新的修正和功能时常会被更新, 在我们时间和技术允许的条件下.

下列是一些**TODO**列表:

- ✅ 支持HTTP协议的GET/HEAD请求与回复
- ✅ 根据[code review](https://codereview.stackexchange.com/questions/282220/tiny-network-web-framework-library-in-c)的建议润色代码
- ✅ 将结构重构为多Reactor模式来增强并发性能
- ✅ 加入性能测试的benchmark
- ✅ 加入缓存层来减轻服务器负载并加快响应
- ✅ 支持动态CGI请求的处理回复
- ✅ 完成单元测试的覆盖
- ✅ 与其他主流网络Web Server库进行性能对比
- [ ] 寻找Turtle的主要运行性能瓶颈
- [ ] 在 [reddit](https://www.reddit.com/r/cpp/comments/10vrv4i/seeking_improve_advice_on_my_c_network_library/)上收到的review建议放在issues上有待仔细研读实验
- ✅ 支持异步日志机制
- ✅ 支持定时器功能来删除不活跃的用户连接
- ✅ 支持数据库连接功能

也欢迎大家提出新的功能需求, 我们会逐一考虑, 如果可行的话将会优先实现. 或者直接提pull request.

### 参考文献

在开发构建本项目的过程中, 收到了许多帮助也参考了不少优秀的参考资料和他人典范作品.

+ [Network Programming Using Internet Sockets](https://beej.us/guide/bgnet/)
+ [Linux多线程服务端编程：使用muduo C++网络库](http://www.amazon.cn/dp/B00FF1XYJI)
+ [Muduo库](https://github.com/chenshuo/muduo)
+ [TinyWebServer](https://github.com/qinguoyi/TinyWebServer)
+ [30天搭建cpp服务器](https://github.com/yuesong-feng/30dayMakeCppServer)
+ [Very basic C++ HTTP Parser](https://codereview.stackexchange.com/questions/205704/very-basic-c-http-parser)
+ [C++ Threaded Logger](https://codereview.stackexchange.com/questions/191880/c-threaded-logger)
+ [MySQL C++ 官方文档](https://dev.mysql.com/doc/connector-cpp/1.1/en/connector-cpp-examples-complete-example-1.html)