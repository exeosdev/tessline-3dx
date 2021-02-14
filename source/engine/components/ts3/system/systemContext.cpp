
#include "systemContext.h"

namespace ts3
{

	SysContext::SysContext() noexcept = default;

	SysContext::~SysContext() noexcept
	{
		_sysRelease();
	}


	SysContextHandle sysCreateContext( const SysContextCreateInfo & pCreateInfo )
	{
		auto sysContext = std::make_shared<SysContext>();
		sysContext->_sysInitialize( pCreateInfo );
		return sysContext;
	}

}
