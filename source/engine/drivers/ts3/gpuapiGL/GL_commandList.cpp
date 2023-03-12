
#include "GL_commandList.h"
#include "GL_commandSystem.h"
#include "GL_gpuDevice.h"
#include "objects/GL_framebufferObject.h"
#include "objects/GL_shaderProgramObject.h"
#include "objects/GL_vertexArrayObject.h"
#include "state/GL_pipelineStateObject.h"
#include "state/GL_graphicsShaderState.h"
#include "state/GL_renderTarget.h"

#include <ts3/gpuapi/state/renderTargetDynamicStates.h>

namespace ts3::gpuapi
{

	GLCommandList::GLCommandList(
			GLCommandSystem & pGLCommandSystem,
			ECommandListType pListType,
			system::OpenGLRenderContextHandle pSysGLRenderContext )
	: CommandList( pGLCommandSystem, pListType, _stateController )
	, mSysGLRenderContext( pSysGLRenderContext )
	{}

	GLCommandList::~GLCommandList() = default;

	bool GLCommandList::beginRenderPass(
			const RenderPassConfigurationImmutableState & pRenderPassState,
			Bitmask<ECommandListActionFlags> pFlags )
	{
		bool beginRenderPassStatus = CommandList::beginRenderPass( pRenderPassState, pFlags );

		if( beginRenderPassStatus )
		{
			const auto * glcRenderPassState = pRenderPassState.queryInterface<GLRenderPassConfigurationImmutableState>();
			_currentRenderPassConfiguration = glcRenderPassState->mRenderPassConfiguration;

			executeRenderPassLoadActions();
		}

		return true;
	}

	bool GLCommandList::beginRenderPass(
			const RenderPassConfigurationDynamicState & pRenderPassState,
			Bitmask<ECommandListActionFlags> pFlags )
	{
		bool beginRenderPassStatus = CommandList::beginRenderPass( pRenderPassState, pFlags );

		if( beginRenderPassStatus )
		{
			_currentRenderPassConfiguration = pRenderPassState.getRenderPassConfiguration();

			executeRenderPassLoadActions();
		}

		return true;
	}

	void GLCommandList::endRenderPass()
	{
		if( !isRenderPassActive() )
		{
			return;
		}

		executeRenderPassStoreActions();

		CommandList::endRenderPass();
	}

	void GLCommandList::beginCommandSequence()
	{
		CommandList::beginCommandSequence();
	}

	void GLCommandList::endCommandSequence()
	{
		CommandList::endCommandSequence();
	}

	void GLCommandList::cmdDrawDirectIndexed( uint32 pIndicesNum, uint32 pIndicesOffset )
	{
		_stateController.applyStateChanges();

		const auto & drawTopologyProperties = _stateController.getGLDrawTopologyProperties();
		const auto * indexDataOffset = reinterpret_cast<void*>( pIndicesOffset * drawTopologyProperties.indexBufferElementByteSize );

		glDrawElements(
				drawTopologyProperties.primitiveTopology,
				static_cast<GLsizei>( pIndicesNum ),
				drawTopologyProperties.indexBufferDataType,
				indexDataOffset );
		ts3OpenGLHandleLastError();
	}

	void GLCommandList::cmdDrawDirectIndexedInstanced( uint32 pIndicesNumPerInstance, uint32 pInstancesNum, uint32 pIndicesOffset )
	{
	}

	void GLCommandList::cmdDrawDirectNonIndexed( uint32 pVerticesNum, uint32 pVerticesOffset )
	{
		_stateController.applyStateChanges();

		const auto & drawTopologyProperties = _stateController.getGLDrawTopologyProperties();

		glDrawArrays(
				drawTopologyProperties.primitiveTopology,
				static_cast<GLint>( pVerticesOffset ),
				static_cast<GLsizei>( pVerticesNum ) );
		ts3OpenGLHandleLastError();
	}

	void GLCommandList::cmdDrawDirectNonIndexedInstanced( uint32 pVerticesNumPerInstance, uint32 pInstancesNum, uint32 pVerticesOffset )
	{
	}

	void GLCommandList::cmdExecuteDeferredContext( CommandContextDeferred & pDeferredContext )
	{
		ts3DebugInterrupt();
	}

	void GLCommandList::executeRenderPassLoadActions()
	{
		if( _currentRenderPassConfiguration.attachmentActionClearMask != 0 )
		{
			smutil::clearRenderPassFramebuffer(
					_stateController.getCurrentRenderTargetBindingInfo(),
					_currentRenderPassConfiguration );
		}
	}

	void GLCommandList::executeRenderPassStoreActions()
	{
		if( _currentRenderPassConfiguration.attachmentActionResolveMask != 0 )
		{
			smutil::resolveRenderPassFramebuffer(
					_stateController.getCurrentRenderTargetBindingInfo(),
					_currentRenderPassConfiguration );
		}
	}

} // namespace ts3::gpuapi
