
#pragma once

#ifndef __TS3_CORE_EXCEPTION_COMMON_H__
#define __TS3_CORE_EXCEPTION_COMMON_H__

#include "prerequisites.h"

namespace ts3
{

    /// @brief Type used to represent exception codes.
    using exception_base_type_value_t = uint8;

    /// @brief Type used to represent exception codes.
    using exception_category_value_t = uint16;

    /// @brief Type used to represent exception codes.
    using exception_code_value_t = uint32;


    /// @brief Enum with possible base types for exception objects.
    ///
    /// ExceptionBaseType unambiguously identifies one of supported exception types defined in the framework.
    /// Base types are defined/provided by the library and there is no possibility of adding a custom one.
    /// Use a custom category (subtype) of an 'External' type for extending the system.
    enum class ExceptionBaseType : exception_base_type_value_t
    {
        //
        Unknown,
        // For debug-specific errors and messages.
        Debug,
        // All exceptions defined by the client libraries and frameworks.
        External,
        // For all normal, error-like exceptions. Defined primarily within the ::Core component.
        FrameworkCore,
        // Internal, implementation-details exception used by the framework.
        FrameworkInternal,
        // For exceptions used as an interrupts (for example in thread proc).
        Interrupt,
        // Defined primarily within the ::Math component.
        Math,
        // Dedicated type for exceptions carrying Results as an error indicator.
        ResultWrapper,
        // System-specific exceptions, extended in platform-specific manner. Defined primarily within the ::System component.
        System,
        //
        _Reserved
    };

    /// @brief Number of total enum values. Used for validation.
    inline constexpr auto CX_ENUM_EXCEPTION_BASE_TYPE_COUNT = static_cast<exception_base_type_value_t>( ExceptionBaseType::_Reserved );

    /// @brief Checks whether the specified value is a valid ExceptionBaseType.
    /// Used primarily by the library in the template-based mappings.
    inline constexpr bool ecIsExceptionBaseTypeValid( ExceptionBaseType pBaseType )
    {
        return ( ( exception_base_type_value_t )( pBaseType ) > 0u ) && ( ( exception_base_type_value_t )( pBaseType ) < CX_ENUM_EXCEPTION_BASE_TYPE_COUNT );
    }

    // Exception code: control key for validation.
    constexpr exception_code_value_t CX_EXCEPTION_CODE_CONTROL_KEY = 0xE7000000;

    // Exception code: mask for type component (ExceptionBaseType, 8-bit integer).
    constexpr exception_code_value_t CX_EXCEPTION_CODE_BASE_TYPE_MASK = 0x00FF0000;

    // Exception code: mask for category component (16-bit integer). Includes base type.
    constexpr exception_code_value_t CX_EXCEPTION_CODE_CATEGORY_MASK = 0x00FFFF00;

    // Exception code: mask for IID (internal ID) component (8-bit integer).
    constexpr exception_code_value_t CX_EXCEPTION_CODE_IID_MASK = 0x000000FF;

    // Exception category: mask for base type component (8-bit integer).
    constexpr exception_category_value_t CX_EXCEPTION_CATEGORY_BASE_TYPE_MASK = 0xFF00;

    ///
    inline constexpr exception_category_value_t ecDeclareExceptionCategory( ExceptionBaseType pBaseType, uint8 pCategoryIID )
    {
        return ( ( ( exception_base_type_value_t )( pBaseType ) << 8 ) | pCategoryIID );
    }

    ///
    inline constexpr exception_code_value_t ecDeclareExceptionCode( exception_category_value_t pCategory, uint8 pCodeIID )
    {
        return ( CX_EXCEPTION_CODE_CONTROL_KEY | ( ( exception_code_value_t )( pCategory ) << 8 ) | pCodeIID );
    }

    ///
    inline constexpr ExceptionBaseType ecGetExceptionCategoryBaseType( exception_category_value_t pCategory )
    {
        return ( ExceptionBaseType )( ( pCategory & 0xFF00 ) >> 8 );
    }

    ///
    inline constexpr ExceptionBaseType ecGetExceptionCodeBaseType( exception_code_value_t pCode )
    {
        return ( ExceptionBaseType )( ( pCode & CX_EXCEPTION_CODE_BASE_TYPE_MASK ) >> 16 );
    }

    ///
    inline constexpr exception_category_value_t ecGetExceptionCodeCategory( exception_code_value_t pCode )
    {
        return ( exception_category_value_t )( ( pCode & CX_EXCEPTION_CODE_CATEGORY_MASK ) >> 8 );
    }

    ///
    inline constexpr bool ecValidateExceptionCode( exception_code_value_t pCode )
    {
        return ( pCode & CX_EXCEPTION_CODE_CONTROL_KEY ) == CX_EXCEPTION_CODE_CONTROL_KEY;
    }

} // namespace ts3

#endif // __TS3_CORE_EXCEPTION_COMMON_H__
