#include <iostream>

#include <utils/logger.h>
using namespace lh::utild;

#include <myorm/database.h>
using namespace lh::myorm;

#include <models/user.h>

int main()
{
    Logger::instance()->open("./main.log");

    // 连接数据库
    Database db;
    db.connect("127.0.0.1", 3306, "root", "3scDRHoyMrqqpUu1", "test", "utf8", true);

    // count
    int num = User(db).where("id", 1).count();
    std::cout << num << std::endl;

    // sum
    int total = User(db).where("id", "in", {1, 2}).sum("age");
    std::cout << total << std::endl;

    // min
    int total1 = User(db).where("id", "in", {1, 2}).min("age");
    std::cout << total1 << std::endl;

    // max
    int total2 = User(db).where("id", "in", {1, 2}).max("age");
    std::cout << total2 << std::endl;

    // average
    double total3 = User(db).where("id", "in", {1, 2}).average("`age`");
    std::cout << total3 << std::endl;

    // exists
    bool exists = User(db).where("name", "jack").exists();
    if (exists)
    {
        std::cout << "yes" << std::endl;
    }
    else
    {
        std::cout << "no" << std::endl;
    }

    // column
    std::vector<Value> all = User(db).where("id", "in", {1, 2}).column("name");
    for (auto it = all.begin(); it != all.end(); it++)
    {
        std::cout << (string)*it << std::endl;
    }

    // scalar
    int age = User(db).where("id", 3).scalar("age");
    std::cout << age << std::endl;

    db.close();
    return 0;
}