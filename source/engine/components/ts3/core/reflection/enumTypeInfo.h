
#pragma once

#ifndef __ts3CORE_ENUM_TYPE_INFO_H__
#define __ts3CORE_ENUM_TYPE_INFO_H__

#include "../prerequisites.h"
#include <unordered_map>

namespace ts3
{

	/// @brief Basic properties of an enum to be registered.
	struct EnumProperties
	{
		/// @brief Name of an enum type which will be stored within its info object.
		std::string enumName;
	};


	template <typename TpEnum>
	struct EnumConstant
	{
		std::string name;
		TpEnum value;

		explicit operator TpEnum() const
		{
			return value;
		}
	};

	template <typename TpEnum>
	using EnumConstantList = std::vector< EnumConstant<TpEnum> >;


	template <typename TpEnum>
	class EnumConstantMap
	{
		friend class EnumTypeInfo<TpEnum>;

	public:
		constexpr EnumConstantMap() = default;

		const EnumConstant<TpEnum> & operator[]( const std::string & pName ) const
		{
			return getByName( pName );
		}

		const EnumConstant<TpEnum> & operator[]( TpEnum pValue ) const
		{
			return getByValue( pValue );
		}

		const EnumConstant<TpEnum> * findByName( const std::string & pName ) const
		{
			auto constantIter = _constantsByName.find( pName );
			if( constantIter != _constantsByName.end() )
			{
				auto & constant = _constants[constantIter->second];
				return &constant;
			}
			return nullptr;
		}

		const EnumConstant<TpEnum> * findByValue( TpEnum pValue ) const
		{
			auto constantIter = _constantsByValue.find( pValue );
			if( constantIter != _constantsByValue.end() )
			{
				auto & constant = _constants[constantIter->second];
				return &constant;
			}
			return nullptr;
		}

		const EnumConstant<TpEnum> & getByName( const std::string & pName ) const
		{
			auto constantIndex = _constantsByName.at( pName );
			return _constants[constantIndex];
		}

		const EnumConstant<TpEnum> & getByValue( TpEnum pValue ) const
		{
			auto constantIndex = _constantsByValue.at( pValue );
			return _constants[constantIndex];
		}

		const EnumConstantList<TpEnum> & list() const
		{
			return _constants;
		}

		size_t size() const
		{
			return _constants.size();
		}

		bool empty() const
		{
			return _constants.empty();
		}

	private:
		EnumConstantMap & add( TpEnum pValue, std::string pName )
		{
			auto constantIndex = _constants.size();
			auto & constantDef = _constants.emplace_back();
			constantDef.name = std::move( pName );
			constantDef.value = pValue;

			_constantsByName[constantDef.name] = constantIndex;
			_constantsByValue[constantDef.value] = constantIndex;

			return *this;
		}

	private:
		//
		EnumConstantList<TpEnum> _constants;
		//
		std::unordered_map<std::string, size_t> _constantsByName;
		//
		std::unordered_map<TpEnum, size_t> _constantsByValue;
	};


	template <typename TpEnum>
	inline typename EnumConstantList<TpEnum>::const_iterator begin( const EnumConstantMap<TpEnum> & pEnumConstantMap )
	{
		return pEnumConstantMap.list().begin();
	}

	template <typename TpEnum>
	inline typename EnumConstantList<TpEnum>::const_iterator end( const EnumConstantMap<TpEnum> & pEnumConstantMap )
	{
		return pEnumConstantMap.list().end();
	}


	template <typename TpEnum>
	class EnumTypeInfoInitializer;


	template <typename TpEnum>
	class EnumTypeInfo
	{
		 friend class EnumTypeInfoInitializer<TpEnum>;

	public:
		static inline const std::string sEmptyConstantName = "";

	public:
		template <typename TpInitFunction>
		explicit EnumTypeInfo( TpInitFunction pInitFunction )
		{
			pInitFunction( *this );
		}

		TS3_FUNC_NO_DISCARD const EnumConstantMap<TpEnum> & getConstantMap() const
		{
			return _constantMap;
		}

		TS3_FUNC_NO_DISCARD const std::string & getConstantName( TpEnum pValue ) const
		{
			if( auto * constantDef = _constantMap.findByValue( pValue ) )
			{
				return constantDef->name;
			}
			return sEmptyConstantName;
		}

		TS3_FUNC_NO_DISCARD TpEnum getConstantValue( const std::string & pName ) const
		{
			if( auto * constantDef = _constantMap.findByName( pName ) )
			{
				return constantDef->value;
			}

			return static_cast<TpEnum>( 0u );
		}

		TS3_FUNC_NO_DISCARD const std::string & getConstantNameOrDefault( TpEnum pValue, const std::string & pDefault = "" ) const
		{
			if( auto * constantDef = _constantMap.findByValue( pValue ) )
			{
				return constantDef->name;
			}
			return pDefault;
		}

		TS3_FUNC_NO_DISCARD TpEnum getConstantValueOrDefault( const std::string & pName, TpEnum pDefault = static_cast<TpEnum>( 0u ) ) const
		{
			if( auto * constantDef = _constantMap.findByName( pName ) )
			{
				return constantDef->value;
			}
			return pDefault;
		}

		TS3_FUNC_NO_DISCARD bool isValid() const
		{
			return !_constantMap.empty();
		}

	private:
		EnumTypeInfo & initialize( EnumProperties pProperties )
		{
			_properties = std::move( pProperties );
			return *this;
		}

		EnumTypeInfo & registerConstant( TpEnum pValue, std::string pName )
		{
			_constantMap.add( pValue, std::move( pName ) );
			return *this;
		}

	private:
		//
		EnumProperties _properties;
		//
		EnumConstantMap<TpEnum> _constantMap;
	};


	template <typename TpEnum>
	class EnumTypeInfoInitializer
	{
	public:
		static EnumTypeInfo<TpEnum> & initialize( EnumTypeInfo<TpEnum> & pEnumTypeInfo, EnumProperties pProperties )
		{
			return pEnumTypeInfo.initialize( std::move( pProperties ) );
		}

		static EnumTypeInfo<TpEnum> & registerConstant( EnumTypeInfo<TpEnum> & pEnumTypeInfo, TpEnum pValue, std::string pName )
		{
			return pEnumTypeInfo.registerConstant( pValue, std::move( pName ) );
		}
	};


	/// @brief Implements an enum's definition/initialization procedure.
	/// Usage and example: see below.
	#define ts3TypeInfoEnumDefine( TpEnum ) \
		/* Forward declaration of an enum-specific init method which registers all constants. */ \
		void initializeEnumTypeInfo##TpEnum( ::ts3::EnumTypeInfo<TpEnum> & ); \
		/* Here, we implement the method declared with ts3TypeInfoEnumDeclare. */ \
		::ts3::EnumTypeInfo<TpEnum> & _typeinfo::queryEnumTypeInfo##TpEnum() \
		{ \
			/* Create static EnumTypeInfo object and pass initializeEnumTypeInfoXXX function declared above. */ \
			/* EnumTypeInfo will call this function in its ctor and pass *this as an argument for initialization. */ \
			static ts3::EnumTypeInfo<TpEnum> enumTypeInfo{ initializeEnumTypeInfo##TpEnum }; \
			return enumTypeInfo; \
		} \
		const std::string & _typeinfo::toString##TpEnum( TpEnum pValue ) \
		{ \
			return queryEnumTypeInfo##TpEnum().getConstantName( pValue ); \
		} \
		void initializeEnumTypeInfo##TpEnum( ts3::EnumTypeInfo<TpEnum> & pEnumTypeInfo ) \

	/// @brief This is a basic initialization of the EnumTypeInfo object (pEnumTypeInfo).
	/// This is a continuation of initializeEnumTypeInfo##TpEnum function.
	/// Usage and example: see below.
	#define ts3TypeInfoEnumBegin( TpEnum ) \
        using EnumInitializer = ts3::EnumTypeInfoInitializer<TpEnum>; \
		/* Create basic definition of an enum - currently only its name. */ \
		ts3::EnumProperties enumProperties; \
		enumProperties.enumName = #TpEnum; \
		/* This is used for scoped enum constants (e.g. Color::Red). Prefix is the length of enum type name plus '::'. */ \
		const size_t enumNamePrefixLength = enumProperties.enumName.length() + 2; ( enumNamePrefixLength ); \
		/* Initialize the EnumTypeInfo object. */ \
		EnumInitializer::initialize( pEnumTypeInfo, std::move( enumProperties ) )

	/// @brief Registers a single enumerator of a scoped enum (enum class/struct) within the EnumTypeInfo object.
	#define ts3TypeInfoEnumRegisterClassConstant( pConstant ) \
		{ \
			/* Preprocessor does the job for us here. */ \
			const char * constantName = #pConstant; \
			/* Register the constant, but do not include the prefix (e.g. 'Color::' in case of 'Color::Red'). */ \
			EnumInitializer::registerConstant( pEnumTypeInfo, pConstant, constantName + enumNamePrefixLength ); \
		}

	/// @brief Registers a single enumerator of an unscoped enum within the EnumTypeInfo object.
	#define ts3TypeInfoEnumRegisterUnscopedConstant( pConstant ) \
		EnumInitializer::registerConstant( pEnumTypeInfo, pConstant, #pConstant )

	/*
	 * Basically, all above macros should be used together to form a definition of an EnumTypeInfo.
	 * Example usage:
	 *
	 * MyHeaderFile.h:
	 *
	 * enum class Color : uint32 {
	 *   Red, Green, Blue
	 * };
	 * enum DeviceFlags : uint32 {
	 *   DEVICE_FLAG_ACTIVE_BIT = 1, DEVICE_FLAG_PRIMARY_BIT = 2
	 * };
	 * ts3TypeInfoEnumDeclare( Color );
	 * ts3TypeInfoEnumDeclare( DeviceFlags );
	 *
	 * MySourceFile.cpp:
	 *
	 * ts3TypeInfoEnumDefine( Color )
	 * {
	 *   ts3TypeInfoEnumBegin( Color );
	 *   ts3TypeInfoEnumRegisterClassConstant( Color::Red );
	 *   ts3TypeInfoEnumRegisterClassConstant( Color::Green );
	 *   ts3TypeInfoEnumRegisterClassConstant( Color::Blue );
	 * }
	 * ts3TypeInfoEnumDefine( DeviceFlags )
	 * {
	 *   ts3TypeInfoEnumBegin( DeviceFlags );
	 *   ts3TypeInfoEnumRegisterUnscopedConstant( DEVICE_FLAG_ACTIVE_BIT );
	 *   ts3TypeInfoEnumRegisterUnscopedConstant( DEVICE_FLAG_PRIMARY_BIT );
	 * }
	 *
	 */

} // namespace ts3

#endif // __ts3CORE_ENUM_TYPE_INFO_H__
