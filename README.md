# 负载均衡OJ
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
### Common
功能类:

1. 提供文件路径以及后缀名的拼接
2. 提供文件操作
3. 提供日志模块以便开发和维护
### CompileServer

### OJServer


