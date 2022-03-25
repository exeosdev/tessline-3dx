
#include <ts3/core/reflection/enumTypeInfo.h>

#include <ts3/engine/utility/scfCommon.h>

namespace ts3
{

	ts3TypeInfoEnumDefine( ESCFEntryType )
	{
		ts3TypeInfoEnumBegin( ESCFEntryType );
		ts3TypeInfoEnumRegisterClassConstant( ESCFEntryType::Resource      );
		ts3TypeInfoEnumRegisterClassConstant( ESCFEntryType::VirtualFolder );
	}

}
