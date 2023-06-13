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

    // #1
    auto one = User(db).where("id", 2).one();
    one.remove();

    // #2
    User(db).where("id", "in", {1, 2}).remove();

    db.close();
    return 0;
}