//
// Created by Dell on 2023/6/9.
//

#ifndef MYSQLORM_CONNECTION_POOL_H
#define MYSQLORM_CONNECTION_POOL_H

#include <list>
#include <myorm/connection.h>

namespace lh {
    namespace myorm {
        class ConnectionPool {
        public :
            ConnectionPool();

            ~ConnectionPool();

            void
            create(const string &host, int port, const string &username, const string &password, const string &database,
                   const string &charset, bool debug = false);

            void size(int size);

            void ping(int ping);

            void put(Connection *conn);

            Connection *get();

        private:
            std::list<Connection*> m_pool;
            int m_size;
            int m_ping;
            bool m_debug;
        };
    }
}

#endif //MYSQLORM_CONNECTION_POOL_H
