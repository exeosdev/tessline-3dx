
#pragma once

#ifndef __TS3DRIVER_GPUAPI_GLCOMMON_GRAPHICS_PIPELINE_STATE_H__
#define __TS3DRIVER_GPUAPI_GLCOMMON_GRAPHICS_PIPELINE_STATE_H__

#include "../GL_prerequisites.h"
#include <ts3/gpuapi/state/pipelineStateDesc.h>
#include <ts3/gpuapi/state/graphicsPipelineStateDescriptors.h>

//#define TS3GX_GL_PLATFORM_TYPE  TS3GX_GL_PLATFORM_TYPE_ES

namespace ts3::gpuapi
{

	class GLBufferObject;
	class GLGraphicsPipelineStateObject;
	class GLVertexStreamStateObject;

	ts3GLDeclareOpenGLObjectHandle( GLShaderPipelineObject );
	ts3GLDeclareOpenGLObjectHandle( GLShaderProgramObject );
	ts3GLDeclareOpenGLObjectHandle( GLVertexArrayObject );

	struct GLColorAttachmentBlendConfig
	{
		GLenum colorEquation;
		GLenum alphaEquation;
		GLenum srcColorFactor;
		GLenum dstColorFactor;
		GLenum srcAlphaFactor;
		GLenum dstAlphaFactor;
		math::RGBAColorR32Norm constantFactor;
	};

	struct GLBlendConfig
	{
		EActiveState blendActive;
		GLColorAttachmentBlendConfig colorAttachmentConfigs[E_GPU_SYSTEM_METRIC_RT_MAX_COLOR_ATTACHMENTS_NUM];
	};

	struct GLDepthStencilConfigDesc
	{
		struct GLDepthSettings
		{
			GLenum depthCompFunc;
			GLuint writeMask;
		};

		struct GLStencilFaceDesc
		{
			GLenum compFunc;
			GLenum opFail;
			GLenum opPassDepthFail;
			GLenum opPassDepthPass;
			GLint  refValue;
			GLuint readMask;
			GLuint writeMask;
		};

		struct GLStencilSettings
		{
			GLStencilFaceDesc frontFace;
			GLStencilFaceDesc backFace;
		};

		EActiveState depthTestState;
		GLDepthSettings depthSettings;
		EActiveState stencilTestState;
		GLStencilSettings stencilSettings;
	};

	struct GLRasterizerConfigDesc
	{
		GLenum cullMode;
		GLenum triangleFrontFaceOrder;
		EActiveState scissorTestState;
	#if( TS3GX_GL_FEATURE_SUPPORT_PRIMITIVE_FILL_MODE )
		GLenum primitiveFillMode;
	#endif
	};

	struct GLBlendStateDescriptor : public GLStateDescriptorCommonData
	{
		GLBlendConfigDesc configDesc;
	};

	struct GLDepthStencilStateDescriptor : public GLStateDescriptorCommonData
	{
		GLDepthStencilConfigDesc configDesc;
	};

	struct GLRasterizerStateDescriptor : public GLStateDescriptorCommonData
	{
		GLRasterizerConfigDesc configDesc;
	};

	struct GLVertexInputFormatStateDescriptor : public GLStateDescriptorCommonData
	{
		GLVertexInputFormatDesc inputFormatDesc;
		GLVertexArrayObjectHandle vertexArrayObject;
	};

	struct GLVertexStreamBindingDescriptor
	{
		struct GLIndexBufferBinding
		{
			GLuint handle;
			GLintptr offset;
			GLenum format;
			GLuint elementByteSize;
		};

		struct GLVertexBuffersBinding
		{
		#if( TS3GX_GL_PLATFORM_TYPE == TS3GX_GL_PLATFORM_TYPE_ES )
			struct GLVBBinding
			{
				GLuint   handle;
				GLintptr offset;
				GLsizei  stride;
			};
			GLVBBinding vbBindingArray[E_GPU_SYSTEM_METRIC_IA_MAX_VERTEX_STREAMS_NUM];
		#else
			GLuint   vbHandleArray[E_GPU_SYSTEM_METRIC_IA_MAX_VERTEX_STREAMS_NUM];
			GLintptr vbOffsetArray[E_GPU_SYSTEM_METRIC_IA_MAX_VERTEX_STREAMS_NUM];
			GLsizei  vbStrideArray[E_GPU_SYSTEM_METRIC_IA_MAX_VERTEX_STREAMS_NUM];
		#endif
		};

		GLIndexBufferBinding indexBufferBinding;
		GLVertexBuffersBinding vertexBuffersBinding;
		uint32 vertexBufferActiveBindingsNum;
	};

	using GLIndexBufferBinding = GLVertexStreamBindingDescriptor::GLIndexBufferBinding;
	using GLVertexBuffersBinding = GLVertexStreamBindingDescriptor::GLVertexBuffersBinding;

	GLVertexStreamBindingDescriptor createGLVertexStreamBindingDescriptor( const VertexDataSourceBinding & pCommonBinding );

	struct GLGraphicsPSDCacheTraits
	{
		using BlendDescriptorType = GLBlendStateDescriptor;
		using DepthStencilDescriptorType = GLDepthStencilStateDescriptor;
		using RasterizerDescriptorType = GLRasterizerStateDescriptor;
		using VertexInputFormatDescriptorType = GLVertexInputFormatStateDescriptor;

		static GLBlendStateDescriptor createBlendDescriptor( const BlendConfigDesc & pConfigDesc,
		                                                     pipeline_input_desc_hash_t pDescHash );

		static GLDepthStencilStateDescriptor createDepthStencilDescriptor( const DepthStencilConfigDesc & pConfigDesc,
		                                                                   pipeline_input_desc_hash_t pDescHash );

		static GLRasterizerStateDescriptor createRasterizerDescriptor( const RasterizerConfigDesc & pConfigDesc,
		                                                               pipeline_input_desc_hash_t pDescHash );

		static GLVertexInputFormatStateDescriptor createVertexInputFormatDescriptor( const VertexInputFormatDesc & pInputFormatDesc,
		                                                                             pipeline_input_desc_hash_t pDescHash );
	};

	using GLGraphicsPipelineStateDescriptorCache = GraphicsPipelineStateDescriptorCache<GLGraphicsPSDCacheTraits>;

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_GLCOMMON_GRAPHICS_PIPELINE_STATE_H__
