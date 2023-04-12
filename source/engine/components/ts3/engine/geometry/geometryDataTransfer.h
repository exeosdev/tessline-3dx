
#pragma once

#ifndef __TS3_ENGINE_GEOMETRY_DATA_GPU_TRANSFER_H__
#define __TS3_ENGINE_GEOMETRY_DATA_GPU_TRANSFER_H__

#include "geometryCommonDefs.h"
#include <ts3/gpuapi/commandContext.h>

namespace ts3
{

	using GpaTransferCommandContext = gpuapi::CommandContextDirectTransfer;
	using GpaTransferCommandContextHandle = gpuapi::GpaHandle<gpuapi::CommandContextDirectTransfer>;

	class GeometryDataGpuTransfer : public CoreEngineObject
	{
	public:
		GpaTransferCommandContext & mTransferCmdContext;

	public:
		GeometryDataGpuTransfer(
				CoreEngineState & pCES,
				GpaTransferCommandContext & pTransferCmdContext );

		virtual ~GeometryDataGpuTransfer();

		virtual void initializeMeshData(
				const GeometryReference & pGeometryRef,
				const GeometryDataReferenceBase & pInputDataRef ) = 0;
	};

	class GeometryDataGpuTransferDirectCopy : public GeometryDataGpuTransfer
	{
	public:
		GeometryDataGpuTransferDirectCopy();

		virtual ~GeometryDataGpuTransferDirectCopy();

		virtual void initializeMeshData(
				const GeometryReference & pGeometryRef,
				const GeometryDataReferenceBase & pInputDataRef ) override final
		{
		}
	};

	class GeometryDataGpuTransferIntermediateBuffer : public GeometryDataGpuTransfer
	{
	public:
		GeometryDataGpuTransferIntermediateBuffer();
		virtual ~GeometryDataGpuTransferIntermediateBuffer();

		virtual void initializeMeshData(
				const GeometryReference & pGeometryRef,
				const GeometryDataReferenceBase & pInputDataRef )  override final
		{
		}
	};

	class GeometryDataGpuTransferUpload : public GeometryDataGpuTransfer
	{
	public:
		GeometryDataGpuTransferUpload(
				CoreEngineState & pCES,
				GpaTransferCommandContext & pTransferCmdContext);

		virtual ~GeometryDataGpuTransferUpload();

		virtual void initializeMeshData(
				const GeometryReference & pGeometryRef,
				const GeometryDataReferenceBase & pInputDataRef )  override final;
	};

} // namespace ts3

#endif // __TS3_ENGINE_GEOMETRY_DATA_GPU_TRANSFER_H__
