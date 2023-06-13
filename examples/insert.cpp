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

    // 把表里面的数据清空，方便测试
    User(db).truncate();

    // 添加一条数据
    auto user = User(db);
    user["name"] = "jack";
    user["age"] = 18;
    user["money"] = 100;
    user.save();

    db.close();
    return 0;
}