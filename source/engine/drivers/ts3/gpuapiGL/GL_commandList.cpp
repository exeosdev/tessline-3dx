
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

	void GLCommandList::cmdDrawDirectIndexed( native_uint pIndicesNum, native_uint pIndicesOffset )
	{
		_stateController.applyStateChanges();

		const auto & drawTopologyProperties = _stateController.getGLDrawTopologyProperties();
		const auto relativeIndexDataOffset = pIndicesOffset * drawTopologyProperties.indexBufferElementByteSize;
		const auto * baseIndexDataOffset = reinterpret_cast<void *>( drawTopologyProperties.indexBufferBaseOffset + relativeIndexDataOffset );

		glDrawElements(
			drawTopologyProperties.primitiveTopology,
			static_cast< GLsizei >( pIndicesNum ),
			drawTopologyProperties.indexBufferDataType,
			baseIndexDataOffset );
		ts3OpenGLHandleLastError();
	}

	void GLCommandList::cmdDrawDirectIndexedInstanced( native_uint pIndicesNumPerInstance, native_uint pInstancesNum, native_uint pIndicesOffset )
	{
	}

	void GLCommandList::cmdDrawDirectNonIndexed( native_uint pVerticesNum, native_uint pVerticesOffset )
	{
		_stateController.applyStateChanges();

		const auto & drawTopologyProperties = _stateController.getGLDrawTopologyProperties();

		glDrawArrays(
			drawTopologyProperties.primitiveTopology,
			static_cast< GLint >( pVerticesOffset ),
			static_cast< GLsizei >( pVerticesNum ) );
		ts3OpenGLHandleLastError();
	}

	void GLCommandList::cmdDrawDirectNonIndexedInstanced( native_uint pVerticesNumPerInstance, native_uint pInstancesNum, native_uint pVerticesOffset )
	{
	}

	void GLCommandList::cmdExecuteDeferredContext( CommandContextDeferred & pDeferredContext )
	{
		ts3DebugInterrupt();
	}

	void GLCommandList::executeRenderPassLoadActions()
	{
		if( _currentRenderPassConfiguration.attachmentsActionClearMask != 0 )
		{
			smutil::clearRenderPassFramebuffer( _currentRenderPassConfiguration );
		}
	}

	void GLCommandList::executeRenderPassStoreActions()
	{
		if( _currentRenderPassConfiguration.attachmentsActionResolveMask != 0 )
		{
			smutil::resolveRenderPassFramebuffer(
					_stateController.getCurrentRenderTargetBindingInfo(),
					_currentRenderPassConfiguration );
		}
	}

} // namespace ts3::gpuapi
