
#ifndef __TS3_STDEXT_TYPE_TRAITS__H__
#define __TS3_STDEXT_TYPE_TRAITS__H__

#include "staticLimits.h"

namespace ts3
{

	/// @brief Contains a static `size_t` member variable `value` which yields the length of `TpTypes...`.
	template <typename... TpTypes>
	struct TypeCounter
	{
		static constexpr size_t value = sizeof...( TpTypes );
	};

	/// @brief 
	template <bool tBoolean, typename TpTrueType, typename TpFalseType>
	struct ConditionalType
	{
		using Type = typename std::conditional<tBoolean, TpTrueType, TpFalseType>::type;
	};

	/// @brief
	template <typename TpValue, bool tBoolean, TpValue tTrueval, TpValue tFalseval>
	struct ConditionalValue;
	
	template <typename TpValue, TpValue tTrueval, TpValue tFalseval>
	struct ConditionalValue<TpValue, true, tTrueval, tFalseval>
	{
		static constexpr TpValue value = tTrueval;
	};
	
	template <typename TpValue, TpValue tTrueval, TpValue tFalseval>
	struct ConditionalValue<TpValue, false, tTrueval, tFalseval>
	{
		static constexpr TpValue value = tFalseval;
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
	struct IntTypeBySize
	{
		using Type = typename ConditionalType<
	        tSize <= sizeof( int8 ), int8, typename ConditionalType<
                tSize <= sizeof( int16 ), int16, typename ConditionalType<
                    tSize == sizeof( int32 ), int32, typename ConditionalType<
						tSize == sizeof( int64 ), int64, void>::Type>::Type>::Type>::Type;
	};

	/// @brief Contains a member typedef `type`, which is the smallest unsigned integral type of at least specified size (in bytes)
	template <size_t tSize>
	struct UintTypeBySize
	{
		using Type = typename ConditionalType<
	        tSize == sizeof( uint8 ), uint8, typename ConditionalType<
                tSize == sizeof( uint16 ), uint16, typename ConditionalType<
                    tSize == sizeof( uint32 ), uint32, typename ConditionalType<
						tSize == sizeof( uint64 ), uint64, void>::Type>::Type>::Type>::Type;
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


	/// @brief Contains a static boolean variable `value`, which yields true if specified type list `TpTypes` contains `void` or false otherwise.
	template <typename... TpTypes>
	struct IsVoidOnTypeList;

	template <typename Tp>
	struct IsVoidOnTypeList<Tp>
	{
		static constexpr bool value = std::is_void<Tp>::value;
	};

	template <typename Tp, typename... TpRest>
	struct IsVoidOnTypeList<Tp, TpRest...>
	{
		static constexpr bool value = std::is_void<Tp>::value || IsVoidOnTypeList<TpRest...>::value;
	};


	template <typename TpType, typename... TpList>
	struct IsTypeOnTypeList;

	template <typename TpType, typename TpCheck>
	struct IsTypeOnTypeList<TpType, TpCheck>
	{
		static constexpr bool value = std::is_same<TpType, TpCheck>::value;
	};

	template <typename TpType, typename TpCheck, typename... TpList>
	struct IsTypeOnTypeList<TpType, TpCheck, TpList...>
	{
		static constexpr bool value = std::is_same<TpType, TpCheck>::value || IsTypeOnTypeList<TpType, TpList...>::value;
	};


	/// @brief Contains a member typedef `type`, which is the first type from `TpTypes...` which matches `TpPred`
	template <template<typename> class TpPred, typename... TpTypes>
	struct FirstMatchingType;

	template <template<typename> class TpPred, typename Tp>
	struct FirstMatchingType<TpPred, Tp>
	{
		using Type = typename ConditionalType<TpPred<Tp>::value, Tp, void>::Type;
	};

	template <template<typename> class TpPred, typename Tp, typename... TpRest>
	struct FirstMatchingType<TpPred, Tp, TpRest...>
	{
		using Type = typename ConditionalType<TpPred<Tp>::value, Tp, typename FirstMatchingType<TpPred, TpRest...>::Type>::Type;
	};

}

#endif /* __TS3_STDEXT_TYPE_TRAITS__H__ */
