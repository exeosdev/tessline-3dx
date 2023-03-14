
#include "memory.h"

namespace ts3
{

	static MemoryAllocationProxy getDefaultAllocationProxy() noexcept
	{
		MemoryAllocationProxy allocationProxy;
		try
		{
			allocationProxy.apiAlloc = []( size_t pSize ) -> void * {
				return std::malloc( pSize );
			};
			allocationProxy.apiFree = []( void * pMemory ) -> void {
				return std::free( pMemory );
			};
			allocationProxy.apiRealloc = []( void * pMemory, size_t pNewSize ) -> void * {
				return std::realloc( pMemory, pNewSize );
			};
			allocationProxy.memoryAlignment = cxdefs::MEMORY_DEFAULT_ALIGNMENT;
		}
		catch( ... )
		{
			ts3DebugInterrupt();
		}
		return allocationProxy;
	}

	const MemoryAllocationProxy cvDefaultMemoryAllocationProxy = getDefaultAllocationProxy();

	bool memCheckMemoryOverlap( const void * pMemory1, size_t pSize1, const void * pMemory2, size_t pSize2 )
	{
		// For a two different regions with at least one being of size 0, there can be no overlap.
		if( ( pSize1 == 0 ) || ( pSize2 == 0 ) )
		{
			return false;
		}
		// If pointers are the same, we treat it as an overlap, no matter the size.
		if( pMemory1 == pMemory2 )
		{
			return true;
		}
		// pMemory2 is within <pMemory1Start, pMemory1End> -> overlap.
		if( ( pMemory2 > pMemory1 ) && ( pMemory2 <= reinterpret_cast<const char *>( pMemory1 ) + pSize1 ) )
		{
			return true;
		}
		// pMemory1 is within <pMemory2Start, pMemory2End> -> overlap.
		if( ( pMemory1 > pMemory2 ) && ( pMemory1 <= reinterpret_cast<const char *>( pMemory2 ) + pSize2 ) )
		{
			return true;
		}

		return false;
	}

	void memCopyChecked( void * pDst, size_t pDstSize, const void * pSrc, size_t pCopySize )
	{
		// For zero copy size, do nothing and just return.
		if( pCopySize == 0 )
		{
			return;
		}
		// Target must not be NULL and have a size greater than zero.
		if( ( pDst == nullptr ) || ( pDstSize == 0u ) )
		{
			throw 0;
		}
		// Check if the data to be copied fits into buffer to prevent writing in someone else's memory.
		if( pCopySize > pDstSize )
		{
			throw 0;
		}
		// For memory copying, regions may not overlap.
		if( memCheckMemoryOverlap( pDst, pCopySize, pSrc, pCopySize ) )
		{
			throw 0;
		}
		TS3_PCL_COPY_MEMORY( pDst, pSrc, pCopySize );
	}

	void memCopyUnchecked( void * pDst, size_t pDstSize, const void * pSrc, size_t pCopySize )
	{
		ts3DebugAssert( ( pDst != nullptr ) && ( pDstSize > 0 ) );
		ts3DebugAssert( pCopySize <= pDstSize );
		ts3DebugAssert( !memCheckMemoryOverlap( pDst, pCopySize, pSrc, pCopySize ) );
		TS3_PCL_COPY_MEMORY( pDst, pSrc, pCopySize );
	}

	void memFillChecked( void * pMemoryPtr, size_t pMemorySize, byte pFillValue, size_t pFillSize )
	{
		if( ( pMemoryPtr == nullptr ) && ( pMemorySize == 0 ) )
		{
			throw 0;
		}
		if( pFillSize > pMemorySize )
		{
			throw 0;
		}
		TS3_PCL_FILL_MEMORY( pMemoryPtr, pFillSize, pFillValue );
	}

	void memFillUnchecked( void * pMemoryPtr, size_t pMemorySize, byte pFillValue, size_t pFillSize )
	{
		ts3DebugAssert( ( pMemoryPtr != nullptr ) && ( pMemorySize > 0 ) );
		ts3DebugAssert( pFillSize <= pMemorySize );
		TS3_PCL_FILL_MEMORY( pMemoryPtr, pFillSize, pFillValue );
	}

	void memMoveChecked( void * pMemoryPtr, size_t pMemorySize, size_t pBaseOffset, size_t pMoveSize, ptrdiff_t pMoveOffset )
	{
		// Memory is assumed to be a non-null pointer with a pointed area of size greater than zero.
		if( ( pMemoryPtr == nullptr ) && ( pMemorySize == 0 ) )
		{
			throw 0;
		}
		// We are moving pCount elements from pBaseOffset by pMoveOffset. Check if the memory block is large enough.
		if( pBaseOffset + pMoveOffset + pMoveSize > pMemorySize )
		{
			throw 0;
		}
		// Source pointer: base memory pointer + base offset
		auto * const srcMemoryPtr = reinterpret_cast<char *>( pMemoryPtr ) + pBaseOffset;
		// Destination pointer: source pointer + number of elements to move the block by.
		auto * const dstMemoryPtr = srcMemoryPtr + pMoveOffset;
		// Do the actual move.
		TS3_PCL_MOVE_MEMORY( dstMemoryPtr, srcMemoryPtr, pMoveSize );
	}

	void memMoveUnchecked( void * pMemoryPtr, size_t pMemorySize, size_t pBaseOffset, size_t pMoveSize, ptrdiff_t pMoveOffset )
	{
		ts3DebugAssert( ( pMemoryPtr != nullptr ) && ( pMemorySize > 0 ) );
		ts3DebugAssert( pBaseOffset + pMoveOffset + pMoveSize <= pMemorySize );
		auto * const srcMemoryPtr = reinterpret_cast<char *>( pMemoryPtr ) + pBaseOffset;
		auto * const dstMemoryPtr = srcMemoryPtr + pMoveOffset;
		TS3_PCL_MOVE_MEMORY( dstMemoryPtr, srcMemoryPtr, pMoveSize );
	}

	void memZeroChecked( void * pMemoryPtr, size_t pMemorySize, size_t pZeroSize )
	{
		if( ( pMemoryPtr == nullptr ) && ( pMemorySize == 0 ) )
		{
			throw 0;
		}
		if( pZeroSize > pMemorySize )
		{
			throw 0;
		}
		TS3_PCL_ZERO_MEMORY( pMemoryPtr, pZeroSize );
	}

	void memZeroUnchecked( void * pMemoryPtr, size_t pMemorySize, size_t pZeroSize )
	{
		ts3DebugAssert( ( pMemoryPtr != nullptr ) && ( pMemorySize > 0 ) );
		ts3DebugAssert( pZeroSize <= pMemorySize );
		TS3_PCL_ZERO_MEMORY( pMemoryPtr, pZeroSize );
	}

}
