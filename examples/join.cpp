#include <iostream>

#include <utils/logger.h>
using namespace lh::utils;

#include <myorm/database.h>
using namespace lh::myorm;

#include <models/user.h>
#include <models/fans.h>

int main()
{
    Logger::instance()->open("./main.log");

    // 连接数据库
    Database db;
    db.connect("127.0.0.1", 3306, "root", "3scDRHoyMrqqpUu1", "test", "utf8", true);

    // join
    User(db).truncate();

    // insert into t_user(`name`,`age`,`money`) values('jack',18,100),('lucy',20,200),('duke',30,300),('sean',40,400);
    // insert into t_fans(`uid`,`fid`) values(1,4),(2,4);
    auto model = User(db);
    model.select("u.*").alias("u").join(Fans().table(), "f", "u.id=f.uid");
    auto all = model.where("f.fid = 4").order("age asc").all();
    for (auto one : all)
    {
        std::cout << one.str() << std::endl;
    }

    db.close();
    return 0;
}