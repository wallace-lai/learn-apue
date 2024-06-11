// proto.h

#ifndef PROTO_H_
#define PROTO_H_

#include <stdint.h>

#define MTGROUP  "224.2.2.2"
#define RECVPORT "1989"
#define NAMESIZE 64

struct msg_s {
    uint8_t name[NAMESIZE];
    uint32_t math;
    uint32_t chinese;
} __attribute__((packed));

#endif /* PROTO_H_ */