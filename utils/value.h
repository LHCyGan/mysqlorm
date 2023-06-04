#pragma once

#include <string>
using std::string;

namespace lh {
	namespace utils {

		class Value {
		public:
			enum Type {
				V_NULL = 0,
				V_BOOL,
				V_INT,
				V_DOYUBLE,
				V_STRING
			};

			Value();
			Value(bool value);
			Value(int value);
			Value(double value);
			Value(const char* value);
			Value(const string& value);
			~Value();

			Type type() const;
			void type(Type type);
			bool is_null() const;
			bool is_int() const;
			bool is_double() const;
			bool is_string() const;

			Value& operator=(bool value);
			Value& operator=(int value);
			Value& operator=(double value);
			Value& operator=(const char* value);
			Value& operator=(const string& value);
			Value& operator=(const Value& value);

			bool operator== (const Value& other);
			bool operator== (const Value& other);

			operator bool();
			operator int();
			operator double();
			operator string();
			operator string() const;

		private:
			Type m_type;
			string m_value;
		};

	}
}