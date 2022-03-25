
#ifndef __TS3_CORE_GDS_COMMON_H__
#define __TS3_CORE_GDS_COMMON_H__

#include "../exception.h"

namespace ts3
{

	struct GdsSerializable
	{};

	template <typename Tp>
	struct IsGdsSerializable
	{
		static inline constexpr bool sValue = std::is_base_of<GdsSerializable, Tp>::value;
	};

	enum : exception_category_value_t
	{
		E_EXCEPTION_CATEGORY_FRAMEWORK_CORE_GDS =
			ecDeclareExceptionCategory( ExceptionBaseType::FrameworkCore, ts3ExcCategoryIID( 0x7C ) ),
	};

	class CoreGdsException : public FrameworkCoreException
	{
	public:
		explicit CoreGdsException( ExceptionInfo pExceptionInfo )
		: FrameworkCoreException( std::move( pExceptionInfo ) )
		{}
	};

	ts3SetExceptionCategoryType( E_EXCEPTION_CATEGORY_FRAMEWORK_CORE_GDS, CoreGdsException );

	enum : exception_code_value_t
	{
		E_EXC_CORE_GDS =
			ecDeclareExceptionCode( E_EXCEPTION_CATEGORY_FRAMEWORK_CORE_GDS, ts3ExcCodeIID( 0x71 ) ),
	};

} // namespace ts3

#endif // __TS3_CORE_GDS_COMMON_H__
