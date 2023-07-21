#pragma once

/*********************************MySQL版本******************************************/

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <cassert>
#include <stdlib.h>
#include "include/mysql.h"
#include "../Common/util.hpp"
#include "../Common/log.hpp"

// 根据题目list文件，加载所有题目信息到内存中
// model: 主要用来和数据进行交互，对外提供访问数据的接口
namespace ns_model
{
    using namespace ns_util;
    using namespace ns_log;
    // 题目细节
    struct Question
    {
        std::string number; // 题目编号，唯一
        std::string title;  // 题目的标题
        std::string desc;   // 题目的描述
        std::string star;   // 难度: 简单 中等 困难
        std::string header; // 题目预设给用户在代码编辑器的代码
        std::string tail;   // 题目的测试用例，需要和header拼接，形成完整的编译代码
        int cpu_limit;      // 时间要求(S)
        int mem_limit;      // 空间要求(KB)
    };
    
    const std::string oj_questions = "oj_questions";
    const std::string host = "127.0.0.1";
    const std::string user = "oj_client";
    const std::string password = "guet@4402.snow";
    const std::string database = "oj";
    const int port = 8000;
    class Model
    {
    public:
        Model() {}
        ~Model() {}
        bool QueryMySQL(const std::string &sql, std::vector<Question> *out)
        {
            // 1. 创建MySQL句柄
            MYSQL *my = mysql_init(nullptr);

            // 2. 连接数据库
            if (mysql_real_connect(my, host.c_str(), user.c_str(), password.c_str(), database.c_str(), port, nullptr, 0) == nullptr)
            {
                LOG(FATAL) << "连接数据库失败!" << "\n";
                return false;
            }

            // 一定要设置该链接的编码格式, 要不然页面会出现乱码问题
            mysql_set_character_set(my, "utf8");
            LOG(INFO) << "连接数据库成功~" << "\n";

            // 3. 执行sql语句
            if (mysql_query(my, sql.c_str()) != 0) // 执行成功返回0
            {
                LOG(WARNING) << sql << " execute error!" << "\n";
                return false;
            }

            // 提取结果
            MYSQL_RES * res = mysql_store_result(my);

            // 分析结果
            int rows = mysql_num_rows(res); // 获得行数量
            int cols = mysql_num_fields(res); // 获得列数量

            struct Question q;
            for (int i = 0; i < rows; i++)
            {
                // 从表中按行拿取数据
                MYSQL_ROW row = mysql_fetch_row(res);
                q.number = row[0];
                q.title = row[1];
                q.star = row[2];
                q.desc = row[3];
                q.header = row[4];
                q.tail = row[5];
                q.cpu_limit = atoi(row[6]);
                q.mem_limit = atoi(row[7]);
                out->push_back(q);
            }
            // 释放结果空间
            free(res);
            // 关闭MySQL连接
            mysql_close(my);
            return true;
        }
        bool GetAllQuestions(std::vector<Question> *out)
        {
            std::string sql = "select * from ";
            sql += oj_questions;
            return QueryMySQL(sql, out);
        }
        bool GetOneQuestion(const std::string &number, Question *q)
        {
            bool res = false;
            std::string sql = "select * from ";
            sql += oj_questions;
            sql += " where number = ";
            sql += number;
            std::vector<Question> result;
            if (QueryMySQL(sql, &result))
            {
                if (result.size() == 1)
                {
                    *q = result[0];
                    res = true;
                }
            }
            return res;
        }
    };
}