
#ifndef CEPH_MESSAGE_H
#define CEPH_MESSAGE_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

struct Message {
    int id;
    int type;
    int tag;
    int length;
    void * data;

    Message() : id(0), type(0), tag(0), length(0), data(nullptr) {
    }

    Message(int _type) : id(0), type(_type), tag(0), length(0), data(nullptr) {
    }

    int getId() const {
        return id;
    }

    int getType() const {
        return type;
    }
};

#endif // CEPH_MESSAGE_H
