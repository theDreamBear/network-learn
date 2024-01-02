//
// Created by 倪超 on 2023/12/30.
//

#ifndef SIMPLE_TCP_PROTOCOL_SIMPLE_PROTOCOL_H
#define SIMPLE_TCP_PROTOCOL_SIMPLE_PROTOCOL_H
int makeHeader(void *buffer, int length) {
    if (length == 0) {
        ((char *) buffer)[0] = 0;
        return 1;
    }
    int i = 0;
    for (; i < length; i++) {
        // 小端存储
        unsigned char byte = length & 0x7f;
        length >>= 7;
        if (length) {
            byte |= 0x80;
        }
        ((unsigned char *) buffer)[i] = byte;
    }
    return i;
}

#endif //SIMPLE_TCP_PROTOCOL_SIMPLE_PROTOCOL_H
