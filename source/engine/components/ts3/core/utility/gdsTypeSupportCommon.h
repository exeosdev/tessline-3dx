
#ifndef __TS3_CORE_GDS_TYPE_SUPPORT_COMMON_H__
#define __TS3_CORE_GDS_TYPE_SUPPORT_COMMON_H__

#include "../prerequisites.h"
#include <ts3/platform/gds.h>

namespace ts3
{

	namespace gds
	{

		/// @brief Computes the actual size of binary representation of a: container.
		template <template <typename...> typename TpC, typename... TpCArgs, typename TpCast>
		inline gds_size_t evalContainerByteSize( const TpC<TpCArgs...> & pContainer,
                                                 const TpCast & pCastTag )
		{
			// Compute the size for the specified container.
			// The space required is the serialized size + binary representation of all elements in the container.
			gds_size_t byteSize = 0;

			// The first member is the number of elements in a container. We are not interested in its value, though.
			byteSize += gds::evalByteSize( emptySizeType() );

			// Enumerate over the container's elements.
			for( const auto & entry : pContainer )
			{
				// Evaluate the size of each element. Propagate cast tag to support container-wise value casting.
				byteSize += gds::evalByteSize( entry, pCastTag );
			}

			return byteSize;
		}

		/// @brief Serializes the content of a specified std:: container and stores the output in the provided buffer.
		template <template <typename...> typename TpC, typename... TpCArgs, typename TpCast>
		inline gds_size_t serializeContainer( byte * pOutputBuffer,
                                              const TpC<TpCArgs...> & pContainer,
                                              const TpCast & pCastTag )
		{
			// First, get the size of the container (number of elements).
			const size_t containerSize = pContainer.size();

			// size_t may have different sizes on different platforms. asSizeTypeValue() ensures this is properly
			// handled by the GDS, so we can deserialize the data on a different platform, and get expected results.
			gds_size_t byteSize = gds::serialize( pOutputBuffer, asSizeType( containerSize ) );

			// Enumerate over the container's elements.
			for( const auto & entry : pContainer )
			{
				// Serialize the element, add the size to the total byte size. Propagate cast tag.
				byteSize += gds::serialize( pOutputBuffer + byteSize, entry, pCastTag );
			}

			return byteSize;
		}

		template <template <typename...> typename TpC, typename... TpCArgs, typename TpCast, typename TpIns>
		inline gds_size_t deserializeContainer( const byte * pInputData,
                                                TpC<TpCArgs...> & pContainer,
                                                const TpCast & pCastTag,
                                                const TpIns & pInsertCallback )
		{
			// The variable to store the container's size.
			size_t containerSize = 0;

			// Deserialize the size. Since we used asSizeTypeValue() during serialization, we must now use asSizeTypeRef()
			// to properly retrieve the deserialized value. The output will be properly stored in the wrapped variable.
			gds_size_t byteSize = gds::deserialize( pInputData, asSizeType( containerSize ) );

			for( size_t entryIndex = 0; entryIndex < containerSize; ++entryIndex )
			{
				// Temporary for the deserialized state.
				typename TpC<TpCArgs...>::value_type tempEntry{};

				// Deserialize current element. Propagate cast tag to support container-wise value casting.
				byteSize += gds::deserialize( pInputData + byteSize, tempEntry, pCastTag );

				// Call the insert callback to add it to the container. Index is needed e.g. for std::array.
				pInsertCallback( pContainer, entryIndex, std::move( tempEntry ) );
			}

			return byteSize;
		}

		/// @brief Computes the actual size of binary representation of a map-type container.
		template <template <typename...> typename TpC, typename... TpCArgs, typename TpCast>
		inline gds_size_t evalMapByteSize( const TpC<TpCArgs...> & pMap,
                                           const TpCast & pCastTag )
		{
			gds_size_t byteSize = 0;

			// The first member is the number of elements in a container. We are not interested in its value, though.
			byteSize += gds::evalByteSize( emptySizeType() );

			// Enumerate over the container's elements.
			for( const auto & mapEntry : pMap )
			{
				// Evaluate the size of each element. Propagate cast tag to support container-wise value casting.
				byteSize += gds::evalByteSize( mapEntry.first );
				byteSize += gds::evalByteSize( mapEntry.second, pCastTag );
			}

			return byteSize;
		}

		/// @brief
		template <template <typename...> typename TpM, typename TpK, typename TpV, typename... TpCArgs, typename TpCast>
		inline gds_size_t serializeMap( byte * pOutputBuffer,
                                        const TpM<TpK, TpV, TpCArgs...> & pMap,
                                        const TpCast & pCastTag )
		{
		    const size_t mapSize = pMap.size();

		    gds_size_t byteSize = gds::serialize( pOutputBuffer, asSizeType( mapSize ) );

			for( const auto & mapEntry : pMap )
			{
				byteSize += gds::serialize( pOutputBuffer + byteSize, mapEntry.first );
				byteSize += gds::serialize( pOutputBuffer + byteSize, mapEntry.second, pCastTag );
			}

			return byteSize;
		}

		template <template <typename...> typename TpM, typename TpK, typename TpV, typename... TpCArgs, typename TpCast>
		inline gds_size_t deserializeMap( const byte * pInputData,
                                          TpM<TpK, TpV, TpCArgs...> & pMap,
                                          const TpCast & pCastTag )
		{
			size_t mapSize = 0;

			gds_size_t byteSize = gds::deserialize( pInputData, asSizeType( mapSize ) );

			for( size_t entryIndex = 0; entryIndex < mapSize; ++entryIndex )
			{
				TpK tempKey{};
				byteSize += gds::deserialize( pInputData + byteSize, tempKey );

				TpV tempValue{};
				byteSize += gds::deserialize( pInputData + byteSize, tempValue, pCastTag );

				pMap.emplace( std::move( tempKey ), std::move( tempValue ) );
			}

			return byteSize;
		}

	}

}

#endif // __TS3_CORE_GDS_TYPE_SUPPORT_COMMON_H__
