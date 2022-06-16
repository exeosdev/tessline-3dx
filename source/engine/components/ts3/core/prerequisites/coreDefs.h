
#pragma once

#ifndef __TS3_CORE_CORE_DEFS_H__
#define __TS3_CORE_CORE_DEFS_H__

#include <memory>

#define _ts3MakeStr2( pValue ) #pValue

#define ts3MakeStr( pValue ) _ts3MakeStr2( pValue )

#define ts3CaseReturn( pCaseValue, pReturnValue ) \
	case pCaseValue: return pReturnValue

#define ts3CaseReturnStr( pCaseValue ) \
	case pCaseValue: return #pCaseValue

namespace ts3
{

	template <typename TpEnum>
	class EnumTypeInfo;

	template <typename TpTarget, typename TpSrc>
	TS3_FUNC_NO_DISCARD inline TpTarget * dynamic_cast_check( TpSrc * pSource )
	{
		if( auto * targetPtr = dynamic_cast<TpTarget *>( pSource ) )
		{
			return targetPtr;
		}
		TS3_PCL_DEBUG_BREAK();
		return nullptr;
	}

	template <typename TpTarget, typename TpSrc>
	TS3_FUNC_NO_DISCARD inline TpTarget * dynamic_cast_throw( TpSrc * pSource )
	{
		if( auto * targetPtr = dynamic_cast<TpTarget *>( pSource ) )
		{
			return targetPtr;
		}
		throw std::bad_cast();
	}

	template <typename TpTarget, typename TpSrc>
	TS3_FUNC_NO_DISCARD inline std::shared_ptr<TpTarget> dynamic_ptr_cast_check( std::shared_ptr<TpSrc> pSource )
	{
		if( auto targetPtr = std::dynamic_pointer_cast<TpTarget>( std::move( pSource ) ) )
		{
			return targetPtr;
		}
		TS3_PCL_DEBUG_BREAK();
		return nullptr;
	}

	template <typename TpTarget, typename TpSrc>
	TS3_FUNC_NO_DISCARD inline std::shared_ptr<TpTarget> dynamic_ptr_cast_throw( std::shared_ptr<TpSrc> pSource )
	{
		if( auto targetPtr = std::dynamic_pointer_cast<TpTarget>( std::move( pSource ) ) )
		{
			return targetPtr;
		}
		throw std::bad_cast();
	}

    /// @brief Declares a namespace-scope function template for querying ts3::EnumTypeInfo for enum types.
    #define ts3EnableEnumTypeInfoSupport() \
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
        TS3_FUNC_NO_DISCARD inline const std::string & toString( TpEnum pValue )                                                 \
        {                                                                                                    \
        	/* Just call the enum-specific function. This allows moving the definition code to .cpp. */      \
        	return _typeinfo::toString##TpEnum( pValue );                                                    \
        }

    // Enable support for enum type info for the whole ts3:: namespace.
    ts3EnableEnumTypeInfoSupport();

}

#endif // __TS3_CORE_CORE_DEFS_H__
