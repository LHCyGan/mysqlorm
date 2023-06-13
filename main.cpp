//
// Created by Dell on 2023/6/6.
//
#include <iostream>
#include <string>
#include <vector>

#include <myorm/database.h>
#include <myorm/transaction.h>
#include <myorm/connection_pool.h>
using namespace lh::myorm;

#include <models/user.h>
#include <myorm/query.h>

#include <mysql.h>

int main()
{
    Logger::instance()->open("./main.log");

    // 连接数据库
    Database db;
    db.connect("127.0.0.1", 3306, "root", "iu111111", "test", "utf8", true);

    auto all = Query(db).table_name("t_user").all();
    for (auto one : all)
    {
        std::cout << one.str() << std::endl;
    }

//    // 批量查询
//    auto batch = User(db).batch(10);
//    for (auto all : batch)
//    {
//        for (auto one : all)
//        {
//            std::cout << one.str() << std::endl;
//        }
//        std::cout << "--------------------------" << std::endl;
//    }

    db.close();

    return 0;
}