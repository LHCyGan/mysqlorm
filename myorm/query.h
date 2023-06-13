//
// Created by Dell on 2023/6/13.
//

#ifndef MYSQLORM_QUERY_H
#define MYSQLORM_QUERY_H

#include <myorm/model.h>
using namespace lh::myorm;

class Query : public Model<Query> {
public:
    Query() : Model() {}
    Query(Database& db) : Model(db()) {}
    Query(Connection *conn) : Model(conn) {}

    string table() const {
        return m_table;
    }
};


#endif //MYSQLORM_QUERY_H
