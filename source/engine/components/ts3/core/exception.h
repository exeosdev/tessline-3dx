
#pragma once

#ifndef __TS3_CORE_EXCEPTION_H__
#define __TS3_CORE_EXCEPTION_H__

#include "exceptionCommon.h"
#include <exception>

namespace ts3
{

    enum : exception_category_value_t
    {
        E_EXCEPTION_CATEGORY_DEBUG              = ecDeclareExceptionCategory( ExceptionBaseType::Debug, 0 ),
        E_EXCEPTION_CATEGORY_EXTERNAL           = ecDeclareExceptionCategory( ExceptionBaseType::External, 0 ),
        E_EXCEPTION_CATEGORY_FRAMEWORK_CORE     = ecDeclareExceptionCategory( ExceptionBaseType::FrameworkCore, 0 ),
        E_EXCEPTION_CATEGORY_FRAMEWORK_INTERNAL = ecDeclareExceptionCategory( ExceptionBaseType::FrameworkInternal, 0 ),
        E_EXCEPTION_CATEGORY_INTERRUPT          = ecDeclareExceptionCategory( ExceptionBaseType::Interrupt, 0 ),
        E_EXCEPTION_CATEGORY_MATH               = ecDeclareExceptionCategory( ExceptionBaseType::Math, 0 ),
        E_EXCEPTION_CATEGORY_RESULT             = ecDeclareExceptionCategory( ExceptionBaseType::Result, 0 ),
        E_EXCEPTION_CATEGORY_SYSTEM             = ecDeclareExceptionCategory( ExceptionBaseType::System, 0 ),
    };


    enum : exception_code_value_t
    {
        E_EXCEPTION_CODE_RESULT_ERROR = ecDeclareExceptionCode( E_EXCEPTION_CATEGORY_RESULT, 0x01 )
    };

	/// @brief
	struct ExceptionInfo
	{
		// Exception code
		exception_code_value_t code;
		//
		std::string description;
		//
		FileLocationInfo fileLocationInfo;

		std::string toString() const;
	};

	/// @brief
	class Exception : public std::exception
	{
	public:
	    // Exception info.
	    ExceptionInfo mInfo;

	    // Text representation of the exception info.
	    std::string mString;

	public:
		/// @brief Constructor.
		/// @param pType Type of the exception, represented as ExceptionBaseType enum.
		/// @param pInfo Exception description, containing basic info about it.
		Exception( ExceptionInfo pInfo )
		: mInfo( std::move( pInfo ) )
		, mString( mInfo.toString() )
		{}

		virtual ~Exception() = default;

		/// @brief Returns the name of the exception.
		/// @return Name of the exception.
		virtual ExceptionBaseType getBaseType() const = 0;

		/// @brief Returns the name of the exception.
		/// @return Name of the exception.
		virtual const std::string & getBaseTypeName() const = 0;

		/// @brief
		/// @return
		virtual const char * what() const noexcept override
		{
		    return mString.c_str();
		}
	};

	/// @brief
	/// @tparam tExceptionBaseType
	template <ExceptionBaseType tExceptionBaseType>
	class ExceptionClass : public Exception
	{
	public:
		/// @brief
		static constexpr auto mBaseType = tExceptionBaseType;

	public:
		explicit ExceptionClass( ExceptionInfo pInfo )
		: Exception( std::move( pInfo ) )
		{}

		/// @refitem
		/// @return
		virtual ExceptionBaseType getBaseType() const override final
		{
			return tExceptionBaseType;
		}

		/// @refitem
		/// @return
		virtual const std::string & getBaseTypeName() const override final
		{
			// static std::once_flag initFlag;
			static std::string baseTypeName;

			// std::call_once( initFlag, []() {
			// 	const auto & enumInfo = queryEnumTypeInfo<ExceptionBaseType>();
			// 	const auto & baseTypeConstantInfo = enumInfo.getConstantMap().getByValue( tExceptionBaseType );
			// 	baseTypeName = baseTypeConstantInfo.name;
			// } );

			return baseTypeName;
		}
	};

	/// @brief Default base class for ExceptionBaseType::Debug.
	using UnknownException = ExceptionClass<ExceptionBaseType::Unknown>;

	/// @brief Default base class for ExceptionBaseType::Debug.
	using DebugException = ExceptionClass<ExceptionBaseType::Debug>;

	/// @brief Default base class for ExceptionBaseType::External.
	using ExternalException = ExceptionClass<ExceptionBaseType::External>;

	/// @brief Default base class for ExceptionBaseType::FrameworkInternal.
	using FrameworkCoreException = ExceptionClass<ExceptionBaseType::FrameworkInternal>;

	/// @brief Default base class for ExceptionBaseType::FrameworkInternal.
	using FrameworkInternalException = ExceptionClass<ExceptionBaseType::FrameworkInternal>;

	/// @brief Default base class for ExceptionBaseType::Interrupt.
	using InterruptException = ExceptionClass<ExceptionBaseType::Interrupt>;

	/// @brief Default base class for ExceptionBaseType::Math.
	using MathException = ExceptionClass<ExceptionBaseType::Math>;

	/// @brief Default base class for ExceptionBaseType::System.
	using SystemException = ExceptionClass<ExceptionBaseType::System>;

	/// @brief Specialized class for ResultWrapper exceptions. Adds Result object.
	class ResultException : public ExceptionClass<ExceptionBaseType::Result>
    {
    public:
        Result mResult;

    public:
        explicit ResultException( ExceptionInfo pInfo, Result pResult )
        : ExceptionClass( std::move( pInfo ) )
        , mResult( pResult )
        {}
    };



	template <ExceptionBaseType tExceptionBaseType, bool tIsValidType>
	struct ExceptionClassResolver
    {
	    using Type = UnknownException;
    };

	template <ExceptionBaseType tExceptionBaseType>
	struct ExceptionClassResolver<tExceptionBaseType, true>
    {
	    using Type = ExceptionClass<tExceptionBaseType>;
    };

	template <>
	struct ExceptionClassResolver<ExceptionBaseType::Result, true>
    {
	    using Type = ResultException;
    };

	template <ExceptionBaseType tExceptionBaseType>
	struct ExceptionBaseTypeClassProxy
    {
	    using Type = typename ExceptionClassResolver<tExceptionBaseType, ecIsExceptionBaseTypeValid( tExceptionBaseType )>::Type;
    };

	template <exception_category_value_t tExceptionCategory>
	struct ExceptionCategoryClassProxy
    {
	    using Type = typename ExceptionBaseTypeClassProxy<ecGetExceptionCategoryBaseType( tExceptionCategory )>::Type;
    };

	template <exception_code_value_t tExceptionCode>
	struct ExceptionCodeClassProxy
    {
	    using Type = typename ExceptionCategoryClassProxy<ecGetExceptionCodeCategory( tExceptionCode )>::Type;
    };

    #define ts3EnableExceptionSupport() \
        template <exception_category_value_t tExceptionCategory> \
        struct ExceptionCategoryClassProxy \
        { \
            using Type = typename ::ts3::ExceptionBaseTypeClassProxy<ecGetExceptionCategoryBaseType( tExceptionCategory )>::Type; \
        }; \
        template <exception_code_value_t tExceptionCode> \
        struct ExceptionCodeClassProxy \
        { \
            using Type = typename ExceptionCategoryClassProxy<ecGetExceptionCodeCategory( tExceptionCode )>::Type; \
        }

    #define ts3SetExceptionCategoryType( pExceptionCategory, pType ) \
        template <> \
        struct ExceptionCategoryClassProxy<pExceptionCategory> \
        { \
            using Type = pType; \
        }

    #define ts3SetExceptionCodeType( pExceptionCode, pType ) \
        template <> \
        struct ExceptionCodeClassProxy<pExceptionCode> \
        { \
            using Type = pType; \
        }

	template <typename TpException, typename... TpArgs>
	TS3_PCL_ATTR_NO_RETURN inline void throwException( ExceptionInfo pExceptionInfo, TpArgs &&... pArgs )
	{
		// TpException is a class derived from ExceptionClass<ExceptionBaseType>. It contains 'baseType'
		// member with type tag. It should match the type embedded within the code. In case of mismatch, there is
		// either a typo (in case of manual call) or a problem with the throwException() function defined below.
		if ( TpException::mBaseType != ecGetExceptionCodeBaseType( pExceptionInfo.code ) )
		{
			ts3DebugInterruptOnce();
		}

		throw TpException( std::move( pExceptionInfo ), std::forward<TpArgs>( pArgs )... );
	}

	template <typename TpException, typename... TpArgs>
	TS3_PCL_ATTR_NO_RETURN inline void throwException( exception_code_value_t pExceptionCode,
                                                       std::string pDescription,
                                                       const FileLocationInfo & pFileLocationInfo,
                                                       TpArgs &&... pArgs )
	{
	    ExceptionInfo exceptionInfo;
	    exceptionInfo.code = pExceptionCode;
	    exceptionInfo.description = std::move( pDescription );
	    exceptionInfo.fileLocationInfo = pFileLocationInfo;

	    throwException<TpException>( std::move( exceptionInfo ), std::forward<TpArgs>( pArgs )... );
    }

} // namespace ts3


#define ts3ThrowAuto( pExceptionCode ) \
    ::ts3::throwException<typename ExceptionCodeClassProxy<pExceptionCode>::Type>( pExceptionCode, "", ts3CurrentFileLocationInfo() )

#define ts3ThrowAutoEx( pExceptionCode, pDescription, ... ) \
    ::ts3::throwException<typename ExceptionCodeClassProxy<pExceptionCode>::Type>( pExceptionCode, pDescription, ts3CurrentFileLocationInfo(), __VA_ARGS__ )

#define ts3ThrowResult( pResult ) \
    ::ts3::throwException<typename ExceptionCodeClassProxy<E_EXCEPTION_CODE_RESULT_ERROR>::Type>( E_EXCEPTION_CODE_RESULT_ERROR, "", ts3CurrentFileLocationInfo(), pResult )

#define ts3ThrowResultEx( pResult, pDescription ) \
    ::ts3::throwException<typename ExceptionCodeClassProxy<E_EXCEPTION_CODE_RESULT_ERROR>::Type>( E_EXCEPTION_CODE_RESULT_ERROR, pDescription, ts3CurrentFileLocationInfo(), pResult )

#define exfCatchIntoWrapper( pResultWrapper ) \
	catch( const ::ts3::Result & eResult ) \
	{ \
        pResultWrapper.setResult( eResult ); \
    } \
    catch( const ::ts3::Result & eResult ) \
    { \
        pResultWrapper.setResult( eResult ); \
    } \
	catch( ... ) \
	{ \
		pResultWrapper.setException( std::current_exception() ); \
	}

#endif // __TS3_CORE_EXCEPTION_H__
