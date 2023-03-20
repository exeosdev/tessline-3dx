
#pragma once

#include "range.h"
#include "utilities.h"
#include <functional>

#define TS3_USE_RUNTIME_CHECKED_MEMORY_ROUTINES 0

namespace ts3
{

	using MemoryAllocCallback = std::function<void *( size_t )>;
	using MemoryFreeCallback = std::function<void( void * )>;
	using MemoryReallocCallback = std::function<void *( void *, size_t )>;

	namespace cxdefs
	{

		inline constexpr uint32 MEMORY_DEFAULT_ALIGNMENT = TS3_PCL_MEMORY_BASE_ALIGNMENT;

	}

	struct MemoryAllocationProxy
	{
	public:
		MemoryAllocCallback apiAlloc;
		MemoryFreeCallback apiFree;
		MemoryReallocCallback apiRealloc;
		uint32 memoryAlignment = cxdefs::MEMORY_DEFAULT_ALIGNMENT;

	public:
		explicit operator bool() const
		{
			return apiAlloc && apiFree && apiRealloc;
		}

		void swap( MemoryAllocationProxy & pOther )
		{
			std::swap( apiAlloc, pOther.apiAlloc );
			std::swap( apiFree, pOther.apiFree );
			std::swap( apiRealloc, pOther.apiRealloc );
			std::swap( memoryAlignment, pOther.memoryAlignment );
		}
	};

	inline void swap( MemoryAllocationProxy & pFirst, MemoryAllocationProxy & pSecond )
	{
		pFirst.swap( pSecond );
	}

	extern const MemoryAllocationProxy cvDefaultMemoryAllocationProxy;

	template <typename TVal>
	struct MemoryBaseSize
	{
		static constexpr size_t size = sizeof( TVal );
	};

	template <>
	struct MemoryBaseSize<void>
	{
		static constexpr size_t size = 1;
	};

	template <typename TSize, typename TOffset = TSize>
	struct AlignedMemoryAllocInfo
	{
		Region<TSize, TOffset> accessibleRegion;
		Region<TSize, TOffset> reservedRegion;
	};

	inline constexpr uint64 memCheckRequestedCopySize( uint64 pBufferSize, uint64 pCopySize, uint64 pCopyOffset )
	{
		const auto copyOffset = getMinOf( pCopyOffset, pBufferSize );
		const auto maxCopySize = pBufferSize - copyOffset;
		const auto copySize = getMinOf( pCopySize, maxCopySize );
		return copySize;
	}

	///
	/// @tparam TValue
	/// @param pValue
	/// @param pAlignment
	/// @return
	template <typename TValue>
	inline constexpr TValue memGetAlignedPowerOf2( TValue pValue, uint32 pAlignment )
	{
		return static_cast<TValue>( ( static_cast<uint64>( pValue ) + pAlignment ) & ( ~( static_cast<uint64>( pAlignment ) - 1 ) ) );
	}

	///
	/// @tparam TValue
	/// @param pValue
	/// @param pAlignment
	/// @return
	template <typename TValue>
	inline TValue memGetAlignedValue( TValue pValue, uint32 pAlignment )
	{
		const auto valueAlignmentMod = pValue % pAlignment;
		return ( valueAlignmentMod != 0 ) ? ( pValue + pAlignment - valueAlignmentMod ) : pValue;
	}

	template <typename TSize, typename TOffset = TSize, typename std::enable_if<std::is_integral<TSize>::value, int>::type = 0>
	inline AlignedMemoryAllocInfo<TSize, TOffset> memComputeAlignedAllocationInfo( TOffset pBaseAddress, TSize pAllocationSize, uint32 pAlignment )
	{
		const auto alignedOffset = memGetAlignedValue( pBaseAddress, pAlignment );

		AlignedMemoryAllocInfo<TSize, TOffset> allocInfo;
		allocInfo.accessibleRegion.offset = alignedOffset;
		allocInfo.accessibleRegion.size = pAllocationSize;
		allocInfo.reservedRegion.offset = pBaseAddress;
		allocInfo.reservedRegion.size = ( alignedOffset - pBaseAddress ) + pAllocationSize;

		return allocInfo;
	}

	template <typename TVal>
	bool memCompareEqual( const TVal & pFirst, const TVal & pSecond )
	{
		return memcmp( &pFirst, &pSecond, sizeof (TVal ) ) == 0;
	}

	template <typename TVal>
	bool memCompareNotEqual( const TVal & pFirst, const TVal & pSecond )
	{
		return memcmp( &pFirst, &pSecond, sizeof (TVal ) ) != 0;
	}

	///
	/// @param pMemory1
	/// @param pSize1
	/// @param pMemory2
	/// @param pSize2
	/// @return
	bool memCheckMemoryOverlap( const void * pMemory1, size_t pSize1, const void * pMemory2, size_t pSize2 );

	///
	/// @param pDst
	/// @param pDstSize
	/// @param pSrc
	/// @param pCopySize
	void memCopyChecked( void * pDst, size_t pDstSize, const void * pSrc, size_t pCopySize );

	///
	/// @param pDst
	/// @param pDstSize
	/// @param pSrc
	/// @param pCopySize
	void memCopyUnchecked( void * pDst, size_t pDstSize, const void * pSrc, size_t pCopySize );

	/// @brief
	void memFillChecked( void * pMemoryPtr, size_t pMemorySize, byte pFillValue, size_t pFillSize );

	/// @brief
	void memFillUnchecked( void * pMemoryPtr, size_t pMemorySize, byte pFillValue, size_t pFillSize );

	/// @brief
	/// @param pMemoryPtr
	/// @param pMemorySize
	/// @param pBaseOffset
	/// @param pCount
	/// @param pMoveOffset
	void memMoveChecked( void * pMemoryPtr, size_t pMemorySize, size_t pBaseOffset, size_t pMoveSize, ptrdiff_t pMoveOffset );

	/// @brief
	/// @param pMemoryPtr
	/// @param pMemorySize
	/// @param pBaseOffset
	/// @param pCount
	/// @param pMoveOffset
	void memMoveUnchecked( void * pMemoryPtr, size_t pMemorySize, size_t pBaseOffset, size_t pMoveSize, ptrdiff_t pMoveOffset );

	/// @brief
	void memZeroChecked( void * pMemoryPtr, size_t pMemorySize, size_t pZeroSize );

	/// @brief
	void memZeroUnchecked( void * pMemoryPtr, size_t pMemorySize, size_t pZeroSize );

	/// @brief
	template <typename TDst, typename TSrc>
	inline void memCopy( TDst * pDst, size_t pCapacity, const TSrc * pSrc, size_t pCopyCount )
	{
		const auto dstByteCapacity = MemoryBaseSize<TDst>::size * pCapacity;
		const auto copyByteSize = MemoryBaseSize<TSrc>::size * pCopyCount;
	#if( TS3_USE_RUNTIME_CHECKED_MEMORY_ROUTINES )
		memCopyChecked( pDst, dstByteCapacity, pSrc, copyByteSize );
	#else
		memCopyUnchecked( pDst, dstByteCapacity, pSrc, copyByteSize );
	#endif
	}

	/// @brief
	template <typename TDst, typename TSrc>
	inline void memCopy( TDst & pDst, const TSrc & pSrc )
	{
		static_assert( sizeof( TDst ) >= sizeof( TSrc ) );
		memCopy( &pDst, 1, &pSrc, 1 );
	}

	/// @brief
	template <typename TData>
	inline void memFill( TData * pMemory, size_t pCapacity, byte pFillValue, size_t pFillCount )
	{
		const auto memoryByteCapacity = MemoryBaseSize<TData>::size * pCapacity;
		const auto fillByteSize = MemoryBaseSize<TData>::size * pFillCount;
	#if( TS3_USE_RUNTIME_CHECKED_MEMORY_ROUTINES )
		memFillChecked( pMemory, memoryByteCapacity, pFillValue, fillByteSize );
	#else
		memFillUnchecked( pMemory, memoryByteCapacity, pFillValue, fillByteSize );
	#endif
	}

	/// @brief
	template <typename TData>
	inline void memMove( TData * pMemory, size_t pCapacity, size_t pBaseOffset, size_t pCount, ptrdiff_t pMoveOffset )
	{
		constexpr auto elemSize = MemoryBaseSize<TData>::size;
	#if( TS3_USE_RUNTIME_CHECKED_MEMORY_ROUTINES )
		memMoveChecked( pMemory, pCapacity * elemSize, pBaseOffset * elemSize, pCount * elemSize, pMoveOffset * elemSize );
	#else
		memMoveUnchecked( pMemory, pCapacity * elemSize, pBaseOffset * elemSize, pCount * elemSize, pMoveOffset * elemSize );
	#endif
	}

	/// @brief
	template <typename TData>
	inline void memZero( TData * pMemory, size_t pCapacity, size_t pZeroCount )
	{
		const auto memoryByteCapacity = MemoryBaseSize<TData>::size * pCapacity;
		const auto zeroByteSize = MemoryBaseSize<TData>::size * pZeroCount;
	#if( TS3_USE_RUNTIME_CHECKED_MEMORY_ROUTINES )
		memZeroChecked( pMemory, memoryByteCapacity, zeroByteSize );
	#else
		memZeroUnchecked( pMemory, memoryByteCapacity, zeroByteSize );
	#endif
	}

	template <typename TVal, size_t tSize>
	inline void memZero( TVal ( &pArray )[tSize], size_t pZeroSize = sizeof( TVal ) * tSize )
	{
		pZeroSize = getMinOf( pZeroSize, sizeof( TVal ) * tSize );
		memZeroUnchecked( &( pArray[0] ), sizeof( TVal ) * tSize, pZeroSize );
	}

	template <typename TVal, size_t tSize>
	inline void memZero( std::array<TVal, tSize> & pArray, size_t pZeroSize = sizeof( TVal ) * tSize )
	{
		pZeroSize = getMinOf( pZeroSize, sizeof( TVal ) * tSize );
		memZeroUnchecked( &pArray, sizeof( TVal ) * tSize, pZeroSize );
	}

	template <typename TVal>
	inline void memZero( std::vector<TVal> & pVector, size_t pZeroOffset = 0, size_t pZeroCount = cxdefs::MAX_SIZE )
	{
		const auto vectorSize = pVector.size();
		pZeroCount = getMinOf( pZeroCount, vectorSize );
		pZeroOffset = getMinOf( pZeroOffset, vectorSize - pZeroCount );
		memZeroUnchecked( pVector.data() + pZeroOffset, sizeof( TVal ) * vectorSize, sizeof( TVal ) * pZeroCount );
	}

	template <typename TVal>
	inline void memZero( TVal & pObject, size_t pZeroSize = sizeof( TVal ) )
	{
		pZeroSize = getMinOf( pZeroSize, sizeof( TVal ) );
		memZeroUnchecked( &pObject, sizeof( TVal ), pZeroSize );
	}

}
