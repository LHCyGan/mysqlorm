#pragma once

#include <vector>
#include <mysql.h>

#include <utils/value.h>
using namespace lh::utils;

namespace lh {
	namespace myorm {

	template <typename T>
	class Batch {
	public:
		Batch();
		Batch(const Batch<T>& other) = delete;
		Batch(Batch<T>&& other) noexcept;
		~Batch();

		Batch& operator = (const Batch<T>& other) = delete;
		Batch& operator = (Batch<T>&& other) noexcept;

		void size(int size);
		int size() const;

		void total(int total);
		int total() const;

		void result(MYSQL_RES *res);

        class iterator {
            friend class Batch<T>;
        public:
            iterator() : m_batch(nullptr), m_next(nullptr) {}

            iterator(Batch<T> *batch) : m_batch(batch), m_next(batch->next()) {}

            iterator(const iterator& other) = delete;

            iterator(iterator&& other) noexcept {
                m_batch = other.m_batch;
                m_next = other.m_next;
                other.m_next = nullptr;
            }

            ~iterator() {
                if (m_next != nullptr) {
                    delete m_next;
                    m_next = nullptr;
                }
            }

            iterator& operator = (const iterator& other) = delete;
            iterator& operator = (iterator&& other) noexcept {
                m_batch = other.m_batch;
                m_next = other.m_next;
                other.m_next = nullptr;

                return *this;
            }

            bool operator != (const iterator& other) {
                return m_next != other.m_next;
            }

            iterator& operator++ () { // 前缀
                if (m_next != nullptr) {
                    delete m_next;
                    m_next = nullptr;
                }
                m_next = m_batch->next;
                return *this;
            }

            iterator& operator++ (int) {
                iterator it = std::move(*this);
                ++(*this);
                return it;
            }

            std::vector<T>& operator* () {
                return *m_next;
            }

            std::vector<T>* operator-> () {
                return m_next;
            }

            typename std::vector<T>::iterator begin() {
                return m_next->begin();
            }

            typename std::vector<T>::iterator end() {
                return m_next->end();
            }



        private:
            Batch<T> *m_batch;
            std::vector<T> *m_next;
        };

        iterator begin();
        iterator end();

    private:
        std::vector<T>* next();

    private:
        MYSQL_RES *m_res;
        int m_size;
        int m_total;
	};

    template <typename T>
    Batch<T>::Batch() : m_res(nullptr), m_size(0), m_total(0) {
    }

    template <typename T>
    Batch<T>::Batch(Batch<T>&& other) noexcept {
        m_res = other.m_res;
        m_size = other.m_size;
        m_total = other.m_total;
        other.m_res = nullptr;
    }

    template <typename T>
    Batch<T>::~Batch() {
        if (m_res != nullptr) {
            mysql_free_result(m_res);
            m_res = nullptr;
        }
    }

    template <typename T>
    Batch<T>& Batch<T>::operator=(Batch<T> &&other) noexcept {
        if (this == &other) {
            return *this;
        }
        m_res = other.m_res;
        m_size = other.m_size;
        m_total = other.m_total;
        other.m_res = nullptr;
        return *this;
    }

    template <typename T>
    void Batch<T>::size(int size) {
        m_size = size;
    }

    template <typename T>
    int Batch<T>::size() const {
        return m_size;
    }

    template <typename T>
    void Batch<T>::total(int total) {
        m_total = total;
    }

    template <typename T>
    int Batch<T>::total() const {
        return m_total;
    }

    template <typename T>
    void Batch<T>::result(MYSQL_RES *res) {
        m_res = res;
    }

    template <typename T>
    std::vector<T> *Batch<T>::next() {
        std::vector<T> *batch = nullptr;
        int fields = mysql_num_fields(m_res);
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(m_res)) != NULL) {
            if (batch == nullptr) {
                batch = new std::vector<T>();
            }
            T one;
            unsigned long *lengths = mysql_fetch_lengths(m_res);
            for (int i = 0;i < fields;i ++) {
                MYSQL_FIELD *field = mysql_fetch_field_direct(m_res, i);
                char *data = row[i];
                unsigned int length = lengths[i];
                Value v;
                v = string(data, length);
                if (IS_NUM(field->type)) {
                    if (length == 0)
                        v.type(Value::V_NULL);
                    else {
                        if (field->type == MYSQL_TYPE_DECIMAL || field->type == MYSQL_TYPE_FLOAT || field->type == MYSQL_TYPE_DOUBLE) {
                            v.type(Value::V_DOUBLE);
                        } else {
                            v.type(Value::V_INT);
                        }
                    }
                } else if (field->type == MYSQL_TYPE_NULL) {
                    v.type(Value::V_NULL);
                } else {
                    v.type(Value::V_STRING);
                }
                one[field->name] = v;
            }

            batch->emplace_back(one);
            int size = batch->size();
            if (size > m_size)
                break;
        }

        return batch;
    }

    template <typename T>
    typename Batch<T>::iterator Batch<T>::begin() {
        return iterator(this);
    }

    template <typename T>
    typename Batch<T>::iterator Batch<T>::end() {
        return iterator();
    }

    }
}