//
// Created by Dell on 2023/6/13.
//

#ifndef MYSQLORM_MODEL_H
#define MYSQLORM_MODEL_H

#include <iostream>
#include <map>
#include <vector>
#include <string>
using std::string;

#include <sstream>
using std::ostringstream;

#include <utils/value.h>
#include <utils/logger.h>
using namespace lh::utils;

#include <myorm/connection.h>
#include <myorm/database.h>

namespace lh {
    namespace myorm {

        template <typename T>
        class Model
        {
        public:
            Model();
            Model(Connection * conn);
            virtual ~Model();

            virtual string table() const = 0;
            virtual string primary_key() const;

            T & table_name(const string & table);

            template <typename P, typename ...Args>
            T & select(P head, Args... args);
            T & select() { return *dynamic_cast<T *>(this); }

            T & where(const string & cond);
            T & where(const string & field, const Value & value);
            T & where(const string & field, const string & op, const Value & value);
            T & where(const string & field, const string & op, const std::vector<Value> & values);
            T & where(const string & field, const string & op, const Value & min, const Value & max);

            T & alias(const string & alias);
            T & join(const string & table, const string & alias, const string & on, const string & type = "inner");
            T & group(const string & group);
            T & having(const string & having);
            T & order(const string & order);
            T & offset(int offset);
            T & limit(int limit);

            Value get(const string & field) const;
            void set(const string & field, const Value & value);

            Value operator () (const string & field);       // get
            Value & operator [] (const string & field);     // set

            string sql() const;
            string str() const;

            bool save();
            void remove();
            void update(const std::map<string, Value> & fields);
            void update(const T & row);
            void insert(const std::vector<T> & rows);
            void truncate();

            int count(const string & field = "*");
            double sum(const string & field);
            double min(const string & field);
            double max(const string & field);
            double average(const string & field);
            bool exists();
            Value scalar(const string & field);
            std::vector<Value> column(const string & field);

            T one();
            std::vector<T> all();

            Batch<T> batch(int size);

        protected:
            string build_select_sql() const;
            string build_join_sql() const;
            string build_where_sql() const;
            string build_other_sql() const;
            string build_simple_sql(const string & field, const string & func = "") const;

        protected:
            Connection * m_conn;
            string m_table;
            std::vector<string> m_select;
            std::map<string, Value> m_old_fields;
            std::map<string, Value> m_new_fields;
            std::map<string, Value> m_update;
            std::vector<string> m_where;
            std::vector<string> m_join;
            string m_alias;
            string m_order;
            string m_group;
            string m_having;
            int m_offset;
            int m_limit;
        };

        template <typename T>
        Model<T>::Model() : m_conn(nullptr)
        {
            m_offset = 0;
            m_limit = 0;
        }

        template <typename T>
        Model<T>::Model(Connection * conn) : m_conn(conn)
        {
            m_offset = 0;
            m_limit = 0;
        }

        template <typename T>
        Model<T>::~Model()
        {
        }

        template <typename T>
        string Model<T>::primary_key() const
        {
            return "id";
        }

        template <typename T>
        T & Model<T>::table_name(const string & table)
        {
            m_table = table;
            return *dynamic_cast<T *>(this);
        }

        template <typename T>
        template <typename P, typename ...Args>
        T & Model<T>::select(P head, Args... args)
        {
            m_select.push_back(head);
            return select(args...);
        }

        template <typename T>
        T & Model<T>::where(const string & cond)
        {
            m_where.push_back(cond);
            return *dynamic_cast<T *>(this);
        }

        template <typename T>
        T & Model<T>::where(const string & field, const Value & value)
        {
            return where(field, "=", value);
        }

        template <typename T>
        T & Model<T>::where(const string & field, const string & op, const Value & value)
        {
            ostringstream oss;
            oss << m_conn->quote(field) << " " << op << " ";
            if (value.is_string())
            {
                oss << "'" << m_conn->escape(value) << "'";
            }
            else
            {
                oss << (string)value;
            }
            m_where.push_back(oss.str());
            return *dynamic_cast<T *>(this);
        }

        template <typename T>
        T & Model<T>::where(const string & field, const string & op, const std::vector<Value> & values)
        {
            ostringstream oss;
            oss << m_conn->quote(field) << " " << op << " ";
            oss << "(";
            for (auto it = values.begin(); it != values.end(); it++)
            {
                if (it != values.begin())
                {
                    oss << ",";
                }
                if (it->is_string())
                {
                    oss << "'" << m_conn->escape(*it) << "'";
                }
                else
                {
                    oss << (string)(*it);
                }
            }
            oss << ")";
            m_where.push_back(oss.str());
            return *dynamic_cast<T *>(this);
        }

        template <typename T>
        T & Model<T>::where(const string & field, const string & op, const Value & min, const Value & max)
        {
            ostringstream oss;
            oss << m_conn->quote(field) << " " << op << " ";
            if (min.is_string())
            {
                oss << "'" << m_conn->escape(min) << "'";
            }
            else
            {
                oss << (string)min;
            }
            oss << " and ";
            if (max.is_string())
            {
                oss << "'" << m_conn->escape(max) << "'";
            }
            else
            {
                oss << (string)max;
            }
            m_where.push_back(oss.str());
            return *dynamic_cast<T *>(this);
        }

        template <typename T>
        T & Model<T>::alias(const string & alias)
        {
            m_alias = alias;
            return *dynamic_cast<T *>(this);
        }

        template <typename T>
        T & Model<T>::join(const string & table, const string & alias, const string & on, const string & type)
        {
            ostringstream oss;
            oss << type << " join " << m_conn->quote(table) << " as " << m_conn->quote(alias) << " on " << on;
            m_join.push_back(oss.str());
            return *dynamic_cast<T *>(this);
        }

        template <typename T>
        T & Model<T>::group(const string & group)
        {
            m_group = group;
            return *dynamic_cast<T *>(this);
        }

        template <typename T>
        T & Model<T>::having(const string & having)
        {
            m_having = having;
            return *dynamic_cast<T *>(this);
        }

        template <typename T>
        T & Model<T>::order(const string & order)
        {
            m_order = order;
            return *dynamic_cast<T *>(this);
        }

        template <typename T>
        T & Model<T>::offset(int offset)
        {
            m_offset = offset;
            return *dynamic_cast<T *>(this);
        }

        template <typename T>
        T & Model<T>::limit(int limit)
        {
            m_limit = limit;
            return *dynamic_cast<T *>(this);
        }

        template <typename T>
        Value Model<T>::get(const string & field) const
        {
            auto it = m_new_fields.find(field);
            if (it != m_new_fields.end())
            {
                return it->second;
            }
            it = m_old_fields.find(field);
            if (it != m_old_fields.end())
            {
                return it->second;
            }
            return Value();
        }

        template <typename T>
        void Model<T>::set(const string & field, const Value & value)
        {
            m_new_fields[field] = value;
        }

        template <typename T>
        Value Model<T>::operator () (const string & field)
        {
            return get(field);
        }

        template <typename T>
        Value & Model<T>::operator [] (const string & field)
        {
            return m_new_fields[field];
        }

        template <typename T>
        string Model<T>::sql() const
        {
            ostringstream oss;
            oss << build_select_sql();
            if (m_alias != "")
            {
                oss << " as " << m_conn->quote(m_alias);
            }
            oss << build_join_sql();
            oss << build_where_sql();
            oss << build_other_sql();
            return oss.str();
        }

        template <typename T>
        string Model<T>::str() const
        {
            std::map<string, Value> fields;
            for (auto it = m_old_fields.begin(); it != m_old_fields.end(); it++)
            {
                fields[it->first] = it->second;
            }
            for (auto it = m_new_fields.begin(); it != m_new_fields.end(); it++)
            {
                fields[it->first] = it->second;
            }
            ostringstream oss;
            oss << "{";
            for (auto it = fields.begin(); it != fields.end(); it++)
            {
                if (it == fields.begin())
                {
                    oss << "\"" << it->first << "\": ";
                }
                else
                {
                    oss << ", \"" << it->first << "\": ";
                }
                switch (it->second.type())
                {
                    case Value::V_NULL:
                        oss << "null";
                        break;
                    case Value::V_BOOL:
                        oss << string(it->second);
                        break;
                    case Value::V_INT:
                        oss << string(it->second);
                        break;
                    case Value::V_DOUBLE:
                        oss << string(it->second);
                        break;
                    case Value::V_STRING:
                        oss << "\"" << string(it->second) << "\"";
                        break;
                    default:
                        break;
                }
            }
            oss << "}";
            return oss.str();
        }

        template <typename T>
        string Model<T>::build_select_sql() const
        {
            ostringstream oss;
            oss << "select ";
            if (m_select.empty())
            {
                oss << "*";
            }
            else
            {
                for (auto it = m_select.begin(); it != m_select.end(); it++)
                {
                    if (it == m_select.begin())
                    {
                        oss << (*it);
                    }
                    else
                    {
                        oss << "," << (*it);
                    }
                }
            }
            oss << " from " << m_conn->quote(table());
            return oss.str();
        }

        template <typename T>
        string Model<T>::build_join_sql() const
        {
            if (m_join.empty())
            {
                return "";
            }
            ostringstream oss;
            oss << " ";
            for (auto it = m_join.begin(); it != m_join.end(); it++)
            {
                if (it == m_join.begin())
                {
                    oss << *it;
                }
                else
                {
                    oss << " " << *it;
                }
            }
            return oss.str();
        }

        template <typename T>
        string Model<T>::build_where_sql() const
        {
            if (m_where.empty())
            {
                return "";
            }
            ostringstream oss;
            oss << " where ";
            for (auto it = m_where.begin(); it != m_where.end(); it++)
            {
                if (it == m_where.begin())
                {
                    oss << (*it);
                }
                else
                {
                    oss << " and " << (*it);
                }
            }
            return oss.str();
        }

        template <typename T>
        string Model<T>::build_other_sql() const
        {
            ostringstream oss;
            if (!m_group.empty())
            {
                oss << " group by " << m_group;
            }
            if (!m_having.empty())
            {
                oss << " having " << m_having;
            }
            if (!m_order.empty())
            {
                oss << " order by " << m_order;
            }
            if (m_limit > 0)
            {
                if (m_offset > 0)
                {
                    oss << " limit " << m_limit << " offset " << m_offset;
                }
                else
                {
                    oss << " limit " << m_limit;
                }
            }
            return oss.str();
        }

        template <typename T>
        string Model<T>::build_simple_sql(const string & field, const string & func) const
        {
            ostringstream oss;
            oss << "select ";
            if (func.empty())
            {
                oss << m_conn->quote(field);
            }
            else
            {
                oss << func << "(" << m_conn->quote(field) << ")";
            }
            oss << " from " << m_conn->quote(table());
            if (!m_alias.empty())
            {
                oss << " as " << m_conn->quote(m_alias);
            }
            oss << build_join_sql();
            oss << build_where_sql();
            oss << build_other_sql();
            return oss.str();
        }

        template <typename T>
        bool Model<T>::save()
        {
            string pk = primary_key();
            if (m_old_fields.find(pk) == m_old_fields.end())
            {
                // insert
                if (m_new_fields.empty())
                {
                    return false;
                }
                ostringstream fields;
                ostringstream values;
                for (auto it = m_new_fields.begin(); it != m_new_fields.end(); it++)
                {
                    if (it != m_new_fields.begin())
                    {
                        fields << ",";
                        values << ",";
                    }
                    fields << m_conn->quote(it->first);
                    if (it->second.is_string())
                    {
                        values << "'" << m_conn->escape(it->second) << "'";
                    }
                    else
                    {
                        values << (string)(it->second);
                    }
                }
                ostringstream oss;
                oss << "insert into " << m_conn->quote(table()) << "(" << fields.str() << ") values(" << values.str() << ")";
                string sql = oss.str();
                int last_id = m_conn->insert(sql);
                m_new_fields[pk] = last_id;
                return true;
            }
            else
            {
                // update
                if (m_new_fields.empty())
                {
                    return false;
                }
                ostringstream update;
                for (auto it = m_new_fields.begin(); it != m_new_fields.end(); it++)
                {
                    if (it != m_new_fields.begin())
                    {
                        update << ", ";
                    }
                    update << m_conn->quote(it->first) << " = ";
                    if (it->second.is_string())
                    {
                        update << "'" << m_conn->escape(it->second) << "'";
                    }
                    else
                    {
                        update << (string)(it->second);
                    }
                }
                ostringstream oss;
                oss << "update " << m_conn->quote(table()) << " set " << update.str() << " where " << m_conn->quote(pk) << " = " << (string)m_old_fields[pk];
                string sql = oss.str();
                m_conn->execute(sql);
                return true;
            }
        }

        template <typename T>
        void Model<T>::remove()
        {
            ostringstream oss;
            oss << "delete from " << m_conn->quote(table());
            string pk = primary_key();
            if (m_old_fields.find(pk) == m_old_fields.end())
            {
                // oss << build_join_sql();
                oss << build_where_sql();
                oss << build_other_sql();
            }
            else
            {
                oss << " where " << m_conn->quote(pk) << " = " << (string)m_old_fields[pk];
            }
            string sql = oss.str();
            m_conn->execute(sql);
        }

        template <typename T>
        void Model<T>::update(const T & row)
        {
            update(row.m_new_fields);
        }

        template <typename T>
        void Model<T>::update(const std::map<string, Value> & fields)
        {
            if (fields.empty())
            {
                return;
            }
            ostringstream oss;
            oss << "update " << m_conn->quote(table()) << " set ";
            for (auto it = fields.begin(); it != fields.end(); it++)
            {
                if (it != fields.begin())
                {
                    oss << ", ";
                }
                oss << m_conn->quote(it->first) << " = ";
                if (it->second.is_string())
                {
                    oss << "'" << m_conn->escape(it->second) << "'";
                }
                else
                {
                    oss << (string)(it->second);
                }
            }
            // oss << build_join_sql();
            oss << build_where_sql();
            oss << build_other_sql();
            string sql = oss.str();
            m_conn->execute(sql);
        }

        template <typename T>
        void Model<T>::insert(const std::vector<T> & rows)
        {
            if (rows.empty())
            {
                return;
            }
            ostringstream oss;
            oss << "insert into " << m_conn->quote(table());
            oss << "(";
            for (auto it = rows[0].m_new_fields.begin(); it != rows[0].m_new_fields.end(); it++)
            {
                if (it != rows[0].m_new_fields.begin())
                {
                    oss << ",";
                }
                oss << m_conn->quote(it->first);
            }
            oss << ") values ";
            for (auto row = rows.begin(); row != rows.end(); row++)
            {
                if (row != rows.begin())
                {
                    oss << ",";
                }
                oss << "(";
                for (auto it = row->m_new_fields.begin(); it != row->m_new_fields.end(); it++)
                {
                    if (it != row->m_new_fields.begin())
                    {
                        oss << ",";
                    }
                    if (it->second.is_string())
                    {
                        oss << "'" << m_conn->escape(it->second) << "'";
                    }
                    else
                    {
                        oss << string(it->second);
                    }
                }
                oss << ")";
            }
            string sql = oss.str();
            m_conn->insert(sql);
        }

        template <typename T>
        void Model<T>::truncate()
        {
            ostringstream oss;
            oss << "truncate table " << m_conn->quote(table());
            string sql = oss.str();
            m_conn->execute(sql);
        }

        template <typename T>
        int Model<T>::count(const string & field)
        {
            ostringstream oss;
            oss << "select count(" << field << ") from " << m_conn->quote(table());
            if (!m_alias.empty())
            {
                oss << " as " << m_conn->quote(m_alias);
            }
            oss << build_join_sql();
            oss << build_where_sql();
            oss << build_other_sql();
            string sql = oss.str();
            auto one = m_conn->fetchone(sql);
            for (auto it = one.begin(); it != one.end(); it++)
            {
                return it->second;
            }
            return 0;
        }

        template <typename T>
        double Model<T>::sum(const string & field)
        {
            string sql = build_simple_sql(field, "sum");
            auto one = m_conn->fetchone(sql);
            for (auto it = one.begin(); it != one.end(); it++)
            {
                return it->second;
            }
            return 0;
        }

        template <typename T>
        double Model<T>::min(const string & field)
        {
            string sql = build_simple_sql(field, "min");
            auto one = m_conn->fetchone(sql);
            for (auto it = one.begin(); it != one.end(); it++)
            {
                return it->second;
            }
            return 0;
        }

        template <typename T>
        double Model<T>::max(const string & field)
        {
            string sql = build_simple_sql(field, "max");
            auto one = m_conn->fetchone(sql);
            for (auto it = one.begin(); it != one.end(); it++)
            {
                return it->second;
            }
            return 0;
        }

        template <typename T>
        double Model<T>::average(const string & field)
        {
            string sql = build_simple_sql(field, "avg");
            auto one = m_conn->fetchone(sql);
            for (auto it = one.begin(); it != one.end(); it++)
            {
                return it->second;
            }
            return 0;
        }

        template <typename T>
        bool Model<T>::exists()
        {
            ostringstream oss;
            oss << "select exists(" << sql() << ")";
            string sql = oss.str();

            int total = 0;
            auto one = m_conn->fetchone(sql);
            for (auto it = one.begin(); it != one.end(); it++)
            {
                total = it->second;
            }
            return total == 1;
        }

        template <typename T>
        Value Model<T>::scalar(const string & field)
        {
            m_limit = 1;
            string sql = build_simple_sql(field);
            auto one = m_conn->fetchone(sql);
            return one[field];
        }

        template <typename T>
        std::vector<Value> Model<T>::column(const string & field)
        {
            string sql = build_simple_sql(field);
            auto all = m_conn->fetchall(sql);
            std::vector<Value> result;
            for (auto it = all.begin(); it != all.end(); it++)
            {
                result.push_back((*it)[field]);
            }
            return result;
        }

        template <typename T>
        T Model<T>::one()
        {
            m_limit = 1;
            T one(m_conn);
            one.m_old_fields = m_conn->fetchone(sql());
            return one;
        }

        template <typename T>
        std::vector<T> Model<T>::all()
        {
            std::vector<T> all;
            std::vector<std::map<string, Value>> result = m_conn->fetchall(sql());
            for (auto it = result.begin(); it != result.end(); it++)
            {
                T one(m_conn);
                one.m_old_fields = (*it);
                all.push_back(one);
            }
            return all;
        }

        template <typename T>
        Batch<T> Model<T>::batch(int size)
        {
            Batch<T> batch = m_conn->batch<T>(sql());
            batch.size(size);
            return batch;
        }

    }}

#endif //MYSQLORM_MODEL_H
