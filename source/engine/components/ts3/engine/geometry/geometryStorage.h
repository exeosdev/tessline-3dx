
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

		const GeometryDataFormat * dataFormat;

		GeometryContainerStorageCapacity capacity;

		GeometryBufferDesc indexBufferDesc;

		GeometryVertexStreamGenericArray<GeometryBufferDesc> vertexBufferDescArray;
	};

	class GeometryStorage : public GeometryContainer
	{
	public:
		using GeometryReferenceList = std::deque<GeometryReference>;

	public:
		GeometryStorage( const GeometryDataFormat & pDataFormat );

		virtual ~GeometryStorage();

		gpuapi::IAVertexStreamImmutableStateHandle getGpaVertexStreamState() const noexcept;

		gpuapi::GPUBufferHandle getIndexBuffer() const noexcept;

		gpuapi::GPUBufferHandle getVertexBuffer( uint32 pIndex ) const noexcept;

		GeometryReference * addIndexedGeometry( uint32 pVertexElementsNum, uint32 pIndexElementsNum );

		GeometryReference * addNonIndexedGeometry( uint32 pVertexElementsNum );

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
