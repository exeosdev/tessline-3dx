
#include <ts3/core/reflection/enumTypeInfo.h>

namespace ts3
{

	ts3TypeInfoEnumDefine( EActiveState )
	{
		ts3TypeInfoEnumBegin( EActiveState );
		ts3TypeInfoEnumRegisterClassConstant( EActiveState::Disabled );
		ts3TypeInfoEnumRegisterClassConstant( EActiveState::Enabled ) ;
		ts3TypeInfoEnumRegisterClassConstant( EActiveState::Unknown  );
	}

	ts3TypeInfoEnumDefine( EAccessModeFlags )
	{
		ts3TypeInfoEnumBegin( EAccessModeFlags );
		ts3TypeInfoEnumRegisterUnscopedConstant( E_ACCESS_MODE_FLAG_READ_BIT );
		ts3TypeInfoEnumRegisterUnscopedConstant( E_ACCESS_MODE_FLAG_WRITE_BIT ) ;
		ts3TypeInfoEnumRegisterUnscopedConstant( E_ACCESS_MODE_FLAGS_FULL_ACCESS );
		ts3TypeInfoEnumRegisterUnscopedConstant( E_ACCESS_MODE_FLAGS_NONE );
	}

}
