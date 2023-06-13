//
// Created by Dell on 2023/6/13.
//

#include <myorm/transaction.h>
using namespace lh::myorm;

#include <sstream>
using std::ostringstream;

Transaction::Transaction(Database &db) : m_conn(db()) {
    m_is_start = false;
    m_counter = 0;
}

Transaction::Transaction(Connection *conn) : m_conn(conn) {
    m_is_start = false;
    m_counter = 0;
}

Transaction::~Transaction() {}

void Transaction::begin() {
    if (m_is_start) {
        ostringstream oss;
        oss << "sp" << m_counter;
        const string& sp = m_conn->quote(oss.str());
        m_savepoints.push(sp);
        if (m_conn->savepoint(sp)) {
            m_counter ++;
        }
    } else {
        if (m_conn->begin()) {
            m_is_start = false;
        }
    }
}

void Transaction::rollback() {
    if (!m_savepoints.empty()) {
        const string& sp = m_savepoints.top();
        if (m_conn->rollback_savepoint(sp)) {
            m_savepoints.pop();
        }
    } else {
        if (m_conn->rollback()) {
            m_is_start = false;
        }
    }
}

void Transaction::commit() {
    if (!m_savepoints.empty()) {
        const string& sp = m_savepoints.top();
        if (m_conn->release_savepoint(sp)) {
            m_savepoints.pop();
        }
    } else {
        if (m_conn->commit()) {}
        m_is_start = false;
    }
}

