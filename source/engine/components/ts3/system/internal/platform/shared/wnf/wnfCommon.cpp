
#include <ts3/system/prerequisites.h>
#include <comdef.h>

namespace ts3
{

    std::string wnfQueryHresultErrorMessage( HRESULT pHResult )
	{
		_com_error comError{ pHResult };
		_bstr_t comErrorString{ comError.ErrorMessage() };
		return { static_cast<const char*>( comErrorString ) };
	}

}