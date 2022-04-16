
#include "sysObject.h"

namespace ts3::system
{

    SysObject::SysObject( system::SysContextHandle pSysContext )
    : mSysContext( std::move( pSysContext ) )
    {}

    SysObject::~SysObject() = default;

	bool SysObject::destroySystemObject()
	{
		if( setDestroyRequestFlag() )
		{
			onDestroySystemObjectRequested();

			ts3DebugAssert( isStateMaskSet( E_SYS_OBJECT_STATE_FLAG_DESTROY_REQUEST_PROCESSED_BIT ) );

			return true;
		}
		return false;
	}

	void SysObject::onDestroySystemObjectRequested()
	{
		ts3DebugAssert( isDestroyRequestSet() );

		setStateFlags( true, E_SYS_OBJECT_STATE_FLAG_DESTROY_REQUEST_PROCESSED_BIT );
	}

} // namespace ts3::system
