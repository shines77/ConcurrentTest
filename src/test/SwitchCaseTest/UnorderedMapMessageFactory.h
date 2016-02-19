//
// Created by skyinno on 2/14/2016.
//

#ifndef CEPH_UNORDERED_MAP_MESSAGEFACTORY_H
#define CEPH_UNORDERED_MAP_MESSAGEFACTORY_H

#include <unordered_map>
#include <functional>
#include <memory>

#include "Message.h"

class UnorderedMapMessageFactory
{
public:
    template<typename T>
    struct register_t
    {
        register_t(unsigned int key)
        {
            UnorderedMapMessageFactory::get().map_.emplace(key, &register_t<T>::create);
        }

        template <typename... Args>
        register_t(unsigned int key, Args... args)
        {
            UnorderedMapMessageFactory::get().map_.emplace(key, [&] { return new T(std::forward<Args>(args...)); });
        }

        inline static Message * create() { return new T(); }
    };

    inline Message * createMessage(unsigned int key)
    {
        std::unordered_map<unsigned int, FuncPtr>::const_iterator it = map_.find(key);
        if (it == map_.end())
            return nullptr;

        return (it->second)();
    }

    /*
    std::unique_ptr<Message> produce_unique(const std::string & key)
    {
        return std::unique_ptr<Message>(produce(key));
    }
    std::shared_ptr<Message> produce_shared(const std::string & key)
    {
        return std::shared_ptr<Message>(produce(key));
    }
    //*/

    typedef Message *(*FuncPtr)();

    inline static UnorderedMapMessageFactory & get()
    {
        static UnorderedMapMessageFactory instance;
        return instance;
    }

private:
    UnorderedMapMessageFactory() {};
    UnorderedMapMessageFactory(const UnorderedMapMessageFactory &) = delete;
    UnorderedMapMessageFactory(UnorderedMapMessageFactory &&) = delete;

    std::unordered_map<unsigned int, FuncPtr> map_;
};

#define UNORDERED_MAP_REGISTER_MESSAGE_VNAME(T)       unordered_map_reg_msg_##T##_
#define UNORDERED_MAP_REGISTER_MESSAGE(T, key, ...)   static UnorderedMapMessageFactory::register_t<T> UNORDERED_MAP_REGISTER_MESSAGE_VNAME(T)(key, ##__VA_ARGS__);

#include "Messages.h"

UNORDERED_MAP_REGISTER_MESSAGE(CephMessage00, CEPH_MSG_ID_0);
UNORDERED_MAP_REGISTER_MESSAGE(CephMessage01, CEPH_MSG_ID_1);
UNORDERED_MAP_REGISTER_MESSAGE(CephMessage02, CEPH_MSG_ID_2);
UNORDERED_MAP_REGISTER_MESSAGE(CephMessage03, CEPH_MSG_ID_3);
UNORDERED_MAP_REGISTER_MESSAGE(CephMessage04, CEPH_MSG_ID_4);
UNORDERED_MAP_REGISTER_MESSAGE(CephMessage05, CEPH_MSG_ID_5);
UNORDERED_MAP_REGISTER_MESSAGE(CephMessage06, CEPH_MSG_ID_6);
UNORDERED_MAP_REGISTER_MESSAGE(CephMessage07, CEPH_MSG_ID_7);
UNORDERED_MAP_REGISTER_MESSAGE(CephMessage08, CEPH_MSG_ID_8);
UNORDERED_MAP_REGISTER_MESSAGE(CephMessage09, CEPH_MSG_ID_9);

UNORDERED_MAP_REGISTER_MESSAGE(CephMessage10, CEPH_MSG_ID_10);
UNORDERED_MAP_REGISTER_MESSAGE(CephMessage11, CEPH_MSG_ID_11);
UNORDERED_MAP_REGISTER_MESSAGE(CephMessage12, CEPH_MSG_ID_12);
UNORDERED_MAP_REGISTER_MESSAGE(CephMessage13, CEPH_MSG_ID_13);
UNORDERED_MAP_REGISTER_MESSAGE(CephMessage14, CEPH_MSG_ID_14);
UNORDERED_MAP_REGISTER_MESSAGE(CephMessage15, CEPH_MSG_ID_15);
UNORDERED_MAP_REGISTER_MESSAGE(CephMessage16, CEPH_MSG_ID_16);
UNORDERED_MAP_REGISTER_MESSAGE(CephMessage17, CEPH_MSG_ID_17);
UNORDERED_MAP_REGISTER_MESSAGE(CephMessage18, CEPH_MSG_ID_18);
UNORDERED_MAP_REGISTER_MESSAGE(CephMessage19, CEPH_MSG_ID_19);

UNORDERED_MAP_REGISTER_MESSAGE(CephMessage20, CEPH_MSG_ID_20);
UNORDERED_MAP_REGISTER_MESSAGE(CephMessage21, CEPH_MSG_ID_21);
UNORDERED_MAP_REGISTER_MESSAGE(CephMessage22, CEPH_MSG_ID_22);
UNORDERED_MAP_REGISTER_MESSAGE(CephMessage23, CEPH_MSG_ID_23);
UNORDERED_MAP_REGISTER_MESSAGE(CephMessage24, CEPH_MSG_ID_24);
UNORDERED_MAP_REGISTER_MESSAGE(CephMessage25, CEPH_MSG_ID_25);
UNORDERED_MAP_REGISTER_MESSAGE(CephMessage26, CEPH_MSG_ID_26);
UNORDERED_MAP_REGISTER_MESSAGE(CephMessage27, CEPH_MSG_ID_27);
UNORDERED_MAP_REGISTER_MESSAGE(CephMessage28, CEPH_MSG_ID_28);
UNORDERED_MAP_REGISTER_MESSAGE(CephMessage29, CEPH_MSG_ID_29);

UNORDERED_MAP_REGISTER_MESSAGE(CephMessage30, CEPH_MSG_ID_30);
UNORDERED_MAP_REGISTER_MESSAGE(CephMessage31, CEPH_MSG_ID_31);
UNORDERED_MAP_REGISTER_MESSAGE(CephMessage32, CEPH_MSG_ID_32);
UNORDERED_MAP_REGISTER_MESSAGE(CephMessage33, CEPH_MSG_ID_33);
UNORDERED_MAP_REGISTER_MESSAGE(CephMessage34, CEPH_MSG_ID_34);
UNORDERED_MAP_REGISTER_MESSAGE(CephMessage35, CEPH_MSG_ID_35);
UNORDERED_MAP_REGISTER_MESSAGE(CephMessage36, CEPH_MSG_ID_36);
UNORDERED_MAP_REGISTER_MESSAGE(CephMessage37, CEPH_MSG_ID_37);
UNORDERED_MAP_REGISTER_MESSAGE(CephMessage38, CEPH_MSG_ID_38);
UNORDERED_MAP_REGISTER_MESSAGE(CephMessage39, CEPH_MSG_ID_39);

#endif // CEPH_UNORDERED_MAP_MESSAGEFACTORY_H
