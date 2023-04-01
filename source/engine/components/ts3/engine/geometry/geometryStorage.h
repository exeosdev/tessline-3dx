
#pragma once

#ifndef __TS3_ENGINE_GEOMETRY_STORAGE_H__
#define __TS3_ENGINE_GEOMETRY_STORAGE_H__

#include "geometryDataFormat.h"
#include "geometryReference.h"
#include <ts3/gpuapi/state/inputAssemblerImmutableStates.h>
#include <deque>

namespace ts3
{

	enum class EGeometryBufferAllocationMode : enum_default_value_t
	{
		AllocLocal,
		ShareExternal
	};

	struct GeometryStorageMetrics
	{
		uint32 indexDataCapacityInElementsNum;

		uint32 vertexDataCapacityInElementsNum;
	};

	struct GeometryStorageCreateInfo
	{
		struct GeometryBufferDesc
		{
			EGeometryBufferAllocationMode allocationMode = EGeometryBufferAllocationMode::AllocLocal;

			EGPUBufferUsagePolicy bufferUsagePolicy = EGPUBufferUsagePolicy::Undefined;
		};

		EGPUBufferUsagePolicy commonBufferUsagePolicy = EGPUBufferUsagePolicy::Undefined;

		const GeometryDataFormat * dataFormat;

		GeometryStorageMetrics metrics;

		GeometryBufferDesc indexBufferDesc;

		GeometryVertexStreamGenericArray<GeometryBufferDesc> vertexBufferDescArray{};
	};

	class GeometryStorage : public CoreEngineObject
	{
	public:
		using GeometryReferenceList = std::deque<GeometryReference>;

		GeometryDataFormat const mDataFormat;

		GeometryStorageMetrics const mStorageMetrics;

	public:
		GeometryStorage(
			const CoreEngineState & pCES,
			const GeometryDataFormat & pDataFormat,
			const GeometryStorageMetrics & pStorageMetrics );

		virtual ~GeometryStorage();

		gpuapi::IAVertexStreamImmutableStateHandle getGpaVertexStreamState() const noexcept;

		gpuapi::GPUBufferHandle getIndexBuffer() const noexcept;

		gpuapi::GPUBufferHandle getVertexBuffer( uint32 pIndex ) const noexcept;

		const GeometryReference & getAllGeometryReference() const noexcept;

		GeometryReference * addIndexedGeometry( uint32 pVertexElementsNum, uint32 pIndexElementsNum );

		GeometryReference * addNonIndexedGeometry( uint32 pVertexElementsNum );

		static std::unique_ptr<GeometryStorage> createStorage(
				const CoreEngineState & pCES,
				const GeometryStorageCreateInfo & pCreateInfo,
				const GeometryStorage * pSharedStorage = nullptr );

	private:
		void createStorageGPUBuffers( const GeometryStorageCreateInfo & pCreateInfo );

		void bindSharedGPUBuffers( const GeometryStorageCreateInfo & pCreateInfo, const GeometryStorage & pSharedStorage );

		void initializeVertexStreamState();

		GeometryReference * addGeometry( uint32 pVertexElementsNum, uint32 pIndexElementsNum );

		TS3_ATTR_NO_DISCARD static gpuapi::GPUBufferHandle createIndexBuffer(
				const CoreEngineState & pCES,
				gpuapi::gpu_memory_size_t pBufferSize,
				EGPUBufferUsagePolicy pUsagePolicy );

		TS3_ATTR_NO_DISCARD static gpuapi::GPUBufferHandle createVertexBuffer(
				const CoreEngineState & pCES,
				gpuapi::gpu_memory_size_t pBufferSize,
				EGPUBufferUsagePolicy pUsagePolicy );

		TS3_ATTR_NO_DISCARD static bool validateSharedBuffersConfiguration(
				const GeometryStorageCreateInfo & pCreateInfo,
				const GeometryStorage & pSharedStorage );

		TS3_ATTR_NO_DISCARD static EGPUBufferUsagePolicy resolveGeometryBufferUsagePolicy(
				EGPUBufferUsagePolicy pBufferUsagePolicy,
				EGPUBufferUsagePolicy pCommonUsagePolicy );

	private:
		struct CurrentAllocationState
		{
			uint32 indicesOffsetInElementsNum = 0;
			uint32 verticesOffsetInElementsNum = 0;
		};

		struct GeometryBufferState
		{
			gpuapi::GPUBufferHandle gpuBuffer;
		};

		CurrentAllocationState _currentAllocationState;
		GeometryReference _allGeometryReference;
		GeometryReferenceList _geometryRefList;
		GeometryBufferState _indexBufferState;
		GeometryVertexStreamGenericArray<GeometryBufferState> _vertexBufferStateArray;
		Bitmask<gpuapi::EIAVertexStreamBindingFlags> _vertexStreamBindingMask;
		gpuapi::IAVertexStreamImmutableStateHandle _gpaVertexStreamState;
	};

} // namespace ts3

#endif // __TS3_ENGINE_GEOMETRY_STORAGE_H__
