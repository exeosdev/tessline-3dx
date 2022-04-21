
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

			setStateFlags( true, E_SYS_OBJECT_STATE_FLAG_DESTROY_REQUEST_PROCESSED_BIT );

			return true;
		}
		return false;
	}

	void SysObject::onDestroySystemObjectRequested()
	{}

} // namespace ts3::system
