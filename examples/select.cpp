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

    // 查询一条数据
    auto one = User(db).where("name", "jack").one();
    std::cout << one.str() << std::endl;
    int uid = one("id");
    string name = one("name");
    int age = one("age");
    std::cout << "id=" << uid << ", name=" << name << ", age=" << age << std::endl;

    // 查询多条数据
    auto model = User(db);
    model.where("age", "between", 10, 22).order("age asc");
    std::vector<User> all = model.all();
    for (auto one : all)
    {
        std::cout << one.str() << std::endl;
    }

    db.close();
    return 0;
}