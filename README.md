# 负载均衡OJ

## 技术栈
* C++ STL 标准库
* Boost 准标准库(字符串切割)
* cpp-httplib 第三方开源网络库
* ctemplate 第三方开源前端网页渲染库
* jsoncpp 第三方开源序列化、反序列化库
* 负载均衡设计
* 多进程、多线程
* MySQL C connect
* Ace前端在线编辑器
* html/css/js/jquery/ajax 

## 1. 项目宏观结构

1. Common模块: 公共模块
2. CompileServer模块: 编译并运行模块
3. OJServer模块: 处理业务模块，如用户获取题目列表，查看题目，编写代码等等，主要还要负责负载均衡的调度

## 2. leetcode 结构
* 实现类似 leetcode 的题目列表+在线编程功能

## 3. 编写思路
  1. 先编写 compile_server
  2. oj_server
  3. version1 基于文件版的在线OJ
  4. 前端的页面设计
  5. version2 基于 MySQL 版的在线OJ
## 4. Common模块
功能类:

1. 提供文件路径以及后缀名的拼接
   
    在编译运行过程中，有可能会使用到源文件，编译报错的文件，源文件生成可执行程序的文件，标准输入的文件，标准输出的文件，标准错误的文件，所以需要生成这些带路径的文件名的函数

2. 提供文件操作
   
    为了能够区别源文件是否能够正常编译成功还是编译报错，假设如果编译成功则可以生成可执行程序，若失败则生成编译报错的文件，这时就需要有判断某个文件是否存在的函数，该函数能够做到传入带路径文件名即可判断该文件是否存在
  
3. 提供日志模块以便开发和维护
   
    为了debug的需要，添加日志是必须的，日志应该带上
    * 日志的等级
    * 日志的文件名(出错的地方所在文件名)
    * 日志的报错行
    * 日志的出错时间
## 5. CompileServer模块

- compile只负责编译服务
    
    编写一个编译类，创建出一个子进程把客户端发送过来的代码文件通过execlp程序替换调用g++编译器去编译代码文件，期间创建一个临时文件，并把标准错误重定向到该临时文件中，以便客户端能够看到编译报错的错误信息
    ```C++
    execlp("g++", "g++", "-o", PathUtil::Exe(file_name).c_str(),
                        PathUtil::Src(file_name).c_str(), "-std=c++11", "-D", "COMPILER_ONLINE", nullptr /*程序替换最后一个参数为空*/);
    ```

- runner只负责运行编译生成的可执行程序

    runner不保证其执行结果的正确与否，结果是否正确应该由测试用例去决定。
    程序运行前应该设置其时间限制和空间限制，运行时只需要指定文件名，不需要带路径，带后缀，程序运行起来后需要可执行程序、标准输入、标准输出、标准错误四个文件，将其运行结果存进stdout文件中，当然也是通过创建子进程的方式去运行该可执行程序

- compile_run负责去编译并运行客户端代码

    需要知道的是客户端发送过来给服务端的代码是通过json串传过来的，需要对json串进行反序列化的过程，可compile_run就是负责调用编译和运行服务，并去处理序列化和反序列化工作，并对程序运行的退出码做出相应的文件记录，最后还需要对这些编译运行过程中形成的临时文件进行清理工作

       * 输入:
       * code: 用户提交提交过来的代码
       * stdin: 用户有可能进行标准输入，可做扩展
       * cpu_limit: 时间要求
       * mem_limit: 空间要求
       *
       * 输出:
       * 必填:
       * status: 用户提交代码运行后的返回值
       * reason: 请求结果
       *
       * 选填:
       * stdout: 程序运行完的结果
       * stderr: 程序运行完的错误结果

- compile_server
  
## 6. OJServer模块