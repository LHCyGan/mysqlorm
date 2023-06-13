#include <iostream>

#include <utils/logger.h>
using namespace lh::utils;

#include <myorm/database.h>
using namespace lh::myorm;

#include <models/user.h>

int main()
{
    Logger::instance()->open("./main.log");

    // 连接数据库
    Database db;
    db.connect("127.0.0.1", 3306, "root", "3scDRHoyMrqqpUu1", "test", "utf8", true);

    // 特殊字符转义
    string str = "What's this";
    std::cout << db.escape(str) << std::endl;

    // 获取数据库里全部表的名称
    auto tables = db.tables();
    for (auto it = tables.begin(); it != tables.end(); it++)
    {
        std::cout << *it << std::endl;
    }

    // 获取指定表的表结构信息
    auto schemas = db.schema("t_user");
    for (auto it = schemas.begin(); it != schemas.end(); it++)
    {
        for (auto iter = it->begin(); iter != it->end(); iter++)
        {
            std::cout << iter->first << "=" << (string)(iter->second) << ", ";
        }
        std::cout << std::endl;
    }

    // 判断数据库里指定表是否存在
    bool exists = db.exists("t_user");
    if (exists)
    {
        std::cout << "table exists" << std::endl;
    }
    else
    {
        std::cout << "table not exists" << std::endl;
    }

    // 获取指定表的主键名称
    auto pk = db.primary_key("t_user");
    std::cout << "primary key: " << pk << std::endl;

    // 直接执行原始 SQL 语句
    string sql = "update t_user set age=22 where name='lucy'";
    db.execute(sql);

    // 直接执行原始 SQL 查询语句
    sql = "select * from t_user";
    std::vector<std::map<string, Value>> all = db.query(sql);
    for (auto one = all.begin(); one != all.end(); one++)
    {
        for (auto it = one->begin(); it != one->end(); it++)
        {
            std::cout << it->first << "=" << string(it->second) << ", ";
        }
        std::cout << std::endl;
    }

    db.close();
    return 0;
}