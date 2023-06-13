#include <iostream>

#include <utils/logger.h>
using namespace lh::utils;

#include <myorm/database.h>
#include <myorm/transaction.h>
using namespace lh::myorm;

#include <models/user.h>

int main()
{
    Logger::instance()->open("./main.log");

    // 连接数据库
    Database db;
    db.connect("127.0.0.1", 3306, "root", "3scDRHoyMrqqpUu1", "test", "utf8", true);

    // 外层事务
    Transaction trx(db);
    trx.begin();

    User u1(db);
    u1["name"] = "jack";
    u1["age"] = 18;
    u1["money"] = 100;
    if (!u1.save())
    {
        trx.rollback();
    }
    {
        // 内层事务
        trx.begin();
        User u2(db);
        u2["name"] = "lucy";
        u2["age"] = 20;
        u2["money"] = 200;
        if (!u2.save())
        {
            trx.rollback();
        }
        trx.commit();
    }

    trx.commit();

    db.close();
    return 0;
}