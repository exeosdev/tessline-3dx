
#pragma once

#include "range.h"
#include "utilities.h"
#include <functional>

#define TS3_USE_RUNTIME_CHECKED_MEMORY_ROUTINES 0

namespace ts3
{

	inline constexpr uint32 CX_MEMORY_DEFAULT_ALIGNMENT = TS3_PCL_MEMORY_BASE_ALIGNMENT;

	using MemoryAllocCallback = std::function<void *( size_t )>;
	using MemoryFreeCallback = std::function<void( void * )>;
	using MemoryReallocCallback = std::function<void *( void *, size_t )>;

	struct MemoryAllocationProxy
	{
	public:
		MemoryAllocCallback apiAlloc;
		MemoryFreeCallback apiFree;
		MemoryReallocCallback apiRealloc;
		uint32 memoryAlignment = CX_MEMORY_DEFAULT_ALIGNMENT;

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

	template <typename Tp>
	struct MemoryBaseSize
	{
		static constexpr size_t size = sizeof( Tp );
	};

	template <>
	struct MemoryBaseSize<void>
	{
		static constexpr size_t size = 1;
	};

	template <typename TpSize, typename TpOffset = TpSize>
	struct AlignedMemoryAllocInfo
	{
		Region<TpSize, TpOffset> accessibleRegion;
		Region<TpSize, TpOffset> reservedRegion;
	};

	inline constexpr uint64 memCheckRequestedCopySize( uint64 pBufferSize, uint64 pCopySize, uint64 pCopyOffset )
	{
		const auto copyOffset = getMinOf( pCopyOffset, pBufferSize );
		const auto maxCopySize = pBufferSize - copyOffset;
		const auto copySize = getMinOf( pCopySize, maxCopySize );
		return copySize;
	}

	///
	/// @tparam TpValue
	/// @param pValue
	/// @param pAlignment
	/// @return
	template <typename TpValue>
	inline constexpr TpValue memGetAlignedPowerOf2( TpValue pValue, uint32 pAlignment )
	{
		return static_cast<TpValue>( ( static_cast<uint64>( pValue ) + pAlignment ) & ( ~( static_cast<uint64>( pAlignment ) - 1 ) ) );
	}

	///
	/// @tparam TpValue
	/// @param pValue
	/// @param pAlignment
	/// @return
	template <typename TpValue>
	inline TpValue memGetAlignedValue( TpValue pValue, uint32 pAlignment )
	{
		const auto valueAlignmentMod = pValue % pAlignment;
		return ( valueAlignmentMod != 0 ) ? ( pValue + pAlignment - valueAlignmentMod ) : pValue;
	}

	template <typename TpSize, typename TpOffset = TpSize, typename std::enable_if<std::is_integral<TpSize>::value, int>::type = 0>
	inline AlignedMemoryAllocInfo<TpSize, TpOffset> memComputeAlignedAllocationInfo( TpOffset pBaseAddress, TpSize pAllocationSize, uint32 pAlignment )
	{
		const auto alignedOffset = memGetAlignedValue( pBaseAddress, pAlignment );

		AlignedMemoryAllocInfo<TpSize, TpOffset> allocInfo;
		allocInfo.accessibleRegion.offset = alignedOffset;
		allocInfo.accessibleRegion.size = pAllocationSize;
		allocInfo.reservedRegion.offset = pBaseAddress;
		allocInfo.reservedRegion.size = ( alignedOffset - pBaseAddress ) + pAllocationSize;

		return allocInfo;
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
	template <typename TpDst, typename TpSrc>
	inline void memCopy( TpDst * pDst, size_t pCapacity, const TpSrc * pSrc, size_t pCopyCount )
	{
		const auto dstByteCapacity = MemoryBaseSize<TpDst>::size * pCapacity;
		const auto copyByteSize = MemoryBaseSize<TpSrc>::size * pCopyCount;
	#if( TS3_USE_RUNTIME_CHECKED_MEMORY_ROUTINES )
		memCopyChecked( pDst, dstByteCapacity, pSrc, copyByteSize );
	#else
		memCopyUnchecked( pDst, dstByteCapacity, pSrc, copyByteSize );
	#endif
	}

	/// @brief
	template <typename TpDst, typename TpSrc>
	inline void memCopy( TpDst & pDst, const TpSrc & pSrc )
	{
		static_assert( sizeof( TpDst ) >= sizeof( TpSrc ) );
		memCopy( &pDst, 1, &pSrc, 1 );
	}

	/// @brief
	template <typename TpData>
	inline void memFill( TpData * pMemory, size_t pCapacity, byte pFillValue, size_t pFillCount )
	{
		const auto memoryByteCapacity = MemoryBaseSize<TpData>::size * pCapacity;
		const auto fillByteSize = MemoryBaseSize<TpData>::size * pFillCount;
	#if( TS3_USE_RUNTIME_CHECKED_MEMORY_ROUTINES )
		memFillChecked( pMemory, memoryByteCapacity, pFillValue, fillByteSize );
	#else
		memFillUnchecked( pMemory, memoryByteCapacity, pFillValue, fillByteSize );
	#endif
	}

	/// @brief
	template <typename TpData>
	inline void memMove( TpData * pMemory, size_t pCapacity, size_t pBaseOffset, size_t pCount, ptrdiff_t pMoveOffset )
	{
		constexpr auto elemSize = MemoryBaseSize<TpData>::size;
	#if( TS3_USE_RUNTIME_CHECKED_MEMORY_ROUTINES )
		memMoveChecked( pMemory, pCapacity * elemSize, pBaseOffset * elemSize, pCount * elemSize, pMoveOffset * elemSize );
	#else
		memMoveUnchecked( pMemory, pCapacity * elemSize, pBaseOffset * elemSize, pCount * elemSize, pMoveOffset * elemSize );
	#endif
	}

	/// @brief
	template <typename TpData>
	inline void memZero( TpData * pMemory, size_t pCapacity, size_t pZeroCount )
	{
		const auto memoryByteCapacity = MemoryBaseSize<TpData>::size * pCapacity;
		const auto zeroByteSize = MemoryBaseSize<TpData>::size * pZeroCount;
	#if( TS3_USE_RUNTIME_CHECKED_MEMORY_ROUTINES )
		memZeroChecked( pMemory, memoryByteCapacity, zeroByteSize );
	#else
		memZeroUnchecked( pMemory, memoryByteCapacity, zeroByteSize );
	#endif
	}

	template <typename Tp, size_t tSize>
	inline void memZero( Tp ( &pArray )[tSize], size_t pZeroSize = sizeof( Tp ) * tSize )
	{
		pZeroSize = getMinOf( pZeroSize, sizeof( Tp ) * tSize );
		memZeroUnchecked( &( pArray[0] ), sizeof( Tp ) * tSize, pZeroSize );
	}

	template <typename Tp, size_t tSize>
	inline void memZero( std::array<Tp, tSize> & pArray, size_t pZeroSize = sizeof( Tp ) * tSize )
	{
		pZeroSize = getMinOf( pZeroSize, sizeof( Tp ) * tSize );
		memZeroUnchecked( &pArray, sizeof( Tp ) * tSize, pZeroSize );
	}

	template <typename Tp>
	inline void memZero( std::vector<Tp> & pVector, size_t pZeroOffset = 0, size_t pZeroCount = CX_MAX_SIZE )
	{
		const auto vectorSize = pVector.size();
		pZeroCount = getMinOf( pZeroCount, vectorSize );
		pZeroOffset = getMinOf( pZeroOffset, vectorSize - pZeroCount );
		memZeroUnchecked( pVector.data() + pZeroOffset, sizeof( Tp ) * vectorSize, sizeof( Tp ) * pZeroCount );
	}

	template <typename Tp>
	inline void memZero( Tp & pObject, size_t pZeroSize = sizeof( Tp ) )
	{
		pZeroSize = getMinOf( pZeroSize, sizeof( Tp ) );
		memZeroUnchecked( &pObject, sizeof( Tp ), pZeroSize );
	}

}
