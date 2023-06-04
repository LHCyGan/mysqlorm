#pragma once

#include <string>
using std::string;

#include <fstream>
using std::ofstream;

namespace lh {
    namespace utils {

#define log_debug(format, ...) \
    Logger::instance()->log(Logger::LOG_DEBUG, __FILE__, __LINE__, format, ##__VA_ARGS__)

#define log_info(format, ...) \
    Logger::instance()->log(Logger::LOG_INFO, __FILE__, __LINE__, format, ##__VA_ARGS__)

#define log_warn(format, ...) \
    Logger::instance()->log(Logger::LOG_WARN, __FILE__, __LINE__, format, ##__VA_ARGS__)

#define log_error(format, ...) \
    Logger::instance()->log(Logger::LOG_ERROR, __FILE__, __LINE__, format, ##__VA_ARGS__)

#define log_fatal(format, ...) \
    Logger::instance()->log(Logger::LOG_FATAL, __FILE__, __LINE__, format, ##__VA_ARGS__)

        class Logger
        {
        public:
            enum Level
            {
                LOG_DEBUG = 0,
                LOG_INFO,
                LOG_WARN,
                LOG_ERROR,
                LOG_FATAL,
                LOG_COUNT
            };

            static Logger* instance();
            void open(const string& filename);
            void close();
            void log(Level level, const char* file, int line, const char* format, ...);
            void max(int bytes);
            void level(int level);

        private:
            Logger();
            ~Logger();
            void rotate();

        private:
            string m_filename;
            ofstream m_fout;
            int m_max;
            int m_len;
            int m_level;
            static const char* s_level[LOG_COUNT];
            static Logger* m_instance;
        };

    }
}
