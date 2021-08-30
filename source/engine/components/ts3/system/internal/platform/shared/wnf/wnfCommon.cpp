
#include <ts3/system/prerequisites.h>
#include <comdef.h>

#if( TS3_PCL_TARGET_OS & TS3_PCL_TARGET_FLAG_OS_WINFAMILY )
namespace ts3::system
{

    std::string wnfQueryHresultErrorMessage( HRESULT pHResult )
	{
		_com_error comError{ pHResult };
		_bstr_t comErrorString{ comError.ErrorMessage() };
		return { static_cast<const char*>( comErrorString ) };
	}

} // namespace ts3::system
#endif
