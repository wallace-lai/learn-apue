// proto.h

#ifndef PROTO_H_
#define PROTO_H_

#define KEYPATH "/etc/services"
#define KEYPROJ 'G'

#define NAME_SIZE 16

typedef struct _msg {
    long mtype;
    char name[NAME_SIZE];
    int math;
    int chinese;
} msg;



#endif /* PROTO_H_ */