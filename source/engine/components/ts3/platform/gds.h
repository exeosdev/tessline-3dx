
#ifndef __TS3_PLATFORM_GDS_H__
#define __TS3_PLATFORM_GDS_H__

#include "platform.h"
#include <cmath>

namespace ts3
{

	using gds_size_t = uint64;
	using gds_type_id_t = uint64;
	
	namespace gds
	{

		/// @brief Precision of the decimal part used for 'float' (de)serialization.
		inline constexpr uint32 CX_SERIALIZE_FLT32_DECIMAL_PRECISION = 10000u;

		/// @brief Precision of the decimal part used for 'double' (de)serialization.
		inline constexpr uint32 CX_SERIALIZE_FLT64_DECIMAL_PRECISION = 100000000u;

		/// @brief Precision of the decimal part used for 'long double' (de)serialization.
		inline constexpr uint64 CX_SERIALIZE_FLT80_DECIMAL_PRECISION = 1000000000000u;

		/// @brief Helper trait with a static bool 'sValue' indicating whether a type can be serialized using byte-wise copy.
		template <typename TVal>
		struct IsTriviallySerializable
		{
			static inline constexpr bool sValue =
				std::is_union<TVal>::value || ( std::is_class<TVal>::value && std::is_standard_layout<TVal>::value && std::is_trivial<TVal>::value );
		};

		/// @brief Helper trait with a static bool 'sValue' indicating whether a type is a character (does not include wchar_t).
		template <typename TVal>
		struct IsCharType
		{
			static inline constexpr bool sValue =
				std::is_same<TVal, char>::value || std::is_same<TVal, signed char>::value || std::is_same<TVal, unsigned char>::value;
		};

		/// @brief Helper trait with a static bool 'sValue' which yields true only for wchar_t.
		template <typename TVal>
		struct IsWideChar
		{
			static inline constexpr bool sValue = std::is_same<TVal, wchar_t>::value;
		};

		/// @brief Helper trait with a static bool 'sValue' indicating whether a type is an arithmetic type.
		/// In contrast to std::is_arithmetic, this trait does not concern character types to be arithmetic.
		template <typename TVal>
		struct IsArithmetic
		{
			static inline constexpr bool sValue =
				std::is_arithmetic<TVal>::value && !std::is_enum<TVal>::value && !IsCharType<TVal>::sValue && !IsWideChar<TVal>::sValue;
		};

		/// @brief Helper trait type. Provides static gds_size_t 'sValue' with a size of the binary representation of a type.
		template <typename TVal>
		struct ArithmeticTypeSerializedSize
		{
			static inline constexpr gds_size_t sValue = sizeof( TVal );
		};

		template <>
		struct ArithmeticTypeSerializedSize<wchar_t>
		{
			// wchar_t is always serialized using 4 bytes
			static inline constexpr gds_size_t sValue = sizeof( uint32 );
		};

		template <>
		struct ArithmeticTypeSerializedSize<float>
		{
			// Base + exponent
			static inline constexpr gds_size_t sValue = sizeof( int32 ) + sizeof( uint32 );
		};

		template <>
		struct ArithmeticTypeSerializedSize<double>
		{
			// Base + exponent
			static inline constexpr gds_size_t sValue = sizeof( int32 ) + sizeof( uint32 );
		};

		template <>
		struct ArithmeticTypeSerializedSize<long double>
		{
			// Base + exponent
			static inline constexpr gds_size_t sValue = sizeof( int64 ) + sizeof( uint32 );
		};

		/// @brief Helper utility type. Implements serialization/deserialization of an integral type of given size.
		/// @tparam tTypeSize The size of the integral type, in bytes.
		template <size_t tTypeSize>
		struct IntegralTypeSerializeProxy
		{
			/// @brief Serializes the specified integral value into byte representation using requested ByteOrder.
			///
			/// @tparam TVal Type of the input value, deduced automatically from the parameter.
			///
			/// @param pOutputBuffer Pointer to the beginning of the buffer for serialized data.
			/// @param pByteOrder	Byte order which should be used for types larger than 1 byte.
			/// @param pValue		Integral value to serialize.
			template <typename TVal>
			static void serializeIntegral( EByteOrder pByteOrder, byte * pOutputBuffer, const TVal pValue );

			/// @brief Deserializes the byte representation of an integral, stored in a given ByteOrder, and returns it.
			///
			/// @tparam TVal Type of the output value.
			///
			/// @param pInputData Pointer to the beginning of the serialized data.
			/// @param pByteOrder Byte order in which the input data has been serialized.
			template <typename TVal>
			static TVal deserializeIntegral( EByteOrder pByteOrder, const byte * pInputData );
		};

		/// @brief Specialization of IntegralTypeSerializeProxy for 1-byte integral types.
		template <>
		struct IntegralTypeSerializeProxy<1>
		{
			template <typename TVal>
			static inline void serializeIntegral( EByteOrder /* pByteOrder */, byte * pOutputBuffer, const TVal pValue )
			{
				*( reinterpret_cast<TVal *>( pOutputBuffer ) ) = pValue;
			}

			template <typename TVal>
			static inline TVal deserializeIntegral( EByteOrder /* pByteOrder */, const byte * pInputData )
			{
				return *( reinterpret_cast<const TVal *>( pInputData ) );
			}
		};

		/// @brief Specialization of IntegralTypeSerializeProxy for 2-byte integral types.
		template <>
		struct IntegralTypeSerializeProxy<2>
		{
			template <typename TVal>
			static inline void serializeIntegral( EByteOrder pByteOrder, byte * pOutputBuffer, const TVal pValue )
			{
				if( pByteOrder == EByteOrder::Native )
				{
					*( reinterpret_cast<TVal *>( pOutputBuffer ) ) = pValue;
				}
				else
				{
					*( reinterpret_cast<TVal *>( pOutputBuffer ) ) = static_cast<TVal>( TS3_PCL_BYTESWAP16( static_cast<uint16>( pValue ) ) );
				}
			}

			/// @brief Specialization of IntegralTypeSerializeProxy for 4-byte integral types.
			template <typename TVal>
			static inline TVal deserializeIntegral( EByteOrder pByteOrder, const byte * pInputData )
			{
				if( pByteOrder == EByteOrder::Native )
				{
					return *( reinterpret_cast<const TVal *>( pInputData ) );
				}
				else
				{
					return static_cast<TVal>( TS3_PCL_BYTESWAP16( *( reinterpret_cast<const uint16 *>( pInputData ) ) ) );
				}
			}
		};

		/// @brief Specialization of IntegralTypeSerializeProxy for 4-byte integral types.
		template <>
		struct IntegralTypeSerializeProxy<4>
		{
			template <typename TVal>
			static inline void serializeIntegral( EByteOrder pByteOrder, byte * pOutputBuffer, const TVal pValue )
			{
				if( pByteOrder == EByteOrder::Native )
				{
					*( reinterpret_cast<TVal *>( pOutputBuffer ) ) = pValue;
				}
				else
				{
					*( reinterpret_cast<TVal *>( pOutputBuffer ) ) = static_cast<TVal>( TS3_PCL_BYTESWAP32( static_cast<uint32>( pValue ) ) );
				}
			}

			template <typename TVal>
			static inline TVal deserializeIntegral( EByteOrder pByteOrder, const byte * pInputData )
			{
				if( pByteOrder == EByteOrder::Native )
				{
					return *( reinterpret_cast<const TVal *>( pInputData ) );
				}
				else
				{
					return static_cast<TVal>( TS3_PCL_BYTESWAP32( *( reinterpret_cast<const uint32 *>( pInputData ) ) ) );
				}
			}
		};

		/// @brief Specialization of IntegralTypeSerializeProxy for 8-byte integral types.
		template <>
		struct IntegralTypeSerializeProxy<8>
		{
			template <typename TVal>
			static inline void serializeIntegral( EByteOrder pByteOrder, byte * pOutputBuffer, const TVal pValue )
			{
				if( pByteOrder == EByteOrder::Native )
				{
					*( reinterpret_cast<TVal *>( pOutputBuffer ) ) = pValue;
				}
				else
				{
					*( reinterpret_cast<TVal *>( pOutputBuffer ) ) = static_cast<TVal>( TS3_PCL_BYTESWAP64( static_cast<uint64>( pValue ) ) );
				}
			}

			template <typename TVal>
			static inline TVal deserializeIntegral( EByteOrder pByteOrder, const byte * pInputData )
			{
				if( pByteOrder == EByteOrder::Native )
				{
					return *( reinterpret_cast<const TVal *>( pInputData ) );
				}
				else
				{
					return static_cast<TVal>( TS3_PCL_BYTESWAP64( *( reinterpret_cast<const uint64 *>( pInputData ) ) ) );
				}
			}
		};

		template <typename TVal, std::enable_if_t<IsCharType<TVal>::sValue || IsArithmetic<TVal>::sValue, int> = 0>
		inline void serializePrimitive( EByteOrder pByteOrder, byte * pOutputBuffer, const TVal pValue )
		{
			IntegralTypeSerializeProxy<sizeof( TVal )>::template serializeIntegral( pByteOrder, pOutputBuffer, pValue );
		}

		template <typename TVal, std::enable_if_t<IsWideChar<TVal>::sValue, int> = 0>
		inline void serializePrimitive( EByteOrder pByteOrder, byte * pOutputBuffer, const TVal pValue )
		{
			serializePrimitive<uint32>( pByteOrder, pOutputBuffer, static_cast<uint32>( pValue ) );
		}

		template <>
		inline void serializePrimitive<float>( EByteOrder pByteOrder, byte * pOutputBuffer, const float pValue )
		{
			int exponent; auto base = frexpf( pValue, &exponent ) * CX_SERIALIZE_FLT32_DECIMAL_PRECISION;
			serializePrimitive<uint32>( pByteOrder, pOutputBuffer, static_cast<uint32>( base ) );
			serializePrimitive<uint32>( pByteOrder, pOutputBuffer + sizeof( uint32 ), static_cast<uint32>( exponent ) );
		}

		template <>
		inline void serializePrimitive<double>( EByteOrder pByteOrder, byte * pOutputBuffer, const double pValue )
		{
			int exponent; auto base = frexp( pValue, &exponent ) * CX_SERIALIZE_FLT64_DECIMAL_PRECISION;
			serializePrimitive<uint32>( pByteOrder, pOutputBuffer, static_cast<uint32>( base ) );
			serializePrimitive<uint32>( pByteOrder, pOutputBuffer + sizeof( uint32 ), static_cast<uint32>( exponent ) );
		}

		template <>
		inline void serializePrimitive<long double>( EByteOrder pByteOrder, byte * pOutputBuffer, const long double pValue )
		{
			int exponent; auto base = frexpl( pValue, &exponent ) * CX_SERIALIZE_FLT80_DECIMAL_PRECISION;
			serializePrimitive<uint64>( pByteOrder, pOutputBuffer, static_cast<uint64>( base ) );
			serializePrimitive<uint32>( pByteOrder, pOutputBuffer + sizeof( uint32 ), static_cast<uint32>( exponent ) );
		}

		template <typename TVal, std::enable_if_t<IsCharType<TVal>::sValue || IsArithmetic<TVal>::sValue, int> = 0>
		inline TVal deserializePrimitive( EByteOrder pByteOrder, const byte * pInputData )
		{
			return IntegralTypeSerializeProxy<sizeof( TVal )>::template deserializeIntegral<TVal>( pByteOrder, pInputData );
		}

		template <typename TVal, std::enable_if_t<IsWideChar<TVal>::sValue, int> = 0>
		inline TVal deserializePrimitive( EByteOrder pByteOrder, const byte * pInputData )
		{
			return static_cast<TVal>( deserializePrimitive<uint32>( pByteOrder, pInputData ) );
		}

		template <>
		inline float deserializePrimitive<float>( EByteOrder pByteOrder, const byte * pInputData )
		{
			auto intBase = deserializePrimitive<uint32>( pByteOrder, pInputData );
			auto exponent = deserializePrimitive<uint32>( pByteOrder, pInputData + sizeof( intBase ) );
			return ldexpf( static_cast<float>( intBase ) / CX_SERIALIZE_FLT32_DECIMAL_PRECISION, exponent );
		}

		template <>
		inline double deserializePrimitive<double>( EByteOrder pByteOrder, const byte * pInputData )
		{
			auto intBase = deserializePrimitive<uint32>( pByteOrder, pInputData );
			auto exponent = deserializePrimitive<uint32>( pByteOrder, pInputData + sizeof( intBase ) );
			return ldexp( static_cast<double>( intBase ) / CX_SERIALIZE_FLT64_DECIMAL_PRECISION, exponent );
		}

		template <>
		inline long double deserializePrimitive<long double>( EByteOrder pByteOrder, const byte * pInputData )
		{
			auto intBase = deserializePrimitive<uint64>( pByteOrder, pInputData );
			auto exponent = deserializePrimitive<uint32>( pByteOrder, pInputData + sizeof( intBase ) );
			return ldexpl( static_cast<long double>( intBase ) / CX_SERIALIZE_FLT80_DECIMAL_PRECISION, exponent );
		}

		template <typename TRef, typename TInternal>
		struct ValueRef
		{
			using RefType = TRef;
			using InternalType = TInternal;

			std::reference_wrapper<TRef> value;
			mutable TInternal internalValue;

			template <typename... TArgs>
			ValueRef( TRef & pRef, TArgs && ...pArgs )
			: value( pRef )
			, internalValue( std::forward<TArgs>( pArgs )... )
			{}

			~ValueRef()
			{
				value.get() = static_cast<TRef>( internalValue );
			}

			TInternal & get() const
			{
				return internalValue;
			}

			void set( TInternal pValue ) const
			{
				value.get() = static_cast<TRef>( pValue );
			}
		};

		template <typename TRef, typename TInternal>
		struct ValueRef<const TRef, TInternal>
		{
			using RefType = TRef;
			using InternalType = TInternal;

			std::reference_wrapper<const TRef> value;

			ValueRef( const TRef & pRef )
			: value( pRef )
			{}

			~ValueRef() = default;

			TInternal get() const
			{
				return static_cast<TInternal>( value.get() );
			}
		};

		template <typename TCast>
		struct TypeCastTag
		{};

		using TypeCastNoneTag = TypeCastTag<void>;

		inline constexpr TypeCastNoneTag cvTypeCastNone {};

		template <typename TRef, typename TCast>
		struct TypeCastInfo
		{
			std::reference_wrapper<TRef> refWrapper;
			TypeCastTag<TCast> castTag;

			TypeCastInfo( TRef & pRef )
			: refWrapper( pRef )
			{}
		};

		template <typename TCast, typename TVal>
		inline constexpr TypeCastInfo<const TVal, TCast> typeCast( const TVal & pValue )
		{
			return { pValue };
		}

		template <typename TCast, typename TVal>
		inline constexpr TypeCastInfo<TVal, TCast> typeCast( TVal & pValue )
		{
			return { pValue };
		}

		template <typename TRef = uint64>
		using NativeIntRef = ValueRef<TRef, uint64>;

		template <typename TRef = uint64>
		using SizeTypeRef = ValueRef<TRef, uint64>;

		template <typename TRef = uint32>
		using TypeIDRef = ValueRef<TRef, uint32>;

		template <typename TVal, std::enable_if_t<std::is_integral<TVal>::value, int> = 0>
		inline NativeIntRef<const TVal> asNativeInt( const TVal & pValue )
		{
			return { pValue };
		}

		template <typename TVal, std::enable_if_t<std::is_integral<TVal>::value, int> = 0>
		inline SizeTypeRef<const TVal> asSizeType( const TVal & pValue )
		{
			return { pValue };
		}

		template <typename TVal, std::enable_if_t<std::is_integral<TVal>::value || std::is_enum<TVal>::value, int> = 0>
		inline TypeIDRef<const TVal> asTypeID( const TVal & pValue )
		{
			return { pValue };
		}

		template <typename TVal, std::enable_if_t<std::is_integral<TVal>::value, int> = 0>
		inline NativeIntRef<TVal> asNativeInt( TVal & pValue )
		{
			return { pValue };
		}

		template <typename TVal, std::enable_if_t<std::is_integral<TVal>::value, int> = 0>
		inline SizeTypeRef<TVal> asSizeType( TVal & pValue )
		{
			return { pValue };
		}

		template <typename TVal, std::enable_if_t<std::is_integral<TVal>::value || std::is_enum<TVal>::value, int> = 0>
		inline TypeIDRef<TVal> asTypeID( TVal & pValue )
		{
			return { pValue };
		}

		inline NativeIntRef<const NativeIntRef<>::RefType> emptyNativeInt()
		{
			return { NativeIntRef<>::RefType{} };
		}

		inline SizeTypeRef<const SizeTypeRef<>::RefType> emptySizeType()
		{
			return { SizeTypeRef<>::RefType{} };
		}

		inline TypeIDRef<const TypeIDRef<>::RefType> emptyTypeID()
		{
			return { TypeIDRef<>::RefType{} };
		}


		template <typename TVal, std::enable_if_t<IsTriviallySerializable<TVal>::sValue, int> = 0>
		inline constexpr gds_size_t evalByteSize( const TVal & )
		{
			return sizeof( TVal );
		}

		template <typename TVal, std::enable_if_t<IsArithmetic<TVal>::sValue || IsCharType<TVal>::sValue || IsWideChar<TVal>::sValue, int> = 0>
		inline constexpr gds_size_t evalByteSize( TVal )
		{
			return ArithmeticTypeSerializedSize<TVal>::sValue;
		}

		template <typename TVal, std::enable_if_t<std::is_enum<TVal>::value, int> = 0>
		inline constexpr gds_size_t evalByteSize( TVal )
		{
			return ArithmeticTypeSerializedSize<typename std::underlying_type<TVal>::type>::sValue;
		}

		template <typename TRef, typename TInternal>
		inline gds_size_t evalByteSize( const ValueRef<TRef, TInternal> & pValueRef )
		{
			return evalByteSize( pValueRef.get() );
		}

		template <typename TVal, typename TInternal, std::enable_if_t<std::is_void<TInternal>::value, int> = 0>
		inline gds_size_t evalByteSize( const TVal & pValue, const TypeCastTag<TInternal> & )
		{
			return evalByteSize( pValue );
		}

		template <typename TVal, typename TInternal, std::enable_if_t<!std::is_void<TInternal>::value, int> = 0>
		inline gds_size_t evalByteSize( const TVal & pValue, const TypeCastTag<TInternal> & )
		{
			return evalByteSize( ValueRef<const TVal, TInternal>{ pValue } );
		}

		template <typename TVal, typename TInternal, std::enable_if_t<!std::is_void<TInternal>::value, int> = 0>
		inline gds_size_t evalByteSize( const TypeCastInfo<TVal, TInternal> & pCastInfo )
		{
			return evalByteSize( pCastInfo.refWrapper.get(), pCastInfo.castTag );
		}


		template <typename TVal, std::enable_if_t<IsTriviallySerializable<TVal>::sValue, int> = 0>
		inline gds_size_t serialize( byte * pOutputBuffer, const TVal & pValue )
		{
			std::memcpy( pOutputBuffer, &pValue, sizeof( TVal ) );
			return sizeof( TVal );
		}

		template <typename TVal, std::enable_if_t<IsArithmetic<TVal>::sValue || IsCharType<TVal>::sValue || IsWideChar<TVal>::sValue, int> = 0>
		inline gds_size_t serialize( byte * pOutputBuffer, const TVal pValue )
		{
			serializePrimitive( EByteOrder::BigEndian, pOutputBuffer, pValue );
			return ArithmeticTypeSerializedSize<TVal>::sValue;
		}

		template <typename TVal, std::enable_if_t<std::is_enum<TVal>::value, int> = 0>
		inline gds_size_t serialize( byte * pOutputBuffer, const TVal pValue )
		{
			serializePrimitive( EByteOrder::BigEndian, pOutputBuffer, static_cast<typename std::underlying_type<TVal>::type>( pValue ) );
			return ArithmeticTypeSerializedSize<typename std::underlying_type<TVal>::type>::sValue;
		}

		template <typename TRef, typename TInternal>
		inline gds_size_t serialize( byte * pOutputBuffer, const ValueRef<TRef, TInternal> & pValueRef )
		{
			// ValueRef::get() returns the original value (of TRef type) cast to TInternal type.
			// Hence, it will trigger serialization as a different type than the original one.
			serializePrimitive( EByteOrder::BigEndian, pOutputBuffer, pValueRef.get() );
			return ArithmeticTypeSerializedSize<typename ValueRef<TRef, TInternal>::InternalType>::sValue;
		}

		template <typename TVal, typename TInternal, std::enable_if_t<std::is_void<TInternal>::value, int> = 0>
		inline gds_size_t serialize( byte * pOutputBuffer, const TVal & pValue, const TypeCastTag<TInternal> & )
		{
			return serialize( pOutputBuffer, pValue );
		}

		template <typename TVal, typename TInternal, std::enable_if_t<!std::is_void<TInternal>::value, int> = 0>
		inline gds_size_t serialize( byte * pOutputBuffer, const TVal & pValue, const TypeCastTag<TInternal> & )
		{
			return serialize( pOutputBuffer, ValueRef<TVal, TInternal>{ pValue } );
		}

		template <typename TVal, typename TInternal, std::enable_if_t<!std::is_void<TInternal>::value, int> = 0>
		inline gds_size_t serialize( byte * pOutputBuffer, const TypeCastInfo<TVal, TInternal> & pCastInfo )
		{
			return serialize( pOutputBuffer, pCastInfo.refWrapper.get(), pCastInfo.castTag );
		}


		template <typename TVal, std::enable_if_t<IsTriviallySerializable<TVal>::sValue, int> = 0>
		inline gds_size_t deserialize( const byte * pInputData, TVal & pValue )
		{
			std::memcpy( &pValue, pInputData, sizeof( TVal ) );
			return sizeof( TVal );
		}

		template <typename TVal, std::enable_if_t<IsArithmetic<TVal>::sValue || IsCharType<TVal>::sValue || IsWideChar<TVal>::sValue, int> = 0>
		inline gds_size_t deserialize( const byte * pInputData, TVal & pValue )
		{
			pValue = deserializePrimitive<TVal>( EByteOrder::BigEndian, pInputData );
			return ArithmeticTypeSerializedSize<TVal>::sValue;
		}

		template <typename TVal, std::enable_if_t<std::is_enum<TVal>::value, int> = 0>
		inline gds_size_t deserialize( const byte * pInputData, TVal & pValue )
		{
			pValue = static_cast<TVal>( deserializePrimitive<typename std::underlying_type<TVal>::type>( EByteOrder::BigEndian, pInputData ) );
			return ArithmeticTypeSerializedSize<typename std::underlying_type<TVal>::type>::sValue;
		}

		template <typename TRef, typename TInternal>
		inline gds_size_t deserialize( const byte * pInputData, const ValueRef<TRef, TInternal> & pValueRef )
		{
			pValueRef.set( deserializePrimitive<TInternal>( EByteOrder::BigEndian, pInputData ) );
			return ArithmeticTypeSerializedSize<TInternal>::sValue;
		}

		template <typename TVal, typename TInternal, std::enable_if_t<std::is_void<TInternal>::value, int> = 0>
		inline gds_size_t deserialize( const byte * pInputData, TVal & pRef, const TypeCastTag<TInternal> & )
		{
			return deserialize( pInputData, pRef );
		}

		template <typename TVal, typename TInternal, std::enable_if_t<!std::is_void<TInternal>::value, int> = 0>
		inline gds_size_t deserialize( const byte * pInputData, TVal & pRef, const TypeCastTag<TInternal> & )
		{
			return deserialize( pInputData, ValueRef<TVal, TInternal>{ pRef } );
		}

		template <typename TVal, typename TInternal, std::enable_if_t<!std::is_void<TInternal>::value, int> = 0>
		inline gds_size_t deserialize( const byte * pInputData, const TypeCastInfo<TVal, TInternal> & pCastInfo )
		{
			return deserialize( pInputData, pCastInfo.refWrapper.get(), pCastInfo.castTag );
		}


		/// @brief Special value set for InstanceMetaData::controlKey to indicate its correctness.
		inline constexpr uint32 CX_GDS_VALUE_META_DATA_CONTROL_KEY = 0x7CCC7333;

		/// @brief Metadata with additional information about an object (or a collection of such) serialized with GDS.
		///
		/// Metadata serves as a solution for a problem with serializing/deserializing objects of dynamic size.
		/// If serialized data is read from an external resource (file, pipe, network) and cannot be read at once,
		/// the problem arises: portion of the loaded data may not contain whole data for a single instance:
		///
		/// char inputBuffer[256];
		/// fread( serializedData.gds, 256, 1, inputBuffer );
		/// SomeStruct S;
		/// gds::deserialize( inputBuffer, S ); // -> crashes, this instance resulted in 670 bytes os serialized data
		///
		/// An object can be serialized with metadata using a dedicated function and then deserialized with its
		/// counterpart. This allows for getting the actual info about the data size and, thus - space needed.
		struct InstanceMetaData
		{
			/// @brief Control key. Set automatically to CX_GDS_VALUE_META_DATA_CONTROL_KEY.
			uint32 controlKey;

			/// @brief Size, in bytes, of serialized data (excluding the size of the metadata).
			uint64 objectDataSize = 0;

			/// @brief Size, in bytes, of the whole output data (objectDataSize + size of the metadata itself).
			uint64 outputBlockSize = 0;

			/// @brief Size of this struct, in bytes.
			static constexpr gds_size_t sByteSize = sizeof( controlKey ) + sizeof( objectDataSize ) + sizeof( outputBlockSize );

			/// @brief Returns true if this metadata is valid: control key is correct and object data size is not zero.
			constexpr explicit operator bool() const
			{
				return ( controlKey == CX_GDS_VALUE_META_DATA_CONTROL_KEY ) && ( objectDataSize > 0 );
			}

			/// @brief Returns the size of this struct, in bytes.
			constexpr size_t size() const
			{
				return sByteSize;
			}
		};

		/// @brief Usual serialize() method for InstanceMetaData struct.
		inline gds_size_t serialize( byte * pOutputBuffer, const InstanceMetaData & pValue )
		{
			gds_size_t byteSize = 0;
			byteSize += serialize( pOutputBuffer, pValue.controlKey );
			byteSize += serialize( pOutputBuffer + byteSize, pValue.objectDataSize );
			byteSize += serialize( pOutputBuffer + byteSize, pValue.outputBlockSize );
			return byteSize;
		}

		/// @brief Usual deserialize() method for InstanceMetaData struct.
		inline gds_size_t deserialize( const byte * pInputData, InstanceMetaData & pValue )
		{
			gds_size_t byteSize = 0;
			byteSize += deserialize( pInputData, pValue.controlKey );
			byteSize += deserialize( pInputData + byteSize, pValue.objectDataSize );
			byteSize += deserialize( pInputData + byteSize, pValue.outputBlockSize );
			return byteSize;
		}

		/// @brief Returns the size, in bytes, of the InstanceMetaData struct.
		inline constexpr gds_size_t getInstanceMetaDataSize()
		{
			return InstanceMetaData::sByteSize;
		}

		/// @brief Useful helper function to read InstanceMetaData from the specified serialized data block.
		inline InstanceMetaData readInstanceMetaData( const byte * pInputData )
		{
			InstanceMetaData valueMetaData;
			deserialize( pInputData, valueMetaData );
			return valueMetaData;
		}

	}

}

#endif // __TS3_PLATFORM_GDS_H__
