//
// Created by skyinno on 2/14/16.
//

#ifndef CEPH_MAP_MESSAGEFACTORY_H
#define CEPH_MAP_MESSAGEFACTORY_H

#include <map>
#include <functional>
#include <memory>

#include "Message.h"

class MapMessageFactory
{
public:
    template<typename T>
    struct register_t
    {
        register_t(unsigned int key)
        {
            MapMessageFactory::get().map_.emplace(key, &register_t<T>::create);
        }

        template<typename... Args>
        register_t(unsigned int key, Args&&... args)
        {
            MapMessageFactory::get().map_.emplace(key, [&] { return new T(std::forward<Args>(args)...); });
            //MapMessageFactory::get().map_.emplace(key, [&] { return new T(args...); });
        }

        inline static Message * create() { return new T(); }
    };

    inline Message * createMessage(unsigned int key)
    {
#if 1
        if (map_.find(key) == map_.end())
            return nullptr;

        return map_[key]();
#else
        std::map<int, FuncPtr>::const_iterator it = map_.find(key);
        if (it == map_.end())
            return nullptr;

        return (it->second)();
#endif
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

    inline static MapMessageFactory & get()
    {
        static MapMessageFactory instance;
        return instance;
    }

private:
    MapMessageFactory() {};
    MapMessageFactory(const MapMessageFactory &) = delete;
    MapMessageFactory(MapMessageFactory &&) = delete;

    std::map<unsigned int, FuncPtr> map_;
};

#define MAP_REGISTER_MESSAGE_VNAME(T)       map_reg_msg_##T##_
#define MAP_REGISTER_MESSAGE(T, key, ...)   static MapMessageFactory::register_t<T> MAP_REGISTER_MESSAGE_VNAME(T)(key, ##__VA_ARGS__);

#include "Messages.h"

MAP_REGISTER_MESSAGE(CephMessage00, CEPH_MSG_ID_0);
MAP_REGISTER_MESSAGE(CephMessage01, CEPH_MSG_ID_1);
MAP_REGISTER_MESSAGE(CephMessage02, CEPH_MSG_ID_2);
MAP_REGISTER_MESSAGE(CephMessage03, CEPH_MSG_ID_3);
MAP_REGISTER_MESSAGE(CephMessage04, CEPH_MSG_ID_4);
MAP_REGISTER_MESSAGE(CephMessage05, CEPH_MSG_ID_5);
MAP_REGISTER_MESSAGE(CephMessage06, CEPH_MSG_ID_6);
MAP_REGISTER_MESSAGE(CephMessage07, CEPH_MSG_ID_7);
MAP_REGISTER_MESSAGE(CephMessage08, CEPH_MSG_ID_8);
MAP_REGISTER_MESSAGE(CephMessage09, CEPH_MSG_ID_9);
MAP_REGISTER_MESSAGE(CephMessage10, CEPH_MSG_ID_10);
//MAP_REGISTER_MESSAGE(CephMessage10, CEPH_MSG_ID_10, 1);

MAP_REGISTER_MESSAGE(CephMessage11, CEPH_MSG_ID_11);
MAP_REGISTER_MESSAGE(CephMessage12, CEPH_MSG_ID_12);
MAP_REGISTER_MESSAGE(CephMessage13, CEPH_MSG_ID_13);
MAP_REGISTER_MESSAGE(CephMessage14, CEPH_MSG_ID_14);
MAP_REGISTER_MESSAGE(CephMessage15, CEPH_MSG_ID_15);
MAP_REGISTER_MESSAGE(CephMessage16, CEPH_MSG_ID_16);
MAP_REGISTER_MESSAGE(CephMessage17, CEPH_MSG_ID_17);
MAP_REGISTER_MESSAGE(CephMessage18, CEPH_MSG_ID_18);
MAP_REGISTER_MESSAGE(CephMessage19, CEPH_MSG_ID_19);
MAP_REGISTER_MESSAGE(CephMessage20, CEPH_MSG_ID_20);
MAP_REGISTER_MESSAGE(CephMessage21, CEPH_MSG_ID_21);

#endif // CEPH_MAP_MESSAGEFACTORY_H
