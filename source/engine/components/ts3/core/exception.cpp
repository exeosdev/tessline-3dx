
#include "exception.h"
#include <sstream>

namespace ts3
{

    std::string ExceptionInfo::toString() const
	{
		std::ostringstream strStream;
		strStream << "[Exception]\n";
		strStream << "- Code: 0x" << std::hex << code << "\n";
		strStream << "- Desc: " << description << "\n";
		strStream << "- Source: " << fileLocationInfo.toString();

		return strStream.str();
	}

}
