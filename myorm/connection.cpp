//
// Created by Dell on 2023/6/7.
//

#include <myorm/connection.h>
using namespace lh::myorm;

#include <ctime>
#include <sstream>
#include <iterator>
using std::stringstream;
using std::ostringstream;

Connection::Connection() {
    mysql_init(&m_mysql);
    m_ping = 3600;
    m_last_ping_time = time(nullptr);
    m_quote = '`';
}

Connection::~Connection() {

}

bool Connection::connect(const string &host, int port, const string &username, const string &password,
                         const string &database, const string &charset, bool debug) {
    m_host = host;
    m_port = port;
    m_username = username;
    m_password = password;
    m_database = m_database;
    m_charset = charset;
    m_debug = debug;
    mysql_options(&m_mysql, MYSQL_SET_CHARSET_NAME, charset.c_str());
    MYSQL *res = mysql_real_connect(&m_mysql, host.c_str(), username.c_str(), password.c_str(), database.c_str(), port, NULL, 0);
    if (res == NULL) {
        log_error("mysql_real_connect errno:%d error:%s", mysql_errno(&m_mysql), mysql_error(&m_mysql));
        return false;
    }

    if (debug) {
        log_debug("mysql connect success: host=%s port=%d username=%s database=%s", host.c_str(), port, username.c_str(), database.c_str());
    }

    m_auto_commit = true;
    mysql_autocommit(&m_mysql, 1);
    return true;
}

bool Connection::reconnect() {
    close();
    return connect(m_host, m_port, m_username, m_password, m_database, m_charset, m_debug);
}

void Connection::close() {
    if (m_debug) {
        log_debug("mysql close: host=%s port=%d username=%s database=%s", m_host.c_str(), m_port, m_username.c_str(), m_database.c_str());
    }
    mysql_close(&m_mysql);
}

bool Connection::ping() {
    int current_time = time(nullptr);
    if (current_time - m_last_ping_time > m_ping) {
        m_last_ping_time = current_time;
        return mysql_ping(&m_mysql) == 0;
    }
    return true;
}

void Connection::set_ping(int seconds) {
    m_ping = seconds;
}

string Connection::escape(const string &str) {
    // 用于将字符串中的特殊字符转义
    int size = 2 * str.size() + 1;
    char *buf = new char[size];
    memset(buf, 0, size);
    int len = mysql_real_escape_string(&m_mysql, buf, str.c_str(), str.size());
    string ret = string(buf, len);
    delete[] buf;
    return ret;
}

string Connection::quote(const string &str) const {
    char separator = '.';
    size_t index = str.find(separator);
    if (index == string::npos) {
        ostringstream oss;
        oss << m_quote << str << m_quote;
        return oss.str();
    }

    std::vector<string> output;
    stringstream ss(str);
    string item;
    while (getline(ss, item, separator)) {
        output.push_back(item);
    }
    ostringstream oss;
    for (auto it = output.begin();it != output.end(); it ++) {
        if (it != output.begin()) {
            oss << ".";
        }
        oss << m_quote << (*it) << m_quote;
    }
    return oss.str();
}

std::vector<string> Connection::tables() {
    std::vector<string> all;
    MYSQL_RES *res = mysql_list_tables(&m_mysql, NULL);
    if (res == NULL) {
        log_error("mysql_list_tables errno:%d error:%s", mysql_errno(&m_mysql), mysql_error(&m_mysql));
        return all;
    }
    int fields = mysql_num_fields(res);
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)) != NULL) {
        for (int i = 0;i < fields;i ++) {
            char *data = row[i];
            all.push_back(data);
        }
    }
    mysql_free_result(res);
    return all;
}

std::vector<std::map<string, Value>> Connection::schema(const string& table) {
    ostringstream  oss;
    oss << "select column_name,column_key,data_type,extra,column_comment from information_schema.columns where table_schema='" << escape(m_database) << "' and table_name='" << escape(table) << "'";
    const string& sql = oss.str();
    return fetchall(sql);
}

bool Connection::table_exists(const string &table) {
    ostringstream oss;
    oss << "select table_name from information_schema.tables WHERE table_schema='" << m_database << "' and table_name='"
        << table << "'";
    const string &sql = oss.str();
    auto one = fetchone(sql);
    return !one.empty();
}

string Connection::primary_key(const string& table) {
    auto all = schema(table);
    for (auto it = all.begin(); it != all.end(); it++) {
        if ((*it)["column_key"] == "PRI") {
            return (*it)["column_name"];
        }
    }
    return "";
}

int Connection::insert(const string& sql) {
    if (m_debug) {
        log_debug("sql: %s", sql.c_str());
    }
    int ret = mysql_real_query(&m_mysql, sql.data(), sql.size());
    if (ret != 0)
    {
        log_error("mysql_real_query errno:%d error:%s", mysql_errno(&m_mysql), mysql_error(&m_mysql));
        return -1;
    }
    return mysql_insert_id(&m_mysql);
}

bool Connection::execute(const string& sql) {
    if (m_debug)
    {
        log_debug("sql: %s", sql.c_str());
    }
    int ret = mysql_real_query(&m_mysql, sql.data(), sql.size());
    if (ret != 0)
    {
        log_error("mysql_real_query errno:%d error:%s", mysql_errno(&m_mysql), mysql_error(&m_mysql));
        return false;
    }
    return true;
}

std::map<string, Value> Connection::fetchone(const string &sql) {
    std::map<string, Value> one;
    if (m_debug) {
        log_debug("sql: %s", sql.c_str());
    }
    int ret = mysql_real_query(&m_mysql, sql.data(), sql.size());
    if (ret != 0)
    {
        log_error("mysql_real_query errno:%d error:%s", mysql_errno(&m_mysql), mysql_error(&m_mysql));
        return one;
    }
    MYSQL_RES *res = mysql_store_result(&m_mysql);
    if (res == NULL) {
        log_error("mysql_store_result errno:%d error:%s", mysql_errno(&m_mysql), mysql_error(&m_mysql));
        return one;
    }
    int fields = mysql_num_fields(res);
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)) != NULL) {
        unsigned long *lengths = mysql_fetch_lengths(res);
        for (int i = 0;i < fields;i ++) {
            MYSQL_FIELD *field = mysql_fetch_field_direct(res, i);
            char *data = row[i];
            unsigned int length = lengths[i];
            Value v;
            v  = string(data, length);
            if (IS_NUM(field->type))
            {
                if (length == 0)
                {
                    v.type(Value::V_NULL);
                }
                else
                {
                    if (field->type == MYSQL_TYPE_DECIMAL || field->type == MYSQL_TYPE_FLOAT || field->type == MYSQL_TYPE_DOUBLE)
                    {
                        v.type(Value::V_DOUBLE);
                    }
                    else
                    {
                        v.type(Value::V_INT);
                    }
                }
            }
            else if (field->type == MYSQL_TYPE_NULL)
            {
                v.type(Value::V_NULL);
            }
            else
            {
                v.type(Value::V_STRING);
            }
            one[field->name] = v;
        }
    }
    mysql_free_result(res);

    return one;
}

std::vector<std::map<string, Value>> Connection::fetchall(const string& sql) {
    std::vector<std::map<string, Value>> all;
    if (m_debug)
    {
        log_debug("sql: %s", sql.c_str());
    }
    int ret = mysql_real_query(&m_mysql, sql.data(), sql.size());
    if (ret != 0)
    {
        log_error("mysql_real_query errno:%d error:%s", mysql_errno(&m_mysql), mysql_error(&m_mysql));
        return all;
    }
    MYSQL_RES * res = mysql_store_result(&m_mysql);
    if (res == NULL)
    {
        log_error("mysql_store_result errno:%d error:%s", mysql_errno(&m_mysql), mysql_error(&m_mysql));
        return all;
    }
    int fields = mysql_num_fields(res);
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)) != NULL)
    {
        std::map<string, Value> one;
        unsigned long * lengths = mysql_fetch_lengths(res);
        for (int i = 0; i < fields; i++)
        {
            MYSQL_FIELD * field = mysql_fetch_field_direct(res, i);
            char * data = row[i];
            unsigned int length = lengths[i];
            Value v;
            v = string(data, length);
            if (IS_NUM(field->type))
            {
                if (length == 0)
                {
                    v.type(Value::V_NULL);
                }
                else
                {
                    if (field->type == MYSQL_TYPE_DECIMAL || field->type == MYSQL_TYPE_FLOAT || field->type == MYSQL_TYPE_DOUBLE)
                    {
                        v.type(Value::V_DOUBLE);
                    }
                    else
                    {
                        v.type(Value::V_INT);
                    }
                }
            }
            else if (field->type == MYSQL_TYPE_NULL)
            {
                v.type(Value::V_NULL);
            }
            else
            {
                v.type(Value::V_STRING);
            }
            one[field->name] = v;
        }
        all.push_back(one);
    }
    mysql_free_result(res);
    return all;
}

bool Connection::auto_commit() {
    return m_auto_commit;
}

void Connection::auto_commit(bool auto_commit) {
    m_auto_commit = auto_commit;
    if (auto_commit) {
        mysql_autocommit(&m_mysql, 1);
    }
    else {
        mysql_autocommit(&m_mysql, 0);
    }
}

bool Connection::begin() {
    string sql = "begin";
    auto_commit(false);
    if (execute(sql)) {
        return true;
    }
    auto_commit(true);
    return false;
}

bool Connection::rollback() {
    string sql = "rollback";
    if (execute(sql)) {
        auto_commit(true);
        return true;
    }
    return false;
}

bool Connection::commit() {
    string sql = "commit";
    if (execute(sql)) {
        auto_commit(true);
        return true;
    }
    return false;
}

bool Connection::savepoint(const string &sp) {
    ostringstream oss;
    oss << "savepoint " << sp;
    const string& sql = oss.str();
    return execute(sql);
}

bool Connection::rollback_savepoint(const string &sp) {
    ostringstream oss;
    oss << "rollback to savepoint " << sp;
    const string& sql = oss.str();
    return execute(sql);
}

bool Connection::release_savepoint(const string &sp) {
    ostringstream oss;
    oss << "release savepoine " << sp;
    const string& sql = oss.str();
    return execute(sql);
}
