
#ifndef __TS3_CORE_GDS_CORE_H__
#define __TS3_CORE_GDS_CORE_H__

#include "../exception.h"
#include <ts3/platform/gds.h>
#include <ts3/stdext/byteArray.h>
#include <functional>

namespace std
{

    template <typename _T, size_t _N>
    class array;

    template <typename _T, typename _Alloc>
    class vector;

}

namespace ts3
{

    namespace gds
    {

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

        /// @brief Serializes given object with additional metadata.
        ///
        /// @note
        /// This function does NOT implement the actual serialization for any type. Instead,
        /// It dispatches the serialization to the usual serialize() method for the specified
        /// type and prepends the result with the metadata, which is filled with the size info
        /// received from the serialize() function.
        /// Thus, a type for which this function is used needs the usual serialize() function.
        /// Otherwise, it will be serialized with one of the default ones defined above.
        template <typename Tp>
        inline gds_size_t serializeWithMetaData( byte * pOutputBuffer, const Tp & pValue )
        {
            // Size of the metadata, in bytes.
            const auto metaDataSize = getInstanceMetaDataSize();

            // The actual serialization. Skip 'metaDataSize' bytes needed for metadata
            // and write after that. serialize() returns the number of bytes written.
            const auto valueByteSize = serialize( pOutputBuffer + metaDataSize, pValue );

            // MetaData for this instance.
            InstanceMetaData metaData;
            metaData.controlKey = CX_GDS_VALUE_META_DATA_CONTROL_KEY;
            metaData.objectDataSize = valueByteSize; // Size of the serialized data of the OBJECT only.
            metaData.outputBlockSize = metaDataSize + valueByteSize; // Size of the whole data block (including MetaData).

            // Now, serialize the data itself.
            serialize( pOutputBuffer, metaData );

            return metaData.outputBlockSize;
        }

        /// @brief Performs deserialization of the specified data, written with serializeWithMetaData().
        template <typename Tp>
        inline gds_size_t deserializeWithMetaData( const byte * pInputData, Tp & pValue )
        {
            // The metadata object to which we will deserialize.
            InstanceMetaData metaData;

            // Deserialize the metadata first. It is always at the beginning of the data block.
            const auto metaDataSize = deserialize( pInputData, metaData );

            // deserialize() returns the number of bytes read from the buffer.
            // This should match the byte size of the InstanceMetaData struct.
            ts3DebugAssert( metaDataSize == getInstanceMetaDataSize() );

            //
            const auto valueByteSize = deserialize( pInputData + metaDataSize, pValue );

            return metaDataSize + valueByteSize;
        }

        /// @brief Returns an evaluated byte size of the specified object which will be serialized using metadata.
        template <typename Tp>
        inline gds_size_t evalByteSizeWithMetaData( const Tp & pValue )
        {
            const auto metaDataSize = getInstanceMetaDataSize();
            const auto valueByteSize = evalByteSize( pValue );

            // Technically, user-defined evalByteSize() can be defined to return 0.
            // We always skip empty entries, so in case of metadata let's do the same.
            // If the object itself has no data, stick to that and report 0 size.

            return ( valueByteSize > 0 ) ? ( metaDataSize + valueByteSize ) : 0u;
        }

        /// @brief Useful helper function to read InstanceMetaData from the specified serialized data block.
        inline InstanceMetaData readInstanceMetaData( const byte * pInputData )
        {
            InstanceMetaData valueMetaData;
            deserialize( pInputData, valueMetaData );
            return valueMetaData;
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


        /// @brief Typedef for a callable compatible with ReadCallback requirements for GDS deserializeExternal() functions.
        using DataReadCallback = std::function<uint64 /* pReadBytes */( void * /* pTargetBuffer */, uint64 /* pReadSize */ )>;

        /// @brief Typedef for a callable compatible with WriteCallback requirements for GDS serializeExternal() functions.
        using DataWriteCallback = std::function<uint64 /* pWrittenBytes */( const void * /* pDataToWrite */, uint64 /* pWriteSize */ )>;

        /// @brief Utility class acting as proxy for serializeAuto(), depending on the buffer type (fixed or resizable).
        template <bool tpResizable>
        struct SerializeAutoProxy;

        /// @brief Specialization for resizable/dynamic output buffers.
        template <>
        struct SerializeAutoProxy<true>
        {
            /// @brief Serializes specified value without MetaData into a dynamic output buffer. Resizes the buffer accordingly.
            template <typename TpResizableBuffer, typename TpValue>
            static gds_size_t serialize( TpResizableBuffer & pOutputBuffer, const TpValue & pValue )
            {
                // Get the size of the serialized object (i.e. the minimum required size of the target buffer).
                const auto totalByteSize = evalByteSize( pValue );

                if( totalByteSize > 0 )
                {
                    // Resize the output buffer.
                    pOutputBuffer.resize( trunc_numeric_cast<size_t>( totalByteSize ) );

                    // Serialize the data into the buffer. This is the call to the actual, value-specific serialize().
                    gds::serialize( pOutputBuffer.data(), pValue );
                }

                return totalByteSize;
            }

            /// @brief Serializes specified value with MetaData into a dynamic output buffer. Resizes the buffer accordingly.
            template <typename TpResizableBuffer, typename TpValue>
            static gds_size_t serializeWithMetaData( TpResizableBuffer & pOutputBuffer, const TpValue & pValue )
            {
                // Same thing as above, but we also include the MetaData block alongside the object itself.

                const auto totalByteSize = evalByteSizeWithMetaData( pValue );

                if( totalByteSize > 0 )
                {
                    pOutputBuffer.resize( trunc_numeric_cast<size_t>( totalByteSize ) );
                    gds::serializeWithMetaData( pOutputBuffer.data(), pValue );
                }

                return totalByteSize;
            }
        };

        /// @brief Specialization for fixed/static output buffers.
        template <>
        struct SerializeAutoProxy<false>
        {
            template <typename TpFixedBuffer, typename TpValue>
            static gds_size_t serialize( TpFixedBuffer & pOutputBuffer, const TpValue & pValue )
            {
                // Get the size of the serialized object (i.e. the minimum required size of the target buffer).
                const auto totalByteSize = evalByteSize( pValue );

                if( totalByteSize > 0 )
                {
                    // The buffer is a fixed one - check if its size is large enough to store the whole data.
                    if( totalByteSize > pOutputBuffer.size() )
                    {
                        // Buffer is too small, throw an error, nothing can be done here.
                        ts3ThrowDesc( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER, "Fixed buffer is to small for the output data." );
                    }

                    // Buffer is large enough, so we can use it as the target for the output.
                    // Serialize the data into the buffer. This is the call to the actual, value-specific serialize().
                    gds::serialize( pOutputBuffer.data(), pValue );
                }

                return totalByteSize;
            }

            template <typename TpFixedBuffer, typename TpValue>
            static gds_size_t serializeWithMetaData( TpFixedBuffer & pOutputBuffer, const TpValue & pValue )
            {
                // Same thing as above, but we also include the MetaData block alongside the object itself.

                const auto totalByteSize = evalByteSizeWithMetaData( pValue );

                if( totalByteSize > 0 )
                {
                    if( totalByteSize > pOutputBuffer.size() )
                    {
                        ts3ThrowDesc( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER, "Fixed buffer is to small for the output data." );
                    }
                    gds::serializeWithMetaData( pOutputBuffer.data(), pValue );
                }

                return totalByteSize;
            }
        };

        // Implementation of serializeAuto()/serializeAutoWithMetaData() accepting various type of output buffers.
        // Each one simply calls the right proxy class member. Right now we support the following types of buffers:
        // 1) Fixed:
        // - ReadWriteMemoryView - wrapper around any form of in-memory storage
        // - std::array<byte, N>
        // 2) Dynamic:
        // - DynamicByteArray
        // - DynamicMemoryBuffer
        // - std::vector<byte>

        template <typename TpValue>
        inline gds_size_t serializeAuto( const ReadWriteMemoryView & pOutputBuffer, const TpValue & pValue )
        {
            return SerializeAutoProxy<false>::serialize( pOutputBuffer, pValue );
        }

        template <typename TpValue, size_t tpSize>
        inline gds_size_t serializeAuto( std::array<byte, tpSize> & pOutputBuffer, const TpValue & pValue )
        {
            return SerializeAutoProxy<false>::serialize( pOutputBuffer, pValue );
        }

        template <typename TpValue>
        inline gds_size_t serializeAutoWithMetaData( const ReadWriteMemoryView & pOutputBuffer, const TpValue & pValue )
        {
            return SerializeAutoProxy<false>::serializeWithMetaData( pOutputBuffer, pValue );
        }

        template <typename TpValue, size_t tpSize>
        inline gds_size_t serializeAutoWithMetaData( std::array<byte, tpSize> & pOutputBuffer, const TpValue & pValue )
        {
            return SerializeAutoProxy<false>::serializeWithMetaData( pOutputBuffer, pValue );
        }

        template <typename TpValue>
        inline gds_size_t serializeAuto( DynamicByteArray & pOutputBuffer, const TpValue & pValue )
        {
            return SerializeAutoProxy<true>::serialize( pOutputBuffer, pValue );
        }

        template <typename TpValue>
        inline gds_size_t serializeAuto( DynamicMemoryBuffer & pOutputBuffer, const TpValue & pValue )
        {
            return SerializeAutoProxy<true>::serialize( pOutputBuffer, pValue );
        }

        template <typename TpValue>
        inline gds_size_t serializeAuto( std::vector<byte> & pOutputBuffer, const TpValue & pValue )
        {
            return SerializeAutoProxy<true>::serialize( pOutputBuffer, pValue );
        }

        template <typename TpValue>
        inline gds_size_t serializeAutoWithMetaData( DynamicByteArray & pOutputBuffer, const TpValue & pValue )
        {
            return SerializeAutoProxy<true>::serializeWithMetaData( pOutputBuffer, pValue );
        }

        template <typename TpValue>
        inline gds_size_t serializeAutoWithMetaData( DynamicMemoryBuffer & pOutputBuffer, const TpValue & pValue )
        {
            return SerializeAutoProxy<true>::serializeWithMetaData( pOutputBuffer, pValue );
        }

        template <typename TpValue>
        inline gds_size_t serializeAutoWithMetaData( std::vector<byte> & pOutputBuffer, const TpValue & pValue )
        {
            return SerializeAutoProxy<true>::serializeWithMetaData( pOutputBuffer, pValue );
        }

        // Implementation of deserializeAuto()/deserializeAutoWithMetaData() accepting various type of input buffers.
        // Nothing tricky in this case - those buffers act as inputs, so we simply forward the call to the actual
        // deserialize()/deserializeWithMetaData() using a pointer to the underlying data in the buffer.

        template <typename TpBuffer, typename TpValue>
        inline gds_size_t deserializeAuto( const TpBuffer & pInputData, TpValue & pValue )
        {
            return deserialize( pInputData.data(), pValue );
        }

        template <typename TpBuffer, typename TpValue>
        inline gds_size_t deserializeAutoWithMetaData( const TpBuffer & pInputData, TpValue & pValue )
        {
            return deserializeWithMetaData( pInputData.data(), pValue );
        }

        // External serialization/deserialization support.
        //
        // This functionality is probably the most commonly used in the real-life scenarios (including some engine parts).
        // Most of the time, serialized data ends up in some fom of an external storage (a file, a network, etc).
        // For big portions of data, loading everything into the memory for deserialization (or serializing everything
        // into a single buffer) may not be an option. Loading chunks creates a problem, though - we can end up having
        // half of the object's data in the buffer (we cannot know how much data to load).
        //
        // MetaData solves this problem by adding fixed-size information before the data itself. External API further
        // enables serialization/deserialization directly to/from an external storage.
        // Each of these functions accepts a write (serialize()) or a read (deserialize()) callback for interaction
        // with an external medium.

        // Serialization is not really a problem. We simply serialize the specified object into a temporary
        // buffer (optionally specified by the user) and write the data to the output using the write callback.

        /// @brief Serializes the specified object and writes the byte representation using the specified callback.
        /// @param pValue The object to be serialized
        /// @param pWriteCallback The write callback for data writing. Cannot be empty.
        /// @param pGdsCache Custom cache to which the data is serialized before writing.
        template <typename TpBuffer, typename TpValue>
        inline gds_size_t serializeExternal( const TpValue & pValue, const DataWriteCallback & pWriteCallback, TpBuffer & pGdsCache )
        {
            // Serialize the object into the cache. We can use our neat serializeAuto()
            // to properly handle different type of buffers here!
            const auto writeSize = serializeAutoWithMetaData( pGdsCache, pValue );

            if( writeSize > 0 )
            {
                if( !pWriteCallback )
                {
                    // This cannot be an empty function.
                    ts3ThrowDesc( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER, "Empty write callback." );
                }

                // Write the data using the callback.
                pWriteCallback( pGdsCache.data(), writeSize );
            }

            return writeSize;
        }

        /// @brief Serializes the specified object and writes the byte representation using the specified callback.
        /// @param pValue The object to be serialized
        /// @param pWriteCallback The write callback for data writing. Cannot be empty.
        template <typename TpValue>
        inline gds_size_t serializeExternal( const TpValue & pValue, const DataWriteCallback & pWriteCallback )
        {
            DynamicByteArray localGdsCache{};
            return serializeExternal( pValue, pWriteCallback, localGdsCache );
        }

        // Deserialization is more problematic, because the data is usually dynamically-sized (containers, strings, ranges).
        // For the best flexibility, we assume, that the data source is a one-way stream (we can read bytes in order
        // and cannot rewind back). This assumption allows us to support all kind of streams directly.
        //
        // To achieve this, we use the MetaData feature (this is why serializeExternal() always does the process using
        // serializeAutoWithMetaData() internally) and obtain the info about how much data we need to fetch from the stream.
        //
        // The data must be obviously loaded into some kind of buffer first, so just like in case of serializeAuto(),
        // we use proxy class to enable usage of different types of buffers as this temporary storage.

        /// @brief Utility class acting as proxy for deserializeExternal(), depending on the buffer type (fixed or resizable).
        template <bool tpResizable>
        struct DeserializeExternalProxy;

        /// @brief Specialization for resizable/dynamic temporary read buffers.
        template <>
        struct DeserializeExternalProxy<true>
        {
            template <typename TpResizableBuffer>
            static bool readDataExternal( const DataReadCallback & pReadCallback, TpResizableBuffer & pReadBuffer )
            {
                // Get the size of the metadata. This is the amount of data we need to read from the stream first.
                const auto metaDataSize = gds::getInstanceMetaDataSize();

                // Resize the buffer accordingly.
                pReadBuffer.resize( metaDataSize );

                // Read the data from the stream. If there is not enough data, we cannot proceed.
                const auto metaDataReadSize = pReadCallback( pReadBuffer.data(), metaDataSize );
                if( metaDataReadSize != metaDataSize )
                {
                    return false;
                }

                // Read the MetaData object itself. It is boolean-convertible, so we can validate it.
                const auto metaData = gds::readInstanceMetaData( pReadBuffer.data() );
                if( !metaData )
                {
                    return false;
                }

                // MetaData contains information about the size. ::outputBlockSize is the size of the whole data block
                // (i.e. MetaData + ObjectData). Resize the buffer so we can read the remaining object data now.
                pReadBuffer.resize( trunc_numeric_cast<size_t>( metaData.outputBlockSize ) );

                // Read the object data. Save it after the MetaData (hence the offset), so we have a full representation.
                const auto objectDataReadSize = pReadCallback( pReadBuffer.data() + metaDataSize, metaData.objectDataSize );
                if( objectDataReadSize != metaData.objectDataSize )
                {
                    return false;
                }

                return true;
            }

            template <typename TpValue, typename TpResizableBuffer>
            static gds_size_t deserializeExternal( TpValue & pValue, const DataReadCallback & pReadCallback, TpResizableBuffer & pGdsCache )
            {
                // Read data from the stream using the MetaData info. If this succeeds, pGdsCache will have
                // a byte representation of a serialized object (including its MetaData block, of course).
                if( !readDataExternal( pReadCallback, pGdsCache ) )
                {
                    return 0;
                }

                // Deserialize the object into the specified value. This will once again perform reading of the
                // MetaData block, but this is super cheap and we will get some additional validation as a bonus.
                return gds::deserializeWithMetaData( pGdsCache.data(), pValue );
            }
        };

        /// @brief Specialization for fixed/static temporary read buffers.
        template <>
        struct DeserializeExternalProxy<false>
        {
            template <typename TpFixedBuffer>
            static bool readDataExternal( const DataReadCallback & pReadCallback, TpFixedBuffer & pReadBuffer )
            {
                // Get the size of the metadata. This is the amount of data we need to read from the stream first.
                const auto metaDataSize = gds::getInstanceMetaDataSize();

                // This is a fixed buffer and cannot be resized. If there is not enough space, we need to abort.
                if( metaDataSize > pReadBuffer.size() )
                {
                    ts3ThrowDesc( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER, "Fixed buffer is to small for the output data." );
                }

                const auto metaDataReadSize = pReadCallback( pReadBuffer.data(), metaDataSize );
                if( metaDataReadSize != metaDataSize )
                {
                    return false;
                }

                const auto metaData = gds::readInstanceMetaData( pReadBuffer.data() );
                if( !metaData )
                {
                    return false;
                }

                // Again - we cannot resize the buffer. If the additional data does not fit, report an error and quit.
                if( metaData.outputBlockSize > pReadBuffer.size() )
                {
                    ts3ThrowDesc( E_EXCEPTION_CODE_DEBUG_PLACEHOLDER, "Fixed buffer is to small for the output data." );
                }

                const auto objectDataReadSize = pReadCallback( pReadBuffer.data() + metaDataSize, metaData.objectDataSize );
                if( objectDataReadSize != metaData.objectDataSize )
                {
                    return false;
                }

                return true;
            }

            template <typename TpValue, typename TpFixedBuffer>
            static gds_size_t deserializeExternal( TpValue & pValue, const DataReadCallback & pReadCallback, TpFixedBuffer & pGdsCache )
            {
                if( !readDataExternal( pReadCallback, pGdsCache ) )
                {
                    return 0;
                }
                return gds::deserializeWithMetaData( pGdsCache.data(), pValue );
            }
        };

        template <typename TpValue>
        inline gds_size_t deserializeExternal( TpValue & pValue, const DataReadCallback & pReadCallback, const ReadWriteMemoryView & pGdsCache )
        {
            return DeserializeExternalProxy<false>::deserializeExternal( pValue, pReadCallback, pGdsCache );
        }

        template <typename TpValue, size_t tpSize>
        inline gds_size_t deserializeExternal( TpValue & pValue, const DataReadCallback & pReadCallback, std::array<byte, tpSize> & pGdsCache )
        {
            return DeserializeExternalProxy<false>::deserializeExternal( pValue, pReadCallback, pGdsCache );
        }

        template <typename TpValue>
        inline gds_size_t deserializeExternal( TpValue & pValue, const DataReadCallback & pReadCallback, DynamicByteArray & pGdsCache )
        {
            return DeserializeExternalProxy<true>::deserializeExternal( pValue, pReadCallback, pGdsCache );
        }

        template <typename TpValue>
        inline gds_size_t deserializeExternal( TpValue & pValue, const DataReadCallback & pReadCallback, DynamicMemoryBuffer & pGdsCache )
        {
            return DeserializeExternalProxy<true>::deserializeExternal( pValue, pReadCallback, pGdsCache );
        }

        /// @brief Deserializes object of the specified type and stores it in pValue. Binary data is read using the specified read callback.
        /// @param pValue The object to store the deserialized state.
        /// @param pReadCallback The read callback for data reading. Cannot be empty.
        /// @param pGdsCache Custom cache that will be used as a read buffer.
        template <typename TpValue>
        inline gds_size_t deserializeExternal( TpValue & pValue, const DataReadCallback & pReadCallback, std::vector<byte> & pGdsCache )
        {
            return DeserializeExternalProxy<true>::deserializeExternal( pValue, pReadCallback, pGdsCache );
        }

        /// @brief Deserializes object of the specified type and stores it in pValue. Binary data is read using the specified read callback.
        /// @param pValue The object to store the deserialized state.
        /// @param pReadCallback The read callback for data reading. Cannot be empty.
        template <typename TpValue>
        inline gds_size_t deserializeExternal( TpValue & pValue, const DataReadCallback & pReadCallback )
        {
            DynamicByteArray localGdsCache{};
            return deserializeExternal( pValue, pReadCallback, localGdsCache );
        }

    } // namespace gds

} // namespace ts3

#endif // __TS3_CORE_GDS_CORE_H__
