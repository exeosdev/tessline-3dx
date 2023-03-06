
#ifndef __TS3DRIVER_GPUAPI_GLCOMMON_COMMON_GPU_STATE_DEFS_H__
#define __TS3DRIVER_GPUAPI_GLCOMMON_COMMON_GPU_STATE_DEFS_H__

#include "../GL_prerequisites.h"
#include <ts3/gpuapi/state/commonGraphicsConfig.h>
#include <ts3/gpuapi/state/inputAssemblerCommon.h>

namespace ts3::gpuapi
{

	struct GLRTColorAttachmentBlendSettings
	{
		uint32 blendActive;
		GLenum colorEquation;
		GLenum alphaEquation;
		GLenum srcColorFactor;
		GLenum dstColorFactor;
		GLenum srcAlphaFactor;
		GLenum dstAlphaFactor;
		math::RGBAColorR32Norm constantFactor;
	};

	struct GLIAVertexAttributeInfo
	{
		uint16 streamIndex;
		uint8 normalized;
		uint8 componentsNum;
		GLenum baseType;
		uint32 byteSize;
		uint32 relativeOffset;
		uint32 instanceRate;
	};

	using GLIAVertexAttributeInfoArray = std::array<GLIAVertexAttributeInfo, cxdefs::IA_MAX_VERTEX_ATTRIBUTES_NUM>;

	struct GLBlendConfig
	{
		Bitmask<ERTAttachmentFlags> attachmentsMask = 0;
		GLRTColorAttachmentBlendSettings attachments[cxdefs::GPU_SYSTEM_METRIC_RT_MAX_COLOR_ATTACHMENTS_NUM];
	};

	struct GLDepthStencilConfig
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

		uint16 depthTestActive;
		uint16 stencilTestActive;
		GLDepthSettings depthSettings;
		GLStencilSettings stencilSettings;
	};

	struct GLIAInputLayoutDefinition
	{
		GLIAVertexAttributeInfoArray attributeArray;
		Bitmask<EIAVertexAttributeFlags> activeAttributesMask;
		GLenum primitiveTopology;
	};

	struct GLRasterizerConfig
	{
		GLenum cullMode;
		GLenum frontFaceVerticesOrder;
		uint32 scissorTestState;
	#if( TS3GX_GL_FEATURE_SUPPORT_PRIMITIVE_FILL_MODE )
		GLenum primitiveFillMode;
	#endif
	};

	namespace smutil
	{

		TS3_ATTR_NO_DISCARD GLBlendConfig translateBlendConfig( const BlendConfig & pConfig );

		TS3_ATTR_NO_DISCARD GLDepthStencilConfig translateDepthStencilConfig( const DepthStencilConfig & pConfig );

		TS3_ATTR_NO_DISCARD GLIAInputLayoutDefinition translateIAInputLayoutDefinition( const IAInputLayoutDefinition & pDefinition );

		TS3_ATTR_NO_DISCARD GLIAVertexAttributeInfo translateIAVertexAttributeInfo( const IAVertexAttributeInfo & pAttributeInfo );

		TS3_ATTR_NO_DISCARD GLRasterizerConfig translateRasterizerConfig( const RasterizerConfig & pConfig );

		TS3_ATTR_NO_DISCARD GLRTColorAttachmentBlendSettings translateRTColorAttachmentBlendSettings( const RTColorAttachmentBlendSettings & pSettings );

	}

}

#endif // __TS3DRIVER_GPUAPI_GLCOMMON_COMMON_GPU_STATE_DEFS_H__
