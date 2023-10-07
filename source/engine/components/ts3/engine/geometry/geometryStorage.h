
#pragma once

#ifndef __TS3_ENGINE_GEOMETRY_STORAGE_H__
#define __TS3_ENGINE_GEOMETRY_STORAGE_H__

#include "geometryContainer.h"
#include <ts3/gpuapi/state/inputAssemblerImmutableStates.h>
#include <deque>

namespace ts3
{

	enum class EGeometryBufferAllocationMode : enum_default_value_t
	{
		AllocLocal,
		ShareExternal
	};

	struct GeometryStorageCreateInfo
	{
		struct GeometryBufferDesc
		{
			EGeometryBufferAllocationMode allocationMode = EGeometryBufferAllocationMode::AllocLocal;
			EGPUBufferUsagePolicy bufferUsagePolicy = EGPUBufferUsagePolicy::Undefined;
		};

		EGPUBufferUsagePolicy commonBufferUsagePolicy = EGPUBufferUsagePolicy::Undefined;

		const GeometryDataFormatBase * dataFormat;

		GeometryContainerStorageCapacity capacity;

		GeometryBufferDesc indexBufferDesc;

		GeometryVertexStreamGenericArray<GeometryBufferDesc> vertexBufferDescArray;
	};

	template <size_t tVertexStreamArraySize>
	class GeometrySharedStorage : public GeometryContainer<tVertexStreamArraySize>
	{
	public:
		using GeometryRefList = std::deque<GeometryReference>;

	public:
		GeometrySharedStorage( const GeometryDataFormatBase & pDataFormat );

		virtual ~GeometrySharedStorage() = default;

		gpuapi::IAVertexStreamImmutableStateHandle getGpaVertexStreamState() const noexcept;

		SharedGeometryRefHandle addIndexedGeometry( uint32 pVertexElementsNum, uint32 pIndexElementsNum );

		SharedGeometryRefHandle addNonIndexedGeometry( uint32 pVertexElementsNum );

		void releaseStorage() {}

		static GeometryStoragePtr createStorage(
				const CoreEngineState & pCES,
				const GeometryStorageCreateInfo & pCreateInfo,
				const GeometryStorage * pSharedStorage = nullptr );

	private:
		void createStorageGPUBuffers( const CoreEngineState & pCES, const GeometryStorageCreateInfo & pCreateInfo );

		void bindSharedGPUBuffers( const GeometryStorageCreateInfo & pCreateInfo, const GeometryStorage & pSharedStorage );

		void initializeVertexStreamState( const CoreEngineState & pCES );

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
		GeometryReferenceList _geometryRefList;
		gpuapi::GPUBufferHandle _indexBuffer;
		GeometryVertexStreamGenericArray<gpuapi::GPUBufferHandle> _vertexBufferArray;
		Bitmask<gpuapi::EIAVertexStreamBindingFlags> _vertexStreamBindingMask;
		gpuapi::IAVertexStreamImmutableStateHandle _gpaVertexStreamState;
	};

} // namespace ts3

#endif // __TS3_ENGINE_GEOMETRY_STORAGE_H__
