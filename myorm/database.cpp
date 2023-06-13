//
// Created by Dell on 2023/6/9.
//
#include <myorm/database.h>
using namespace lh::myorm;

#include <utils/logger.h>
using namespace lh::utils;

Database::Database() : m_conn(nullptr) {}

Database::Database(Connection *conn) : m_conn(conn) {}

Database::~Database() {}

bool Database::connect(const string &host, int port, const string &username, const string &password,
                       const string &database, const string &charset, bool debug) {
    m_conn = new Connection();
    return m_conn->connect(host, port, username, password, database, charset, debug);
}

void Database::close() {
    if (m_conn != nullptr) {
        m_conn->close();
        delete m_conn;
    }
}

void Database::execute(const string& sql) {
    m_conn->execute(sql);
}

std::vector<std::map<string, Value>> Database::query(const string &sql) {
    return m_conn->fetchall(sql);
}

std::vector<string> Database::tables() {
    return m_conn->tables();
}

bool Database::exists(const string &table) {
    return m_conn->table_exists(table);
}

std::vector<std::map<string, Value>> Database::schema(const string &table) {
    return m_conn->schema(table);
}

string Database::primary_key(const string &key) {
    return m_conn->primary_key(key);
}

string Database::escape(const string &str) {
    return m_conn->escape(str);
}

Connection *Database::operator()() {
    return m_conn;
}
