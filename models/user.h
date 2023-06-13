#pragma once

#include <myorm/model.h>
using namespace lh::myorm;

class User : public Model<User>
{
public:
    User() : Model() {}
    User(Database & db) : Model(db()) {}
    User(Connection * conn) : Model(conn) {}

    string table() const
    {
        return "t_user";
    }
};