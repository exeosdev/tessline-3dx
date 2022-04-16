
#include <ts3/system/prerequisites.h>
#include "mseCommon.h"
#include <comdef.h>

#if( TS3_PCL_TARGET_OS & TS3_PCL_TARGET_FLAG_OS_MSE )
namespace ts3::system
{

	namespace platform
	{

		std::string mseQueryCOMErrorMessage( HRESULT pHResult )
		{
			_com_error comError{ pHResult };
			_bstr_t comErrorString{ comError.ErrorMessage() };

			std::string errorMessage;
			if( comErrorString.length() > 0 )
			{
				const auto * strBuffer = static_cast<const char *>( comErrorString );
				errorMessage.assign( strBuffer, comErrorString.length() );
			}

			return errorMessage;
		}

		std::string mseQuerySystemErrorMessage( DWORD pErrorCode )
		{
			char * messagePtr = nullptr;

			const auto messageLength = FormatMessageA(
				FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_ALLOCATE_BUFFER,
				nullptr, // No format string provided for FORMAT_MESSAGE_FROM_SYSTEM
				pErrorCode,
				MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
				reinterpret_cast<LPTSTR>( &messagePtr ),
				0,
				nullptr );

			std::string errorMessage;
			if( messageLength > 0 )
			{
				errorMessage.assign( messagePtr, messageLength );

				// FORMAT_MESSAGE_ALLOCATE_BUFFER requires a manual release of the buffer allocated by the system.
				// In case of 'messageLength' being 0, no buffer gets allocated!
				::LocalFree( messagePtr );
			}

			return errorMessage;
		}


	}

} // namespace ts3::system
#endif
