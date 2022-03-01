
#ifndef __TS3_PLATFORM_GDS_H__
#define __TS3_PLATFORM_GDS_H__

#include "platform.h"
#include <typeinfo>

namespace ts3
{

	using gds_size_t = uint64;
	using gds_size_marker_t = uint32;
	using gds_type_id_t = uint64;
	
	enum class EByteOrder : uint32
	{
		BigEndian = TS3_PCL_ENDIANNESS_BE,
		LittleEndian = TS3_PCL_ENDIANNESS_LE,
		Native = TS3_PCL_ENDIANNESS_NATIVE,
		NonNative = TS3_PCL_ENDIANNESS_NON_NATIVE,
		Default = static_cast<uint32>( BigEndian )
	};

	enum : gds_type_id_t
	{
		E_GDS_TYPE_ID_UNKNOWN = 0,
	};
	
	namespace gds
	{

		inline constexpr auto CX_SIZE_MARKER_SIZE = static_cast<gds_size_t>( sizeof( gds_size_marker_t ) );

		/// @brief Precision of the decimal part used for 'float' (de)serialization.
		inline constexpr uint32 CX_SERIALIZE_FLT32_DECIMAL_PRECISION = 10000u;

		/// @brief Precision of the decimal part used for 'double' (de)serialization.
		inline constexpr uint32 CX_SERIALIZE_FLT64_DECIMAL_PRECISION = 100000000u;

		/// @brief Precision of the decimal part used for 'long double' (de)serialization.
		inline constexpr uint64 CX_SERIALIZE_FLT80_DECIMAL_PRECISION = 1000000000000u;

		/// @brief Helper trait with a static bool 'sValue' indicating whether a type can be serialized using byte-wise copy.
		template <typename Tp>
		struct IsTriviallySerializable
		{
			static inline constexpr bool sValue =
				std::is_union<Tp>::value || ( std::is_class<Tp>::value && std::is_standard_layout<Tp>::value && std::is_trivial<Tp>::value );
		};

		/// @brief Helper trait with a static bool 'sValue' indicating whether a type is a character (does not include wchar_t).
		template <typename Tp>
		struct IsCharType
		{
			static inline constexpr bool sValue =
				std::is_same<Tp, char>::value || std::is_same<Tp, signed char>::value || std::is_same<Tp, unsigned char>::value;
		};

		/// @brief Helper trait with a static bool 'sValue' which yields true only for wchar_t.
		template <typename Tp>
		struct IsWideChar
		{
			static inline constexpr bool sValue = std::is_same<Tp, wchar_t>::value;
		};

		/// @brief Helper trait with a static bool 'sValue' indicating whether a type is an arithmetic type.
		/// In contrast to std::is_arithmetic, this trait does not concern character types to be arithmetic.
		template <typename Tp>
		struct IsArithmetic
		{
			static inline constexpr bool sValue =
				std::is_arithmetic<Tp>::value && !std::is_enum<Tp>::value && !IsCharType<Tp>::sValue && !IsWideChar<Tp>::sValue;
		};

		/// @brief Helper trait type. Provides static gds_size_t 'sValue' with a size of the binary representation of a type.
		template <typename Tp>
		struct ArithmeticTypeSerializedSize
		{
			static inline constexpr gds_size_t sValue = sizeof( Tp );
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
		/// @tparam tpTypeSize The size of the integral type, in bytes.
		template <size_t tpTypeSize>
		struct IntegralTypeSerializeProxy
		{
			/// @brief Serializes the specified integral value into byte representation using requested ByteOrder.
			///
			/// @tparam Tp Type of the input value, deduced automatically from the parameter.
			///
			/// @param pOutputBuffer Pointer to the beginning of the buffer for serialized data.
			/// @param pByteOrder    Byte order which should be used for types larger than 1 byte.
			/// @param pValue        Integral value to serialize.
			template <typename Tp>
			static void serializeIntegral( EByteOrder pByteOrder, byte * pOutputBuffer, const Tp pValue );

			/// @brief Deserializes the byte representation of an integral, stored in a given ByteOrder, and returns it.
			///
			/// @tparam Tp Type of the output value.
			///
			/// @param pInputData Pointer to the beginning of the serialized data.
			/// @param pByteOrder Byte order in which the input data has been serialized.
			template <typename Tp>
			static Tp deserializeIntegral( EByteOrder pByteOrder, const byte * pInputData );
		};

		/// @brief Specialization of IntegralTypeSerializeProxy for 1-byte integral types.
		template <>
		struct IntegralTypeSerializeProxy<1>
		{
			template <typename Tp>
			static inline void serializeIntegral( EByteOrder /* pByteOrder */, byte * pOutputBuffer, const Tp pValue )
			{
				*( reinterpret_cast<Tp *>( pOutputBuffer ) ) = pValue;
			}

			template <typename Tp>
			static inline Tp deserializeIntegral( EByteOrder /* pByteOrder */, const byte * pInputData )
			{
				return *( reinterpret_cast<const Tp *>( pInputData ) );
			}
		};

		/// @brief Specialization of IntegralTypeSerializeProxy for 2-byte integral types.
		template <>
		struct IntegralTypeSerializeProxy<2>
		{
			template <typename Tp>
			static inline void serializeIntegral( EByteOrder pByteOrder, byte * pOutputBuffer, const Tp pValue )
			{
				if( pByteOrder == EByteOrder::Native )
				{
					*( reinterpret_cast<Tp *>( pOutputBuffer ) ) = pValue;
				}
				else
				{
					*( reinterpret_cast<Tp *>( pOutputBuffer ) ) = static_cast<Tp>( TS3_PCL_BYTESWAP16( static_cast<uint16>( pValue ) ) );
				}
			}

			/// @brief Specialization of IntegralTypeSerializeProxy for 4-byte integral types.
			template <typename Tp>
			static inline Tp deserializeIntegral( EByteOrder pByteOrder, const byte * pInputData )
			{
				if( pByteOrder == EByteOrder::Native )
				{
					return *( reinterpret_cast<const Tp *>( pInputData ) );
				}
				else
				{
					return static_cast<Tp>( TS3_PCL_BYTESWAP16( *( reinterpret_cast<const uint16 *>( pInputData ) ) ) );
				}
			}
		};

		/// @brief Specialization of IntegralTypeSerializeProxy for 4-byte integral types.
		template <>
		struct IntegralTypeSerializeProxy<4>
		{
			template <typename Tp>
			static inline void serializeIntegral( EByteOrder pByteOrder, byte * pOutputBuffer, const Tp pValue )
			{
				if( pByteOrder == EByteOrder::Native )
				{
					*( reinterpret_cast<Tp *>( pOutputBuffer ) ) = pValue;
				}
				else
				{
					*( reinterpret_cast<Tp *>( pOutputBuffer ) ) = static_cast<Tp>( TS3_PCL_BYTESWAP32( static_cast<uint32>( pValue ) ) );
				}
			}

			template <typename Tp>
			static inline Tp deserializeIntegral( EByteOrder pByteOrder, const byte * pInputData )
			{
				if( pByteOrder == EByteOrder::Native )
				{
					return *( reinterpret_cast<const Tp *>( pInputData ) );
				}
				else
				{
					return static_cast<Tp>( TS3_PCL_BYTESWAP32( *( reinterpret_cast<const uint32 *>( pInputData ) ) ) );
				}
			}
		};

		/// @brief Specialization of IntegralTypeSerializeProxy for 8-byte integral types.
		template <>
		struct IntegralTypeSerializeProxy<8>
		{
			template <typename Tp>
			static inline void serializeIntegral( EByteOrder pByteOrder, byte * pOutputBuffer, const Tp pValue )
			{
				if( pByteOrder == EByteOrder::Native )
				{
					*( reinterpret_cast<Tp *>( pOutputBuffer ) ) = pValue;
				}
				else
				{
					*( reinterpret_cast<Tp *>( pOutputBuffer ) ) = static_cast<Tp>( TS3_PCL_BYTESWAP64( static_cast<uint64>( pValue ) ) );
				}
			}

			template <typename Tp>
			static inline Tp deserializeIntegral( EByteOrder pByteOrder, const byte * pInputData )
			{
				if( pByteOrder == EByteOrder::Native )
				{
					return *( reinterpret_cast<const Tp *>( pInputData ) );
				}
				else
				{
					return static_cast<Tp>( TS3_PCL_BYTESWAP64( *( reinterpret_cast<const uint64 *>( pInputData ) ) ) );
				}
			}
		};

		template <typename Tp, std::enable_if_t<IsCharType<Tp>::sValue || IsArithmetic<Tp>::sValue, int> = 0>
		inline void serializePrimitive( EByteOrder pByteOrder, byte * pOutputBuffer, const Tp pValue )
		{
			IntegralTypeSerializeProxy<sizeof( Tp )>::template serializeIntegral( pByteOrder, pOutputBuffer, pValue );
		}

		template <typename Tp, std::enable_if_t<IsWideChar<Tp>::sValue, int> = 0>
		inline void serializePrimitive( EByteOrder pByteOrder, byte * pOutputBuffer, const Tp pValue )
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

		template <typename Tp, std::enable_if_t<IsCharType<Tp>::sValue || IsArithmetic<Tp>::sValue, int> = 0>
		inline Tp deserializePrimitive( EByteOrder pByteOrder, const byte * pInputData )
		{
			return IntegralTypeSerializeProxy<sizeof( Tp )>::template deserializeIntegral<Tp>( pByteOrder, pInputData );
		}

		template <typename Tp, std::enable_if_t<IsWideChar<Tp>::sValue, int> = 0>
		inline Tp deserializePrimitive( EByteOrder pByteOrder, const byte * pInputData )
		{
			return static_cast<Tp>( deserializePrimitive<uint32>( pByteOrder, pInputData ) );
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

		template <typename TpRef, typename TpInternal>
		struct ValueRef
		{
			using RefType = TpRef;
			using InternalType = TpInternal;

			std::reference_wrapper<TpRef> value;

			ValueRef( TpRef & pRef )
			: value( pRef )
			{}

			void set( TpInternal pValue ) const
			{
				value.get() = static_cast<TpRef>( pValue );
			}

			TpInternal get() const
			{
				return static_cast<TpInternal>( value.get() );
			}
		};

		template <typename TpCast>
		struct TypeCastTag
		{};

		using TypeCastNoneTag = TypeCastTag<void>;

		inline constexpr TypeCastNoneTag cvTypeCastNone {};

		template <typename TpRef, typename TpCast>
		struct TypeCastInfo
		{
			std::reference_wrapper<TpRef> refWrapper;
			TypeCastTag<TpCast> castTag;

			TypeCastInfo( TpRef & pRef )
			: refWrapper( pRef )
			{}
		};

		template <typename TpCast, typename Tp>
		inline constexpr TypeCastInfo<const Tp, TpCast> typeCast( const Tp & pValue )
		{
			return { pValue };
		}

		template <typename TpCast, typename Tp>
		inline constexpr TypeCastInfo<Tp, TpCast> typeCast( Tp & pValue )
		{
			return { pValue };
		}

		template <typename TpRef = uint64>
		using NativeIntRef = ValueRef<TpRef, uint64>;

		template <typename TpRef = uint64>
		using SizeTypeRef = ValueRef<TpRef, uint64>;

		template <typename TpRef = uint32>
		using TypeIDRef = ValueRef<TpRef, uint32>;

		template <typename Tp, std::enable_if_t<std::is_integral<Tp>::value, int> = 0>
		inline NativeIntRef<const Tp> asNativeInt( const Tp & pValue )
		{
			return { pValue };
		}

		template <typename Tp, std::enable_if_t<std::is_integral<Tp>::value, int> = 0>
		inline SizeTypeRef<const Tp> asSizeType( const Tp & pValue )
		{
			return { pValue };
		}

		template <typename Tp, std::enable_if_t<std::is_integral<Tp>::value || std::is_enum<Tp>::value, int> = 0>
		inline TypeIDRef<const Tp> asTypeID( const Tp & pValue )
		{
			return { pValue };
		}

		template <typename Tp, std::enable_if_t<std::is_integral<Tp>::value, int> = 0>
		inline NativeIntRef<Tp> asNativeInt( Tp & pValue )
		{
			return { pValue };
		}

		template <typename Tp, std::enable_if_t<std::is_integral<Tp>::value, int> = 0>
		inline SizeTypeRef<Tp> asSizeType( Tp & pValue )
		{
			return { pValue };
		}

		template <typename Tp, std::enable_if_t<std::is_integral<Tp>::value || std::is_enum<Tp>::value, int> = 0>
		inline TypeIDRef<Tp> asTypeID( Tp & pValue )
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


		template <typename Tp, std::enable_if_t<std::is_integral<Tp>::value, int> = 0>
		inline constexpr gds_size_marker_t sizeMarker( const Tp pSizeMarker )
		{
			return static_cast<gds_size_marker_t>( pSizeMarker );
		}



		template <typename Tp, std::enable_if_t<IsTriviallySerializable<Tp>::sValue, int> = 0>
		inline gds_size_t serialize( byte * pOutputBuffer, const Tp & pValue )
		{
			serializePrimitive( EByteOrder::BigEndian, pOutputBuffer, sizeMarker( sizeof( Tp ) ) );
			std::memcpy( pOutputBuffer + CX_SIZE_MARKER_SIZE, &pValue, sizeof( Tp ) );
			return sizeof( Tp ) + CX_SIZE_MARKER_SIZE;
		}

		template <typename Tp, std::enable_if_t<IsArithmetic<Tp>::sValue || IsCharType<Tp>::sValue || IsWideChar<Tp>::sValue, int> = 0>
		inline gds_size_t serialize( byte * pOutputBuffer, const Tp pValue )
		{
			serializePrimitive( EByteOrder::BigEndian, pOutputBuffer, pValue );
			return ArithmeticTypeSerializedSize<Tp>::sValue;
		}

		template <typename Tp, std::enable_if_t<std::is_enum<Tp>::value, int> = 0>
		inline gds_size_t serialize( byte * pOutputBuffer, const Tp pValue )
		{
			serializePrimitive( EByteOrder::BigEndian, pOutputBuffer, static_cast<typename std::underlying_type<Tp>::type>( pValue ) );
			return ArithmeticTypeSerializedSize<typename std::underlying_type<Tp>::type>::sValue;
		}

		template <typename TpRef, typename TpInternal>
		inline gds_size_t serialize( byte * pOutputBuffer, const ValueRef<TpRef, TpInternal> & pValueRef )
		{
			// ValueRef::get() returns the original value (of TpRef type) cast to TpInternal type.
			// Hence, it will trigger serialization as a different type than the original one.
			serializePrimitive( EByteOrder::BigEndian, pOutputBuffer, pValueRef.get() );
			return ArithmeticTypeSerializedSize<typename ValueRef<TpRef, TpInternal>::InternalType>::sValue;
		}

		template <typename Tp, typename TpInternal, std::enable_if_t<std::is_void<TpInternal>::value, int> = 0>
		inline gds_size_t serialize( byte * pOutputBuffer, const Tp & pValue, const TypeCastTag<TpInternal> & )
		{
			return serialize( pOutputBuffer, pValue );
		}

		template <typename Tp, typename TpInternal, std::enable_if_t<!std::is_void<TpInternal>::value, int> = 0>
		inline gds_size_t serialize( byte * pOutputBuffer, const Tp & pValue, const TypeCastTag<TpInternal> & )
		{
			return serialize( pOutputBuffer, ValueRef<Tp, TpInternal>{ pValue } );
		}

		template <typename Tp, typename TpInternal, std::enable_if_t<!std::is_void<TpInternal>::value, int> = 0>
		inline gds_size_t serialize( byte * pOutputBuffer, const TypeCastInfo<Tp, TpInternal> & pCastInfo )
		{
			return serialize( pOutputBuffer, pCastInfo.refWrapper.get(), pCastInfo.castTag );
		}

		template <typename Tp>
		gds_size_t serializeAll( byte * pOutputBuffer, const Tp & pValue )
		{
			return serialize( pOutputBuffer, pValue );
		}

		template <typename Tp, typename... TpRest>
		gds_size_t serializeAll( byte * pOutputBuffer, const Tp & pValue, TpRest && ...pRest )
		{
			gds_size_t byteSize = serialize( pOutputBuffer, pValue );
			byteSize += serializeAll( pOutputBuffer + byteSize, std::forward<TpRest>( pRest )... );
			return byteSize;
		}



		template <typename Tp, std::enable_if_t<IsTriviallySerializable<Tp>::sValue, int> = 0>
		inline gds_size_t deserialize( const byte * pInputData, Tp & pValue )
		{
			const auto sizeMarker = deserializePrimitive<gds_size_marker_t>( EByteOrder::BigEndian, pInputData );
			ts3DebugAssert( sizeMarker == sizeof( Tp ) );
			std::memcpy( &pValue, pInputData + CX_SIZE_MARKER_SIZE, sizeof( Tp ) );
			return sizeof( Tp ) + CX_SIZE_MARKER_SIZE;
		}

		template <typename Tp, std::enable_if_t<IsArithmetic<Tp>::sValue || IsCharType<Tp>::sValue || IsWideChar<Tp>::sValue, int> = 0>
		inline gds_size_t deserialize( const byte * pInputData, Tp & pValue )
		{
			pValue = deserializePrimitive<Tp>( EByteOrder::BigEndian, pInputData );
			return ArithmeticTypeSerializedSize<Tp>::sValue;
		}

		template <typename Tp, std::enable_if_t<std::is_enum<Tp>::value, int> = 0>
		inline gds_size_t deserialize( const byte * pInputData, Tp & pValue )
		{
			pValue = static_cast<Tp>( deserializePrimitive<typename std::underlying_type<Tp>::type>( EByteOrder::BigEndian, pInputData ) );
			return ArithmeticTypeSerializedSize<typename std::underlying_type<Tp>::type>::sValue;
		}

		template <typename TpRef, typename TpInternal>
		inline gds_size_t deserialize( const byte * pInputData, const ValueRef<TpRef, TpInternal> & pValueRef )
		{
			pValueRef.set( deserializePrimitive<TpInternal>( EByteOrder::BigEndian, pInputData ) );
			return ArithmeticTypeSerializedSize<TpInternal>::sValue;
		}

		template <typename Tp, typename TpInternal, std::enable_if_t<std::is_void<TpInternal>::value, int> = 0>
		inline gds_size_t deserialize( const byte * pInputData, Tp & pRef, const TypeCastTag<TpInternal> & )
		{
			return deserialize( pInputData, pRef );
		}

		template <typename Tp, typename TpInternal, std::enable_if_t<!std::is_void<TpInternal>::value, int> = 0>
		inline gds_size_t deserialize( const byte * pInputData, Tp & pRef, const TypeCastTag<TpInternal> & )
		{
			return deserialize( pInputData, ValueRef<Tp, TpInternal>{ pRef } );
		}

		template <typename Tp, typename TpInternal, std::enable_if_t<!std::is_void<TpInternal>::value, int> = 0>
		inline gds_size_t deserialize( const byte * pInputData, const TypeCastInfo<Tp, TpInternal> & pCastInfo )
		{
			return deserialize( pInputData, pCastInfo.refWrapper.get(), pCastInfo.castTag );
		}

		template <typename Tp>
		gds_size_t deserializeAll( const byte * pInputData, Tp & pValue )
		{
			return deserialize( pInputData, pValue );
		}

		template <typename Tp, typename... TpRest>
		gds_size_t deserializeAll( const byte * pInputData, Tp & pValue, TpRest && ...pRest )
		{
			gds_size_t byteSize = deserialize( pInputData, pValue );
			byteSize += deserializeAll( pInputData + byteSize, std::forward<TpRest>( pRest )... );
			return byteSize;
		}


		template <typename Tp, std::enable_if_t<IsTriviallySerializable<Tp>::sValue, int> = 0>
		inline constexpr gds_size_t evalByteSize( const Tp & )
		{
			return sizeof( Tp );
		}

		template <typename Tp, std::enable_if_t<IsArithmetic<Tp>::sValue || IsCharType<Tp>::sValue || IsWideChar<Tp>::sValue, int> = 0>
		inline constexpr gds_size_t evalByteSize( Tp )
		{
			return ArithmeticTypeSerializedSize<Tp>::sValue;
		}

		template <typename Tp, std::enable_if_t<std::is_enum<Tp>::value, int> = 0>
		inline constexpr gds_size_t evalByteSize( Tp )
		{
			return ArithmeticTypeSerializedSize<typename std::underlying_type<Tp>::type>::sValue;
		}

		template <typename TpRef, typename TpInternal>
		inline gds_size_t evalByteSize( const ValueRef<TpRef, TpInternal> & pValueRef )
		{
			return evalByteSize( pValueRef.get() );
		}

		template <typename Tp, typename TpInternal, std::enable_if_t<std::is_void<TpInternal>::value, int> = 0>
		inline gds_size_t evalByteSize( const Tp & pValue, const TypeCastTag<TpInternal> & )
		{
			return evalByteSize( pValue );
		}

		template <typename Tp, typename TpInternal, std::enable_if_t<!std::is_void<TpInternal>::value, int> = 0>
		inline gds_size_t evalByteSize( const Tp & pValue, const TypeCastTag<TpInternal> & )
		{
			return evalByteSize( ValueRef<const Tp, TpInternal>{ pValue } );
		}

		template <typename Tp, typename TpInternal, std::enable_if_t<!std::is_void<TpInternal>::value, int> = 0>
		inline gds_size_t evalByteSize( const TypeCastInfo<Tp, TpInternal> & pCastInfo )
		{
			return evalByteSize( pCastInfo.refWrapper.get(), pCastInfo.castTag );
		}

		template <typename Tp>
		gds_size_t evalByteSizeAll( const Tp & pValue )
		{
			return evalByteSize( pValue );
		}

		template <typename Tp, typename... TpRest>
		gds_size_t evalByteSizeAll( const Tp & pValue, TpRest && ...pRest )
		{
			gds_size_t byteSize = evalByteSize( pValue );
			byteSize += evalByteSizeAll( std::forward<TpRest>( pRest )... );
			return byteSize;
		}


		inline constexpr uint32 CX_GDS_VALUE_META_INFO_CONTROL_KEY = 0x7CCC7333;

		struct ValueMetaInfo
		{
			uint32 controlKey;
			uint64 dataSize;

			static constexpr gds_size_t sByteSize = sizeof( controlKey ) + sizeof( dataSize );

			constexpr explicit operator bool() const
			{
				return controlKey == CX_GDS_VALUE_META_INFO_CONTROL_KEY;
			}
		};

		inline gds_size_t serialize( EByteOrder pByteOrder, byte * pOutputBuffer, const ValueMetaInfo & pValue )
		{
			gds_size_t byteSize = 0;
			byteSize += serialize( pOutputBuffer, pValue.controlKey );
			byteSize += serialize( pOutputBuffer + byteSize, pValue.dataSize );
			return byteSize;
		}

		inline gds_size_t deserialize( EByteOrder pByteOrder, const byte * pInputData, ValueMetaInfo & pValue )
		{
			gds_size_t byteSize = 0;
			byteSize += deserialize( pInputData, pValue.controlKey );
			byteSize += deserialize( pInputData + byteSize, pValue.dataSize );
			return byteSize;
		}

		inline constexpr gds_size_t evalByteSize( const ValueMetaInfo & pValue )
		{
			return ValueMetaInfo::sByteSize;
		}

		inline constexpr gds_size_t getValueMetaInfoSize()
		{
			return ValueMetaInfo::sByteSize;
		}

		template <typename Tp>
		inline gds_size_t serializeWithMetaInfo( byte * pOutputBuffer, const Tp & pValue )
		{
			const auto metaInfoSize = getValueMetaInfoSize();

			const auto byteSize = serialize( pOutputBuffer + metaInfoSize, pValue );

			ValueMetaInfo metaInfo;
			metaInfo.dataSize = byteSize;

			serialize( pOutputBuffer, metaInfo );

			return metaInfoSize + byteSize;
		}

		template <typename Tp>
		inline gds_size_t deserializeWithMetaInfo( const byte * pInputData, Tp & pValue )
		{
			ValueMetaInfo valueMetaInfo;
			const auto valueDataOffset = deserialize( pInputData, valueMetaInfo );

			const auto byteSize = deserialize( pInputData + valueDataOffset, pValue );

			return valueDataOffset + byteSize;
		}

		template <typename Tp>
		inline gds_size_t evalByteSizeWithMetaInfo( const Tp & pValue )
		{
			const auto metaInfoSize = getValueMetaInfoSize();
			const auto valueByteSize = evalByteSize( pValue );

			if( valueByteSize == 0 )
			{
				return 0;
			}

			return metaInfoSize + valueByteSize;
		}

		inline ValueMetaInfo readValueMetaInfo( const byte * pInputData )
		{
			ValueMetaInfo valueMetaInfo;
			deserialize( pInputData, valueMetaInfo );
			return valueMetaInfo;
		}

	}

}

#endif // __TS3_PLATFORM_GDS_H__
