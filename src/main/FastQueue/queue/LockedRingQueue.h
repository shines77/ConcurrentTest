
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

template <typename ImplType, typename ItemType>
class LockedRingQueueAbstract {
public:
    typedef LockedRingQueueAbstract<ImplType, ItemType>   this_type;
    typedef LockedRingQueueAbstract<ImplType, ItemType> * pthis_type;

    typedef ImplType            impl_type;
    typedef ImplType *          pimpl_type;
    typedef ImplType const *    const_pimpl_type;
    typedef ItemType            item_type;
    typedef std::size_t         size_type;

public:
    LockedRingQueueAbstract() {}
    virtual ~LockedRingQueueAbstract() {}

private:
    inline pimpl_type staic_cast_this() {
        pimpl_type pThis = static_cast<pimpl_type>(this);
        assert(pThis != nullptr);
        return pThis;
    }

    inline const_pimpl_type const_cast_this() const {
        pthis_type pOrigThisNonConst = const_cast<pthis_type>(this);
        pimpl_type pThisNonConst = static_cast<pimpl_type>(pOrigThisNonConst);
        const_pimpl_type pThisConst = const_cast<const_pimpl_type>(pThisNonConst);
        assert(pThisConst != nullptr);
        return pThisConst;
    }

public:
    bool is_valid() const {
        const_pimpl_type pThis = const_cast_this();
        return (pThis->entries_ != nullptr);
    }

    bool is_empty() const {
        const_pimpl_type pThis = const_cast_this();
        bool _isEmpty;
        pThis->lock_.lock();
        _isEmpty = (pThis->head_ == pThis->tail_);
        pThis->lock_.unlock();
        assert((pThis->head_ - pThis->tail_) <= pThis->capacity());
        return _isEmpty;
    }

    size_type capacity() const {
        const_pimpl_type pThis = const_cast_this();
        return pThis->capacity_;
    }

    size_type sizes() const {
        const_pimpl_type pThis = const_cast_this();
        size_type size;
        pThis->lock_.lock();
        size = pThis->head_ - pThis->tail_;
        pThis->lock_.unlock();
        assert(size <= pThis->capacity());
        return size;
    }

    int push_front(item_type const & item) {
        pimpl_type pThis = staic_cast_this();
        return pThis->inner_push_front(item);
    }

    int push_front(item_type && item) {
        pimpl_type pThis = staic_cast_this();
        return pThis->inner_push_front(std::move(item));
    }

    int pop_back(item_type & item) {
        pimpl_type pThis = staic_cast_this();
        return pThis->inner_pop_back(item);
    }

    int push(item_type const & item) {
        return this->push_front(item);
    }

    int push(item_type && item) {
        return this->push_front(item);
    }

    int pop(item_type & item) {
        return this->pop_back(item);
    }
};

template <typename T, typename LockType = std::mutex,
          typename IndexType = uint64_t,
          size_t initCapacity = kQueueDefaultCapacity>
class FixedLockedRingQueue :
    public LockedRingQueueAbstract<FixedLockedRingQueue<T, LockType, IndexType, initCapacity>, T> {
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

    template <typename U, typename T>
    friend class LockedRingQueueAbstract;

private:
    index_type          head_;
    index_type          tail_;
    value_type          entries_;
    size_type           capacity_;
    value_type          allocEntries_;
    size_type           allocSize_;
    mutable lock_type   lock_;

public:
    FixedLockedRingQueue()
        : head_(kInitCursor), tail_(kInitCursor), capacity_(kCapacity),
          entries_(nullptr), allocEntries_(nullptr), allocSize_(0), lock_() {
        init();
    }

    virtual ~FixedLockedRingQueue() {
        lock_.lock();
        free_queue();
        lock_.unlock();
    }

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

protected:
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

    template <typename U>
    int inner_push_front(U && item) {
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

    template <typename U>
    int inner_pop_back(U & item) {
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

public:
    void resize(size_type new_size) {
        // Do nothing!!
    }

}; // class Fixed_LockedRingQueue<T, ...>

template <typename T, typename LockType = std::mutex,
          typename IndexType = uint64_t>
class LockedRingQueue :
    public LockedRingQueueAbstract<LockedRingQueue<T, LockType, IndexType>, T> {
public:
    typedef T               item_type;
    typedef T *             value_type;
    typedef LockType        lock_type;
    typedef IndexType       index_type;
    typedef std::size_t     size_type;

    static const index_type kInitCursor = (index_type)(-1);
    static const index_type kDefaultCapacity = (index_type)compile_time::round_to_pow2<kQueueDefaultCapacity>::value;
    static const size_type  kAlignment = compile_time::round_to_pow2<kCacheLineSize>::value;

    template <typename U, typename T>
    friend class LockedRingQueueAbstract;

private:
    index_type          head_;
    index_type          tail_;
    value_type          entries_;
    index_type          index_mask_;
    size_type           capacity_;
    value_type          allocEntries_;
    size_type           allocSize_;
    mutable lock_type   lock_;

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

    template <typename U>
    int inner_push_front(U && item) {
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

    template <typename U>
    int inner_pop_back(U & item) {
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

public:
    void resize(size_type newCapacity) {
        lock_.lock();
        free_queue();
        init(newCapacity);
        lock_.unlock();
    }

    void create(size_type nCapacity) {
        resize(nCapacity);
    }

}; // class LockedRingQueue<T, ...>

} // namespace FastQueue

#endif  /* FASTQUEUE_QUEUE_LOCKEDRINGQUEUE_H */
