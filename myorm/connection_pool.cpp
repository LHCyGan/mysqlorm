//
// Created by Dell on 2023/6/9.
//

#include <utils/logger.h>
using namespace lh::utils;

#include <myorm/connection_pool.h>
using namespace lh::myorm;

ConnectionPool::ConnectionPool() : m_size(0), m_ping(3600), m_debug(false) {}

ConnectionPool::~ConnectionPool() {
    for (auto it = m_pool.begin();it != m_pool.end(); it ++) {
        (*it)->close();
        delete (*it);
    }
    m_pool.clear();
}

void ConnectionPool::create(const string &host, int port, const string &username, const string &password,
                            const string &database, const string &charset, bool debug) {
    if (m_size <= 0) {
        log_error("connection pool size error");
        return;
    }

    m_debug = m_debug;
    for (int i = 0;i < m_size;i ++) {
        auto *conn = new Connection();
        conn->connect(host, port, username, password, database, charset, debug);
        if (m_ping > 0) {
            conn->set_ping(m_ping);
        }
        put(conn);
    }
}

void ConnectionPool::size(int size) {
    m_size = size;
}

void ConnectionPool::ping(int ping) {
    m_ping = ping;
}

void ConnectionPool::put(Connection *conn) {
    m_pool.push_back(conn);
}

Connection *ConnectionPool::get() {
    if (m_pool.empty()) {
        return nullptr;
    }
    auto conn = m_pool.front();
    m_pool.pop_front();
    if (!conn->ping()) {
        conn->reconnect();
    }
    return conn;
}

