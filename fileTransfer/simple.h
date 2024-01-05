#ifndef _SIMPLE_H_
#define _SIMPLE_H_

#include "net.h"
#include "log.h"

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
    int size;
    char data[0];
} command_t;


char command_max() {
    return CHECKOUT;
}

char* command_data(command_t* cmd) {
    return cmd->data;
}
char* command_inner_data(command_t* cmd) {
    return cmd->data + 1;
}

TYPE command_type(command_t* cmd) {
    return (TYPE)cmd->data[0];
}

int command_size(int data_size) {
    return sizeof(TYPE) + data_size;
}

command_t* new_command(TYPE type, int len) {
    command_t* cmd = (command_t*)malloc(command_size(len) + sizeof(command_t));
    cmd->size = command_size(len);
    cmd->data[0] = type;
    return cmd;
}

command_t* new_command(TYPE type, int len, char* data) {
    command_t* cmd = new_command(type, len);
    cmd->size = command_size(len);
    memcpy(command_inner_data(cmd), data, len);
    return cmd;
}

void command_set_data(command_t* cmd, const char* data, int len) {
    memcpy(command_inner_data(cmd), data, len);
    cmd->size = command_size(len);
}

bool has_data(command_t* cmd) {
    return command_type(cmd) == READ;
}

void free_command(command_t* cmd) {
    free(cmd);
}

/**
 * @brief 写入命令
 * @param sock socket
 * @param cmd 命令
 * @return int 写入的字节数
 * @retval -1 写入失败
 * @retval >0 写入的字节数
*/
int write_command(int sock, command_t* cmd) {
    int bodyLen = cmd->size;
    int nlen = htonl(bodyLen);
    int hret = writen(sock, (char*)&nlen, sizeof(nlen));
    if (hret < 0) {
        Perrorf("write length error");
        return -1;
    }
    int ret = writen(sock, command_data(cmd), bodyLen);
    if (ret < 0) {
        Perrorf("write error");
        return -1;
    }
    return hret + ret;
}

/**
 * @brief 读取命令
 * @param sock socket
 * @param cmd 命令
 * @return int 读取的字节数
 * @retval -1 读取失败
 * @retval 0 socket关闭
 * @retval >0 读取的字节数
*/
int read_command(int sock, command_t** cmd) {
    int nlen = 0;
    int hret = readn(sock, (char*)&nlen, sizeof(nlen));
    if (hret < 0) {
        Perrorf("read error");
        return -1;
    }
    if (hret == 0) {
        *cmd = NULL;
        INFO << "socket quit";
        return 0;
    }
    int bodyLen = ntohl(nlen);
    *cmd = new_command(QUIT, bodyLen);
    int ret = readn(sock, command_data(*cmd), bodyLen);
    if (ret < 0) {
        Perrorf("read error");
        return -1;
    }
    if (ret == 0) {
        INFO << "socket quit";
        return 0;
    }
    return hret + ret;
}

void print_command(command_t* cmd) {
    if (cmd == NULL) {
        return;
    }
    if (command_type(cmd) > command_max()) {
        INFO << "unknown command";
        return;
    }
    char buffer[4096] = {0};
    memcpy(buffer, command_inner_data(cmd), cmd->size - sizeof(TYPE));
    buffer[cmd->size - sizeof(TYPE)] = '\0';
    DEBUG << "Command: " << TYPE_STR[command_type(cmd)] << " " << "Size: " << cmd->size << " " << "Data: \n" << buffer;
}

class command_guard {
public:
    command_guard(command_t* cmd) : cmd_(cmd) {}
    ~command_guard() {
        if (cmd_) {
            free_command(cmd_);
        }
    }
private:
    command_t* cmd_;
};

#endif