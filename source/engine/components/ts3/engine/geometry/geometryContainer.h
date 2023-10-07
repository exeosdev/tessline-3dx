
#pragma once

#ifndef __TS3_ENGINE_GEOMETRY_CONTAINER_H__
#define __TS3_ENGINE_GEOMETRY_CONTAINER_H__

#include "geometryDataFormat.h"
#include "geometryBuffer.h"

namespace ts3
{

	struct GeometrySizeMetrics
	{
		uint32 indexDataSizeInElementsNum;
		uint32 indexElementSizeInBytes;
		gpuapi::EIndexDataFormat indexElementType;
		uint32 vertexDataSizeInElementsNum;
	};

	struct GeometryStorageCapacity
	{
		uint32 indexElementsNum = 0;
		uint32 vertexElementsNum = 0;
	};

	struct GeometrySharedStorageMetrics
	{
		GeometryStorageCapacity capacity;
		GeometrySize geometrySize;
		uint32 indexDataAllocationOffsetInElementsNum = 0;
		uint32 vertexDataAllocationOffsetInElementsNum = 0;
	};

	class GeometryDataStoreBase
	{
	public:
		GeometryDataFormatBase const mDataFormat;

	public:
		virtual GeometryBufferDataRefReadOnly getIndexDataPtrReadOnly() const noexcept = 0;

		virtual GeometryBufferDataRefReadWrite getIndexDataPtrReadWrite() = 0;

		virtual GeometryBufferDataRefReadOnly getVertexDataPtrReadOnly( size_t pVertexStreamIndex ) const noexcept = 0;

		virtual GeometryBufferDataRefReadWrite getVertexDataPtrReadWrite( size_t pVertexStreamIndex ) = 0;

		GeometryBufferDataRefReadOnly getVertexAttributeDataSubRegionReadOnly() const noexcept;

		GeometryBufferDataRefReadWrite getVertexAttributeDataSubRegionReadWrite();
	};

	template <typename TBufferData, size_t tVertexStreamArraySize>
	class GeometryDataStore : public GeometryDataStoreBase
	{
	protected:
		TBufferData _indexBufferData
	};

	template <typename TBufferData>
	class GeometryDataStore<TBufferData, 0> : public GeometryDataStoreBase
	{
	};

	class GeometryContainerBase
	{
	public:
		GeometryDataFormatBase const mDataFormat;
		GeometryStorageCapacity const mStorageCapacity;

	public:
		GeometryContainerBase( const GeometryDataFormatBase & pDataFormat, const GeometryStorageCapacity & pStorageCapacity )
		: mDataFormat( pDataFormat )
		, mStorageCapacity( pStorageCapacity )
		{}

		virtual ~GeometryContainerBase();

		TS3_ATTR_NO_DISCARD bool isStorageInitialized() const noexcept;

		TS3_ATTR_NO_DISCARD bool isIndexedGeometry() const noexcept;

		TS3_ATTR_NO_DISCARD GeometryIndexBufferReference getIndexBuffer() const noexcept;

		TS3_ATTR_NO_DISCARD GeometryVertexBufferReference getVertexBuffer( size_t pVertexStreamIndex ) const noexcept;

		bool setIndexBuffer( const GeometryIndexBufferReference & pIndexBufferReference );

		bool setVertexBuffer( uint32 pVertexStreamIndex, const GeometryVertexBufferReference & pVertexBufferReference );

	protected:
		void setVertexBufferRefsStorage( GeometryVertexBufferReference * pVertexBufferRefsPtr );

		void initializeContainerStorage();

	protected:
		GeometryDataReference _allGeometryDataRef;
		GeometryIndexBufferReference _indexBufferRefPtr;
		GeometryVertexBufferReference * _vertexBufferRefsPtr;
	};

	template <size_t tVertexStreamArraySize>
	class GeometryContainer : public GeometryContainerBase
	{
	public:
		GeometryContainer( const GeometryDataFormatBase & pDataFormat, const GeometryStorageCapacity & pStorageCapacity )
		: GeometryContainerBase( pDataFormat, pStorageCapacity )
		{
			setVertexBufferRefsStorage( _vertexBufferRefs.data() );
		}

	private:
		using GeometryVertexBufferRefArray = std::array<GeometryVertexBufferReference, tVertexStreamArraySize>;
		GeometryVertexBufferRefArray _vertexBufferRefs;
	};

	template <>
	class GeometryContainer<0> : public GeometryContainerBase
	{
	public:
		GeometryContainer( const GeometryDataFormatBase & pDataFormat, const GeometryStorageCapacity & pStorageCapacity )
		: GeometryContainerBase( pDataFormat, pStorageCapacity )
		{
			_vertexBufferRefs.resize( pDataFormat.activeVertexStreamsNum() );
			setVertexBufferRefsStorage( _vertexBufferRefs.data() );
		}

	private:
		using GeometryVertexBufferRefDynamicArray = std::vector<GeometryVertexBufferReference>;
		GeometryVertexBufferRefDynamicArray _vertexBufferRefs;
	};

	class GeometryDataSource
	{
	public:
		GeometryDataSource();
		~GeometryDataSource();

	private:
		GeometryReference _geometryRef;
	};

	class GeometryContainer2
	{
	public:
		const GeometryDataFormatBase & mDataFormat;

	public:
		GeometryContainer2( const GeometryDataFormatBase & pDataFormat );

		TS3_ATTR_NO_DISCARD bool isStorageInitialized() const noexcept;

		TS3_ATTR_NO_DISCARD bool isIndexedGeometryContainer() const noexcept;

		TS3_ATTR_NO_DISCARD const GeometryDataReferenceBase & getAllGeometryDataRef() const noexcept;

		TS3_ATTR_NO_DISCARD const GeometryContainerStorageMetrics & getStorageMetrics() const noexcept;

		TS3_ATTR_NO_DISCARD const GeometrySize & getGeometrySize() const noexcept;

	protected:
		bool checkStorageAvailableSpace( uint32 pVertexElementsNum, uint32 pIndexElementsNum ) const noexcept;

		void setContainerCapacity( uint32 pVertexElementsNum, uint32 pIndexElementsNum );

		void updateStoredGeometrySize( uint32 pVertexElementsNum, uint32 pIndexElementsNum );

	protected:
		GeometryContainerStorageMetrics _storageMetrics;
		GeometryDataReferenceBase _allGeometryDataRef;
	};

} // namespace ts3

#endif // __TS3_ENGINE_GEOMETRY_CONTAINER_H__
