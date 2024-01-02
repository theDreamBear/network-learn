//
// Created by 倪超 on 2023/12/30.
//
#ifndef CALCULATOR_PROTOCAL_H
#define CALCULATOR_PROTOCAL_H

#include <vector>
#include <string>

struct Data {
    int numSize;
    std::vector<int> nums;
    char op;
};

std::pair<int, const char *> isValid(const Data &data, int bufferSize) {
    if (data.numSize != data.nums.size()) {
        return {-1, "numSize != nums.size()"};
    }
    switch (data.op) {
        case '+':
        case '-':
        case '*':
        case '/':
            break;
        default:
            std::cout << "op is invalid: " << data.op << std::endl;
            return {-1, "op is not valid"};
    }
    if (bufferSize < (sizeof(char) + sizeof(int) * (data.numSize + 1))) {
        return {-1, "bufferSize is too small"};
    }
    return {0, nullptr};
}

// 大端序
char *encodeInt(int value, char *buffer) {
    int pos = 0;
    buffer[pos++] = (value >> 24) & 0xFF;
    buffer[pos++] = (value >> 16) & 0xFF;
    buffer[pos++] = (value >> 8) & 0xFF;
    buffer[pos++] = value & 0xFF;
    return buffer + pos;
}

// 大端序
char *encodeChar(char value, char *buffer) {
    buffer[0] = value;
    return buffer + 1;
}

// 大端序
char *decodeInt(char *buffer, int &value) {
    int pos = 0;
    value |= (buffer[pos++] & 0xFF) << 24;
    value |= (buffer[pos++] & 0xFF) << 16;
    value |= (buffer[pos++] & 0xFF) << 8;
    value |= buffer[pos++] & 0xFF;
    //printf("readInt: %02x %02x %02x %02x\n", buffer[0], buffer[1], buffer[2], buffer[3]);
    return buffer + pos;
}

// 大端序
char *decodeChar(char *buffer, char &value) {
    value = buffer[0];
    return buffer + 1;
}


std::pair<int, const char *> encode(const Data &data, char *buffer, int bufferSize) {
    auto valid = isValid(data, bufferSize);
    if (valid.first != 0) {
        return valid;
    }
    char *pos = buffer;
    // encode numSize
    pos = encodeInt(data.numSize, pos);
    for (auto v: data.nums) {
        pos = encodeInt(v, pos);
    }
    pos = encodeChar(data.op, pos);
    return {pos - buffer, nullptr};
}

std::pair<int, const char *> decode(char *buffer, int bufferSize, Data &data) {
    // 至少一个数字，一个运算符
    if (bufferSize < (sizeof(char) + sizeof(int))) {
        return {-1, "bufferSize is too small"};
    }
    char *pos = buffer;
    pos = decodeInt(pos, data.numSize);
    if (bufferSize < (sizeof(char) + sizeof(int) * (data.numSize + 1))) {
        return {-1, "bufferSize is too small"};
    }
    for (int i = 0; i < data.numSize; ++i) {
        pos = decodeInt(pos, data.nums[i]);
    }
    pos = decodeChar(pos, data.op);
    return {pos - buffer, nullptr};
}

struct Conn {
    int conn_fd;
    char *buffer;
    int capacity;
    int readPos;
    int writePos;
    bool isClosed;

    Conn(int fd) : conn_fd(fd), readPos(0), writePos(0), isClosed(false) {
        buffer = new char[1024];
        capacity = 1024;
    }

    int leftSize() {
        return capacity - writePos;
    }

    void makeRoomFor(int need_size) {
        if (leftSize() < need_size) {
            // 扩容
            // 1. capacity <= 1k,  * 2 + 64
            // 2. capacity <= 2k,  * 2
            // 3. capacity <= 4k,  * 1.5
            // 4. capacity <= 8k,  * 1.25
            // 5. capacity > 8k, + 1k
            if (capacity <= 1024) {
                capacity = capacity * 2 + 64;
            } else if (capacity <= 2 * 1024) {
                capacity = capacity * 2;
            } else if (capacity <= 4 * 1024) {
                capacity = capacity * 3 / 2.0;
            } else if (capacity <= 8 * 1024) {
                capacity = capacity * 5 / 4.0;
            } else {
                capacity += 1024;
            }
            char *new_buffer = new char[capacity];
            memcpy(new_buffer, buffer + readPos, writePos - readPos);
            writePos -= readPos;
            readPos = 0;
            delete[] buffer;
            buffer = new_buffer;
        }
    }

    int read_n(int n) {
        int rd = 0;
        while (rd < n) {
            int n = read(conn_fd, buffer + writePos, capacity - writePos);
            if (n == -1) {
                return -1;
            }
            writePos += n;
            rd += n;
        }
        return 0;
    }


    int makeRoom(int size) {
        int need_read = readPos + size - writePos;
        if (need_read > 0) {
            makeRoomFor(need_read);
            int rd = read_n(need_read);
            if (rd == -1) {
                return -1;
            }
        }
        return 0;
    }

    int readInt(int &value) {
        if (makeRoom(sizeof(int)) == -1) {
            return -1;
        }
        value = 0;
        value |= (buffer[readPos++] & 0xFF) << 24;
        value |= (buffer[readPos++] & 0xFF) << 16;
        value |= (buffer[readPos++] & 0xFF) << 8;
        value |= buffer[readPos++] & 0xFF;
        return 0;
    }

    int readChar(char &value) {
        if (makeRoom(sizeof(char)) == -1) {
            return -1;
        }
        value = buffer[readPos++];
        return 0;
    }

    int writeInt(int value) {
        char writeBuffer[16];
        // 大端序
        int pos = 0;
        writeBuffer[pos++] = (value >> 24) & 0xFF;
        writeBuffer[pos++] = (value >> 16) & 0xFF;
        writeBuffer[pos++] = (value >> 8) & 0xFF;
        writeBuffer[pos++] = value & 0xFF;
        int wn = 0;
        while (wn < sizeof(value)) {
            int n = write(conn_fd, writeBuffer + wn, sizeof(value) - wn);
            if (n == -1) {
                return -1;
            }
            wn += n;
        }
        // 打印输出的四个字节
        // printf("writeInt: %02x %02x %02x %02x\n", writeBuffer[0], writeBuffer[1], writeBuffer[2], writeBuffer[3]);
        return wn;
    }
};

int readData(Conn &conn, Data &data) {
    int rd = conn.readInt(data.numSize);
    if (rd == -1) {
        std::cout << "read numSize error" << std::endl;
        return -1;
    }
    if (data.numSize < 0 || data.numSize > 2) {
        std::cout << "numSize is invalid:" << data.numSize << std::endl;
        return -1;
    }
    data.nums.resize(data.numSize);
    for (int i = 0; i < data.numSize; ++i) {
        rd = conn.readInt(data.nums[i]);
        if (rd == -1) {
            std::cout << "read num error" << std::endl;
            return -1;
        }
    }
    // 读取op
    rd = conn.readChar(data.op);
    if (rd == -1) {
        std::cout << "read op error" << std::endl;
        return -1;
    }
    return 0;
}

int writeData(int fd, const Data &data) {
    char buffer[1024];
    auto[size, msg] = encode(data, buffer, 1024);
    if (msg != nullptr) {
        std::cout << "encode error: " << msg << std::endl;
        return -1;
    }
    // 发送数据
    int wn = 0;
    while (wn < size) {
        int n = write(fd, buffer + wn, size - wn);
        if (n == -1) {
            return -1;
        }
        wn += n;
    }
    return 0;
}

#endif //CALCULATOR_PROTOCAL_H
