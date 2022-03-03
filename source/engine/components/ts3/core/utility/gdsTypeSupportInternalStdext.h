
#ifndef __TS3_CORE_GDS_TYPE_SUPPORT_INTERNAL_STDEXT_H__
#define __TS3_CORE_GDS_TYPE_SUPPORT_INTERNAL_STDEXT_H__

#include "gdsTypeSupportCommon.h"
#include <ts3/platform/gds.h>

namespace ts3
{

    enum class HashAlgo : enum_default_value_t;

    template <typename TpIntegral>
    class AtomicBitmask;

    template <typename TpIntegral>
    class Bitmask;

    template <HashAlgo tpHashAlgo>
    struct HashObject;

	template <typename Tp, typename TpCompare, typename TpAlloc>
    class SortedArray;

	namespace gds
	{

		/***********************************************************************************************************/
		/************************************************* ArrayView ***********************************************/
		/***********************************************************************************************************/
		
		template <typename Tp, typename TpCast = void>
		inline gds_size_t evalByteSize( const ArrayView<Tp> & pArrayView,
                                        const TypeCastTag<TpCast> & pCastTag = cvTypeCastNone )
		{
			gds_size_t byteSize = evalByteSize( emptySizeType() );

			for( const auto & entry : pArrayView )
			{
				byteSize += evalByteSize( entry, pCastTag );
			}

			return byteSize;
		}

		template <typename Tp, typename TpCast = void>
		inline gds_size_t serialize( byte * pOutputBuffer,
                                     const ArrayView<Tp> & pArrayView,
                                     const TypeCastTag<TpCast> & pCastTag = cvTypeCastNone )
		{
			gds_size_t byteSize = serialize( pOutputBuffer, asSizeType( pArrayView.size ) );

			for( const auto & value : pArrayView )
			{
				byteSize += serialize(  pOutputBuffer + byteSize, value, pCastTag );
			}

			return byteSize;
		}

		template <typename Tp, typename TpCast = void>
		inline gds_size_t deserialize( const byte * pInputData,
                                       ArrayView<Tp> & pArrayView,
                                       const TypeCastTag<TpCast> & pCastTag = cvTypeCastNone )
		{
			size_t arraySize = 0;
			gds_size_t byteSize = deserialize( pInputData, asSizeType( arraySize ) );

			for( auto & value : pArrayView )
			{
				byteSize += deserialize( pInputData + byteSize, value, pCastTag );
			}

			return byteSize;
		}


		/***********************************************************************************************************/
		/*********************************************** AtomicBitmask *********************************************/
		/***********************************************************************************************************/

		template <typename TpIntegral, typename TpCast = void>
        inline gds_size_t evalByteSize( const AtomicBitmask<TpIntegral> & pBitmask,
                                        const TypeCastTag<TpCast> & pCastTag = cvTypeCastNone )
		{
			return evalByteSize( static_cast<TpIntegral>( pBitmask ), pCastTag );
		}

		template <typename TpIntegral, typename TpCast = void>
		inline gds_size_t serialize( byte * pOutputBuffer,
                                     const AtomicBitmask<TpIntegral> & pBitmask,
                                     const TypeCastTag<TpCast> & pCastTag = cvTypeCastNone )
		{
		    return serialize( pOutputBuffer, static_cast<TpIntegral>( pBitmask ), pCastTag );
		}

		template <typename TpIntegral, typename TpCast = void>
		inline gds_size_t deserialize( const byte * pInputData,
                                       AtomicBitmask<TpIntegral> & pBitmask,
                                       const TypeCastTag<TpCast> & pCastTag = cvTypeCastNone )
		{
		    return deserialize( pInputData, static_cast<TpIntegral>( pBitmask ), pCastTag );
		}


		/***********************************************************************************************************/
		/************************************************** Bitmask ************************************************/
		/***********************************************************************************************************/

		template <typename TpIntegral, typename TpCast = void>
        inline gds_size_t evalByteSize( const Bitmask<TpIntegral> & pBitmask,
                                        const TypeCastTag<TpCast> & pCastTag = cvTypeCastNone )
		{
			return evalByteSize( static_cast<TpIntegral>( pBitmask ), pCastTag );
		}

		template <typename TpIntegral, typename TpCast = void>
		inline gds_size_t serialize( byte * pOutputBuffer,
                                     const Bitmask<TpIntegral> & pBitmask,
                                     const TypeCastTag<TpCast> & pCastTag = cvTypeCastNone )
		{
		    return serialize( pOutputBuffer, static_cast<TpIntegral>( pBitmask ), pCastTag );
		}

		template <typename TpIntegral, typename TpCast = void>
		inline gds_size_t deserialize( const byte * pInputData,
                                       Bitmask<TpIntegral> & pBitmask,
                                       const TypeCastTag<TpCast> & pCastTag = cvTypeCastNone )
		{
		    return deserialize( pInputData, static_cast<TpIntegral>( pBitmask ), pCastTag );
		}


		/***********************************************************************************************************/
		/************************************************* HashObject **********************************************/
		/***********************************************************************************************************/

		template <HashAlgo tpHashAlgo, typename TpCast = void>
        inline gds_size_t evalByteSize( const HashObject<tpHashAlgo> & pHashObject,
                                        const TypeCastTag<TpCast> & pCastTag = cvTypeCastNone )
		{
            return evalByteSize( static_cast<typename HashObject<tpHashAlgo>::ValueType>( pHashObject ), pCastTag );
		}

		template <HashAlgo tpHashAlgo, typename TpCast = void>
		inline gds_size_t serialize( byte * pOutputBuffer,
                                     const HashObject<tpHashAlgo> & pHashObject,
                                     const TypeCastTag<TpCast> & pCastTag = cvTypeCastNone )
		{
		    return serialize( pOutputBuffer, static_cast<typename HashObject<tpHashAlgo>::ValueType>( pHashObject ), pCastTag );
		}

		template <HashAlgo tpHashAlgo, typename TpCast = void>
		inline gds_size_t deserialize( const byte * pInputData,
                                       HashObject<tpHashAlgo> & pHashObject,
                                       const TypeCastTag<TpCast> & pCastTag = cvTypeCastNone )
		{
		    return deserialize( pInputData, static_cast<typename HashObject<tpHashAlgo>::ValueType>( pHashObject ), pCastTag );
		}


		/***********************************************************************************************************/
		/************************************************ SortedArray **********************************************/
		/***********************************************************************************************************/

		template <typename Tp, typename TpCompare, typename TpAlloc, typename TpCast = void>
		inline gds_size_t evalByteSize( const SortedArray<Tp, TpCompare, TpAlloc> & pSortedArray,
                                        const TypeCastTag<TpCast> & pCastTag = cvTypeCastNone )
		{
			return evalContainerByteSize( pSortedArray, pCastTag );
		}

		template <typename Tp, typename TpCompare, typename TpAlloc, typename TpCast = void>
		inline gds_size_t serialize( byte * pOutputBuffer,
                                     const SortedArray<Tp, TpCompare, TpAlloc> & pSortedArray,
                                     const TypeCastTag<TpCast> & pCastTag = cvTypeCastNone )
		{
			return serializeContainer( pOutputBuffer, pSortedArray, pCastTag );
		}

		template <typename Tp, typename TpCompare, typename TpAlloc, typename TpCast = void>
		inline gds_size_t deserialize( const byte * pInputData,
                                       const SortedArray<Tp, TpCompare, TpAlloc> & pSortedArray,
                                       const TypeCastTag<TpCast> & pCastTag = cvTypeCastNone )
		{
			return deserializeContainer(
			    pInputData, pSortedArray, pCastTag, []( auto & SA, auto, auto && E ) -> void { SA.insert( std::move( E ) ); } );
		}

	}

}

#endif // __TS3_CORE_GDS_TYPE_SUPPORT_INTERNAL_STDEXT_H__
