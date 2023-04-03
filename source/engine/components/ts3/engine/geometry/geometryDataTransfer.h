
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
				const GeometryDataReference & pInputDataRef ) = 0;
	};

	class GeometryDataGpuTransferDirect : public GeometryDataGpuTransfer
	{
	public:
		GeometryDataGpuTransferDirect(
				CoreEngineState & pCES,
				GpaTransferCommandContext & pTransferCmdContext );

		virtual ~GeometryDataGpuTransferDirect();

		virtual void initializeMeshData(
				const GeometryReference & pGeometryRef,
				const GeometryDataReference & pInputDataRef ) override final;
	};

	class GeometryDataGpuTransferIntermediateBuffer : public GeometryDataGpuTransfer
	{
	public:
		GeometryDataGpuTransferIntermediateBuffer();
		virtual ~GeometryDataGpuTransferIntermediateBuffer();

		virtual void initializeMeshData(
				const GeometryReference & pGeometryRef,
				const GeometryDataReference & pInputDataRef )  override final
		{
		}
	};

	class GeometryDataGpuTransferUpload : public GeometryDataGpuTransfer
	{
	public:
		GeometryDataGpuTransferUpload();
		virtual ~GeometryDataGpuTransferUpload();

		virtual void initializeMeshData(
				const GeometryReference & pGeometryRef,
				const GeometryDataReference & pInputDataRef )  override final
		{
		}
	};

} // namespace ts3

#endif // __TS3_ENGINE_GEOMETRY_DATA_GPU_TRANSFER_H__
