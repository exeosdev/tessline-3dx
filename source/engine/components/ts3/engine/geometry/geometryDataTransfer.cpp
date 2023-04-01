
#include "geometryDataTransfer.h"
#include "geometryDataFormat.h"
#include "geometryReference.h"
#include "geometryStorage.h"

namespace ts3
{

	GeometryDataGpuTransfer::GeometryDataGpuTransfer(
			CoreEngineState & pCES,
			GpaTransferCommandContext & pTransferCmdContext )
	: CoreEngineObject( pCES )
	, mTransferCmdContext( pTransferCmdContext )
	{}

	GeometryDataGpuTransfer::~GeometryDataGpuTransfer() = default;


	GeometryDataGpuTransferDirect::GeometryDataGpuTransferDirect(
			CoreEngineState & pCES,
			GpaTransferCommandContext & pTransferCmdContext )
	: GeometryDataGpuTransfer( pCES, pTransferCmdContext )
	{}

	GeometryDataGpuTransferDirect::~GeometryDataGpuTransferDirect() = default;

	void GeometryDataGpuTransferDirect::initializeMeshData(
			const GeometryDataFormat & pGeometryDataFormat,
			const GeometryReference & pGeometryRef,
			const GeometryDataReference & pInputDataRef )
	{
		for( uint32 iVertexStream = 0; iVertexStream < gpa::MAX_GEOMETRY_VERTEX_STREAMS_NUM; ++iVertexStream )
		{
			if( pGeometryDataFormat.isVertexStreamActive( iVertexStream ) )
			{
				const auto & vertexStreamFormat = pGeometryDataFormat.vertexStream( iVertexStream );
				const auto & inputDataRegion = pInputDataRef.vertexStreamDataRegions[iVertexStream];
				const auto & vertexBufferRegion = pGeometryRef.dataReference.vertexStreamDataRegions[iVertexStream];

				const auto vertexBuffer = pGeometryRef.storage->getVertexBuffer( iVertexStream );

				gpuapi::GPUBufferSubDataUploadDesc gpaDataUploadDesc;
				gpaDataUploadDesc.bufferRegion.offset = vertexBufferRegion.offsetInElementsNum * vertexStreamFormat.elementSizeInBytes;
				gpaDataUploadDesc.bufferRegion.size = vertexBufferRegion.sizeInElementsNum * vertexStreamFormat.elementSizeInBytes;
				gpaDataUploadDesc.inputDataDesc.pointer = inputDataRegion.dataPtr;
				gpaDataUploadDesc.inputDataDesc.size = inputDataRegion.sizeInElementsNum * inputDataRegion.elementSize;

				mTransferCmdContext.updateBufferSubDataUpload( *vertexBuffer, gpaDataUploadDesc );
			}
		}

		if( pGeometryDataFormat.isIndexedGeometry() )
		{
			const auto & inputDataRegion = pInputDataRef.indexDataRegion;
			const auto & indexBufferRegion = pGeometryRef.dataReference.indexDataRegion;

			const auto indexBuffer = pGeometryRef.storage->getIndexBuffer();

			gpuapi::GPUBufferSubDataUploadDesc gpaDataUploadDesc;
			gpaDataUploadDesc.bufferRegion.offset = indexBufferRegion.offsetInElementsNum * pGeometryDataFormat.indexElementSizeInBytes();
			gpaDataUploadDesc.bufferRegion.size = indexBufferRegion.sizeInElementsNum * pGeometryDataFormat.indexElementSizeInBytes();
			gpaDataUploadDesc.inputDataDesc.pointer = inputDataRegion.dataPtr;
			gpaDataUploadDesc.inputDataDesc.size = inputDataRegion.sizeInElementsNum * inputDataRegion.elementSize;

			mTransferCmdContext.updateBufferSubDataUpload( *indexBuffer, gpaDataUploadDesc );
		}
	}

}
