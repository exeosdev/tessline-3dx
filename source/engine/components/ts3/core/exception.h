
#pragma once

#ifndef __TS3_CORE_EXCEPTION_H__
#define __TS3_CORE_EXCEPTION_H__

#include "exceptionCommon.h"
#include <exception>

namespace ts3
{

    enum : exception_category_value_t
    {
        E_EXCEPTION_CATEGORY_GENERIC_DEBUG              = ecDeclareExceptionCategory( ExceptionBaseType::Debug, 0 ),
        E_EXCEPTION_CATEGORY_GENERIC_EXTERNAL           = ecDeclareExceptionCategory( ExceptionBaseType::External, 0 ),
        E_EXCEPTION_CATEGORY_GENERIC_FRAMEWORK_CORE     = ecDeclareExceptionCategory( ExceptionBaseType::FrameworkCore, 0 ),
        E_EXCEPTION_CATEGORY_GENERIC_FRAMEWORK_INTERNAL = ecDeclareExceptionCategory( ExceptionBaseType::FrameworkInternal, 0 ),
        E_EXCEPTION_CATEGORY_GENERIC_INTERRUPT          = ecDeclareExceptionCategory( ExceptionBaseType::Interrupt, 0 ),
        E_EXCEPTION_CATEGORY_GENERIC_MATH               = ecDeclareExceptionCategory( ExceptionBaseType::Math, 0 ),
        E_EXCEPTION_CATEGORY_GENERIC_RESULT_WRAPPER     = ecDeclareExceptionCategory( ExceptionBaseType::ResultWrapper, 0 ),
        E_EXCEPTION_CATEGORY_GENERIC_SYSTEM             = ecDeclareExceptionCategory( ExceptionBaseType::System, 0 ),
    };


    enum : exception_code_value_t
    {
        E_EXCEPTION_CODE_RESULT_ERROR = ecDeclareExceptionCode( E_EXCEPTION_CATEGORY_GENERIC_RESULT_WRAPPER, 0x01 )
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
	class ResultWrapperException : public ExceptionClass<ExceptionBaseType::ResultWrapper>
    {
    public:
        Result mResult;

    public:
        explicit ResultWrapperException( ExceptionInfo pInfo, Result pResult )
        : ExceptionClass( std::move( pInfo ) )
        , mResult( pResult )
        {}
    };


	template <ExceptionBaseType tExceptionBaseType, bool tIsValidType>
	struct ExceptionClassResolver
    {
	    using Type = UnknownException;
    };


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
