#pragma once

#include "../Thread/Atomic.h"
#include "../Sys/Assert.h"
#include "../Sys/Inline.h"
#include "../Sys/Limits.h"
#include "CustomDeleter.h"

namespace lethe
{

// used for intrusive reference counting
// base class for all reference counted objects
// note: this forces the class to be virtual!
// copy handling: none; we don't want to copy refcount!
// FIXME: seems racy after WeakPtr support
class LETHE_VISIBLE LETHE_API RefCounted
{
	typedef AtomicUInt AtomicType;
	typedef UInt NonAtomicType;

	mutable AtomicType strongRefCount;
	mutable AtomicType weakRefCount;
public:
	inline RefCounted() : strongRefCount(0), weakRefCount(1) {}
	inline RefCounted(const RefCounted &) : strongRefCount(0), weakRefCount(1) {}

	virtual inline ~RefCounted()
	{
		// FIXME: we can't test weakRefCount to 0
		LETHE_ASSERT(!strongRefCount && weakRefCount <= 1);
	}

	inline RefCounted &operator =(const RefCounted &)
	{
		return *this;
	}

	inline UInt AddRef() const
	{
		UInt res = Atomic::Increment(strongRefCount);
		LETHE_ASSERT(res);
		return res;
	}

	// decrement strong ref count but don't release
	// use at your own risk!
	inline UInt DecRefCount() const
	{
		return Atomic::Decrement(strongRefCount);
	}

	inline UInt Release() const
	{
		UInt res = DecRefCount();
		LETHE_ASSERT(res != (NonAtomicType)(((UInt)0-1) & Limits<NonAtomicType>::Max()));

		if (LETHE_UNLIKELY(!res))
			ReleaseAfterStrongZero();

		return res;
	}

	inline UInt AddWeakRef() const
	{
		LETHE_ASSERT(strongRefCount != 0);
		UInt res = Atomic::Increment(weakRefCount);
		LETHE_ASSERT(res);
		return res;
	}

	inline UInt ReleaseWeak() const
	{
		UInt res = Atomic::Decrement(weakRefCount);
		LETHE_ASSERT(res != (NonAtomicType)(((UInt)0-1) & Limits<NonAtomicType>::Max()));

		if (LETHE_UNLIKELY(!res && Atomic::CompareAndSwap(strongRefCount, (NonAtomicType)0, (NonAtomicType)0)))
			CustomDeleteObjectSkeleton();

		return res;
	}

	inline bool HasStrongRef() const
	{
		return Atomic::Load(strongRefCount) != 0;
	}

	// note: Finalize callback NOT called when object is on stack => only called for dynamically allocated objects!
	inline virtual void Finalize() const {}

protected:
	void ResetRefCounters()
	{
		strongRefCount = weakRefCount = 0;
	}

	using CustomDeleterFunc = void(*)(const void *);

	virtual CustomDeleterFunc GetCustomDeleter() const;

	void LETHE_NOINLINE CustomDeleteObjectSkeleton() const;

	void LETHE_NOINLINE ReleaseAfterStrongZero() const;
};

}
