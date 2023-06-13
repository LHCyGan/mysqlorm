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
    auto one = User(db).where("id", 1).one();
    one["name"] = "sean";
    one["age"] = 30;
    one.save();

    // #2
    User(db).where("id", 1).update({
        {"name", "duke"},
        {"age", 20}
    });

    // #3
    auto user = User();
    user["name"] = "lucy";
    user["age"] = 30;
    User(db).where("id", 1).update(user);

    db.close();
    return 0;
}