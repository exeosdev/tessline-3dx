
#include "commonMemoryDefs.h"
#include <ts3/stdext/memory.h>

void * operator new( size_t pSize, const ts3::AllocNewSizeExplicitTag &, size_t pExtraSize )
{
	const auto requestedObjectSize = pSize + pExtraSize;
	const auto allocationSize = ts3::memGetAlignedValue( requestedObjectSize, ts3::cxdefs::MEMORY_DEFAULT_ALIGNMENT );
	return std::malloc( allocationSize );
}
