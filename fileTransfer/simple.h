#ifndef _SIMPLE_H_
#define _SIMPLE_H_

#include <assert.h>
#include "log.h"
#include "net.h"

enum TYPE : signed char {
    QUIT = 0,
    LIST = 1,
    READ = 2,
    CHECKOUT = 3,
};

const char* TYPE_STR[] = {
    "QUIT",
    "LIST",
    "READ",
    "CHECKOUT",
};

typedef struct command {
    int32_t size;
    char data[0];
} command_t;

/**
 * @brief 获取命令最大值
 * @return char 命令最大值
 * @retval CHECKOUT
 */
static TYPE command_max() {
    return CHECKOUT;
}

/**
 * @brief 获取命令数据
 * @param cmd 命令
 * @return char* 命令数据, 包括type
 */
char* command_data(command_t* cmd) {
    assert(cmd != nullptr);
    return cmd->data;
}

/**
 * @brief 获取命令数据
 * @param cmd 命令
 * @return char* 命令数据, 不包括type
 */
char* command_inner_data(command_t* cmd) {
    assert(cmd != nullptr);
    return cmd->data + sizeof(TYPE);
}

/**
 * @brief 获取命令类型
 * @param cmd 命令
 * @return TYPE 命令类型
 */
TYPE command_type(command_t* cmd) {
    assert(cmd != nullptr);
    return (TYPE)cmd->data[0];
}

/**
 * @brief 获取命令长度
 * @param cmd 命令
 * @return int32_t 命令长度
 * @note 用于从data size 获得整个data需要的size
 */
static int32_t _data_size(int32_t data_size) {
    return sizeof(TYPE) + data_size;
}

/**
 * @brief 创建命令
 * @param type 命令类型
 * @param len 命令长度
 * @return command_t* 命令
 * @note 用于创建命令
 */
command_t* new_command(TYPE type, int len) {
    command_t* cmd = (command_t*)malloc(_data_size(len) + sizeof(command_t));
    assert(cmd != nullptr);
    cmd->size = _data_size(len);
    cmd->data[0] = type;
    return cmd;
}

/**
 * @brief 创建命令
 * @param type 命令类型
 * @param len 命令长度
 * @param data 命令数据
 * @return command_t* 命令
 * @note 用于创建命令
 */
command_t* new_command(TYPE type, int len, char* data) {
    command_t* cmd = new_command(type, len);
    memcpy(command_inner_data(cmd), data, len);
    return cmd;
}

/**
 * @brief 判断命令是否有数据
 * @param cmd 命令
 * @return bool 是否有数据
 * @retval true 有数据
 * @retval false 无数据
 * @note 用于判断命令是否有数据, 目前只支持READ
 */
bool has_data(command_t* cmd) {
    assert(cmd != nullptr);
    return command_type(cmd) == READ;
}

/**
 * @brief 释放命令
 * @param cmd 命令
 * @note 用于释放命令
 */
void free_command(command_t* cmd) {
    if (cmd == NULL) {
        return;
    }
    free(cmd);
}

/**
 * @brief 写入命令长度
 * @param sock socket
 * @param lenght 长度
 * @return int32_t 写入的字节数
 * @retval -1 写入失败
 * @retval 0 socket关闭
 * @retval >0 写入的字节数
 */
int32_t read_command_length(int sock, int& length) {
    int32_t nlen = 0;
    int32_t hret = readn(sock, (char*)&nlen, sizeof(nlen));
    if (hret < 0) {
        Perrorf("read error");
        return -1;
    } else if (hret == 0) {
        WARN << "socket quit";
        return 0;
    }
    length = ntohl(nlen);
    return hret;
}

/**
 * @brief 读取命令长度
 * @param sock socket
 * @param lenght 长度
 * @return int32_t 读取的字节数
 * @retval -1 读取失败
 * @retval 0 socket关闭
 * @retval >0 读取的字节数
 */
int32_t write_command_length(int sock, int32_t length) {
    int32_t nlen = htonl(length);
    int32_t ret = writen(sock, (char*)&nlen, sizeof(nlen));
    if (ret < 0) {
        Perrorf("write length error");
        return -1;
    } else if (ret == 0) {
        WARN << "socket quit";
        return 0;
    }
    DEBUG << "length:" << length << " nlen:" << nlen << " ret:" << ret;
    return ret;
}

/**
 * @brief 读取命令数据
 * @param sock socket
 * @param cmd 命令
 * @param bodyLen 数据长度
 * @return int32_t 读取的字节数
 * @retval -1 读取失败
 * @retval 0 socket关闭
 * @retval >0 读取的字节数
 */
int32_t read_command_body(int sock, command_t* cmd, int32_t bodyLen) {
    assert(cmd != nullptr);
    if (bodyLen <= 0) {
        WARN << "bodyLen le 0" << bodyLen;
        return -1;
    }
    int32_t ret = readn(sock, command_data(cmd), bodyLen);
    if (ret < 0) {
        Perrorf("read error");
        return -1;
    } else if (ret == 0) {
        WARN << "socket quit";
        return 0;
    }
    return ret;
}

/**
 * @brief 写入命令数据
 * @param sock socket
 * @param cmd 命令
 * @return int32_t 写入的字节数
 * @retval -1 写入失败
 * @retval 0 socket关闭
 * @retval >0 写入的字节数
 */
int32_t write_command_body(int sock, command_t* cmd) {
    assert(cmd != nullptr);
    if (cmd->size <= 0) {
        WARN << "cmd->size le 0" << cmd->size;
        return -1;
    }
    int ret = writen(sock, command_data(cmd), cmd->size);
    if (ret < 0) {
        Perrorf("write error");
        return -1;
    } else if (ret == 0) {
        WARN << "socket quit";
        return 0;
    }
    return ret;
}

void set_command_body(command_t* cmd, const char* data, int32_t len) {
    assert(cmd != nullptr);
    memcpy(command_inner_data(cmd), data, len);
    cmd->size = _data_size(len);
}

/**
 * @brief 写入命令
 * @param sock socket
 * @param cmd 命令
 * @return int32_t 写入的数据字节数
 * @retval -1 写入失败
 * @retval >0 写入的字节数
 */
int32_t write_command(int sock, command_t* cmd) {
    DEBUG << "write command";
    int hret = write_command_length(sock, cmd->size);
    if (hret < 0) {
        Perrorf("write length error");
        return -1;
    } else if (hret == 0) {
        WARN << "socket quit";
        return 0;
    }
    DEBUG << "write length:" << hret;
    if (cmd->size == 0) {
        return 0;
    }
    int ret = write_command_body(sock, cmd);
    if (ret < 0) {
        Perrorf("write body error");
        return -1;
    } else if (ret == 0) {
        WARN << "socket quit";
        return 0;
    }
    return ret;
}

/**
 * @brief 读取命令
 * @param sock socket
 * @param cmd 命令
 * @return int32_t 读取的数据子节数
 * @retval -1 读取失败 _cmd为NULL
 * @retval 0 socket关闭 cmd为NULL
 * @retval >0 读取的字节数 cmd不为NULL
 */
int32_t read_command(int sock, command_t** cmd) {
    int32_t bodyLen = 0;
    int ret = read_command_length(sock, bodyLen);
    if (ret < 0) {
        Perrorf("read length error");
        return -1;
    } else if (ret == 0) {
        WARN << "socket quit";
        return 0;
    }
    DEBUG << "bodyLen:" << bodyLen;
    *cmd = new_command(QUIT, bodyLen);
    ret = readn(sock, command_data(*cmd), bodyLen);
    if (ret < 0) {
        Perrorf("read error");
        return -1;
    } else if (ret == 0) {
        free_command(*cmd);
        cmd = NULL;
        INFO << "socket quit";
        return 0;
    }
    (*cmd)->size = bodyLen;
    return bodyLen;
}

#define print_command(cmd)                                                 \
    {                                                                      \
        if (cmd == NULL) {                                                 \
            return -1;                                                     \
        }                                                                  \
        if (command_type(cmd) > command_max()) {                           \
            INFO << "unknown command";                                     \
            return -1;                                                     \
        }                                                                  \
        char buffer[8192] = {0};                                           \
        memcpy(buffer, command_inner_data(cmd), cmd->size - sizeof(TYPE)); \
        buffer[cmd->size - sizeof(TYPE)] = '\0';                           \
        const char* type = TYPE_STR[command_type(cmd)];                    \
        DEBUG << "Command: [" << type << "] "                              \
              << "Size: [" << cmd->size << "] "                            \
              << "Data: [" << buffer << "]\n";                             \
    }

class command_guard {
   public:
    command_guard(command_t*& cmd) : cmd_(cmd) {}
    ~command_guard() {
        if (cmd_) {
            free_command(cmd_);
            cmd_ = NULL;
        }
    }

   private:
    command_t*& cmd_;
};

#endif