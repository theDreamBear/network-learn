#ifndef __LOG_LOG_H__
#define __LOG_LOG_H__

#include <iostream>
#include <string.h>
#include <string>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sstream>
#include <math.h>

const int LOG_LEVEL_DEBUG = 0;
const int LOG_LEVEL_INFO = 1;
const int LOG_LEVEL_WARN = 2;
const int LOG_LEVEL_ERROR = 3;
const int LOG_LEVEL_FATAL = 4;

const char* LOG_LEVEL_STR[] = {
    "DEBUG",
    "INFO",
    "WARN",
    "ERROR",
    "FATAL",
};

// 日志都输入到标准输出
// 日志格式：[时间 日志级别 行号 函数名] 日志内容
// 日志级别：DEBUG INFO WARN ERROR FATAL

const int g_log_level = LOG_LEVEL_INFO;

class Log {
public:
    Log(const char* file, int line, const char* func, int level) {
        // file去掉路径
        const char* pos = strrchr(file, '/');
        if (pos != NULL) {
            _file = pos + 1;
        } else {
            _file = file;
        }
        //_file = file;
        _line = line;
        _func = func;
        _level = level;
        _fd = STDOUT_FILENO;
        _is_open = false;
    }

    Log(const char* file, int line, const char* func, int level, const char* path) {
        _file = file;
        _line = line;
        _func = func;
        _level = level;
        _fd = open(path, O_CREAT | O_WRONLY | O_APPEND, 0664);
        _is_open = true;
    }

    ~Log() {
       flush();
    }

    std::ostream& log() {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        char time_buf[64] = {0};
        // 月-日 时:分:秒
        int ret = strftime(time_buf, sizeof(time_buf), "%m-%d %H:%M:%S", localtime(&tv.tv_sec));
        if (ret == 0) {
            std::cerr << "strftime error" << std::endl;
            return _stream;
        }
        // 毫秒
        snprintf(time_buf + ret, sizeof(time_buf) - ret, ":%03d", ceil(tv.tv_usec / 1000.0));
        char buf[1024] = {0};
        snprintf(buf, sizeof(buf), "[%s %s file=%s:%d func=%s] ",LOG_LEVEL_STR[_level], time_buf, _file, _line, _func);
        _stream << buf;
        return _stream;
    }

    void flush() {
        _stream << std::endl;
        if (_level < g_log_level) {
            _stream.str("");
            return;
        }
        if (_is_open) {
            write(_fd, _stream.str().c_str(), _stream.str().size());
        } else {
            std::cout << _stream.str();
        }
        _stream.str("");
    }

private:
    const char* _file;
    int _line;
    const char* _func;
    int _level;
    int _fd;
    bool _is_open;
    std::stringstream _stream;
};

#define LOG(level) Log(__FILE__, __LINE__, __func__, level).log()

#define DEBUG Log(__FILE__, __LINE__, __func__, LOG_LEVEL_DEBUG).log()
#define INFO Log(__FILE__, __LINE__, __func__, LOG_LEVEL_INFO).log()
#define WARN Log(__FILE__, __LINE__, __func__, LOG_LEVEL_WARN).log()
#define ERROR Log(__FILE__, __LINE__, __func__, LOG_LEVEL_ERROR).log()
#define FATAL Log(__FILE__, __LINE__, __func__, LOG_LEVEL_FATAL).log()
#endif // !__LOG_LOG_H__
