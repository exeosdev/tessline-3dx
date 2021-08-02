
#include "systemContext.h"

namespace ts3
{

	Context::Context() noexcept = default;

	Context::~Context() noexcept
	{
		_sysRelease();
	}


	ContextHandle sysCreateContext( const ContextCreateInfo & pCreateInfo )
	{
		auto sysContext = std::make_shared<Context>();
		sysContext->_sysInitialize( pCreateInfo );
		return sysContext;
	}

}
