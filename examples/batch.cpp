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

    // // 把表里面的数据清空，方便测试
    // User(db).truncate();

    // // 批量添加数据
    // std::vector<User> rows;
    // for (int i = 0; i < 22; i++)
    // {
    //     ostringstream name;
    //     name << "name_" << i;
    //     auto user = User();
    //     user["name"] = name.str();
    //     user["age"] = i;
    //     rows.push_back(user);
    // }
    // User(db).insert(rows);

    // #1
    auto batch = User(db).batch(10);
    for (auto all = batch.begin(); all != batch.end(); all++)
    {
        for (auto one = all.begin(); one != all.end(); one++)
        {
            std::cout << one->str() << std::endl;
        }
    }

    // #2
    auto batch = User(db).batch(10);
    for (auto all : batch)
    {
        for (auto one : all)
        {
            std::cout << one.str() << std::endl;
        }
    }

    db.close();
    return 0;
}