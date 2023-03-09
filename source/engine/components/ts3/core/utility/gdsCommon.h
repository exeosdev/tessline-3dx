
#ifndef __TS3_CORE_GDS_COMMON_H__
#define __TS3_CORE_GDS_COMMON_H__

#include "../exception.h"

namespace ts3
{

	/// @brief Base type for GDS-enabled user types. It makes IsGdsSerializable<> to return 'true' for such type.
	///
	/// If a class/struct derives from GdsSerializable, it is assumed to provide the following public functions:
	/// - gds_size_t evalByteSize() const;
	/// - gds_size_t serialize( void * ) const;
	/// - gds_size_t deserialize( const void * );
	/// This enables such type to be passes directly into various gdsCore:: functions.
	/// Of course, absence of at least one of these functions will immediately result in a compilation error.
	struct GdsSerializable
	{};

	template <typename TVal>
	struct IsGdsSerializable
	{

		static inline constexpr bool sValue = std::is_base_of<GdsSerializable, TVal>::value;
	};

	enum : exception_category_value_t
	{
		E_EXCEPTION_CATEGORY_FRAMEWORK_CORE_GDS =
			cxdefs::declareExceptionCategory( ExceptionBaseType::FrameworkCore, ts3ExcCategoryIID( 0x7C ) ),
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
			cxdefs::declareExceptionCode( E_EXCEPTION_CATEGORY_FRAMEWORK_CORE_GDS, ts3ExcCodeIID( 0x71 ) ),
	};

} // namespace ts3

#endif // __TS3_CORE_GDS_COMMON_H__
