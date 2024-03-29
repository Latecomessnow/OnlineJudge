# 负载均衡OJ

## 项目简介  
项目的核心框架是由OJ服务器模块和编译并运行模块两个模块构成的。用户访问网站，有可能会请求题目列表，有可能会请求编写某道题，还有可能会提交代码请求判题，OJ服务器需要去做这些路由工作，根据不同的请求去构建数据返回对应的网页信息。如果是请求判题的话，OJ服务器还需要去找到负载最小的主机请求编译并运行服务(编译并运行服务也是一套网络服务)，返回相应的结果给用户，框架图如下。
    ![image](https://github.com/Latecomessnow/OnlineJudge/assets/101911487/ff417eec-00de-4ebc-b3d7-9f9138d1a5b4)

## 项目演示
- 项目首页  
  
    ![image](https://github.com/Latecomessnow/OnlineJudge/assets/101911487/ca8afbfa-b2ee-4bee-8820-35f77e30f5a7)

 <br>
 
- 题目列表
   
    ![image](https://github.com/Latecomessnow/OnlineJudge/assets/101911487/540759d8-3878-492b-aea9-965740246e6f)

 <br>
 
- 在线判题  
  
    ![image](https://github.com/Latecomessnow/OnlineJudge/assets/101911487/47cd3875-a9d0-433f-91f4-9ceb27f22ac6)

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

## 项目地址
   
[网站链接](http:/119.91.112.204:8080) 

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

    - 运行程序
            
            /******************************************
             * 程序运行
             * 1. 代码跑完，结果正确
             * 2. 代码跑完，结果不正确
             * 3. 代码没有跑完，出现异常
             * Run只需要考虑代码跑完，而不需要考虑结果正确与否
             * 结果是否正确应该用测试用例去决定
             * 我们只考虑代码是否正确运行完毕
             *
             * 我们必须知道可执行程序是哪一个
             * 一个程序在默认启动的时候会有如下文件描述符
             * 标准输入: 不处理
             * 标准输出: 程序运行完成，结果是什么
             * 标准错误: 获取运行时错误的信息
             ********************************************/
             

- compile_run负责去编译并运行客户端代码

    需要知道的是客户端发送过来给服务端的代码是通过json串传过来的，需要对json串进行反序列化的过程，可compile_run就是负责调用编译和运行服务，并去处理序列化和反序列化工作，并对程序运行的退出码做出相应的文件记录，最后还需要对这些编译运行过程中形成的临时文件进行清理工作
    ```C++
    static void Start(const std::string &in_json, std::string *out_json)
    ```
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
    

    把用户提交过来的in_json串含有的以下东西进行反序列化工作

        1. code: 用户在OJ上编写的代码并经过本地拼接之后的完整代码
        2. input: 可做扩展用，提供用户进行标准输入
        3. cpu_limit: 代码的时间限制
        4. mem_limit: 代码的空间限制
   反序列化工作完成后得到一个序列化的json串，就可以获得json串中相应的信息了，再对其中的的代码文件进行编译并运行，将运行之后的结果存进out_value序列化的json串中，然后再反序列化给our_json
   ```C++
    out_value["status"] = status_code;
    out_value["reason"] = CodeToDesc(status_code, file_name);
    ```
    最后填入编译并运行之后的退出码和运行结果信息，然后清理产生的临时文件

- compile_server.cc
    主函数，主要负责启动后端的编译服务，将能够提供编译服务的主机启动起来，并能够去处理请求编译服务的HTTP请求，对HTTP请求进行响应
    ```C++
        svr.Post("/compile_and_run", [](const Request &req, Response & resp){
        // 用户请求的服务正文正是我们想要的json string
        std::string in_json = req.body; // 请求正文
        std::string out_json;
        if (!in_json.empty())
        {
            CompileAndRun::Start(in_json, &out_json);
            resp.set_content(out_json, "application/json;charset=utf-8");
        }
    });
    ```
    再去获取编译并运行代码后的结果out_json，然后构建响应发送回客户端
## 6. OJServer模块

基于MVC结构的oj服务设计，搭建一个小型网站

    1. 获取首页，用题目列表充当
    2. 编辑区域页面
    3. 提交判题功能(编译并运行)
   
    M: Model,通常是和数据交互的模块，比如，对题库进行增删改查（文件版，MySQL）
    V: view, 通常是拿到数据之后，要进行构建网页，渲染网页内容，展示给用户的(浏览器)
    C: control, 控制器，就是我们的核心业务逻辑
### 第一个功能: 用户请求的服务路由功能

用户在浏览器网页端，有可能会请求网站首页，或题目列表，或某一道特定题目，还有可能会提交代码，服务路由功能需要对这些不同请求做出相应的响应，在oj_server.cc中调用Control模块进行对上述请求的处理，如获取题目列表，就去调用Control的Questions函数获取所有题目列表的html网页，获取某道题目就调用Control的Qestion(num, html)获取某道题的网页，还有提交代码就调用Control的Judge函数去调用编译服务

### 第二个功能: Model功能，提供对数据的操作

主要负责对数据进行交互，将存在于文件或数据库中的所有题目信息加载到内存中，以供上层对题目列表的调用，还需要提供获取某道题目的接口
### 第三个功能: Control功能，逻辑控制模块

Control模块是核心业务逻辑模块，负责负载均衡的设计，还负责从Model模块中拿到数据后，再调用View模块进行网页的渲染工作
### 第四个功能: View功能，需要有数据渲染

通过Model模块拿到数据后，再利用Ctemplate库进行网页的渲染

## 7. MySQL版本
   - 创建表结构  

```Java
    create table if not exists `oj_questions`(
    number int primary key auto_increment comment '题目的编号',
    title varchar(64) not null comment '题目的标题',
    star varchar(8) not null comment '题目的难度',
    desc text not null comment '题目描述',
    header text not null comment '题目头部，给用户看的代码',
    tail text not null comment '题目尾部，包含我们的测试用例',
    cpu_limit int default 1 comment '题目的时间限制',
    mem_limit int default 5000000 comment '题目的空间限制'
    )engine=innodb default charset=utf8;
```
