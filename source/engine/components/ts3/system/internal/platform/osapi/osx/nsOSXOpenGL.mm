
#include "nsOSXOpenGL.h"

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif // __clang__

@implementation NSOSXOpenGLContext : NSOpenGLContext

-( void ) update
{
	[self setUpdateFlag];
	[self updateConditional];
}

-( void ) setUpdateFlag
{
	self->_updateFlag.store( 1, std::memory_order_release );
}

-( BOOL ) isUpdateRequired
{
	return self->_updateFlag.load( std::memory_order_acquire ) != 0;
}

-( BOOL ) updateConditional
{
	uint32_t expectedFlag = 1;
	if( self->_updateFlag.compare_exchange_strong( expectedFlag, 0, std::memory_order_acq_rel ) )
	{
		[self updateExplicit];
		return YES;
	}
	return NO;
}

-( void ) updateExplicit
{
	if( [NSThread isMainThread] )
	{
		[super update];
	}
	else
	{
		const auto mainQueue = dispatch_get_main_queue();
		dispatch_async( mainQueue, ^{ [super update]; } );
	}
}

@end

#ifdef __clang__
#pragma clang diagnostic pop
#endif // __clang__
