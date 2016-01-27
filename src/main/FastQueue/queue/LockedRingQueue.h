
#ifndef FASTQUEUE_QUEUE_LOCKEDRINGQUEUE_H
#define FASTQUEUE_QUEUE_LOCKEDRINGQUEUE_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <assert.h>

#include "FastQueue/basic/stddef.h"
#include "FastQueue/basic/stdsize.h"
#include "FastQueue/utils/PowOf2.h"

namespace FastQueue {

enum queue_trait_value_t {
    kQueueDefaultCapacity = 1024,
    kCacheLineSize = 64
};

enum queue_op_state_t {
    OP_STATE_EMPTY = -2,
    OP_STATE_FAILURE = -1,
    OP_STATE_SUCCESS = 0
};

template <typename T, typename LockType = std::mutex,
          typename IndexType = uint64_t,
          size_t initCapacity = kQueueDefaultCapacity>
class Fixed_LockedRingQueue {
public:
    typedef T               item_type;
    typedef T *             value_type;
    typedef LockType        lock_type;
    typedef IndexType       index_type;
    typedef std::size_t     size_type;

    static const index_type kInitCursor = (index_type)(-1);
    static const index_type kDefaultCapacity = (index_type)compile_time::round_to_pow2<kQueueDefaultCapacity>::value;
    static const index_type kCapacity = (index_type)compile_time::round_to_pow2<initCapacity>::value;
    static const index_type kIndexMask = (index_type)(kCapacity - 1);
    static const size_type  kAlignment = compile_time::round_to_pow2<kCacheLineSize>::value;

private:
    index_type      head_;
    index_type      tail_;
    value_type      entries_;
    size_type       capacity_;
    value_type      allocEntries_;
    size_type       allocSize_;
    lock_type       lock_;

private:
    inline void init() {
        assert(run_time::is_pow2(kAlignment));
        static const size_type kAligntMask = ~(kAlignment - 1);
        static const size_type kAlignPadding = kAlignment - 1;
        assert(kAlignment != 0);
        assert(kAligntMask != 0);
        assert((kAlignPadding != (size_type)(-1)) && ((unsigned)kAlignPadding < (unsigned)kAlignment));
        // entries_ addr must align to kAlignment byte.
        assert(run_time::is_pow2(kCapacity));
        size_type allocSize = kCapacity + kAlignPadding;
        value_type newEntries = new item_type[allocSize];
        if (newEntries) {
            allocEntries_ = newEntries;
            allocSize_ = allocSize;
            entries_ = reinterpret_cast<value_type>(reinterpret_cast<uintptr_t>(newEntries + kAlignPadding) & kAligntMask);
        }
    }

public:
    Fixed_LockedRingQueue()
        : head_(kInitCursor), tail_(kInitCursor), capacity_(kCapacity),
          entries_(nullptr), allocEntries_(nullptr), allocSize_(0), lock_() {
        init();
    }

    virtual ~Fixed_LockedRingQueue() {
        lock_.lock();
        free_queue();
        lock_.unlock();
    }

    inline void free_queue() {
        if (allocEntries_) {
            delete[] allocEntries_;
            allocEntries_ = nullptr;
#if defined(_DEBUG) || !defined(NDEBUG)
            allocSize_ = 0;
            entries_ = nullptr;
            head_ = tail_ = kInitCursor;
#endif
        }
    }

    bool is_empty() const {
        bool _isEmpty;
        lock_.lock();
        _isEmpty = (head_ == tail_);
        lock_.unlock();
        assert((head_ - tail_) <= kCapacity);
        return _isEmpty;
    }

    size_type capacity() const {
        return kCapacity;
    }

    size_type sizes() {
        size_type size;
        lock_.lock();
        size = head_ - tail_;
        lock_.unlock();
        assert(size <= capacity_);
        return size;
    }

    int push_front(T && item) {
        lock_.lock();

        if ((head_ - tail_) > kCapacity) {
            lock_.unlock();
            return OP_STATE_EMPTY;
        }

        index_type next = head_ + 1;
        index_type index = next & kIndexMask;
        assert(entries_ != nullptr);
        entries_[index] = item;
        head_ = next;

        lock_.unlock();
        return OP_STATE_SUCCESS;
    }

    int pop_front(T & item) {
        lock_.lock();

        if (head_ == tail_) {
            lock_.unlock();
            return OP_STATE_EMPTY;
        }

        index_type next = tail_ + 1;
        index_type index = next & kIndexMask;
        assert(entries_ != nullptr);
        item = entries_[index];
        tail_ = next;

        lock_.unlock();
        return OP_STATE_SUCCESS;
    }

    int push_front_ref(const T & item);
    int pop_front_ref(T & item);

    int push_front_point(const T * item);
    int pop_front_point(T * item);
}; // class Fixed_LockedRingQueue<T, ...>

template <typename T, typename LockType = std::mutex,
          typename IndexType = uint64_t>
class LockedRingQueue {
public:
    typedef T               item_type;
    typedef T *             value_type;
    typedef LockType        lock_type;
    typedef IndexType       index_type;
    typedef std::size_t     size_type;

    static const index_type kInitCursor = (index_type)(-1);
    static const index_type kDefaultCapacity = (index_type)compile_time::round_to_pow2<kQueueDefaultCapacity>::value;
    static const size_type  kAlignment = compile_time::round_to_pow2<kCacheLineSize>::value;

private:
    index_type      head_;
    index_type      tail_;
    value_type      entries_;
    index_type      index_mask_;
    size_type       capacity_;
    value_type      allocEntries_;
    size_type       allocSize_;
    lock_type       lock_;

private:
    inline size_type internal_init(size_type nCapacity) {
        assert(run_time::is_pow2(kAlignment));
        static const size_type kAligntMask = ~(kAlignment - 1);
        static const size_type kAlignPadding = kAlignment - 1;
        assert(kAlignment != 0);
        assert(kAligntMask != 0);
        assert((kAlignPadding != (size_type)(-1)) && ((unsigned)kAlignPadding < (unsigned)kAlignment));
        // nCapacity must align to power of 2.
        size_type newCapacity = run_time::round_to_pow2<size_type>(nCapacity);
        assert(run_time::is_pow2(newCapacity));
        // entries_ addr must align to kAlignment byte.
        size_type allocSize = newCapacity + kAlignPadding;
        value_type newEntries = new item_type[allocSize];
        if (newEntries) {
            allocEntries_ = newEntries;
            allocSize_ = allocSize;
            entries_ = reinterpret_cast<value_type>(reinterpret_cast<uintptr_t>(newEntries + kAlignPadding) & kAligntMask);
            return newCapacity;
        }
        else {
            return capacity_;
        }
    }

protected:
    inline void init(size_type nCapacity) {
        // capacity_(capacity of entries) must align to power of 2.
        capacity_ = internal_init(nCapacity);
    }

public:
    LockedRingQueue(uint32_t nCapacity = kDefaultCapacity)
        : head_(kInitCursor), tail_(kInitCursor), capacity_(nCapacity), index_mask_(nCapacity - 1),
          entries_(nullptr), allocEntries_(nullptr), allocSize_(0), lock_() {
        capacity_ = internal_init(nCapacity);
        index_mask_ = capacity_ - 1;
        assert(run_time::is_pow2(capacity_));
    }

    virtual ~LockedRingQueue() {
        lock_.lock();
        free_queue_fast();
        lock_.unlock();
    }

    inline void free_queue_fast() {
        if (allocEntries_) {
            delete[] allocEntries_;
            allocEntries_ = nullptr;
#if defined(_DEBUG) || !defined(NDEBUG)
            allocSize_ = 0;
            entries_ = nullptr;
            capacity_ = 0;
            index_mask_ = 0;
            head_ = tail_ = kInitCursor;
#endif
        }
    }

    inline void free_queue() {
        if (allocEntries_) {
            delete[] allocEntries_;
            allocEntries_ = nullptr;
            allocSize_ = 0;
            entries_ = nullptr;
            capacity_ = 0;
            index_mask_ = 0;
            head_ = tail_ = kInitCursor;
        }
    }

    bool is_empty() const {
        bool _isEmpty;
        lock_.lock();
        _isEmpty = (head_ == tail_);
        lock_.unlock();
        return _isEmpty;
    }

    size_type capacity() const {
        return capacity_;
    }

    size_type sizes() {
        size_type size;
        lock_.lock();
        size = head_ - tail_;
        lock_.unlock();
        assert(size <= capacity_);
        return size;
    }

    void resize(size_type nCapacity) {
        lock_.lock();
        free_queue();
        init(nCapacity);
        lock_.unlock();
    }

    void create(size_type nCapacity) {
        resize(nCapacity);
    }

    int push_front(T && item) {
        lock_.lock();

        if ((head_ - tail_) > capacity_) {
            lock_.unlock();
            return OP_STATE_EMPTY;
        }

        index_type next = head_ + 1;
        index_type index = next & index_mask_;
        assert(entries_ != nullptr);
        entries_[index] = item;
        head_ = next;

        lock_.unlock();
        return OP_STATE_SUCCESS;
    }

    int pop_front(T & item) {
        lock_.lock();

        if (head_ == tail_) {
            lock_.unlock();
            return OP_STATE_EMPTY;
        }

        index_type next = tail_ + 1;
        index_type index = next & index_mask_;
        assert(entries_ != nullptr);
        item = entries_[index];
        tail_ = next;

        lock_.unlock();
        return OP_STATE_SUCCESS;
    }

    int push_front_ref(const T & item);
    int pop_front_ref(T & item);

    int push_front_point(const T * item);
    int pop_front_point(T * item);
}; // class LockedRingQueue<T, ...>

} // namespace FastQueue

#endif  /* FASTQUEUE_QUEUE_LOCKEDRINGQUEUE_H */
