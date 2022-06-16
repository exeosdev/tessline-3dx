
#pragma once

#ifndef __TS3DRIVER_GPUAPI_GLCOMMON_PIPELINE_STATE_DESC_H__
#define __TS3DRIVER_GPUAPI_GLCOMMON_PIPELINE_STATE_DESC_H__

#include "../GL_prerequisites.h"
#include <ts3/gpuapi/state/pipelineStateDesc.h>
#include <ts3/gpuapi/state/vertexDataSourceDesc.h>
#include <ts3/gpuapi/state/graphicsPipelineStateDescriptorCache.h>

//#define TS3GX_GL_PLATFORM_TYPE  TS3GX_GL_PLATFORM_TYPE_ES

namespace ts3::gpuapi
{

	class GLBufferObject;
	class GLGraphicsPipelineStateObject;
	class GLVertexStreamStateObject;

	ts3GLDeclareOpenGLObjectHandle( GLShaderPipelineObject );
	ts3GLDeclareOpenGLObjectHandle( GLShaderProgramObject );
	ts3GLDeclareOpenGLObjectHandle( GLVertexArrayObject );

	struct GLBlendConfigDesc
	{
		struct Equation
		{
			GLenum color;
			GLenum alpha;
		};
		struct Factor
		{
			GLenum srcColor;
			GLenum dstColor;
			GLenum srcAlpha;
			GLenum dstAlpha;
		};
		EBlendState blendState;
		math::RGBAColorR32Norm constantFactor;
		Equation equation;
		Factor factor;
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
		EDepthTestState depthTestState;
		GLDepthSettings depthSettings;
		EStencilTestState stencilTestState;
		GLStencilSettings stencilSettings;
	};

	struct GLRasterizerConfigDesc
	{
		GLenum cullMode;
		GLenum triangleFrontFaceOrder;
		EScissorTestState scissorTestState;
	#if( TS3GX_GL_FEATURE_SUPPORT_PRIMITIVE_FILL_MODE )
		GLenum primitiveFillMode;
	#endif
	};

	struct GLVertexInputFormatDesc
	{
		struct GLVertexAttribute
		{
			GLuint attributeIndex = ts3::Limits<GLuint>::maxValue;
			GLuint streamIndex = ts3::Limits<GLuint>::maxValue;
			uint8 normalized = 0;
			uint8 componentsNum = 0;
			GLenum baseType = 0;
			uint32 relativeOffset = 0;
			uint32 instanceRate = 0;
			constexpr explicit operator bool () const
			{
				return streamIndex != CX_INVALID_VERTEX_STREAM_INDEX;
			}
		};
		using GLVertexAttributeArray = std::array<GLVertexAttribute, E_GPU_SYSTEM_METRIC_IA_MAX_VERTEX_ATTRIBUTES_NUM>;
		GLenum primitiveTopology;
		uint32 activeVertexAttributesNum;
		GLVertexAttributeArray vertexAttributeArray;
	};

	// Common data for all OpenGL state descriptors.
	// State descriptor contains a set of related pipeline state
	// configuration values, which can be treated as a single state.
	struct GLStateDescriptorCommonData
	{
		// Hash of the input Desc structure used to create this descriptor.
		// For instance, GLBlendStateDescriptor contains a hash of the
		// BlendConfigDesc structure passed as a definition of the blend.
		pipeline_input_desc_hash_t inputDescHash;
		// ID of the descriptor itself. Internally it is a 64-bit hash value.
		pipeline_state_descriptor_id_t descriptorID;
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
			GLVBBinding vbBindingArray[E_GPU_SYSTEM_METRIC_IA_MAX_VERTEX_INPUT_STREAMS_NUM];
		#else
			GLuint   vbHandleArray[E_GPU_SYSTEM_METRIC_IA_MAX_VERTEX_INPUT_STREAMS_NUM];
			GLintptr vbOffsetArray[E_GPU_SYSTEM_METRIC_IA_MAX_VERTEX_INPUT_STREAMS_NUM];
			GLsizei  vbStrideArray[E_GPU_SYSTEM_METRIC_IA_MAX_VERTEX_INPUT_STREAMS_NUM];
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

#endif // __TS3DRIVER_GPUAPI_GLCOMMON_PIPELINE_STATE_DESC_H__
