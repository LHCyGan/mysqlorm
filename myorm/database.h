//
// Created by Dell on 2023/6/9.
//

#ifndef MYSQLORM_DATABASE_H
#define MYSQLORM_DATABASE_H

#include <myorm/connection.h>
using namespace lh::myorm;

namespace lh {
    namespace myorm {

        class Database {
        public:
            Database();
            Database(Connection *conn);
            ~Database();

            bool connect(const string &host, int port, const string & username, const string & password, const string & database, const string & charset = "utf8", bool debug = false);
            void close();
            void execute(const string& sql);
            std::vector<std::map<string, Value>> query(const string& sql);
            std::vector<string> tables();
            bool exists(const string& table);
            std::vector<std::map<string, Value>> schema(const string& table);
            string primary_key(const string& key);
            string escape(const string& str);

            Connection *operator() ();

        private:
            Connection *m_conn;

        };
    }
}

#endif //MYSQLORM_DATABASE_H
