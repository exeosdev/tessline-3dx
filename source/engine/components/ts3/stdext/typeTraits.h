
#ifndef __TS3_STDEXT_TYPE_TRAITS__H__
#define __TS3_STDEXT_TYPE_TRAITS__H__

#include "staticLimits.h"

namespace ts3
{

	/// @brief Contains a static `size_t` member variable `value` which yields the length of `TTypes...`.
	template <typename... TTypes>
	struct TypeCounter
	{
		static constexpr size_t value = sizeof...( TTypes );
	};

	/// @brief 
	template <bool tBoolean, typename TTrueType, typename TFalseType>
	struct ConditionalType
	{
		using Type = typename std::conditional<tBoolean, TTrueType, TFalseType>::type;
	};

	/// @brief
	template <typename TValue, bool tBoolean, TValue tTrueval, TValue tFalseval>
	struct ConditionalValue;
	
	template <typename TValue, TValue tTrueval, TValue tFalseval>
	struct ConditionalValue<TValue, true, tTrueval, tFalseval>
	{
		static constexpr TValue value = tTrueval;
	};
	
	template <typename TValue, TValue tTrueval, TValue tFalseval>
	struct ConditionalValue<TValue, false, tTrueval, tFalseval>
	{
		static constexpr TValue value = tFalseval;
	};


	/// @brief
	template <int... tSeq>
	struct IntegerSequence
	{};

	template <int tN, int... tSeq>
	struct IntegerSequenceGenerator : IntegerSequenceGenerator<tN - 1, tN - 1, tSeq...>
	{};

	template <int... tSeq>
	struct IntegerSequenceGenerator<0, tSeq...>
	{
		using Type = IntegerSequence<tSeq...>;
	};


	/// @brief Contains a member typedef `type`, which is the smallest signed integral type of at least specified size (in bytes)
	template <size_t tSize>
	struct IntTypeByBits
	{
		using Type = typename ConditionalType<
	        tSize <= ( sizeof( int8 ) * CHAR_BIT ), int8, typename ConditionalType<
                tSize <= ( sizeof( int16 ) * CHAR_BIT  ), int16, typename ConditionalType<
                    tSize <= ( sizeof( int32 ) * CHAR_BIT ), int32, typename ConditionalType<
						tSize <= ( sizeof( int64 ) * CHAR_BIT ), int64, void>::Type>::Type>::Type>::Type;
	};

	/// @brief Contains a member typedef `type`, which is the smallest unsigned integral type of at least specified size (in bytes)
	template <size_t tSize>
	struct UintTypeByBits
	{
		using Type = typename ConditionalType<
	        tSize <= ( sizeof( uint8 ) * CHAR_BIT ), uint8, typename ConditionalType<
                tSize <= ( sizeof( uint16 ) * CHAR_BIT ), uint16, typename ConditionalType<
                    tSize <= ( sizeof( uint32 ) * CHAR_BIT ), uint32, typename ConditionalType<
						tSize <= ( sizeof( uint64 ) * CHAR_BIT ), uint64, void>::Type>::Type>::Type>::Type;
	};


	/// @brief Contains a member typedef `type`, which is the smallest signed integral type of at least specified size (in bytes)
	template <size_t tSize>
	struct IntTypeBySize
	{
		using Type = typename ConditionalType<
	        tSize <= sizeof( int8 ), int8, typename ConditionalType<
                tSize <= sizeof( int16 ), int16, typename ConditionalType<
                    tSize <= sizeof( int32 ), int32, typename ConditionalType<
						tSize <= sizeof( int64 ), int64, void>::Type>::Type>::Type>::Type;
	};

	/// @brief Contains a member typedef `type`, which is the smallest unsigned integral type of at least specified size (in bytes)
	template <size_t tSize>
	struct UintTypeBySize
	{
		using Type = typename ConditionalType<
	        tSize <= sizeof( uint8 ), uint8, typename ConditionalType<
                tSize <= sizeof( uint16 ), uint16, typename ConditionalType<
                    tSize <= sizeof( uint32 ), uint32, typename ConditionalType<
						tSize <= sizeof( uint64 ), uint64, void>::Type>::Type>::Type>::Type;
	};

	/// @brief Contains a member typedef `type`, which is the smallest signed integral type required to hold the specified value.
	template <intmax_t tValue>
	struct IntTypeByValue
	{
		using Type = typename ConditionalType<
	        ( tValue >= Limits<int8>::minValue ) && ( tValue <= Limits<int8>::maxValue ), int8, typename ConditionalType<
                ( tValue >= Limits<int16>::minValue ) && ( tValue <= Limits<int16>::maxValue ), int16, typename ConditionalType<
                    ( tValue >= Limits<int32>::minValue ) && ( tValue <= Limits<int32>::maxValue ), int32, int64>::Type>::Type>::Type;
	};

	/// @brief Contains a member typedef `type`, which is the smallest unsigned integral type required to hold the specified value
	template <uintmax_t tValue>
	struct UintTypeByValue
	{
		using Type = typename ConditionalType<
	        tValue <= Limits<uint8>::maxValue, uint8, typename ConditionalType<
                tValue <= Limits<uint16>::maxValue, uint16, typename ConditionalType<
                    tValue <= Limits<uint32>::maxValue, uint32, uint64>::Type>::Type>::Type;
	};


	/// @brief Contains a static boolean variable `value`, which yields true if specified type list `TTypes` contains `void` or false otherwise.
	template <typename... TTypes>
	struct IsVoidOnTypeList;

	template <typename TVal>
	struct IsVoidOnTypeList<TVal>
	{
		static constexpr bool value = std::is_void<TVal>::value;
	};

	template <typename TVal, typename... TRest>
	struct IsVoidOnTypeList<TVal, TRest...>
	{
		static constexpr bool value = std::is_void<TVal>::value || IsVoidOnTypeList<TRest...>::value;
	};


	template <typename TType, typename... TList>
	struct IsTypeOnTypeList;

	template <typename TType, typename TCheck>
	struct IsTypeOnTypeList<TType, TCheck>
	{
		static constexpr bool value = std::is_same<TType, TCheck>::value;
	};

	template <typename TType, typename TCheck, typename... TList>
	struct IsTypeOnTypeList<TType, TCheck, TList...>
	{
		static constexpr bool value = std::is_same<TType, TCheck>::value || IsTypeOnTypeList<TType, TList...>::value;
	};


	/// @brief Contains a member typedef `type`, which is the first type from `TTypes...` which matches `TPred`
	template <template<typename> class TPred, typename... TTypes>
	struct FirstMatchingType;

	template <template<typename> class TPred, typename TVal>
	struct FirstMatchingType<TPred, TVal>
	{
		using Type = typename ConditionalType<TPred<TVal>::value, TVal, void>::Type;
	};

	template <template<typename> class TPred, typename TVal, typename... TRest>
	struct FirstMatchingType<TPred, TVal, TRest...>
	{
		using Type = typename ConditionalType<TPred<TVal>::value, TVal, typename FirstMatchingType<TPred, TRest...>::Type>::Type;
	};

}

#endif /* __TS3_STDEXT_TYPE_TRAITS__H__ */
