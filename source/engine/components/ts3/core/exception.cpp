
#include "exception.h"
#include <sstream>

namespace ts3
{

	std::string Exception::convertExceptionInfoToString( const ExceptionInfo & pInfo )
	{
		std::ostringstream strStream;
		strStream << "[Exception]\n";
		strStream << "- Code: 0x" << std::hex << pInfo.code << "\n";
		strStream << "- Desc: " << pInfo.description << "\n";
		strStream << "- Source: " << pInfo.fileLocationInfo.toString();

		return strStream.str();
	}

}
