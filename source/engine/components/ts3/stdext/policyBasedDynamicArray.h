
#ifndef __TS3_STDEXT_POLICY_BASED_DYNAMIC_ARRAY_H__
#define __TS3_STDEXT_POLICY_BASED_DYNAMIC_ARRAY_H__

#include "staticLimits.h"
#include "memoryBuffer.h"
#include <variant>

namespace ts3
{

	template <size_t tFixedCapacity, size_t tResizeFactor, size_t tMaximumCapacity = Limits<size_t>::maxValue>
	struct DynamicArrayPolicy
	{
		static constexpr size_t sFixedCapacity = tFixedCapacity;
		static constexpr size_t sResizeFactor = tResizeFactor;
		static constexpr size_t sMaximumCapacity = tMaximumCapacity;
	};

	template <typename TValue, typename TMemoryPolicy = DynamicArrayPolicy<0, 2>, typename TAlloc = std::allocator<TValue> >
	class PolicyBasedDynamicArray
	{
	private:

	};

}

#endif // __TS3_STDEXT_POLICY_BASED_DYNAMIC_ARRAY_H__
