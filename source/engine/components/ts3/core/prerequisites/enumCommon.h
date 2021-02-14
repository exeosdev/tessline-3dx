
#pragma once

namespace ts3
{

    template < typename TpEnum >
    class EnumTypeInfo;

    /// @brief Declares a namespace-scope function template for querying ts3::EnumTypeInfo for enum types.
    #define TS3_EnumDeclareQueryFunctionTemplate() \
        /* Template function used to retrieve an EnumTypeInfo object for an enum type. */ \
        /* Not implemented, specialized for every enum using TS3_EnumDeclareTypeInfo. */  \
        template <typename TpEnum> const ts3::EnumTypeInfo<TpEnum> & queryEnumTypeInfo();

    /// @brief Declares an enum for which an ts3::EnumTypeInfo object can be queried.
    /// Extra arguments are used as a function specifier. You can pass e.g. declspec(dllexport/import) if the
    /// client library which provides the implementation is a dynamic library.
    /// Usage: call it once after an enum definition (in a header file), passing enum type as an argument.
    /// Example:
    /// enum class Color { Red, Green, Blue, Unknown };
    /// TS3_EnumDeclareTypeInfo( Color );
    #define TS3_EnumDeclareTypeInfo( TpEnum, ... ) \
        /* Forward declaration of an enum-specific query function, e.g. queryEnumTypeInfoResultType() */ \
        __VA_ARGS__ ts3::EnumTypeInfo<TpEnum> & queryEnumTypeInfo##TpEnum();                                         \
        /* Implementation of queryEnumTypeInfo<TpEnum> template function specialization. */              \
        template <> inline const ts3::EnumTypeInfo<TpEnum> & queryEnumTypeInfo<TpEnum>()                 \
        {                                                                                                \
            /* Just call the enum-specific function. This allows moving the definition code to .cpp. */  \
            return queryEnumTypeInfo##TpEnum();                                                          \
        }

	#define TS3_EnumDeclareTypeInfoInternal( TpEnum, ... ) \
		TS3_EnumDeclareTypeInfo( TpEnum, TS3_API )
}
