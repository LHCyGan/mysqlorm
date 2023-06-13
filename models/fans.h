#pragma once

#include <myorm/model.h>
using namespace lh::myorm;

class Fans : public Model<Fans>
{
public:
    Fans() : Model() {}
    Fans(Database & db) : Model(db()) {}
    Fans(Connection * conn) : Model(conn) {}

    string table() const
    {
        return "t_fans";
    }
};