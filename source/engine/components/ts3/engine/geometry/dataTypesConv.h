
#pragma once

#ifndef __TS3_ENGINE_DATA_TYPES_CONV_H__
#define __TS3_ENGINE_DATA_TYPES_CONV_H__

#include "geometryCommonDefs.h"
#include <ts3/math/vector.h>

namespace ts3
{

	using DataTypeConversionFunction = std::function<void( const void *, void *, uint32 )>;

	template <typename TInput, typename TOutput>
	struct DataTypeConverter
	{
		static void convertValue( const TInput * pInputElements, TOutput * pOutputElements )
		{
			*pOutputElements = numeric_cast<TOutput>( *pInputElements );
		}

		static void convertVector( const TInput * pInputElements, TOutput * pOutputElements, uint32 pElementsNum )
		{
			for( uint32 iElement = 0; iElement < pElementsNum; ++iElement )
			{
				pOutputElements[iElement] = numeric_cast<TOutput>( pInputElements[iElement] );
			}
		}
	};

	template <typename TInput>
	struct GeometryDataConverter
	{
		template <typename TOutput>
		static DataTypeConversionFunction getConversionFunction()
		{
			return []( const void * pInputElements, void * pOutputElements, uint32 ) {
				DataTypeConverter<TInput, TOutput>::convertValue(
						reinterpret_cast<const TInput *>( pInputElements ),
						reinterpret_cast<TOutput *>( pOutputElements ) );
			};
		}
	};

	template <typename TValue, size_t tVectorSize>
	struct GeometryDataConverter<math::Vector<TValue, tVectorSize>>
	{
		template <typename TOutput>
		static DataTypeConversionFunction getConversionFunction()
		{
			return []( const void * pInputElements, void * pOutputElements, uint32 pElementsNum ) {
				DataTypeConverter<TValue, TOutput>::convertVector(
						reinterpret_cast<const TValue *>( pInputElements ),
						reinterpret_cast<TOutput *>( pOutputElements ),
						getMinOf( tVectorSize, pElementsNum ) );
			};
		}
	};


	namespace gmutil
	{

		template <typename TInput>
		inline DataTypeConversionFunction getGeometryConversionFunction( gpuapi::EBaseDataType pOutputElementType )
		{
			switch( pOutputElementType )
			{
				case gpuapi::EBaseDataType::Byte:
				{
					return GeometryDataConverter<TInput>::template getConversionFunction<int8>();
				}
				case gpuapi::EBaseDataType::Ubyte:
				{
					return GeometryDataConverter<TInput>::template getConversionFunction<uint8>();
				}
				case gpuapi::EBaseDataType::Int16:
				{
					return GeometryDataConverter<TInput>::template getConversionFunction<int16>();
				}
				case gpuapi::EBaseDataType::Uint16:
				{
					return GeometryDataConverter<TInput>::template getConversionFunction<uint16>();
				}
				case gpuapi::EBaseDataType::Int32:
				{
					return GeometryDataConverter<TInput>::template getConversionFunction<int32>();
				}
				case gpuapi::EBaseDataType::Uint32:
				{
					return GeometryDataConverter<TInput>::template getConversionFunction<uint32>();
				}
				case gpuapi::EBaseDataType::Float16:
				{
					return GeometryDataConverter<TInput>::template getConversionFunction<uint16>();
				}
				case gpuapi::EBaseDataType::Float32:
				{
					return GeometryDataConverter<TInput>::template getConversionFunction<float>();
				}
				default:
				{
					break;
				}
			}

			return {};
		}

	}

} // namespace ts3

#endif // __TS3_ENGINE_DATA_TYPES_CONV_H__
