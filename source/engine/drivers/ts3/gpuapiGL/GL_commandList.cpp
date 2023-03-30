
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
			system::OpenGLRenderContextHandle pSysGLRenderContext,
			GLGraphicsPipelineStateController & pStateController )
	: CommandListRenderPassDefault( pGLCommandSystem, pListType, pStateController )
	, mSysGLRenderContext( pSysGLRenderContext )
	, _graphicsPipelineStateControllerGL( &pStateController )
	{}

	GLCommandList::~GLCommandList() = default;

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
		_graphicsPipelineStateControllerGL->applyStateChanges();

		const auto & drawTopologyProperties = _graphicsPipelineStateControllerGL->getGLDrawTopologyProperties();
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
		_graphicsPipelineStateControllerGL->applyStateChanges();

		const auto & drawTopologyProperties = _graphicsPipelineStateControllerGL->getGLDrawTopologyProperties();

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

	void GLCommandList::executeRenderPassLoadActions(
			const RenderPassConfiguration & pRenderPassConfiguration,
			const GraphicsPipelineDynamicState & pDynamicState )
	{
		if( pRenderPassConfiguration.attachmentsActionClearMask != 0 )
		{
			smutil::clearRenderPassFramebuffer( pRenderPassConfiguration, pDynamicState );
		}
	}

	void GLCommandList::executeRenderPassStoreActions(
			const RenderPassConfiguration & pRenderPassConfiguration,
			const GraphicsPipelineDynamicState & pDynamicState )
	{
		if( pRenderPassConfiguration.attachmentsActionResolveMask != 0 )
		{
			smutil::resolveRenderPassFramebuffer(
				_graphicsPipelineStateControllerGL->getCurrentRenderTargetBindingInfo(),
				pRenderPassConfiguration );
		}
	}

} // namespace ts3::gpuapi
