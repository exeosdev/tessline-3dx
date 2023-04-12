
#include "geometryDataTransfer.h"
#include "geometryDataFormat.h"
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


	GeometryDataGpuTransferUpload::GeometryDataGpuTransferUpload(
			CoreEngineState & pCES,
			GpaTransferCommandContext & pTransferCmdContext )
	: GeometryDataGpuTransfer( pCES, pTransferCmdContext )
	{}

	GeometryDataGpuTransferUpload::~GeometryDataGpuTransferUpload() = default;

	void GeometryDataGpuTransferUpload::initializeMeshData(
			const GeometryReference & pGeometryRef,
			const GeometryDataReferenceBase & pInputDataRef )
	{
		for( auto iVertexStream : pGeometryRef.dataReference.dataFormat->activeVertexStreams() )
		{
			const auto & vertexStreamFormat = pGeometryRef.dataReference.dataFormat->vertexStream( iVertexStream );
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

		if( pGeometryRef.dataReference.dataFormat->isIndexedGeometry() )
		{
			const auto & inputDataRegion = pInputDataRef.indexDataRegion;
			const auto & indexBufferRegion = pGeometryRef.dataReference.indexDataRegion;

			const auto indexBuffer = pGeometryRef.storage->getIndexBuffer();

			gpuapi::GPUBufferSubDataUploadDesc gpaDataUploadDesc;
			gpaDataUploadDesc.bufferRegion.offset = indexBufferRegion.offsetInElementsNum * pGeometryRef.dataReference.dataFormat->indexElementSizeInBytes();
			gpaDataUploadDesc.bufferRegion.size = indexBufferRegion.sizeInElementsNum * pGeometryRef.dataReference.dataFormat->indexElementSizeInBytes();
			gpaDataUploadDesc.inputDataDesc.pointer = inputDataRegion.dataPtr;
			gpaDataUploadDesc.inputDataDesc.size = inputDataRegion.sizeInElementsNum * inputDataRegion.elementSize;

			mTransferCmdContext.updateBufferSubDataUpload( *indexBuffer, gpaDataUploadDesc );
		}
	}

}
