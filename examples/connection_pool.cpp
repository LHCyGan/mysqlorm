#include <iostream>

#include <utils/logger.h>
using namespace lh::utils;

#include <myorm/database.h>
#include <myorm/transaction.h>
#include <myorm/connection_pool.h>
using namespace lh::myorm;

#include <models/user.h>
#include <models/fans.h>

int main()
{
    Logger::instance()->open("./main.log");

    ConnectionPool pool;
    pool.size(3);
    pool.create("127.0.0.1", 3306, "root", "3scDRHoyMrqqpUu1", "test", "utf8", true);
    
    auto conn = pool.get();

    // 添加一条数据
    auto user = User(conn);
    user["name"] = "jack";
    user["age"] = 18;
    user["money"] = 100;
    user.save();

    pool.put(conn);

    return 0;
}
