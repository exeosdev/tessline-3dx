#pragma once

#ifndef __TS3_CORE_ENUM_COMMON_H__
#define __TS3_CORE_ENUM_COMMON_H__

namespace ts3
{

    template <typename TpEnum>
    class EnumTypeInfo;

    /// @brief Declares a namespace-scope function template for querying ts3::EnumTypeInfo for enum types.
    #define ts3TypeInfoEnableEnumSupport() \
        /* Template function used to retrieve an EnumTypeInfo object for an enum type. */ \
        /* Not implemented, specialized for every enum using ts3TypeInfoEnumDeclare. */  \
        template <typename TpEnum> const ::ts3::EnumTypeInfo<TpEnum> & queryEnumTypeInfo();

    /// @brief Declares an enum for which an ts3::EnumTypeInfo object can be queried.
    /// Extra arguments are used as a function specifier. You can pass e.g. declspec(dllexport/import) if the
    /// client library which provides the implementation is a dynamic library.
    /// Usage: call it once after an enum definition (in a header file), passing enum type as an argument.
    /// Example:
    /// enum class Color { Red, Green, Blue, Unknown };
    /// ts3TypeInfoEnumDeclare( Color );
    #define ts3TypeInfoEnumDeclare( TpEnum, ... ) \
        namespace _typeinfo {                                                                                \
            /* Forward declaration of an enum-specific query function, e.g. queryEnumTypeInfoColor() */      \
            ::ts3::EnumTypeInfo<TpEnum> & queryEnumTypeInfo##TpEnum();                                       \
            const std::string & toString##TpEnum( TpEnum );                                                  \
        } /* namespace _typeinfo */                                                                          \
        /* Implementation of queryEnumTypeInfo<TpEnum> template function specialization. */                  \
        template <> inline const ::ts3::EnumTypeInfo<TpEnum> & queryEnumTypeInfo<TpEnum>()                   \
        {                                                                                                    \
            /* Just call the enum-specific function. This allows moving the definition code to .cpp. */      \
            return _typeinfo::queryEnumTypeInfo##TpEnum();                                                   \
        }                                                                                                    \
        inline const std::string & toString( TpEnum pValue )                                                 \
        {                                                                                                    \
        	/* Just call the enum-specific function. This allows moving the definition code to .cpp. */      \
        	return _typeinfo::toString##TpEnum( pValue );                                                    \
        }

    ts3TypeInfoEnableEnumSupport();

} // namespace ts3

#endif // __TS3_CORE_ENUM_COMMON_H__
