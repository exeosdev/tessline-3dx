
#pragma once

#include "prerequisites.h"
#include <exception>
#include <mutex>

namespace ts3
{

	/// @brief Type used to represent exception codes.
	using exception_code_value_t = enum_default_value_t;

	/// @brief
	enum class ExceptionBaseType : uint8
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
		//
		ResultWrapper,
		// System-specific exceptions, extended in platform-specific manner. Defined primarily within the ::System component.
		System
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
	};


	/// @brief
	class Exception : public std::exception
	{
	public:
		/// @brief Constructor.
		/// @param pType Type of the exception, represented as ExceptionBaseType enum.
		/// @param pInfo Exception description, containing basic info about it.
		Exception( ExceptionInfo pInfo )
		: _info( std::move( pInfo ) )
		, _string( convertExceptionInfoToString( _info ) )
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
			return _string.c_str();
		}

		/// @brief Returns the info object of the exception.
		/// @return Info object of the exception.
		const ExceptionInfo & getInfo() const
		{
			return _info;
		}

		/// @brief
		/// @return
		std::string toString() const
		{
			return _string;
		}

	private:
		static std::string convertExceptionInfoToString( const ExceptionInfo & pInfo );

	private:
		// Exception info.
		ExceptionInfo _info;
		//
		std::string _string;
	};

	/// @brief
	/// @tparam tpExceptionBaseType
	template <ExceptionBaseType tpExceptionBaseType>
	class ExceptionSubtype : public Exception
	{
	public:
		/// @brief
		static constexpr auto baseType = tpExceptionBaseType;

	public:
		explicit ExceptionSubtype( ExceptionInfo pInfo )
		: Exception( std::move( pInfo ) )
		{}

		/// @refitem
		/// @return
		virtual ExceptionBaseType getBaseType() const override final
		{
			return tpExceptionBaseType;
		}

		/// @refitem
		/// @return
		virtual const std::string & getBaseTypeName() const override final
		{
			static std::once_flag initFlag;
			static std::string baseTypeName;

			// std::call_once( initFlag, []() {
			// 	const auto & enumInfo = queryEnumTypeInfo<ExceptionBaseType>();
			// 	const auto & baseTypeConstantInfo = enumInfo.getConstantMap().getByValue( tpExceptionBaseType );
			// 	baseTypeName = baseTypeConstantInfo.name;
			// } );

			return baseTypeName;
		}
	};

	/// @brief Specialized class for ResultWrapper exceptions. Adds result member and accessor.
	class ResultWrapperException : public ExceptionSubtype<ExceptionBaseType::ResultWrapper>
	{
	public:
		explicit ResultWrapperException( ExceptionInfo pInfo, Result pResult )
		: ExceptionSubtype( std::move( pInfo ) )
		{}

		/// @brief
		/// @return
		TS3_PCL_ATTR_NO_DISCARD const Result & getResult() const
		{
			return _result;
		}

	private:
		//
		Result _result;
	};

	/// @brief Default class for ExceptionBaseType::Debug.
	using DebugException = ExceptionSubtype<ExceptionBaseType::Debug>;
	/// @brief Default class for ExceptionBaseType::External.
	using ExternalException = ExceptionSubtype<ExceptionBaseType::External>;
	/// @brief Default class for ExceptionBaseType::FrameworkInternal.
	using FrameworkCoreException = ExceptionSubtype<ExceptionBaseType::FrameworkInternal>;
	/// @brief Default class for ExceptionBaseType::FrameworkInternal.
	using FrameworkInternalException = ExceptionSubtype<ExceptionBaseType::FrameworkInternal>;
	/// @brief Default class for ExceptionBaseType::Interrupt.
	using InterruptException = ExceptionSubtype<ExceptionBaseType::Interrupt>;
	/// @brief Default class for ExceptionBaseType::Math.
	using MathException = ExceptionSubtype<ExceptionBaseType::Math>;
	/// @brief Default class for ExceptionBaseType::System.
	using SystemException = ExceptionSubtype<ExceptionBaseType::System>;


	// Class type proxy. Used to get the actual exception type from an ExceptionBaseType value.
	template < ExceptionBaseType tpExceptionBaseType >
	struct ExceptionClassTypeProxy
	{
		using Type = ExceptionSubtype<tpExceptionBaseType>;
	};

	// Specialized definition for ResultWrapperException.
	template <>
	struct ExceptionClassTypeProxy<ExceptionBaseType::ResultWrapper>
	{
		using Type = ResultWrapperException;
	};


	namespace enumbits
	{

		// Exception code: control key for validation
		constexpr exception_code_value_t exceptionCodeControlKey = 0xE7000000;
		// Exception code: mask for type component (ExceptionBaseType, stored as 1 byte)
		constexpr exception_code_value_t exceptionCodeTypeMask = 0x00FF0000;
		// Exception code: mask for IID (internal ID) component (16-bit integer)
		constexpr exception_code_value_t exceptionCodeIIDMask = 0x0000FFFF;

		///
		inline constexpr exception_code_value_t declareExceptionCode( ExceptionBaseType pBaseType, uint16 pInternalID )
		{
			return ( exceptionCodeControlKey | ( ( exception_code_value_t )( pBaseType ) << 16 ) | pInternalID );
		}

		///
		inline constexpr ExceptionBaseType getExceptionCodeBaseType( exception_code_value_t pExceptionCode )
		{
			return ( ExceptionBaseType )( ( pExceptionCode & exceptionCodeTypeMask ) >> 16 );
		}

		///
		inline constexpr bool validateExceptionCode( exception_code_value_t pExceptionCode )
		{
			return ( pExceptionCode & exceptionCodeControlKey ) == exceptionCodeControlKey;
		}

	}


	enum : exception_code_value_t
	{
		EXC_ResultWrapper = enumbits::declareExceptionCode( ExceptionBaseType::ResultWrapper, 0x01 )
	};


	template < typename TpException, typename... TpArgs >
	TS3_PCL_ATTR_NO_RETURN inline void throwException( const ExceptionInfo & pInfo, TpArgs &&... pArgs )
	{
		// TpException is a class derived from ExceptionSubtype<ExceptionBaseType>. It contains 'baseType'
		// member with type tag. It should match the type embedded within the code. In case of mismatch, there is
		// either a typo (in case of manual call) or a problem with the throwException() function defined below.
		if ( TpException::baseType != enumbits::getExceptionCodeBaseType( pInfo.code ) )
		{
			ts3DebugInterruptOnce();
		}

		throw TpException( pInfo, std::forward<TpArgs>( pArgs )... );
	}

	template < exception_code_value_t tpExceptionCode, typename... TpArgs >
	TS3_PCL_ATTR_NO_RETURN inline void throwException( const char * pDescription, const FileLocationInfo & pFileLocationInfo, TpArgs &&... pArgs )
	{
		// Fetch the type tag embedded within the specified (presumably valid) exception code.
		constexpr auto exceptionTypeTag = enumbits::getExceptionCodeBaseType( tpExceptionCode );
		// The type is retrieved using pre-defined mappings. If the code isn't valid, this will immediately fail.
		using ExceptionType = typename ExceptionClassTypeProxy<exceptionTypeTag>::Type;

		ExceptionInfo exceptionInfo;
		exceptionInfo.code = tpExceptionCode;
		exceptionInfo.description = pDescription;
		exceptionInfo.fileLocationInfo = pFileLocationInfo;

		throwException<ExceptionType>( exceptionInfo, std::forward<TpArgs>( pArgs )... );
	}

	TS3_PCL_ATTR_NO_RETURN inline void ThrowFromResult( const char * pDescription, const FileLocationInfo & pFileLocationInfo, Result pResult )
	{
		// Fetch the type tag embedded within the specified (presumably valid) exception code.
		constexpr auto exceptionTypeTag = enumbits::getExceptionCodeBaseType( EXC_ResultWrapper );
		// The type is retrieved using pre-defined mappings. If the code isn't valid, this will immediately fail.
		using ExceptionType = typename ExceptionClassTypeProxy<exceptionTypeTag>::Type;

		ExceptionInfo exceptionInfo;
		exceptionInfo.code = EXC_ResultWrapper;
		exceptionInfo.description = pDescription;
		exceptionInfo.fileLocationInfo = pFileLocationInfo;

		throwException<ExceptionType>( exceptionInfo, std::move( pResult ) );
	}

	/// @brief
	struct ResultWrapper
	{
	public:
		//
		Result result;
		//
		std::exception_ptr exception;

	public:
		ResultWrapper() = default;

		explicit operator bool() const
		{
			return !isError();
		}

		void setResult( Result pResult )
		{
			result = pResult;
		}

		void setException( std::exception_ptr pExceptionPtr )
		{
			exception = std::move( pExceptionPtr );
		}

		void saveCurrentException()
		{
			exception = std::current_exception();
		}

		void rethrowException()
		{
			auto localException = std::exception_ptr();
			std::swap( exception, localException );
			std::rethrow_exception( localException );
		}

		TS3_PCL_ATTR_NO_DISCARD bool hasException() const
		{
			return exception ? true : false;
		}

		TS3_PCL_ATTR_NO_DISCARD bool hasResult() const
		{
			return !result.empty();
		}

		TS3_PCL_ATTR_NO_DISCARD bool isError() const
		{
			return ts3ResultIsError( result ) || exception;
		}
	};

	/// @brief
	template <typename TpValue>
	struct ReturnValueWrapper : public ResultWrapper
	{
	public:
		//
		mutable TpValue value;

	public:
		ReturnValueWrapper() = default;

		explicit ReturnValueWrapper( ResultWrapper pResultWrapper )
		: ResultWrapper( std::move( pResultWrapper ) )
		{}

		TpValue & operator*() const
		{
			return value;
		}

		TpValue * operator->() const
		{
			return &( value );
		}
	};

	/// @brief
	template <>
	struct ReturnValueWrapper<void> : public ResultWrapper
	{
	public:
		explicit ReturnValueWrapper() = default;
	};

}


#define exfThrow( pExceptionCode ) \
	::ts3::throwException<pExceptionCode>( "", ts3CurrentFileLocationInfo() )

#define exfThrowEx( pExceptionCode, pDescription ) \
	::ts3::throwException<pExceptionCode>( pDescription, ts3CurrentFileLocationInfo() )

#define exfThrowResult( pResult ) \
	::ts3::throwException<EXC_ResultWrapper>( "", ts3CurrentFileLocationInfo(), pResult )

#define exfCatchIntoWrapper( pResultWrapper ) \
	catch( const ::ts3::Result & eResult ) \
	{ \
		pResultWrapper.setResult( eResult ); \
	} \
	catch( ... ) \
	{ \
		pResultWrapper.setException( std::current_exception() ); \
	}
