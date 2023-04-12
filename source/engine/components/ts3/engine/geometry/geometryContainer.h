
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

	template <typename TGeometryBufferType>
	struct GeometryBufferReference
	{
		SharedHandle<TGeometryBufferType> buffer;

		explicit operator bool() const noexcept
		{
			return static_cast<bool>( buffer );
		}

		IndexBuffer * operator->() const noexcept
		{
			ts3DebugAssert( buffer );
			return buffer.get();
		}

		IndexBuffer & operator*() const noexcept
		{
			ts3DebugAssert( buffer );
			return *buffer;
		}
	};

	using GeometryIndexBufferReference = GeometryBufferReference<IndexBuffer>;
	using GeometryVertexBufferReference = GeometryBufferReference<VertexBuffer>;

	template <size_t tVSN>
	struct GeometryStorageMetrics
	{
	};

	class GeometryContainerBase
	{
	public:
		GeometryDataFormat const mDataFormat;

	public:
		GeometryContainerBase( const GeometryDataFormat & pDataFormat )
		: mDataFormat( pDataFormat )
		, _activeVertexStreamsNum( pDataFormat.activeVertexStreamsNum() )
		{}

		virtual ~GeometryContainerBase();

		TS3_ATTR_NO_DISCARD bool isStorageInitialized() const noexcept;

		TS3_ATTR_NO_DISCARD bool isIndexedGeometryContainer() const noexcept;

		TS3_ATTR_NO_DISCARD GeometryIndexBufferReference getIndexBuffer() const noexcept;

		TS3_ATTR_NO_DISCARD GeometryVertexBufferReference getVertexBuffer( size_t pBufferIndex ) const noexcept;

	protected:
		void setVertexBufferRefsStorage( GeometryVertexBufferReference * pVertexBufferRefsPtr );

	protected:
		uint32 _activeVertexStreamsNum;
		GeometryIndexBufferReference _indexBufferRefPtr;
		GeometryVertexBufferReference * _vertexBufferRefsPtr;
	};

	template <size_t tVSN>
	class GeometryContainer : public GeometryContainerBase
	{
	public:
		using DataReference = GeometryDataReference<tVSN>;
		using SharedGeometryRef = SharedGeometryReference<tVSN>;

		GeometryContainer( const GeometryDataFormat & pDataFormat )
		: GeometryContainerBase( pDataFormat )
		{
			setVertexBufferRefsStorage( _vertexBufferRefs.data() );
		}

	private:
		using GeometryVertexBufferRefArray = std::array<GeometryVertexBufferReference, tVSN>;
		DataReference _dataRef;
		GeometryVertexBufferRefArray _vertexBufferRefs;
		SharedGeometryRefHandle _sharedGeometryRef;
	};

	template <>
	class GeometryContainer<0> : public GeometryContainerBase
	{
	public:
		using DataReference = GeometryDataReference<gpa::MAX_GEOMETRY_VERTEX_STREAMS_NUM>;
		using SharedGeometryRef = SharedGeometryReference<gpa::MAX_GEOMETRY_VERTEX_STREAMS_NUM>;

		GeometryDataSource( const GeometryDataFormat & pDataFormat )
		: GeometryContainerBase( pDataFormat )
		{
			_vertexBufferRefs.resize( pDataFormat.activeVertexStreamsNum() );
			setVertexBufferRefsStorage( _vertexBufferRefs.data() );
		}

	private:
		using GeometryVertexBufferRefDynamicArray = std::vector<GeometryVertexBufferReference>;
		GeometryVertexBufferRefDynamicArray _vertexBufferRefs;
	};

	// Static mesh, dynamic mesh, skinned mesh, particle, etc.
	class GeometryDataSourceBase : public GeometryContainer
	{
	public:
		GeometryDataSourceBase( const GeometryDataFormat & pDataFormat )
		: GeometryContainer( pDataFormat )
		, _geometryDataRef( nullptr )
		{}

		GeometryDataSourceBase( GeometryDataReferenceBase & pGeometryDataRef )
		: GeometryContainer( *pGeometryDataRef.dataFormat )
		, _geometryDataRef( &pGeometryDataRef )
		{}

		bool setIndexBuffer( const GeometryIndexBufferReference & pIndexBufferReference );

		bool setVertexBuffer( uint32 pBufferIndex, const GeometryVertexBufferReference & pVertexBufferReference );

	private:
		GeometryDataReferenceBase * _geometryDataRef;
		SharedGeometryRefHandle _sharedGeometryRef;
	};

	template <size_t tVSN>
	class GeometryDataSource : public GeometryDataSourceBase
	{
	public:
		using DataReference = GeometryDataReference<tVSN>;
		using SharedGeometryRef = SharedGeometryReference<tVSN>;

		GeometryDataSource( const GeometryDataFormat & pDataFormat )
		: GeometryDataSourceBase( pDataFormat )
		{
			setVertexBufferRefsStorage( _vertexBufferRefs.data() );
		}

		GeometryDataSource( SharedGeometryRefHandle pSharedGeometryRef )
		: GeometryDataSourceBase( *pSharedGeometryRef->dataReferencePtr->dataFormat )
		, _sharedGeometryRef( pSharedGeometryRef )
		{
			setVertexBufferRefsStorage( _vertexBufferRefs.data() );
		}

	private:
		using GeometryVertexBufferRefArray = std::array<GeometryVertexBufferReference, tVSN>;
		DataReference _dataRef;
		GeometryVertexBufferRefArray _vertexBufferRefs;
		SharedGeometryRefHandle _sharedGeometryRef;
	};

	template <>
	class GeometryDataSource<0> : public GeometryDataSourceBase
	{
	public:
		using DataReference = GeometryDataReference<gpa::MAX_GEOMETRY_VERTEX_STREAMS_NUM>;
		using SharedGeometryRef = SharedGeometryReference<gpa::MAX_GEOMETRY_VERTEX_STREAMS_NUM>;

		GeometryDataSource( const GeometryDataFormat & pDataFormat )
		: GeometryDataSourceBase( pDataFormat )
		{
			_vertexBufferRefs.resize( pDataFormat.activeVertexStreamsNum() );
			setVertexBufferRefsStorage( _vertexBufferRefs.data() );
		}

	private:
		using GeometryVertexBufferRefDynamicArray = std::vector<GeometryVertexBufferReference>;
		GeometryVertexBufferRefDynamicArray _vertexBufferRefs;
	};

	struct GeometryContainerStorageCapacity
	{
		uint32 indexDataCapacityInElementsNum = 0;
		uint32 vertexDataCapacityInElementsNum = 0;
	};

	struct GeometryContainerStorageMetrics
	{
		GeometryContainerStorageCapacity capacity;
		GeometrySize geometrySize;
		uint32 indexDataAllocationOffsetInElementsNum = 0;
		uint32 vertexDataAllocationOffsetInElementsNum = 0;
	};

	class GeometryContainer2
	{
	public:
		const GeometryDataFormat & mDataFormat;

	public:
		GeometryContainer2( const GeometryDataFormat & pDataFormat );

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
