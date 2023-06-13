//
// Created by Dell on 2023/6/7.
//

#ifndef MYSQLORM_CONNECTION_H
#define MYSQLORM_CONNECTION_H

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <string.h>
#include <string>
using std::string;

#include <vector>
#include <map>

#include <utils/value.h>
#include <utils/logger.h>
using namespace lh::utils;

#include <mysql.h>
#include <myorm/batch.h>

namespace lh {
namespace myorm {

    class Connection {
    public:
        Connection();
        ~Connection();

        bool connect(const string& host, int port, const string& username, const string& password, const string& database, const string& charset, bool debug);
        bool reconnect();
        void close();
        bool ping();
        void set_ping(int seconds);
        string escape(const string& str);
        string quote(const string& str) const;
        std::vector<string> tables();
        std::vector<std::map<string, Value>> schema(const string& table);
        bool table_exists(const string& table);
        string primary_key(const string& table);

        int insert(const string& sql);
        bool execute(const string& sql);
        std::map<string, Value> fetchone(const string& sql);
        std::vector<std::map<string, Value>> fetchall(const string& sql);

        template <typename T>
        Batch<T> batch(const string& sql);

        // transaction
        bool auto_commit();
        void auto_commit(bool auto_commit);
        bool begin();
        bool rollback();
        bool commit();
        bool savepoint(const string& sp);
        bool rollback_savepoint(const string& sp);
        bool release_savepoint(const string& sp);

    private:
        MYSQL m_mysql;
        string m_host;
        int m_port;
        string m_username;
        string m_password;
        string m_database;
        string m_charset;
        bool m_debug;
        bool m_auto_commit;
        int m_ping;
        int m_last_ping_time;
        char m_quote;
    };

    template <typename T>
    Batch<T> Connection::batch(const string& sql) {
        Batch<T> batch;

        if (m_debug) {
            log_debug("sql: %s", sql.c_str());
        }
        int ret = mysql_real_query(&m_mysql, sql.data(), sql.size());
        if (ret != 0) {
            log_error("mysql_real_query errno:%d error: %s", mysql_errno(&m_mysql), mysql_error(&m_mysql));
            return batch;
        }
        MYSQL_RES *res = mysql_store_result(&m_mysql);
        if (res == NULL) {
            log_error("mysql_real_query errno:%d error: %s", mysql_errno(&m_mysql), mysql_error(&m_mysql));
            return batch;
        }
        int rows = mysql_num_rows(res);
        batch.total(rows);
        batch.result(res);
        return batch;
    }
}
}


#endif //MYSQLORM_CONNECTION_H
