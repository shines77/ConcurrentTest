#pragma once

#include <atomic>

class SpinLock final
{
public:
	inline SpinLock(void) : atomicLock({ false })	{ ; }
	SpinLock(SpinLock const &) = default;
	SpinLock(SpinLock &&);
	SpinLock &operator=(SpinLock const &) = default;
	SpinLock &operator=(SpinLock &&);
	~SpinLock(void) = default;

	inline void lock(void)
	{
		while (atomicLock.exchange(true));
		//
		bool old_value = atomicLock.load();
		while (atomicLock.compare_exchange_weak(old_value, true));
	}

	inline void unlock(void)
	{
		atomicLock = false;
	}

	inline bool isLocked(void) const
	{
		return atomicLock;
	}

private:
	std::atomic_bool atomicLock;
};