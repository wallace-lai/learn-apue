// proto2.h

#ifndef PROTO_H_
#define PROTO_H_

#define KEYPATH "/etc/services"
#define KEYPROJ 'X'
#define PATHMAX 1024
#define DATAMAX 1024

typedef enum msg_type_e {
    MSG_TYPE_PATH = 1,
    MSG_TYPE_DATA,
    MSG_TYPE_EOT,
} msg_type_t;

typedef struct msg_path_s {
    char path[PATHMAX];
} msg_path_t;

typedef struct msg_c2s_s {
    long mtype;
    msg_path_t msg_path;
} msg_c2s_t;

typedef struct msg_data_s {
    char data[DATAMAX];
    unsigned len;
} msg_data_t;

typedef struct msg_eot_s {

} msg_eot_t;

typedef struct msg_s2c_s {
    long mtype;
    union {
        msg_data_t msg_data;
        msg_eot_t msg_eot;
    };
} msg_s2c_t;


#endif /* PROTO_H_ */