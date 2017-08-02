//
// Created by skyinno on 2/14/2016.
//

#ifndef CEPH_ARRAY_MESSAGEFACTORY_H
#define CEPH_ARRAY_MESSAGEFACTORY_H

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <vector>
#include <functional>
#include <memory>
#include <algorithm>    // For std::max

#include "Message.h"

#include "FastQueue/basic/stddef.h"

class ArrayMessageFactory
{
public:
    template <typename T>
    struct register_t
    {
        register_t(unsigned int key)
        {
            ArrayMessageFactory & factory = ArrayMessageFactory::get();
            factory.reserve_key(key);
            if (factory.array_)
                factory.array_[key] = &register_t<T>::create;
        }

        template <typename... Args>
        register_t(unsigned int key, Args... args)
        {
            ArrayMessageFactory & factory = ArrayMessageFactory::get();
            factory.reserve_key(key);
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
        assert(max_key_ < (int)max_capacity_);
        if ((int)key <= max_key_) {
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

    void reserve(unsigned capacity) {
#if 1
        // Use realloc() to implement reserve the old data.
        if (capacity > max_capacity_) {
            FuncPtr * new_array = (FuncPtr *)std::realloc(array_, sizeof(FuncPtr) * capacity);
            if (new_array) {
                array_ = new_array;
                max_capacity_ = capacity;
            }
        }
#else
        if (capacity > max_capacity_) {
            FuncPtr * new_array = (FuncPtr *)std::malloc(sizeof(FuncPtr) * capacity);
            if (new_array) {
                if (array_) {
                    // If array_ and new_array is different, reserve the old data.
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
#endif
    }

    void reserve_key(unsigned int key)
    {
        if ((int)key > max_key_) {
            max_key_ = (int)key;
            if (key >= max_capacity_) {
                unsigned new_capacity = std::max<unsigned>(max_capacity_ * 2, 1);
                if (key >= new_capacity)
                    new_capacity *= 2;
                this->reserve(new_capacity);
            }
        }
    }

    void resize(unsigned capacity, bool force_shrink = true, bool force_clear = false) {
        if (force_shrink || (capacity > max_capacity_)) {
            FuncPtr * new_array;
            // If you need clear the data, needn't to use realloc(),
            // because realloc() maybe copy the old data to new data area.
            if (!force_clear) {
                new_array = (FuncPtr *)std::realloc(array_, sizeof(FuncPtr) * capacity);
                if (new_array) {
                    array_ = new_array;
                    max_capacity_ = capacity;
                }
                else {
                    array_ = nullptr;
                    max_capacity_ = 0;
                }
                // Adjust the max key value by max_capacity_.
                if (max_key_ >= (int)max_capacity_)
                    max_key_ = max_capacity_ - 1;
            }
            else {
                if (array_) {
                    std::free(array_);
                    array_ = nullptr;
                    max_capacity_ = 0;
                }
                new_array = (FuncPtr *)std::malloc(sizeof(FuncPtr) * capacity);
                if (new_array) {
                    ::memset(new_array, 0, sizeof(FuncPtr) * capacity);
                    array_ = new_array;
                    max_capacity_ = capacity;
                }
                else {
                    array_ = nullptr;
                    max_capacity_ = 0;
                }
                // Adjust the max key value by max_capacity_.
                if (max_key_ >= (int)max_capacity_)
                    max_key_ = max_capacity_ - 1;
            }
        }
    }

protected:
    void force_resize(unsigned capacity, bool clear = true) {
        if (array_) {
            std::free(array_);
            array_ = nullptr;
            max_capacity_ = 0;
        }
        FuncPtr * new_array = (FuncPtr *)std::malloc(sizeof(FuncPtr) * capacity);
        if (new_array) {
            if (clear)
                ::memset(new_array, 0, sizeof(FuncPtr) * capacity);
            array_ = new_array;
            max_capacity_ = capacity;
        }
        // Adjust the max key value by max_capacity_.
        if (max_key_ >= (int)max_capacity_)
            max_key_ = max_capacity_ - 1;
    }

private:
    ArrayMessageFactory() : max_key_(-1), max_capacity_(0), array_(nullptr) {
        this->force_resize(128, true);
    };
    ArrayMessageFactory(const ArrayMessageFactory &) = delete;
    ArrayMessageFactory(ArrayMessageFactory &&) = delete;

    virtual ~ArrayMessageFactory() {
        if (array_) {
            std::free(array_);
#if !defined(NDEBUG)
            array_ = nullptr;
            max_key_ = 0;
            max_capacity_ = 0;
#endif
        }
    };

    FuncPtr *   array_;
    int         max_key_;
    unsigned    max_capacity_;
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
ARRAY_REGISTER_MESSAGE(CephMessage22, CEPH_MSG_ID_22);
ARRAY_REGISTER_MESSAGE(CephMessage23, CEPH_MSG_ID_23);
ARRAY_REGISTER_MESSAGE(CephMessage24, CEPH_MSG_ID_24);
ARRAY_REGISTER_MESSAGE(CephMessage25, CEPH_MSG_ID_25);
ARRAY_REGISTER_MESSAGE(CephMessage26, CEPH_MSG_ID_26);
ARRAY_REGISTER_MESSAGE(CephMessage27, CEPH_MSG_ID_27);
ARRAY_REGISTER_MESSAGE(CephMessage28, CEPH_MSG_ID_28);
ARRAY_REGISTER_MESSAGE(CephMessage29, CEPH_MSG_ID_29);

ARRAY_REGISTER_MESSAGE(CephMessage30, CEPH_MSG_ID_30);
ARRAY_REGISTER_MESSAGE(CephMessage31, CEPH_MSG_ID_31);
ARRAY_REGISTER_MESSAGE(CephMessage32, CEPH_MSG_ID_32);
ARRAY_REGISTER_MESSAGE(CephMessage33, CEPH_MSG_ID_33);
ARRAY_REGISTER_MESSAGE(CephMessage34, CEPH_MSG_ID_34);
ARRAY_REGISTER_MESSAGE(CephMessage35, CEPH_MSG_ID_35);
ARRAY_REGISTER_MESSAGE(CephMessage36, CEPH_MSG_ID_36);
ARRAY_REGISTER_MESSAGE(CephMessage37, CEPH_MSG_ID_37);
ARRAY_REGISTER_MESSAGE(CephMessage38, CEPH_MSG_ID_38);
ARRAY_REGISTER_MESSAGE(CephMessage39, CEPH_MSG_ID_39);

#endif // CEPH_ARRAY_MESSAGEFACTORY_H
