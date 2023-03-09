
#ifndef __TS3DRIVER_GPUAPI_GLCOMMON_COMMON_GRAPHICS_CONFIG_H__
#define __TS3DRIVER_GPUAPI_GLCOMMON_COMMON_GRAPHICS_CONFIG_H__

#include "../GL_prerequisites.h"
#include <ts3/gpuapi/state/commonGraphicsConfigImmutableStates.h>

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

		GLDepthSettings depthSettings;
		GLStencilSettings stencilSettings;
	};

	struct GLRasterizerConfig
	{
		Bitmask<ERasterizerConfigFlags> flags;
		GLenum cullMode;
		GLenum frontFaceVerticesOrder;
	#if( TS3GX_GL_FEATURE_SUPPORT_PRIMITIVE_FILL_MODE )
		GLenum primitiveFillMode;
	#endif
	};

	///
	class GLBlendImmutableState : public BlendImmutableState
	{
	public:
		GLBlendConfig const mGLBlendConfig;

	public:
		GLBlendImmutableState( GLGPUDevice & pGPUDevice, const GLBlendConfig & pGLBlendConfig, Bitmask<EBlendConfigFlags> pBlendFlags );
		virtual ~GLBlendImmutableState();

		static GpaHandle<GLBlendImmutableState> createInstance( GLGPUDevice & pGPUDevice, const BlendConfig & pBlendConfig );
	};

	///
	class GLDepthStencilImmutableState : public DepthStencilImmutableState
	{
	public:
		GLDepthStencilConfig const mGLDepthStencilConfig;

	public:
		GLDepthStencilImmutableState( GLGPUDevice & pGPUDevice, const GLDepthStencilConfig & pGLDepthStencilConfig, Bitmask<EDepthStencilConfigFlags> pDepthStencilFlags  );
		virtual ~GLDepthStencilImmutableState();

		static GpaHandle<GLDepthStencilImmutableState> createInstance( GLGPUDevice & pGPUDevice, const DepthStencilConfig & pDepthStencilConfig );
	};

	///
	class GLRasterizerImmutableState : public RasterizerImmutableState
	{
	public:
		GLRasterizerConfig const mGLRasterizerConfig;

	public:
		GLRasterizerImmutableState( GLGPUDevice & pGPUDevice, const GLRasterizerConfig & pGLRasterizerConfig, Bitmask<ERasterizerConfigFlags> pRasterizerFlags );
		virtual ~GLRasterizerImmutableState();

		static GpaHandle<GLRasterizerImmutableState> createInstance( GLGPUDevice & pGPUDevice, const RasterizerConfig & pRasterizerConfig );
	};

	namespace smutil
	{

		TS3_ATTR_NO_DISCARD GLBlendConfig translateBlendConfig( const BlendConfig & pConfig );

		TS3_ATTR_NO_DISCARD GLDepthStencilConfig translateDepthStencilConfig( const DepthStencilConfig & pConfig );

		TS3_ATTR_NO_DISCARD GLRasterizerConfig translateRasterizerConfig( const RasterizerConfig & pConfig );

		TS3_ATTR_NO_DISCARD GLRTColorAttachmentBlendSettings translateRTColorAttachmentBlendSettings(
				const RTColorAttachmentBlendSettings & pSettings );

	}

}

#endif // __TS3DRIVER_GPUAPI_GLCOMMON_COMMON_GRAPHICS_CONFIG_H__
