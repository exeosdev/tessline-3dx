
#ifndef __TS3_PLATFORM_GDS_H__
#define __TS3_PLATFORM_GDS_H__

#include "platform.h"

namespace ts3
{

	using gds_size_t = uint64;
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

		/// @brief Precision of the decimal part used for 'float' (de)serialization.
		inline constexpr uint32 CX_SERIALIZE_FLT32_DECIMAL_PRECISION = 10000u;

		/// @brief Precision of the decimal part used for 'double' (de)serialization.
		inline constexpr uint32 CX_SERIALIZE_FLT64_DECIMAL_PRECISION = 100000000u;

		/// @brief Precision of the decimal part used for 'long double' (de)serialization.
		inline constexpr uint64 CX_SERIALIZE_FLT80_DECIMAL_PRECISION = 1000000000000u;

		/// @brief Helper trait type. Provides static boolean 'sValue' indicating whether a type can be serialized using byte-wise copy.
		template <typename Tp>
		struct IsTriviallySerializable
		{
			static inline constexpr bool sValue =
				std::is_union<Tp>::value || ( std::is_class<Tp>::value && std::is_standard_layout<Tp>::value && std::is_trivial<Tp>::value );
		};

		/// @brief Helper trait type. Provides static boolean 'sValue' indicating whether a type can be serialized using byte-wise copy.
		template <typename Tp>
		struct IsCharType
		{
			static inline constexpr bool sValue =
				std::is_same<Tp, char>::value || std::is_same<Tp, signed char>::value || std::is_same<Tp, unsigned char>::value;
		};

		/// @brief
		template <typename Tp>
		struct IsWideChar
		{
			static inline constexpr bool sValue = std::is_same<Tp, wchar_t>::value;
		};

		/// @brief
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
			// wchar_t is always 4 bytes
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

		/// @brief Helper utility type. Implements
		template <size_t tpTypeSize>
		struct IntegralTypeSerializeProxy;

		template <>
		struct IntegralTypeSerializeProxy<1>
		{
			template <typename Tp>
			static inline void serializeIntegral( byte * pOutputBuffer, EByteOrder, const Tp pValue )
			{
				*( reinterpret_cast<Tp *>( pOutputBuffer ) ) = pValue;
			}

			template <typename Tp>
			static inline Tp deserializeIntegral( const byte * pInputData, EByteOrder )
			{
				return *( reinterpret_cast<const Tp *>( pInputData ) );
			}
		};

		template <>
		struct IntegralTypeSerializeProxy<2>
		{
			template <typename Tp>
			static inline void serializeIntegral( byte * pOutputBuffer, EByteOrder pByteOrder, const Tp pValue )
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

			template <typename Tp>
			static inline Tp deserializeIntegral( const byte * pInputData, EByteOrder pByteOrder )
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

		template <>
		struct IntegralTypeSerializeProxy<4>
		{
			template <typename Tp>
			static inline void serializeIntegral( byte * pOutputBuffer, EByteOrder pByteOrder, const Tp pValue )
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
			static inline Tp deserializeIntegral( const byte * pInputData, EByteOrder pByteOrder )
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

		template <>
		struct IntegralTypeSerializeProxy<8>
		{
			template <typename Tp>
			static inline void serializeIntegral( byte * pOutputBuffer, EByteOrder pByteOrder, const Tp pValue )
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
			static inline Tp deserializeIntegral( const byte * pInputData, EByteOrder pByteOrder )
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
		inline void serializePrimitive( byte * pOutputBuffer, EByteOrder pByteOrder, const Tp pValue )
		{
			IntegralTypeSerializeProxy<sizeof( Tp )>::template serializeIntegral( pOutputBuffer, pByteOrder, pValue );
		}

		template <typename Tp, std::enable_if_t<IsWideChar<Tp>::sValue, int> = 0>
		inline void serializePrimitive( byte * pOutputBuffer, EByteOrder pByteOrder, const Tp pValue )
		{
			serializePrimitive<uint32>( pOutputBuffer, pByteOrder, static_cast<uint32>( pValue ) );
		}

		template <>
		inline void serializePrimitive<float>( byte * pOutputBuffer, EByteOrder pByteOrder, const float pValue )
		{
			int exponent; auto base = frexpf( pValue, &exponent ) * CX_SERIALIZE_FLT32_DECIMAL_PRECISION;
			serializePrimitive<uint32>( pOutputBuffer, pByteOrder, static_cast<uint32>( base ) );
			serializePrimitive<uint32>( pOutputBuffer + sizeof( uint32 ), pByteOrder, static_cast<uint32>( exponent ) );
		}

		template <>
		inline void serializePrimitive<double>( byte * pOutputBuffer, EByteOrder pByteOrder, const double pValue )
		{
			int exponent; auto base = frexp( pValue, &exponent ) * CX_SERIALIZE_FLT64_DECIMAL_PRECISION;
			serializePrimitive<uint32>( pOutputBuffer, pByteOrder, static_cast<uint32>( base ) );
			serializePrimitive<uint32>( pOutputBuffer + sizeof( uint32 ), pByteOrder, static_cast<uint32>( exponent ) );
		}

		template <>
		inline void serializePrimitive<long double>( byte * pOutputBuffer, EByteOrder pByteOrder, const long double pValue )
		{
			int exponent; auto base = frexpl( pValue, &exponent ) * CX_SERIALIZE_FLT80_DECIMAL_PRECISION;
			serializePrimitive<uint64>( pOutputBuffer, pByteOrder, static_cast<uint64>( base ) );
			serializePrimitive<uint32>( pOutputBuffer + sizeof( uint32 ), pByteOrder, static_cast<uint32>( exponent ) );
		}

		template <typename Tp, std::enable_if_t<IsCharType<Tp>::sValue || IsArithmetic<Tp>::sValue, int> = 0>
		inline Tp deserializePrimitive( const byte * pInputData, EByteOrder pByteOrder )
		{
			return IntegralTypeSerializeProxy<sizeof( Tp )>::template deserializeIntegral<Tp>( pInputData, pByteOrder );
		}

		template <typename Tp, std::enable_if_t<IsWideChar<Tp>::sValue, int> = 0>
		inline Tp deserializePrimitive( const byte * pInputData, EByteOrder pByteOrder )
		{
			return static_cast<Tp>( deserializePrimitive<uint32>( pInputData, pByteOrder ) );
		}

		template <>
		inline float deserializePrimitive<float>( const byte * pInputData, EByteOrder pByteOrder )
		{
			auto intBase = deserializePrimitive<uint32>( pInputData, pByteOrder );
			auto exponent = deserializePrimitive<uint32>( pInputData + sizeof( intBase ), pByteOrder );
			return ldexpf( static_cast<float>( intBase ) / CX_SERIALIZE_FLT32_DECIMAL_PRECISION, exponent );
		}

		template <>
		inline double deserializePrimitive<double>( const byte * pInputData, EByteOrder pByteOrder )
		{
			auto intBase = deserializePrimitive<uint32>( pInputData, pByteOrder );
			auto exponent = deserializePrimitive<uint32>( pInputData + sizeof( intBase ), pByteOrder );
			return ldexp( static_cast<double>( intBase ) / CX_SERIALIZE_FLT64_DECIMAL_PRECISION, exponent );
		}

		template <>
		inline long double deserializePrimitive<long double>( const byte * pInputData, EByteOrder pByteOrder )
		{
			auto intBase = deserializePrimitive<uint64>( pInputData, pByteOrder );
			auto exponent = deserializePrimitive<uint32>( pInputData + sizeof( intBase ), pByteOrder );
			return ldexpl( static_cast<long double>( intBase ) / CX_SERIALIZE_FLT80_DECIMAL_PRECISION, exponent );
		}

		template <typename TpVal, typename TpSrc>
		struct ValueProxy
		{
			using ValueType = TpVal;

			TpVal value;

			ValueProxy() = default;

			constexpr ValueProxy( const TpSrc pValue )
			: value( static_cast<TpVal>( pValue ) )
			{}

			ValueProxy( ValueProxy && ) = default;
			ValueProxy & operator=( ValueProxy && ) = default;

			TpVal get() const
			{
				return value;
			}
		};

		template <typename TpVal, typename TpTgt>
		struct RefProxy
		{
			using ValueType = TpVal;

			TpTgt * targetPtr = nullptr;

			constexpr RefProxy( TpTgt & pTarget )
			: targetPtr( &pTarget )
			{}

			RefProxy( RefProxy && ) = default;
			RefProxy & operator=( RefProxy && ) = default;

			explicit operator bool() const
			{
				return targetPtr != nullptr;
			}

			void set( const TpVal & pValue ) const
			{
				*targetPtr = static_cast<TpTgt>( pValue );
			}

			const TpVal & get() const
			{
				return *targetPtr;
			}
		};

		template <typename TpSrc = uint32>
		using UI32ValueProxy = ValueProxy<uint32, TpSrc>;

		template <typename TpSrc = uint64>
		using UI64ValueProxy = ValueProxy<uint64, TpSrc>;

		template <typename TpTgt = uint32>
		using UI32RefProxy = RefProxy<uint32, TpTgt>;

		template <typename TpTgt = uint64>
		using UI64RefProxy = RefProxy<uint64, TpTgt>;

		template <typename TpVal, typename TpSrc>
		struct ValueCast
		{
			using ValueType = TpVal;
			std::reference_wrapper<const TpSrc> srcRef;
		};

		template <>
		struct ValueCast<void, void>
		{};

		inline constexpr ValueCast<void, void> cvValueCastNone {};

		template <typename TpVal, typename TpTgt>
		struct RefCast
		{
			using ValueType = TpVal;
			std::reference_wrapper<TpTgt> targetRef;
		};

		template <>
		struct RefCast<void, void>
		{};

		inline constexpr RefCast<void, void> cvRefCastNone {};

		template <typename TpCast, typename TpSrc>
		inline ValueCast<TpCast, TpSrc> valueTypeCast( const TpSrc & pSrcValue )
		{
			return { pSrcValue };
		}

		template <typename TpCast, typename TpTgt, std::enable_if_t<!std::is_const<TpTgt>::value, int> = 0>
		inline RefCast<TpCast, TpTgt> refTypeCast( TpTgt & pTargetRef )
		{
			return { pTargetRef };
		}


		template <typename Tp, std::enable_if_t<std::is_integral<Tp>::value, int> = 0>
		constexpr inline UI64ValueProxy<Tp> asNativeIntValue( const Tp pValue )
		{
			return { pValue };
		}

		template <typename Tp, std::enable_if_t<std::is_integral<Tp>::value, int> = 0>
		constexpr inline UI64ValueProxy<Tp> asSizeTypeValue( const Tp pValue )
		{
			return { pValue };
		}

		template <typename Tp, std::enable_if_t<std::is_integral<Tp>::value || std::is_enum<Tp>::value, int> = 0>
		constexpr inline UI64ValueProxy<Tp> asTypeIDValue( const Tp pValue )
		{
			return { pValue };
		}

		constexpr inline UI64ValueProxy<> emptyNativeIntValue()
		{
			return {};
		}

		constexpr inline UI64ValueProxy<> emptySizeTypeValue()
		{
			return {};
		}

		constexpr inline UI64ValueProxy<> emptyTypeIDValue()
		{
			return {};
		}

		template <typename Tp, std::enable_if_t<std::is_integral<Tp>::value, int> = 0>
		inline UI64RefProxy<Tp> asNativeIntRef( Tp & pValue )
		{
			return { pValue };
		}

		template <typename Tp, std::enable_if_t<std::is_integral<Tp>::value, int> = 0>
		inline UI64RefProxy<Tp> asSizeTypeRef( Tp & pValue )
		{
			return { pValue };
		}

		template <typename Tp, std::enable_if_t<std::is_integral<Tp>::value || std::is_enum<Tp>::value, int> = 0>
		inline UI64RefProxy<Tp> asTypeIDRef( Tp & pValue )
		{
			return { pValue };
		}


		template <typename Tp, std::enable_if_t<IsTriviallySerializable<Tp>::sValue, int> = 0>
		inline gds_size_t serialize( byte * pOutputBuffer, EByteOrder /* pByteOrder */, const Tp & pValue )
		{
			std::memcpy( pOutputBuffer, &pValue, sizeof( Tp ) );
			return sizeof( Tp );
		}

		template <typename Tp, std::enable_if_t<IsArithmetic<Tp>::sValue || IsCharType<Tp>::sValue || IsWideChar<Tp>::sValue, int> = 0>
		inline gds_size_t serialize( byte * pOutputBuffer, EByteOrder pByteOrder, const Tp pValue )
		{
			serializePrimitive( pOutputBuffer, pByteOrder, pValue );
			return ArithmeticTypeSerializedSize<Tp>::sValue;
		}

		template <typename Tp, std::enable_if_t<std::is_enum<Tp>::value, int> = 0>
		inline gds_size_t serialize( byte * pOutputBuffer, EByteOrder pByteOrder, const Tp pValue )
		{
			serializePrimitive( pOutputBuffer, pByteOrder, static_cast<typename std::underlying_type<Tp>::type>( pValue ) );
			return ArithmeticTypeSerializedSize<typename std::underlying_type<Tp>::type>::sValue;
		}

//		template <typename TpCast, typename TpSrc>
//		inline ValueCast<TpCast, TpSrc> valueTypeCast( const TpSrc & pSrcValue )
//		{
//			return { pSrcValue };
//		}
//
//		template <typename TpCast, typename TpTgt, std::enable_if_t<!std::is_const<TpTgt>::value, int> = 0>
//				inline RefCast<TpCast, TpTgt> refTypeCast( TpTgt & pTargetRef )
//				{
//					return { pTargetRef };
//				}

		template <typename TpVal, typename TpSrc>
		inline gds_size_t serialize( byte * pOutputBuffer, EByteOrder pByteOrder, const ValueProxy<TpVal, TpSrc> & pValueProxy )
		{
			serializePrimitive( pOutputBuffer, pByteOrder, pValueProxy.get() );
			return ArithmeticTypeSerializedSize<typename ValueProxy<TpVal, TpSrc>::ValueType>::sValue;
		}

		template <typename TpVal>
		inline gds_size_t serialize( byte * pOutputBuffer, EByteOrder pByteOrder, const TpVal & pValue, const ValueCast<void, void> & )
		{
			return serialize( pOutputBuffer, pByteOrder, pValue );
		}

		template <typename TpVal, typename TpSrc>
		inline gds_size_t serialize( byte * pOutputBuffer, EByteOrder pByteOrder, const TpVal & pValue, const ValueCast<TpVal, TpSrc> & )
		{
			return serialize( pOutputBuffer, pByteOrder, ValueProxy<TpVal, TpSrc>{ pValue } );
		}

		template <typename TpVal, typename TpSrc, std::enable_if_t<!std::is_void<TpVal>::value && !std::is_void<TpSrc>::value, int> = 0>
		inline gds_size_t serialize( byte * pOutputBuffer, EByteOrder pByteOrder, const ValueCast<TpVal, TpSrc> & pValueCast )
		{
			return serialize( pOutputBuffer, pByteOrder, pValueCast.srcRef, pValueCast );
		}

		template <typename Tp>
		gds_size_t serializeAll( byte * pOutputBuffer, EByteOrder pByteOrder, const Tp & pValue )
		{
			return serialize( pOutputBuffer, pByteOrder, pValue );
		}

		template <typename Tp, typename... TpRest>
		gds_size_t serializeAll( byte * pOutputBuffer, EByteOrder pByteOrder, const Tp & pValue, TpRest && ...pRest )
		{
			gds_size_t byteSize = serialize( pOutputBuffer, pByteOrder, pValue );
			byteSize += serializeAll( pOutputBuffer + byteSize, pByteOrder, std::forward<TpRest>( pRest )... );
			return byteSize;
		}



		template <typename Tp, std::enable_if_t<IsTriviallySerializable<Tp>::sValue, int> = 0>
		inline gds_size_t deserialize( const byte * pInputData, EByteOrder /* pByteOrder */, Tp & pValue )
		{
			std::memcpy( &pValue, pInputData, sizeof( Tp ) );
			return sizeof( Tp );
		}

		template <typename Tp, std::enable_if_t<IsArithmetic<Tp>::sValue || IsCharType<Tp>::sValue || IsWideChar<Tp>::sValue, int> = 0>
		inline gds_size_t deserialize( const byte * pInputData, EByteOrder pByteOrder, Tp & pValue )
		{
			pValue = deserializePrimitive<Tp>( pInputData, pByteOrder );
			return ArithmeticTypeSerializedSize<Tp>::sValue;
		}

		template <typename Tp, std::enable_if_t<std::is_enum<Tp>::value, int> = 0>
		inline gds_size_t deserialize( const byte * pInputData, EByteOrder pByteOrder, Tp & pValue )
		{
			pValue = static_cast<Tp>( deserializePrimitive<typename std::underlying_type<Tp>::type>( pInputData, pByteOrder ) );
			return ArithmeticTypeSerializedSize<typename std::underlying_type<Tp>::type>::sValue;
		}

		template <typename TpVal, typename TpTgt>
		inline gds_size_t deserialize( const byte * pInputData, EByteOrder pByteOrder, const RefProxy<TpVal, TpTgt> & pRefProxy )
		{
			using ValueType = typename RefProxy<TpVal, TpTgt>::ValueType;
			pRefProxy.set( deserializePrimitive<ValueType>( pInputData, pByteOrder ) );
			return ArithmeticTypeSerializedSize<typename RefProxy<TpVal, TpTgt>::ValueType>::sValue;
		}

		template <typename TpVal>
		inline gds_size_t deserialize( const byte * pInputData, EByteOrder pByteOrder, TpVal & pRef, const RefCast<void, void> & )
		{
			return deserialize( pInputData, pByteOrder, pRef );
		}

		template <typename TpVal, typename TpTgt>
		inline gds_size_t deserialize( const byte * pInputData, EByteOrder pByteOrder, TpVal & pRef, const RefCast<TpVal, TpTgt> & )
		{
			return deserialize( pInputData, pByteOrder, RefProxy<TpVal, TpTgt>{ pRef } );
		}

		template <typename TpVal, typename TpTgt, std::enable_if_t<!std::is_void<TpVal>::value && !std::is_void<TpTgt>::value, int> = 0>
		inline gds_size_t deserialize( const byte * pInputData, EByteOrder pByteOrder, const RefCast<TpVal, TpTgt> & pRefCast )
		{
			return deserialize( pInputData, pByteOrder, pRefCast.targetRef.get(), pRefCast );
		}

		template <typename Tp>
		gds_size_t deserializeAll( const byte * pInputData, EByteOrder pByteOrder, Tp & pValue )
		{
			return deserialize( pInputData, pByteOrder, pValue );
		}

		template <typename Tp, typename... TpRest>
		gds_size_t deserializeAll( const byte * pInputData, EByteOrder pByteOrder, Tp & pValue, TpRest && ...pRest )
		{
			gds_size_t byteSize = deserialize( pInputData, pByteOrder, pValue );
			byteSize += deserializeAll( pInputData + byteSize, pByteOrder, std::forward<TpRest>( pRest )... );
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

		template <typename TpVal, typename TpSrc>
		inline gds_size_t evalByteSize( const ValueProxy<TpVal, TpSrc> & pValueProxy )
		{
			return evalByteSize( pValueProxy.get() );
		}

		template <typename TpVal, typename TpSrc>
		inline gds_size_t evalByteSize( const TpVal & pValue, const ValueCast<void, void> & )
		{
			return evalByteSize( pValue );
		}

		template <typename TpVal, typename TpSrc>
		inline gds_size_t evalByteSize( const TpVal & pValue, const ValueCast<TpVal, TpSrc> & )
		{
			return evalByteSize( ValueProxy<TpVal, TpSrc>{ pValue } );
		}

		template <typename TpVal, typename TpSrc, std::enable_if_t<!std::is_void<TpVal>::value && !std::is_void<TpSrc>::value, int> = 0>
		inline gds_size_t evalByteSize( const ValueCast<TpVal, TpSrc> & pValueCast )
		{
			return evalByteSize( pValueCast.srcRef, pValueCast );
		}

//		template <typename Tp>
//		inline constexpr gds_size_t evalByteSize( const InputValueWrapper<Tp> & pValue )
//		{
//			return ArithmeticTypeSerializedSize<typename InputValueWrapper<Tp>::ValueType>::sValue;
//		}
//
//		template <typename Tp>
//		inline constexpr gds_size_t evalByteSize( const Tp & pValue, const TypeCastIgnoreTag & )
//		{
//			return evalByteSize( pValue );
//		}
//
//		template <typename Tp>
//		inline constexpr gds_size_t evalByteSize( const Tp & pValue, const TypeCastNativeIntTag & )
//		{
//			return evalByteSize( asNativeIntValue( pValue ) );
//		}
//
//		template <typename Tp>
//		inline constexpr gds_size_t evalByteSize( const Tp & pValue, const TypeCastSizeTypeTag & )
//		{
//			return evalByteSize( asSizeTypeValue( pValue ) );
//		}
//
//		template <typename Tp>
//		inline constexpr gds_size_t evalByteSize( const Tp & pValue, const TypeCastTypeIDTag & )
//		{
//			return evalByteSize( asTypeIDValue( pValue ) );
//		}
//
//		template <typename Tp, typename TpCast>
//		inline constexpr gds_size_t evalByteSize( const TypeCastProxy<Tp, TpCast> & pCastProxy )
//		{
//			return evalByteSize( pCastProxy.first.get(), pCastProxy.second.get() );
//		}

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


		template <typename Tp, std::enable_if_t<IsTriviallySerializable<Tp>::sValue, int> = 0>
		inline constexpr gds_size_t queryMinByteSize( const Tp & )
		{
			return sizeof( Tp );
		}

		template <typename Tp, std::enable_if_t<IsArithmetic<Tp>::sValue || IsCharType<Tp>::sValue || IsWideChar<Tp>::sValue, int> = 0>
		inline constexpr gds_size_t queryMinByteSize( Tp )
		{
			return ArithmeticTypeSerializedSize<Tp>::sValue;
		}

		template <typename Tp, std::enable_if_t<std::is_enum<Tp>::value, int> = 0>
		inline constexpr gds_size_t queryMinByteSize( Tp )
		{
			return ArithmeticTypeSerializedSize<typename std::underlying_type<Tp>::type>::sValue;
		}



		template <typename TpVal, typename TpSrc>
		inline gds_size_t queryMinByteSize( const ValueProxy<TpVal, TpSrc> & pValueProxy )
		{
			return queryMinByteSize( pValueProxy.get() );
		}

		template <typename TpVal, typename TpSrc>
		inline gds_size_t queryMinByteSize( const TpVal & pValue, const ValueCast<void, void> & )
		{
			return queryMinByteSize( pValue );
		}

		template <typename TpVal, typename TpSrc>
		inline gds_size_t queryMinByteSize( const TpVal & pValue, const ValueCast<TpVal, TpSrc> & )
		{
			return queryMinByteSize( ValueProxy<TpVal, TpSrc>{ pValue } );
		}

		template <typename TpVal, typename TpSrc, std::enable_if_t<!std::is_void<TpVal>::value && !std::is_void<TpSrc>::value, int> = 0>
		inline gds_size_t queryMinByteSize( const ValueCast<TpVal, TpSrc> & pValueCast )
		{
			return queryMinByteSize( pValueCast.srcRef, pValueCast );
		}


		template <typename TpVal, typename TpTgt>
		inline gds_size_t queryMinByteSize( const RefProxy<TpVal, TpTgt> & pRefProxy )
		{
			return queryMinByteSize( pRefProxy.get() );
		}

		template <typename TpVal, typename TpTgt>
		inline gds_size_t queryMinByteSize( TpVal & pRef, const RefCast<void, void> & )
		{
			return queryMinByteSize( pRef );
		}

		template <typename TpVal, typename TpTgt>
		inline gds_size_t queryMinByteSize( TpVal & pRef, const RefCast<TpVal, TpTgt> & )
		{
			return queryMinByteSize( RefProxy<TpVal, TpTgt>{ pRef } );
		}

		template <typename TpVal, typename TpTgt, std::enable_if_t<!std::is_void<TpVal>::value && !std::is_void<TpTgt>::value, int> = 0>
		inline gds_size_t queryMinByteSize( const RefCast<TpVal, TpTgt> & pRefCast )
		{
			return queryMinByteSize( pRefCast.targetRef.get(), pRefCast );
		}

//		template <typename TpVal, typename TpTgt>
//		inline constexpr gds_size_t queryMinByteSize( const OutputRefWrapper<TpVal, TpTgt> & pRef )
//		{
//			return ArithmeticTypeSerializedSize<typename OutputRefWrapper<TpVal, TpTgt>::ValueType>::sValue;
//		}
//
//		template <typename Tp>
//		inline constexpr gds_size_t queryMinByteSize( const Tp & pRef, const TypeCastIgnoreTag & )
//		{
//			return queryMinByteSize( pRef );
//		}
//
//		template <typename Tp>
//		inline constexpr gds_size_t queryMinByteSize( const Tp &, const TypeCastNativeIntTag & )
//		{
//			return queryMinByteSize( emptyNativeIntRef() );
//		}
//
//		template <typename Tp>
//		inline constexpr gds_size_t queryMinByteSize( const Tp &, const TypeCastSizeTypeTag & )
//		{
//			return queryMinByteSize( emptySizeTypeRef() );
//		}
//
//		template <typename Tp>
//		inline constexpr gds_size_t queryMinByteSize( const Tp &, const TypeCastTypeIDTag & )
//		{
//			return queryMinByteSize( emptyTypeIDRef() );
//		}
//
//		template <typename Tp, typename TpCast>
//		inline constexpr gds_size_t queryMinByteSize( const TypeCastProxy<Tp, TpCast> & pCastProxy )
//		{
//			return queryMinByteSize( pCastProxy.first.get(), pCastProxy.second.get() );
//		}

		template <typename Tp>
		gds_size_t queryMinByteSizeAll( const Tp & pValue )
		{
			return queryMinByteSize( pValue );
		}

		template <typename Tp, typename... TpRest>
		gds_size_t queryMinByteSizeAll( const Tp & pValue, TpRest && ...pRest )
		{
			gds_size_t byteSize = queryMinByteSize( pValue );
			byteSize += queryMinByteSizeAll( std::forward<TpRest>( pRest )... );
			return byteSize;
		}

	}

}

#endif // __TS3_PLATFORM_GDS_H__
