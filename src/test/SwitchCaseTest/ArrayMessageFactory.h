//
// Created by skyinno on 2/14/16.
//

#ifndef CEPH_ARRAY_MESSAGEFACTORY_H
#define CEPH_ARRAY_MESSAGEFACTORY_H

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <vector>
#include <functional>
#include <memory>
#include <iostream>     // For std::cout
#include <algorithm>    // For std::max

#include "Message.h"

#include "FastQueue/basic/stddef.h"

class ArrayMessageFactory
{
public:
    template<typename T>
    struct register_t
    {
        register_t(unsigned int key)
        {
            ArrayMessageFactory & factory = ArrayMessageFactory::get();
            factory.reserve(key);
            if (factory.array_)
                factory.array_[key] = &register_t<T>::create;
        }

        template <typename... Args>
        register_t(unsigned int key, Args... args)
        {
            ArrayMessageFactory & factory = ArrayMessageFactory::get();
            factory.reserve(key);
            if (factory.array_)
                factory.array_[key] = &register_t<T>::create<Args...>;
        }

        static inline Message * create() {
            return new T();
        }

        //template <typename U>
        //inline static Message * create() { return new T(U); }

        template <typename... Args>
        static inline Message * create(Args... args)
        {
            return new T(std::forward<Args...>(args...));
        }

        //template <typename... Args>
        //inline static Message * create<void, Args...>() { return new T(); }
    };

    inline Message * createMessage(unsigned int key)
    {
        //assert(key < max_capacity_);
        assert(max_key_ < max_capacity_);
        if (key <= max_key_) {
            assert(array_ != nullptr);
            //FuncPtr createFunc = array_[key];
            FuncPtr createFunc = *(array_ + key);
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

    static inline ArrayMessageFactory & get()
    {
        static ArrayMessageFactory instance;
        return instance;
    }

    void _reserve(unsigned capacity) {
        if (capacity > max_capacity_) {
            FuncPtr * new_array = (FuncPtr *)std::malloc(sizeof(FuncPtr) * capacity);
            if (new_array) {
                if (array_) {
                    if (new_array != array_) {
                        for (unsigned i = 0; i < max_capacity_; ++i) {
                            new_array[i] = array_[i];
                        }
                    }
                    std::free(array_);
                }
                array_ = new_array;
                max_capacity_ = capacity;
            }
        }
    }

    void reserve(unsigned int key)
    {
        if (key > max_key_) {
            max_key_ = key;
            if (key >= max_capacity_) {
                unsigned new_capacity = std::max<unsigned>(max_capacity_ * 2, 1);
                if (key >= new_capacity)
                    new_capacity *= 2;
                this->_reserve(new_capacity);
            }
        }
    }

    void resize(unsigned capacity, bool clear = true) {
        if (capacity > max_capacity_) {
            FuncPtr * new_array;
            if (array_)
                new_array = (FuncPtr *)std::realloc(array_, sizeof(FuncPtr) * capacity);
            else
                new_array = (FuncPtr *)std::malloc(sizeof(FuncPtr) * capacity);
            if (new_array) {
                if (clear)
                    ::memset(new_array, 0, sizeof(FuncPtr) * capacity);
                array_ = new_array;
                max_capacity_ = capacity;
            }
            else {
                array_ = nullptr;
                max_capacity_ = 0;
            }
        }
    }

protected:
    void force_resize(unsigned capacity, bool clear = true) {
        if (array_) {
            std::free(array_);
            array_ = nullptr;
        }
        FuncPtr * new_array = (FuncPtr *)std::malloc(sizeof(FuncPtr) * capacity);
        if (new_array) {
            if (clear)
                ::memset(new_array, 0, sizeof(FuncPtr) * capacity);
            array_ = new_array;
            max_capacity_ = capacity;
        }
    }

private:
    ArrayMessageFactory() : max_key_(0), max_capacity_(0), array_(nullptr) {
        this->force_resize(128, true);
#if 0
        for (unsigned i = 0; i < max_capacity_; ++i) {
            array_[i] = nullptr;
        }
#endif
    };
    ArrayMessageFactory(const ArrayMessageFactory &) = delete;
    ArrayMessageFactory(ArrayMessageFactory &&) = delete;

    FuncPtr * array_;
    unsigned max_key_;
    unsigned max_capacity_;
};

#define ARRAY_REGISTER_MESSAGE_VNAME(T)        array_reg_msg_##T##_
#define ARRAY_REGISTER_MESSAGE(T, key, ...)    static ArrayMessageFactory::register_t<T> ARRAY_REGISTER_MESSAGE_VNAME(T)(key, ##__VA_ARGS__);

#define ARRAY_REGISTER_MESSAGE_01(T, key, arg_type, ...)   \
                                                static ArrayMessageFactory::register_t<T, arg_type> ARRAY_REGISTER_MESSAGE_VNAME(T)(key, ##__VA_ARGS__);

#include "Messages.h"

ARRAY_REGISTER_MESSAGE(CephMessage00, CEPH_MSG_ID_0);
ARRAY_REGISTER_MESSAGE(CephMessage01, CEPH_MSG_ID_1);
ARRAY_REGISTER_MESSAGE(CephMessage02, CEPH_MSG_ID_2);
ARRAY_REGISTER_MESSAGE(CephMessage03, CEPH_MSG_ID_3);
ARRAY_REGISTER_MESSAGE(CephMessage04, CEPH_MSG_ID_4);
ARRAY_REGISTER_MESSAGE(CephMessage05, CEPH_MSG_ID_5);
ARRAY_REGISTER_MESSAGE(CephMessage06, CEPH_MSG_ID_6);
ARRAY_REGISTER_MESSAGE(CephMessage07, CEPH_MSG_ID_7);
ARRAY_REGISTER_MESSAGE(CephMessage08, CEPH_MSG_ID_8);
ARRAY_REGISTER_MESSAGE(CephMessage09, CEPH_MSG_ID_9);
ARRAY_REGISTER_MESSAGE(CephMessage10, CEPH_MSG_ID_10);
//ARRAY_REGISTER_MESSAGE_01(CephMessage10, CEPH_MSG_ID_10, int, 1);

ARRAY_REGISTER_MESSAGE(CephMessage11, CEPH_MSG_ID_11);
ARRAY_REGISTER_MESSAGE(CephMessage12, CEPH_MSG_ID_12);
ARRAY_REGISTER_MESSAGE(CephMessage13, CEPH_MSG_ID_13);
ARRAY_REGISTER_MESSAGE(CephMessage14, CEPH_MSG_ID_14);
ARRAY_REGISTER_MESSAGE(CephMessage15, CEPH_MSG_ID_15);
ARRAY_REGISTER_MESSAGE(CephMessage16, CEPH_MSG_ID_16);
ARRAY_REGISTER_MESSAGE(CephMessage17, CEPH_MSG_ID_17);
ARRAY_REGISTER_MESSAGE(CephMessage18, CEPH_MSG_ID_18);
ARRAY_REGISTER_MESSAGE(CephMessage19, CEPH_MSG_ID_19);
ARRAY_REGISTER_MESSAGE(CephMessage20, CEPH_MSG_ID_20);
ARRAY_REGISTER_MESSAGE(CephMessage21, CEPH_MSG_ID_21);

#endif // CEPH_ARRAY_MESSAGEFACTORY_H
