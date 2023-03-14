
#pragma once

#ifndef __TS3_CORE_EXCEPTION_H__
#define __TS3_CORE_EXCEPTION_H__

#include "exceptionCommon.h"
#include <exception>

namespace ts3
{

    enum : exception_category_value_t
    {
        E_EXCEPTION_CATEGORY_DEBUG =
            cxdefs::declareExceptionCategory( ExceptionBaseType::Debug, ts3ExcCategoryIID( 0 ) ),

        E_EXCEPTION_CATEGORY_ENGINE_SUBMODULE =
            cxdefs::declareExceptionCategory( ExceptionBaseType::EngineSubModule, ts3ExcCategoryIID( 0 ) ),

        E_EXCEPTION_CATEGORY_FRAMEWORK_CORE =
            cxdefs::declareExceptionCategory( ExceptionBaseType::FrameworkCore, ts3ExcCategoryIID( 0 ) ),

        E_EXCEPTION_CATEGORY_INTERNAL =
            cxdefs::declareExceptionCategory( ExceptionBaseType::Internal, ts3ExcCategoryIID( 0 ) ),

        E_EXCEPTION_CATEGORY_INTERRUPT =
            cxdefs::declareExceptionCategory( ExceptionBaseType::Interrupt, ts3ExcCategoryIID( 0 ) ),

        E_EXCEPTION_CATEGORY_MATH =
            cxdefs::declareExceptionCategory( ExceptionBaseType::Math, ts3ExcCategoryIID( 0 ) ),

        E_EXCEPTION_CATEGORY_RESULT_PROXY =
            cxdefs::declareExceptionCategory( ExceptionBaseType::ResultProxy, ts3ExcCategoryIID( 0 ) ),

        E_EXCEPTION_CATEGORY_SYSTEM =
            cxdefs::declareExceptionCategory( ExceptionBaseType::System, ts3ExcCategoryIID( 0 ) ),

        E_EXCEPTION_CATEGORY_USER_EXTERNAL =
            cxdefs::declareExceptionCategory( ExceptionBaseType::UserExternal, ts3ExcCategoryIID( 0 ) ),
    };

    enum : exception_code_value_t
    {
        E_EXC_DEBUG_PLACEHOLDER =
            cxdefs::declareExceptionCode( E_EXCEPTION_CATEGORY_DEBUG, ts3ExcCodeIID( 0x01 ) ),

        E_EXC_RESULT_CODE_ERROR =
            cxdefs::declareExceptionCode( E_EXCEPTION_CATEGORY_RESULT_PROXY, ts3ExcCodeIID( 0x01 ) ),
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
	    ExceptionInfo mExceptionInfo;

	    // Text representation of the exception info.
	    std::string mString;

	public:
		/// @brief Constructor.
		/// @param pType Type of the exception, represented as ExceptionBaseType enum.
		/// @param pInfo Exception description, containing basic info about it.
		Exception( ExceptionInfo pExceptionInfo )
		: mExceptionInfo( std::move( pExceptionInfo ) )
		, mString( mExceptionInfo.toString() )
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
		explicit ExceptionClass( ExceptionInfo pExceptionInfo )
		: Exception( std::move( pExceptionInfo ) )
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

	/// @brief Default base class for ExceptionBaseType::EngineSubModule.
	using EngineSubModuleException = ExceptionClass<ExceptionBaseType::EngineSubModule>;

	/// @brief Default base class for ExceptionBaseType::FrameworkCore.
	using FrameworkCoreException = ExceptionClass<ExceptionBaseType::FrameworkCore>;

	/// @brief Default base class for ExceptionBaseType::Internal.
	using InternalException = ExceptionClass<ExceptionBaseType::Internal>;

	/// @brief Default base class for ExceptionBaseType::Interrupt.
	using InterruptException = ExceptionClass<ExceptionBaseType::Interrupt>;

	/// @brief Default base class for ExceptionBaseType::Math.
	using MathException = ExceptionClass<ExceptionBaseType::Math>;

	/// @brief Default base class for ExceptionBaseType::System.
	using SystemException = ExceptionClass<ExceptionBaseType::System>;

	/// @brief Default base class for ExceptionBaseType::UserExternal.
	using UserExternalException = ExceptionClass<ExceptionBaseType::UserExternal>;

	/// @brief Specialized class for ResultWrapper exceptions. Adds ResultProxy object.
	template <typename TValue, typename TErrorPredicate>
	class ResultProxyException : public ExceptionClass<ExceptionBaseType::ResultProxy>
    {
    public:
        ResultProxy<TValue, TErrorPredicate> mResult;

    public:
        ResultProxyException( ExceptionInfo pExceptionInfo, ResultProxy<TValue, TErrorPredicate> pResult )
        : ExceptionClass( std::move( pExceptionInfo ) )
        , mResult( pResult )
        {}
    };

	class ResultCodeException : public ResultProxyException<ResultCode::ValueType, ResultCode::ErrorPredicateType>
	{
	public:
		ResultCodeException( ExceptionInfo pExceptionInfo, ResultCode pResultCode )
		: ResultProxyException( std::move( pExceptionInfo ), pResultCode )
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
	struct ExceptionClassResolver<ExceptionBaseType::ResultProxy, true>
    {
		template <typename TValue, typename TErrorPredicate>
	    using Type = ResultProxyException<TValue, TErrorPredicate>;
    };

	template <ExceptionBaseType tExceptionBaseType>
	struct ExceptionBaseTypeClassProxy
    {
	    using Type = typename ExceptionClassResolver<tExceptionBaseType, cxdefs::isExceptionBaseTypeValid( tExceptionBaseType )>::Type;
    };

	template <exception_category_value_t tExceptionCategory>
	struct ExceptionCategoryClassProxy
    {
	    using Type = typename ExceptionBaseTypeClassProxy<cxdefs::getExceptionCategoryBaseType( tExceptionCategory )>::Type;
    };

	template <exception_code_value_t tExceptionCode>
	struct ExceptionCodeClassProxy
    {
	    using Type = typename ExceptionCategoryClassProxy<cxdefs::getExceptionCodeCategory( tExceptionCode )>::Type;
    };

	template <>
	struct ExceptionCodeClassProxy<E_EXC_RESULT_CODE_ERROR>
	{
		using Type = ResultCodeException;
	};

    #define ts3EnableCustomExceptionSupport() \
        template <exception_category_value_t tExceptionCategory> \
        struct ExceptionCategoryClassProxy \
        { \
            using Type = typename ::ts3::ExceptionBaseTypeClassProxy<ts3::cxdefs::getExceptionCategoryBaseType( tExceptionCategory )>::Type; \
        }; \
        template <exception_code_value_t tExceptionCode> \
        struct ExceptionCodeClassProxy \
        { \
            using Type = typename ExceptionCategoryClassProxy<ts3::cxdefs::getExceptionCodeCategory( tExceptionCode )>::Type; \
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

	template <typename TException, typename... TArgs>
	TS3_PCL_ATTR_NO_RETURN inline void throwException( ExceptionInfo pExceptionInfo, TArgs &&... pArgs )
	{
		// TException is a class derived from ExceptionClass<ExceptionBaseType>. It contains 'baseType'
		// member with type tag. It should match the type embedded within the code. In case of mismatch, there is
		// either a typo (in case of manual call) or a problem with the throwException() function defined below.
		ts3DebugAssert( TException::mBaseType == cxdefs::getExceptionCodeBaseType( pExceptionInfo.code ) );

		throw TException( std::move( pExceptionInfo ), std::forward<TArgs>( pArgs )... );
	}

	template <typename TException, typename... TArgs>
	TS3_PCL_ATTR_NO_RETURN inline void throwException( exception_code_value_t pExceptionCode,
                                                       std::string pDescription,
                                                       const FileLocationInfo & pFileLocationInfo,
                                                       TArgs &&... pArgs )
	{
	    ExceptionInfo exceptionInfo;
	    exceptionInfo.code = pExceptionCode;
	    exceptionInfo.description = std::move( pDescription );
	    exceptionInfo.fileLocationInfo = pFileLocationInfo;

	    throwException<TException>( std::move( exceptionInfo ), std::forward<TArgs>( pArgs )... );
    }

} // namespace ts3


#define ts3Throw( pExceptionCode ) \
    ::ts3::throwException<typename ExceptionCodeClassProxy<pExceptionCode>::Type>( pExceptionCode, #pExceptionCode, ts3CurrentFileLocationInfo() )

#define ts3ThrowDesc( pExceptionCode, pDescription ) \
    ::ts3::throwException<typename ExceptionCodeClassProxy<pExceptionCode>::Type>( pExceptionCode, pDescription, ts3CurrentFileLocationInfo() )

#define ts3ThrowEx( pExceptionCode, ... ) \
    ::ts3::throwException<typename ExceptionCodeClassProxy<pExceptionCode>::Type>( pExceptionCode, #pExceptionCode, ts3CurrentFileLocationInfo(), __VA_ARGS__ )

#define ts3ThrowExDesc( pExceptionCode, pDescription, ... ) \
    ::ts3::throwException<typename ExceptionCodeClassProxy<pExceptionCode>::Type>( pExceptionCode, pDescription, ts3CurrentFileLocationInfo(), __VA_ARGS__ )

#define ts3CatchIntoWrapper( pResultWrapper ) \
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
