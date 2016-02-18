
#ifndef CEPH_MESSAGES_H
#define CEPH_MESSAGES_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "Message.h"

enum {
    CEPH_MSG_FIRST = 0,
    CEPH_MSG_ID_0 = 20,
    CEPH_MSG_ID_VALID_FIRST = CEPH_MSG_ID_0,
    CEPH_MSG_ID_1,
    CEPH_MSG_ID_2,
    CEPH_MSG_ID_3,
    CEPH_MSG_ID_4,
    CEPH_MSG_ID_5,
    CEPH_MSG_ID_6,
    CEPH_MSG_ID_7,
    CEPH_MSG_ID_8,
    CEPH_MSG_ID_9,
    CEPH_MSG_ID_10,
    CEPH_MSG_ID_11,
    CEPH_MSG_ID_12,
    CEPH_MSG_ID_13,
    CEPH_MSG_ID_14,
    CEPH_MSG_ID_15,
    CEPH_MSG_ID_16,
    CEPH_MSG_ID_17,
    CEPH_MSG_ID_18,
    CEPH_MSG_ID_19,
    CEPH_MSG_ID_20,
    CEPH_MSG_ID_21,
    CEPH_MSG_ID_VALID_LAST = CEPH_MSG_ID_21,
    CEPH_MSG_MAX = 200,
    CEPH_MSG_LAST = CEPH_MSG_MAX + 1
};

struct CephMessage00 : public Message {
    char reserve01;
    char reserve02;

    CephMessage00() : Message(CEPH_MSG_ID_0) {
    }
};

struct CephMessage01 : public Message {
    int  reserve01;

    CephMessage01() : Message(CEPH_MSG_ID_1) {
    }
};

struct CephMessage02 : public Message {
    int  reserve01;
    char reserve02;

    CephMessage02() : Message(CEPH_MSG_ID_2) {
    }
};

struct CephMessage03 : public Message {
    int  reserve01;
    char reserve02;
    char reserve03;

    CephMessage03() : Message(CEPH_MSG_ID_3) {
    }
};

struct CephMessage04 : public Message {
    int  reserve01;
    int  reserve02;

    CephMessage04() : Message(CEPH_MSG_ID_4) {
    }
};

struct CephMessage05 : public Message {
    int  reserve01;
    char reserve02;
    int  reserve03;

    CephMessage05() : Message(CEPH_MSG_ID_5) {
    }
};

struct CephMessage06 : public Message {
    int  reserve01;
    int  reserve02;
    char reserve03;

    CephMessage06() : Message(CEPH_MSG_ID_6) {
    }
};

struct CephMessage07 : public Message {
    int  reserve01;
    int  reserve02;
    char reserve03;
    char reserve04;

    CephMessage07() : Message(CEPH_MSG_ID_7) {
    }
};

struct CephMessage08 : public Message {
    int  reserve01;
    int  reserve02;
    int  reserve03;

    CephMessage08() : Message(CEPH_MSG_ID_8) {
    }
};

struct CephMessage09 : public Message {
    int  reserve01;
    int  reserve02;

    CephMessage09() : Message(CEPH_MSG_ID_9) {
    }
};

struct CephMessage10 : public Message {
    int  reserve01;
    int  reserve02;
    int  reserve03;
    int  reserve04;

    CephMessage10() : Message(CEPH_MSG_ID_10) {
    }

    CephMessage10(int i) : Message(CEPH_MSG_ID_10) {
        id = i;
    }
};

struct CephMessage11 : public Message {
    int  reserve01;
    int  reserve02;
    int  reserve03;
    char reserve04;

    CephMessage11() : Message(CEPH_MSG_ID_11) {
    }
};

struct CephMessage12 : public Message {
    int  reserve01;
    char reserve02;
    int  reserve03;
    int  reserve04;

    CephMessage12() : Message(CEPH_MSG_ID_12) {
    }
};

struct CephMessage13 : public Message {
    int  reserve01;
    char reserve02;
    char reserve03;
    int  reserve04;

    CephMessage13() : Message(CEPH_MSG_ID_13) {
    }
};

struct CephMessage14 : public Message {
    int  reserve01;
    char reserve02;
    char reserve03;
    char reserve04;

    CephMessage14() : Message(CEPH_MSG_ID_14) {
    }
};

struct CephMessage15 : public Message {
    char reserve01;
    int  reserve02;
    int  reserve03;
    int  reserve04;    

    CephMessage15() : Message(CEPH_MSG_ID_15) {
    }
};

struct CephMessage16 : public Message {
    char reserve01;
    char reserve02;
    char reserve03;
    char reserve04;

    CephMessage16() : Message(CEPH_MSG_ID_16) {
    }
};

struct CephMessage17 : public Message {
    char reserve01;
    char reserve02;
    char reserve03;
    char reserve04;
    int  reserve05;

    CephMessage17() : Message(CEPH_MSG_ID_17) {
    }
};

struct CephMessage18 : public Message {
    int  reserve01;
    char reserve02;
    char reserve03;
    char reserve04;
    int  reserve05;

    CephMessage18() : Message(CEPH_MSG_ID_18) {
    }
};

struct CephMessage19 : public Message {
    int  reserve05;
    char reserve01;
    char reserve02;
    char reserve03;
    char reserve04;

    CephMessage19() : Message(CEPH_MSG_ID_19) {
    }
};

struct CephMessage20 : public Message {
    char reserve01;
    char reserve02;
    int  reserve03;
    char reserve04;
    int  reserve05;

    CephMessage20() : Message(CEPH_MSG_ID_20) {
    }
};

struct CephMessage21 : public Message {
    int  reserve01;
    char reserve02;
    int  reserve03;
    char reserve04;
    int  reserve05;

    CephMessage21() : Message(CEPH_MSG_ID_21) {
    }
};

#endif // CEPH_MESSAGES_H
