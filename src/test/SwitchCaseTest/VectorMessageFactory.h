//
// Created by skyinno on 2/14/2016.
//

#ifndef CEPH_VECTOR_MESSAGEFACTORY_H
#define CEPH_VECTOR_MESSAGEFACTORY_H

#include <vector>
#include <functional>
#include <memory>
#include <algorithm>    // For std::max

#include "Message.h"

#include "FastQueue/basic/stddef.h"

class VectorMessageFactory
{
public:
    template<typename T>
    struct register_t
    {
        register_t(unsigned int key)
        {
            VectorMessageFactory & factory = VectorMessageFactory::get();
            factory.reserve_key(key);
            factory.vec_[key] = &register_t<T>::create;
        }

        template <typename... Args>
        register_t(unsigned int key, Args... args)
        {
            VectorMessageFactory & factory = VectorMessageFactory::get();
            factory.reserve_key(key);
            factory.vec_[key] = &register_t<T>::create<Args...>;
        }

        static inline Message * create() {
            return new T();
        }

        //template <typename U>
        //inline static Message * create() { return new T(U); }

        template <typename... Args>
        static inline Message * create(Args... args)
        {
            return new T(std::forward<Args>(args)...);
        }

        //template <typename... Args>
        //inline static Message * create<void, Args...>() { return new T(); }
    };

    __forceinline Message * createMessage(unsigned int key)
    {
        //assert(key < max_capacity_);
        assert(max_key_ < max_capacity_);
        if (key <= max_key_) {
          //FuncPtr createFunc = vec_[key];
          std::vector<FuncPtr>::const_iterator it = vec_.cbegin();
          FuncPtr createFunc = *(it + key);
          if (createFunc)
            return createFunc();
        }
        return nullptr;
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

    static __forceinline VectorMessageFactory & get()
    {
        static VectorMessageFactory instance;
        return instance;
    }

    void reserve_key(unsigned int key)
    {
        if (key > max_key_)
            max_key_ = key;
        if (key >= max_capacity_) {
            unsigned new_capacity = std::max<unsigned>(max_capacity_ * 2, 1);
            if (key >= new_capacity)
                new_capacity *= 2;
            vec_.reserve(new_capacity);
            max_capacity_ = new_capacity;
        }
    }

private:
    VectorMessageFactory() : max_key_(0), max_capacity_(128) {
        vec_.resize(max_capacity_);
        for (unsigned i = 0; i < vec_.size(); ++i) {
            vec_[i] = nullptr;
        }
    };
    VectorMessageFactory(const VectorMessageFactory &) = delete;
    VectorMessageFactory(VectorMessageFactory &&) = delete;

    std::vector<FuncPtr> vec_;
    unsigned max_key_;
    unsigned max_capacity_;
};

#define VECTOR_REGISTER_MESSAGE_VNAME(T)        vector_reg_msg_##T##_
#define VECTOR_REGISTER_MESSAGE(T, key, ...)    static VectorMessageFactory::register_t<T> VECTOR_REGISTER_MESSAGE_VNAME(T)(key, ##__VA_ARGS__);

#define VECTOR_REGISTER_MESSAGE_01(T, key, arg_type, ...)   \
                                                static VectorMessageFactory::register_t<T, arg_type> VECTOR_REGISTER_MESSAGE_VNAME(T)(key, ##__VA_ARGS__);

#include "Messages.h"

VECTOR_REGISTER_MESSAGE(CephMessage00, CEPH_MSG_ID_0);
VECTOR_REGISTER_MESSAGE(CephMessage01, CEPH_MSG_ID_1);
VECTOR_REGISTER_MESSAGE(CephMessage02, CEPH_MSG_ID_2);
VECTOR_REGISTER_MESSAGE(CephMessage03, CEPH_MSG_ID_3);
VECTOR_REGISTER_MESSAGE(CephMessage04, CEPH_MSG_ID_4);
VECTOR_REGISTER_MESSAGE(CephMessage05, CEPH_MSG_ID_5);
VECTOR_REGISTER_MESSAGE(CephMessage06, CEPH_MSG_ID_6);
VECTOR_REGISTER_MESSAGE(CephMessage07, CEPH_MSG_ID_7);
VECTOR_REGISTER_MESSAGE(CephMessage08, CEPH_MSG_ID_8);
VECTOR_REGISTER_MESSAGE(CephMessage09, CEPH_MSG_ID_9);

VECTOR_REGISTER_MESSAGE(CephMessage10, CEPH_MSG_ID_10);
//VECTOR_REGISTER_MESSAGE_01(CephMessage10, CEPH_MSG_ID_10, int, 1);
VECTOR_REGISTER_MESSAGE(CephMessage11, CEPH_MSG_ID_11);
VECTOR_REGISTER_MESSAGE(CephMessage12, CEPH_MSG_ID_12);
VECTOR_REGISTER_MESSAGE(CephMessage13, CEPH_MSG_ID_13);
VECTOR_REGISTER_MESSAGE(CephMessage14, CEPH_MSG_ID_14);
VECTOR_REGISTER_MESSAGE(CephMessage15, CEPH_MSG_ID_15);
VECTOR_REGISTER_MESSAGE(CephMessage16, CEPH_MSG_ID_16);
VECTOR_REGISTER_MESSAGE(CephMessage17, CEPH_MSG_ID_17);
VECTOR_REGISTER_MESSAGE(CephMessage18, CEPH_MSG_ID_18);
VECTOR_REGISTER_MESSAGE(CephMessage19, CEPH_MSG_ID_19);

VECTOR_REGISTER_MESSAGE(CephMessage20, CEPH_MSG_ID_20);
VECTOR_REGISTER_MESSAGE(CephMessage21, CEPH_MSG_ID_21);
VECTOR_REGISTER_MESSAGE(CephMessage22, CEPH_MSG_ID_22);
VECTOR_REGISTER_MESSAGE(CephMessage23, CEPH_MSG_ID_23);
VECTOR_REGISTER_MESSAGE(CephMessage24, CEPH_MSG_ID_24);
VECTOR_REGISTER_MESSAGE(CephMessage25, CEPH_MSG_ID_25);
VECTOR_REGISTER_MESSAGE(CephMessage26, CEPH_MSG_ID_26);
VECTOR_REGISTER_MESSAGE(CephMessage27, CEPH_MSG_ID_27);
VECTOR_REGISTER_MESSAGE(CephMessage28, CEPH_MSG_ID_28);
VECTOR_REGISTER_MESSAGE(CephMessage29, CEPH_MSG_ID_29);

VECTOR_REGISTER_MESSAGE(CephMessage30, CEPH_MSG_ID_30);
VECTOR_REGISTER_MESSAGE(CephMessage31, CEPH_MSG_ID_31);
VECTOR_REGISTER_MESSAGE(CephMessage32, CEPH_MSG_ID_32);
VECTOR_REGISTER_MESSAGE(CephMessage33, CEPH_MSG_ID_33);
VECTOR_REGISTER_MESSAGE(CephMessage34, CEPH_MSG_ID_34);
VECTOR_REGISTER_MESSAGE(CephMessage35, CEPH_MSG_ID_35);
VECTOR_REGISTER_MESSAGE(CephMessage36, CEPH_MSG_ID_36);
VECTOR_REGISTER_MESSAGE(CephMessage37, CEPH_MSG_ID_37);
VECTOR_REGISTER_MESSAGE(CephMessage38, CEPH_MSG_ID_38);
VECTOR_REGISTER_MESSAGE(CephMessage39, CEPH_MSG_ID_39);

#endif // CEPH_VECTOR_MESSAGEFACTORY_H
