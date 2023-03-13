
#pragma once

#ifndef __TS3_GPUAPI_GPU_CMD_CONTEXT_H__
#define __TS3_GPUAPI_GPU_CMD_CONTEXT_H__

#include "commonCommandDefs.h"
#include "resources/commonGPUResourceDefs.h"

namespace ts3::gpuapi
{

	class RenderPassConfigurationDynamicState;
	class RenderPassConfigurationImmutableState;

	enum ECommandListActionFlags : uint32;

	class CommandContext : public GPUDeviceChildObject
	{
	public:
		static const Bitmask<ECommandListFlags> sListFlagsCommon;

		CommandList * const mCommandList = nullptr;
		CommandSystem * const mCommandSystem = nullptr;

		CommandContext( CommandSystem & pCommandSystem, CommandList & pCommandList );
		virtual ~CommandContext();

		void beginCommandSequence();
		void endCommandSequence();

		bool mapBuffer( GPUBuffer & pBuffer, EGPUMemoryMapMode pMapMode );
		bool mapBufferRegion( GPUBuffer & pBuffer, const GPUMemoryRegion & pRegion, EGPUMemoryMapMode pMapMode );
		bool unmapBuffer( GPUBuffer & pBuffer );
		bool flushMappedBuffer( GPUBuffer & pBuffer );
		bool flushMappedBufferRegion( GPUBuffer & pBuffer, const GPUMemoryRegion & pRegion );

	protected:
		bool checkCommandListSupport( Bitmask<ECommandListFlags> pCmdListFlags );
	};

	class CommandContextDirect : public CommandContext
	{
	public:
		static const Bitmask<ECommandListFlags> sListFlagsDirect;

		CommandContextDirect( CommandSystem & pCommandSystem, CommandList & pCommandList )
		: CommandContext( pCommandSystem, pCommandList )
		{}

		virtual ~CommandContextDirect() = default;

		void submit();
		CommandSync submit( const CommandContextSubmitInfo & pSubmitInfo );

		void cmdExecuteDeferredContext( CommandContextDeferred & pDeferredContext );

		bool invalidateBuffer( GPUBuffer & pBuffer );
		bool invalidateBufferRegion( GPUBuffer & pBuffer, const GPUMemoryRegion & pRegion );
	};

	class CommandContextDirectTransfer : public CommandContextDirect
	{
	public:
		static ECommandContextType const sContextType = ECommandContextType::DirectTransfer;
		static Bitmask<ECommandListFlags> const sListFlagsDirectTransfer;

		CommandContextDirectTransfer( CommandSystem & pCommandSystem, CommandList & pCommandList )
		: CommandContextDirect( pCommandSystem, pCommandList )
		{}

		virtual ~CommandContextDirectTransfer() = default;

		bool updateBufferDataCopy( GPUBuffer & pBuffer, GPUBuffer & pSourceBuffer, const GPUBufferDataCopyDesc & pCopyDesc );
		bool updateBufferSubDataCopy( GPUBuffer & pBuffer, GPUBuffer & pSourceBuffer, const GPUBufferSubDataCopyDesc & pCopyDesc );
		bool updateBufferDataUpload( GPUBuffer & pBuffer, const GPUBufferDataUploadDesc & pUploadDesc );
		bool updateBufferSubDataUpload( GPUBuffer & pBuffer, const GPUBufferSubDataUploadDesc & pUploadDesc );
	};

	class CommandContextDirectCompute : public CommandContextDirectTransfer
	{
	public:
		static ECommandContextType const sContextType = ECommandContextType::DirectCompute;
		static Bitmask<ECommandListFlags> const sListFlagsDirectCompute;

		CommandContextDirectCompute( CommandSystem & pCommandSystem, CommandList & pCommandList )
		: CommandContextDirectTransfer( pCommandSystem, pCommandList )
		{}

		virtual ~CommandContextDirectCompute() = default;

		void cmdDispatchCompute( uint32 pThrGroupSizeX, uint32 pThrGroupSizeY, uint32 pThrGroupSizeZ );
		void cmdDispatchComputeIndirect( uint32 pIndirectBufferOffset );
	};

	class CommandContextDirectGraphics : public CommandContextDirectCompute
	{
	public:
		static ECommandContextType const sContextType = ECommandContextType::DirectGraphics;
		static Bitmask<ECommandListFlags> const sListFlagsDirectGraphics;

		CommandContextDirectGraphics( CommandSystem & pCommandSystem, CommandList & pCommandList )
		: CommandContextDirectCompute( pCommandSystem, pCommandList )
		{}

		~CommandContextDirectGraphics() = default;
		
		bool beginRenderPass(
			const RenderPassConfigurationImmutableState & pRenderPassState,
			Bitmask<ECommandListActionFlags> pFlags );

		bool beginRenderPass(
			const RenderPassConfigurationDynamicState & pRenderPassState,
			Bitmask<ECommandListActionFlags> pFlags );

		void endRenderPass();

		bool setGraphicsPipelineStateObject( const GraphicsPipelineStateObject & pGraphicsPSO );
		bool setIAVertexStreamState( const IAVertexStreamImmutableState & pIAVertexStreamState );
		bool setIAVertexStreamState( const IAVertexStreamDynamicState & pIAVertexStreamState );
		bool setRenderTargetBindingState( const RenderTargetBindingImmutableState & pRenderTargetBindingState );
		bool setRenderTargetBindingState( const RenderTargetBindingDynamicState & pRenderTargetBindingState );

		bool cmdSetBlendConstantColor( const math::RGBAColorR32Norm & pColor );
		bool cmdSetViewport( const ViewportDesc & pViewportDesc );
		bool cmdSetShaderConstant( shader_input_ref_id_t pParamRefID, const void * pData );
		bool cmdSetShaderConstantBuffer( shader_input_ref_id_t pParamRefID, GPUBuffer & pConstantBuffer );
		bool cmdSetShaderTextureImage( shader_input_ref_id_t pParamRefID, Texture & pTexture );
		bool cmdSetShaderTextureSampler( shader_input_ref_id_t pParamRefID, Sampler & pSampler );

		void cmdDrawDirectIndexed( native_uint pIndicesNum, native_uint pIndicesOffset );
		void cmdDrawDirectIndexedInstanced( native_uint pIndicesNumPerInstance, native_uint pInstancesNum, native_uint pIndicesOffset );
		void cmdDrawDirectNonIndexed( native_uint pVerticesNum, native_uint pVerticesOffset );
		void cmdDrawDirectNonIndexedInstanced( native_uint pVerticesNumPerInstance, native_uint pInstancesNum, native_uint pVerticesOffset );
	};

	class CommandContextDeferred : public CommandContext
	{
	public:
		static const Bitmask<ECommandListFlags> sListFlagsDeferred;

		CommandContextDeferred( CommandSystem & pCommandSystem, CommandList & pCommandList )
		: CommandContext( pCommandSystem, pCommandList )
		{}

		virtual ~CommandContextDeferred() = default;

		bool mapBufferDeferred( GPUBuffer & pBuffer );
		bool mapBufferRegionDeferred( GPUBuffer & pBuffer, const GPUMemoryRegion & pRegion );
		bool unmapBufferDeferred( GPUBuffer & pBuffer );
	};

	class CommandContextDeferredGraphics : public CommandContextDeferred
	{
	public:
		static ECommandContextType const sContextType = ECommandContextType::DirectTransfer;
		static Bitmask<ECommandListFlags> const sListFlagsDeferredGraphics;

		CommandContextDeferredGraphics( CommandSystem & pCommandSystem, CommandList & pCommandList )
		: CommandContextDeferred( pCommandSystem, pCommandList )
		{}

		virtual ~CommandContextDeferredGraphics() = default;

		bool beginRenderPass(
			const RenderPassConfigurationImmutableState & pRenderPassState,
			Bitmask<ECommandListActionFlags> pFlags );

		bool beginRenderPass(
			const RenderPassConfigurationDynamicState & pRenderPassState,
			Bitmask<ECommandListActionFlags> pFlags );

		void endRenderPass();

		bool setGraphicsPipelineStateObject( const GraphicsPipelineStateObject & pGraphicsPSO );
		bool setIAVertexStreamState( const IAVertexStreamImmutableState & pIAVertexStreamState );
		bool setIAVertexStreamState( const IAVertexStreamDynamicState & pIAVertexStreamState );
		bool setRenderTargetBindingState( const RenderTargetBindingImmutableState & pRenderTargetBindingState );
		bool setRenderTargetBindingState( const RenderTargetBindingDynamicState & pRenderTargetBindingState );

		bool cmdSetBlendConstantColor( const math::RGBAColorR32Norm & pColor );
		bool cmdSetViewport( const ViewportDesc & pViewportDesc );
		bool cmdSetShaderConstant( shader_input_ref_id_t pParamRefID, const void * pData );
		bool cmdSetShaderConstantBuffer( shader_input_ref_id_t pParamRefID, GPUBuffer & pConstantBuffer );
		bool cmdSetShaderTextureImage( shader_input_ref_id_t pParamRefID, Texture & pTexture );
		bool cmdSetShaderTextureSampler( shader_input_ref_id_t pParamRefID, Sampler & pSampler );

		void cmdDrawDirectIndexed( native_uint pIndicesNum, native_uint pIndicesOffset, EIndexDataFormat pIndexFormat );
		void cmdDrawDirectIndexedInstanced( native_uint pIndicesNumPerInstance, native_uint pInstancesNum, native_uint pIndicesOffset, EIndexDataFormat pIndexFormat );
		void cmdDrawDirectNonIndexed( native_uint pVerticesNum, native_uint pVerticesOffset );
		void cmdDrawDirectNonIndexedInstanced( native_uint pVerticesNumPerInstance, native_uint pInstancesNum, native_uint pVerticesOffset );
	};

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_GPU_CMD_CONTEXT_H__
