//
// Created by Dell on 2023/6/13.
//

#ifndef MYSQLORM_TRANSACTION_H
#define MYSQLORM_TRANSACTION_H
#include <string>
#include <stack>
using std::string;

#include <myorm/database.h>

namespace lh {
    namespace myorm {

        class Transaction {
        public :
            Transaction(Database& db);
            Transaction(Connection* conn);
            ~Transaction();

            void begin();
            void rollback();
            void commit();

        private:
            Connection *m_conn;
            bool m_is_start;
            std::stack<string> m_savepoints;
            int m_counter;
        };
    }
}

#endif //MYSQLORM_TRANSACTION_H
