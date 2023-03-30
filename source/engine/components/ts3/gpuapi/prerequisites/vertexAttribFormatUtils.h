
#pragma once

#ifndef __TS3_GPUAPI_VERTEX_FORMAT_ATTRIB_UTILS_H__
#define __TS3_GPUAPI_VERTEX_FORMAT_ATTRIB_UTILS_H__

namespace ts3::gpuapi
{

	template <typename TScalar>
	struct BaseScalarDataTypeTraits;

	template <>
	struct BaseScalarDataTypeTraits<int8>
	{
		static constexpr auto sBaseDataType = EBaseDataType::Byte;
		static constexpr auto sBaseDataFlags = E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT;
		static constexpr auto sByteSize = 1;
	};

	template <>
	struct BaseScalarDataTypeTraits<uint8>
	{
		static constexpr auto sBaseDataType = EBaseDataType::Ubyte;
		static constexpr auto sBaseDataFlags = E_GPU_DATA_FORMAT_FLAG_TYPE_UNSIGNED_BIT;
		static constexpr auto sByteSize = 1;
	};

	template <>
	struct BaseScalarDataTypeTraits<int16>
	{
		static constexpr auto sBaseDataType = EBaseDataType::Int16;
		static constexpr auto sBaseDataFlags = E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT;
		static constexpr auto sByteSize = 2;
	};

	template <>
	struct BaseScalarDataTypeTraits<uint16>
	{
		static constexpr auto sBaseDataType = EBaseDataType::Uint16;
		static constexpr auto sBaseDataFlags = E_GPU_DATA_FORMAT_FLAG_TYPE_UNSIGNED_BIT;
		static constexpr auto sByteSize = 2;
	};

	template <>
	struct BaseScalarDataTypeTraits<int32>
	{
		static constexpr auto sBaseDataType = EBaseDataType::Int32;
		static constexpr auto sBaseDataFlags = E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT;
		static constexpr auto sByteSize = 4;
	};

	template <>
	struct BaseScalarDataTypeTraits<uint32>
	{
		static constexpr auto sBaseDataType = EBaseDataType::Uint32;
		static constexpr auto sBaseDataFlags = E_GPU_DATA_FORMAT_FLAG_TYPE_UNSIGNED_BIT;
		static constexpr auto sByteSize = 4;
	};

	template <>
	struct BaseScalarDataTypeTraits<float>
	{
		static constexpr auto sBaseDataType = EBaseDataType::Float32;
		static constexpr auto sBaseDataFlags = E_GPU_DATA_FORMAT_FLAG_TYPE_SIGNED_BIT;
		static constexpr auto sByteSize = 4;
	};

	template <typename TMathType>
	struct VertexAttribFormatDataTypeTraits;

	template <size_t tSize>
	struct VertexAttribFormatDataTypeTraits<math::Vector<int64, tSize>>;

	template <size_t tSize>
	struct VertexAttribFormatDataTypeTraits<math::Vector<uint64, tSize>>;

	template <size_t tSize>
	struct VertexAttribFormatDataTypeTraits<math::Vector<double, tSize>>;

	template <size_t tRows, size_t tColumns>
	struct VertexAttribFormatDataTypeTraits<math::Matrix<int64, tRows, tColumns>>;

	template <size_t tRows, size_t tColumns>
	struct VertexAttribFormatDataTypeTraits<math::Matrix<uint64, tRows, tColumns>>;

	template <size_t tRows, size_t tColumns>
	struct VertexAttribFormatDataTypeTraits<math::Matrix<double, tRows, tColumns>>;

	template <typename TScalar, size_t tSize>
	struct VertexAttribFormatDataTypeTraits<math::Vector<TScalar, tSize>>
	{
		static constexpr auto sVertexAttribFormat = ( EVertexAttribFormat )cxdefs::declareVertexAttribFormat(
				BaseScalarDataTypeTraits<TScalar>::sBaseDataType,
				tSize,
				BaseScalarDataTypeTraits<TScalar>::sBaseDataFlags );

		static constexpr auto sVertexAttribComponentsNum = 1;
	};

	template <typename TScalar, size_t tRows, size_t tColumns>
	struct VertexAttribFormatDataTypeTraits<math::Matrix<TScalar, tRows, tColumns>>
	{
		static constexpr auto sVertexAttribFormat = ( EVertexAttribFormat )cxdefs::declareVertexAttribFormat(
				BaseScalarDataTypeTraits<TScalar>::sBaseDataType,
				tColumns,
				BaseScalarDataTypeTraits<TScalar>::sBaseDataFlags );

		static constexpr auto sVertexAttribComponentsNum = tRows;
	};

	template <>
	struct VertexAttribFormatDataTypeTraits<math::RGBAColor>
	{
		static constexpr auto sVertexAttribFormat = ( EVertexAttribFormat )cxdefs::declareVertexAttribFormat(
				BaseScalarDataTypeTraits<uint8>::sBaseDataType,
				4,
				BaseScalarDataTypeTraits<uint8>::sBaseDataFlags | E_GPU_DATA_FORMAT_FLAG_NORMALIZED_BIT );

		static constexpr auto sVertexAttribComponentsNum = 1;
	};

	template <>
	struct VertexAttribFormatDataTypeTraits<math::RGBAColorNorm<float>>
	{
		static constexpr auto sVertexAttribFormat = ( EVertexAttribFormat )cxdefs::declareVertexAttribFormat(
				BaseScalarDataTypeTraits<float>::sBaseDataType,
				4,
				BaseScalarDataTypeTraits<float>::sBaseDataFlags );

		static constexpr auto sVertexAttribComponentsNum = 1;
	};

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_VERTEX_FORMAT_ATTRIB_UTILS_H__
