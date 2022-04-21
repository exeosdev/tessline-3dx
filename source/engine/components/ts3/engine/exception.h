
#pragma once

#ifndef __TS3_ENGINE_EXCEPTION_H__
#define __TS3_ENGINE_EXCEPTION_H__

#include "prerequisites.h"
#include <ts3/core/exception.h>

namespace ts3
{

	enum : exception_category_value_t
	{
		E_EXCEPTION_CATEGORY_ENGINE_SUBMODULE_MAIN =
			ecDeclareExceptionCategory( ExceptionBaseType::EngineSubModule, ts3ExcCategoryIID( 0x01 ) ),
	};

	class EngineMainSubModuleException : public EngineSubModuleException
	{
	public:
		explicit EngineMainSubModuleException( ExceptionInfo pExceptionInfo )
		: EngineSubModuleException( std::move( pExceptionInfo ) )
		{}
	};

	ts3SetExceptionCategoryType( E_EXCEPTION_CATEGORY_ENGINE_SUBMODULE_MAIN, EngineMainSubModuleException );

	enum : exception_code_value_t
	{
		E_EXC_ESM_MAIN_SCF_ERROR =
			ecDeclareExceptionCode( E_EXCEPTION_CATEGORY_ENGINE_SUBMODULE, ts3ExcCodeIID( 0x71 ) ),
	};

} // namespace ts3

#endif // __TS3_ENGINE_EXCEPTION_H__
